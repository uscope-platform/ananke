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

#include "data_model/HDL/types/HDL_struct.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(HDL_struct)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_type, HDL_struct)

std::set<qualified_identifier> HDL_struct::get_dependencies() {
    std::set<qualified_identifier> result;
    for (auto &m: member) {
        if (m.type) {
            auto deps = m.type->get_dependencies();
            result.insert(deps.begin(), deps.end());
        }
    }
    return result;
}

std::string HDL_struct::to_print() const {
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
