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
#include "data_model/HDL/factories/parameters/cast_factory.hpp"
#include "data_model/HDL/factories/parameters/concatenation_factory.hpp"
#include "data_model/HDL/factories/parameters/replication_factory.hpp"

void HDL_functions_factory::start_assignment(const std::string &n) {
    current_assigned_variable = n;
    bit_index = std::make_shared<Expression>();
}

void HDL_functions_factory::add_argument(const std::string &a) {
    f.add_argument(a);
}

void HDL_functions_factory::add_component(const Token &c) {
    if (phase == body) {
        if (in_bit_selection) {
            bit_index->as<Expression>().push_back(c);
        } else if (is_raw_body()) {
            new_expression.push_back(c);
        } else {
            expr_factory_.add_component(c);
        }
    }
}

void HDL_functions_factory::add_value(const std::shared_ptr<Parameter_value_base> &v) {
    if (!consumer_stack.empty()) {
        consumer_stack.top()->consume(v);
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
    bit_index = std::make_shared<Expression>();
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
    auto repl = std::make_unique<replication_factory>();
    repl->start_replication(true);
    consumer_stack.push(std::move(repl));
    expr_factory_.decrease_level();
}

void HDL_functions_factory::stop_replication() {
    if (top_as<replication_factory>()) {
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        if (!consumer_stack.empty()) {
            consumer_stack.top()->consume(result);
        } else {
            assignment_value = result;
        }
        expr_factory_.increase_level();
    }
}

void HDL_functions_factory::start_concat() {
    auto concat = std::make_unique<concatenation_factory>();
    concat->start_concatenation();
    consumer_stack.push(std::move(concat));
    expr_factory_.push_level();
}

void HDL_functions_factory::stop_concat() {
    if (top_as<concatenation_factory>()) {
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        if (!consumer_stack.empty()) {
            consumer_stack.top()->consume(result);
        } else {
            assignment_value = result;
        }
        expr_factory_.pop_level();
    }
}

void HDL_functions_factory::start_cast(bool expression_size) {
    auto cast = std::make_unique<cast_factory>();
    cast->start();
    consumer_stack.push(std::move(cast));
    if (expression_size) {
        start_expression();
    } else {
        expr_factory_.decrease_level();
    }
}

void HDL_functions_factory::stop_cast() {
    if (top_as<cast_factory>()) {
        auto expr = expr_factory_.get_expression();
        expr_factory_.clear_expression();
        if (expr.has_value()) {
            consumer_stack.top()->consume(std::make_shared<Expression>(expr.value()));
        }
        expr_factory_.increase_level();

        auto cast_value = consumer_stack.top()->result();
        consumer_stack.pop();
        if (!consumer_stack.empty()) {
            consumer_stack.top()->consume(cast_value);
        } else {
            assignment_value = cast_value;
        }
    }
}

void HDL_functions_factory::set_cast_type(const std::string &t) {
    auto* cast = top_as<cast_factory>();
    if (cast) {
        cast->set_type(t);
    }
}

void HDL_functions_factory::advance_cast() {
    auto* cast = top_as<cast_factory>();
    if (cast) {
        auto expr = expr_factory_.get_expression();
        if (expr.has_value()) {
            cast->consume(std::make_shared<Expression>(expr.value()));
            expr_factory_.clear_expression();
        }
        cast->advance_cast();
    }
}

void HDL_functions_factory::start_expression() {
    expr_factory_.start_expression(false);
}

void HDL_functions_factory::stop_expression() {
    expr_factory_.stop_expression(false);
    if (expr_factory_.get_level() == 0) {
        auto expr = expr_factory_.get_expression();
        if (expr.has_value()) {
            add_value(std::make_shared<Expression>(expr.value()));
        }
        expr_factory_.clear_expression();
    }
}

bool HDL_functions_factory::is_component_relevant() const {
    if (paused) return false;
    return expr_factory_.active() || !consumer_stack.empty();
}

void HDL_functions_factory::start_bit_selection() {
    if (!top_as<replication_factory>()) {
        in_bit_selection = true;
        bit_index = std::make_shared<Expression>();
    }
}

void HDL_functions_factory::stop_bit_selection() {
    if (in_bit_selection) {
        expr_factory_.add_index(bit_index);
        in_bit_selection = false;
    }
}
