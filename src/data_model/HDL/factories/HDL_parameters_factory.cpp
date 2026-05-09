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

#include "data_model/HDL/factories/HDL_parameters_factory.hpp"


void HDL_parameters_factory::new_parameter() {
      new_basic_resource();
      current_resource.set_type(HDL_parameter::expression_parameter);
      init_list.set_dimensions(index_factory.get_dimensions(), true);
}

std::shared_ptr<HDL_parameter> HDL_parameters_factory::get_parameter() {
    auto resource = get_resource();
    return std::make_shared<HDL_parameter>(resource);
}

void HDL_parameters_factory::set_value(const std::string &s) {
    current_resource.set_value(s);
}

void HDL_parameters_factory::add_component(const Expression_component &c, bool is_call_argument) {
    if (f_factory.is_active() && !concat_factory.in_concatenation() && !repl_factory.in_replication()) {
        f_factory.add_component(c);
    }else if (is_call_argument) {
        calls_factory.add_argument(std::make_shared<Expression>(Expression({c})));
        expr_factory.increase_level();//this is a ugly hack, why is it needed?
        expr_factory.stop_expression();
    } else if (index_factory.is_active() && !index_factory.is_range()) {
        index_factory.add_component(c);
    } else {
        expr_factory.add_component(c);
    }
}

void HDL_parameters_factory::start_initialization_list() {
    if (in_param_assignment || in_packed_assignment ||f_factory.is_active() || in_param_override) {
        in_initialization_list = true;
        expr_factory.decrease_level(); // This is needed because in the grammar there is an expression before the list initialization;
    }
}


void HDL_parameters_factory::stop_initialization_list(bool default_assignment) {
    if (in_initialization_list) {
        if (repl_factory.in_replication()) {
            stop_replication();
        }
        in_initialization_list = false;
        if (default_assignment){
            init_list.set_default();
        }
        init_list.set_dimensions(index_factory.get_dimensions(), false);
        current_resource.add_initialization_list(init_list);
        init_list = Initialization_list();
        expr_factory.increase_level();
    }
}


void HDL_parameters_factory::start_bit_selection() {
    index_factory.start_index(false);
}

void HDL_parameters_factory::stop_bit_selection() {
    expr_factory.add_index(index_factory.get_index());
    index_factory.stop_index();

}

void HDL_parameters_factory::close_array_index() {
    if(index_factory.is_active() & (in_param_assignment || in_packed_assignment || repl_factory.is_assignment_context() || in_param_override)){
        index_factory.stop_index();
        expr_factory.add_index(index_factory.get_index());
    }
}

void HDL_parameters_factory::start_param_assignment() {
    in_param_assignment = true;
}

void HDL_parameters_factory::stop_param_assignment() {
    in_param_assignment = false;
    if (!init_list.empty()  && current_resource.get_i_l().empty()) {
        current_resource.add_initialization_list(init_list);
    }
}

void HDL_parameters_factory::stop_unpacked_dimension_declaration() {
    if (index_factory.is_active() && index_factory.is_range()) {
        index_factory.stop_index();
    }
}

void HDL_parameters_factory::stop_replication() {
    if(repl_factory.in_replication()){
        if (f_factory.is_active()) {
            f_factory.add_value(repl_factory.finish());
        } else {
            init_list.add_item(repl_factory.finish());
        }
        expr_factory.increase_level();
    }
}

void HDL_parameters_factory::start_replication_assignment() {
    repl_factory.start_replication(false);
}

void HDL_parameters_factory::stop_replication_assignment() {
    init_list.set_scalar(repl_factory.finish());
}

void HDL_parameters_factory::stop_packed_assignment() {
    if(in_packed_assignment && !in_initialization_list){
        init_list.set_dimensions(index_factory.get_dimensions(), false);
        current_resource.add_initialization_list(init_list);
        init_list = Initialization_list();
        in_packed_assignment = false;
    }
}

void HDL_parameters_factory::start_expression_new() {
    expr_factory.start_expression();
}

void HDL_parameters_factory::stop_expression_new() {
    expr_factory.stop_expression();
    if(expr_factory.get_level() == 0){
        auto expr = expr_factory.get_expression();
        if (expr.has_value()) {
            if (c_factory.in_cast()){
                c_factory.set_content(std::make_shared<Expression>(expr.value()));
            }else if (t_factory.in_ternary()) {
                t_factory.add_component(std::make_shared<Expression>(expr.value()));
            } else if(repl_factory.in_replication()) {
                repl_factory.add_expression(expr.value());
            } else if (index_factory.is_range()){
                index_factory.add_expression(expr.value());
            } else if(concat_factory.in_concatenation()) {
                concat_factory.add_component(std::make_shared<Expression>(expr.value()));
            } else if(calls_factory.in_function_call()) {
                calls_factory.add_argument(std::make_shared<Expression>(expr.value()));
            } else if(in_initialization_list) {
                init_list.add_item(std::make_shared<Expression>(expr.value()));
            } else {
                current_resource.set_expression(std::make_shared<Expression>(expr.value()));
            }
        }
        expr_factory.clear_expression();
    }
}

void HDL_parameters_factory::start_packed_assignment() {
    in_packed_assignment = true;
}

void HDL_parameters_factory::start_concatenation() {
    if(in_param_assignment || in_packed_assignment || in_initialization_list || f_factory.is_active()){
        expr_factory.push_level();
        concat_factory.start_concatenation();
    }

}

