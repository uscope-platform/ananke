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


void HDL_parameters_factory::new_parameter(const std::string &name) {
    current_resource.set_name(name);
    current_resource.set_declared_type(current_type);
}

std::shared_ptr<HDL_parameter> HDL_parameters_factory::get_parameter() {
    auto resource = get_resource();
    if (typedefs.contains(current_type)) {
        auto dims = typedefs.at(current_type).get_packed_dimensions();
        resource.set_packed_dimensions(dims);
        dims = typedefs.at(current_type).get_unpacked_dimensions();
        resource.set_unpacked_dimensions(dims);
    } else {
        auto [packed, unpacked] = r_factory.get_dimensions();
        resource.set_packed_dimensions(packed);
        resource.set_unpacked_dimensions(unpacked);
        r_factory.clear();
    }
    return std::make_shared<HDL_parameter>(resource);
}

void HDL_parameters_factory::set_type(const std::string &type) {
    current_type = type;
}

void HDL_parameters_factory::set_value(const std::string &s) {
    current_resource.set_value(s);
}

void HDL_parameters_factory::add_component(const Expression_component &c, bool is_call_argument) {
    if (f_factory.is_active() && f_factory.is_raw_body()) {
        f_factory.add_component(c);
    }else if (is_call_argument) {
        calls_factory.consume(std::make_shared<Expression>(Expression({c})));
        // The grammar parses the data_type argument of a system function call
        // inside an expression context. This increase_level balances the
        // subsequent stop_expression so it doesn't underflow the outer expression level.
        expr_factory.increase_level();
        expr_factory.stop_expression();
    } else if (in_bit_selection) {
        bit_index.push_back(c);
    } else {
        expr_factory.add_component(c);
    }
}

void HDL_parameters_factory::start_initialization_list() {
    if (in_param_assignment || in_packed_assignment || in_param_override) {
        concat_factory.start_concatenation();
        expr_factory.decrease_level(); // This is needed because in the grammar there is an expression before the list initialization;
    }
}


void HDL_parameters_factory::stop_initialization_list(bool default_assignment) {
    if (concat_factory.active()) {
        if (repl_factory.active()) {
            stop_replication();
        }
        if (default_assignment){
           concat_factory.set_default_init();
        }
        current_resource.add_item(concat_factory.get_concatenation());
        concat_factory.stop_concatenation();
        expr_factory.increase_level();
    }
}


void HDL_parameters_factory::start_bit_selection() {
    if (!r_factory.active()) {
        in_bit_selection = true;
        bit_index = Expression();
    }
}

void HDL_parameters_factory::stop_bit_selection() {
    if (in_bit_selection) {
        expr_factory.add_index(bit_index);
        in_bit_selection = false;
    }
}

void HDL_parameters_factory::close_array_index() {
    if(in_bit_selection && (in_param_assignment || in_packed_assignment || repl_factory.is_assignment_context() || in_param_override)){
        in_bit_selection = false;
        expr_factory.add_index(bit_index);
    }
}

void HDL_parameters_factory::start_param_assignment() {
    in_param_assignment = true;
}

void HDL_parameters_factory::stop_param_assignment() {
    in_param_assignment = false;
}

void HDL_parameters_factory::stop_replication() {
    if (f_factory.is_active()) {
        f_factory.stop_replication();
        expr_factory.increase_level();
    }
    if(repl_factory.active()){
        if (concat_factory.active()){
            concat_factory.consume(repl_factory.finish());
        } else {
            current_resource.add_item(repl_factory.finish());
        }
        expr_factory.increase_level();
    }
}

void HDL_parameters_factory::start_replication_assignment() {
    r_factory.stop();
    repl_factory.start_replication(false);
    expr_factory.push_level();
}

void HDL_parameters_factory::stop_replication_assignment() {
    current_resource.add_item(repl_factory.finish());
    expr_factory.pop_level();
}

void HDL_parameters_factory::stop_packed_assignment() {
    if(in_packed_assignment && !concat_factory.active()){
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
            if (c_factory.active()){
                c_factory.consume(std::make_shared<Expression>(expr.value()));
            }else if (t_factory.active()) {
                t_factory.consume(std::make_shared<Expression>(expr.value()));
            } else if(repl_factory.active()) {
                repl_factory.consume(std::make_shared<Expression>(expr.value()));
            } else if (r_factory.active()) {
                r_factory.add_expression(expr.value());
            } else if(concat_factory.active()) {
                concat_factory.consume(std::make_shared<Expression>(expr.value()));
            } else if (f_factory.is_active()) {
                f_factory.add_value(std::make_shared<Expression>(expr.value()));
            } else if(calls_factory.active()) {
                calls_factory.consume(std::make_shared<Expression>(expr.value()));
            } else {
                current_resource.set_scalar(std::make_shared<Expression>(expr.value()));
            }
        }
        expr_factory.clear_expression();
    }
}

void HDL_parameters_factory::start_packed_assignment() {
    r_factory.stop();
    in_packed_assignment = true;
}

void HDL_parameters_factory::start_concatenation() {
    if (f_factory.is_active()) {
        f_factory.start_concat();
        expr_factory.push_level();
    }
    if(in_param_assignment || in_packed_assignment){
        expr_factory.push_level();
        concat_factory.start_concatenation();
    }

}

