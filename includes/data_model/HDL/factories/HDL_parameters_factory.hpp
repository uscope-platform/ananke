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

#include <memory>
#include <stack>

#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "resource_factory_base.hpp"
#include "data_model/HDL/factories/parameters/expressions_factory.hpp"
#include "data_model/HDL/factories/parameters/ranges_factory.hpp"
#include "data_model/HDL/factories/parameters/factory_base.hpp"

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

    bool in_packed_context() const {return ctx == param_context::packed_dim; }
    bool is_param_assignment() const {return ctx == param_context::declaration;}
    bool is_param_override() const {return ctx == param_context::override;}
    bool is_component_relevant() const {
        return expr_factory.active() || !consumer_stack.empty() || ctx == param_context::packed_dim || r_factory.active();
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

    void close_packed_dimensions();

private:
    enum class param_context {
        idle,
        declaration,
        override,
        packed_dim
    };

    template<typename T>
    T* top_as() {
        if (consumer_stack.empty()) return nullptr;
        return dynamic_cast<T*>(consumer_stack.top().get());
    }

    expressions_factory expr_factory;
    ranges_factory r_factory;

    std::stack<std::unique_ptr<factory_base>> consumer_stack;

    param_context ctx = param_context::idle;

    bool in_bit_selection = false;
    Expression bit_index;

    std::string current_type;
};


#endif //ANANKE_HDL_PARAMETERS_FACTORY_HPP
