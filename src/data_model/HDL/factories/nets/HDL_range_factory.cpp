

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

#include "data_model/HDL/factories/nets/HDL_range_factory.hpp"

void HDL_range_factory::open_range(bool direct) {
    if(direct) {
        factory_state = accessor;
    } else {
        factory_state = wait_name;
    }
}

void HDL_range_factory::advance_state() {
    if(factory_state == accessor) {
        factory_state = range;
    }
}

void HDL_range_factory::add_component(const Token &tok) {
    if(factory_state == wait_name) {
        factory_state = accessor;
        return;
    }

    Expression_v2 *target = nullptr;
    if(factory_state == accessor) {
        target = &current_range.accessor;
    } else if(factory_state == range) {
        target = &current_range.range;
    } else {
        return;
    }

    if (!target->get_lhs()) {
        target->set_lhs(std::make_shared<Token>(tok));
    } else if (target->get_lhs() && !target->get_rhs()) {
        target->set_rhs(std::make_shared<Token>(tok));
    }
}

void HDL_range_factory::set_range_type(HDL_range::range_type_t t) {
    current_range.type = t;
    advance_state();
}

HDL_range HDL_range_factory::get_range() {
    assert(factory_state == range);
    auto ret = current_range;
    current_range.range = Expression_v2();
    current_range.accessor = Expression_v2();
    current_range.type  = HDL_range::explicit_range;
    factory_state = idle;
    return ret;
}

bool HDL_range_factory::is_active() const {
    return factory_state != idle;
}

void HDL_range_factory::set_operation(Expression_v2::expression_operator op) {

    Expression_v2 *target = nullptr;
    if(factory_state == accessor) {
        target = &current_range.accessor;
    } else if(factory_state == range) {
        target = &current_range.range;
    } else {
        return;
    }
    target->set_operation(op);
}

