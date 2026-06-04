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

#include "data_model/HDL/factories/parameters/replication_factory.hpp"

void replication_factory::start_replication(bool is_ass) {
    new_replication = Replication();
    is_assignment = is_ass;
    state = build_phase::size;
}


std::shared_ptr<Parameter_value_base> replication_factory::result() {
    auto ret_val = std::make_shared<Replication>(new_replication);
    state = build_phase::inactive;
    return ret_val;
}


bool replication_factory::is_assignment_context() const {
    return is_assignment;
}

void replication_factory::consume(const std::shared_ptr<Parameter_value_base> &expr) {
    if (state == build_phase::size) {
        state = build_phase::item;
        new_replication.set_size(expr);
    }
    if (state== build_phase::item) {
        new_replication.set_item(expr);
    }
}

bool replication_factory::active() const {
    return state != build_phase::inactive;
}
