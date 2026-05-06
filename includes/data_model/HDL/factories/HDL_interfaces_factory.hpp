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

#ifndef ANANKE_HDL_INTERFACES_FACTORY_HPP
#define ANANKE_HDL_INTERFACES_FACTORY_HPP


#include "data_model/HDL/HDL_Resource.hpp"
#include "resource_factory_base.hpp"


class HDL_interfaces_factory : protected resources_factory_base<HDL_Resource>{

public:
    void new_interface(const std::string &p, unsigned int line_n);
    HDL_Resource get_interface();
    void set_interface_name(const std::string &n) { set_name(n);};
    bool is_current_valid(){return valid_resource;}
    void add_parameter(const std::shared_ptr<HDL_parameter> &p);

};


#endif //ANANKE_HDL_INTERFACES_FACTORY_HPP
