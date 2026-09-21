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
#include "data_model/HDL/types/HDL_union_type.hpp"

#include <unordered_map>

void topological_sorter::analyze(const std::vector<std::shared_ptr<HDL_parameter>> &params, const std::map<qualified_identifier, resolved_parameter> & context) {

    std::unordered_map<std::string, std::shared_ptr<HDL_parameter>> by_name;
    for (auto &parameter : params) {
        if (parameter) by_name[parameter->get_name()] = parameter;
    }

    for (auto &parameter : params) {
        if (!parameter) continue;
        const auto &name = parameter->get_name();
        auto deps_list = parameter->get_dependencies();
        if (!topo_map.contains(qualified_identifier(name))) topo_map[qualified_identifier(name)] = {};
        if (parameter->is_type_param) {
            // Dtype parameters solve unconditionally (ctx=0, their value
            // expression is never evaluated): a same-name override (.T(T))
            // is a type-position reference, not a value dependency on
            // itself. Recording it self-loops the sort into a false
            // "circular dependency". Dependents' edges onto dtypes are kept.
            continue;
        }
        for (auto &dep:deps_list.data) {

            if (!context.contains(dep)) {
                qualified_identifier effective_dep = dep;
                for (auto &inst : dep.get_instance()) {
                    if (by_name.contains(inst)) {
                        auto sp = by_name.at(inst);
                        if (sp && sp->get_type() && (sp->get_type()->is<HDL_struct_type>() || sp->get_type()->is<HDL_union_type>())) {
                            effective_dep = qualified_identifier(inst);
                        }
                        break;
                    }
                }
                if (!by_name.contains(effective_dep.get_name())) continue;
                topo_map[effective_dep].dependents.insert(qualified_identifier(name));
                topo_map[qualified_identifier(name)].dependencies.insert(effective_dep);
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

bool topological_sorter::empty() const {
    return topo_map.empty();
}

std::vector<qualified_identifier> topological_sorter::get_remaining_nodes() const {
    std::vector<qualified_identifier> remaining;
    for (const auto &[node_name, node] : topo_map) {
        remaining.push_back(node_name);
    }
    return remaining;
}
