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
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"

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
    } else if (!factory_active || skip_nesting) {
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
            auto tmp_exp = Expression_v2::unwrap(current_v2);
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

std::optional<Expression_v2> expressions_factory::get_expression_v2() {
    return current_v2;
}

void expressions_factory::pause() {
    paused = true;
    skip_nesting = true;
}

void expressions_factory::start_bit_selection() {
    bit_select_active = true;
    bit_select_v2 = Expression_v2();
}

void expressions_factory::stop_bit_selection() {
    if (bit_select_active) {
        bit_select_active = false;
        auto has_op = bit_select_v2.get_operation() != Expression_v2::none;
        auto has_rhs = bit_select_v2.get_rhs() != nullptr;
        if (!has_op && !has_rhs && bit_select_v2.get_lhs()) {
            auto idx = bit_select_v2.get_lhs();
            add_index(idx);
        } else if (has_op || has_rhs) {
            auto idx = Expression_v2::unwrap(std::move(bit_select_v2));
            add_index(idx);
        }
        bit_select_v2 = Expression_v2();
    }
}

void expressions_factory::add_component(const std::shared_ptr<Expression_base> &ec) {
    if (bit_select_active) {
        if (!bit_select_v2.get_lhs()) {
            bit_select_v2.set_lhs(ec);
        } else {
            bit_select_v2.set_rhs(ec);
        }
        return;
    }

    if (current_v2.get_lhs() == nullptr) {
        current_v2.set_lhs(ec);
    } else {
        current_v2.set_rhs(ec);
    }
    if (paused) paused = false;
}

void expressions_factory::set_operation(const Expression_v2::expression_operator &op) {
    if (bit_select_active) {
        if (bit_select_v2.get_lhs() && bit_select_v2.get_rhs()) {
            auto nested = bit_select_v2;
            bit_select_v2 = Expression_v2();
            bit_select_v2.set_lhs(std::make_shared<Expression_v2>(nested));
        }
        bit_select_v2.set_operation(op);
    } else {
        current_v2.set_operation(op);
        operation_set = true;
    }
}


void expressions_factory::add_index(const std::shared_ptr<Expression_base>  &idx) {
    std::shared_ptr<Expression_base> target;
    if (current_v2.get_rhs()) {
        target = current_v2.get_rhs();
    } else if (current_v2.get_lhs()) {
        target = current_v2.get_lhs();
    }
    if (auto tok = std::dynamic_pointer_cast<Identifier_token>(target)) {
        tok->add_array_index(idx);
    }
}

void expressions_factory::consume(const std::shared_ptr<Expression_base> &v) {
    if (current_v2.get_lhs() == nullptr) {
        current_v2.set_lhs(v);
    } else {
        current_v2.set_rhs(v);
    }
}

bool expressions_factory::active() const {
    return factory_active;
}