void HDL_parameters_factory::stop_concatenation() {
    if(f_factory.is_active()){
        f_factory.stop_concat();
        expr_factory.pop_level();
    }
    if(concat_factory.active()){
        expr_factory.pop_level();
        if (!concat_factory.in_nested()) current_resource.add_item(concat_factory.get_concatenation());
        concat_factory.stop_concatenation();
    }
}

void HDL_parameters_factory::start_replication() {
    if (f_factory.is_active()) {
        f_factory.start_replication();
        expr_factory.decrease_level();
    }
    if(in_param_assignment || in_packed_assignment || concat_factory.active()){
        repl_factory.start_replication(true);
        expr_factory.decrease_level();
    }
}

void HDL_parameters_factory::start_unpacked_dimension_declaration() {
    r_factory.advance_stage();
}

void HDL_parameters_factory::start_packed_dimension() {
}

void HDL_parameters_factory::stop_packed_dimension() {
}


void HDL_parameters_factory::advance_range() {
    r_factory.advance_range();
}

void HDL_parameters_factory::start_instance_parameter_assignment(const std::string& parameter_name) {
    new_basic_resource(parameter_name);
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
        if (concat_factory.active()) {
            concat_factory.consume(t_factory.finish());
        } else {
            current_resource.set_scalar(t_factory.finish());
        }
    }else {
        t_factory.consume(t_factory.finish());
    }
}

void HDL_parameters_factory::start_param_override()  {
    in_param_override = true;
}

void HDL_parameters_factory::stop_param_override() {
    in_param_override = false;
}

void HDL_parameters_factory::start_range() {
    r_factory.start();
}

void HDL_parameters_factory::stop_range() {
    r_factory.stop();
}

void HDL_parameters_factory::open_range() {
    r_factory.open_range();
}

void HDL_parameters_factory::close_range() {
    r_factory.close_range();
}


void HDL_parameters_factory::start_type_declaration() {
    r_factory.start();
}

HDL_type HDL_parameters_factory::stop_type_declaration(const std::string &name) {
    r_factory.stop();
    HDL_type t;
    auto [packed, unpacked] = r_factory.get_dimensions();
    t.set_packed_dimensions(packed);
    t.set_unpacked_dimensions(unpacked);
    r_factory.clear();
    typedefs[name] = t;
    return t;
}



void HDL_parameters_factory::close_packed_dimensions() {
    r_factory.advance_stage();
}

void HDL_parameters_factory::close_dimension() {

}

void HDL_parameters_factory::start_function_decl(const std::string &name) {
    f_factory.set_name(name);
}

HDL_function_def HDL_parameters_factory::stop_function_decl() {
    auto f =   f_factory.get_function();
    return f;
}

void HDL_parameters_factory::start_cast(bool expression_size) {
    if (f_factory.is_active()) {
        f_factory.start_cast();
        if (expression_size) {
            expr_factory.start_expression();
        } else {
            expr_factory.decrease_level();
        }
        return;
    }
    if (in_param_assignment || in_param_override || in_packed_assignment || concat_factory.active()) {
        if (concat_factory.active() || expression_size) {
            expr_factory.start_expression();
        } else {
            expr_factory.decrease_level();
        }
        c_factory.start();
    }
}

void HDL_parameters_factory::set_cast_type(const std::string &t) {
    if (f_factory.is_active()) {
        f_factory.set_cast_type(t);
        return;
    }
    c_factory.set_type(t);
}

void HDL_parameters_factory::stop_cast() {
    if (f_factory.is_active()) {
        auto expr = expr_factory.get_expression();
        expr_factory.clear_expression();
        if (expr.has_value()) {
            f_factory.add_value(std::make_shared<Expression>(expr.value()));
        }
        f_factory.stop_cast();
        expr_factory.increase_level();
        return;
    }
    if(c_factory.active()){
        auto expr = expr_factory.get_expression();
        expr_factory.clear_expression();
        if (expr.has_value()) {
            c_factory.consume(std::make_shared<Expression>(expr.value()));
        }

        auto cast_value = c_factory.get_cast(); // This restores the outer cast context
        expr_factory.increase_level();

        if (c_factory.active()) {
            // Hand the finished inner cast to the outer cast
            c_factory.consume(cast_value);
        } else if (concat_factory.active()) {
            concat_factory.consume(cast_value);
        } else {
            current_resource.set_scalar(cast_value);
        }
    }
}

void HDL_parameters_factory::advance_cast() {
    if (f_factory.is_active()) {
        auto expr = expr_factory.get_expression();
        if (expr.has_value()) {
            f_factory.add_value(std::make_shared<Expression>(expr.value()));
            expr_factory.clear_expression();
        }
        f_factory.advance_cast();
        return;
    }
    if (c_factory.active()) {
        auto expr = expr_factory.get_expression();
        if (expr.has_value()) {
            c_factory.consume(std::make_shared<Expression>(expr.value()));
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
        if (concat_factory.active()) {
            concat_factory.consume(calls_factory.get_function());
        } else {
            current_resource.set_scalar(calls_factory.get_function());
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
        if(t_factory.active()) {
            t_factory.consume(call);
        } else if(expr_factory.active()) {
            expr_factory.add_component(ec);
        } else if (in_bit_selection) {
            bit_index.push_back(ec);
        } else if (concat_factory.active()) {
            concat_factory.consume(std::make_shared<Expression>(Expression({ec})));
        } else if (repl_factory.active()) {
            repl_factory.consume(std::make_shared<Expression>(Expression({ec})));
        } else if (in_packed_assignment || in_param_assignment || in_param_override) {
            current_resource.set_scalar(call);
        }
    }
}
