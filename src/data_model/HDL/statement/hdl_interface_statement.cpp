//  Copyright  2026 University of Nottingham
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

#include "data_model/HDL/statement/hdl_interface_statement.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_interface_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_interface_statement)

hdl_interface_statement::hdl_interface_statement() {
    name = "";
}

hdl_interface_statement::hdl_interface_statement(const hdl_interface_statement &c) {
    name = c.name;
    line_n = c.line_n;
    language = c.language;
    typedefs = c.typedefs;
    statements = c.statements;
}

parameter_deps_t hdl_interface_statement::get_dependencies() const {
    parameter_deps_t deps;
    merge_body_deps(deps, statements);
    for (const auto &[_, type] : typedefs)
        deps.merge(type->get_dependencies());
    return deps;
}

bool hdl_interface_statement::equals(const hdl_statement_base& other) const {
    return *this == static_cast<const hdl_interface_statement&>(other);
}

std::string hdl_interface_statement::print() const {
    return name;
}

bool hdl_interface_statement::is_empty() {
    bool ret = true;

    ret &= name.empty();
    ret &= statements.empty();
    ret &= typedefs.empty();

    return ret;
}

std::unordered_map<std::string, hdl_function_statement> hdl_interface_statement::get_functions() {
    std::unordered_map<std::string, hdl_function_statement> result;
    for (auto &stmt : statements) {
        auto f = std::dynamic_pointer_cast<hdl_function_statement>(stmt);
        if (f) result[f->get_name()] = *f;
    }
    return result;
}

std::optional<hdl_function_statement> hdl_interface_statement::get_function(const std::string &fname) {
    if (auto f = get_function_shared(fname)) return *f;
    return std::nullopt;
}

std::shared_ptr<const hdl_function_statement> hdl_interface_statement::get_function_shared(const std::string &fname) const {
    for (const auto &stmt : statements) {
        auto f = std::dynamic_pointer_cast<hdl_function_statement>(stmt);
        if (f && f->get_name() == fname) return f;
    }
    return nullptr;
}

bool operator==(const hdl_interface_statement &lhs, const hdl_interface_statement &rhs) {
    bool ret = true;

    ret &= lhs.name == rhs.name;
    ret &= lhs.line_n == rhs.line_n;
    ret &= lhs.language == rhs.language;
    ret &= lhs.typedefs == rhs.typedefs;
    if (lhs.statements.size() != rhs.statements.size()) return false;
    for (size_t i = 0; i < lhs.statements.size(); i++) {
        const auto &l = lhs.statements[i];
        const auto &r = rhs.statements[i];
        ret &= (!l && !r) || (l && r && *l == *r);
    }

    return ret;
}

bool operator<(const hdl_interface_statement &lhs, const hdl_interface_statement &rhs) {
    return lhs.name < rhs.name;
}

void PrintTo(const hdl_interface_statement &res, std::ostream *os) {
    std::string result = "\n----------------------------------------------------";
    result += "\nHDL Interface:\n  NAME: " + res.name;
    result += "\n  LINE: "  + std::to_string(res.line_n) ;
    result += "\n  PARAMETERS: \n";
    for (const auto& item : res.statements) {
        if (item->is<HDL_parameter>()) result += item->as<HDL_parameter>().to_string() + "\n";
    }
    result += "\n----------------------------------------------------";

    *os << result;
}
