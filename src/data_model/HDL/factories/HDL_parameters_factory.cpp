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
#include "data_model/HDL/factories/parameters/replication_factory.hpp"
#include "data_model/HDL/factories/parameters/concatenation_factory.hpp"
#include "data_model/HDL/factories/parameters/function_calls_factory.hpp"
#include "data_model/HDL/factories/parameters/ternary_factory.hpp"
#include "data_model/HDL/factories/parameters/cast_factory.hpp"


void HDL_parameters_factory::new_parameter(const std::string &name) {
    current_resource.set_name(name);
    current_resource.set_declared_type(current_type);
}

std::shared_ptr<HDL_parameter> HDL_parameters_factory::get_parameter() {
    auto resource = get_resource();
    if (type_engine && type_engine->has_type(current_type)) {
        auto dims = type_engine->get_type(current_type).get_packed_dimensions();
        resource.set_packed_dimensions(dims);
        dims = type_engine->get_type(current_type).get_unpacked_dimensions();
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

void HDL_parameters_factory::add_component(const Expression_component &c, bool is_call_argument) {
    if (is_call_argument) {
        if (top_as<function_calls_factory>()) {
            consumer_stack.top()->consume(std::make_shared<Expression>(Expression({c})));
        }
        expr_factory.increase_level();
        expr_factory.stop_expression();
    } else if (in_bit_selection) {
        bit_index.push_back(c);
    } else {
        expr_factory.add_component(c);
    }
}

void HDL_parameters_factory::start_initialization_list() {
    if (ctx == param_context::declaration || ctx == param_context::packed_dim || ctx == param_context::override) {
        auto concat = std::make_unique<concatenation_factory>();
        concat->start_concatenation();
        consumer_stack.push(std::move(concat));
        expr_factory.decrease_level();
    }
}


void HDL_parameters_factory::stop_initialization_list(bool default_assignment) {
    if (top_as<concatenation_factory>()) {
        if (top_as<replication_factory>()) {
            stop_replication();
        }
        if (default_assignment) {
            top_as<concatenation_factory>()->set_default_init();
        }
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        current_resource.add_item(result);
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
    if(in_bit_selection && (ctx == param_context::declaration || ctx == param_context::packed_dim || (top_as<replication_factory>() && top_as<replication_factory>()->is_assignment_context()) || ctx == param_context::override)){
        in_bit_selection = false;
        expr_factory.add_index(bit_index);
    }
}

void HDL_parameters_factory::start_param_assignment() {
    ctx = param_context::declaration;
}

void HDL_parameters_factory::stop_param_assignment() {
    ctx = param_context::idle;
}

void HDL_parameters_factory::stop_replication() {
    if (top_as<replication_factory>()) {
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        if (!consumer_stack.empty()) {
            consumer_stack.top()->consume(result);
        } else {
            current_resource.add_item(result);
        }
        expr_factory.increase_level();
    }
}

void HDL_parameters_factory::start_replication_assignment() {
    r_factory.stop();
    auto repl = std::make_unique<replication_factory>();
    repl->start_replication(false);
    consumer_stack.push(std::move(repl));
    expr_factory.push_level();
}

void HDL_parameters_factory::stop_replication_assignment() {
    if (top_as<replication_factory>()) {
        current_resource.add_item(consumer_stack.top()->result());
        consumer_stack.pop();
        expr_factory.pop_level();
    }
}

void HDL_parameters_factory::stop_packed_assignment() {
    if(ctx == param_context::packed_dim && !top_as<concatenation_factory>()){
        ctx = param_context::idle;
    }
}

void HDL_parameters_factory::start_expression_new() {
    expr_factory.start_expression();
}

void HDL_parameters_factory::stop_expression_new() {
    expr_factory.stop_expression();
    if (expr_factory.get_level() == 0) {
        auto expr = expr_factory.get_expression();
        if (expr.has_value()) {
            if (type_engine && type_engine->active()) {
                type_engine->add_expression(expr.value());
            } else if (r_factory.active()) {
                r_factory.add_expression(expr.value());
            } else if (!consumer_stack.empty()) {
                consumer_stack.top()->consume(std::make_shared<Expression>(expr.value()));
            } else {
                current_resource.set_scalar(std::make_shared<Expression>(expr.value()));
            }
        }
        expr_factory.clear_expression();
    }
}

void HDL_parameters_factory::start_packed_assignment() {
    r_factory.stop();
    ctx = param_context::packed_dim;
}

void HDL_parameters_factory::start_concatenation() {
    if(ctx == param_context::declaration || ctx == param_context::packed_dim){
        expr_factory.push_level();
        auto concat = std::make_unique<concatenation_factory>();
        concat->start_concatenation();
        consumer_stack.push(std::move(concat));
    }

}

void HDL_parameters_factory::stop_concatenation() {
    if (top_as<concatenation_factory>()) {
        expr_factory.pop_level();
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        if (!consumer_stack.empty()) {
            consumer_stack.top()->consume(result);
        } else {
            current_resource.add_item(result);
        }
    }
}

void HDL_parameters_factory::start_replication() {
    if(ctx == param_context::declaration || ctx == param_context::packed_dim || top_as<concatenation_factory>()){
        auto repl = std::make_unique<replication_factory>();
        repl->start_replication(true);
        consumer_stack.push(std::move(repl));
        expr_factory.decrease_level();
    }
}

void HDL_parameters_factory::start_unpacked_dimension_declaration() {
    if (type_engine && type_engine->active())
        type_engine->start_unpacked_dimension_declaration();
    else
        r_factory.advance_stage();
}

void HDL_parameters_factory::advance_range() {
    if (type_engine && type_engine->active())
        type_engine->advance_range();
    else
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
    auto ternary = std::make_unique<ternary_factory>();
    ternary->start_conditional();
    consumer_stack.push(std::move(ternary));
}

void HDL_parameters_factory::stop_ternary(){
    expr_factory.pop_level();
    if (top_as<ternary_factory>()) {
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        if (!consumer_stack.empty()) {
            consumer_stack.top()->consume(result);
        } else {
            current_resource.set_scalar(result);
        }
    }
}

void HDL_parameters_factory::start_param_override()  {
    ctx = param_context::override;
}

void HDL_parameters_factory::stop_param_override() {
    ctx = param_context::idle;
}

void HDL_parameters_factory::start_range() {
    if (type_engine && type_engine->active())
        type_engine->start_range();
    else
        r_factory.start();
}

void HDL_parameters_factory::stop_range() {
    if (type_engine && type_engine->active())
        type_engine->stop_range();
    else
        r_factory.stop();
}

void HDL_parameters_factory::open_range() {
    if (type_engine && type_engine->active())
        type_engine->open_range();
    else
        r_factory.open_range();
}

void HDL_parameters_factory::close_range() {
    if (type_engine && type_engine->active())
        type_engine->close_range();
    else
        r_factory.close_range();
}




void HDL_parameters_factory::close_packed_dimensions() {
    if (type_engine && type_engine->active())
        type_engine->close_packed_dimensions();
    else
        r_factory.advance_stage();
}

void HDL_parameters_factory::start_cast(bool expression_size) {
    bool in_ctx = ctx != param_context::idle || top_as<concatenation_factory>();
    if (in_ctx) {
        if (top_as<concatenation_factory>() || expression_size) {
            expr_factory.start_expression();
        } else {
            expr_factory.decrease_level();
        }
        auto cast = std::make_unique<cast_factory>();
        cast->start();
        consumer_stack.push(std::move(cast));
    }
}

void HDL_parameters_factory::set_cast_type(const std::string &t) {
    auto* cast = top_as<cast_factory>();
    if (cast) {
        cast->set_type(t);
    }
}

void HDL_parameters_factory::stop_cast() {
    if (top_as<cast_factory>()) {
        auto expr = expr_factory.get_expression();
        expr_factory.clear_expression();
        if (expr.has_value()) {
            consumer_stack.top()->consume(std::make_shared<Expression>(expr.value()));
        }

        auto cast_value = consumer_stack.top()->result();
        consumer_stack.pop();
        expr_factory.increase_level();

        if (!consumer_stack.empty()) {
            consumer_stack.top()->consume(cast_value);
        } else {
            current_resource.set_scalar(cast_value);
        }
    }
}

void HDL_parameters_factory::advance_cast() {
    auto* cast = top_as<cast_factory>();
    if (cast) {
        auto expr = expr_factory.get_expression();
        if (expr.has_value()) {
            cast->consume(std::make_shared<Expression>(expr.value()));
            expr_factory.clear_expression();
        }
        cast->advance_cast();
    }
}


void HDL_parameters_factory::start_function_assignment(const std::string &f_name) {
    auto calls = std::make_unique<function_calls_factory>();
    calls->start_function(f_name);
    consumer_stack.push(std::move(calls));
    expr_factory.pause();
    expr_factory.push_level();
}

void HDL_parameters_factory::stop_function_assignment() {
    if (top_as<function_calls_factory>()) {
        auto result = consumer_stack.top()->result();
        consumer_stack.pop();
        if (!consumer_stack.empty()) {
            consumer_stack.top()->consume(result);
        } else {
            current_resource.set_scalar(result);
        }
    }
    expr_factory.pop_level();
}

void HDL_parameters_factory::start_function_call(const std::string &f_name) {
    auto calls = std::make_unique<function_calls_factory>();
    calls->start_function(f_name);
    consumer_stack.push(std::move(calls));
    expr_factory.push_level();
}

void HDL_parameters_factory::stop_function_call() {
    if (top_as<function_calls_factory>()) {
        auto call = consumer_stack.top()->result();
        consumer_stack.pop();
        expr_factory.pop_level();
        Expression_component ec(call);
        if (top_as<function_calls_factory>()) {
            consumer_stack.top()->consume(call);
        } else if (top_as<ternary_factory>()) {
            consumer_stack.top()->consume(call);
        } else if(expr_factory.active()) {
            expr_factory.add_component(ec);
        } else if (in_bit_selection) {
            bit_index.push_back(ec);
        } else if (top_as<concatenation_factory>()) {
            consumer_stack.top()->consume(std::make_shared<Expression>(Expression({ec})));
        } else if (top_as<replication_factory>()) {
            consumer_stack.top()->consume(std::make_shared<Expression>(Expression({ec})));
        } else if (ctx == param_context::packed_dim || ctx == param_context::declaration || ctx == param_context::override) {
            current_resource.set_scalar(call);
        }
    }
}
