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

#include <algorithm>
#include <ctre.hpp>

#include "frontend/analysis/system_verilog/sv_visitor.hpp"

#include <sys/stat.h>

#include "frontend/analysis/system_verilog/sv_parsing_helpers.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/components/token/String_token.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"


void sv_visitor::route_expression_text(const std::string& text) {
    if(loops_factory.in_loop()) {
        loops_factory.add_component(sv_parsing_helpers::make_value(text));
    }
    if(type_engine.active() || type_engine.is_ranging()){
        type_engine.add_component(sv_parsing_helpers::make_value(text));
    } else if(params_factory.is_component_relevant()){
        params_factory.add_component(sv_parsing_helpers::make_value(text));
    }
    if (f_factory.is_active()) {
        f_factory.add_component(sv_parsing_helpers::make_value(text));
    }
    if(deps_factory.is_valid_dependency()){
        deps_factory.add_connection_element(text);
    }
}

void sv_visitor::route_expression_component(const std::shared_ptr<Expression_base> &ec) {
    auto clone = [](const std::shared_ptr<Expression_base> &src) -> std::shared_ptr<Expression_base> {
        if (auto id = std::dynamic_pointer_cast<const Identifier_token>(src))
            return std::make_shared<Identifier_token>(*id);
        if (auto num = std::dynamic_pointer_cast<const Numeric_token>(src))
            return std::make_shared<Numeric_token>(*num);
        if (auto str = std::dynamic_pointer_cast<const String_token>(src))
            return std::make_shared<String_token>(*str);
        return src;
    };

    bool routed = false;
    if(loops_factory.in_loop()) {
        loops_factory.add_component(routed ? clone(ec) : ec);
        routed = true;
    }
    if (f_factory.is_active()) {
        f_factory.add_component(routed ? clone(ec) : ec);
        routed = true;
    }
    if(type_engine.active() || type_engine.is_ranging()){
        type_engine.add_component(routed ? clone(ec) : ec);
        routed = true;
    } else if(params_factory.is_component_relevant()){
        params_factory.add_component(routed ? clone(ec) : ec);
        routed = true;
    }
}


void sv_visitor::enterModule_declaration(sv2017::Module_declarationContext *ctx) {
    current_declaration_type = "module";
    size_t line_number = ctx->getStart()->getLine();
    auto module_name = ctx->module_header_common()->identifier()->getText();
    modules_factory.new_module(module_name, module, line_number);
}


void sv_visitor::exitModule_declaration(sv2017::Module_declarationContext *ctx) {
    entities.push_back(modules_factory.get_module());
}

void sv_visitor::enterInterface_declaration(sv2017::Interface_declarationContext *ctx) {
    current_declaration_type = "interface";
    size_t line_number = ctx->getStart()->getLine();
    std::string interface_name = ctx->interface_header()->identifier()->getText();
    interfaces_factory.new_interface(interface_name, line_number);
}

void sv_visitor::exitInterface_declaration(sv2017::Interface_declarationContext *ctx) {
    entities.push_back(interfaces_factory.get_interface());
}


void sv_visitor::enterModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *ctx) {
    std::string module_name = ctx->identifier()->getText();
    std::string instance_name = ctx->hierarchical_instance(0)->name_of_instance()->identifier()->getText();

    deps_factory.new_dependency(instance_name, module_name, module);
}


void sv_visitor::exitModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *ctx) {
    if(loops_factory.in_loop()){
        loops_factory.add_statement(deps_factory.get_dependency());
    } else {
        modules_factory.add_statement(deps_factory.get_dependency());
    }

}

void sv_visitor::enterName_of_instance(sv2017::Name_of_instanceContext *ctx) {
    if(!ctx->unpacked_dimension().empty()){
        params_factory.start_param_assignment();
        params_factory.set_type(std::make_shared<HDL_simple_type>());
        params_factory.new_parameter("instance_array_qualifier");
    }
}

void sv_visitor::exitName_of_instance(sv2017::Name_of_instanceContext *ctx) {
    if(!ctx->unpacked_dimension().empty()){
        deps_factory.add_array_quantifier(params_factory.get_parameter());
    }
}

void sv_visitor::enterTf_port_item(sv2017::Tf_port_itemContext *ctx) {
    if (ctx->identifier()) {
        f_factory.add_argument(ctx->identifier()->getText());
    } else if (ctx->data_type_or_implicit()) {
        auto name = ctx->data_type_or_implicit()->getText();
        f_factory.add_argument(name);
    } else {
        int i =0;
    }

}

void sv_visitor::exitTf_port_list(sv2017::Tf_port_listContext *ctx) {
    f_factory.start_body();
}

std::shared_ptr<hdl_type> sv_visitor::setup_data_type(sv2017::Data_type_or_implicitContext *dtoi) {
    type_engine.set_base_type(std::make_shared<HDL_simple_type>());
    if (dtoi && dtoi->data_type()) {
        if (dtoi->data_type()->package_or_class_scoped_path() &&
            !dtoi->data_type()->package_or_class_scoped_path()->DOUBLE_COLON().empty()) {
            auto pkg_ctx = dtoi->data_type()->package_or_class_scoped_path();
            auto qi = sv_parsing_helpers::parse_qualified_identifier(pkg_ctx);
            auto ext_type = std::make_shared<HDL_external_type>(qi);
            type_engine.set_base_type(ext_type);
            return ext_type;
        }
        std::string type;
        if (dtoi->data_type()->data_type_primitive())
            type = dtoi->data_type()->data_type_primitive()->getText();
        if (dtoi->data_type()->package_or_class_scoped_path())
            type = dtoi->data_type()->package_or_class_scoped_path()->getText();
        auto resolved = type_engine.resolve_type(type);
        type_engine.set_base_type(resolved);
        return resolved;
    }
    if (!dtoi) {
        auto implicit = Type_engine::create_primitive_type("implicit");
        type_engine.set_base_type(implicit);
        return implicit;
    }
    return std::make_shared<HDL_simple_type>();
}

