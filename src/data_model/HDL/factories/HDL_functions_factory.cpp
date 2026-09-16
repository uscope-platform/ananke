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
#include "data_model/HDL/factories/parameters/function_calls_factory.hpp"
#include "data_model/HDL/factories/parameters/ternary_factory.hpp"
#include "data_model/HDL/factories/parameters/streaming_factory.hpp"

void HDL_functions_factory::start_assignment(const qualified_identifier &n) {
    current_assigned_variable = n;
    lvalue_open = true;
}

void HDL_functions_factory::start_return() {
    current_assigned_variable = qualified_identifier(f.get_name());
    lvalue_open = false;
}

void HDL_functions_factory::add_argument(const std::string &a) {
    f.add_argument(a);
}

void HDL_functions_factory::set_operation(Expression_v2::expression_operator op) {
    if (phase == body) {
        // Selections build in a pushed expression context (see
        // start_bit_selection), so no special-casing here: index operators
        // nest exactly like statement operators.
        expr_factory_.set_operation(op);
    }
}
void HDL_functions_factory::add_component(const std::shared_ptr<Expression_base> &c) {
    if (phase == body) {
        expr_factory_.add_component(c);
    }
}

void HDL_functions_factory::sink_value(const std::shared_ptr<Expression_base> &v) {
    if (!consumer_stack.empty()) {
        consumer_stack.top()->consume(v);
    } else if (selection_depth > 0 || expr_factory_.active()) {
        expr_factory_.consume(v);
    } else {
        assignment_value = v;
    }
}

void HDL_functions_factory::add_value(const std::shared_ptr<Expression_base> &v) {
    sink_value(v);
}

void HDL_functions_factory::close_lvalue() {
    lvalue_open = false;
    current_lhs_index = pending_lhs_index;
    pending_lhs_index = nullptr;
}

void HDL_functions_factory::finish_assignment() {
    if (!assignment_value) return;
    lvalue_open = false;

    auto val = assignment_value;
    if (val->is<Expression_v2>()) {
        auto &e = val->as<Expression_v2>();
        if (e.get_operation() != Expression_v2::none) {
            // keep as-is
        } else if (auto lhs = e.get_lhs()) {
            val = lhs;
        }
    }

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target(current_assigned_variable);
    if (current_lhs_index) stmt->set_index(current_lhs_index);
    stmt->set_value(val);
    f.add_statement(stmt);
    current_lhs_index = nullptr;
}

hdl_function_statement HDL_functions_factory::get_function() {
    auto current_function = f;
    current_function.set_language(language);
    f = hdl_function_statement();
    return_type_name.clear();
    active = false;
    return current_function;
}

void HDL_functions_factory::start_replication() {
    auto repl = std::make_unique<replication_factory>();
    repl->start_replication(true);
    consumer_stack.push(std::move(repl));
    expr_factory_.push_level();
}

void HDL_functions_factory::stop_replication() {
    if (top_as<replication_factory>()) {
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        sink_value(result);
        expr_factory_.pop_level();
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
        sink_value(result);
        expr_factory_.pop_level();
    }
}

void HDL_functions_factory::start_cast(bool expression_size) {
    auto cast = std::make_unique<cast_factory>();
    cast->start();
    auto pending = expr_factory_.get_expression_v2();
    if (pending.has_value() && pending->get_operation() != Expression_v2::none &&
        !pending->get_rhs() && expr_factory_.active()) {
        expr_factory_.start_expression(true);
        cast->set_outer_suspended(true);
        }
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
        const bool resume_outer = top_as<cast_factory>()->is_outer_suspended();
        consumer_stack.pop();

        if (resume_outer) {
            // Resume an outer `A op` suspended in advance_cast: it continues
            // as the pending expression (completing at statement end like
            // any other) instead of assigning the bare cast.
            expr_factory_.consume(cast_value);
            expr_factory_.stop_expression(true);
        } else {
            sink_value(cast_value);
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

void HDL_functions_factory::start_expression(bool new_expr) {
    expr_factory_.start_expression(new_expr);
}

void HDL_functions_factory::start_function_call(const std::string &name) {
    auto calls = std::make_unique<function_calls_factory>();
    calls->set_language(language);
    calls->start_function(name);
    consumer_stack.push(std::move(calls));
    expr_factory_.pause();
    expr_factory_.push_level();
}

void HDL_functions_factory::stop_function_call() {
    if (top_as<function_calls_factory>()) {
        auto call = consumer_stack.top()->result();
        consumer_stack.pop();
        expr_factory_.pop_level();
        sink_value(call);
    }
}

void HDL_functions_factory::set_function_package_prefix(const std::string &p) {
    if (top_as<function_calls_factory>()) {
        top_as<function_calls_factory>()->set_package_prefix(p);
    }
}

void HDL_functions_factory::add_call_argument(const std::shared_ptr<Expression_base> &ec) {
    if (top_as<function_calls_factory>()) {
        consumer_stack.top()->consume(ec);
    } else {
        add_component(ec);
    }
}

void HDL_functions_factory::start_ternary() {
    expr_factory_.push_level();
    auto ternary = std::make_unique<ternary_factory>();
    ternary->start_conditional();
    consumer_stack.push(std::move(ternary));
}

void HDL_functions_factory::start_streaming() {
    expr_factory_.push_level();
    auto stream = std::make_unique<streaming_factory>();
    stream->start_streaming();
    stream->set_direction(pending_stream_direction);
    if (pending_stream_slice_size) stream->set_slice_size(pending_stream_slice_size);
    pending_stream_direction = Streaming::left;
    pending_stream_slice_size = nullptr;
    consumer_stack.push(std::move(stream));
}

void HDL_functions_factory::stop_streaming() {
    if (top_as<streaming_factory>()) {
        expr_factory_.pop_level();
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        sink_value(result);
    }
}

void HDL_functions_factory::set_stream_direction(Streaming::stream_direction d) {
    pending_stream_direction = d;
}

void HDL_functions_factory::set_stream_slice_size(const std::shared_ptr<Expression_base> &s) {
    pending_stream_slice_size = s;
}

void HDL_functions_factory::stop_ternary() {
    expr_factory_.pop_level();
    if (top_as<ternary_factory>()) {
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        sink_value(result);
    }
}

void HDL_functions_factory::stop_expression(bool new_expr) {
    expr_factory_.stop_expression(new_expr);
    // A [...] selection owns a pushed context until stop_bit_selection: its
    // level-0 closings are index fragments, not statement values.
    if (selection_depth > 0) return;
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

void HDL_functions_factory::start_bit_selection() {
    if (!top_as<replication_factory>()) {
        // Open a pushed expression context for the selection, mirroring
        // start_function_call: index operators nest exactly like statement
        // operators, and composite index parts sink back into it.
        expr_factory_.pause();
        expr_factory_.push_level();
        ++selection_depth;
    }
}

void HDL_functions_factory::stop_bit_selection() {
    if (selection_depth == 0) return;
    std::shared_ptr<Expression_base> idx;
    if (auto cur = expr_factory_.get_expression_v2()) {
        idx = Expression_v2::unwrap(std::move(*cur));
    }
    expr_factory_.pop_level();
    --selection_depth;
    if (idx) {
        if (selection_depth == 0 && lvalue_open) {
            pending_lhs_index = idx;
        } else {
            expr_factory_.add_index(idx);
        }
    }
}

