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

#include "data_model/HDL/statement/hdl_resource_statement.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_resource_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_resource_statement)

hdl_resource_statement::hdl_resource_statement() {
    hdl_dependency_type = module;
    name = "";
}

hdl_resource_statement::hdl_resource_statement(const hdl_resource_statement &c) {
    name = c.name;
    line_n = c.line_n;
    hdl_dependency_type = c.hdl_dependency_type;
    parameters_spec = c.parameters_spec;
    doc = c.doc;
    processor_docs = c.processor_docs;
    port_specs = c.port_specs;
    typedefs = c.typedefs;
    statements = c.statements;
}

parameter_deps_t hdl_resource_statement::get_dependencies() const {
    parameter_deps_t deps;
    for (const auto &stmt : statements)
        deps.merge(stmt->get_dependencies());
    for (const auto &[_, param] : parameters_spec)
        deps.merge(param->get_dependencies());
    for (const auto &[_, type] : typedefs)
        deps.merge(type->get_dependencies());
    return deps;
}

std::unique_ptr<hdl_statement_base> hdl_resource_statement::clone() const {
    return std::make_unique<hdl_resource_statement>(*this);
}

bool hdl_resource_statement::equals(const hdl_statement_base& other) const {
    return *this == static_cast<const hdl_resource_statement&>(other);
}

std::string hdl_resource_statement::print() const {
    return name;
}

bool hdl_resource_statement::is_interface() {
    return hdl_dependency_type == interface;
}

void hdl_resource_statement::process_calls() {
    for (auto &stmt : statements) {
        auto function = std::dynamic_pointer_cast<hdl_function_statement>(stmt);
        if (!function) continue;

        if (!function->get_return_type_name().empty()) {
            auto it = typedefs.find(function->get_return_type_name());
            if (it != typedefs.end() && it->second->is<HDL_simple_type>()) {
                auto& simple = it->second->as<HDL_simple_type>();
                auto udims = simple.get_unpacked_dimensions();
                if (!udims.empty()) {
                    function->set_return_unpacked_bounds(udims[0].first_bound, udims[0].second_bound);
                }
            }
        }
    }
}

bool hdl_resource_statement::is_empty() {
    bool ret = true;

    ret &= name.empty();
    ret &= hdl_dependency_type == module;
    ret &= processor_docs.empty();
    ret &= port_specs.empty();
    ret &= parameters_spec.empty();
    ret &= statements.empty();
    ret &= typedefs.empty();

    return ret;
}

std::unordered_map<std::string, hdl_function_statement> hdl_resource_statement::get_functions() {
    std::unordered_map<std::string, hdl_function_statement> result;
    for (auto &stmt : statements) {
        auto f = std::dynamic_pointer_cast<hdl_function_statement>(stmt);
        if (f) result[f->get_name()] = *f;
    }
    return result;
}

hdl_function_statement hdl_resource_statement::get_function(const std::string &fname) {
    for (auto &stmt : statements) {
        auto f = std::dynamic_pointer_cast<hdl_function_statement>(stmt);
        if (f && f->get_name() == fname) return *f;
    }
    return {};
}

void hdl_resource_statement::set_parameters(Parameters_map p) {
    parameters_spec = std::move(p);
}

bool operator==(const hdl_resource_statement &lhs, const hdl_resource_statement &rhs) {
    bool ret = true;

    ret &= lhs.name == rhs.name;
    ret &= lhs.line_n == rhs.line_n;
    ret &= lhs.hdl_dependency_type == rhs.hdl_dependency_type;
    ret &= lhs.processor_docs == rhs.processor_docs;
    ret &= lhs.port_specs == rhs.port_specs;
    ret &= lhs.parameters_spec == rhs.parameters_spec;
    ret &= lhs.typedefs == rhs.typedefs;
    if (lhs.statements.size() != rhs.statements.size()) return false;
    for (int i = 0; i < lhs.statements.size(); i++) {
        ret &= *lhs.statements[i] == *rhs.statements[i];
    }

    return ret;
}

bool operator<(const hdl_resource_statement &lhs, const hdl_resource_statement &rhs) {
    return lhs.name < rhs.name;
}

void PrintTo(const hdl_resource_statement &res, std::ostream *os) {
    std::string result = "\n----------------------------------------------------";
    result += "\nHDL Resource:\n  NAME: " + res.name;
    result += "\n  LINE: "  + std::to_string(res.line_n) ;
    result += "\n  PARAMETERS: \n";
    for (const auto& [item_name, item] : res.parameters_spec) {
        result += item->to_string() + "\n";
    }
    result += "\n----------------------------------------------------";

    *os << result;
}