void sv_visitor::enterData_declaration(sv2017::Data_declarationContext *ctx) {
    if (f_factory.is_active() && !ctx->type_declaration() &&
        !(ctx->data_type_or_implicit() && ctx->data_type_or_implicit()->data_type() &&
          ctx->data_type_or_implicit()->data_type()->struct_union())) {
        in_function_var_decl = true;
        setup_data_type(ctx->data_type_or_implicit());
        type_engine.start_range();
        return;
    }
    if (ctx->type_declaration()) {
        if (
            ctx->type_declaration()->data_type() &&
            ctx->type_declaration()->data_type()->struct_union()
        ) {
            if (ctx->type_declaration()->data_type()->struct_union()->KW_STRUCT())
                type_engine.start_composite_type_declaration(Type_engine::struct_type);
        } else {
            type_engine.start_simple_type_declaration();
        }
    } else {
        if (ctx->data_type_or_implicit() &&
            ctx->data_type_or_implicit()->data_type() &&
            ctx->data_type_or_implicit()->data_type()->struct_union()
        ) {
            in_anonymous_struct = true;
            type_engine.start_composite_type_declaration(Type_engine::struct_type);
            params_factory.start_param_assignment();
        }
    }
}

void sv_visitor::exitData_declaration(sv2017::Data_declarationContext *ctx) {
    if (in_function_var_decl) {
        in_function_var_decl = false;
        auto var_name = ctx->list_of_variable_decl_assignments()
            ->variable_decl_assignment(0)->identifier()->getText();
        auto t = type_engine.finalize_type();
        type_engine.stop_range();
        if (!t) {
            t = Type_engine::create_primitive_type("implicit");
        }
        auto param = std::make_shared<HDL_parameter>(var_name);
        param->set_type(t);
        f_factory.add_local_variable(param);
        return;
    }
    if (ctx->type_declaration()) {
        auto name = ctx->type_declaration()->identifier(0)->getText();
        if (type_engine.is_simple_type()) {
            modules_factory.add_typedef(name, type_engine.stop_type_declaration(name));
        } else {;
            modules_factory.add_struct_def(name, type_engine.stop_composite_type_declaration(name, false));
        }
    } else {
        if (ctx->data_type_or_implicit() &&
            ctx->data_type_or_implicit()->data_type() &&
            ctx->data_type_or_implicit()->data_type()->struct_union()
        ) {
            in_anonymous_struct = false;
            auto name = ctx->list_of_variable_decl_assignments()
                ->variable_decl_assignment(0)->identifier()->getText();
            params_factory.set_type(pending_anon_struct_type);
            params_factory.new_parameter(name);
            params_factory.stop_param_assignment();
            auto param = params_factory.get_parameter();
            param->set_type(pending_anon_struct_type);
            modules_factory.add_parameter(param);
        }
    }
}

void sv_visitor::enterStruct_union_member(sv2017::Struct_union_memberContext *ctx) {
    type_engine.open_composite_member();
}

void sv_visitor::exitStruct_union_member(sv2017::Struct_union_memberContext *ctx) {
    auto name = ctx->list_of_variable_decl_assignments()->variable_decl_assignment(0)->identifier()->getText();
    type_engine.close_composite_member(name);
}

void sv_visitor::enterData_type_primitive(sv2017::Data_type_primitiveContext *ctx) {
    if (ctx->integer_type()) {
        auto it = ctx->integer_type();
        std::string base_type;
        if (it->integer_vector_type()) {
            base_type = it->integer_vector_type()->getText();
        } else if (it->integer_atom_type()) {
            base_type = it->integer_atom_type()->getText();
        } else {
            base_type = it->getText();
        }
        type_engine.set_type(base_type);
        if (ctx->signing()) {
            type_engine.set_member_signed(ctx->signing()->getText() == "signed");
        }
    }
}

void sv_visitor::enterData_type(sv2017::Data_typeContext *ctx) {
    auto packed =  ctx->KW_PACKED() != nullptr;
    if (type_engine.active() && packed) {
        type_engine.set_packed();
    }
}

void sv_visitor::exitData_type(sv2017::Data_typeContext *ctx) {
    type_engine.close_packed_dimensions();
    if (in_anonymous_struct && ctx->struct_union() && type_engine.active()) {
        pending_anon_struct_type = type_engine.stop_composite_type_declaration("", true);
    }
}

void sv_visitor::exitInterface_header(sv2017::Interface_headerContext *ctx) {
    std::string interface_name = ctx->identifier()->getText();
    if(modules_factory.is_current_valid()){
        hdl_instance_statement statement;
        statement.set_name("__scoped_declaration__");
        statement.set_type(interface_name);
        statement.set_dependency_class(interface);
        modules_factory.add_statement(std::make_shared<hdl_instance_statement>(statement));
    }

}

std::vector<std::shared_ptr<hdl_statement_base>> sv_visitor::get_entities() {
    return entities;
}

