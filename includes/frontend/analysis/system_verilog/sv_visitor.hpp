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

#ifndef ANANKE_SV_VISITOR_HPP
#define ANANKE_SV_VISITOR_HPP

#include "mgp_sv/sv2017BaseListener.h"
#include "mgp_sv/sv2017.h"

#include "data_model/HDL/factories/HDL_parameters_factory.hpp"
#include "data_model/HDL/factories/HDL_modules_factory.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"
#include "data_model/HDL/factories/HDL_interfaces_factory.hpp"
#include "data_model/HDL/factories/HDL_instances_factory.hpp"
#include "data_model/HDL/factories/HDL_loops_factory.hpp"
#include "data_model/HDL/factories/HDL_functions_factory.hpp"

#include "data_model/HDL/HDL_Resource.hpp"

#include "data_model/HDL/parameters/components/Parameter_components.hpp"

#include <utility>
#include <vector>
#include <string>
#include <iostream>
#include <unordered_map>
#include <filesystem>
#include <nlohmann/json.hpp>

using namespace mgp_sv;

class sv_visitor : public sv2017BaseListener {
public:
    sv_visitor() = default;
    void enterModule_declaration(sv2017::Module_declarationContext *ctx) override;
    void exitModule_declaration(sv2017::Module_declarationContext *ctx) override;
    void enterInterface_declaration(sv2017::Interface_declarationContext *ctx) override;
    void exitInterface_declaration(sv2017::Interface_declarationContext *ctx) override;
    void enterModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *ctx) override;
    void exitModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *ctx) override;
    void exitInterface_header(sv2017::Interface_headerContext *ctx) override;
    void enterPrimaryTfCall(sv2017::PrimaryTfCallContext *ctx) override;

    void enterCast_separator(sv2017::Cast_separatorContext *ctx) override;
    void enterPrimaryCast2(sv2017::PrimaryCast2Context *ctx) override;
    void exitPrimaryCast2(sv2017::PrimaryCast2Context *ctx) override;
    void enterPrimaryCast(sv2017::PrimaryCastContext *ctx) override;
    void exitPrimaryCast(sv2017::PrimaryCastContext *ctx) override;


    void enterClass_declaration(sv2017::Class_declarationContext *ctx) override;
    void exitClass_declaration(sv2017::Class_declarationContext *ctx) override;

    void exitPrimaryTfCall(sv2017::PrimaryTfCallContext *ctx) override;
    void enterPackage_declaration(sv2017::Package_declarationContext *ctx) override;
    void exitPackage_declaration(sv2017::Package_declarationContext *ctx) override;
    void exitPackage_or_class_scoped_path(sv2017::Package_or_class_scoped_pathContext *ctx) override;
    void enterParameter_declaration(sv2017::Parameter_declarationContext *ctx) override;
    void exitParameter_declaration(sv2017::Parameter_declarationContext *ctx) override;
    void enterParam_assignment(sv2017::Param_assignmentContext *ctx) override;
    void exitParam_assignment(sv2017::Param_assignmentContext *ctx) override;
    void exitAnsi_port_declaration(sv2017::Ansi_port_declarationContext *ctx) override;

    void enterExpression(sv2017::ExpressionContext *ctx) override;
    void exitExpression(sv2017::ExpressionContext *ctx) override;
    void exitOperator_mul_div_mod(sv2017::Operator_mul_div_modContext *ctx) override;
    void exitOperator_plus_minus(sv2017::Operator_plus_minusContext *ctx) override;
    void exitOperator_shift(sv2017::Operator_shiftContext *ctx) override;
    void exitUnary_operator(sv2017::Unary_operatorContext *ctx) override;
    void exitOperator_cmp(sv2017::Operator_cmpContext *ctx) override;
    void exitOperator_eq_neq(sv2017::Operator_eq_neqContext *ctx) override;
    void exitOperator_bitwise_and(sv2017::Operator_bitwise_andContext *ctx) override;
    void exitOperator_bitwise_or(sv2017::Operator_bitwise_orContext *ctx) override;
    void exitOperator_xor(sv2017::Operator_xorContext *ctx) override;
    void exitOperator_power(sv2017::Operator_powerContext *ctx) override;
    void exitOperator_log_and(sv2017::Operator_log_andContext *ctx) override;
    void exitOperator_log_or(sv2017::Operator_log_orContext *ctx) override;

    void exitPrimaryLit(sv2017::PrimaryLitContext *ctx) override;
    void enterPrimaryPath(sv2017::PrimaryPathContext *ctx) override;
    void exitPrimaryPath(sv2017::PrimaryPathContext *ctx) override;

    void enterNamed_port_connection(sv2017::Named_port_connectionContext *ctx) override;
    void exitNamed_port_connection(sv2017::Named_port_connectionContext *ctx) override;
    void enterNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *ctx) override;
    void exitNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *ctx) override;

    void enterAssignment_pattern(sv2017::Assignment_patternContext *ctx) override;
    void exitAssignment_pattern(sv2017::Assignment_patternContext *ctx) override;

    void exitPrimaryBitSelect(sv2017::PrimaryBitSelectContext *ctx) override;

    void exitPrimaryIndex(sv2017::PrimaryIndexContext *ctx) override;

    void enterPrimaryDot(sv2017::PrimaryDotContext *ctx) override;
    void exitPrimaryDot(sv2017::PrimaryDotContext *ctx) override;

    void enterReplication_value(sv2017::Replication_valueContext *ctx) override;


    void enterPrimaryCall(sv2017::PrimaryCallContext *ctx) override;
    void exitPrimaryCall(sv2017::PrimaryCallContext *ctx) override;

    void enterBit_select(sv2017::Bit_selectContext *ctx) override;
    void exitBit_select(sv2017::Bit_selectContext *ctx) override;

    void exitFirst_range_identifier(sv2017::First_range_identifierContext *ctx) override;

    void exitRange_separator(sv2017::Range_separatorContext *ctx) override;
    void enterRange_expression(sv2017::Range_expressionContext *ctx) override;
    void exitRange_expression(sv2017::Range_expressionContext *ctx) override;

    void enterArray_range_expression(sv2017::Array_range_expressionContext *ctx) override;
    void exitArray_range_expression(sv2017::Array_range_expressionContext *ctx) override;

    void enterUnpacked_dimension(sv2017::Unpacked_dimensionContext *ctx) override;

    void exitConcatenation_item(sv2017::Concatenation_itemContext *ctx) override;

    void enterConstant_param_expression(sv2017::Constant_param_expressionContext *ctx) override;

    void enterReplication_assignment(sv2017::Replication_assignmentContext *ctx) override;
    void exitReplication_assignment(sv2017::Replication_assignmentContext *ctx) override;


    void enterConcatenation(sv2017::ConcatenationContext *ctx) override;
    void exitConcatenation(sv2017::ConcatenationContext *ctx) override;


    void enterReplication(sv2017::ReplicationContext *ctx) override;
    void exitReplication(sv2017::ReplicationContext *ctx) override;

    static uint32_t parse_number(const std::string& s);
    std::vector<std::shared_ptr<hdl_statement_base>> get_entities();

    void exitData_type_or_implicit(sv2017::Data_type_or_implicitContext *ctx) override;


    void enterData_type(sv2017::Data_typeContext *ctx) override;
    void exitData_type(sv2017::Data_typeContext *ctx) override;

    void enterLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) override;
    void exitLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) override;

    void enterLoop_generate_construct(sv2017::Loop_generate_constructContext *ctx) override;
    void exitLoop_generate_construct(sv2017::Loop_generate_constructContext *ctx) override;

    void enterGenvar_initialization(sv2017::Genvar_initializationContext *ctx) override;
    void exitGenvar_initialization(sv2017::Genvar_initializationContext *ctx) override;

    void enterGenvar_expression(sv2017::Genvar_expressionContext *ctx) override;
    void exitGenvar_expression(sv2017::Genvar_expressionContext *ctx) override;

    void enterUntyped_function_declaration(sv2017::Untyped_function_declarationContext *ctx) override;

    void exitFunction_declaration(sv2017::Function_declarationContext *ctx) override;

    void enterLoop_statement(sv2017::Loop_statementContext *ctx) override;
    void exitLoop_statement(sv2017::Loop_statementContext *ctx) override;

    void exitStatement_item(sv2017::Statement_itemContext *ctx) override;
    void exitAssignment_operator(sv2017::Assignment_operatorContext *ctx) override;

    void enterFor_initialization(sv2017::For_initializationContext *ctx) override;
    void exitFor_initialization(sv2017::For_initializationContext *ctx) override;

    void enterFor_end_expression(sv2017::For_end_expressionContext *ctx) override;
    void exitFor_end_expression(sv2017::For_end_expressionContext *ctx) override;

    void enterFor_step(sv2017::For_stepContext *ctx) override;
    void exitFor_step(sv2017::For_stepContext *ctx) override;

    void enterInc_or_dec_expressionPost(sv2017::Inc_or_dec_expressionPostContext *ctx) override;

    void exitBlocking_assignment(sv2017::Blocking_assignmentContext *ctx) override;

    void enterVariable_lvalue(sv2017::Variable_lvalueContext *ctx) override;
    void exitVariable_lvalue(sv2017::Variable_lvalueContext *ctx) override;

    void exitGenvar_iteration(sv2017::Genvar_iterationContext *ctx) override;

    void enterName_of_instance(sv2017::Name_of_instanceContext *ctx) override;
    void exitName_of_instance(sv2017::Name_of_instanceContext *ctx) override;

    void enterTf_port_item(sv2017::Tf_port_itemContext *ctx) override;
    void exitTf_port_list(sv2017::Tf_port_listContext *ctx) override;

    void enterData_declaration(sv2017::Data_declarationContext *ctx) override;
    void exitData_declaration(sv2017::Data_declarationContext *ctx) override;

    void enterStruct_union_member(sv2017::Struct_union_memberContext *ctx) override;
    void exitStruct_union_member(sv2017::Struct_union_memberContext *ctx) override;
    void enterData_type_primitive(sv2017::Data_type_primitiveContext *ctx) override;



private:
    void route_expression_text(const std::string& text);
    void route_expression_component(const std::shared_ptr<Expression_base>& ec);
    void process_operation(Expression_v2::expression_operator op);
    std::shared_ptr<hdl_type> setup_data_type(sv2017::Data_type_or_implicitContext *dtoi);

    bool in_param_declaration = false;
    bool in_function_var_decl = false;
    bool in_class = false;
    std::vector<std::shared_ptr<hdl_statement_base>> entities;

    std::string current_parameter;
    std::string current_declaration_type;

    Type_engine type_engine;
    HDL_functions_factory f_factory;
    HDL_modules_factory modules_factory;
    HDL_interfaces_factory interfaces_factory;
    HDL_instances_factory deps_factory;
    HDL_parameters_factory params_factory;
    HDL_loops_factory loops_factory;

    bool in_anonymous_struct = false;
    std::shared_ptr<hdl_type> pending_anon_struct_type;

    std::string instance_prefix;
    std::string instance_item;
};


#endif //ANANKE_SV_VISITOR_HPP
