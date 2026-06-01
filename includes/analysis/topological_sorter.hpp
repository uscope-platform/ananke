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

#ifndef ANANKE_TOPOLOGICAL_SORTER_HPP
#define ANANKE_TOPOLOGICAL_SORTER_HPP

#include "data_model/HDL/parameters/Parameters_map.hpp"


class topological_sorter {
public:
    topological_sorter() = default;
    void analyze(const Parameters_map &p,const std::map<qualified_identifier, resolved_parameter> &context);
    std::optional<qualified_identifier> get_next();
    void purge(const qualified_identifier &id);
private:
    struct topo_node {
        std::string name;
        std::set<qualified_identifier> dependencies;
        std::set<qualified_identifier> dependents;
    };
    std::map<qualified_identifier, topo_node> topo_map{};

};



#endif //ANANKE_TOPOLOGICAL_SORTER_HPP
