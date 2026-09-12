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


#include "data_model/HDL/types/HDL_external_type.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(HDL_external_type)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_type, HDL_external_type)

parameter_deps_t HDL_external_type::get_dependencies() {
    parameter_deps_t deps;
    deps.types.insert(value);
    for (auto &dim : unpacked_dimensions) {
        if (dim.first_bound) deps.merge(dim.first_bound->get_dependencies());
        if (dim.second_bound) deps.merge(dim.second_bound->get_dependencies());
    }
    return deps;
}

bool HDL_external_type::is_scalar() const {
    return false;
}

std::optional<resolved_type> HDL_external_type::evaluate_type(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    return  std::nullopt;
}

std::string HDL_external_type::to_print() const {
    return " " + value.print();
}

bool HDL_external_type::operator==(const HDL_external_type &hdl_external) const {
    if (!(value == hdl_external.value)) return false;
    if (unpacked_dimensions.size() != hdl_external.unpacked_dimensions.size()) return false;
    for (size_t i = 0; i < unpacked_dimensions.size(); i++) {
        if (!(unpacked_dimensions[i] == hdl_external.unpacked_dimensions[i])) return false;
    }
    return true;
}

bool HDL_external_type::is_equal(const hdl_type &other) const {
    if (auto* o = dynamic_cast<const HDL_external_type*>(&other)) {
        return *this == *o;
    }
    return false;
}
