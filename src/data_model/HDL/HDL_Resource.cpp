// Copyright 2021 University of Nottingham Ningbo China
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "data_model/HDL/HDL_Resource.hpp"


HDL_Resource::HDL_Resource() {
    hdl_type = module;
    name = "";
    path = "";
}

HDL_Resource::HDL_Resource(const HDL_Resource &c) {
    name = c.name;
    path = c.path;
    line_n = c.line_n;
    hdl_type = c.hdl_type;
    dependencies = c.dependencies;
    functions = c.functions;
    default_values = c.default_values;
    parameters_spec = c.parameters_spec;
    doc = c.doc;
    processor_docs = c.processor_docs;
    ports = c.ports;
    typedefs = c.typedefs;
    if_specs = c.if_specs;
}

bool HDL_Resource::is_interface() {
    return hdl_type == interface;
}

void HDL_Resource::process_parameters() {
    default_values = parameter_solver::process_parameters(parameters_spec, "module::" + name, {}, {});
}

void HDL_Resource::process_calls() {
    for(auto &function: functions | std::views::values) {
        for(const auto &param:parameters_spec) {
            param->propagate_function(function);
        }
    }
}


bool HDL_Resource::is_empty() {
    bool ret = true;

    ret &= name.empty();
    ret &= path.empty();
    ret &= hdl_type == module;
    ret &= processor_docs.empty();
    ret &= dependencies.empty();
    ret &= ports.empty();
    ret &= if_specs.empty();
    ret &= parameters_spec.empty();
    ret &= functions.empty();
    ret &= typedefs.empty();
    ret &= default_values.empty();

    return ret;
}

void HDL_Resource::add_dependency(const HDL_instance &dep) {
    dependencies.push_back(dep);
}

std::unordered_map<std::string, HDL_Resource>::mapped_type HDL_Resource::clone() {
    HDL_Resource ret;

    ret.name = name;
    ret.path = path;
    ret.line_n = line_n;
    ret.hdl_type = hdl_type;
    ret.ports = ports;
    ret.if_specs  = if_specs;
    ret.default_values = default_values;
    ret.processor_docs = processor_docs;
    ret.doc = doc;
    ret.typedefs = typedefs;
    ret.parameters_spec  = parameters_spec;

    for(auto &[function_name,  function_def]:functions) {
        ret.functions.insert({function_name, function_def.clone()});
    }

    for(auto &d:dependencies) {
        ret.dependencies.push_back(d.clone());
    }

    return ret;

}

void HDL_Resource::add_dependencies(std::vector<HDL_instance> deps) {
    dependencies.insert(dependencies.begin(), deps.begin(), deps.end());
}



bool operator==(const HDL_Resource &lhs, const HDL_Resource &rhs) {
    bool ret = true;

    ret &= lhs.name == rhs.name;
    ret &= lhs.line_n == rhs.line_n;
    ret &= lhs.path == rhs.path;
    ret &= lhs.hdl_type == rhs.hdl_type;
    ret &= lhs.dependencies == rhs.dependencies;
    ret &= lhs.processor_docs == rhs.processor_docs;
    ret &= lhs.ports == rhs.ports;
    ret &= lhs.if_specs == rhs.if_specs;
    ret &= lhs.default_values == rhs.default_values;
    ret &= lhs.parameters_spec == rhs.parameters_spec;
    ret &= lhs.functions == rhs.functions;
    ret &= lhs.typedefs == rhs.typedefs;

    return ret;
}

bool operator<(const HDL_Resource &lhs, const HDL_Resource &rhs) {
    return lhs.name<rhs.name;
}

void HDL_Resource::add_if_port_specs(const std::string &p_n, const std::string &if_name, const std::string &modport) {
    if_specs[p_n] = {if_name, modport};
}

std::unordered_map<std::string, std::array<std::string, 2>> HDL_Resource::get_if_port_specs() {
    return if_specs;
}

void HDL_Resource::set_parameters(Parameters_map p) {
    parameters_spec = std::move(p);
}


void PrintTo(const HDL_Resource &res, std::ostream *os) {

    std::string result = "\n----------------------------------------------------";
    result += "\nHDL Resource:\n  NAME: " + res.name;
    result += "\n  PATH: " + res.path + "[" + std::to_string(res.line_n) + "]";
    result += "\n  PARAMETERS: \n";
    for(const auto& item:res.parameters_spec){
        result += item->to_string();
    }
    result += "\n----------------------------------------------------";

    *os << result;


}

