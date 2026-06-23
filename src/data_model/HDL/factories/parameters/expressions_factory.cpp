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

static Expression_v2::expression_operator map_operator(Token::sv_operators op) {
    switch (op) {
        case Token::logic_neg: return Expression_v2::logic_neg;
        case Token::bitwise_neg: return Expression_v2::bitwise_neg;
        case Token::power: return Expression_v2::power;
        case Token::multiply: return Expression_v2::multiply;
        case Token::divide: return Expression_v2::divide;
        case Token::modulo: return Expression_v2::modulo;
        case Token::add: return Expression_v2::add;
        case Token::subtract: return Expression_v2::subtract;
        case Token::logic_shift_left: return Expression_v2::logic_shift_left;
        case Token::logic_shift_right: return Expression_v2::logic_shift_right;
        case Token::arithmetic_shift_left: return Expression_v2::arithmetic_shift_left;
        case Token::arithmetic_shift_right: return Expression_v2::arithmetic_shift_right;
        case Token::greater: return Expression_v2::greater;
        case Token::greater_equal: return Expression_v2::greater_equal;
        case Token::less: return Expression_v2::less;
        case Token::less_equal: return Expression_v2::less_equal;
        case Token::equal: return Expression_v2::equal;
        case Token::not_equal: return Expression_v2::not_equal;
        case Token::bitwise_and: return Expression_v2::bitwise_and;
        case Token::bitwise_xor: return Expression_v2::bitwise_xor;
        case Token::bitwise_xnor: return Expression_v2::bitwise_xnor;
        case Token::bitwise_or: return Expression_v2::bitwise_or;
        case Token::logical_and: return Expression_v2::logical_and;
        case Token::logical_or: return Expression_v2::logical_or;
    }
    return Expression_v2::none;
}

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

std::optional<Expression_v2> expressions_factory::pop_expression() {
    if (nested_expressions.empty()) return std::nullopt;
    auto inner = std::move(current_v2);
    current_v2 = nested_expressions.top();
    nested_expressions.pop();
    return inner;
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
            auto idx = std::make_shared<Expression_v2>(std::move(bit_select_v2));
            add_index(idx);
        }
        bit_select_v2 = Expression_v2();
    }
}

void expressions_factory::add_component(const Token &ec) {
    if (bit_select_active) {
        if (ec.is_operator()) {
            bit_select_v2.set_operation(map_operator(ec.get_operation()));
        } else if (!bit_select_v2.get_lhs()) {
            bit_select_v2.set_lhs(std::make_shared<Token>(ec));
        } else {
            bit_select_v2.set_rhs(std::make_shared<Token>(ec));
        }
        return;
    }

    if (ec.is_operator()) {

    } else {
        auto tok = std::make_shared<Token>(ec);
        if (current_v2.get_lhs() == nullptr) {
            current_v2.set_lhs(tok);
        } else {
            current_v2.set_rhs(tok);
        }
        if (factory_active && !paused) {
            current.emplace_back(tok);
        }
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
    if (current_v2.get_lhs() == nullptr) {
        current_v2.set_lhs(v);
    } else {
        current_v2.set_rhs(v);
    }
}

bool expressions_factory::active() const {
    return factory_active;
}
