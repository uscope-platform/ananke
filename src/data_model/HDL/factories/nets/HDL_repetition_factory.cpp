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

void HDL_repetition_factory::add_component(const std::shared_ptr<Parameter_value_base> &tok) {
    if (!current_expression.get_lhs()) {
        current_expression.set_lhs(tok);
    } else if (current_expression.get_lhs() && !current_expression.get_rhs()) {
        current_expression.set_rhs(tok);
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

void HDL_repetition_factory::set_operation(Expression_v2::expression_operator op) {
    current_expression.set_operation(op);
}
