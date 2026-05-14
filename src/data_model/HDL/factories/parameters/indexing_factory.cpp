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


#include "data_model/HDL/factories/parameters/indexing_factory.hpp"

void indexing_factory::start_index(bool r) {
    if (!quantifier) {
        active = true;
        range = r;
        if(r) {
            first_bound = true;
            dim.emplace_back();
        } else {
            index = Expression();
        }
    }
}

void indexing_factory::stop_index() {
    active = false;
}

void indexing_factory::add_expression(const Expression &e) {
    if (active && range && !quantifier) {
        if (first_bound) {
            dim.back().first_bound = e;
            first_bound = false;
        } else {
            dim.back().second_bound = e;
        }
    }
}


void indexing_factory::set_packed(bool p) {
    dim.back().packed = p;
}

std::vector<dimension_t> indexing_factory::get_dimensions() {
    auto ret = dim;
    dim.clear();
    return ret;
}

void indexing_factory::set_quantifier(bool q) {
    quantifier = q;
}
