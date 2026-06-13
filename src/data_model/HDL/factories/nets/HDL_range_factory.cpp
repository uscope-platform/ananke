

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

void HDL_range_factory::add_component(const std::string &c) {
    if(factory_state == wait_name) {
        factory_state = accessor;
    }else if(factory_state == accessor) {
        current_range.accessor.emplace_back(c, Token::get_type(c));
    } else if(factory_state == range) {
        current_range.range.emplace_back(c, Token::get_type(c));
    }
}

void HDL_range_factory::add_component(const Token &ec) {
    if(factory_state == wait_name) {
        factory_state = accessor;
    }else if(factory_state == accessor) {
        current_range.accessor.push_back(ec);
    } else if(factory_state == range) {
        current_range.range.push_back(ec);
    }
}

void HDL_range_factory::set_range_type(HDL_range::range_type_t t) {
    current_range.type = t;
    advance_state();
}

HDL_range HDL_range_factory::get_range() {
    assert(factory_state == range);
    auto ret = current_range;
    current_range.range.clear();
    current_range.accessor.clear();
    current_range.type  = HDL_range::explicit_range;
    factory_state = idle;
    return ret;
}

bool HDL_range_factory::is_active() const {
    return factory_state != idle;
}

