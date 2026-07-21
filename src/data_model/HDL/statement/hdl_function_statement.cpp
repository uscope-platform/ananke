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
#include "data_model/HDL/statement/hdl_assignment_statement.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_function_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_function_statement)

bool hdl_function_statement::is_scalar() const {
    auto assignment = std::dynamic_pointer_cast<hdl_assignment_statement>(body[0]);
    return body.size() == 1
        && assignment != nullptr
        && assignment->get_target() == name
        && assignment->get_index() == nullptr;
}

parameter_deps_t hdl_function_statement::get_dependencies() const {
    parameter_deps_t deps;
    for (const auto& stmt : body) {
        deps.merge(stmt->get_dependencies());
    }
    return deps;
}

std::unique_ptr<hdl_statement_base> hdl_function_statement::clone() const {
    auto result = std::make_unique<hdl_function_statement>();
    result->name = name;
    result->argument_names = argument_names;
    result->return_type_name = return_type_name;
    result->return_unpacked_range_left = return_unpacked_range_left;
    result->return_unpacked_range_right = return_unpacked_range_right;
    result->body = body;
    return result;
}

bool hdl_function_statement::equals(const hdl_statement_base& other) const {
    const auto& rhs = static_cast<const hdl_function_statement&>(other);
    bool retval = name == rhs.name;
    retval &= argument_names == rhs.argument_names;
    retval &= return_type_name == rhs.return_type_name;
    if (body.size() != rhs.body.size()) return false;
    for (size_t i = 0; i < body.size(); i++)
        retval &= *body[i] == *rhs.body[i];
    return retval;
}

std::string hdl_function_statement::print() const {
    return "function " + name + "(...)";
}

void PrintTo(const hdl_function_statement& s, std::ostream* os) {
    *os << "\nfunction: " << s.name << " body: " << s.body.size();
    for (const auto& a : s.body) {
        *os << "\n  " << a->print();
    }
}
