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
#include <nlohmann/json.hpp>

#include "third_party/uintwide_t.h"

using int1024_t = ::math::wide_integer::int1024_t;

namespace math {
    namespace wide_integer {
        inline void to_json(nlohmann::json& j, const int1024_t& val) {
            std::stringstream ss;
            // Check sign manually because stream formatting can vary by library version
            if (val < 0) {
                // Negate to print absolute value safely
                int1024_t abs_val = -val;
                ss << "-0x" << std::hex << abs_val;
            } else {
                ss << "0x" << std::hex << val;
            }
            j = ss.str();
        }

        inline void from_json(const nlohmann::json& j, int1024_t& val) {
            std::string s = j.get<std::string>();
            bool is_negative = false;

            if (s.rfind("-", 0) == 0) {
                is_negative = true;
                s = s.substr(1);
            }
            if (s.rfind("0x", 0) == 0 || s.rfind("0X", 0) == 0) {
                s = s.substr(2);
            }

            val = int1024_t(("0x" + s).c_str());
            if (is_negative) {
                val = -val;
            }
        }

        template <class Archive>
            void save(Archive& ar, const int1024_t& val) {
            std::stringstream ss;
            ss << std::hex << val;
            std::string s = ss.str();
            ar(s); // Save it safely as a string
        }

        template <class Archive>
        void load(Archive& ar, int1024_t& val) {
            std::string s;
            ar(s); // Load the string representation
            val = int1024_t(("0x" + s).c_str());
        }
    }
}

class hdl_integer {

public:
    hdl_integer() = default;

    hdl_integer(const hdl_integer &other)
        : value(other.value),
          wide_value(other.wide_value),
          size(other.size),
          signedness(other.signedness) {
    }

    hdl_integer(hdl_integer &&other) noexcept
        : value(other.value),
          wide_value(std::move(other.wide_value)),
          size(other.size),
          signedness(other.signedness) {
    }

    hdl_integer & operator=(const hdl_integer &other) {
        if (this == &other)
            return *this;
        value = other.value;
        wide_value = other.wide_value;
        size = other.size;
        signedness = other.signedness;
        return *this;
    }

    hdl_integer & operator=(hdl_integer &&other) noexcept {
        if (this == &other)
            return *this;
        value = other.value;
        wide_value = std::move(other.wide_value);
        size = other.size;
        signedness = other.signedness;
        return *this;
    }

    hdl_integer(int64_t val) {
        value = val;
    }
    void set_size(const int64_t s) {size = s;}
    void set_value(const uint64_t v) {value = v;}
    void set_value(const int1024_t v) {wide_value = v;}
    void set_signed(const bool s) {signedness = s;}
    [[nodiscard]] int64_t get_value() const {return  value;}

    uint64_t get_size();
    bool get_signed(){return signedness;}

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

    hdl_integer& operator+=(int64_t rhs) {
        value += rhs;
        return *this;
    }

    hdl_integer& operator|=(hdl_integer rhs) {
        value |= rhs.value;
        return *this;
    }

    std::string to_string() const {
        return std::to_string(value);
    }

    friend bool operator==(const hdl_integer &lhs, const hdl_integer &rhs) {
        bool ret = true;
        ret &= lhs.value == rhs.value;
        ret &= lhs.size == rhs.size;
        ret &= lhs.signedness == rhs.signedness;
        ret &= lhs.wide_value == rhs.wide_value;
        return ret;
    }


    friend bool operator<(const hdl_integer &lhs, const hdl_integer &rhs) {
        return lhs.value < rhs.value;
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
        ar(value, size, signedness, wide_value);
    }
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(hdl_integer, value, size, signedness, wide_value);
private:

    int64_t value = 0;
    int1024_t wide_value = 0;
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
        int64_t raw_val = s.get_value();
        if (raw_val < 0) {
            hdl_integer result = s;
            result.set_value(static_cast<uint64_t>(-raw_val));
            return result;
        }
        return s;
    }
    inline hdl_integer pow(const hdl_integer &a, const hdl_integer &b) {
        return static_cast<int64_t>(std::pow(a.get_value(), b.get_value()));
    }
}


#endif //ANANKE_HDL_INTEGER_HPP