void HDL_parameters_factory::stop_concatenation() {
    if(concat_factory.in_concatenation()){
        expr_factory.pop_level();
        if (!concat_factory.in_nested()) {
            if (f_factory.is_active()) {
                f_factory.add_value(concat_factory.get_concatenation());
            }else if(in_initialization_list) {
                init_list.add_item(concat_factory.get_concatenation());
            } else {
                init_list.set_scalar(concat_factory.get_concatenation());
            }

        }
        concat_factory.stop_concatenation();
    }
}

void HDL_parameters_factory::start_replication() {
    if(in_param_assignment || in_initialization_list || in_packed_assignment || f_factory.is_active()){
        repl_factory.start_replication(true);
        expr_factory.decrease_level();
    }
}

void HDL_parameters_factory::start_unpacked_dimension_declaration() {
    if(in_param_assignment){
        index_factory.start_index(true);
    }
}

void HDL_parameters_factory::start_packed_dimension() {
    index_factory.start_index(true);
}

void HDL_parameters_factory::stop_packed_dimension() {
    if (index_factory.is_active()) {
        index_factory.stop_index();
        index_factory.set_packed(true);
    }
}


void HDL_parameters_factory::start_instance_parameter_assignment(const std::string& parameter_name) {
    new_basic_resource();
    current_resource.set_type(HDL_parameter::expression_parameter);
    current_resource.set_name(parameter_name);
}

void HDL_parameters_factory::clear_expression() {

    expr_factory.clear_level();
}

void HDL_parameters_factory::start_ternary_operator() {
    expr_factory.push_level();
    t_factory.start_conditional();
}

void HDL_parameters_factory::stop_ternary(){
    expr_factory.pop_level();
    if (!t_factory.is_nested()) {
        if (in_initialization_list) {
            init_list.add_item(t_factory.finish());
        } else {
            current_resource.set_expression(t_factory.finish());
        }
    }else {
        t_factory.add_component(t_factory.finish());
    }
}

void HDL_parameters_factory::start_param_override()  {
    in_param_override = true;
}

void HDL_parameters_factory::stop_param_override() {
    in_param_override = false;
    if (!init_list.empty()  && current_resource.get_i_l().empty()) {
        current_resource.add_initialization_list(init_list);
    }
}

void HDL_parameters_factory::start_function_decl(const std::string &name) {
    f_factory.set_name(name);
}

HDL_function_def HDL_parameters_factory::stop_function_decl() {
    auto f =   f_factory.get_function();
    return f;
}

void HDL_parameters_factory::start_array_quantifier() {
    index_factory.set_quantifier(true);
}

void HDL_parameters_factory::stop_array_quantifier() {
    index_factory.set_quantifier(false);
}

void HDL_parameters_factory::start_cast(bool expression_size) {
    if (in_param_assignment || in_initialization_list || in_param_override || in_packed_assignment|| f_factory.is_active()) {
        if (concat_factory.in_concatenation() || expression_size) {
            expr_factory.start_expression();
        } else {
            expr_factory.decrease_level();
        }
        c_factory.start();
    }
}

void HDL_parameters_factory::set_cast_type(const std::string &t) {
    c_factory.set_type(t);
}

void HDL_parameters_factory::stop_cast() {
    if(c_factory.in_cast()){
        auto expr = expr_factory.get_expression();
        expr_factory.clear_expression();
        if (expr.has_value()) {
            c_factory.set_content(std::make_shared<Expression>(expr.value()));

        }

        auto cast_value = c_factory.get_cast(); // This restores the outer cast context
        expr_factory.increase_level();

        // --- THE FIX ---
        if (c_factory.in_cast()) {
            // Hand the finished inner cast to the outer cast
            c_factory.set_content(cast_value);
        } else if (concat_factory.in_concatenation()) {
            concat_factory.add_component(cast_value);
        } else {
            init_list.set_scalar(cast_value);
        }
    }
}

void HDL_parameters_factory::advance_cast() {
if (c_factory.in_cast()) {
        auto expr = expr_factory.get_expression();
        if (expr.has_value()) {
            c_factory.set_content(std::make_shared<Expression>(expr.value()));
            expr_factory.clear_expression();
        }
        c_factory.advance_cast();
    }
}


void HDL_parameters_factory::start_function_assignment(const std::string &f_name) {
    calls_factory.start_function(f_name);
    expr_factory.pause();
    expr_factory.push_level();
}

void HDL_parameters_factory::stop_function_assignment() {
    if (!calls_factory.is_nested()) {
        if (in_initialization_list) {
            init_list.add_item(calls_factory.get_function());
        } else {
            current_resource.set_expression(calls_factory.get_function());
        }
    }

    calls_factory.finish();
    expr_factory.pop_level();
}

void HDL_parameters_factory::start_function_call(const std::string &f_name) {
    calls_factory.start_function(f_name);
    expr_factory.push_level();
}

void HDL_parameters_factory::stop_function_call() {
    bool nested = calls_factory.is_nested();
    calls_factory.finish();
    expr_factory.pop_level();
    if (!nested) {
        auto call = calls_factory.get_function();
        Expression_component ec(call);
        if(t_factory.in_ternary()) {
            t_factory.add_component(call);
        } else if(expr_factory.is_active()) {
            expr_factory.add_component(ec);
        } else if (index_factory.is_active()) {
            index_factory.add_component(ec);
        } else if (concat_factory.in_concatenation()) {
            concat_factory.add_component(std::make_shared<Expression>(Expression({ec})));
        } else if (repl_factory.in_replication()) {
            repl_factory.add_expression(Expression({ec}));
        } else if (in_packed_assignment || in_param_assignment || in_param_override) {
            init_list.set_scalar(call);
        } else if (in_initialization_list) {
            init_list.add_item(call);
        }
    }
}
