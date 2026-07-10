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

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(HDL_struct_type)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_type, HDL_struct_type)

std::optional<resolved_type> HDL_struct_type::evaluate_type(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    resolved_type result;
    uint64_t global_size = 0;
    for (auto &m:member) {
        if (m.type->is<HDL_struct_type>()) {
            spdlog::warn("Nested structures are not supported yet");
            return std::nullopt;
        }
        auto s = m.type->as<HDL_simple_type>().evaluate_type(context);
        result.struct_sizes.push_back({s->unpacked_sizes, s->packed_sizes});
        uint64_t member_width = 1;
        for (auto &ps : s->packed_sizes)
            member_width *= ps;
        global_size +=member_width;
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
