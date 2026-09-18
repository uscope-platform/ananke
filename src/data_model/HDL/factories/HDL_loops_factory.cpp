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
#include "data_model/HDL/parameters/components/token/LoopVar_token.hpp"
#include "data_model/HDL/statement/hdl_while_statement.hpp"
#include "data_model/HDL/statement/hdl_repeat_statement.hpp"
#include "data_model/HDL/statement/hdl_do_while_statement.hpp"

void HDL_loops_factory::new_loop(capture_form_t form) {
    // Push only when genuinely nesting: an unconditional push would leave a
    // stale "parent" behind, making the final pop restore a phantom loop
    // context that swallows the finished top-level loop.
    if (active) push_frame();
    end_cond_valid = false;
    active = true;
    capture = form;
    while_stmt = form == while_form ? std::make_shared<hdl_while_statement>() : nullptr;
    repeat_stmt = form == repeat_form ? std::make_shared<hdl_repeat_statement>() : nullptr;
    do_stmt = form == do_while_form ? std::make_shared<hdl_do_while_statement>() : nullptr;
    _statement = hdl_loop_statement();
    current_expression = Expression_v2();
    if (form == do_while_form) {
        loop_phase = body;
    } else {
        loop_phase = init;
    }
}

void HDL_loops_factory::push_frame() {
    loop_frame frame;
    frame.statement = _statement;
    frame.current_expression = current_expression;
    frame.loop_phase = loop_phase;
    frame.end_cond_valid = end_cond_valid;
    frame.capture = capture;
    frame.while_stmt = while_stmt;
    frame.repeat_stmt = repeat_stmt;
    frame.do_stmt = do_stmt;
    frame_stack.push_back(std::move(frame));
}

void HDL_loops_factory::pop_frame() {
    if (frame_stack.empty()) {
        active = false;
        return;
    }
    const auto &frame = frame_stack.back();
    _statement = frame.statement;
    current_expression = frame.current_expression;
    loop_phase = frame.loop_phase;
    end_cond_valid = frame.end_cond_valid;
    capture = frame.capture;
    while_stmt = frame.while_stmt;
    repeat_stmt = frame.repeat_stmt;
    do_stmt = frame.do_stmt;
    frame_stack.pop_back();
    active = true;
}

void HDL_loops_factory::clear() {
    _statement = hdl_loop_statement();
    while_stmt.reset();
    repeat_stmt.reset();
    do_stmt.reset();
    capture = for_form;
    current_expression = Expression_v2();
    loop_phase = init;
    end_cond_valid = false;
    active = false;
    frame_stack.clear();
}

void HDL_loops_factory::set_operation(const Expression_v2::expression_operator &op) {
    if (loop_phase == body) return; // bodies are owned by the caller (e.g.
                                    // f_factory): the loop keeps header only.
    current_expression.set_operation(op);
}

void HDL_loops_factory:: add_component(const std::shared_ptr<Expression_base> &c) {
    if (loop_phase == body) return; // bodies are owned by the caller: the
                                    // loop keeps header (init/end/step) only.
    if (current_expression.get_lhs() == nullptr) {
        current_expression.set_lhs(c);
    } else {
        current_expression.set_rhs(c);
    }
}

void HDL_loops_factory::add_loop_variable(const std::string &p) {
    HDL_parameter param;
    param.set_name(p);
    _statement.set_init(std::make_shared<HDL_parameter>(param));
}

void HDL_loops_factory::set_phase(loop_phase_t p) {
    loop_phase = p;
    auto loop_var_name = [](const std::shared_ptr<Expression_base> &node) -> std::optional<std::string> {
        if (auto id = std::dynamic_pointer_cast<Identifier_token>(node))
            return id->get_value().get_name();
        if (auto lv = std::dynamic_pointer_cast<LoopVar_token>(node))
            return lv->get_name();
        return std::nullopt;
    };
    if(p==init) {
        current_expression = Expression_v2();
    } else if(p==end) {
        if (!_statement.get_init()) {
            if (auto name = loop_var_name(current_expression.get_lhs())) {
                HDL_parameter param;
                param.set_name(*name);
                _statement.set_init(std::make_shared<HDL_parameter>(param));
            }
        }
        auto init = _statement.get_init();
        if (init) {
            auto copy = std::make_shared<HDL_parameter>(*init);
            auto lhs = current_expression.get_lhs();
            auto rhs = current_expression.get_rhs();
            if (auto name = loop_var_name(lhs)) {
                if (rhs && *name == init->get_name()) {
                    copy->set_raw_value(rhs);
                } else {
                    copy->set_raw_value(Expression_v2::unwrap(current_expression));
                }
            } else {
                copy->set_raw_value(Expression_v2::unwrap(current_expression));
            }
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
    }
}

void HDL_loops_factory::advance_phase() {
    if (loop_phase == init) set_phase(end);
    else if (loop_phase == end) set_phase(step);
    else if (loop_phase == step) set_phase(body);
}

void HDL_loops_factory::begin_loop_body() {
    if (loop_phase == body || end_cond_valid) return;
    auto head = Expression_v2::unwrap(current_expression);
    if (!head) return;
    if (capture == while_form && while_stmt) {
        while_stmt->set_end_condition(head);
    } else if (capture == repeat_form && repeat_stmt) {
        repeat_stmt->set_count(head);
    } else {
        return;
    }
    current_expression = Expression_v2();
    end_cond_valid = true;
    loop_phase = body;
}

void HDL_loops_factory::begin_do_condition() {
    if (capture != do_while_form || end_cond_valid) return;
    end_cond_valid = true;
    loop_phase = end;
    current_expression = Expression_v2();
}

void HDL_loops_factory::finish_do_condition() {
    if (capture != do_while_form || !do_stmt) return;
    if (auto cond = Expression_v2::unwrap(current_expression)) {
        do_stmt->set_end_condition(cond);
    }
    current_expression = Expression_v2();
}

void HDL_loops_factory::add_expression(const Expression_v2 &e) {
    current_expression = e;
}

void HDL_loops_factory::add_body_stmt(const std::shared_ptr<hdl_statement_base> &stmt) {
    if (while_stmt) while_stmt->add_body_stmt(stmt);
    else if (repeat_stmt) repeat_stmt->add_body_stmt(stmt);
    else if (do_stmt) do_stmt->add_body_stmt(stmt);
    else _statement.add_body_stmt(stmt);
}

void HDL_loops_factory::add_statement(const std::shared_ptr<hdl_statement_base> &stmt) {
    add_body_stmt(stmt);
}


