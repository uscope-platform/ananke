//  Copyright 2024 Filippo Savi
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


#ifndef ANANKE_HDL_LOOP_SOLVER_HPP
#define ANANKE_HDL_LOOP_SOLVER_HPP


#include <memory>
#include "data_model/HDL/parameters/Parameters_map.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"
#include "data_model/HDL/statement/hdl_loop_statement.hpp"

class loop_solver {
public:
    loop_solver() = default;
    static std::vector<hdl_integer> solve_loop(const hdl_loop_statement &loop, const std::map<qualified_identifier, resolved_parameter> &context);
};


#endif //ANANKE_HDL_LOOP_SOLVER_HPP
