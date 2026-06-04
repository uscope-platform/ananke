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

#include "data_model/HDL/factories/parameters/function_calls_factory.hpp"

void function_calls_factory::start_function(const std::string &name) {
    new_call = HDL_function_call();
    new_call.set_name(name);
    state = build_phase::arguments;
}


void function_calls_factory::consume(const std::shared_ptr<Parameter_value_base> &arg) {
    new_call.add_argument(arg);
}

bool function_calls_factory::active() const {
    return state != build_phase::inactive;
}

std::shared_ptr<Parameter_value_base> function_calls_factory::result() {
    auto call = std::make_shared<HDL_function_call>(new_call);
    new_call = HDL_function_call();
    return call;
}

