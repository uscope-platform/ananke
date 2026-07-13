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


std::vector<hdl_integer> loop_solver::solve_loop(std::shared_ptr<HDL_instance_AST> &node, const std::map<qualified_identifier, resolved_parameter> &context) {
    if (node->get_n_loops() == 0) return {};
    if (node->get_n_loops()>1) {
        spdlog::warn("Nested loops are not supported by parameter analysis\n In HDL instance: " + node->get_name() + " of type: " + node->get_type() + " is in a nested loop");
        return {};
    }
   return solve_loop(node->get_inner_loop(), context);
}

std::vector<hdl_integer> loop_solver::solve_loop(const HDL_loop_metadata &loop, const std::map<qualified_identifier, resolved_parameter> &context) {

    std::vector<hdl_integer> ret;

    auto loop_variable = get_init_variable(loop, context);


    while(!is_loop_done(loop_variable, loop.get_end_c(), context)){

        auto val = loop_variable->get_numeric_value();
        if(!val.has_value()) throw std::runtime_error("Could not get the numeric value of a loop variable, something is seriously wrong");
        ret.push_back(val.value());

        update_loop(loop.get_iter(), loop_variable, context);

    }

    return ret;
}

bool loop_solver::is_loop_done(std::shared_ptr<HDL_parameter> &lv, Expression_v2 end_cond,const std::map<qualified_identifier, resolved_parameter> &context) {
    auto val = lv->get_numeric_value();
    if(!val.has_value()) throw std::runtime_error("Could not get the numeric value of a loop variable, something is seriously wrong");
    auto ctx = context;
    ctx[lv->get_identifier()] = val.value();

    auto ec = end_cond.evaluate(ctx);
    if (!ec.has_value()) throw std::runtime_error("Could not evaluate loop end condition");
    if (!ec.value().is_integer()) throw std::runtime_error("loop end condition expression must ret");
    return ec.value().get_integer() == 0;
}

std::shared_ptr<HDL_parameter> loop_solver::get_init_variable(const HDL_loop_metadata &l,const std::map<qualified_identifier, resolved_parameter> &context) {
    auto loop_variable = std::make_shared<HDL_parameter>(l.get_init());
    auto variable_val = loop_variable->evaluate(context);
    if (!variable_val.has_value()) return{};
    if (!variable_val.value().is_integer()) return {};

     loop_variable->set_value(variable_val.value().get_integer());
    return loop_variable;
}

std::shared_ptr<HDL_parameter> loop_solver::update_loop(
    Expression_v2 e,
    std::shared_ptr<HDL_parameter> loop_var,
    const std::map<qualified_identifier, resolved_parameter> &context
    ) {
    auto val = loop_var->get_numeric_value();
    if(!val.has_value()) throw std::runtime_error("Could not get the numeric value of a loop variable, something is seriously wrong");
    auto ctx = context;
    ctx[loop_var->get_identifier()] = val.value();
    auto res = e.evaluate(ctx);
    if (!res.has_value()) throw std::runtime_error("Could not evaluate loop end condition");
    if (!res.value().is_integer()) throw std::runtime_error("loop end condition expression must ret");

    loop_var->set_value(res.value().get_integer());
    return loop_var;
}
