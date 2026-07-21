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

#include "data_model/HDL/statement/hdl_instance_statement.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_instance_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_instance_statement)

void hdl_instance_statement::add_port_connection(const std::string& port_name, std::vector<HDL_net> value) {
    ports_map[port_name] = std::move(value);
}

parameter_deps_t hdl_instance_statement::get_dependencies() const {
    parameter_deps_t deps;
    for (const auto& [key, param] : parameters) {
        if (param) deps.merge(param->get_dependencies());
    }
    return deps;
}

std::unique_ptr<hdl_statement_base> hdl_instance_statement::clone() const {
    auto c = std::make_unique<hdl_instance_statement>();
    c->name = name;
    c->type = type;
    c->dep_class = dep_class;
    c->parameters = parameters;
    c->ports_map = ports_map;
    c->wildcard_assignment = wildcard_assignment;
    c->groups = groups;
    c->array_quantifier = array_quantifier;
    return c;
}

bool hdl_instance_statement::equals(const hdl_statement_base& other) const {
    const auto& rhs = static_cast<const hdl_instance_statement&>(other);
    bool ret = name == rhs.name;
    ret &= type == rhs.type;
    ret &= dep_class == rhs.dep_class;
    ret &= parameters == rhs.parameters;
    ret &= ports_map == rhs.ports_map;
    ret &= wildcard_assignment == rhs.wildcard_assignment;
    ret &= groups == rhs.groups;
    ret &= (!array_quantifier && !rhs.array_quantifier) ||
           (array_quantifier && rhs.array_quantifier && *array_quantifier == *rhs.array_quantifier);
    return ret;
}

std::string hdl_instance_statement::print() const {
    return type + " " + name;
}

void PrintTo(const hdl_instance_statement& s, std::ostream* os) {
    *os << s.print();
}
