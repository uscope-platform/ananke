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
    bit_index = std::make_shared<Expression_v2>();
}

void HDL_functions_factory::add_argument(const std::string &a) {
    f.add_argument(a);
}

void HDL_functions_factory::set_operation(Expression_v2::expression_operator op) {
    if (phase == body) {
        if (in_bit_selection) {
            bit_index->as<Expression_v2>().set_operation(op);
        } else {
            expr_factory_.set_operation(op);
        }
    }
}
void HDL_functions_factory::add_component(const std::shared_ptr<Parameter_value_base> &c) {
    if (phase == body) {
        if (in_bit_selection) {
            if (!bit_index->as<Expression_v2>().get_lhs()) {
                bit_index->as<Expression_v2>().set_lhs(c);
            } else {
                bit_index->as<Expression_v2>().set_rhs(c);
            }
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

    auto unwrapped_bit_index = Expression_v2::unwrap(*std::dynamic_pointer_cast<Expression_v2>(bit_index));
    if (bit_index->is<Expression_v2>() &&
        bit_index->as<Expression_v2>().get_operation() == Expression_v2::none &&
        !bit_index->as<Expression_v2>().get_lhs()
        ) unwrapped_bit_index = nullptr;
    if(current_assigned_variable == f.name) {
        f.start_assignment(current_assigned_variable, unwrapped_bit_index);
    } else {
        ignore_assignment = true;
    }
    bit_index = std::make_shared<Expression_v2>();
}

void HDL_functions_factory::close_assignment() {
    if(!ignore_assignment && assignment_value != nullptr) {
        if (assignment_value->is<Expression_v2>()) {
            auto &e = assignment_value->as<Expression_v2>();
            if (e.get_operation() != Expression_v2::none) {
                f.close_assignment(assignment_value);
            } else if (auto lhs = e.get_lhs()) {
                f.close_assignment(lhs);
            } else if (auto lhs = e.get_lhs()) {
                f.close_assignment(lhs);
            } else {
                f.close_assignment(assignment_value);
            }
        } else {
            f.close_assignment(assignment_value);
        }
    }
    ignore_assignment = false;
}

HDL_function_def HDL_functions_factory::get_function() {
    auto current_function = f;
    f = HDL_function_def();
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
        auto expr = expr_factory_.get_expression_v2();
        expr_factory_.clear_expression();
        if (expr.has_value()) {
            if (expr->get_operation() != Expression_v2::none) {
                consumer_stack.top()->consume(Expression_v2::unwrap(std::move(*expr)));
            } else if (auto lhs = expr->get_lhs()) {
                consumer_stack.top()->consume(lhs);
            }
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
        auto expr = expr_factory_.get_expression_v2();
        if (expr.has_value()) {
            if (expr->get_operation() != Expression_v2::none) {
                cast->consume(Expression_v2::unwrap(std::move(*expr)));
            } else if (auto lhs = expr->get_lhs()) {
                cast->consume(lhs);
            }
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
        auto expr = expr_factory_.get_expression_v2();
        if (expr.has_value()) {
            if (expr->get_operation() != Expression_v2::none) {
                add_value(Expression_v2::unwrap(std::move(*expr)));
            } else if (auto lhs = expr->get_lhs()) {
                add_value(lhs);
            }
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
        bit_index = std::make_shared<Expression_v2>();
    }
}

void HDL_functions_factory::stop_bit_selection() {
    if (in_bit_selection) {
        expr_factory_.add_index(bit_index);
        in_bit_selection = false;
    }
}

