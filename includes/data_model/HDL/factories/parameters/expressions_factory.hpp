//  Copyright 2026 Filippo Savi
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


#ifndef ANANKE_EXPRESSIONS_FACTORY_HPP
#define ANANKE_EXPRESSIONS_FACTORY_HPP

#include "data_model/HDL/parameters/components/Expression.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"
#include "data_model/HDL/factories/parameters/factory_base.hpp"

class expressions_factory {
public:
    void increase_level() {expression_level++;}
    void decrease_level() {expression_level--;}
    void push_level();
    void pop_level();
    void clear_level() {expression_level = 0;}
    void start_expression(bool new_expr);
    void stop_expression(bool new_expr);
    void clear_expression() {current.clear();}
    std::optional<Expression> get_expression();
    std::optional<Expression_v2> get_expression_v2();
    [[nodiscard]] int get_level() const {return expression_level;}
    void add_component(const Token &ec);
    void set_operation(const Expression_v2::expression_operator &op);
    void pause();
    void add_index(const std::shared_ptr<Parameter_value_base> &idx);

    void consume(const std::shared_ptr<Parameter_value_base>& v);
    bool active() const;

private:
    bool factory_active = false;
    bool paused = false;
    int expression_level = 0;
    Expression current;
    Expression_v2 current_v2 ={};
    std::stack<Expression_v2> nested_expressions;
    std::stack<int> levels_stack;

};


#endif //ANANKE_EXPRESSIONS_FACTORY_HPP