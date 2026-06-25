

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

#ifndef ANANKE_HDL_RANGE_FACTORY_HPP
#define ANANKE_HDL_RANGE_FACTORY_HPP

#include "data_model/HDL/HDL_net.hpp"


class HDL_range_factory {
public:
    void open_range(bool direct);
    void advance_state();
    void add_component(const Token &ec);
    void set_range_type(HDL_range::range_type_t t);
    HDL_range get_range();
    bool is_active() const;

    void set_operation(Expression_v2::expression_operator op);
private:

    enum  {
        idle,
        wait_name,
        accessor,
        range
    } factory_state = idle;

    HDL_range current_range;
    std::vector<HDL_range> ranges_stack;
};



#endif //ANANKE_HDL_RANGE_FACTORY_HPP
