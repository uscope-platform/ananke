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


#ifndef ANANKE_CONCATENATION_FACTORY_HPP
#define ANANKE_CONCATENATION_FACTORY_HPP

#include <stack>
#include <vector>
#include "data_model/HDL/parameters/components/Concatenation.hpp"

class concatenation_factory {
public:
    void start_concatenation();
    void stop_concatenation();
    void set_default_init();
    void add_component(const std::shared_ptr<Parameter_value_base> &expr);
    std::shared_ptr<Concatenation> get_concatenation() {return  std::make_shared<Concatenation>(new_concatenation);}
    [[nodiscard]] bool in_concatenation() const { return active;}
    [[nodiscard]] bool in_nested() const {return !concatenations_stack.empty();}
private:
    bool active = false;
    std::stack<Concatenation> concatenations_stack;
    std::vector<Parameter_value_base> concat_components;
    Concatenation new_concatenation;
};


#endif //ANANKE_CONCATENATION_FACTORY_HPP