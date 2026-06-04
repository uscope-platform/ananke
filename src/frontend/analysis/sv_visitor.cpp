// Copyright 2021 University of Nottingham Ningbo China
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "frontend/analysis/sv_visitor.hpp"

sv_visitor::sv_visitor(std::string p) {
    path = std::move(p);
}

void sv_visitor::route_expression_component(const std::string& text) {
    Expression_component ec(text, Expression_component::get_type(text));
    if(loops_factory.in_loop()) {
        loops_factory.add_component(ec);
    }
    if(params_factory.is_component_relevant()){
        params_factory.add_component(ec);
    }
    if (f_factory.is_active()) {
        f_factory.add_component(ec);
    }
    if(deps_factory.is_valid_dependency()){
        deps_factory.add_connection_element(text);
    }
}

void sv_visitor::route_expression_component(const Expression_component& ec) {
    if(loops_factory.in_loop()) {
        loops_factory.add_component(ec);
    }
    if (f_factory.is_active()) {
        f_factory.add_component(ec);
    }
    if(params_factory.is_component_relevant()){
        params_factory.add_component(ec);
    }
}


void sv_visitor::enterModule_declaration(sv2017::Module_declarationContext *ctx) {
    current_declaration_type = "module";
    size_t line_number = ctx->getStart()->getLine();
    auto module_name = ctx->module_header_common()->identifier()->getText();
    modules_factory.new_module(module_name, path, module, line_number);
}


void sv_visitor::exitModule_declaration(sv2017::Module_declarationContext *ctx) {
    entities.push_back(modules_factory.get_module());
}

void sv_visitor::enterInterface_declaration(sv2017::Interface_declarationContext *ctx) {
    current_declaration_type = "interface";
    size_t line_number = ctx->getStart()->getLine();
    std::string interface_name = ctx->interface_header()->identifier()->getText();
    interfaces_factory.new_interface(interface_name, path, line_number);
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
        auto dep = deps_factory.get_dependency();
         loops_factory.add_instance(dep);
    } else {
        modules_factory.add_instance(deps_factory.get_dependency());
    }

}

