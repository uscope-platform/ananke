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


#include "data_model/HDL/factories/nets/HDL_repetition_factory.hpp"
#include "data_model/HDL/factories/parameters/expressions_factory.hpp"

void HDL_repetition_factory::stop_repetition() {
    if (current_expression.get_lhs()) {
        if (phase == size_phase) repetition.size = current_expression;
        else repetition.target = current_expression;
    }
    current_expression = Expression_v2();
    is_active = false;
    repetition = HDL_replication();
    phase = size_phase;
}

void HDL_repetition_factory::add_component(const std::string &c) {
    Token tok(c, Token::get_type(c));
    add_to_current(tok);
}

void HDL_repetition_factory::add_component(const Token &ec) {
    add_to_current(ec);
}

void HDL_repetition_factory::add_to_current(const Token &tok) {
    if (tok.is_operator() && current_expression.get_lhs() && current_expression.get_operation() == Expression_v2::none) {
        current_expression.set_operation(expressions_factory::map_operator(tok.get_operation()));
    } else if (!current_expression.get_lhs()) {
        current_expression.set_lhs(std::make_shared<Token>(tok));
    } else if (current_expression.get_lhs() && !current_expression.get_rhs()) {
        current_expression.set_rhs(std::make_shared<Token>(tok));
    }
}

void HDL_repetition_factory::advance_phase() {
    if (current_expression.get_lhs()) {
        if (phase == size_phase) repetition.size = current_expression;
    }
    current_expression = Expression_v2();
    phase = target_phase;
}

HDL_replication HDL_repetition_factory::get_repetition() {
    if (current_expression.get_lhs()) {
        if (phase == size_phase) repetition.size = current_expression;
        else repetition.target = current_expression;
    }
    current_expression = Expression_v2();
    return repetition;
}
