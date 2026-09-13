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


#ifndef ANANKE_HDL_UNION_TYPE_HPP
#define ANANKE_HDL_UNION_TYPE_HPP

#include <string>
#include <sstream>
#include <algorithm>
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"

class HDL_union_type : public hdl_type {
public:
    virtual ~HDL_union_type() = default;
    bool packed = false;
    std::vector<struct_member> members;

    [[nodiscard]] std::optional<resolved_type> evaluate_type(
        const std::map<qualified_identifier, resolved_parameter> &context) override {
        resolved_type result;
        uint64_t max_width = 0;
        for (auto &m : members) {
            if (!m.type) continue;
            struct_member_resolved_type smrt;
            uint64_t member_width = 1;
            if (m.type->is<HDL_struct_type>()) {
                auto s = m.type->as<HDL_struct_type>().evaluate_type(context);
                if (!s) return std::nullopt;
                smrt.packed_sizes = s->packed_sizes;
                smrt.unpacked_sizes = s->unpacked_sizes;
                smrt.unpacked_ascending = s->unpacked_ascending;
                smrt.members = s->struct_sizes;
                for (auto &ps : s->packed_sizes) member_width *= ps;
                for (auto &us : s->unpacked_sizes) member_width *= us;
            } else if (m.type->is<HDL_union_type>()) {
                auto s = m.type->as<HDL_union_type>().evaluate_type(context);
                if (!s) return std::nullopt;
                smrt.packed_sizes = s->packed_sizes;
                smrt.unpacked_sizes = s->unpacked_sizes;
                smrt.unpacked_ascending = s->unpacked_ascending;
                smrt.members = s->struct_sizes;
                for (auto &ps : s->packed_sizes) member_width *= ps;
                for (auto &us : s->unpacked_sizes) member_width *= us;
            } else if (m.type->is<HDL_enum_type>()) {
                auto s = m.type->as<HDL_enum_type>().evaluate_type(context);
                if (!s) return std::nullopt;
                smrt.packed_sizes = s->packed_sizes;
                smrt.unpacked_sizes = s->unpacked_sizes;
                smrt.unpacked_ascending = s->unpacked_ascending;
                for (auto &ps : s->packed_sizes) member_width *= ps;
                for (auto &us : s->unpacked_sizes) member_width *= us;
            } else {
                auto s = m.type->as<HDL_simple_type>().evaluate_type(context);
                if (!s) return std::nullopt;
                smrt.packed_sizes = s->packed_sizes;
                smrt.unpacked_sizes = s->unpacked_sizes;
                smrt.unpacked_ascending = s->unpacked_ascending;
                for (auto &ps : s->packed_sizes) member_width *= ps;
                for (auto &us : s->unpacked_sizes) member_width *= us;
            }
            result.struct_sizes.push_back(smrt);
            max_width = std::max(max_width, member_width);
        }
        result.packed_sizes.push_back(max_width);
        result.packed_ascending.push_back(true);
        result.packed_struct = packed;
        return result;
    }

    [[nodiscard]] bool is_scalar() const override { return packed; }

    parameter_deps_t get_dependencies() override {
        parameter_deps_t result;
        for (auto &m : members) {
            if (m.type) result.merge(m.type->get_dependencies());
        }
        return result;
    }

    [[nodiscard]] std::string to_print() const override {
        std::string result = "union {";
        for (size_t i = 0; i < members.size(); ++i) {
            if (i > 0) result += ", ";
            result += members[i].name + ": ";
            if (members[i].type) result += members[i].type->to_print();
        }
        result += "}";
        return result;
    }

    [[nodiscard]] bool is_equal(const hdl_type &other) const override {
        if (auto *o = dynamic_cast<const HDL_union_type *>(&other)) {
            return members == o->members && packed == o->packed;
        }
        return false;
    }

    friend bool operator==(const HDL_union_type &lhs, const HDL_union_type &rhs) {
        return lhs.members == rhs.members && lhs.packed == rhs.packed;
    }

    template<class Archive>
    void serialize(Archive & ar) {
        ar(members, packed);
    }
};

#endif //ANANKE_HDL_UNION_TYPE_HPP
