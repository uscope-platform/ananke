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

#include "data_model/HDL/parameters/Initialization_list.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"

Initialization_list::Initialization_list(const Initialization_list &i) {
    unpacked_dimensions = i.unpacked_dimensions;
    packed_dimensions = i.packed_dimensions;
    solved_value = i.solved_value;

    expression_leaves = i.expression_leaves;
    default_initialization = i.default_initialization;
    scalar = i.scalar;
}

Initialization_list Initialization_list::clone() const{
    Initialization_list i_l;
    i_l.scalar = scalar;
    i_l.solved_value = solved_value;
    i_l.unpacked_dimensions = unpacked_dimensions;
    i_l.packed_dimensions = packed_dimensions;
    i_l.default_initialization = default_initialization;
    for(auto &item:expression_leaves) {
        i_l.expression_leaves.push_back(item->clone_ptr());
    }
    return i_l;
}

Initialization_list::Initialization_list(const std::shared_ptr<Parameter_value_base> &e) {
    expression_leaves.push_back(e);
}


void Initialization_list::add_dimension(const dimension_t &d) {
    scalar = false;
    if(d.packed){
        packed_dimensions.push_back(d);
    } else{
        unpacked_dimensions.push_back(d);
    }

}

void Initialization_list::set_dimensions(const std::vector<dimension_t> &d, bool packed) {
    if (packed) {
        packed_dimensions = d;
    } else {
        unpacked_dimensions = d;
    }
}

void Initialization_list::add_item(const std::shared_ptr<Parameter_value_base> &e) {
    scalar = false;
    expression_leaves.push_back(e);
}

bool operator==(const Initialization_list &lhs, const Initialization_list &rhs) {
    bool ret = true;

    // last dimension is an internal variable only needed during construction, as such it does not need comparison
    if(lhs.expression_leaves.size() != rhs.expression_leaves.size()) return false;
    for(int i = 0; i<lhs.expression_leaves.size(); i++) {
        ret &= *lhs.expression_leaves[i] == *rhs.expression_leaves[i];
    }


    ret &= lhs.default_initialization == rhs.default_initialization;
    ret &= lhs.scalar == rhs.scalar;
    if(lhs.unpacked_dimensions.size() != rhs.unpacked_dimensions.size()) return false;
    for(int i = 0; i<lhs.unpacked_dimensions.size(); i++){
        ret &= lhs.unpacked_dimensions[i].packed == rhs.unpacked_dimensions[i].packed;
        ret &= lhs.unpacked_dimensions[i].first_bound == rhs.unpacked_dimensions[i].first_bound;
        ret &= lhs.unpacked_dimensions[i].second_bound == rhs.unpacked_dimensions[i].second_bound;
    }

    if(lhs.packed_dimensions.size() != rhs.packed_dimensions.size()) return false;
    for(int i = 0; i<lhs.packed_dimensions.size(); i++){
        ret &= lhs.packed_dimensions[i].packed == rhs.packed_dimensions[i].packed;
        ret &= lhs.packed_dimensions[i].first_bound == rhs.packed_dimensions[i].first_bound;
        ret &= lhs.packed_dimensions[i].second_bound == rhs.packed_dimensions[i].second_bound;
    }

    ret &= lhs.solved_value == rhs.solved_value;

    return ret;
}

bool Initialization_list::empty() const {

    if (scalar) {
        if (expression_leaves.empty()) return true;
        return expression_leaves[0]->empty();

    }
    return expression_leaves.empty();
}


