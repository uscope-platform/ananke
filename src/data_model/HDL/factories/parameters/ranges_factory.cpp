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

#include "data_model/HDL/factories/parameters/ranges_factory.hpp"

#include "data_model/HDL/parameters/components/Expression_v2.hpp"

void ranges_factory::start() {
    is_active = true;
}

void ranges_factory::open_range() {
    if (!is_active) return;
    status = first_bound;
}

void ranges_factory::advance_range() {
    if (!is_active) return;
    status = second_bound;
}

void ranges_factory::clear() {
    status = first_bound;
    stage = packed;
    packed_dimensions.clear();
    unpacked_dimensions.clear();
    current_dim = dimension_t();
}

std::pair<std::vector<dimension_t>, std::vector<dimension_t>> ranges_factory::get_dimensions() const {
    return {packed_dimensions, unpacked_dimensions};
}

void ranges_factory::consume(const std::shared_ptr<Parameter_value_base> &v) {
}


void ranges_factory::close_range() {
    if (!is_active) return;
    if (stage == packed) {
        current_dim.packed = true;
        packed_dimensions.push_back(current_dim);
    } else {
        unpacked_dimensions.push_back(current_dim);
    }
    current_dim = dimension_t();
}

void ranges_factory::advance_stage() {
    if (!is_active) return;
    stage = unpacked;
}

void ranges_factory::add_expression(const std::shared_ptr<Parameter_value_base> &e) {
    if (!is_active) return;
    if (status == first_bound)
        current_dim.first_bound = e;
    else if (status == second_bound)
        current_dim.second_bound = e;
}


void ranges_factory::stop() {
    is_active = false;
}


