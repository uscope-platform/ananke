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
#include "data_model/HDL/HDL_instance.hpp"
#include "data_model/HDL/HDL_loop.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"

class HDL_loops_factory {
public:
    void new_loop();

    void clear();

    void add_instance(HDL_instance &i);
    void add_expression(const Expression &e);
    void set_identifier(HDL_parameter &id) {loop_specs.set_init(id);}
    void start_assignment(const std::string &name);
    std::vector<HDL_instance> get_instances();
    HDL_loop_metadata get_loop_specs() {return loop_specs;}
    bool in_loop(){return active;}

    enum loop_phase_t {init, end, step, body};

    bool in_initialization() const {return active && loop_phase == init;}
    bool in_end_condition() const {return active && loop_phase == end;}
    bool in_step_expression() const {return active && loop_phase == step;}
    bool in_definition() const {return active && (in_step_expression()|| in_initialization()||in_end_condition());}
    void add_component(const Expression_component &c);
    void add_loop_variable(const std::string &p);
    void set_phase(loop_phase_t p);
    void advance_expression();
    void close_expression();
private:
    std::vector<HDL_instance> repeated_instances;
    HDL_loop_metadata loop_specs;

    Expression current_expression;

    loop_phase_t loop_phase = init;
    bool expression_valid = false;

    bool end_cond_valid = false;
    bool active = false;
};


#endif //ANANKE_HDL_LOOPS_FACTORY_H
