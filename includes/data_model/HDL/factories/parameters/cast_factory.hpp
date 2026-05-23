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


#ifndef ANANKE_CAST_FACTORY_HPP
#define ANANKE_CAST_FACTORY_HPP

#include <memory>

#include "data_model/HDL/parameters/components/Expression.hpp"
#include "data_model/HDL/parameters/components/Cast.hpp"
#include "data_model/HDL/parameters/components/Parameter_value_base.hpp"

class cast_factory {
public:
    void start();
    void set_content(const std::shared_ptr<Parameter_value_base> &c);
    void set_type(const std::string &t);
    void advance_cast() {state = build_phase::content;}
    std::shared_ptr<Cast> get_cast();
    bool in_cast() const {return state != build_phase::inactive;}
    bool in_size() const {return state == build_phase::size;}
private:

    enum class build_phase {
        inactive,
        size,
        content
    };

    std::stack<Cast> cast_stack;
    std::stack<build_phase> phases_stack;

    Cast new_cast;

    build_phase state = build_phase::inactive;

};


#endif //ANANKE_CAST_FACTORY_HPP