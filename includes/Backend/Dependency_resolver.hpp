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

#ifndef ANANKE_DEPENDENCY_RESOLVER_HPP
#define ANANKE_DEPENDENCY_RESOLVER_HPP

#include <string>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <utility>

#include "data_model/HDL/hdl_ast_node.hpp"
#include "data_model/data_store.hpp"

class Dependency_resolver_v2 {
public:
    Dependency_resolver_v2(const std::vector<std::shared_ptr<hdl_ast_node>> &i, std::shared_ptr<data_store> store);

    std::set<std::string> get_dependencies();
    std::set<std::string> get_packages();
    std::set<std::string> get_data();

private:
    std::set<std::string> modules;
    std::set<std::string> packages;
    std::set<std::string> data;

    void solve_dep(std::shared_ptr<hdl_ast_node> &i);
    std::vector<std::shared_ptr<hdl_ast_node>> AST;
    std::shared_ptr<data_store> d_store;
};





#endif //ANANKE_DEPENDENCY_RESOLVER_HPP
