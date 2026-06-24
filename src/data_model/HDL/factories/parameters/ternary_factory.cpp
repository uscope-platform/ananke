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


#include "data_model/HDL/factories/parameters/ternary_factory.hpp"

void ternary_factory::start_conditional() {
    current = Ternary();
    state = build_phase::condition;
}

void ternary_factory::consume(const std::shared_ptr<Parameter_value_base> &component) {
    if (state == build_phase::condition) {
        current.set_condition(component);
        state = build_phase::true_assignment;
    }else if (state == build_phase::true_assignment) {
        current.set_true_value(component);
        state = build_phase::false_assignment;
    }else if (state == build_phase::false_assignment) {
        current.set_false_value(component);
    }
}

bool ternary_factory::active() const {
    return state != build_phase::inactive;
}

std::shared_ptr<Parameter_value_base> ternary_factory::result() {
    auto ret = std::make_shared<Ternary>(current);
    current = Ternary();
    state = build_phase::inactive;
    return ret;
}
