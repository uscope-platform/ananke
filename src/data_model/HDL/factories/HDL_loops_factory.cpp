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

void HDL_loops_factory::new_loop() {
    repeated_instances.clear();
    end_cond_valid = false;
    active = true;
}

void HDL_loops_factory::clear() {
    repeated_instances.clear();
    loop_specs = HDL_loop_metadata();
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
        loop_specs.add_assignment(assignment(name, {}, {}));
    }
}

std::vector<HDL_instance> HDL_loops_factory::get_instances() {
    active = false;
    return repeated_instances;
}

void HDL_loops_factory:: add_component(const Token &c) {
    if (loop_phase == body) {
        if (in_body_bit_selection) {
            body_expr_factory.add_component(c);
        } else {
            body_expr_factory.add_component(c);
        }
    } else {
        auto tok = std::make_shared<Token>(c);
        if (current_expression.get_lhs() == nullptr) {
            current_expression.set_lhs(tok);
        } else {
            current_expression.set_rhs(tok);
        }
    }
}

void HDL_loops_factory::add_loop_variable(const std::string &p) {
    HDL_parameter param;
    param.set_name(p);
    loop_specs.set_init(param);
}

void HDL_loops_factory::set_phase(loop_phase_t p) {
    loop_phase = p;
    if(p==init) {
        current_expression = Expression_v2();
    } else if(p==end) {
        auto init = loop_specs.get_init();
        init.set_raw_value(Expression_v2::unwrap(current_expression));
        loop_specs.set_init(init);
        current_expression = Expression_v2();
    } else if(p==step) {
        if(!end_cond_valid) {
            loop_specs.set_end_c(current_expression);
        }
        current_expression = Expression_v2();
    } else if(p==body) {
        loop_specs.set_iter(current_expression);
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
        auto assignments = loop_specs.get_assignments();
        auto expr = body_expr_factory.get_expression_v2();
        if (expr.has_value()) {
            auto raw = Expression_v2::unwrap(std::move(*expr));
            if (raw && raw->is<Token>()) {
                auto &tok = raw->as<Token>();
                if (tok.is_subscripted()) {
                    auto indices = tok.get_array_index();
                    if (indices.size() == 1) {
                        assignments.back().set_index(indices[0]);
                    }
                } else {
                    assignments.back().set_index(raw);
                }
            } else if (raw && raw->is<Expression_v2>()) {
                assignments.back().set_index(raw);
            }
        }
        loop_specs.set_assignments(assignments);
        body_expr_factory.clear_expression();
    }
}

void HDL_loops_factory::close_expression() {
    if(expression_valid) {
        auto assignments = loop_specs.get_assignments();
        auto expr = body_expr_factory.get_expression_v2();
        if (expr.has_value()) {
            if (expr->get_operation() != Expression_v2::none) {
                assignments.back().set_value(Expression_v2::unwrap(std::move(*expr)));
            } else if (auto lhs = expr->get_lhs(); lhs && lhs->is<Token>()) {
                assignments.back().set_value(lhs);
            } else if (auto lhs = expr->get_lhs()) {
                assignments.back().set_value(lhs);
            }
        }
        loop_specs.set_assignments(assignments);
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

void HDL_loops_factory::add_instance(HDL_instance &i) {
    i.add_loop(loop_specs);
    repeated_instances.push_back(i);
}


