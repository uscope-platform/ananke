//  Copyright 2023 Filippo Savi
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

#include "data_model/HDL/parameters/HDL_parameter.hpp"


HDL_parameter::HDL_parameter(const HDL_parameter &c) {

    name = c.name;
    type = c.type;
    solved_value = c.solved_value;

    expression_leaves = c.expression_leaves;
    default_initialization = c.default_initialization;
}


bool operator==(const HDL_parameter &lhs, const HDL_parameter &rhs) {
    bool ret = true;

    ret &= lhs.name == rhs.name;

    // last dimension is an internal variable only needed during construction, as such it does not need comparison
    if(lhs.expression_leaves.size() != rhs.expression_leaves.size()) return false;
    for(int i = 0; i<lhs.expression_leaves.size(); i++) {
        ret &= *lhs.expression_leaves[i] == *rhs.expression_leaves[i];
    }


    ret &= lhs.default_initialization == rhs.default_initialization;
    ret &= lhs.type == rhs.type;

    ret &= lhs.solved_value == rhs.solved_value;

    return ret;
}

bool operator<(const HDL_parameter &lhs, const HDL_parameter &rhs) {
    return lhs.get_name()<rhs.get_name();
}


std::shared_ptr<HDL_parameter> HDL_parameter::clone() const {
    HDL_parameter par;
    par.name = name;
    par.type = type;
    par.solved_value = solved_value;
    par.default_initialization = default_initialization;
    for(auto &item:expression_leaves) {
        par.expression_leaves.push_back(item->clone_ptr());
    }
    return std::make_shared<HDL_parameter>(par);
}


void HDL_parameter::add_dimension(const dimension_t &d) {
    type.add_dimension(d);
}

std::optional<resolved_parameter> HDL_parameter::evaluate() {
    for (auto &l:expression_leaves) {
        auto container_size = type.evaluate_type();
        if (!container_size) return std::nullopt;
        l->set_container_sizes(container_size.value());
    }
    std::optional<resolved_parameter> result;
        if (type.is_scalar()) {
        if(expression_leaves[0]->is_expression() || expression_leaves[0]->is_function()) {
            result = expression_leaves[0]->evaluate(false);
        } else if(expression_leaves[0]->is_concatenation() || expression_leaves[0]->is_replication()) {
            result = expression_leaves[0]->evaluate(type.get_unpacked_dimensions().empty());
        }
    } else {
        result  = evaluate_vector();
    }
    return result;
}

bool HDL_parameter::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &constant_value) {
    bool retval = true;
    retval &= type.propagate_constant(constant_id, constant_value);

    for (auto &expr:expression_leaves) {
        retval &=  expr->propagate_constant(constant_id, constant_value);
    }
    return retval;
}

void HDL_parameter::propagate_function(const HDL_function_def &def) {
    for (auto &expr:expression_leaves) {
        expr->propagate_function(def);
    }
}

HDL_parameter::operator std::string() {
    if (!solved_value.has_value()) return "";
    std::string ret_val;
    if(std::holds_alternative<std::string>(solved_value.value())) {
        ret_val = std::get<std::string>(solved_value.value());
    }
    if(std::holds_alternative<int64_t>(solved_value.value())) {
        auto val = std::get<mdarray<int64_t>>(solved_value.value()).get_scalar();
        if(val.has_value()) ret_val =  std::to_string(val.value());
    }
    if(std::holds_alternative<mdarray<int64_t>>(solved_value.value())) {
        ret_val = "array value";
    }

    return ret_val;

}


void PrintTo(const HDL_parameter &param, std::ostream *os) {
    std::string result = param.to_string();
    *os << result;
}

bool HDL_parameter::empty() const {
    if (type.is_scalar()) {
        if (expression_leaves.empty()) return true;
        return expression_leaves[0]->empty();

    }
    return expression_leaves.empty();
}

void HDL_parameter::add_component(const Expression_component &component) {
    if (expression_leaves.empty()) expression_leaves.push_back(std::make_shared<Expression>());
    if(expression_leaves[0]->is_expression()) {
        auto expr = static_cast<Expression *>(expression_leaves[0].get());
        expr->push_back(component);
    }
}

void HDL_parameter::set_scalar(const std::shared_ptr<Parameter_value_base> &expr) {
    type.set_scalar(true);
    expression_leaves  = {expr};
}

void HDL_parameter::add_item(const std::shared_ptr<Parameter_value_base> &e) {
    type.set_scalar(false);
    expression_leaves.push_back(e);
}

std::string HDL_parameter::to_string() const {
    std::string result = "\nHDL parameter:\n  NAME: " + name +
                         "\n  TYPE: ";

    if (!solved_value.has_value()) result += "unknown parameter type";
    else if (std::holds_alternative<std::string>(solved_value.value())) result += "string_parameter";
    else if (std::holds_alternative<int64_t>(solved_value.value())) {
        result += "numeric_parameter";
        auto val = std::get<mdarray<int64_t>>(solved_value.value()).get_scalar();
        if(val.has_value()) result += "\n  VALUE: " + std::to_string(val.value());
        else result += "\n  VALUE: xxx";
    }
    else if (std::holds_alternative<mdarray<int64_t>>(solved_value.value())) result += "array_parameter";

    result += "\n  INITIALIZATION LIST:\n    ";

    std::ostringstream s;

    if (type.is_scalar()) {
        if (expression_leaves.empty()) result += "!!EMPTY SCALAR!!";
        else  result += expression_leaves[0]->print();
    }
    result += "-----------------------------\n";
    result += "-----------------------------\n";
    result += "UNPACKED DIMENSIONS\n";
    for(const auto &item:type.get_unpacked_dimensions()){
        result +="[" + item.first_bound.print() + ":" + item.second_bound.print()+ "]";
    }
    result += "PACKED DIMENSIONS\n";
    for(const auto &item:type.get_packed_dimensions()){
        result +="[" + item.first_bound.print() + ":" + item.second_bound.print()+ "]";
    }

    result += "EXPRESSION LEAVES\n";
    for(const auto &item:expression_leaves){
        result +=  "  " + item->print() + "\n";
    }

    result += "-----------------------------\n";
    result += "-----------------------------\n";

    return result;
}