void sv_visitor::enterPrimaryTfCall(sv2017::PrimaryTfCallContext *ctx) {
    if(params_factory.is_component_relevant() || f_factory.is_active()){
        std::string call_name = ctx->any_system_tf_identifier()->getText();
        if (f_factory.is_active()) {
            // TODO: sort out calls in functions
        } else {
            params_factory.start_function_call(call_name);
        }
        if(ctx->data_type()){
            auto scoped_ctx = ctx->data_type()->package_or_class_scoped_path();
            if (scoped_ctx && !scoped_ctx->DOUBLE_COLON().empty()) {
                auto qi = sv_parsing_helpers::parse_qualified_identifier(scoped_ctx);
                auto ec = std::make_shared<Identifier_token>(qi);
                if (f_factory.is_active()) {
                    f_factory.add_component(ec);
                } else {
                    params_factory.add_component(ec);
                }
            } else{
                auto text = ctx->data_type()->getText();
                if (f_factory.is_active()) {
                    f_factory.add_component(sv_parsing_helpers::make_value(text));
                } else {
                    params_factory.add_component(sv_parsing_helpers::make_value(text), true);
                }

            }

        }
    }
}

void sv_visitor::enterCast_separator(sv2017::Cast_separatorContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.advance_cast();
    } else {
        params_factory.advance_cast();
    }
}

void sv_visitor::enterPrimaryCast2(sv2017::PrimaryCast2Context *ctx) {
    auto expression_size = ctx->primary()->getText().starts_with("(");
    if (f_factory.is_active()) {
        f_factory.start_cast(expression_size);
    } else {
        params_factory.start_cast(expression_size);
    }
}

void sv_visitor::exitPrimaryCast2(sv2017::PrimaryCast2Context *ctx) {
    if (f_factory.is_active()) {
        f_factory.stop_cast();
    } else {
        params_factory.stop_cast();
    }
}

void sv_visitor::enterPrimaryCast(sv2017::PrimaryCastContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.start_cast(false);
    } else {
        params_factory.start_cast(false);
    }
    std::string cast_type;

    if (ctx->signing()) {
        cast_type = ctx->signing()->getText();
    }else if (ctx->integer_type()) {
        cast_type = ctx->integer_type()->getText();
    } else if (ctx->non_integer_type()) {
        cast_type = ctx->non_integer_type()->getText();
    }
    if (f_factory.is_active()) {
        f_factory.set_cast_type(cast_type);
    } else {
        params_factory.set_cast_type(cast_type);
    }
}

void sv_visitor::exitPrimaryCast(sv2017::PrimaryCastContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.stop_cast();
    } else {
        params_factory.stop_cast();
    }
}

void sv_visitor::enterClass_declaration(sv2017::Class_declarationContext *ctx) {
    in_class = true;
}

void sv_visitor::exitClass_declaration(sv2017::Class_declarationContext *ctx) {
    in_class = false;
}


void sv_visitor::exitPrimaryTfCall(sv2017::PrimaryTfCallContext *ctx) {
    std::string call_name = ctx->any_system_tf_identifier()->getText();
    if(call_name=="$readmemh" || call_name=="$readmemb"){
        std::string data_file = ctx->list_of_arguments()->expression()[0]->getText();
        data_file.erase(std::remove(data_file.begin(), data_file.end(), '\\'), data_file.end());
        data_file.erase(std::remove(data_file.begin(), data_file.end(), '"'), data_file.end());
        std::filesystem::path p = data_file;
        auto ext = p.extension().string();
        if(ext == ".dat"|| ext == ".mem"){
            auto stmt = std::make_shared<hdl_instance_statement>();
            stmt->set_name("__init_file__");
            stmt->set_type(p.stem());
            stmt->set_dependency_class(memory_init);
            modules_factory.add_statement(stmt);
        }
    }
    if(params_factory.is_component_relevant()) {
        params_factory.stop_function_call();
    }
}

void sv_visitor::enterPackage_declaration(sv2017::Package_declarationContext *ctx) {
    size_t line_number = ctx->getStart()->getLine();
    auto package_name = ctx->identifier()[0]->getText();
    modules_factory.new_module(package_name, package, line_number);
}

void sv_visitor::exitPackage_declaration(sv2017::Package_declarationContext *ctx) {
    auto package = modules_factory.get_module();
    entities.push_back(package);
}

void sv_visitor::exitPackage_or_class_scoped_path(sv2017::Package_or_class_scoped_pathContext *ctx) {
    if (type_engine.active() && ctx->DOUBLE_COLON().empty()) {
        type_engine.set_type(ctx->getText());
    }
    if(!ctx->DOUBLE_COLON().empty()){
        auto qi = sv_parsing_helpers::parse_qualified_identifier(ctx);
        auto pkg_prefix = qi.get_package_prefix();
        std::string prefix_str = pkg_prefix.empty() ? "" : pkg_prefix.back();
        auto stmt = std::make_shared<hdl_instance_statement>();
        stmt->set_name(qi.get_name());
        stmt->set_type(prefix_str);
        stmt->set_dependency_class(package);
        modules_factory.add_statement(stmt);
    }
}

void sv_visitor::enterParameter_declaration(sv2017::Parameter_declarationContext *ctx) {
    if (ctx->list_of_type_assignments() ) return;
    in_param_declaration = true;
    if (!ctx->list_of_param_assignments()) {
        throw std::runtime_error("Encountered non existent list of parameter declarations");
    }
    auto resolved = setup_data_type(ctx->data_type_or_implicit());
    type_engine.start_range();
    params_factory.set_type(resolved);
    current_parameter = ctx->list_of_param_assignments()[0].param_assignment()[0]->identifier()->getText();
}

