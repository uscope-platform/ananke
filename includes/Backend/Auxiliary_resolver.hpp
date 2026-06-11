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

#ifndef ANANKE_AUXILIARY_RESOLVER_HPP
#define ANANKE_AUXILIARY_RESOLVER_HPP

#include <utility>
#include <unordered_set>
#include <vector>

#include "data_model/data_store.hpp"
#include "Backend/backend_types.hpp"


class Auxiliary_resolver {
public:
    explicit Auxiliary_resolver(std::shared_ptr<data_store> store);
    std::vector<script_source> get_tcl_script_paths(const std::vector<Script> &names);
    std::unordered_set<std::string> get_python_script_paths(const std::vector<Script> &names);
    std::vector<Script> get_tcl_objects(const std::vector<Script> &names);
    std::vector<Script> get_python_objects(const std::vector<Script> &names);
    std::unordered_set<std::string> get_constraints(const std::vector<Constraints> &names);
private:
    std::unordered_set<std::string> get_script_paths_by_type(const std::vector<Script> &names, script_type_t type);
    std::vector<Script> get_script_objects_by_type(const std::vector<Script> &names, script_type_t type);
    std::shared_ptr<data_store> d_store;
};


#endif //ANANKE_AUXILIARY_RESOLVER_HPP
