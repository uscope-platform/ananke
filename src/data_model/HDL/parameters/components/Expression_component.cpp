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

#include "data_model/HDL/parameters/components/Expression.hpp"
#include "data_model/HDL/parameters/components/Expression_component.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"

using namespace std::string_literals;

const std::regex Expression_component::sv_constant_regex(R"(^\d*'(s)?(h|d|o|b)([0-9a-fA-F]+))");
const std::regex Expression_component::number_regex(R"(^\d+$)");
const std::regex Expression_component::float_regex(R"(^[+-]?(\d+\.\d*|\.\d+)([eE][+-]?\d+)?$|^[+-]?\d+[eE][+-]?\d+$)");
const std::regex Expression_component::size_regex(R"(^(\d*)'[0-9a-zA-Z]+)");
const std::regex Expression_component::time_lit_regex(R"(\d+(\.\d+)?(s|ms|us|ns|ps|fs))");

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

Expression_component::Expression_component(std::variant<int64_t, double> n, int64_t b_s) {
    if(std::holds_alternative<double>(n)) {
        value = std::get<double>(n);
    } else {
        value = std::get<int64_t>(n);
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

bool Expression_component::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &const_value) {
    bool retval = true;

    bool index_need_rewrite = false;
    std::vector<int64_t> indices;
    for (auto &component:array_index) {
        retval &= component.propagate_constant(constant_id, const_value);
        auto idx_val = component.evaluate();
        if (idx_val.has_value()) {
            index_need_rewrite = true;
            if (!idx_val.value().is_integer()) return false;
            indices.push_back(idx_val.value().get_integer());
        }
    }
    if(array_index.size() != indices.size()) return false;
    if(index_need_rewrite) {
        for(int i = 0; i<array_index.size(); i++) {
            array_index[i] = {Expression_component(indices[i], 0)};
        }
    }
    if (value.is_string()) {
        if (value.get_string() == constant_id.name && package_prefix == constant_id.prefix) {
            if (!array_index.empty()) {
                if(const_value.is_int_array()) {
                    auto values = const_value.get_int_array();
                    auto array_val = values.get_value(indices);
                    if (array_val.has_value()) {
                        type = number;
                        value = array_val.value();
                    }
                    else {
                        type = number;
                        value = 0; // if the array value is not found (because of some dimensional issue) substitute with a 0 rather than crashing
                    }
                }else if(const_value.is_string_array()) {
                    auto values = const_value.get_string_array();
                    auto array_val = values.get_value(indices);
                    if (array_val.has_value()) {
                        type = string;
                        value = array_val.value();
                    }
                    else {
                        type = string;
                        value = ""s; // if the array value is not found (because of some dimensional issue) substitute with a 0 rather than crashing
                    }
                } else {
                    std::bitset<64> bits(const_value.get_integer());
                    type = number;
                    value = static_cast<int64_t>(bits[indices[0]]);
                }

            } else {
                type = number;
                value = const_value;
            }
        }
        return retval;
    }
    return true;
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
    if(test_parameter_type(float_regex, s)) {
        ret_size = 64;
        ret_value = std::stod(s);
    } else if(test_parameter_type(number_regex, s)) {
        ret_value = static_cast<int64_t>(std::stoul(s));
        ret_size = calculate_binary_size(ret_value.get_integer());
    } else if(test_parameter_type(sv_constant_regex, s)){
        std::smatch base_match;
        if(std::regex_search(s, base_match, sv_constant_regex)){
            // Process value
            std::string base;
            std::string str_value;
            switch (base_match.size()) {
                case 3:
                    base = base_match[1].str();
                    str_value = base_match[2].str();
                    break;
                case 4:
                    base = base_match[2].str();
                    str_value = base_match[3].str();
                    break;
            }
            if(base =="h"){
                ret_value = static_cast<int64_t>(std::stoul(str_value, nullptr, 16));
            } else if(base =="d") {
                ret_value = static_cast<int64_t>(std::stoul(str_value, nullptr, 10));
            } else if(base =="o") {
                ret_value = static_cast<int64_t>(std::stoul(str_value, nullptr, 8));
            } else if(base =="b") {
                ret_value = static_cast<int64_t>(std::stoul(str_value, nullptr, 2));
            }
            // Process size

            if(std::regex_search(s, base_match, size_regex)){
                if(!base_match[1].str().empty()) {
                    ret_size = std::stoll(base_match[1].str());
                } else {
                    ret_size = calculate_binary_size(ret_value.get_integer());
                }
            }
        }
    }
    return std::make_pair(ret_value, ret_size);
}

bool Expression_component::test_parameter_type(const std::regex &r, const std::string &s) {

    std::smatch base_match;
    if(std::regex_search(s, base_match, r)){
        return true;
    } else{
        return false;
    }
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
    if (type == function) {
        if (call) return call->evaluate();
        else return std::nullopt;
    } else {
        return value;
    }
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

int64_t Expression_component::calculate_binary_size(int64_t in) {
    auto n_bits = std::log2(in);
    if(std::isinf(n_bits) || n_bits == 0) {
        return 1;
    }else{
        return  std::ceil(n_bits);
    }
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
    if(test_parameter_type(number_regex, s) || test_parameter_type(sv_constant_regex, s)|| test_parameter_type(float_regex, s)) return number;
    if(is_string_operator(s)) return operation;
    if(is_string_function(s)) return function;
    if(is_string_parenthesis(s)) return parenthesis;
    if(s.starts_with("\"") | test_parameter_type(time_lit_regex, s)) return string;
    return identifier;
}
