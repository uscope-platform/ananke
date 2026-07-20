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

#include "data_model/HDL/factories/HDL_loops_factory.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"

void HDL_loops_factory::new_loop() {
    end_cond_valid = false;
    active = true;
    _statement = hdl_loop_statement();
}

void HDL_loops_factory::clear() {
    _statement = hdl_loop_statement();
    current_expression = Expression_v2();
    body_expr_factory = expressions_factory();
    in_body_bit_selection = false;
    loop_phase = init;
    end_cond_valid = false;
    active = false;
}

void HDL_loops_factory::set_operation(const Expression_v2::expression_operator &op) {
    if (loop_phase == body) {
        body_expr_factory.set_operation(op);
    } else {
        current_expression.set_operation(op);
    }
}

void HDL_loops_factory::start_assignment(const std::string &name) {
    if(loop_phase == body) {
        expression_valid = true;
        body_expr_factory.clear_expression();
        body_target = name;
        body_index = nullptr;
    }
}

void HDL_loops_factory:: add_component(const std::shared_ptr<Expression_base> &c) {
    if (loop_phase == body) {
        if (in_body_bit_selection) {
            body_expr_factory.add_component(c);
        } else {
            body_expr_factory.add_component(c);
        }
    } else {
        if (current_expression.get_lhs() == nullptr) {
            current_expression.set_lhs(c);
        } else {
            current_expression.set_rhs(c);
        }
    }
}

void HDL_loops_factory::add_loop_variable(const std::string &p) {
    HDL_parameter param;
    param.set_name(p);
    _statement.set_init(std::make_shared<HDL_parameter>(param));
}

void HDL_loops_factory::set_phase(loop_phase_t p) {
    loop_phase = p;
    if(p==init) {
        current_expression = Expression_v2();
    } else if(p==end) {
        auto init = _statement.get_init();
        if (init) {
            auto copy = std::make_shared<HDL_parameter>(*init);
            copy->set_raw_value(Expression_v2::unwrap(current_expression));
            _statement.set_init(copy);
        }
        current_expression = Expression_v2();
    } else if(p==step) {
        if(!end_cond_valid) {
            _statement.set_end_condition(std::make_shared<Expression_v2>(current_expression));
        }
        current_expression = Expression_v2();
    } else if(p==body) {
        _statement.set_iteration(std::make_shared<Expression_v2>(current_expression));
        current_expression = Expression_v2();
        body_expr_factory = expressions_factory();
        in_body_bit_selection = false;
    }
}

void HDL_loops_factory::advance_phase() {
    if (loop_phase == init) set_phase(end);
    else if (loop_phase == end) set_phase(step);
    else if (loop_phase == step) set_phase(body);
}

void HDL_loops_factory::advance_expression() {
    if(expression_valid) {
        auto expr = body_expr_factory.get_expression_v2();
        if (expr.has_value()) {
            auto raw = Expression_v2::unwrap(std::move(*expr));
            if (raw && raw->is<Identifier_token>()) {
                auto &tok = raw->as<Identifier_token>();
                if (tok.is_subscripted()) {
                    auto indices = tok.get_array_index();
                    if (indices.size() == 1) body_index = indices[0];
                } else {
                    body_index = raw;
                }
            } else if (raw && raw->is<Expression_v2>()) {
                body_index = raw;
            }
        }
        body_expr_factory.clear_expression();
    }
}

void HDL_loops_factory::close_expression() {
    if(expression_valid) {
        auto expr = body_expr_factory.get_expression_v2();
        std::shared_ptr<Expression_base> val;
        if (expr.has_value()) {
            if (expr->get_operation() != Expression_v2::none) {
                val = Expression_v2::unwrap(std::move(*expr));
            } else if (auto lhs = expr->get_lhs()) {
                val = lhs;
            }
        }

        auto stmt = std::make_shared<hdl_assignment_statement>();
        stmt->set_target(body_target);
        if (body_index) stmt->set_index(body_index);
        if (val) stmt->set_value(val);
        _statement.add_body_stmt(stmt);

        expression_valid = false;
        body_expr_factory.clear_expression();
    }
}

void HDL_loops_factory::start_expression(bool new_expr) {
    if (loop_phase == body && !in_body_bit_selection) {
        body_expr_factory.start_expression(new_expr);
    }
}

void HDL_loops_factory::stop_expression(bool new_expr) {
    if (loop_phase == body && !in_body_bit_selection) {
        body_expr_factory.stop_expression(new_expr);
    }
}

void HDL_loops_factory::start_bit_selection() {
    if (loop_phase == body) {
        in_body_bit_selection = true;
        body_expr_factory.start_bit_selection();
    }
}

void HDL_loops_factory::stop_bit_selection() {
    if (loop_phase == body) {
        in_body_bit_selection = false;
        body_expr_factory.stop_bit_selection();
    }
}

void HDL_loops_factory::add_expression(const Expression_v2 &e) {
    current_expression = e;
}

void HDL_loops_factory::add_statement(const std::shared_ptr<hdl_statement_base> &stmt) {
    _statement.add_body_stmt(stmt);
}