void sv_visitor::exitParameter_declaration(sv2017::Parameter_declarationContext *ctx) {
    params_factory.set_type(std::make_shared<HDL_simple_type>() );
    in_param_declaration = false;
}

void sv_visitor::enterExpression(sv2017::ExpressionContext *ctx) {
    if (loops_factory.in_loop() && loops_factory.in_body()) {
        loops_factory.start_expression(ctx->primary() == nullptr);
    }
    if(type_engine.active() || type_engine.is_ranging()){
        type_engine.start_expression();
    } else if(params_factory.is_component_relevant()|| params_factory.is_param_assignment() || params_factory.is_param_override()) {
        if (auto primary = dynamic_cast<sv2017::PrimaryAssigContext*>(ctx->primary())) {
            if (primary->assignment_pattern_expression()) return;
        }
        params_factory.start_expression_new(ctx->primary() == nullptr);
        if(ctx->QUESTIONMARK()){
            params_factory.start_ternary_operator();
        }
    } else if (f_factory.is_active()) {
            f_factory.start_expression();
    }
    if (deps_factory.is_valid_dependency()) {
        deps_factory.start_expression(ctx->primary() == nullptr);
    }
}

void sv_visitor::exitExpression(sv2017::ExpressionContext *ctx) {
    if (loops_factory.in_loop() && loops_factory.in_body()) {
        loops_factory.stop_expression(ctx->primary() == nullptr);
    }
    if (type_engine.active() || type_engine.is_ranging()) {
        type_engine.stop_expression();
    } else if (params_factory.is_component_relevant() || params_factory.is_param_assignment() || params_factory.is_param_override()) {
        if (auto primary = dynamic_cast<sv2017::PrimaryAssigContext*>(ctx->primary())) {
            if (primary->assignment_pattern_expression()) return;
        }
        std::string type;
        if(ctx->QUESTIONMARK()){
            params_factory.stop_ternary();
        }
        params_factory.stop_expression_new(ctx->primary() == nullptr);
    }else if (f_factory.is_active()) {
        f_factory.stop_expression();
    }
    if (deps_factory.is_valid_dependency()) {
        deps_factory.stop_expression(ctx->primary() == nullptr);
    }
}

void sv_visitor::exitPrimaryLit(sv2017::PrimaryLitContext *ctx) {
    auto text = ctx->getText();
    route_expression_text(text);
    if(deps_factory.is_valid_dependency() && deps_factory.is_in_port()) {
        deps_factory.start_scalar_net(text);
    }
}

void sv_visitor::enterPrimaryPath(sv2017::PrimaryPathContext *ctx) {
    auto dbg = ctx->getText();
    if(deps_factory.is_valid_dependency()){
        if(!deps_factory.is_interface()) {
            deps_factory.add_connection_element(ctx->getText());
            if(deps_factory.is_in_port()) {
                deps_factory.start_scalar_net(ctx->getText());
            }
        }
    }
}


void sv_visitor::exitPrimaryPath(sv2017::PrimaryPathContext *ctx) {
    std::shared_ptr<Expression_base> ec;

    auto scoped_ctx = ctx->package_or_class_scoped_path();
    if (scoped_ctx && !scoped_ctx->DOUBLE_COLON().empty()) {
        auto qi = sv_parsing_helpers::parse_qualified_identifier(scoped_ctx);
        ec = std::make_shared<Identifier_token>(qi);
    } else {
        std::vector<std::string> dot_chain;
        auto node = ctx->parent;
        while (node) {
            if (auto dot = dynamic_cast<sv2017::PrimaryDotContext *>(node)) {
                dot_chain.push_back(dot->identifier()->getText());
                node = dot->parent;
            } else {
                break;
            }
        }
        if (dot_chain.empty()) {
            ec = sv_parsing_helpers::make_value(ctx->getText());
        } else {
            auto leaf = ctx->package_or_class_scoped_path()->getText();
            qualified_identifier qi(dot_chain.back());
            std::vector<std::string> instance = {leaf};
            for (size_t i = 0; i < dot_chain.size() - 1; ++i)
                instance.push_back(dot_chain[i]);
            qi.set_instance_prefix(instance);
            ec = std::make_shared<Identifier_token>(qi);
        }
    }

    route_expression_component(ec);
}

void sv_visitor::process_operation(Expression_v2::expression_operator op) {
    params_factory.set_operation(op);
    type_engine.set_operation(op);
    if (loops_factory.in_loop()) loops_factory.set_operation(op);
    if (f_factory.is_active()) f_factory.set_operation(op);
    if (deps_factory.is_valid_dependency()) deps_factory.set_operation(op);
}

void sv_visitor::exitOperator_plus_minus(sv2017::Operator_plus_minusContext *ctx) {

    if (ctx->PLUS())  process_operation(Expression_v2::expression_operator::add);
    if (ctx->MINUS()) process_operation(Expression_v2::expression_operator::subtract);
}

void sv_visitor::exitOperator_mul_div_mod(sv2017::Operator_mul_div_modContext *ctx) {

    if (ctx->DIV()) process_operation(Expression_v2::expression_operator::divide);
    if (ctx->MOD()) process_operation(Expression_v2::expression_operator::modulo);
    if (ctx->MUL()) process_operation(Expression_v2::expression_operator::multiply);

}


void sv_visitor::exitOperator_shift(sv2017::Operator_shiftContext *ctx) {

    if (ctx->SHIFT_LEFT()) process_operation(Expression_v2::expression_operator::logic_shift_left);
    if (ctx->SHIFT_RIGHT()) process_operation(Expression_v2::expression_operator::logic_shift_right);
    if (ctx->ARITH_SHIFT_LEFT()) process_operation(Expression_v2::expression_operator::arithmetic_shift_left);
    if (ctx->ARITH_SHIFT_RIGHT()) process_operation(Expression_v2::expression_operator::arithmetic_shift_right);
}

