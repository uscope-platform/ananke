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


#include "data_model/HDL/factories/parameters/concatenation_factory.hpp"

void concatenation_factory::start_concatenation() {
    if (active) {
        concatenations_stack.push(new_concatenation);
    }
    new_concatenation = Concatenation();
    active = true;
}

void concatenation_factory::stop_concatenation() {
    if (concatenations_stack.empty()) {
        active = false;
    } else {
       auto current = new_concatenation;
        new_concatenation = concatenations_stack.top();
        concatenations_stack.pop();
        new_concatenation.add_component(std::make_shared<Concatenation>(current));
    }
}

void concatenation_factory::set_default_init() {
    new_concatenation.set_default_init();
}

void concatenation_factory::add_component(const std::shared_ptr<Parameter_value_base> &expr) {
    new_concatenation.add_component(expr);
}

