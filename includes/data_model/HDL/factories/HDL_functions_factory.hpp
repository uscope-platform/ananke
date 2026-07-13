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

#include <memory>
#include <stack>
#include <string>

#include "data_model/HDL/HDL_loop.hpp"
#include "data_model/HDL/parameters/HDL_function_def.hpp"
#include "data_model/HDL/factories/parameters/expressions_factory.hpp"
#include "data_model/HDL/factories/parameters/factory_base.hpp"

class HDL_functions_factory {
public:
    HDL_functions_factory() = default;
    void set_name(const std::string  &s) {
        f.set_name(s);
        phase = arguments;
        active = true;
    }
    void start_assignment(const std::string &n);
    void add_argument(const std::string &a);
    void add_component(const std::shared_ptr<Expression_base> &c);
    void add_value(const std::shared_ptr<Expression_base> &v);
    void close_lvalue();
    void start_body(){phase = body;}
    void close_assignment();
    void add_loop(const HDL_loop_metadata &md){f.add_loop_metadata(md);}
    HDL_function_def get_function();
    void set_return_type_name(const std::string &n) { f.set_return_type_name(n); }
    bool is_active()const{return active;}
    bool is_raw_body()const{return consumer_stack.empty();}

    void start_replication();
    void stop_replication();

    void start_concat();
    void stop_concat();

    void start_cast(bool expression_size);
    void stop_cast();
    void set_cast_type(const std::string &t);
    void advance_cast();

    void start_expression();
    void stop_expression();
    void pause() { paused = true; }
    void resume() { paused = false; }
    bool is_paused() const { return paused; }
    bool is_component_relevant() const;

    void start_bit_selection();
    std::shared_ptr<Expression_base> get_last_value() const { return assignment_value; }

    void stop_bit_selection();

    void set_operation(Expression_v2::expression_operator op);

private:
    template<typename T>
    T* top_as() {
        if (consumer_stack.empty()) return nullptr;
        return dynamic_cast<T*>(consumer_stack.top().get());
    }

    expressions_factory expr_factory_;
    bool paused = false;
    bool active = false;
    bool in_bit_selection = false;
    std::stack<std::unique_ptr<factory_base>> consumer_stack;
    HDL_function_def f;
    std::shared_ptr<Expression_base> bit_index;

    enum{
        arguments,
        body
    }phase;
    bool ignore_assignment = false;
    std::shared_ptr<Expression_base> assignment_value;
    std::string current_assigned_variable;
};



#endif //ANANKE_HDL_FUNCTIONS_FACTORY_HPP
