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


#include "data_model/HDL/statement/hdl_function_statement.hpp"

#include "data_model/HDL/parameters/components/Expression_v2.hpp"


bool hdl_function_statement::is_scalar() const {
    return  !loop_metadata.has_value() && assignments.size() == 1;
}

parameter_deps_t hdl_function_statement::get_dependencies() const {
    parameter_deps_t deps;
    for (const auto& a : assignments) {
        if (a.get_name() != name) continue;
        deps.merge(a.get_value()->get_dependencies());
        if (a.get_index()) deps.merge(a.get_index().value()->get_dependencies());
    }
    if (loop_metadata) deps.merge(loop_metadata->get_dependencies());
    return deps;
}

std::unique_ptr<hdl_statement_base> hdl_function_statement::clone() const {
    auto result = std::make_unique<hdl_function_statement>();
    result->name = name;
    result->assignments = assignments;
    result->loop_metadata = loop_metadata;
    result->argument_names = argument_names;
    result->return_type_name = return_type_name;
    result->return_unpacked_range_left = return_unpacked_range_left;
    result->return_unpacked_range_right = return_unpacked_range_right;
    return result;
}

bool hdl_function_statement::equals(const hdl_statement_base& other) const {
    const auto& rhs = static_cast<const hdl_function_statement&>(other);
    bool retval = name == rhs.name;
    retval &= assignments == rhs.assignments;
    retval &= loop_metadata == rhs.loop_metadata;
    retval &= argument_names == rhs.argument_names;
    retval &= return_type_name == rhs.return_type_name;
    return retval;
}

std::string hdl_function_statement::print() const {
    return "function " + name + "(...)";
}
