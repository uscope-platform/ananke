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


#include "analysis/loop_solver.hpp"


std::vector<hdl_integer> loop_solver::solve_loop(const hdl_loop_statement &loop, const std::map<qualified_identifier, resolved_parameter> &context) {
    std::vector<hdl_integer> ret;
    auto loop_var = loop.get_init()->get_identifier();

    auto init_copy = std::make_shared<HDL_parameter>(*loop.get_init());
    auto init_val = init_copy->evaluate(context);
    if (!init_val.has_value() || !init_val.value().is_integer()) return ret;

    hdl_integer idx = init_val.value().get_integer();
    auto ctx = context;
    ctx[loop_var] = resolved_parameter(idx);

    auto cond = loop.get_end_condition()->evaluate(ctx);
    while (cond.has_value() && cond.value().is_integer() && cond.value().get_integer() != 0) {
        ret.push_back(idx);

        auto next = loop.get_iteration()->evaluate(ctx);
        if (!next.has_value() || !next.value().is_integer()) break;
        idx = next.value().get_integer();

        ctx[loop_var] = resolved_parameter(idx);
        cond = loop.get_end_condition()->evaluate(ctx);
    }

    return ret;
}