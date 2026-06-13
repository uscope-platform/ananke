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
    is_active = false;
    repetition = HDL_replication();
    phase = size;
}

void HDL_repetition_factory::add_component(const std::string &c) {
    if(phase == size) {
        repetition.size.emplace_back(c, Expression_component::get_type(c));
    } else {
        repetition.target.emplace_back(c,Expression_component::get_type(c));
    }
}

void HDL_repetition_factory::add_component(const Expression_component &ec) {
    if(phase == size) {
        repetition.size.push_back(ec);
    } else {
        repetition.target.push_back(ec);
    }
}

void HDL_repetition_factory::advance_phase() {
    phase = target;
}

HDL_replication HDL_repetition_factory::get_repetition() {
    return repetition;
}
