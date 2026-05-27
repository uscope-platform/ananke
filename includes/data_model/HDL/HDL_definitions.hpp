// Copyright 2021 Filippo Savi
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// Unless required by applicable law or agreed to in writing, software


#ifndef ANANKE_HDL_DEFINITIONS_HPP
#define ANANKE_HDL_DEFINITIONS_HPP

enum dependency_class {
    module=0,
    interface=1,
    program=2,
    udp=3,
    memory_init=4,
    package=5
};


enum port_direction_t {
    input_port = 0,
    output_port = 1,
    inout_port = 2,
    interface_port = 3,
    raw_port = 4
};

#endif //ANANKE_HDL_DEFINITIONS_HPP
