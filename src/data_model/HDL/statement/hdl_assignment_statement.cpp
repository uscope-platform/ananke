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

#include "data_model/HDL/statement/hdl_assignment_statement.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

#include <sstream>

CEREAL_REGISTER_TYPE(hdl_assignment_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_assignment_statement)

parameter_deps_t hdl_assignment_statement::get_dependencies() const {
    parameter_deps_t deps = lvalue.get_dependencies();
    if (value) deps.merge(value->get_dependencies());
    return deps;
}

void hdl_assignment_statement::propagate_function(const hdl_function_def_ptr &def) {
    if (value) value->propagate_function(def);
    lvalue.propagate_function(def);
}

bool hdl_assignment_statement::equals(const hdl_statement_base& other) const {
    const auto& rhs = static_cast<const hdl_assignment_statement&>(other);
    if (!lvalue.equals(rhs.lvalue)) return false;
    return (!value && !rhs.value) || (value && rhs.value && *value == *rhs.value);
}

std::string hdl_assignment_statement::print() const {
    std::ostringstream oss;
    oss << lvalue.print() << " = ";
    if (value) oss << value->print();
    return oss.str();
}