void sv_visitor::exitUnary_operator(sv2017::Unary_operatorContext *ctx) {

    if (ctx->PLUS()) process_operation(Expression_v2::expression_operator::add);
    if (ctx->MINUS()) process_operation(Expression_v2::expression_operator::subtract);

}

void sv_visitor::exitOperator_cmp(sv2017::Operator_cmpContext *ctx) {

    if (ctx->GT()) process_operation(Expression_v2::expression_operator::greater);
    if (ctx->GE()) process_operation(Expression_v2::expression_operator::greater_equal);
    if (ctx->LT())  process_operation(Expression_v2::expression_operator::less);
    if (ctx->LE()) process_operation(Expression_v2::expression_operator::less_equal);
}

void sv_visitor::exitOperator_eq_neq(sv2017::Operator_eq_neqContext *ctx) {
    if (ctx->EQ()) process_operation(Expression_v2::expression_operator::equal);
    if (ctx->NE()) process_operation(Expression_v2::expression_operator::not_equal);
}

void sv_visitor::exitOperator_bitwise_and(sv2017::Operator_bitwise_andContext *ctx) {
    process_operation(Expression_v2::expression_operator::bitwise_and);
}

void sv_visitor::exitOperator_bitwise_or(sv2017::Operator_bitwise_orContext *ctx) {
    process_operation(Expression_v2::expression_operator::bitwise_or);
}

void sv_visitor::exitOperator_xor(sv2017::Operator_xorContext *ctx) {

    if (ctx->XOR()) process_operation(Expression_v2::expression_operator::bitwise_xor);
    if (ctx->XORN()|| ctx->NXOR()) process_operation(Expression_v2::expression_operator::bitwise_xnor);

}

void sv_visitor::exitOperator_power(sv2017::Operator_powerContext *ctx) {
    process_operation(Expression_v2::expression_operator::power);
}

void sv_visitor::exitOperator_log_and(sv2017::Operator_log_andContext *ctx) {
    process_operation(Expression_v2::expression_operator::logical_and);
};
void sv_visitor::exitOperator_log_or(sv2017::Operator_log_orContext *ctx) {
    process_operation(Expression_v2::expression_operator::logical_or);
};


uint32_t sv_visitor::parse_number(const std::string& s) {
    if(auto m = ctre::match<R"(\d*'h([0-9a-fA-F]*))">(s)) {
        return std::stoul(m.get<1>().str(),nullptr, 16);
    } else if(auto m = ctre::match<R"(^(?:\d*'d)?([0-9]*)$)">(s)) {
        return std::stoul(m.get<1>().str(),nullptr, 10);
    } else if(auto m = ctre::match<R"(^\d*'o([0-7]*)$)">(s)) {
        return std::stoul(m.get<1>().str(),nullptr, 8);
    } else if(auto m = ctre::match<R"(\d*'b([0-1]*))">(s)) {
        return std::stoul(m.get<1>().str(),nullptr, 2);
    }
    return 0;
}

void sv_visitor::exitAnsi_port_declaration(sv2017::Ansi_port_declarationContext *ctx) {
    if(current_declaration_type == "module"){
        std::string port_name = ctx->port_identifier()->getText();
        HDL_port port;
        port.direction = raw_port;
        if(!ctx->port_direction()){
            if(ctx->DOT()){
                port.direction = interface_port;
                if(ctx->identifier().size() >= 2) {
                    port.if_info.type =ctx->identifier(0)->getText();
                    port.if_info.modport = ctx->identifier(1)->getText();
                }
            } else if(ctx->net_or_var_data_type()){
                port.direction  = interface_port;
                port.if_info.type = ctx->net_or_var_data_type()->getText();
            } else{
                port.direction = raw_port;
            }
        } else{
            std::string dir_s = ctx->port_direction()->getText();
            if(dir_s=="input")
                port.direction = input_port;
            else if(dir_s=="output")
                port.direction = output_port;
            else if(dir_s=="inout")
                port.direction = inout_port;
        }
        modules_factory.add_port(port_name, port);
    }
}

void sv_visitor::enterNamed_port_connection(sv2017::Named_port_connectionContext *ctx) {
    if(deps_factory.is_valid_dependency()){
        if(ctx->port_concatenation_connection()){
            deps_factory.start_concat_port(ctx->identifier()->getText());
        }
        if(ctx->port_replication_connection()) {
            deps_factory.start_replication_port(ctx->identifier()->getText());
        }
        deps_factory.start_port();
    }
}

void sv_visitor::exitNamed_port_connection(sv2017::Named_port_connectionContext *ctx) {
    if (!ctx->identifier()) {
        if (ctx->MUL()) {
            deps_factory.set_wildcard(true);
            return;
        }
        else throw std::runtime_error("Encountered a named port connection without an identifier, this is not supported yet");
    }
    auto port_name = ctx->identifier()->getText();

    deps_factory.stop_port();
    if(ctx->port_expression_connection()){\
        if(deps_factory.is_valid_dependency()){
            deps_factory.add_port(ctx->identifier()->getText());
        }
    }
    if(ctx->port_concatenation_connection()){
        if(deps_factory.is_valid_dependency()){
            deps_factory.stop_concat_port();
        }
    }
    if(ctx->port_replication_connection()){
        if(deps_factory.is_valid_dependency()){
            deps_factory.add_port(ctx->identifier()->getText());
        }
    }
}

