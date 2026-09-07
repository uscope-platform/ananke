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

#include "data_model/HDL/statement/hdl_parameter_override_statement.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_parameter_override_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_parameter_override_statement)

parameter_deps_t hdl_parameter_override_statement::get_dependencies() const {
    parameter_deps_t deps;
    if (value) deps.merge(value->get_dependencies());
    return deps;
}

bool hdl_parameter_override_statement::equals(const hdl_statement_base &other) const {
    const auto& rhs = static_cast<const hdl_parameter_override_statement&>(other);
    return instance_path == rhs.instance_path
        && parameter_name == rhs.parameter_name
        && ((value && rhs.value && *value == *rhs.value) || (!value && !rhs.value));
}

std::string hdl_parameter_override_statement::print() const {
    std::string path;
    for (const auto& seg : instance_path) {
        path += seg + ".";
    }
    path += parameter_name;
    std::ostringstream oss;
    oss << "defparam " << path << " = ";
    if (value) oss << value->print();
    return oss.str();
}
