//  Copyright 2025 Filippo Savi
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

#include "data_model/HDL/factories/HDL_functions_factory.hpp"

void HDL_functions_factory::add_argument(const std::string &a) {
    f.add_argument(a);
}

void HDL_functions_factory::add_component(const Expression_component &c) {
    if (phase == body) {
        if (in_bit_selection) {
            bit_index.push_back(c);
        } else if (is_raw_body()) {
            new_expression.push_back(c);
        } else {
            expr_factory.add_component(c);
        }
    }
}

void HDL_functions_factory::add_value(const std::shared_ptr<Parameter_value_base> &v) {
    if (cast_factory.active()) {
        cast_factory.consume(v);
    } else if (c_factory.active()) {
        c_factory.consume(v);
    } else if (r_factory.active()) {
        r_factory.consume(v);
    } else {
        assignment_value = v;
    }
}

void HDL_functions_factory::close_lvalue() {
    if(current_assigned_variable == f.name) {
        f.start_assignment(current_assigned_variable, bit_index);
    } else {
        ignore_assignment = true;
    }
    new_expression.clear();
    bit_index = Expression();
}

void HDL_functions_factory::close_assignment() {
    if(!ignore_assignment) {
        if (assignment_value != nullptr) {
            f.close_assignment(assignment_value);
        } else {
            f.close_assignment(std::make_shared<Expression>(new_expression));
        }
    }

    ignore_assignment = false;
    new_expression.clear();
}

HDL_function_def HDL_functions_factory::get_function() {
    auto current_function = f;
    f = HDL_function_def();
    new_expression.clear();
    active = false;
    return current_function;
}

void HDL_functions_factory::start_replication() {
    r_factory.start_replication(true);
    expr_factory.decrease_level();
}

void HDL_functions_factory::stop_replication() {
    assignment_value = r_factory.result();
    expr_factory.increase_level();
}

void HDL_functions_factory::start_concat() {
    c_factory.start_concatenation();
    expr_factory.push_level();
}

void HDL_functions_factory::stop_concat() {
    c_factory.stop_concatenation();
    assignment_value = c_factory.result();
    expr_factory.pop_level();
}

void HDL_functions_factory::start_cast(bool expression_size) {
    cast_factory.start();
    if (expression_size) {
        start_expression();
    } else {
        expr_factory.decrease_level();
    }
}

void HDL_functions_factory::stop_cast() {
    auto expr = expr_factory.get_expression();
    expr_factory.clear_expression();
    if (expr.has_value()) {
        cast_factory.consume(std::make_shared<Expression>(expr.value()));
    }
    expr_factory.increase_level();


    auto cast_value = cast_factory.result();
    if (cast_factory.active()) {
        cast_factory.consume(cast_value);
    } else if (c_factory.active()) {
        c_factory.consume(cast_value);
    } else if (r_factory.active()) {
        r_factory.consume(cast_value);
    } else {
        assignment_value = cast_value;
    }
}

void HDL_functions_factory::set_cast_type(const std::string &t) {
    cast_factory.set_type(t);
}

void HDL_functions_factory::advance_cast() {
    auto expr = expr_factory.get_expression();
    if (expr.has_value()) {
        cast_factory.consume(std::make_shared<Expression>(expr.value()));
        expr_factory.clear_expression();
    }
    cast_factory.advance_cast();
}

void HDL_functions_factory::start_expression() {
    expr_factory.start_expression();
}

void HDL_functions_factory::stop_expression() {
    expr_factory.stop_expression();
    if (expr_factory.get_level() == 0) {
        auto expr = expr_factory.get_expression();
        if (expr.has_value()) {
            add_value(std::make_shared<Expression>(expr.value()));
        }
        expr_factory.clear_expression();
    }
}

bool HDL_functions_factory::is_component_relevant() const {
    if (paused) return false;
    return expr_factory.active() || c_factory.active() || r_factory.active() || cast_factory.active();
}

void HDL_functions_factory::start_bit_selection() {
    if (!r_factory.active()) {
        in_bit_selection = true;
        bit_index = Expression();
    }
}

void HDL_functions_factory::stop_bit_selection() {
    if (in_bit_selection) {
        expr_factory.add_index(bit_index);
        in_bit_selection = false;
    }
}
