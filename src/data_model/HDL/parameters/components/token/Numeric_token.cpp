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


#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"


Numeric_token::Numeric_token(const Numeric_token &c) {
    value = c.value;
    container_size = c.container_size;
    binary_size = c.binary_size;
}

Numeric_token::Numeric_token(const std::string &s) {
    auto [v, b] = process_number(s);
    value = v;
    binary_size = b;
}

Numeric_token::Numeric_token(std::variant<hdl_integer, double> n, int64_t b_s) {
    if(std::holds_alternative<double>(n)) {
        value = std::get<double>(n);
    } else {
        value = std::get<hdl_integer>(n);
    }
    binary_size = b_s;
}

std::optional<resolved_parameter> Numeric_token::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    return value;
}

std::string Numeric_token::print() const {

    if (value.is_real())
        return  std::to_string(value.get_real());
    return std::to_string(value.get_integer());
}

int64_t Numeric_token::get_size() {
    if (binary_size != 0) return binary_size;
    return container_size;
}

bool operator==(const Numeric_token &lhs, const Numeric_token &rhs) {
    bool ret_val = true;
    ret_val &= lhs.value == rhs.value;
    ret_val &= lhs.binary_size == rhs.binary_size;
    return ret_val;
}

void Numeric_token::set_container_sizes(const resolved_type &s,
    const std::map<qualified_identifier, resolved_parameter> &context) {
    container_size = 1;
    for (auto &ps : s.packed_sizes) container_size *= ps;
    for (auto &us : s.unpacked_sizes) container_size *= us;
}

std::pair<resolved_parameter, int64_t> Numeric_token::process_number(const std::string_view &s) {
    std::string_view raw_number = s;
    int explicit_size = -1;
    bool signed_number = false;
    bool negative_number =  s.starts_with("-");

    if (s.contains('.')) {
        double value;
        std::from_chars(s.data(), s.data() + s.size(), value);
        return {value, 64};
    }

    if( s.starts_with("+") || negative_number) {
        raw_number = raw_number.substr(1);
        signed_number = true;
    }

    std::string_view raw_value;
    if (raw_number.contains('\'')) {
        raw_value = raw_number.substr(raw_number.find_first_of('\'')+1);
        auto size_str = raw_number.substr(0, raw_number.find_first_of('\''));
        std::from_chars(size_str.data(), size_str.data() + size_str.size(), explicit_size, 10);
    } else {
        raw_value = raw_number;
    }

    if (raw_value.starts_with('s')) {
        raw_value = raw_value.substr(1);
        signed_number = true;
    }

    int base = 10;
    if (raw_value.starts_with("d")) {
        base = 10;
        raw_value = raw_value.substr(1);
    }
    if (raw_value.starts_with("b")) {
        base = 2;
        raw_value = raw_value.substr(1);
    }
    if (raw_value.starts_with("o")) {
        base = 8;
        raw_value = raw_value.substr(1);
    }
    if (raw_value.starts_with("h")) {
        base = 16;
        raw_value = raw_value.substr(1);
    }
    std::string purged_value(raw_value);
    std::erase(purged_value, '_');

    if (signed_number) {
        int64_t value;
        auto [ptr, ec] = std::from_chars(purged_value.data(), purged_value.data() + purged_value.size(), value, base);
        if (ec == std::errc::result_out_of_range) {
            return process_wide_integer(purged_value, base, signed_number);
        } else {
            hdl_integer ret(value);
            ret.set_signed(signed_number);
            if (explicit_size<0) explicit_size = ret.get_size();
            return{ret, explicit_size};
        }
    } else {
        uint64_t value;
        auto [ptr, ec] = std::from_chars(purged_value.data(), purged_value.data() + purged_value.size(), value, base);
        if (ec == std::errc::result_out_of_range) {
            return process_wide_integer(purged_value, base, signed_number);
        } else {
            hdl_integer ret;
            ret.set_value(value);
            ret.set_signed(signed_number);
            if (explicit_size<0) explicit_size = ret.get_size();
            return{ret, explicit_size};
        }
    }
}

std::pair<resolved_parameter, int64_t> Numeric_token::process_wide_integer(const std::string_view &raw_string, uint8_t base,
    bool signed_number) {
    hdl_integer res;

    std::string prefixed_string;
    switch (base) {
        case 16: prefixed_string = "0x" + std::string(raw_string); break;
        case 10: prefixed_string = std::string(raw_string); break;
        case 8: prefixed_string = '0' + std::string(raw_string); break;
        case 2: prefixed_string = "0b" + std::string(raw_string); break;

    }
    int1024_t wide_num(prefixed_string.c_str());

    res.set_value(wide_num);
    return {res, 0};
}

bool Numeric_token::isEqual(const Expression_base &other) const {
    const auto& rhs = static_cast<const Numeric_token&>(other);

    return std::tie( value, binary_size) == std::tie(rhs.value, rhs.binary_size);
}
