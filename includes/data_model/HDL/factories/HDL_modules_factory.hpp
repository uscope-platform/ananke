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

#ifndef ANANKE_HDL_MODULES_FACTORY_HPP
#define ANANKE_HDL_MODULES_FACTORY_HPP

#include "data_model/HDL/parameters/HDL_parameter.hpp"

#include "data_model/HDL/factories/resource_factory_base.hpp"
#include "data_model/HDL/statement/hdl_resource_statement.hpp"

class HDL_modules_factory : protected resources_factory_base<hdl_resource_statement> {

public:
    void new_module(const std::string &name, const dependency_class &type, unsigned int line_n);
    std::shared_ptr<hdl_resource_statement> get_module();

    void add_statement(std::shared_ptr<hdl_statement_base> s);
    void add_typedef(const std::string &name, const std::shared_ptr<hdl_type> &type);
    bool has_typedef(const std::string &name) {
        return current_resource.get_typedefs().contains(name);
    }
    std::shared_ptr<hdl_type> get_typedef(const std::string &name) {
        return current_resource.get_typedefs().at(name);
    }
    void add_struct_def(const std::string & name, const std::shared_ptr<hdl_type> & hdl_struct);

    void add_parameter(const std::shared_ptr<HDL_parameter> &p);
    void add_function(const hdl_function_statement &f);
    void add_port(const std::string &p_n, HDL_port p);
    bool is_current_valid() {return valid_resource;}

};


#endif //ANANKE_HDL_MODULES_FACTORY_HPP