std::optional<resolved_parameter>Initialization_list::get_values() {
    mdarray<int64_t> ret;
    mdarray<std::string> ret_s;
    if(default_initialization){
        return process_default_initialization();
    }
    bool ret_string = true;
    for(auto &expr:expression_leaves | std::views::reverse) {
        auto expr_depth = expr->get_depth();
        bool pack = unpacked_dimensions.size() <= expr_depth;
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

    if(unpacked_dimensions.empty()) return ret.get_scalar();
    return ret;
}



std::set<qualified_identifier> Initialization_list::get_dependencies() {
    std::set<qualified_identifier> result;
    for (auto &dim:unpacked_dimensions) {
        auto deps = dim.first_bound.get_dependencies();
        result.insert(deps.begin(), deps.end());
        deps = dim.second_bound.get_dependencies();
        result.insert(deps.begin(), deps.end());
    }

    for (auto &dim:packed_dimensions) {
        auto deps = dim.first_bound.get_dependencies();
        result.insert(deps.begin(), deps.end());
        deps = dim.second_bound.get_dependencies();
        result.insert(deps.begin(), deps.end());
    }
    for (auto &expr:expression_leaves) {
        std::set<qualified_identifier> deps = expr->get_dependencies();
        result.insert(deps.begin(), deps.end());
    }
    return result;

}

void Initialization_list::set_solved_value(const resolved_parameter &val) {
    solved_value = val;
}

std::variant<mdarray<int64_t>, mdarray<std::string>::md_1d_array> Initialization_list::get_solved_value() const {
    if(std::holds_alternative<std::string>(solved_value)) {
        mdarray<std::string>::md_1d_array arr= {std::get<std::string>(solved_value)};
        return arr;
    } else if(std::holds_alternative<int64_t>(solved_value)) {
        if (!std::holds_alternative<mdarray<int64_t>>(solved_value)) {
            return mdarray({1,1,1}, std::get<int64_t>(solved_value));
        } else {
            mdarray<int64_t> v;
            v.set_scalar(std::get<int64_t>(solved_value));
        }
    } else if(std::holds_alternative<mdarray<int64_t>>(solved_value)) {
        return std::get<mdarray<int64_t>>(solved_value);
    }

    return {};
}

void PrintTo(const Initialization_list &il, std::ostream *os) {
    if (il.scalar) {
        if (il.expression_leaves.empty()) *os << "!!EMPTY SCALAR!!";
        else *os << il.expression_leaves[0]->print();
    }
    std::string result = "-----------------------------\n";
    result += "-----------------------------\n";
    result += "UNPACKED DIMENSIONS\n";
    for(const auto &item:il.unpacked_dimensions){
        result +="[" + item.first_bound.print() + ":" + item.second_bound.print()+ "]";
    }
    result += "PACKED DIMENSIONS\n";
    for(const auto &item:il.packed_dimensions){
        result +="[" + item.first_bound.print() + ":" + item.second_bound.print()+ "]";
    }

    result += "EXPRESSION LEAVES\n";
    for(const auto &item:il.expression_leaves){
        result +=  "  " + item->print() + "\n";
    }

    result += "-----------------------------\n";
    result += "-----------------------------\n";

    *os << result;
}


std::optional<resolved_parameter> Initialization_list::evaluate() {
    std::optional<resolved_parameter> result;
    if (scalar) {
        if(expression_leaves[0]->is_expression() || expression_leaves[0]->is_function()) {
            result = expression_leaves[0]->evaluate(false);
        } else if(expression_leaves[0]->is_concatenation() || expression_leaves[0]->is_replication()) {
            bool packed_concat = unpacked_dimensions.empty();
            result = expression_leaves[0]->evaluate(packed_concat);
        }
    } else {
       result  = get_values();
    }
    return result;
}

bool Initialization_list::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value) {
    bool retval = true;

    for (auto &dim: packed_dimensions) {
        retval &= dim.first_bound.propagate_constant(constant_id, value);
        retval &= dim.second_bound.propagate_constant(constant_id, value);
    }

    for (auto &dim: unpacked_dimensions) {
        retval &= dim.first_bound.propagate_constant(constant_id, value);
        retval &= dim.second_bound.propagate_constant(constant_id, value);
    }

    for (auto &expr:expression_leaves) {
        retval &=  expr->propagate_constant(constant_id, value);
    }
    return retval;
}

void Initialization_list::propagate_function(const HDL_function_def &def) {
    for (auto &expr:expression_leaves) {
        expr->propagate_function(def);
    }
}


std::optional<std::shared_ptr<Parameter_value_base>> Initialization_list::get_scalar() {
    if (scalar) return expression_leaves[0];
    return std::nullopt;
}

void Initialization_list::set_scalar(const std::shared_ptr<Parameter_value_base> &expr) {
    scalar = true;
    expression_leaves  = {expr};
}

void Initialization_list::clear_scalar() {
    if (scalar)
    expression_leaves[0] = std::make_shared<Expression>();
}

void Initialization_list::push_scalar_component(const Expression_component &comp) {
    if (expression_leaves.empty()) expression_leaves.push_back(std::make_shared<Expression>());
    if(expression_leaves[0]->is_expression()) {
        auto expr = static_cast<Expression *>(expression_leaves[0].get());
        expr->push_back(comp);
    }
}

resolved_parameter Initialization_list::process_default_initialization() {

    std::vector<uint64_t> dimensions;
    mdarray<int64_t> result;

    if(unpacked_dimensions.size()>3){
        throw std::runtime_error("Error: unpacked arrays with more than 3 dimensions are not supported");
    }

    for(auto &item : unpacked_dimensions){
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
        mdarray<int64_t> ret_i = {dimensions,std::get<int64_t>(init_value.value())};
        return ret_i;
    }
    if(std::holds_alternative<std::string>(init_value.value())) {
        mdarray<std::string> ret_s = {dimensions,std::get<std::string>(init_value.value())};
        return ret_s;
    }
    throw std::runtime_error("Error: initializer of default array should be a string or a number");

}




