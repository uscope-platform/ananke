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


#include <charconv>
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Numeric_token)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, Numeric_token)

Numeric_token::Numeric_token(const Numeric_token &c) {
    value = c.value;
    binary_size = c.binary_size;
    sized_explicit = c.sized_explicit;
}

Numeric_token::Numeric_token(const std::string &s) {
    auto r = process_number(s);
    value = r.value;
    binary_size = r.binary_size;
    sized_explicit = r.sized_explicit;
}

Numeric_token::Numeric_token(std::variant<hdl_integer, double> n, int64_t b_s) {
    if(std::holds_alternative<double>(n)) {
        value = std::get<double>(n);
    } else {
        value = std::get<hdl_integer>(n);
    }
    binary_size = b_s;
    sized_explicit = b_s >= 0;
}

std::expected<resolved_parameter, solver_errors> Numeric_token::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) {
    return value;
}

std::string Numeric_token::print() const {

    if (value.is_real())
        return  std::to_string(value.get_real());
    return std::to_string(value.get_integer());
}

int64_t Numeric_token::get_size() {
    return binary_size;
}

bool operator==(const Numeric_token &lhs, const Numeric_token &rhs) {
    bool ret_val = true;
    ret_val &= lhs.value == rhs.value;
    ret_val &= lhs.binary_size == rhs.binary_size;
    return ret_val;
}

void Numeric_token::set_container_sizes(const resolved_type &s,
                                        const std::map<qualified_identifier, resolved_parameter> &context) {
    // If the token was unconstrained/unsized, snap its binary_size to the target signal width
    if (!sized_explicit && !s.packed_sizes.empty()) {
        binary_size = s.packed_sizes.front();
    }
}

std::optional<resolved_type> Numeric_token::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) const {
    resolved_type result;
    if (value.is_real()) {
        result.is_real = true;
        return result;
    }
    uint64_t width = 32;
    if (sized_explicit && binary_size > 0) {
        width = static_cast<uint64_t>(binary_size);
    } else if (!sized_explicit) {
        auto int_val = value.get_integer();
        auto minimal = int_val.get_size();
        if (minimal > 32) width = minimal;
    }
    result.packed_sizes.push_back(width);
    result.packed_ascending.push_back(false);
    result.packed_left.push_back(static_cast<int64_t>(width) - 1);
    result.packed_right.push_back(0);
    return result;
}

Numeric_token::numeric_parse_result Numeric_token::process_number(const std::string_view &s) {
    numeric_parse_result result;
    result.binary_size = 0;
    result.sized_explicit = false;

    bool signed_number = false;
    bool negative_number = false;
    auto body = strip_sign(s, signed_number, negative_number);

    auto qualifiers = parse_qualifiers(body, signed_number);
    result.sized_explicit = qualifiers.sized_explicit;

    // Direct interception for unbased unsized literals ('0, '1, 'x, 'z)
    if (!qualifiers.sized_explicit && !qualifiers.based && body.starts_with('\'')) {
        hdl_integer val;
        if (qualifiers.digits == "1") {
            wide_integer all_ones = -1;
            val.set_value(all_ones);
        } else {
            val.set_value(0);
        }
        val.set_signed(signed_number);
        result.value = val;
        result.binary_size = -1;
        return result;
    }

    auto digits = purge_underscores(qualifiers.digits);

    auto [value, binary_size] = parse_integer_digits(digits, qualifiers.base, signed_number, qualifiers.explicit_size);
    if (!qualifiers.based && qualifiers.explicit_size < 0 && binary_size < 32)
        binary_size = 32;
    if (negative_number) {
        auto int_val = value.get_integer();
        wide_integer negated = -int_val.to_wide();
        hdl_integer neg_val;
        neg_val.set_value(negated);
        neg_val.set_signed(int_val.get_signed());
        value = neg_val;
    }
    result.value = value;
    result.binary_size = binary_size;
    return result;
}

std::string_view Numeric_token::strip_sign(const std::string_view &s, bool &is_signed, bool &is_negative) {
    if (s.starts_with("+") || s.starts_with("-")) {
        is_signed = true;
        is_negative = s.starts_with("-");
        return s.substr(1);
    }
    return s;
}

