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

void expressions_factory::start_expression() {
    active = true;
    expression_level++;
}

void expressions_factory::stop_expression() {
    expression_level--;
    if (expression_level == 0) {
        active = false;
    }
}

std::optional<Expression> expressions_factory::get_expression() {
    if (current.empty()) return std::nullopt;
    return current;
}

void expressions_factory::pause() {
    paused = true;
}

void expressions_factory::add_component(const Expression_component &ec) {
    if (active && !paused) {
        current.push_back(ec);
    }
    if (paused) paused = false;
}


void expressions_factory::add_index(const Expression &idx) {
    current.add_index(idx);
}
