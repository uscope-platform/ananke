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
#include <sstream>
#include "data_model/HDL/types/HDL_simple_type.hpp"

struct struct_member {
    std::string name;
    std::shared_ptr<hdl_type> type;

    friend bool operator==(const struct_member &lhs, const struct_member &rhs) {
        bool ret = true;
        if (lhs.type && rhs.type) {
            ret &= lhs.type->is_equal(*rhs.type);
        } else {
            ret &= lhs.type == rhs.type;
        }
        ret &= lhs.name == rhs.name;
        return ret;
    }

    [[nodiscard]] std::string to_print() const{
        std::stringstream ss;
        PrintTo(*this, &ss);
        return ss.str();
    }

    friend void PrintTo(const struct_member& m, std::ostream* os) {
        *os << "struct_member { name: " << m.name
            << ", type: {";
        if (m.type) {
            *os << m.type->to_print();
        }
        *os << " } }";
    }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, type);
    }
};

class HDL_struct_type : public hdl_type{
public:
    virtual ~HDL_struct_type() = default;
    bool packed = false;
    std::vector<struct_member> member;


    std::optional<resolved_type> evaluate_type(const std::map<qualified_identifier, resolved_parameter> &context) override;

    [[nodiscard]] bool is_scalar()const override {return packed;}
    parameter_deps_t get_dependencies() override;

    [[nodiscard]] std::string to_print() const override;

    [[nodiscard]] bool is_equal(const hdl_type &other) const override {
        if (auto* o = dynamic_cast<const HDL_struct_type*>(&other)) {
            return *this == *o;
        }
        return false;
    }

    friend bool operator==(const HDL_struct_type &lhs, const HDL_struct_type &rhs)  {
        bool ret = true;
        ret &= lhs.packed == rhs.packed;
        ret &= lhs.member == rhs.member;
        return ret;
    }

    friend void PrintTo(const HDL_struct_type& s, std::ostream* os) {
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
