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

#include <bitset>

#include "data_model/HDL/parameters/components/Token.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

using namespace std::string_literals;

CEREAL_REGISTER_TYPE(Token)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Parameter_value_base, Token)

Token::Token(const Token &c) {
    value = c.value;
    array_index = c.array_index;
    package_prefix = c.package_prefix;
    instance_prefix = c.instance_prefix;
    binary_size = c.binary_size;
    type = c.type;
}


Token::Token() {
    value = 0;
}

Token::Token(const std::string &s, const token_type &t) {
    value = s;
    type = t;
    if(t == number) {
        auto ret_val = process_number(s);
        value = ret_val.first;
        binary_size = ret_val.second;
    }
}

Token::Token(std::variant<hdl_integer, double> n, int64_t b_s) {
    if(std::holds_alternative<double>(n)) {
        value = std::get<double>(n);
    } else {
        value = std::get<hdl_integer>(n);
    }
    binary_size = b_s;
    type = number;
}


std::set<qualified_identifier> Token::get_dependencies() const {
    std::set<qualified_identifier> result;
    if (is_identifier()){
        result.insert({package_prefix, instance_prefix, value.get_string()});
    }
    for (const auto &idx : array_index) {
        auto idx_deps = idx->get_dependencies();
        result.insert(idx_deps.begin(), idx_deps.end());
    }
    return result;
}

void Token::propagate_function(const HDL_function_def &def) {
    for (auto &component : array_index) {
        component->propagate_function(def);
    }
}

std::optional<resolved_parameter> Token::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    if (type == identifier) {
        qualified_identifier id{package_prefix, instance_prefix, value.get_string()};
        auto it = context.find(id);
        if (it != context.end()) {
            const auto &resolved = it->second;
            if (array_index.empty()) return resolved;

            std::vector<int64_t> indices;
            for (const auto &idx_expr : array_index) {
                auto idx_val = idx_expr->evaluate(context);
                if (!idx_val.has_value() || !idx_val.value().is_integer()) return std::nullopt;
                indices.push_back(idx_val.value().get_integer().get_value());
            }

            if (resolved.is_int_array()) {
                auto values = resolved.get_int_array();
                auto array_val = values.get_value(indices);
                if (array_val.has_value()) return array_val.value();
                return static_cast<hdl_integer>(0);
            } else if (resolved.is_string_array()) {
                auto values = resolved.get_string_array();
                auto array_val = values.get_value(indices);
                if (array_val.has_value()) return resolved_parameter(array_val.value());
                return resolved_parameter("");
            } else if (resolved.is_integer() && !indices.empty()) {
                std::bitset<64> bits(resolved.get_integer().get_value());
                return static_cast<hdl_integer>(bits[indices[0]]);
            }
            return std::nullopt;
        }
        return std::nullopt;
    }
    return value;
}


std::string Token::print() const {
    std::string ret_val;
    if(is_numeric()){
        if (value.is_real()) ret_val = std::to_string(value.get_real());
        else ret_val = std::to_string(value.get_integer());
    } else {
        if(!array_index.empty()){
            ret_val = value.get_string() + print_index(array_index);
        } else {
            ret_val = value.get_string();
        }
    }
    return ret_val;
}

int64_t Token::get_size() {
    if (binary_size != 0) return binary_size;
    return container_size;
}

void Token::set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context) {
    container_size = 1;
    for (auto &ps : s.packed_sizes) container_size *= ps;
    for (auto &us : s.unpacked_sizes) container_size *= us;
}


Token::token_type Token::get_type(const std::string &s) {
    if(ctre::match<R"(^\d+$)">(s) || ctre::search<R"(^\d*'(s)?(h|d|o|b)([0-9a-fA-F]+))">(s)|| ctre::match<R"(^[+\-]?(\d+\.\d*|\.\d+)([eE][+\-]?\d+)?$|^[+\-]?\d+[eE][+\-]?\d+$)">(s)) return number;
    if(s.starts_with("\"") | ctre::match<R"(\d+(\.\d+)?(s|ms|us|ns|ps|fs))">(s)) return string;
    return identifier;
}

std::pair<resolved_parameter, int64_t> Token::process_number(const std::string_view &s) {
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

std::pair<resolved_parameter, int64_t> Token::process_wide_integer(const std::string_view &raw_string, uint8_t base, bool signed_number) {
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

std::string Token::print_index(const std::vector<std::shared_ptr<Parameter_value_base>> &index) const {
    std::string ret_val;
    for(const auto &item:index){
        ret_val += "[" + item->print() + "]";
    }
    return ret_val;
}

bool Token::isEqual(const Parameter_value_base& other) const {
    const auto& rhs = static_cast<const Token&>(other);
    bool res = std::tie(type, value, package_prefix, instance_prefix, binary_size) == std::tie(rhs.type, rhs.value, rhs.package_prefix, rhs.instance_prefix, rhs.binary_size);
    if (array_index.size() != rhs.array_index.size()) return false;
    for (size_t i = 0; i < array_index.size(); i++) {
        res &= *array_index[i] == *rhs.array_index[i];
    }
    return res;
}

bool operator==(const Token &lhs, const Token &rhs) {
    bool ret_val = true;
    if (lhs.type != rhs.type) return false;
    ret_val &= lhs.value == rhs.value;
    ret_val &= lhs.array_index == rhs.array_index;
    ret_val &= lhs.package_prefix == rhs.package_prefix;
    ret_val &= lhs.instance_prefix == rhs.instance_prefix;
    ret_val &= lhs.binary_size == rhs.binary_size;
    return ret_val;
}

bool Token::try_replace_identifier(std::shared_ptr<Parameter_value_base> &slot,
                                    const qualified_identifier &constant_id,
                                    const std::shared_ptr<Parameter_value_base> &value) {
    if (slot->is<Token>() && slot->as<Token>().is_identifier()) {
        auto tok_val = slot->as<Token>().get_value();
        if (tok_val.has_value() && tok_val.value().get_string() == constant_id.name) {
            slot = value;
            return true;
        }
    }
    return false;
}
