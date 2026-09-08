//  Copyright 2025 Filippo Savi
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

#include "data_model/HDL/factories/HDL_modules_factory.hpp"

#include "data_model/HDL/statement/hdl_resource_statement.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"


void HDL_modules_factory::new_module(const std::string &name, const dependency_class &type, unsigned int line_n) {
    new_basic_resource(name);
    current_resource.set_type(type);
    current_resource.set_line_n(line_n);
    function_return_types.clear();
}


void HDL_modules_factory:: add_statement(std::shared_ptr<hdl_statement_base> s) {
    current_resource.add_statement(s);
}

void HDL_modules_factory::add_typedef(const std::string &name, const std::shared_ptr<hdl_type> &type) {
    current_resource.add_typedef(name, type);
}

void HDL_modules_factory::add_struct_def(const std::string &name, const std::shared_ptr<hdl_type> &hdl_struct) {
    current_resource.add_typedef(name, hdl_struct);
}

void HDL_modules_factory::add_port(const std::string &p_n, HDL_port p) {
    current_resource.add_ports(p_n, p);
}


std::shared_ptr<hdl_resource_statement> HDL_modules_factory::get_module() {
    auto res = get_resource();
    auto typedefs = res.get_typedefs();
    for (auto &stmt : res.get_statements()) {
        auto function = std::dynamic_pointer_cast<hdl_function_statement>(stmt);
        if (!function) continue;
        auto it = function_return_types.find(function->get_name());
        if (it == function_return_types.end() || it->second.empty()) continue;
        auto type_it = typedefs.find(it->second);
        if (type_it != typedefs.end()) {
            function->set_return_type(type_it->second);
            if (type_it->second->is<HDL_simple_type>()) {
                auto& simple = type_it->second->as<HDL_simple_type>();
                auto udims = simple.get_unpacked_dimensions();
                if (!udims.empty()) {
                    function->set_return_unpacked_bounds(udims[0].first_bound, udims[0].second_bound);
                }
            }
        } else if (auto sep = it->second.find("::"); sep != std::string::npos) {
            // Package-qualified return type from another package: keep it as
            // an external reference instead of dropping it, so later passes
            // (typedef propagation, struct-return packing) can resolve it.
            qualified_identifier ext_qi(it->second.substr(sep + 2));
            ext_qi.set_package_prefix({it->second.substr(0, sep)});
            function->set_return_type(std::make_shared<HDL_external_type>(ext_qi));
        }
    }
    function_return_types.clear();
    return std::make_shared<hdl_resource_statement>(res);
}


void HDL_modules_factory::add_parameter(const std::shared_ptr<HDL_parameter> &p) {
    current_resource.add_parameter(p);
}

void HDL_modules_factory::add_function(const hdl_function_statement &f) {
    current_resource.add_function(f);
}

void HDL_modules_factory::add_function(const hdl_function_statement &f, const std::string &return_type_name) {
    current_resource.add_function(f);
    if (!return_type_name.empty())
        function_return_types[f.get_name()] = return_type_name;
}



