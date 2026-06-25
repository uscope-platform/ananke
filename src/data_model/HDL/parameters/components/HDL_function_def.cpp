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


#include "data_model/HDL/parameters/HDL_function_def.hpp"

#include "analysis/loop_solver.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"

#include "data_model/HDL/parameters/components/Expression_v2.hpp"

HDL_function_def HDL_function_def::clone() {
    HDL_function_def def;
    def.name = name;
    for(auto &a:assignments) {
        def.assignments.push_back(a.clone());
    }
    if( loop_metadata.has_value())
        def.loop_metadata =  loop_metadata->clone();
    else
        def.loop_metadata = std::nullopt;
    return def;
}

void HDL_function_def::start_assignment(const std::string &n, const std::shared_ptr<Parameter_value_base> &idx) {
   if (idx == nullptr) assignments.push_back({name, {}, {}});
    else assignments.push_back({name, idx, {}});
}

void HDL_function_def::close_assignment(const std::shared_ptr<Parameter_value_base> &val) {
    if (assignments.empty()) {
        throw std::runtime_error(fmt::format("parsing of the {} function definition lead to an inconsistent state, report it as a possible bug", name));
    }
    assignments.back().set_value(val);
}

bool HDL_function_def::is_scalar() const {
    return  !loop_metadata.has_value() && assignments.size() == 1;
}

bool HDL_function_def::operator==(const HDL_function_def &rhs) const {
    bool retval = true;
    retval &= name == rhs.name;
    retval &= assignments == rhs.assignments;
    retval &= loop_metadata == rhs.loop_metadata;
    retval &= argument_names == rhs.argument_names;
    return retval;
}

