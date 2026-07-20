//  Copyright 2023 Filippo Savi
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

#include "data_model/HDL/hdl_ast_node.hpp"
#include "data_model/HDL/statement/hdl_instance_statement.hpp"

#include <sstream>

hdl_ast_node::hdl_ast_node(const hdl_instance_statement &stmt) {
    name = stmt.get_name();
    type = stmt.get_type();
    dep_class = stmt.get_dependency_class();
    wildcard_assignment = stmt.get_wildcard();
    groups = stmt.get_channel_groups();
    array_quantifier = stmt.get_array_quantifier();

    for (const auto& [key, param] : stmt.get_parameters()) {
        parameters.insert(param);
    }
    for (const auto& [port_name, nets] : stmt.get_ports()) {
        ports_map[port_name] = nets;
    }
}

hdl_ast_node::hdl_ast_node(const hdl_ast_node &c) {
    name = c.name;
    type = c.type;
    dep_class = c.dep_class;
    parameters = c.parameters;
    ports_map = c.ports_map;
    wildcard_assignment = c.wildcard_assignment;
    loop_specs = c.loop_specs;
    groups = c.groups;
    array_quantifier = c.array_quantifier;

    parent = c.parent;
    child_instances = c.child_instances;
    package_dependencies = c.package_dependencies;
    data_dependencies = c.data_dependencies;
    leaf_module_top = c.leaf_module_top;
    bus_address = c.bus_address;
    processors = c.processors;
    proxy_specs = c.proxy_specs;
    proxy_ast = c.proxy_ast;
    array_index = c.array_index;
}

void hdl_ast_node::add_port_connection(const std::string& port_name, std::vector<HDL_net> value) {
    ports_map[port_name] = std::move(value);
}

void hdl_ast_node::add_parameter(const std::shared_ptr<HDL_parameter> &p) {
    parameters.insert(p);
}

void hdl_ast_node::add_parameters(Parameters_map &p) {
    for (const auto& [key, param] : p) {
        parameters.insert(param);
    }
}

void hdl_ast_node::set_parameters(Parameters_map &p) {
    parameters = std::move(p);
}

Parameters_map hdl_ast_node::get_parameters() {
    return parameters;
}

bool operator==(const hdl_ast_node &lhs, const hdl_ast_node &rhs) {
    bool ret = true;

    ret &= lhs.name == rhs.name;
    ret &= lhs.type == rhs.type;
    ret &= lhs.dep_class == rhs.dep_class;
    ret &= lhs.ports_map == rhs.ports_map;
    ret &= lhs.parameters == rhs.parameters;
    ret &= lhs.loop_specs == rhs.loop_specs;
    ret &= lhs.wildcard_assignment == rhs.wildcard_assignment;
    ret &= lhs.groups == rhs.groups;

    if(lhs.array_quantifier != nullptr && rhs.array_quantifier != nullptr) {
        ret &= *lhs.array_quantifier == *rhs.array_quantifier;
    } else if(lhs.array_quantifier == nullptr && rhs.array_quantifier == nullptr) {
        ret &= true;
    } else {
        ret = false;
    }

    ret &= lhs.child_instances == rhs.child_instances;
    ret &= lhs.bus_address == rhs.bus_address;
    ret &= lhs.data_dependencies == rhs.data_dependencies;
    ret &= lhs.package_dependencies == rhs.package_dependencies;
    ret &= lhs.leaf_module_top == rhs.leaf_module_top;
    ret &= lhs.processors == rhs.processors;
    ret &= lhs.proxy_specs == rhs.proxy_specs;
    ret &= lhs.array_index == rhs.array_index;
    return ret;
}

nlohmann::json hdl_ast_node::dump() {
    nlohmann::json ret;
    ret["instance_name"] = name;
    ret["instance_type"] = type;
    ret["wildcard_assignment"] = wildcard_assignment;

    std::unordered_map<std::string, std::vector<std::string>> port_map_dump = {};
    for(auto &[pname, nets] : ports_map) {
        for(auto &n : nets) {
            port_map_dump[pname].push_back(n.get_full_name());
        }
    }
    ret["ports_map"] = port_map_dump;

    std::map<std::string, nlohmann::json> params_vect;
    for(auto &[n, param] : parameters) {
        params_vect.insert({n, param->dump()});
    }
    ret["parameters"] = params_vect;

    std::vector<nlohmann::json> children;
    children.reserve(child_instances.size());
    for(auto &child:child_instances) {
        children.push_back(child->dump());
    }
    ret["children"] = children;

    if(!bus_address.empty()) ret["address"] = bus_address;
    if(!leaf_module_top.empty()) ret["leaf_module_top"] = leaf_module_top;
    if(!leaf_module_prefix.empty()) ret["leaf_module_prefix"] = leaf_module_prefix;
    if(!proxy_specs.interface.empty() || !proxy_specs.module.empty()) {
        ret["proxy"] = nlohmann::json{
            {"module", proxy_specs.module},
            {"interface", proxy_specs.interface}
        };
    }
    return ret;
}

std::string hdl_ast_node::dump_structure() {
    std::ostringstream oss;
    oss << name << ":" << type << "\n";
    for (const auto &dep: get_dependencies()) {
        oss << dump_structure(dep, "    ");
    }
    return oss.str();
}

std::string hdl_ast_node::dump_structure(const std::shared_ptr<hdl_ast_node> &ast, const std::string &prefix) {
    std::ostringstream oss;
    oss << prefix << ast->get_name() << ":" << ast->get_type() << "\n";
    for (const auto &dep: ast->get_dependencies()) {
        oss << prefix + dump_structure(dep, "    ");
    }
    return oss.str();
}