Numeric_token::literal_qualifiers Numeric_token::parse_qualifiers(const std::string_view &body, bool &is_signed) {
    literal_qualifiers qualifiers;
    qualifiers.explicit_size = -1;
    qualifiers.sized_explicit = false;
    qualifiers.based = false;

    std::string_view raw_value;
    if (body.contains('\'')) {
        auto quote_pos = body.find_first_of('\'');
        raw_value = body.substr(quote_pos + 1);
        auto size_str = body.substr(0, quote_pos);

        if (!size_str.empty()) {
            auto [ptr, ec] = std::from_chars(size_str.data(), size_str.data() + size_str.size(), qualifiers.explicit_size, 10);
            if (ec == std::errc() && ptr != size_str.data()) qualifiers.sized_explicit = true;
        }
    } else {
        raw_value = body;
    }

    if (raw_value.starts_with('s')) {
        raw_value = raw_value.substr(1);
        is_signed = true;
    }

    qualifiers.based = raw_value.starts_with("d") || raw_value.starts_with("b")
                       || raw_value.starts_with("o") || raw_value.starts_with("h");

    // Strictly check for unbased unsized literals: no size prefix, starts with single quote, and is 0/1/x/z
    if (!qualifiers.sized_explicit && !qualifiers.based && body.starts_with('\'')) {
        if (raw_value == "1" || raw_value == "0" || raw_value == "x" || raw_value == "z" || raw_value == "X" || raw_value == "Z") {
            qualifiers.base = 2;
            qualifiers.digits = raw_value;
            return qualifiers;
        }
    }

    qualifiers.base = detect_base(raw_value);
    qualifiers.digits = raw_value;
    return qualifiers;
}

int Numeric_token::detect_base(std::string_view &digits) {
    int base = 10;
    if (digits.starts_with("d")) {
        base = 10;
        digits = digits.substr(1);
    }
    if (digits.starts_with("b")) {
        base = 2;
        digits = digits.substr(1);
    }
    if (digits.starts_with("o")) {
        base = 8;
        digits = digits.substr(1);
    }
    if (digits.starts_with("h")) {
        base = 16;
        digits = digits.substr(1);
    }
    return base;
}

std::string Numeric_token::purge_underscores(const std::string_view &digits) {
    std::string purged_value(digits);
    std::erase(purged_value, '_');
    return purged_value;
}

std::pair<resolved_parameter, int64_t> Numeric_token::parse_integer_digits(const std::string &digits, int base, bool is_signed, int64_t explicit_size) {
    // Standard integer parsing path (used by 'hfffffffa, 1, 2, etc.)
    if (is_signed) {
        int64_t value;
        auto [ptr, ec] = std::from_chars(digits.data(), digits.data() + digits.size(), value, base);
        if (ec == std::errc::result_out_of_range)
            return process_wide_integer(digits, base, is_signed, explicit_size);
        if (ec != std::errc() || ptr != digits.data() + digits.size())
            return {0, 0};
        hdl_integer ret(value);
        return finalize_integer(ret, explicit_size, is_signed);
    } else {
        uint64_t value;
        auto [ptr, ec] = std::from_chars(digits.data(), digits.data() + digits.size(), value, base);
        if (ec == std::errc::result_out_of_range)
            return process_wide_integer(digits, base, is_signed, explicit_size);
        if (ec != std::errc() || ptr != digits.data() + digits.size())
            return {0, 0};
        hdl_integer ret;
        ret.set_value(value);
        return finalize_integer(ret, explicit_size, is_signed);
    }
}

std::pair<resolved_parameter, int64_t> Numeric_token::finalize_integer(hdl_integer int_val, int64_t explicit_size,
                                                                       bool is_signed) {
    int_val.set_signed(is_signed);
    if (explicit_size < 0) explicit_size = int_val.get_size();
    return {int_val, explicit_size};
}

std::pair<resolved_parameter, int64_t> Numeric_token::process_wide_integer(const std::string_view &raw_string, uint8_t base,
                                                                           bool signed_number, int64_t explicit_size) {
    hdl_integer res;

    wide_integer wide_num;
    std::stringstream ss;
    std::string test =  std::string(raw_string);
    ss << std::setbase(base) << test;
    ss >> wide_num;

    res.set_value(wide_num);
    res.set_signed(signed_number);
    if (explicit_size < 0) explicit_size = res.get_size();
    return {res, explicit_size};
}

bool Numeric_token::isEqual(const Expression_base &other) const {
    const auto& rhs = static_cast<const Numeric_token&>(other);

    return std::tie( value, binary_size) == std::tie(rhs.value, rhs.binary_size);
}
