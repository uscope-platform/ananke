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

#include "data_model/HDL/parameters/components/Expression.hpp"
#include "data_model/HDL/parameters/components/Expression_component.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"

using namespace std::string_literals;

Expression_component::Expression_component(const Expression_component &c) {
    value = c.value;
    array_index = c.array_index;
    package_prefix = c.package_prefix;
    instance_prefix = c.instance_prefix;
    binary_size = c.binary_size;
    type = c.type;
    if (call) call = std::make_shared<HDL_function_call>(*c.call);
}


Expression_component::Expression_component() {
    value = 0;
}

Expression_component::Expression_component(const std::string &s, const component_type &t) {
    value =  s;
    type = t;
    if(t == number) {
        auto ret_val = process_number(s);
        value = ret_val.first;
        binary_size = ret_val.second;
    }
}

Expression_component::Expression_component(std::variant<hdl_integer, double> n, int64_t b_s) {
    if(std::holds_alternative<double>(n)) {
        value = std::get<double>(n);
    } else {
        value = std::get<hdl_integer>(n);
    }
    binary_size = b_s;
    type = number;
}

Expression_component::Expression_component(const std::shared_ptr<Parameter_value_base> &param) {
    if (!param->is_function()) {
        throw std::invalid_argument("Only functions are supported as expression components");
    }
    call = std::make_shared<HDL_function_call>(param->as<HDL_function_call>());
    type = function;
}

std::set<qualified_identifier> Expression_component::get_dependencies()const {
    std::set<qualified_identifier> result;
    if (is_identifier()){
        result.insert({package_prefix, instance_prefix, value.get_string()});
    }
    for (const auto &idx:array_index) {
        auto idx_deps = idx.get_dependencies();
        result.insert(idx_deps.begin(), idx_deps.end());
    }
    if (type == function && call) {
        auto call_deps = call->get_dependencies();
        result.insert(call_deps.begin(), call_deps.end());
    }
    return result;
}

void Expression_component::propagate_function(HDL_function_def def) {
    for (auto &component:array_index) {
        component.propagate_function(def);
    }
}


bool operator==(const Expression_component &lhs, const Expression_component &rhs) {
     bool ret_val = true;
    ret_val &= lhs.value == rhs.value;
    ret_val &= lhs.array_index == rhs.array_index;
    ret_val &= lhs.package_prefix == rhs.package_prefix;
    ret_val &= lhs.instance_prefix == rhs.instance_prefix;
    ret_val &= lhs.binary_size == rhs.binary_size;
    if (lhs.call == nullptr ^ rhs.call == nullptr) return false;
    if (!(lhs.call == nullptr && rhs.call == nullptr)) ret_val &= *lhs.call == *rhs.call;
    return ret_val;
}

std::string Expression_component::print_value() const {
    std::string ret_val;
     if(is_numeric()){
        ret_val = std::to_string(value.get_integer());
    } else {
        if(!array_index.empty()){
            ret_val = value.get_string() + print_index(array_index);
        } else {
            ret_val = value.get_string();
        }
    }


    return ret_val;
}

std::pair<resolved_parameter, int64_t>  Expression_component::process_number(const std::string &s) {

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
        // Process value
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
        // Process size

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

Expression_component::operator_type_t Expression_component::get_operator_type() {
    if(!operators_types.contains(value.get_string())){
        throw std::runtime_error("Error: attempted to get the type of a non operator/function expression component");
    }
    return operators_types[value.get_string()];
}

int64_t Expression_component::get_operator_precedence() {
    if (value.is_integer())  throw std::runtime_error("Error: attempted to get the precedence of a number");
    auto string_value = value.get_string();
    if(!operators_precedence.contains(string_value)){
        throw std::runtime_error("Error: attempted to get the precedence of a non operator/function expression component");
    }
    return operators_precedence[string_value];
}

std::optional<resolved_parameter> Expression_component::get_value() const {
    return value;
}

std::optional<resolved_parameter> Expression_component::get_value(const std::map<qualified_identifier, resolved_parameter> &context) const {
    if (type == identifier) {
        qualified_identifier id{package_prefix, instance_prefix, value.get_string()};
        auto it = context.find(id);
        if (it != context.end()) {
            const auto &resolved = it->second;
            if (array_index.empty()) return resolved;

            std::vector<int64_t> indices;
            for (const auto &idx_expr : array_index) {
                auto idx_val = const_cast<Expression&>(idx_expr).evaluate(context);
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

bool Expression_component::is_right_associative() {
    if (value.is_integer())  throw std::runtime_error("Error: attempted to get the associativity of a number");
    auto string_value = value.get_string();
    if((type != operation) && (type != function)){
        throw std::runtime_error("Error: attempted to get the right associativity of a non operator/function expression component");
    }
    return right_associative_set.contains(value.get_string());
}

std::string Expression_component::print_index(const std::vector<Expression> &index) const {
    std::string ret_val;
    for(const auto &item:index){
        ret_val += "[" + item.print() + "]";
    }
    return ret_val;
}

nlohmann::json Expression_component::dump() {
    nlohmann::json ret;

    return ret;
}


void Expression_component::set_array_index(const std::vector<Expression> &v) {
    array_index = v;
}

void Expression_component::add_array_index(const Expression &c) {
    array_index.push_back(c);
}

std::vector<Expression> Expression_component::get_array_index() {
    return array_index;
}

Expression_component::component_type Expression_component::get_type(const std::string &s) {
    if(ctre::match<R"(^\d+$)">(s) || ctre::search<R"(^\d*'(s)?(h|d|o|b)([0-9a-fA-F]+))">(s)|| ctre::match<R"(^[+\-]?(\d+\.\d*|\.\d+)([eE][+\-]?\d+)?$|^[+\-]?\d+[eE][+\-]?\d+$)">(s)) return number;
    if(is_string_operator(s)) return operation;
    if(is_string_function(s)) return function;
    if(is_string_parenthesis(s)) return parenthesis;
    if(s.starts_with("\"") | ctre::match<R"(\d+(\.\d+)?(s|ms|us|ns|ps|fs))">(s)) return string;
    return identifier;
}
