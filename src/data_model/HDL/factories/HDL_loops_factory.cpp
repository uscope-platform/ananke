//  Copyright 2025 Filippo Savi
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

#include "data_model/HDL/factories/HDL_loops_factory.hpp"

void HDL_loops_factory::new_loop() {
    repeated_instances.clear();
    end_cond_valid = false;
    active = true;
}

void HDL_loops_factory::clear() {
    repeated_instances.clear();
    loop_specs = HDL_loop_metadata();
    current_expression = Expression_v2();
    loop_phase = init;
    end_cond_valid = false;
    active = false;
}


void HDL_loops_factory::start_assignment(const std::string &name) {
    if(loop_phase == body) {
        expression_valid = true;
        loop_specs.add_assignment(assignment(name, {}, {}));
    }
}

std::vector<HDL_instance> HDL_loops_factory::get_instances() {
    active = false;
    return repeated_instances;
}

void HDL_loops_factory::add_component(const Token &c) {
    if (c.is_operator()) {
        current_expression.set_operation(map_operator(c.get_operation()));
    } else if (current_expression.get_lhs() == nullptr) {
        current_expression.set_lhs(std::make_shared<Token>(c));
    } else {
        current_expression.set_rhs(std::make_shared<Token>(c));
    }
}

void HDL_loops_factory::add_loop_variable(const std::string &p) {
    HDL_parameter param;
    param.set_name(p);
    loop_specs.set_init(param);
}

void HDL_loops_factory::set_phase(loop_phase_t p) {
    loop_phase = p;
    if(p==init) {
        current_expression = Expression_v2();
    } else if(p==end) {
        auto init = loop_specs.get_init();
        init.set_raw_value(Expression_v2::unwrap(current_expression));
        loop_specs.set_init(init);
        current_expression = Expression_v2();
    } else if(p==step) {
        loop_specs.set_end_c(current_expression);
        current_expression = Expression_v2();
    } else if(p==body) {
        loop_specs.set_iter(current_expression);
        current_expression = Expression_v2();
    }
}

void HDL_loops_factory::advance_expression() {
    if(expression_valid) {
        auto assignments = loop_specs.get_assignments();
        assignments.back().set_index(Expression_v2::unwrap(current_expression));
        loop_specs.set_assignments(assignments);
        current_expression = Expression_v2();
    }
}

void HDL_loops_factory::close_expression() {
    if(expression_valid) {
        auto assignments = loop_specs.get_assignments();
        assignments.back().set_value(Expression_v2::unwrap(current_expression));
        loop_specs.set_assignments(assignments);
        expression_valid = false;
        current_expression = Expression_v2();
    }
}

void HDL_loops_factory::add_expression(const Expression_v2 &e) {
    if(loop_phase == step) {
        current_expression = e;
    } else {
        if(end_cond_valid){
            loop_specs.set_iter(e);
        } else {
            loop_specs.set_end_c(e);
            end_cond_valid = true;
        }
    }

}

void HDL_loops_factory::add_instance(HDL_instance &i) {
    i.add_loop(loop_specs);
    repeated_instances.push_back(i);
}

Expression_v2::expression_operator HDL_loops_factory::map_operator(Token::sv_operators op) {
    switch (op) {
        case Token::logic_neg: return Expression_v2::logic_neg;
        case Token::bitwise_neg: return Expression_v2::bitwise_neg;
        case Token::power: return Expression_v2::power;
        case Token::multiply: return Expression_v2::multiply;
        case Token::divide: return Expression_v2::divide;
        case Token::modulo: return Expression_v2::modulo;
        case Token::add: return Expression_v2::add;
        case Token::subtract: return Expression_v2::subtract;
        case Token::logic_shift_left: return Expression_v2::logic_shift_left;
        case Token::logic_shift_right: return Expression_v2::logic_shift_right;
        case Token::arithmetic_shift_left: return Expression_v2::arithmetic_shift_left;
        case Token::arithmetic_shift_right: return Expression_v2::arithmetic_shift_right;
        case Token::greater: return Expression_v2::greater;
        case Token::greater_equal: return Expression_v2::greater_equal;
        case Token::less: return Expression_v2::less;
        case Token::less_equal: return Expression_v2::less_equal;
        case Token::equal: return Expression_v2::equal;
        case Token::not_equal: return Expression_v2::not_equal;
        case Token::bitwise_and: return Expression_v2::bitwise_and;
        case Token::bitwise_xor: return Expression_v2::bitwise_xor;
        case Token::bitwise_xnor: return Expression_v2::bitwise_xnor;
        case Token::bitwise_or: return Expression_v2::bitwise_or;
        case Token::logical_and: return Expression_v2::logical_and;
        case Token::logical_or: return Expression_v2::logical_or;
        default: return Expression_v2::none;
    }
}

