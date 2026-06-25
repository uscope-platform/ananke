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

#include "data_model/HDL/parameters/components/Cast.hpp"
#include "data_model/HDL/parameters/components/Parameter_value_base.hpp"
#include "data_model/HDL/factories/parameters/factory_base.hpp"

class cast_factory : public factory_base{
public:
    void start();
    void start(bool is_expr_size);
    void set_type(const std::string &t);
    void advance_cast() {state = build_phase::content;}
    bool in_size() const {return state == build_phase::size;}
    bool is_expression_size() const {return expression_size;}
    void consume(const std::shared_ptr<Parameter_value_base>& v) override;
    bool active() const override;
    std::shared_ptr<Parameter_value_base> result() override;
private:

    enum class build_phase {
        inactive,
        size,
        content
    };

    Cast new_cast;
    bool expression_size = false;

    build_phase state = build_phase::inactive;

};


#endif //ANANKE_CAST_FACTORY_HPP