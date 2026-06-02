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

#ifndef ANANKE_TERNARY_FACTORY_HPP
#define ANANKE_TERNARY_FACTORY_HPP

#include "data_model/HDL/parameters/components/Parameter_value_base.hpp"
#include "data_model/HDL/parameters/components/Ternary.hpp"
#include "data_model/HDL/factories/parameters/factory_base.hpp"

class ternary_factory : public factory_base{
public:
    void start_conditional();

    std::shared_ptr<Ternary> finish();

    bool is_nested() {return !ternary_stack.empty();}

    void consume(const std::shared_ptr<Parameter_value_base>& v) override;
    bool active() const override;

private:
    enum class build_phase {
        inactive,
        condition,
        true_assignment,
        false_assignment
    };
    Ternary current;
    build_phase state = build_phase::inactive;
    std::stack<std::pair<Ternary, build_phase>> ternary_stack;
};


#endif //ANANKE_TERNARY_FACTORY_HPP