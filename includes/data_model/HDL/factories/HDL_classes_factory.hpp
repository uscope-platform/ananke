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

#ifndef ANANKE_HDL_CLASSES_FACTORY_HPP
#define ANANKE_HDL_CLASSES_FACTORY_HPP


#include "data_model/HDL/parameters/HDL_parameter.hpp"

#include "resource_factory_base.hpp"
#include "data_model/HDL/statement/hdl_class_statement.hpp"

#include <map>
#include <stack>

class HDL_classes_factory : protected resources_factory_base<hdl_class_statement>{

public:
    void new_class(const std::string &name, unsigned int line_n);
    std::shared_ptr<hdl_class_statement> get_class();
    bool is_current_valid(){return valid_resource;}
    void set_base_class(const std::string &base) { current_resource.set_base_class(base); }
    void add_nested_class(const std::shared_ptr<hdl_class_statement> &c) {
        current_resource.add_nested_class(c);
    }
    // Generic entry point for the shared routing paths: properties and
    // methods are sorted into their own containers by dynamic type, anything
    // else has no class-level representation and is ignored.
    void add_statement(std::shared_ptr<hdl_statement_base> s);
    void add_property(const std::shared_ptr<HDL_parameter> &p) { current_resource.add_property(p); }
    void add_typedef(const std::string &name, const std::shared_ptr<hdl_type> &type);
    void add_struct_def(const std::string & name, const std::shared_ptr<hdl_type> & hdl_struct);
    void add_function(const hdl_function_statement &f);
    void add_function(const hdl_function_statement &f, const std::string &return_type_name);

private:
    std::map<std::string, std::string> function_return_types;
    // Saved per nesting level: entering a nested class must not wipe the
    // enclosing class's pending return-type fixups.
    std::stack<std::map<std::string, std::string>> return_type_stack;

};


#endif //ANANKE_HDL_CLASSES_FACTORY_HPP
