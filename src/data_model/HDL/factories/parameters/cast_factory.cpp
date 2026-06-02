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


#include "data_model/HDL/factories/parameters/cast_factory.hpp"

void cast_factory::start() {
    if (state != build_phase::inactive) {
        phases_stack.push(state);
        cast_stack.push(new_cast);
    }
    new_cast = Cast();
    state = build_phase::size;
}


void cast_factory::set_type(const std::string &t) {
    new_cast.set_type_cast();
    new_cast.set_target_type(t);
    state = build_phase::content;
}


std::shared_ptr<Cast> cast_factory::get_cast() {
    auto cast = new_cast;
    if (!cast_stack.empty()) {
        new_cast = cast_stack.top();
        state = phases_stack.top();
        cast_stack.pop();
        phases_stack.pop();
    }else {
        state = build_phase::inactive;
    }
    return std::make_shared<Cast>(cast);
}

void cast_factory::consume(const std::shared_ptr<Parameter_value_base> &c) {
    if (state == build_phase::size) {
        if (c->is_expression()) {
            new_cast.set_size(c->as<Expression>());
        } else {
            spdlog::warn("non expression cast size: {}", c->print());
        }
    } else {
        new_cast.set_content(c);
    }
}

bool cast_factory::active() const {
    return state != build_phase::inactive;
}
