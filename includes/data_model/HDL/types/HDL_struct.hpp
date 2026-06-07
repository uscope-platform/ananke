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

#include "data_model/HDL/types/HDL_type.hpp"

struct struct_member {
    std::string name;
    HDL_type type;

    friend bool operator==(const struct_member &lhs, const struct_member &rhs) {
        bool ret = true;
        ret &= lhs.type == rhs.type;
        ret &= lhs.name == rhs.name;
        return ret;
    }

    friend void PrintTo(const struct_member& m, std::ostream* os) {
        *os << "struct_member { name: " << m.name
            << ", type: { scalar: " << (m.type.is_scalar() ? "true" : "false");
        auto packed = m.type.get_packed_dimensions();
        if (!packed.empty()) {
            *os << ", packed: [";
            for (size_t i = 0; i < packed.size(); ++i) {
                if (i > 0) *os << ", ";
                *os << packed[i];
            }
            *os << "]";
        }
        auto unpacked = m.type.get_unpacked_dimensions();
        if (!unpacked.empty()) {
            *os << ", unpacked: [";
            for (size_t i = 0; i < unpacked.size(); ++i) {
                if (i > 0) *os << ", ";
                *os << unpacked[i];
            }
            *os << "]";
        }
        *os << " } }";
    }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, type);
    }
};

class HDL_struct : public hdl_type_base{
public:
    bool packed = false;
    std::vector<struct_member> member;

    friend bool operator==(const HDL_struct &lhs, const HDL_struct &rhs)  {
        bool ret = true;
        ret &= lhs.packed == rhs.packed;
        ret &= lhs.member == rhs.member;
        return ret;
    }

    friend void PrintTo(const HDL_struct& s, std::ostream* os) {
        *os << "HDL_struct { packed: " << (s.packed ? "true" : "false")
            << ", members: [";
        for (size_t i = 0; i < s.member.size(); ++i) {
            if (i > 0) *os << ", ";
            PrintTo(s.member[i], os);
        }
        *os << "] }";
    }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(member, packed);
    }
};


#endif //ANANKE_HDL_STRUCT_HPP
