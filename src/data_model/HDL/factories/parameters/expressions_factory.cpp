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

#include "data_model/HDL/factories/parameters/expressions_factory.hpp"

void expressions_factory::push_level() {
    levels_stack.push(expression_level);
    expression_level = 0;
}

void expressions_factory::pop_level() {
    expression_level = levels_stack.top();
    levels_stack.pop();
}

void expressions_factory::start_expression(bool new_expr) {
    if (new_expr) {
        if (factory_active && !skip_nesting) {
            nested_expressions.push(current_v2);
            current_v2 = Expression_v2();
        } else {
            current_v2 = Expression_v2();
        }
    } else if (!factory_active) {
        current_v2 = Expression_v2();
    }
    factory_active = true;
    operation_set = false;
    skip_nesting = false;
    expression_level++;

}

void expressions_factory::stop_expression(bool new_expr) {
    if (new_expr) {
        if (!nested_expressions.empty()) {
            auto tmp_exp = std::make_shared<Expression_v2>(current_v2);
            current_v2 = nested_expressions.top(); nested_expressions.pop();
            if (current_v2.get_lhs()!= nullptr) {
                current_v2.set_rhs(tmp_exp);
            } else {
                current_v2.set_lhs(tmp_exp);
            }
        }
    }
    expression_level--;
    if (expression_level == 0) {
        factory_active = false;
    }
}

std::optional<Expression> expressions_factory::get_expression() {
    if (current.empty()) return std::nullopt;
    return current;
}

std::optional<Expression_v2> expressions_factory::get_expression_v2() {
    return current_v2;
}

void expressions_factory::pause() {
    paused = true;
    skip_nesting = true;
}

void expressions_factory::add_component(const Token &ec) {
    if (ec.is_operator()) {

    }else if (current_v2.get_lhs() == nullptr) {
        current_v2.set_lhs(std::make_shared<Token>(ec));
    } else {
        current_v2.set_rhs(std::make_shared<Token>(ec));
    }

    if (factory_active && !paused) {
        current.push_back(ec);
    }
    if (paused) paused = false;
}

void expressions_factory::set_operation(const Expression_v2::expression_operator &op) {
    current_v2.set_operation(op);
    operation_set = true;
}


void expressions_factory::add_index(const std::shared_ptr<Parameter_value_base>  &idx) {
    current.add_index(idx);
}

void expressions_factory::consume(const std::shared_ptr<Parameter_value_base> &v) {
}

bool expressions_factory::active() const {
    return factory_active;
}
