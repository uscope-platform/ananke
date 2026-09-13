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

#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_union_type.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(HDL_struct_type)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_type, HDL_struct_type)

std::optional<resolved_type> HDL_struct_type::evaluate_type(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    resolved_type result;
    uint64_t global_size = 0;
    for (auto &m:member) {
        struct_member_resolved_type smrt;
        uint64_t member_width = 1;
        std::optional<resolved_type> s;
        if (m.type->is<HDL_struct_type>()) {
            s = m.type->as<HDL_struct_type>().evaluate_type(context);
        } else if (m.type->is<HDL_union_type>()) {
            s = m.type->as<HDL_union_type>().evaluate_type(context);
        } else if (m.type->is<HDL_enum_type>()) {
            s = m.type->as<HDL_enum_type>().evaluate_type(context);
        } else {
            s = m.type->as<HDL_simple_type>().evaluate_type(context);
        }
        if (!s) return std::nullopt;
        smrt.packed_sizes = s->packed_sizes;
        smrt.unpacked_sizes = s->unpacked_sizes;
        smrt.unpacked_ascending = s->unpacked_ascending;
        smrt.members = s->struct_sizes;
        for (auto &ps : s->packed_sizes)
            member_width *= ps;
        for (auto &us : s->unpacked_sizes)
            member_width *= us;
        result.struct_sizes.push_back(smrt);
        global_size += member_width;
    }
    auto span_size = [](const hdl_integer &f_b, const hdl_integer &s_b) -> hdl_integer {
        auto a = f_b.to_wide();
        auto b = s_b.to_wide();
        auto span = a > b ? (a - b) : (b - a);
        hdl_integer diff;
        diff.set_value(span + 1);
        return diff;
    };
    // Array dimensions applied to the struct reference (e.g. pkg::T [0:N-1]):
    // the element stays packed (struct_sizes/packed_sizes above), these ride
    // along as unpacked/packed container dimensions for array literals.
    for (auto &dim : unpacked_dimensions) {
        auto f_b = dim.first_bound->evaluate(context);
        auto s_b = dim.second_bound->evaluate(context);
        if (!(f_b.has_value() && s_b.has_value()) || !f_b.value().is_integer() || !s_b.value().is_integer()) return std::nullopt;
        auto diff = span_size(f_b.value().get_integer(), s_b.value().get_integer());
        result.unpacked_sizes.push_back(diff.get_value());
        result.unpacked_ascending.push_back(f_b.value().get_integer() < s_b.value().get_integer());
        result.unpacked_left.push_back(f_b.value().get_integer().get_value());
        result.unpacked_right.push_back(f_b.value().get_integer().get_value());
    }
    result.packed_sizes.push_back(global_size);
    result.packed_ascending.push_back(true);
    result.packed_struct = packed;
    return result;
}

parameter_deps_t HDL_struct_type::get_dependencies() {
    parameter_deps_t result;
    for (auto &m: member) {
        if (m.type) {
            result.merge(m.type->get_dependencies());
        }
    }
    for (auto &dim : unpacked_dimensions) {
        if (dim.first_bound) result.merge(dim.first_bound->get_dependencies());
        if (dim.second_bound) result.merge(dim.second_bound->get_dependencies());
    }
    return result;
}

std::string HDL_struct_type::to_print() const {
    std::string result;
    result += " (";
    result += packed ? "packed" : "unpacked";
    result += ") members: [";
    for (size_t i = 0; i < member.size(); ++i) {
        if (i > 0) result += ", ";
        result += member[i].name + ": ";
        if (member[i].type) {
            result += member[i].type->to_print();
        }
    }
    result += "]";
    return result;
}
