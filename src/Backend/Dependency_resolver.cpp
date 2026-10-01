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

#include "Backend/Dependency_resolver.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"



Dependency_resolver_v2::Dependency_resolver_v2(const std::vector<std::shared_ptr<hdl_ast_node>> &i, std::shared_ptr<data_store> store) {
    AST = i;
    d_store = std::move(store);
    for(auto &a:AST){
        solve_dep(a);
        std::string res_path;
        if (d_store->get_elaboratable(a->get_type(), res_path).has_value())
            modules.insert(res_path);
    }

}

std::set<std::string> Dependency_resolver_v2::get_dependencies() {
    return modules;
}

std::set<std::string> Dependency_resolver_v2::get_packages() {
    return packages;
}

std::set<std::string> Dependency_resolver_v2::get_data() {
    return data;
}

void Dependency_resolver_v2::solve_dep(std::shared_ptr<hdl_ast_node> &i) {

    auto type = i->get_type();

    std::string res_path;
    //TODO: this processing step, going back to the data store to get package and memory init dependencies is a bad leaky abstraction
    // needed because the AST cant represent these constructs for now, it will go away once the AST is made properly polymorphic
    auto res = d_store->get_elaboratable(type, res_path);
    if (res.has_value())
        modules.insert(res_path);

    if (res.has_value()) {
        for (auto &stmt : res.value()->get_statements()) {
            auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(stmt);
            if (!inst) continue;
            auto dc = inst->get_dependency_class();
            if (dc == package) {
                std::string pkg_path;
                if (d_store->get_package(inst->get_type(), pkg_path).has_value())
                    packages.insert(pkg_path);
            } else if (dc == memory_init) {
                auto df = d_store->get_data_file(inst->get_type());
                if (df.has_value())
                    data.insert(df.value().get_path());
            }
        }
    }

    for(auto &dep:i->get_dependencies()){
        std::string dep_path;
        if (d_store->get_elaboratable(dep->get_type(), dep_path).has_value())
            modules.insert(dep_path);
        solve_dep(dep);
    }

    for(auto &item:i->get_package_dependencies()){
        std::string pkg_path;
        if (d_store->get_package(item, pkg_path).has_value())
            packages.insert(pkg_path);
    }

    for(auto &item:i->get_data_dependencies()){
        data.insert(item);
    }

}