std::set<qualified_identifier> HDL_parameter::get_dependencies() {
    std::set<qualified_identifier> result = type.get_dependencies();

    for (auto &expr:expression_leaves) {
        std::set<qualified_identifier> deps = expr->get_dependencies();
        result.insert(deps.begin(), deps.end());
    }
    return result;
}



inline resolved_parameter HDL_parameter::process_default_initialization() {

    std::vector<uint64_t> dimensions;
    mdarray<int64_t> result;

    if(type.get_unpacked_dimensions().size()>3){
        throw std::runtime_error("Error: unpacked arrays with more than 3 dimensions are not supported");
    }

    for(auto &item : type.get_unpacked_dimensions()){
        auto first_dim = item.first_bound.evaluate(false);
        auto second_dim = item.second_bound.evaluate(false);
        if (!first_dim.has_value() || !second_dim.has_value())   throw std::runtime_error("Error: dimensions of default initialized parameters should be fully defined");
        if (!std::holds_alternative<int64_t>(first_dim.value()) || !std::holds_alternative<int64_t>(second_dim.value()))   throw std::runtime_error("Error: dimensions of default initialized parameters should be integers");
        auto first_i = std::get<int64_t>(first_dim.value());
        auto second_i = std::get<int64_t>(second_dim.value());
        dimensions.push_back(std::max(first_i, second_i)+1);
    }

    while(dimensions.size()<3){
        dimensions.insert(dimensions.begin(), 1);
    }

    auto init_value = expression_leaves[0]->evaluate(false);

    if (!init_value.has_value()) throw std::runtime_error("Error: initializer of default array should be defined");
    if(std::holds_alternative<int64_t>(init_value.value())) {
        mdarray ret_i = {dimensions,std::get<int64_t>(init_value.value())};
        return ret_i;
    }
    if(std::holds_alternative<std::string>(init_value.value())) {
        mdarray ret_s = {dimensions,std::get<std::string>(init_value.value())};
        return ret_s;
    }
    throw std::runtime_error("Error: initializer of default array should be a string or a number");

}

nlohmann::json HDL_parameter::dump() {
    nlohmann::json ret;


    ret["name"] = name;

    if (!solved_value.has_value())  ret["type"] = "unknown parameter type";
    else if (std::holds_alternative<std::string>(solved_value.value())) {
        ret["type"] = "string_parameter";
        std::vector<std::string> values_s;
        auto value = solved_value.value();
        values_s.push_back(std::regex_replace(std::get<std::string>(value), std::regex(R"ctrl(^"(.*)"$)ctrl"), "$1"));
        ret["value"] = values_s;
    }
    else if (std::holds_alternative<int64_t>(solved_value.value())) {
        ret["type"] = "numeric_parameter";
        ret["value"]= std::vector<int64_t>();
        if(solved_value.has_value()) ret["value"].push_back(std::get<int64_t>(solved_value.value()));
    }
    else if (std::holds_alternative<mdarray<int64_t>>(solved_value.value())) {
        ret["type"] = "array_parameter";
        ret["value"] = std::get<mdarray<int64_t>>(solved_value.value()).dump();
    }

    return ret;
}

inline std::optional<resolved_parameter> HDL_parameter::evaluate_vector() {
    mdarray<int64_t> ret;
    mdarray<std::string> ret_s;
    if(default_initialization){
        return process_default_initialization();
    }
    bool ret_string = true;
    for(auto &expr:expression_leaves | std::views::reverse) {
        auto expr_depth = expr->get_depth();
        bool pack = type.get_unpacked_dimensions().size() <= expr_depth;
        auto expr_value = expr->evaluate(pack);
        if (expr_value.has_value()) {
            if (std::holds_alternative<std::string>(expr_value.value())) {
                auto stacked_arr = mdarray<std::string>::stack(ret_s, std::get<std::string>(expr_value.value()));
                if (stacked_arr.has_value()) {
                    ret_s = stacked_arr.value();
                }
            } else if (std::holds_alternative<int64_t>(expr_value.value())) {
                ret_string = false;
                auto stacked_arr = mdarray<int64_t>::stack(ret, std::get<int64_t>(expr_value.value()));
                if (stacked_arr.has_value()) {
                    ret = stacked_arr.value();
                }
            } else if (std::holds_alternative<mdarray<int64_t>>(expr_value.value())) {
                ret_string = false;
                auto stacked_arr = mdarray<int64_t>::stack(ret, std::get<mdarray<int64_t>>(expr_value.value()));
                if (stacked_arr.has_value()) {
                    ret = stacked_arr.value();
                }
            }
        }
    }

    if(ret_string) {
        return ret_s;
    }

    if(type.get_unpacked_dimensions().empty()) return ret.get_scalar();
    return ret;
}
