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
    if (state == build_phase::inactive) {
        state = build_phase::size;
    } else {
        replication_stack.emplace(new_replication, is_assignment);
    }
    new_replication = Replication();
    is_assignment = is_ass;
}

void replication_factory::add_expression(const std::shared_ptr<Expression> &expr) {
    if (state == build_phase::size) {
        state = build_phase::item;
        new_replication.set_size(expr);
    }
    if (state== build_phase::item) {
        new_replication.set_item(expr);
    }
}

std::shared_ptr<Replication> replication_factory::finish() {
    auto ret_val = std::make_shared<Replication>(new_replication);
    if (replication_stack.empty()) {
        state = build_phase::inactive;
    } else {
        auto r = replication_stack.top();
        new_replication = r.first;
        is_assignment = r.second;
        replication_stack.pop();
    }
    return ret_val;
}

bool replication_factory::is_assignment_context() const {
    return is_assignment;
}

void replication_factory::consume(const std::shared_ptr<Parameter_value_base> &v) {
}

bool replication_factory::active() const {
    return state != build_phase::inactive;
}
