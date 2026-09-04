//  Copyright 2026 Filippo Savi
//  Author: Filippo Savi <filssavi@gmail.com>
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.


#ifndef ANANKE_HDL_INTEGER_HPP
#define ANANKE_HDL_INTEGER_HPP

#include <cstdint>
#include <cmath>
#include <string>
#include <variant>
#include <bitset>
#include <nlohmann/json.hpp>
#include <cereal/types/variant.hpp>
#include <cereal/types/string.hpp>

#include "third_party/boost/multiprecision/cpp_int.hpp"


using wide_integer = boost::multiprecision::number<
    boost::multiprecision::cpp_int_backend<>,
    boost::multiprecision::et_off
>;

namespace cereal {

    // Save boost::multiprecision::number as a string representation
    template <class Archive, class Backend, boost::multiprecision::expression_template_option ExpressionTemplates>
    std::string save_minimal(Archive const &, boost::multiprecision::number<Backend, ExpressionTemplates> const & num) {
        return num.str();
    }

    // Load boost::multiprecision::number from a string representation
    template <class Archive, class Backend, boost::multiprecision::expression_template_option ExpressionTemplates>
    void load_minimal(Archive const &, boost::multiprecision::number<Backend, ExpressionTemplates> & num, std::string const & str) {
        num = boost::multiprecision::number<Backend, ExpressionTemplates>(str);
    }

} // namespace cereal

class hdl_integer {

public:
    hdl_integer() = default;

    hdl_integer(const hdl_integer &other) = default;
    hdl_integer(hdl_integer &&other) noexcept = default;
    hdl_integer & operator=(const hdl_integer &other) = default;
    hdl_integer & operator=(hdl_integer &&other) noexcept = default;

    hdl_integer(int64_t val) {
        content = val;
    }

    [[nodiscard]] wide_integer to_wide() const {
        if (std::holds_alternative<wide_integer>(content))
            return std::get<wide_integer>(content);
        // Zero-extend the narrow value: treat its int64 bit pattern as unsigned,
        // so a negative narrow value does not sign-extend into the high bits.
        return wide_integer(static_cast<uint64_t>(std::get<int64_t>(content)));
    }
    [[nodiscard]] bool is_wide() const { return std::holds_alternative<wide_integer>(content); }

    void set_size(const int64_t s);
    void set_value(const uint64_t v);
    void set_value(const wide_integer v);
    void set_signed(const bool s) {signedness = s;}

    // Produce an all-ones mask of `width` bits
    static hdl_integer width_mask(int64_t width) {
        if (width <= 0) return hdl_integer(0);
        if (width >= 1024) {
            hdl_integer m;
            wide_integer all_ones = (wide_integer(1) << (1024 - 1)) | ((wide_integer(1) << (1024 - 1)) - 1);
            m.set_value(all_ones);
            return m;
        }
        return (hdl_integer(1) << hdl_integer(width)) - 1;
    }

    // Truncate the value to `width` bits, preserving signedness. Width <= 0
    // yields 0; width >= 1024 leaves the value unchanged.
    [[nodiscard]] hdl_integer truncate_to(int64_t width) const {
        hdl_integer result;
        wide_integer masked = to_wide() & width_mask(width).to_wide();
        result.set_value(masked);
        result.set_signed(signedness);
        return result;
    }

    // Truncate to `width` bits and sign-extend from bit (width - 1).
    [[nodiscard]] hdl_integer sign_extend(int64_t width) const {
        if (width <= 0) return hdl_integer(0);
        if (width >= 1024) return *this;
        wide_integer mask = width_mask(width).to_wide();
        wide_integer wide = to_wide() & mask;
        if ((wide & (wide_integer(1) << (width - 1))) != 0)
            wide |= ~mask;
        hdl_integer result;
        result.set_value(wide);
        result.set_signed(signedness);
        return result;
    }

    [[nodiscard]] int64_t get_value() const {
        if (std::holds_alternative<wide_integer>(content))
            return static_cast<int64_t>(std::get<wide_integer>(content));
        return std::get<int64_t>(content);
    }

    // The value as a double, preserving the sign of narrow negatives (unlike
    // to_wide(), which zero-extends them) and the full width of wide values.
    [[nodiscard]] double to_double() const {
        if (std::holds_alternative<wide_integer>(content))
            return static_cast<double>(std::get<wide_integer>(content));
        return static_cast<double>(std::get<int64_t>(content));
    }

    // The value's full bit pattern as a bitset (width encapsulated here).
    [[nodiscard]] std::bitset<1024> to_bitset() const {
        std::bitset<1024> result;
        wide_integer w = to_wide();
        for (size_t i = 0; i < 1024; i++)
            if (((w >> i) & 1) != 0) result.set(i);
        return result;
    }

    uint64_t get_size() const;
    bool get_signed() const {return signedness;}

    hdl_integer operator+(const hdl_integer &o) const;
    hdl_integer operator-(const hdl_integer &o) const;
    hdl_integer operator*(const hdl_integer &o) const;
    hdl_integer operator/(const hdl_integer &o) const;
    hdl_integer operator%(const hdl_integer &o) const;

    hdl_integer operator&&(const hdl_integer &o) const;
    hdl_integer operator||(const hdl_integer &o) const;
    hdl_integer operator&(const hdl_integer &o) const;
    hdl_integer operator|(const hdl_integer &o) const;
    hdl_integer operator^(const hdl_integer &o) const;
    hdl_integer operator~() const;
    hdl_integer operator!() const;

