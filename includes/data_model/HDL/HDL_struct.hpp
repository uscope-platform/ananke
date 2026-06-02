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

#ifndef ANANKE_HDL_STRUCT_HPP
#define ANANKE_HDL_STRUCT_HPP

#include <string>

#include "data_model/HDL/parameters/common/HDL_type.hpp"

struct struct_member {
    std::string name;
    HDL_type type;

    friend bool operator==(const struct_member &lhs, const struct_member &rhs)  = default;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, type);
    }
};

struct HDL_struct {
public:
    bool packed = false;
    std::vector<struct_member> member;

    friend bool operator==(const HDL_struct &lhs, const HDL_struct &rhs)  = default;


    template<class Archive>
    void serialize( Archive & ar ) {
        ar(member, packed);
    }
};


#endif //ANANKE_HDL_STRUCT_HPP