void sv_visitor::enterName_of_instance(sv2017::Name_of_instanceContext *ctx) {
    if(!ctx->unpacked_dimension().empty()){
        params_factory.start_param_assignment();
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

void sv_visitor::enterType_declaration(sv2017::Type_declarationContext *ctx) {
    params_factory.start_type_declaration();
}

void sv_visitor::exitType_declaration(sv2017::Type_declarationContext *ctx) {
    auto name = ctx->identifier(0)->getText();
    modules_factory.add_typedef(name,  params_factory.stop_type_declaration(name));
}

void sv_visitor::exitData_type(sv2017::Data_typeContext *ctx) {
    params_factory.close_packed_dimensions();
}

void sv_visitor::exitInterface_header(sv2017::Interface_headerContext *ctx) {
    std::string interface_name = ctx->identifier()->getText();
    if(modules_factory.is_current_valid()){
        HDL_instance dep("__scoped_declaration__", interface_name, interface);
        modules_factory.add_instance(dep);
    }

}

std::vector<HDL_Resource> sv_visitor::get_entities() {
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
            if(!package_prefix.empty()){
                Expression_component ec(package_item, Expression_component::get_type(package_item));
                ec.set_package_prefix(package_prefix);
                if (f_factory.is_active()) {
                    f_factory.add_component(ec);
                }else {
                    params_factory.add_component(ec);
                }
                package_prefix.clear();
                package_item.clear();
            } else{
                auto text = ctx->data_type()->getText();
                if (f_factory.is_active()) {
                    f_factory.add_component(Expression_component(text, Expression_component::get_type(text)));
                } else {
                    params_factory.add_component(Expression_component(text, Expression_component::get_type(text)), true);
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
        data_file = std::regex_replace(data_file, std::regex("\\\""), "");
        std::filesystem::path p = data_file;
        auto ext = p.extension().string();
        if(ext == ".dat"|| ext == ".mem"){
            HDL_instance dep("__init_file__", p.stem(), memory_init);
            modules_factory.add_instance(dep);
        }
    }
    if(params_factory.is_component_relevant()) {
        params_factory.stop_function_call();
    }
}

void sv_visitor::enterPackage_declaration(sv2017::Package_declarationContext *ctx) {
    size_t line_number = ctx->getStart()->getLine();
    auto package_name = ctx->identifier()[0]->getText();
    modules_factory.new_module(package_name, path, package, line_number);
}

void sv_visitor::exitPackage_declaration(sv2017::Package_declarationContext *ctx) {
    auto package = modules_factory.get_module();
    entities.push_back(package);
}

void sv_visitor::exitPackage_or_class_scoped_path(sv2017::Package_or_class_scoped_pathContext *ctx) {
    if(!ctx->DOUBLE_COLON().empty()){
        package_prefix = ctx->package_or_class_scoped_path_item()[0]->identifier()->getText();
        package_item = ctx->package_or_class_scoped_path_item()[1]->identifier()->getText();
        HDL_instance dep(package_item, package_prefix, package);
        modules_factory.add_instance(dep);
    }
}

void sv_visitor::enterParameter_declaration(sv2017::Parameter_declarationContext *ctx) {
    if (ctx->list_of_type_assignments() ) return;
    in_param_declaration = true;
    if (!ctx->list_of_param_assignments()) {
        throw std::runtime_error("Encountered non existent list of parameter declarations");
    }
    if (ctx->data_type_or_implicit() && ctx->data_type_or_implicit()->data_type()) {
        std::string type;
        if (ctx->data_type_or_implicit()->data_type()->data_type_primitive())
            type = ctx->data_type_or_implicit()->data_type()->data_type_primitive()->getText();
        if (ctx->data_type_or_implicit()->data_type()->package_or_class_scoped_path())
            type = ctx->data_type_or_implicit()->data_type()->package_or_class_scoped_path()->getText();
        params_factory.set_type(type);
    }
    if (!ctx->data_type_or_implicit()) {
        params_factory.set_type("implicit");
    }
    params_factory.start_range();
    current_parameter = ctx->list_of_param_assignments()[0].param_assignment()[0]->identifier()->getText();
}

void sv_visitor::exitParameter_declaration(sv2017::Parameter_declarationContext *ctx) {

    params_factory.set_type("");
    in_param_declaration = false;
}

void sv_visitor::enterExpression(sv2017::ExpressionContext *ctx) {
    if(params_factory.is_component_relevant()|| params_factory.is_param_assignment() || params_factory.is_param_override()) {
        params_factory.start_expression_new();
        if(ctx->QUESTIONMARK()){
            params_factory.start_ternary_operator();
        }
    } else if (f_factory.is_active()) {
            f_factory.start_expression();
    }

}

void sv_visitor::exitExpression(sv2017::ExpressionContext *ctx) {
    if (params_factory.is_component_relevant() || params_factory.is_param_assignment() || params_factory.is_param_override()) {
        std::string type;
        if(ctx->QUESTIONMARK()){
            params_factory.stop_ternary();
        }
        params_factory.stop_expression_new();
    }else if (f_factory.is_active()) {
        f_factory.stop_expression();
    }

}

void sv_visitor::exitPrimaryLit(sv2017::PrimaryLitContext *ctx) {
    auto text = ctx->getText();
    route_expression_component(text);
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
    Expression_component ec;

    if(!package_prefix.empty()) {
        ec = Expression_component(package_item, Expression_component::get_type(package_item));
        ec.set_package_prefix(package_prefix);
        package_prefix.clear();
        package_item.clear();
    } else if (!instance_prefix.empty()){
        ec = Expression_component(instance_item, Expression_component::get_type(instance_item));
        ec.set_instance_prefix(instance_prefix);
        instance_item.clear();
        instance_prefix.clear();
    } else {
        ec = Expression_component(ctx->getText(), Expression_component::get_type(ctx->getText()));
    }

    route_expression_component(ec);
}

void sv_visitor::exitOperator_plus_minus(sv2017::Operator_plus_minusContext *ctx) {
    route_expression_component(ctx->getText());
}

void sv_visitor::exitOperator_mul_div_mod(sv2017::Operator_mul_div_modContext *ctx) {
    route_expression_component(ctx->getText());
}


void sv_visitor::exitOperator_shift(sv2017::Operator_shiftContext *ctx) {
    route_expression_component(ctx->getText());
}

void sv_visitor::exitUnary_operator(sv2017::Unary_operatorContext *ctx) {
    if(ctx->PLUS() || ctx->MINUS()){
        route_expression_component(ctx->getText());
    }
}

void sv_visitor::exitOperator_cmp(sv2017::Operator_cmpContext *ctx) {
    route_expression_component(ctx->getText());
}

void sv_visitor::exitOperator_eq_neq(sv2017::Operator_eq_neqContext *ctx) {
    route_expression_component(ctx->getText());
}


uint32_t sv_visitor::parse_number(const std::string& s) {
    std::regex hex_number(R"(\d*'h([0-9a-fA-F]*))");
    std::regex dec_number(R"(^(?:\d*'d)?([0-9]*)$)");
    std::regex oct_number(R"(^\d*'o([0-7]*)$)");
    std::regex bin_number(R"(\d*'b([0-1]*))");

    std::smatch sm;
    if(std::regex_match(s,sm, hex_number)){
        return std::stoul(sm[1],nullptr, 16);
    } else if(std::regex_match(s,sm, dec_number)){
        return std::stoul(sm[1],nullptr, 10);
    } else if(std::regex_match(s,sm, oct_number)){
        return std::stoul(sm[1],nullptr, 8);
    } else if(std::regex_match(s,sm, bin_number)){
        return std::stoul(sm[1],nullptr, 2);
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
        throw std::runtime_error("Encountered a named port connection without an identifier, this is not supported yet");
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
        if(!package_prefix.empty()){
            Expression_component ec(package_item, Expression_component::get_type(package_item));
            ec.set_package_prefix(package_prefix);
            params_factory.add_component(ec);
            package_prefix.clear();
            package_item.clear();
        } else{
            if (ctx->constant_param_expression()) {
                auto val = ctx->constant_param_expression()->getText();
                params_factory.add_component(Expression_component(val, Expression_component::get_type(val)));
            }
        }
    }
    if (!in_class) {
        if(modules_factory.is_current_valid()){
            auto param = params_factory.get_parameter();
            modules_factory.add_parameter(param);
        } else if(interfaces_factory.is_current_valid()){
            interfaces_factory.add_parameter(params_factory.get_parameter());
        }
    }


}


void sv_visitor::enterPrimaryPar(sv2017::PrimaryParContext *ctx) {
    params_factory.add_component(Expression_component("(", Expression_component::parenthesis));
}

void sv_visitor::exitPrimaryPar(sv2017::PrimaryParContext *ctx) {
    params_factory.add_component(Expression_component(")", Expression_component::parenthesis));
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
        params_factory.start_function_assignment(ctx->primary()->getText());
    }
}


void sv_visitor::exitPrimaryCall(sv2017::PrimaryCallContext *ctx) {
    if(params_factory.is_component_relevant()) {
        params_factory.stop_function_assignment();
    }
}

void sv_visitor::enterConstant_param_expression(sv2017::Constant_param_expressionContext *ctx) {
    params_factory.stop_range();
    if(ctx->concatenation()){
        params_factory.start_packed_assignment();
    }
}

void sv_visitor::enterBit_select(sv2017::Bit_selectContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.start_bit_selection();
    } else {
        params_factory.start_bit_selection();
    }

    deps_factory.start_bit_selection();
}

void sv_visitor::exitBit_select(sv2017::Bit_selectContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.stop_bit_selection();
    } else {
        params_factory.stop_bit_selection();
    }

    deps_factory.stop_bit_selection();
}