    hdl_integer operator<<(const hdl_integer &o) const;
    hdl_integer operator>>(const hdl_integer &o) const;

    hdl_integer& operator+=(const hdl_integer &rhs) {
        if (!is_wide() && !rhs.is_wide()) {
            content = get_value() + rhs.get_value();
        } else {
            content = to_wide() + rhs.to_wide();
        }
        return *this;
    }

    hdl_integer& operator|=(const hdl_integer &rhs) {
        if (!is_wide() && !rhs.is_wide()) {
            content = get_value() | rhs.get_value();
        } else {
            content = to_wide() | rhs.to_wide();
        }
        return *this;
    }

    std::string to_string() const {
        if (std::holds_alternative<wide_integer>(content)) {
            std::stringstream ss;
            ss << std::get<wide_integer>(content);
            return ss.str();
        }
        return std::to_string(std::get<int64_t>(content));
    }

    // The value interpreted as a signed number (sign-extends narrow values).
    static wide_integer signed_value(const hdl_integer &v) {
        if (std::holds_alternative<wide_integer>(v.content)) return std::get<wide_integer>(v.content);
        return wide_integer(std::get<int64_t>(v.content));
    }

    // Extend to a common comparison width: sign-extend when signed, zero-extend otherwise.
    static wide_integer extend_to_width(const hdl_integer &v, int64_t width) {
        wide_integer mask = (width >= 1024) ? wide_integer(-1) : (wide_integer(1) << width) - 1;
        wide_integer m = v.to_wide() & mask;
        if (v.get_signed() && width > 0 && width < 1024) {
            wide_integer sign_bit = wide_integer(1) << (width - 1);
            if ((m & sign_bit) != 0) m |= ~mask;
        }
        return m;
    }

    // Interpret a possibly sign-extended pattern as an unsigned width-bit value.
    static wide_integer as_unsigned_bits(const wide_integer &v, int64_t width) {
        if (width >= 1024 || v >= 0) return v;
        return v + (wide_integer(1) << width);
    }

    // Minimum bit width that can hold a signed value (magnitude bits + sign bit).
    static int64_t signed_fit_width(const hdl_integer &v) {
        if (!v.get_signed()) return 0;
        wide_integer mag = signed_value(v);
        if (mag < 0) mag = -mag;
        int64_t bits = 0;
        while (mag != 0) { mag >>= 1; bits++; }
        return bits + 1;
    }

    friend bool operator==(const hdl_integer &lhs, const hdl_integer &rhs) {
        int64_t width = std::max(std::max(static_cast<int64_t>(lhs.get_size()), static_cast<int64_t>(rhs.get_size())),
                                 std::max(signed_fit_width(lhs), signed_fit_width(rhs)));
        auto a = extend_to_width(lhs, width);
        auto b = extend_to_width(rhs, width);
        if (lhs.get_signed() && rhs.get_signed()) return a == b;
        return as_unsigned_bits(a, width) == as_unsigned_bits(b, width);
    }

    friend bool operator<(const hdl_integer &lhs, const hdl_integer &rhs) {
        int64_t width = std::max(std::max(static_cast<int64_t>(lhs.get_size()), static_cast<int64_t>(rhs.get_size())),
                                 std::max(signed_fit_width(lhs), signed_fit_width(rhs)));
        auto a = extend_to_width(lhs, width);
        auto b = extend_to_width(rhs, width);
        if (lhs.get_signed() && rhs.get_signed()) return a < b;
        return as_unsigned_bits(a, width) < as_unsigned_bits(b, width);
    }

    friend bool operator<=(const hdl_integer &lhs, const hdl_integer &rhs) {
        return !(rhs < lhs);
    }

    friend bool operator>(const hdl_integer &lhs, const hdl_integer &rhs) {
        return rhs < lhs;
    }

    friend bool operator>=(const hdl_integer &lhs, const hdl_integer &rhs) {
        return !(lhs < rhs);
    }

    template<class Archive>
    void serialize(Archive &ar) {
        ar(content, size, signedness);
    }

    friend void to_json(nlohmann::json& j, const hdl_integer& val) {
        j["value"] = val.get_value();
        j["size"] = val.size;
        j["signedness"] = val.signedness;
        auto str = "0x" + val.to_wide().str(0, std::ios_base::hex);
        j["wide_value"] = str;
        j["wide"] = val.is_wide();
    }

    friend void from_json(const nlohmann::json& j, hdl_integer& val) {
        if (j.at("wide").get<bool>()) {
            val.content = boost::multiprecision::cpp_int(j.at("wide_value").get<std::string>());
        } else {
            val.content = j.at("value").get<int64_t>();
        }
        val.size = j.at("size").get<uint64_t>();
        val.signedness = j.at("signedness").get<bool>();
    }

private:

    std::variant<int64_t, wide_integer> content;
    uint64_t size = 0;
    bool signedness = false;
};
namespace std {

    // Support for std::to_string(hdl_integer)
    inline std::string to_string(const hdl_integer& s) {
        return s.to_string();
    }

    // Support for std::abs(hdl_integer)
    inline hdl_integer abs(const hdl_integer& s) {
        auto w = s.to_wide();
        if (w < 0) {
            hdl_integer result;
            result.set_value(-w);
            return result;
        }
        return s;
    }
    inline hdl_integer pow(const hdl_integer &a, const hdl_integer &b) {
        double base = static_cast<double>(a.to_wide());
        double exp = static_cast<double>(b.to_wide());
        return static_cast<int64_t>(std::pow(base, exp));
    }
}


#endif //ANANKE_HDL_INTEGER_HPP