void sv_visitor::enterNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *ctx) {
    params_factory.start_instance_parameter_assignment(ctx->identifier()->getText());
    params_factory.start_param_override();
}

void sv_visitor::exitNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *ctx) {
    params_factory.stop_param_override();
    auto param = params_factory.get_parameter();
    auto t = type_engine.finalize_dimensions();
    param->set_type(t);
    if(deps_factory.is_valid_dependency()){
        deps_factory.add_parameter(param);
    }
}

void sv_visitor::enterParam_assignment(sv2017::Param_assignmentContext *ctx) {
    auto p_n = ctx->identifier()->getText();
    params_factory.start_param_assignment();
    params_factory.new_parameter(p_n);
}


void sv_visitor::exitParam_assignment(sv2017::Param_assignmentContext *ctx) {
    params_factory.stop_param_assignment();


    auto p_n = ctx->identifier()->getText();
    if(params_factory.in_packed_context()) {
        params_factory.stop_packed_assignment();
    }else if(ctx->replication_assignment()){
        params_factory.start_packed_assignment();
        params_factory.stop_packed_assignment();
    } else {
        if (ctx->constant_param_expression()) {
            auto val = ctx->constant_param_expression()->getText();
            params_factory.add_component(sv_parsing_helpers::make_value(val));
        }
    }
    if (!in_class) {
        auto param = params_factory.get_parameter();
        auto t = type_engine.finalize_type();
        if (!t) {
            t = Type_engine::create_primitive_type("implicit");
        }
        param->set_type(t);
        if(modules_factory.is_current_valid()){
            modules_factory.add_parameter(param);
        } else if(interfaces_factory.is_current_valid()){
            interfaces_factory.add_parameter(param);
        }
    }
}


void sv_visitor::enterAssignment_pattern(sv2017::Assignment_patternContext *ctx) {
    if (!ctx->replication_assignment()) params_factory.start_initialization_list();
}

void sv_visitor::exitAssignment_pattern(sv2017::Assignment_patternContext *ctx) {
    bool default_assignment = false;
    if(!ctx->structure_pattern_key().empty()){
        if(ctx->structure_pattern_key()[0]->assignment_pattern_key()){
            if(ctx->structure_pattern_key()[0]->assignment_pattern_key()->KW_DEFAULT()){
                default_assignment = true;
            }
        }
    }

    if (!ctx->replication_assignment()) params_factory.stop_initialization_list(default_assignment);
}


void sv_visitor::exitPrimaryBitSelect(sv2017::PrimaryBitSelectContext *ctx) {
    params_factory.close_array_index();
}


void sv_visitor::exitPrimaryIndex(sv2017::PrimaryIndexContext *ctx) {
    if(deps_factory.is_valid_dependency()){
        deps_factory.add_connection_element(ctx->getText());
    }
}

void sv_visitor::enterPrimaryDot(sv2017::PrimaryDotContext *ctx) {
    if(deps_factory.is_valid_dependency()) {
        deps_factory.start_interface();
        if(!deps_factory.in_concatenation() || deps_factory.is_in_replication()) {
            if(deps_factory.is_in_port()) {
                deps_factory.start_scalar_net(ctx->getText());
            }
        }
    }
    if(params_factory.is_component_relevant()|| f_factory.is_active()) {
        instance_prefix = ctx->primary()->getText();
        instance_item = ctx->identifier()->getText();
    }

}

void sv_visitor::exitPrimaryDot(sv2017::PrimaryDotContext *ctx) {
    if(deps_factory.is_valid_dependency()){
        deps_factory.add_connection_element(ctx->getText());
        deps_factory.stop_interface();
    }
}

void sv_visitor::enterReplication_value(sv2017::Replication_valueContext *ctx) {
    if(deps_factory.is_valid_dependency() && deps_factory.is_in_replication()) {
        deps_factory.advance_replication();
    }
}

void sv_visitor::enterPrimaryCall(sv2017::PrimaryCallContext *ctx) {
    if(params_factory.is_component_relevant()) {
        std::string call_text = ctx->primary()->getText();
        auto pos = call_text.find("::");
        if (pos != std::string::npos) {
            std::string prefix = call_text.substr(0, pos);
            std::string func_name = call_text.substr(pos + 2);
            params_factory.start_function_assignment(func_name);
            params_factory.set_function_package_prefix(prefix);
        } else {
            params_factory.start_function_assignment(call_text);
        }
    }
}


void sv_visitor::exitPrimaryCall(sv2017::PrimaryCallContext *ctx) {
    if(params_factory.is_component_relevant()) {
        params_factory.stop_function_assignment();
    }
}

void sv_visitor::enterConstant_param_expression(sv2017::Constant_param_expressionContext *ctx) {
    type_engine.stop_range();
    if(ctx->concatenation()){
        params_factory.start_packed_assignment();
    }
}

void sv_visitor::enterBit_select(sv2017::Bit_selectContext *ctx) {
    if (loops_factory.in_loop() && loops_factory.in_body()) {
        loops_factory.start_bit_selection();
    }
    if (f_factory.is_active()) {
        f_factory.start_bit_selection();
    } else {
        params_factory.start_bit_selection();
    }

    deps_factory.start_bit_selection();
}

void sv_visitor::exitBit_select(sv2017::Bit_selectContext *ctx) {
    if (loops_factory.in_loop() && loops_factory.in_body()) {
        loops_factory.stop_bit_selection();
    }
    if (f_factory.is_active()) {
        f_factory.stop_bit_selection();
    } else {
        params_factory.stop_bit_selection();
    }

    deps_factory.stop_bit_selection();
}

