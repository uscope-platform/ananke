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


#include "analysis/topological_sorter.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"

void topological_sorter::analyze(const Parameters_map &p, const std::map<qualified_identifier, resolved_parameter> & context) {


    for (auto &[name, parameter]:p) {

        auto deps_list = parameter->get_dependencies();
        topo_map.insert({{"", "", name},{}});
        for (auto &dep:deps_list.data) {

            if (!context.contains(dep)) {
                qualified_identifier effective_dep = dep;
                if (!dep.instance.empty() && p.contains(dep.instance)) {
                    auto sp = p.const_get(dep.instance);
                    if (sp && sp->get_type() && sp->get_type()->is<HDL_struct_type>()) {
                        effective_dep = {"", "", dep.instance};
                    }
                }
                topo_map[effective_dep].dependents.insert({"", "", name});
                topo_map[{"", "", name}].dependencies.insert(effective_dep);
            }

        }
    }
}

std::optional<qualified_identifier> topological_sorter::get_next() {
    for (auto  &[node_name, node_content]:topo_map) {
        if (node_content.dependencies.empty()) {
            return node_name;
        }
    }
    return {};
}

void topological_sorter::purge(const qualified_identifier &id) {
    for (auto  &dep:topo_map[id].dependents) {
      topo_map[dep].dependencies.erase(id);
    }
    topo_map.erase(id);
}
