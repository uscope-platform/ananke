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


#ifndef ANANKE_HDL_FUNCTIONS_FACTORY_HPP
#define ANANKE_HDL_FUNCTIONS_FACTORY_HPP
#include <string>

#include "data_model/HDL/HDL_loop.hpp"
#include "data_model/HDL/parameters/components/Expression_component.hpp"
#include "data_model/HDL/parameters/components/Expression.hpp"
#include "data_model/HDL/parameters/HDL_function_def.hpp"
#include "parameters/concatenation_factory.hpp"
#include "parameters/replication_factory.hpp"

class HDL_functions_factory {
public:
    HDL_functions_factory() = default;
    void set_name(const std::string  &s) {
        f.set_name(s);
        phase = arguments;
        active = true;
    }
    void start_assignment(const std::string &n) {current_assigned_variable = n;}
    void add_argument(const std::string &a);
    void add_component(const Expression_component &c);
    void add_value(const std::shared_ptr<Parameter_value_base> &v);
    void close_lvalue();
    void start_body(){phase = body;}
    void close_assignment();
    void add_loop(const HDL_loop_metadata &md){f.add_loop_metadata(md);}
    HDL_function_def get_function();
    bool is_active()const{return active;}
    bool is_raw_body()const{return !r_factory.active() && !c_factory.active();}

    void start_replication();
    void stop_replication();


    void start_concat();
    void stop_concat();

private:
    bool active = false;
    replication_factory r_factory;
    concatenation_factory c_factory;
    HDL_function_def f;

    enum{
        arguments,
        body
    }phase;
    bool ignore_assignment = false;
    Expression new_expression;
    std::shared_ptr<Parameter_value_base> assignment_value;
    std::string current_assigned_variable;
};



#endif //ANANKE_HDL_FUNCTIONS_FACTORY_HPP
