

//  Copyright  2026 University of Nottingham
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

#ifndef ANANKE_HDL_PORT_HPP
#define ANANKE_HDL_PORT_HPP


#include "data_model/HDL/HDL_definitions.hpp"
#include <string>

struct if_port_specs {
    std::string type;
    std::string modport;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(type, modport);
    }

    bool operator==(const if_port_specs&) const = default;
};

struct HDL_port {
    port_direction_t direction;
    if_port_specs if_info;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(direction, if_info);
    }

    bool operator==(const HDL_port&) const = default;
};


#endif //ANANKE_HDL_PORT_HPP
