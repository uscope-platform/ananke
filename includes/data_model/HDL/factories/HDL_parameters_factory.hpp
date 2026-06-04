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

#ifndef ANANKE_HDL_PARAMETERS_FACTORY_HPP
#define ANANKE_HDL_PARAMETERS_FACTORY_HPP

#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "resource_factory_base.hpp"
#include "data_model/HDL/factories/parameters/replication_factory.hpp"
#include "data_model/HDL/factories/parameters/concatenation_factory.hpp"
#include "data_model/HDL/factories/parameters/function_calls_factory.hpp"
#include "data_model/HDL/factories/parameters/expressions_factory.hpp"
#include "data_model/HDL/factories/parameters/ranges_factory.hpp"
#include "data_model/HDL/factories/parameters/ternary_factory.hpp"
#include "parameters/cast_factory.hpp"


class  HDL_parameters_factory : protected resources_factory_base<HDL_parameter> {

public:
    void new_parameter(const std::string &name);

    std::shared_ptr<HDL_parameter> get_parameter();

    void set_type(const std::string &type);

    void add_component(const Expression_component &c){add_component(c, false);}
    void add_component(const Expression_component &c, bool is_call_argument);


    void start_initialization_list();
    void stop_initialization_list(bool default_assignment);

    void start_bit_selection();
    void stop_bit_selection();

    void start_unpacked_dimension_declaration();

    void close_array_index();

    void start_param_assignment();
    void stop_param_assignment();

    void start_replication();
    void stop_replication();

    void start_replication_assignment();
    void stop_replication_assignment();
    void start_expression_new();
    void stop_expression_new();

    void start_packed_assignment();
    void stop_packed_assignment();

    void start_concatenation();
    void stop_concatenation();

    void start_cast(bool expression_size);
    void set_cast_type(const std::string &t);
    void stop_cast();
    void advance_cast();

    void start_function_assignment(const std::string &f_name);
    void stop_function_assignment();

    void start_function_call(const std::string &f_name);
    void stop_function_call();

    bool in_packed_context() const {return in_packed_assignment && !paused; }
    bool is_param_assignment() const {return in_param_assignment && !paused;}
    bool is_param_override() const {return in_param_override && !paused;}
    bool is_component_relevant() const {
        if (paused) return false;
        return expr_factory.active() ||
            c_factory.active() || in_packed_assignment || calls_factory.active() || concat_factory.active() ||
            t_factory.active() || r_factory.active();
    }

    void advance_range();

    void start_instance_parameter_assignment(const std::string& parameter_name);

    void clear_expression();

    void start_ternary_operator();
    void stop_ternary();

    void start_param_override();
    void stop_param_override();

    void start_range();
    void stop_range();

    void open_range();
    void close_range();

    void start_type_declaration();
    HDL_type stop_type_declaration(const std::string &name);
    void close_packed_dimensions();

private:
    cast_factory c_factory;
    replication_factory repl_factory;
    concatenation_factory concat_factory;
    function_calls_factory calls_factory;
    ranges_factory r_factory;

    expressions_factory expr_factory;
    ternary_factory t_factory;

    bool in_param_override = false;
    bool in_param_assignment = false;
    bool in_packed_assignment = false;
    bool paused = false;
    bool in_bit_selection = false;
    Expression bit_index;

    std::string current_type;

    std::map<std::string, HDL_type> typedefs;
};


#endif //ANANKE_HDL_PARAMETERS_FACTORY_HPP