void sv_visitor::exitFirst_range_identifier(sv2017::First_range_identifierContext *ctx) {
    type_engine.advance_range();
}


void sv_visitor::exitRange_separator(sv2017::Range_separatorContext *ctx) {
    if(deps_factory.is_valid_dependency()) {
        if(ctx->PLUS()) {
            deps_factory.advance_array_range_phase( "+");
        } else if(ctx->MINUS()) {
            deps_factory.advance_array_range_phase( "-");
        } else {
            deps_factory.advance_array_range_phase("");
        }
    }
    type_engine.advance_range();
}

void sv_visitor::enterRange_expression(sv2017::Range_expressionContext *ctx) {
    type_engine.open_range();
}

void sv_visitor::exitRange_expression(sv2017::Range_expressionContext *ctx) {
   type_engine.close_range();
}

void sv_visitor::enterArray_range_expression(sv2017::Array_range_expressionContext *ctx) {
    type_engine.open_range();
    if(deps_factory.is_valid_dependency()) {
        deps_factory.start_array_range();
    }
}

void sv_visitor::exitArray_range_expression(sv2017::Array_range_expressionContext *ctx) {
    type_engine.close_range();
    if(deps_factory.is_valid_dependency()) {
        deps_factory.stop_array_range();
    }
}

void sv_visitor::enterUnpacked_dimension(sv2017::Unpacked_dimensionContext *ctx) {
    type_engine.close_packed_dimensions();
    type_engine.start_unpacked_dimension_declaration();
}


void sv_visitor::exitConcatenation_item(sv2017::Concatenation_itemContext *ctx) {
    if(deps_factory.is_valid_dependency()) {
        deps_factory.add_concatenation_net();
    }
}


void sv_visitor::enterReplication(sv2017::ReplicationContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.start_replication();
    } else {
        params_factory.start_replication();
    }
    if(deps_factory.is_valid_dependency()) {
        deps_factory.start_replication();
    }
}



void sv_visitor::exitReplication(sv2017::ReplicationContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.stop_replication();
    } else {
        params_factory.stop_replication();
    }
    if(deps_factory.is_valid_dependency()) {
        deps_factory.stop_replication();
    }
}

void sv_visitor::enterReplication_assignment(sv2017::Replication_assignmentContext *ctx) {
    type_engine.stop_range();
    params_factory.start_replication_assignment();
}

void sv_visitor::exitReplication_assignment(sv2017::Replication_assignmentContext *ctx) {
    params_factory.stop_replication_assignment();
}

void sv_visitor::enterConcatenation(sv2017::ConcatenationContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.start_concat();
    } else {
        params_factory.start_concatenation();
    }
}

void sv_visitor::exitConcatenation(sv2017::ConcatenationContext *ctx) {
    if(f_factory.is_active()){
        f_factory.stop_concat();
    } else {
        params_factory.stop_concatenation();
    }
}


void sv_visitor::exitData_type_or_implicit(sv2017::Data_type_or_implicitContext *ctx) {
    if(!in_param_declaration) {
        params_factory.clear_expression();
    }
}

void sv_visitor::enterLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) {
    in_param_declaration = true;
    auto resolved = setup_data_type(ctx->data_type_or_implicit());
    type_engine.start_range();
    params_factory.set_type(resolved);
}

void sv_visitor::exitLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) {
    params_factory.set_type(std::make_shared<HDL_simple_type>() );
    in_param_declaration = false;
}

void sv_visitor::enterLoop_generate_construct(sv2017::Loop_generate_constructContext *) {
    loops_factory.new_loop();
}

void sv_visitor::exitLoop_generate_construct(sv2017::Loop_generate_constructContext *) {
    modules_factory.add_statement(loops_factory.get_loop_statement());
}

void sv_visitor::enterGenvar_initialization(sv2017::Genvar_initializationContext *ctx) {
    std::string id = ctx->identifier()->getText();
    loops_factory.set_phase(HDL_loops_factory::init);
    params_factory.start_param_assignment();
    params_factory.new_parameter(id);
}

void sv_visitor::exitGenvar_initialization(sv2017::Genvar_initializationContext *ctx) {
    auto param = params_factory.get_parameter();
    params_factory.stop_param_assignment();
    loops_factory.set_loop_init(*param);
    loops_factory.advance_phase();
}

void sv_visitor::enterGenvar_expression(sv2017::Genvar_expressionContext *ctx) {
    params_factory.start_param_assignment();
    params_factory.new_parameter("genvar_expr");
}

void sv_visitor::exitGenvar_expression(sv2017::Genvar_expressionContext *ctx) {
    loops_factory.advance_phase();
    auto param = params_factory.get_parameter();
    auto ex = param->get_expression();
    if (ex) {
        if (ex->is<Expression_v2>()) {
            loops_factory.add_expression(ex->as<Expression_v2>());
        } else {
            Expression_v2 e;
            e.set_lhs(ex);
            loops_factory.add_expression(e);
        }
    }
    params_factory.stop_param_assignment();
}

void sv_visitor::enterUntyped_function_declaration(sv2017::Untyped_function_declarationContext *ctx) {
    auto name = ctx->task_and_function_declaration_common()->identifier()[0]->getText();
    f_factory.set_name(name);
}

