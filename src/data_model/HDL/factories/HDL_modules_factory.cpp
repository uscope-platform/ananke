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


void HDL_modules_factory::new_module(const std::string &name, const std::string &path, const dependency_class &type, unsigned int line_n) {
    new_basic_resource(name);
    current_resource.set_path(path);
    current_resource.set_type(type);
    current_resource.set_line_n(line_n);
}

void HDL_modules_factory::add_instance(const HDL_instance &i) {
    current_resource.add_dependency(i);
}


void HDL_modules_factory::add_typedef(const std::string &name, const HDL_type &type) {
    current_resource.add_typedef(name, type);
}

void HDL_modules_factory::add_port(const std::string &p_n, HDL_port p) {
    current_resource.add_ports(p_n, p);
}


HDL_Resource HDL_modules_factory::get_module() {
    auto res = get_resource();
    res.process_calls();
    return res;
}


void HDL_modules_factory::add_parameter(const std::shared_ptr<HDL_parameter> &p) {
    current_resource.add_parameter(p);
}

void HDL_modules_factory::add_function(const HDL_function_def &f) {
    current_resource.add_function(f);
}



