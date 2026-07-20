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
    hdl_dependency_type = module;
    name = "";
    path = "";
}

HDL_Resource::HDL_Resource(const HDL_Resource &c) {
    name = c.name;
    path = c.path;
    line_n = c.line_n;
    hdl_dependency_type = c.hdl_dependency_type;
    dependencies = c.dependencies;
    functions = c.functions;
    parameters_spec = c.parameters_spec;
    doc = c.doc;
    processor_docs = c.processor_docs;
    port_specs = c.port_specs;
    typedefs = c.typedefs;
    statements = c.statements;
}

bool HDL_Resource::is_interface() {
    return hdl_dependency_type == interface;
}

void HDL_Resource::process_calls() {
    for(auto &function: functions | std::views::values) {
        if (!function.get_return_type_name().empty()) {
            auto it = typedefs.find(function.get_return_type_name());
            if (it != typedefs.end() && it->second->is<HDL_simple_type>()) {
                auto& simple = it->second->as<HDL_simple_type>();
                auto udims = simple.get_unpacked_dimensions();
                if (!udims.empty()) {
                    function.set_return_unpacked_bounds(udims[0].first_bound, udims[0].second_bound);
                }
            }
        }
    }
}


bool HDL_Resource::is_empty() {
    bool ret = true;

    ret &= name.empty();
    ret &= path.empty();
    ret &= hdl_dependency_type == module;
    ret &= processor_docs.empty();
    ret &= dependencies.empty();
    ret &= port_specs.empty();
    ret &= parameters_spec.empty();
    ret &= functions.empty();
    ret &= typedefs.empty();

    return ret;
}

void HDL_Resource::add_dependency(const HDL_instance &dep) {
    dependencies.push_back(dep);
}


void HDL_Resource::add_dependencies(std::vector<HDL_instance> deps) {
    dependencies.insert(dependencies.begin(), deps.begin(), deps.end());
}



bool operator==(const HDL_Resource &lhs, const HDL_Resource &rhs) {
    bool ret = true;

    ret &= lhs.name == rhs.name;
    ret &= lhs.line_n == rhs.line_n;
    ret &= lhs.path == rhs.path;
    ret &= lhs.hdl_dependency_type == rhs.hdl_dependency_type;
    ret &= lhs.dependencies == rhs.dependencies;
    ret &= lhs.processor_docs == rhs.processor_docs;
    ret &= lhs.port_specs == rhs.port_specs;
    ret &= lhs.parameters_spec == rhs.parameters_spec;
    ret &= lhs.functions == rhs.functions;
    ret &= lhs.typedefs == rhs.typedefs;
    if (lhs.statements.size() != rhs.statements.size()) return false;
    for (int i =0; i<lhs.statements.size(); i++) {
        ret &= *lhs.statements[i] == *rhs.statements[i];
    }

    return ret;
}

bool operator<(const HDL_Resource &lhs, const HDL_Resource &rhs) {
    return lhs.name<rhs.name;
}


void HDL_Resource::set_parameters(Parameters_map p) {
    parameters_spec = std::move(p);
}


void PrintTo(const HDL_Resource &res, std::ostream *os) {

    std::string result = "\n----------------------------------------------------";
    result += "\nHDL Resource:\n  NAME: " + res.name;
    result += "\n  PATH: " + res.path + "[" + std::to_string(res.line_n) + "]";
    result += "\n  PARAMETERS: \n";
    for(const auto& [item_name, item]:res.parameters_spec){
        result += item->to_string();
    }
    result += "\n----------------------------------------------------";

    *os << result;


}