void sv_visitor::exitFirst_range_identifier(sv2017::First_range_identifierContext *ctx) {
    if (params_factory.is_component_relevant()) {
        params_factory.advance_range();
    }
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
    if (params_factory.is_component_relevant()) {
        params_factory.advance_range();
    }
}

void sv_visitor::enterRange_expression(sv2017::Range_expressionContext *ctx) {
    params_factory.open_range();
}

void sv_visitor::exitRange_expression(sv2017::Range_expressionContext *ctx) {
   params_factory.close_range();
}

void sv_visitor::enterArray_range_expression(sv2017::Array_range_expressionContext *ctx) {
    params_factory.open_range();
    if(deps_factory.is_valid_dependency()) {
        deps_factory.start_array_range();
    }
}

void sv_visitor::exitArray_range_expression(sv2017::Array_range_expressionContext *ctx) {
    params_factory.close_range();
    if(deps_factory.is_valid_dependency()) {
        deps_factory.stop_array_range();
    }
}

void sv_visitor::enterUnpacked_dimension(sv2017::Unpacked_dimensionContext *ctx) {
    params_factory.close_packed_dimensions();
    params_factory.start_unpacked_dimension_declaration();
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
    if (ctx->data_type_or_implicit() && ctx->data_type_or_implicit()->data_type()) {
        std::string type;
        if (ctx->data_type_or_implicit()->data_type()->data_type_primitive())
            type = ctx->data_type_or_implicit()->data_type()->data_type_primitive()->getText();
        if (ctx->data_type_or_implicit()->data_type()->package_or_class_scoped_path())
            type = ctx->data_type_or_implicit()->data_type()->package_or_class_scoped_path()->getText();
        params_factory.set_type(type);
    }


    params_factory.start_range();
    if (!ctx->data_type_or_implicit()) {
        params_factory.set_type("implicit");
    }
}

