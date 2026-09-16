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

#ifndef ANANKE_HDL_LOOPS_FACTORY_H
#define ANANKE_HDL_LOOPS_FACTORY_H

#include <vector>
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"

// The loop factory owns the loop itself: header (init/end/iteration) and the
// body statement list. Body statements are built by whoever owns their
// context (e.g. HDL_functions_factory for function bodies, dep/conditional
// factories for generate constructs) and handed in fully built via
// add_body_stmt/add_statement.
class HDL_loops_factory {
public:
    enum capture_form_t {for_form, while_form, repeat_form, do_while_form};
    void new_loop(capture_form_t form = for_form);

    void clear();

    void add_statement(const std::shared_ptr<hdl_statement_base> &stmt);
    void add_expression(const Expression_v2 &e);
    void set_loop_init(const HDL_parameter &id) { _statement.set_init(std::make_shared<HDL_parameter>(id)); }
    std::shared_ptr<hdl_loop_statement> get_loop_statement() {auto ret = std::make_shared<hdl_loop_statement>(_statement); pop_frame(); return ret;};
    std::shared_ptr<hdl_while_statement> get_while_statement() {auto ret = while_stmt; while_stmt.reset(); pop_frame(); return ret;};
    std::shared_ptr<hdl_repeat_statement> get_repeat_statement() {auto ret = repeat_stmt; repeat_stmt.reset(); pop_frame(); return ret;};
    std::shared_ptr<hdl_do_while_statement> get_do_while_statement() {auto ret = do_stmt; do_stmt.reset(); pop_frame(); return ret;};
    bool in_loop(){return active;}

    enum loop_phase_t {init, end, step, body};

    bool in_initialization() const {return active && loop_phase == init;}
    bool in_end_condition() const {return active && loop_phase == end;}
    bool in_step_expression() const {return active && loop_phase == step;}
    bool in_definition() const {return active && (in_step_expression()|| in_initialization()||in_end_condition());}
    bool in_while_capture() const {return active && capture == while_form;}
    bool in_repeat_capture() const {return active && capture == repeat_form;}
    bool in_do_capture() const {return active && capture == do_while_form;}
    void begin_loop_body();
    void begin_do_condition();
    void finish_do_condition();
    void add_component(const std::shared_ptr<Expression_base> &c);
    void add_loop_variable(const std::string &p);
    void set_phase(loop_phase_t p);
    void advance_phase();
    void add_body_stmt(const std::shared_ptr<hdl_statement_base> &stmt);
    void set_operation(const Expression_v2::expression_operator &op);

    bool in_body() const {return active && loop_phase == body;}
private:
    // One frame per open loop: generate loops nest (a pack loop inside a
    // lane loop, a for inside a for in a function body), so a single slot
    // would let the inner header wipe the outer one. new_loop() pushes the
    // in-progress state; the get_*() accessors pop it back.
    struct loop_frame {
        hdl_loop_statement statement;
        Expression_v2 current_expression;
        loop_phase_t loop_phase = init;
        bool end_cond_valid = false;
        capture_form_t capture = for_form;
        std::shared_ptr<hdl_while_statement> while_stmt;
        std::shared_ptr<hdl_repeat_statement> repeat_stmt;
        std::shared_ptr<hdl_do_while_statement> do_stmt;
    };
    std::vector<loop_frame> frame_stack;
    void push_frame();
    void pop_frame();
    hdl_loop_statement _statement;

    Expression_v2 current_expression;

    loop_phase_t loop_phase = init;
    bool end_cond_valid = false;
    bool active = false;
    capture_form_t capture = for_form;
    std::shared_ptr<hdl_while_statement> while_stmt;
    std::shared_ptr<hdl_repeat_statement> repeat_stmt;
    std::shared_ptr<hdl_do_while_statement> do_stmt;
};


#endif //ANANKE_HDL_LOOPS_FACTORY_H
