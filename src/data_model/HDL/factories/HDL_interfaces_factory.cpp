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

#include "data_model/HDL/factories/HDL_interfaces_factory.hpp"


void HDL_interfaces_factory::new_interface(const std::string &name, const std::string &path, unsigned int line_n) {
    new_basic_resource(name);
    current_resource.set_path(path);
    current_resource.set_line_n(line_n);
    current_resource.set_type(interface);
}

HDL_Resource HDL_interfaces_factory::get_interface() {
    return get_resource();
}

void HDL_interfaces_factory::add_parameter(const std::shared_ptr<HDL_parameter> &p) {
    current_resource.add_parameter(p);
}

