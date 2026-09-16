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
    auto kind = (language == hdl_language::system_verilog)
        ? HDL_builtin_function::from_sv(name)
        : HDL_builtin_function::parse(name);
    is_builtin = kind.has_value();
    if (is_builtin) {
        new_builtin = std::make_shared<HDL_builtin_function>(kind.value());
    }
    state = build_phase::arguments;
}

void function_calls_factory::set_package_prefix(const std::string &p) {
    // An explicit package qualifier always denotes a user-defined function:
    // builtins take no package prefix (the VHDL path strips qualification
    // before classifying, and `$`-builtins are unqualified on the SV path).
    // Without this, e.g. p::maximum(...) is parsed as $max with the package
    // silently discarded, shadowing the user's definition.
    if (!p.empty() && is_builtin) {
        is_builtin = false;
        new_builtin.reset();
    }
    // Package prefixes only apply to user-defined function calls.
    if (!is_builtin) new_call.add_package_prefix(p);
}

void function_calls_factory::consume(const std::shared_ptr<Expression_base> &arg) {
    if (is_builtin)
        new_builtin->add_argument(arg);
    else
        new_call.add_argument(arg);
}

bool function_calls_factory::active() const {
    return state != build_phase::inactive;
}

std::shared_ptr<Expression_base> function_calls_factory::result() {
    std::shared_ptr<Expression_base> ret;
    if (is_builtin) {
        ret = new_builtin;
        new_builtin.reset();
    } else {
        ret = std::make_shared<HDL_function_call>(new_call);
        new_call = HDL_function_call();
    }
    return ret;
}