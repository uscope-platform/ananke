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

#ifndef ANANKE_HDL_REPETITION_FACTORY_HPP
#define ANANKE_HDL_REPETITION_FACTORY_HPP

#include <string>

#include "data_model/HDL/HDL_net.hpp"

class HDL_repetition_factory {
public:
    void start_repetition() {is_active = true;}
    void stop_repetition();
    bool is_in_repetition() const {return is_active;}

    void add_component(const std::string &c);
    void add_component(const Expression_component &ec);
    void advance_phase();
    HDL_replication get_repetition();
private:
    enum  {
        size,
        target
    } phase = size;
    bool is_active = false;
    HDL_replication repetition;
};



#endif //ANANKE_HDL_REPETITION_FACTORY_HPP