void sv_visitor::exitFunction_declaration(sv2017::Function_declarationContext *ctx) {
    auto func_type = ctx->function_data_type_or_implicit();
    if (func_type) {
        auto dv = func_type->data_type_or_void();
        if (dv && !dv->KW_VOID()) {
            auto dt = dv->data_type();
            if (dt && dt->package_or_class_scoped_path()) {
                f_factory.set_return_type_name(dt->package_or_class_scoped_path()->getText());
            }
        }
    }
    auto func = f_factory.get_function();
    if (modules_factory.is_current_valid()) {
        modules_factory.add_function(func);
    } else {
        entities.push_back(std::make_shared<hdl_function_statement>(func));
    }
}

void sv_visitor::enterLoop_statement(sv2017::Loop_statementContext *ctx) {
    if(f_factory.is_active()) {
        f_factory.pause();
        loops_factory.new_loop();
    }
}


void sv_visitor::exitLoop_statement(sv2017::Loop_statementContext *ctx) {
    if(f_factory.is_active()) {
        f_factory.add_loop(loops_factory.get_loop_statement());
        loops_factory.clear();
        f_factory.resume();
    }
}

void sv_visitor::exitStatement_item(sv2017::Statement_itemContext *ctx) {
    if(f_factory.is_active() && loops_factory.in_loop()) {
        loops_factory.close_expression();
    }
}

void sv_visitor::exitAssignment_operator(sv2017::Assignment_operatorContext *ctx) {
    if(f_factory.is_active() && loops_factory.in_loop()) {
        loops_factory.advance_expression();
    }
}


void sv_visitor::enterFor_initialization(sv2017::For_initializationContext *ctx) {
    if(f_factory.is_active()) {
        loops_factory.set_phase(HDL_loops_factory::init);
        if(!ctx->for_variable_declaration().empty()) {
            auto decl = ctx->for_variable_declaration()[0]->for_variable_declaration_var_assign();
            loops_factory.add_loop_variable(decl[0]->identifier()->getText());
        }
    }
}

void sv_visitor::exitFor_initialization(sv2017::For_initializationContext *ctx) {
    sv2017BaseListener::exitFor_initialization(ctx);
}

void sv_visitor::enterFor_end_expression(sv2017::For_end_expressionContext *ctx) {
    if(f_factory.is_active()) {
        loops_factory.set_phase(HDL_loops_factory::end);
        params_factory.start_param_assignment();
        params_factory.new_parameter("for_end_expr");
    }
}

void sv_visitor::exitFor_end_expression(sv2017::For_end_expressionContext *ctx) {
    if(f_factory.is_active()) {
        auto param = params_factory.get_parameter();
        auto ex = param->get_expression();
        if (ex->is<Expression_v2>()) {
            loops_factory.add_expression(ex->as<Expression_v2>());
        } else {
            Expression_v2 e;
            e.set_lhs(ex);
            loops_factory.add_expression(e);
        }
        params_factory.stop_param_assignment();
    }
}

void sv_visitor::enterFor_step(sv2017::For_stepContext *ctx) {
    if(f_factory.is_active()) {
        loops_factory.set_phase(HDL_loops_factory::step);
    }
}

void sv_visitor::exitFor_step(sv2017::For_stepContext *ctx) {
    if(f_factory.is_active()) {
        loops_factory.set_phase(HDL_loops_factory::body);
    }
}

void sv_visitor::enterInc_or_dec_expressionPost(sv2017::Inc_or_dec_expressionPostContext *ctx) {
    if(f_factory.is_active()) {
        if(loops_factory.in_definition()) {
            auto name = ctx->variable_lvalue()->getText();
            loops_factory.add_component(sv_parsing_helpers::make_value(name));
            if(ctx->inc_or_dec_operator()->INCR()){
                loops_factory.set_operation(Expression_v2::add);
            } else if(ctx->inc_or_dec_operator()->DECR()){
                loops_factory.set_operation(Expression_v2::subtract);
            }
            loops_factory.add_component(std::make_shared<Numeric_token>("1"));
        }
    }
}

void sv_visitor::exitBlocking_assignment(sv2017::Blocking_assignmentContext *ctx) {
    if(!loops_factory.in_loop() && f_factory.is_active()) {
        f_factory.finish_assignment();
    }

}

void sv_visitor::enterVariable_lvalue(sv2017::Variable_lvalueContext *ctx) {
    if(f_factory.is_active()) {
        auto var_name = ctx->package_or_class_scoped_hier_id_with_select()->package_or_class_scoped_path()->getText();
        if(loops_factory.in_loop()) {
            loops_factory.start_assignment(var_name);
            if (loops_factory.in_body()) {
                auto var_token = sv_parsing_helpers::make_value(var_name);
                loops_factory.add_component(var_token);
            }
        } else {
            f_factory.start_assignment(var_name);
        }

    }
}

void sv_visitor::exitVariable_lvalue(sv2017::Variable_lvalueContext *ctx) {
    if(f_factory.is_active() && !loops_factory.in_loop()) {
        f_factory.close_lvalue();
    }
}


void sv_visitor::exitGenvar_iteration(sv2017::Genvar_iterationContext *ctx) {
   if(ctx->inc_or_dec_operator()) {
       auto str = ctx->identifier()->getText();
       loops_factory.add_component(sv_parsing_helpers::make_value(str));
       if(ctx->inc_or_dec_operator()->INCR()){
           loops_factory.set_operation(Expression_v2::add);
       } else if(ctx->inc_or_dec_operator()->DECR()){
           loops_factory.set_operation(Expression_v2::subtract);
       }
       loops_factory.add_component(std::make_shared<Numeric_token>("1"));
   }
    if (!ctx->genvar_expression()) loops_factory.advance_phase();
}





