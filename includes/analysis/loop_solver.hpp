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
#include "data_model/HDL/HDL_instance_AST.hpp"

class loop_solver {
public:
    loop_solver() = default;
    static std::vector<hdl_integer> solve_loop(std::shared_ptr<HDL_instance_AST> &node, const std::map<qualified_identifier, resolved_parameter> &context);
    static std::vector<hdl_integer> solve_loop(const HDL_loop_metadata &loop, const std::map<qualified_identifier, resolved_parameter> &context);
private:
    static std::shared_ptr<HDL_parameter> get_init_variable(const HDL_loop_metadata &l, const std::map<qualified_identifier, resolved_parameter> &context);
    static std::shared_ptr<HDL_parameter> update_loop(
        Expression e,
        std::shared_ptr<HDL_parameter> loop_var,
        const std::map<qualified_identifier, resolved_parameter> &context
    );
    static bool is_loop_done(std::shared_ptr<HDL_parameter> &lv, Expression end_cond,const std::map<qualified_identifier, resolved_parameter> &context);
};


#endif //ANANKE_HDL_LOOP_SOLVER_HPP