void sv_visitor::exitLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) {
    in_param_declaration = false;
    params_factory.set_type("");
}

void sv_visitor::enterLoop_generate_construct(sv2017::Loop_generate_constructContext *) {
    loops_factory.new_loop();
}

void sv_visitor::exitLoop_generate_construct(sv2017::Loop_generate_constructContext *) {
    auto inst = loops_factory.get_instances();
    for(auto &i:loops_factory.get_instances()){
        modules_factory.add_instance(i);
    }
}

void sv_visitor::enterGenvar_initialization(sv2017::Genvar_initializationContext *ctx) {
    std::string id = ctx->identifier()->getText();
    params_factory.start_param_assignment();
    params_factory.new_parameter(id);
}

void sv_visitor::exitGenvar_initialization(sv2017::Genvar_initializationContext *ctx) {
    auto param = params_factory.get_parameter();
    params_factory.stop_param_assignment();
    loops_factory.set_identifier(*param);
}

void sv_visitor::enterGenvar_expression(sv2017::Genvar_expressionContext *ctx) {
    params_factory.start_param_assignment();
    params_factory.new_parameter("genvar_expr");
}

void sv_visitor::exitGenvar_expression(sv2017::Genvar_expressionContext *ctx) {
    auto param = params_factory.get_parameter();
    if(!(param->get_expression()->is_expression() || param->get_expression()->is_cast())) {
        throw std::runtime_error("Concatenations or replications are not allowed in loop declarations");
    }
    auto ex = static_cast<Expression *>(param->get_expression().get());
    loops_factory.add_expression(*ex);

}

void sv_visitor::enterUntyped_function_declaration(sv2017::Untyped_function_declarationContext *ctx) {
    auto name = ctx->task_and_function_declaration_common()->identifier()[0]->getText();
    f_factory.set_name(name);
}

void sv_visitor::exitFunction_declaration(sv2017::Function_declarationContext *ctx) {
    modules_factory.add_function( f_factory.get_function());
}

void sv_visitor::enterLoop_statement(sv2017::Loop_statementContext *ctx) {
    if(f_factory.is_active()) {
        f_factory.pause();
        loops_factory.new_loop();
    }
}


void sv_visitor::exitLoop_statement(sv2017::Loop_statementContext *ctx) {
    if(f_factory.is_active()) {
        auto spec = loops_factory.get_loop_specs();
        f_factory.add_loop(spec);
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
    }
}

void sv_visitor::exitFor_end_expression(sv2017::For_end_expressionContext *ctx) {
    sv2017BaseListener::exitFor_end_expression(ctx);
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
            Expression e;
            e.emplace_back(name, Expression_component::get_type(name));
            if(ctx->inc_or_dec_operator()->INCR()){
                e.emplace_back("+", Expression_component::operation);
            } else if(ctx->inc_or_dec_operator()->DECR()){
                e.emplace_back("-",Expression_component::operation);
            }
            e.emplace_back("1", Expression_component::number);
            loops_factory.add_expression(e);
        }
    }
}

void sv_visitor::exitBlocking_assignment(sv2017::Blocking_assignmentContext *ctx) {
    if(!loops_factory.in_loop() && f_factory.is_active()) {
        f_factory.close_assignment();
    }

}

void sv_visitor::enterVariable_lvalue(sv2017::Variable_lvalueContext *ctx) {
    if(f_factory.is_active()) {
        auto var_name = ctx->package_or_class_scoped_hier_id_with_select()->package_or_class_scoped_path()->getText();
        if(loops_factory.in_loop()) {
            loops_factory.start_assignment(var_name);
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

       Expression e;
       auto str = ctx->identifier()->getText();
       e.emplace_back(str, Expression_component::get_type(str));
       if(ctx->inc_or_dec_operator()->INCR()){
           e.emplace_back("+", Expression_component::operation);
       } else if(ctx->inc_or_dec_operator()->DECR()){
           e.emplace_back("-", Expression_component::operation);
       }
       e.emplace_back(1);
       loops_factory.add_expression(e);
   }
}





