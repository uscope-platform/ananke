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


#ifndef ANANKE_FUNCTION_CALLS_FACTORY_HPP
#define ANANKE_FUNCTION_CALLS_FACTORY_HPP

#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/factories/parameters/factory_base.hpp"

class function_calls_factory : public factory_base{
public:
    void start_function(const std::string& name);
    void set_package_prefix(const std::string &p);

    void consume(const std::shared_ptr<Parameter_value_base> &arg) override;
    bool active() const override;
    std::shared_ptr<Parameter_value_base> result() override;

private:
    HDL_function_call new_call;

    enum class build_phase {
        inactive,
        arguments
    };

    build_phase state = build_phase::inactive;
};


#endif //ANANKE_FUNCTION_CALLS_FACTORY_HPP