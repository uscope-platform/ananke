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
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"

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
    if (c.call) call = std::make_shared<HDL_function_call>(*c.call);
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

Token::Token(const std::shared_ptr<Parameter_value_base> &param) {
    if (!param->is<HDL_function_call>()) {
        throw std::invalid_argument("Only functions are supported as expression components");
    }
    call = std::make_shared<HDL_function_call>(param->as<HDL_function_call>());
    type = function;
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
    if (type == function && call) {
        auto call_deps = call->get_dependencies();
        result.insert(call_deps.begin(), call_deps.end());
    }
    return result;
}

void Token::propagate_function(const HDL_function_def &def) {
    for (auto &component : array_index) {
        component->propagate_function(def);
    }
    if (call) call->propagate_function(def);
}

std::optional<resolved_parameter> Token::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    if (type == function && call) {
        return call->evaluate(context);
    }
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

std::pair<resolved_parameter, int64_t> Token::process_number(const std::string &s) {
    int64_t ret_size;
    resolved_parameter ret_value;

    if(ctre::match<R"(^[+\-]?(\d+\.\d*|\.\d+)([eE][+\-]?\d+)?$|^[+\-]?\d+[eE][+\-]?\d+$)">(s)) {
        ret_size = 64;
        ret_value = std::stod(s);
    } else if(ctre::match<R"(^\d+$)">(s)) {
        ret_value = static_cast<hdl_integer>(std::stoul(s));
        ret_size = ret_value.get_integer().get_size();
    } else if(ctre::search<R"(^\d*'(s)?(h|d|o|b)([0-9a-fA-F]+))">(s)){
        auto sv_match = ctre::search<R"(^\d*'(s)?(h|d|o|b)([0-9a-fA-F]+))">(s);
        std::string base;
        std::string str_value;
        if (sv_match.get<1>()) {
            base = sv_match.get<2>().str();
            str_value = sv_match.get<3>().str();
        } else {
            base = sv_match.get<2>().str();
            str_value = sv_match.get<3>().str();
        }
        if(base =="h"){
            ret_value = static_cast<hdl_integer>(std::stoul(str_value, nullptr, 16));
        } else if(base =="d") {
            ret_value = static_cast<hdl_integer>(std::stoul(str_value, nullptr, 10));
        } else if(base =="o") {
            ret_value = static_cast<hdl_integer>(std::stoul(str_value, nullptr, 8));
        } else if(base =="b") {
            ret_value = static_cast<hdl_integer>(std::stoul(str_value, nullptr, 2));
        }

        auto size_match = ctre::search<R"(^(\d*)'[0-9a-zA-Z]+)">(s);
        if(size_match){
            if(!size_match.get<1>().str().empty()) {
                ret_size = std::stoll(size_match.get<1>().str());
            } else {
                ret_size = ret_value.get_integer().get_size();
            }
        }
    }
    return std::make_pair(ret_value, ret_size);
}


Token::token_type Token::get_type(const std::string &s) {
    if(ctre::match<R"(^\d+$)">(s) || ctre::search<R"(^\d*'(s)?(h|d|o|b)([0-9a-fA-F]+))">(s)|| ctre::match<R"(^[+\-]?(\d+\.\d*|\.\d+)([eE][+\-]?\d+)?$|^[+\-]?\d+[eE][+\-]?\d+$)">(s)) return number;
    if(s.starts_with("\"") | ctre::match<R"(\d+(\.\d+)?(s|ms|us|ns|ps|fs))">(s)) return string;
    return identifier;
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
    if (call == nullptr ^ rhs.call == nullptr) return false;
    if (!(call == nullptr && rhs.call == nullptr)) res &= *call == *rhs.call;
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
    if (lhs.call == nullptr ^ rhs.call == nullptr) return false;
    if (!(lhs.call == nullptr && rhs.call == nullptr)) ret_val &= *lhs.call == *rhs.call;
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
