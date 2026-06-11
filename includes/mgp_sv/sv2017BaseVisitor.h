
// Generated from /Users/filss/git/makefile_gen/grammars/sv2017.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "sv2017Visitor.h"


namespace mgp_sv {

/**
 * This class provides an empty implementation of sv2017Visitor, which can be
 * extended to create a visitor which only needs to handle a subset of the available methods.
 */
class  sv2017BaseVisitor : public sv2017Visitor {
public:

  virtual std::any visitSource_text(sv2017::Source_textContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDescription(sv2017::DescriptionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignment_operator(sv2017::Assignment_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEdge_identifier(sv2017::Edge_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier(sv2017::IdentifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInteger_type(sv2017::Integer_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInteger_atom_type(sv2017::Integer_atom_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInteger_vector_type(sv2017::Integer_vector_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNon_integer_type(sv2017::Non_integer_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNet_type(sv2017::Net_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnary_module_path_operator(sv2017::Unary_module_path_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnary_operator(sv2017::Unary_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInc_or_dec_operator(sv2017::Inc_or_dec_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImplicit_class_handle(sv2017::Implicit_class_handleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIntegral_number(sv2017::Integral_numberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReal_number(sv2017::Real_numberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAny_system_tf_identifier(sv2017::Any_system_tf_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSigning(sv2017::SigningContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNumber(sv2017::NumberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTimeunits_declaration(sv2017::Timeunits_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLifetime(sv2017::LifetimeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPort_direction(sv2017::Port_directionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAlways_keyword(sv2017::Always_keywordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitJoin_keyword(sv2017::Join_keywordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnique_priority(sv2017::Unique_priorityContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDrive_strength(sv2017::Drive_strengthContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStrength0(sv2017::Strength0Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStrength1(sv2017::Strength1Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCharge_strength(sv2017::Charge_strengthContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_lvar_port_direction(sv2017::Sequence_lvar_port_directionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBins_keyword(sv2017::Bins_keywordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_item_qualifier(sv2017::Class_item_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRandom_qualifier(sv2017::Random_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_qualifier(sv2017::Property_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMethod_qualifier(sv2017::Method_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstraint_prototype_qualifier(sv2017::Constraint_prototype_qualifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCmos_switchtype(sv2017::Cmos_switchtypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnable_gatetype(sv2017::Enable_gatetypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMos_switchtype(sv2017::Mos_switchtypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitN_input_gatetype(sv2017::N_input_gatetypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitN_output_gatetype(sv2017::N_output_gatetypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPass_en_switchtype(sv2017::Pass_en_switchtypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPass_switchtype(sv2017::Pass_switchtypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAny_implication(sv2017::Any_implicationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTiming_check_event_control(sv2017::Timing_check_event_controlContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImport_export(sv2017::Import_exportContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArray_method_name(sv2017::Array_method_nameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_mul_div_mod(sv2017::Operator_mul_div_modContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_plus_minus(sv2017::Operator_plus_minusContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_shift(sv2017::Operator_shiftContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_cmp(sv2017::Operator_cmpContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_eq_neq(sv2017::Operator_eq_neqContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_xor(sv2017::Operator_xorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_impl(sv2017::Operator_implContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_nonansi_declaration(sv2017::Udp_nonansi_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_ansi_declaration(sv2017::Udp_ansi_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_declaration(sv2017::Udp_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_declaration_port_list(sv2017::Udp_declaration_port_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_port_declaration(sv2017::Udp_port_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_output_declaration(sv2017::Udp_output_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_input_declaration(sv2017::Udp_input_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_reg_declaration(sv2017::Udp_reg_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_body(sv2017::Udp_bodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCombinational_body(sv2017::Combinational_bodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCombinational_entry(sv2017::Combinational_entryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequential_body(sv2017::Sequential_bodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_initial_statement(sv2017::Udp_initial_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequential_entry(sv2017::Sequential_entryContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSeq_input_list(sv2017::Seq_input_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLevel_input_list(sv2017::Level_input_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEdge_input_list(sv2017::Edge_input_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEdge_indicator(sv2017::Edge_indicatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCurrent_state(sv2017::Current_stateContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNext_state(sv2017::Next_stateContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterface_declaration(sv2017::Interface_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterface_header(sv2017::Interface_headerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterface_item(sv2017::Interface_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModport_declaration(sv2017::Modport_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModport_item(sv2017::Modport_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModport_ports_declaration(sv2017::Modport_ports_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModport_clocking_declaration(sv2017::Modport_clocking_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModport_simple_ports_declaration(sv2017::Modport_simple_ports_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModport_simple_port(sv2017::Modport_simple_portContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModport_tf_ports_declaration(sv2017::Modport_tf_ports_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModport_tf_port(sv2017::Modport_tf_portContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_or_null(sv2017::Statement_or_nullContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInitial_construct(sv2017::Initial_constructContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDefault_clocking_or_dissable_construct(sv2017::Default_clocking_or_dissable_constructContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement(sv2017::StatementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStatement_item(sv2017::Statement_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCycle_delay(sv2017::Cycle_delayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClocking_drive(sv2017::Clocking_driveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClockvar_expression(sv2017::Clockvar_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFinal_construct(sv2017::Final_constructContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlocking_assignment(sv2017::Blocking_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProcedural_timing_control_statement(sv2017::Procedural_timing_control_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProcedural_timing_control(sv2017::Procedural_timing_controlContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEvent_control(sv2017::Event_controlContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDelay_or_event_control(sv2017::Delay_or_event_controlContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDelay3(sv2017::Delay3Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDelay2(sv2017::Delay2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDelay_value(sv2017::Delay_valueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDelay_control(sv2017::Delay_controlContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNonblocking_assignment(sv2017::Nonblocking_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProcedural_continuous_assignment(sv2017::Procedural_continuous_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariable_assignment(sv2017::Variable_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAction_block(sv2017::Action_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSeq_block(sv2017::Seq_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPar_block(sv2017::Par_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCase_statement(sv2017::Case_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCase_keyword(sv2017::Case_keywordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCase_item(sv2017::Case_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCase_pattern_item(sv2017::Case_pattern_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCase_inside_item(sv2017::Case_inside_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRandcase_statement(sv2017::Randcase_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRandcase_item(sv2017::Randcase_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCond_predicate(sv2017::Cond_predicateContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConditional_statement(sv2017::Conditional_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSubroutine_call_statement(sv2017::Subroutine_call_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDisable_statement(sv2017::Disable_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEvent_trigger(sv2017::Event_triggerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLoop_statement(sv2017::Loop_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_variable_assignments(sv2017::List_of_variable_assignmentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFor_initialization(sv2017::For_initializationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFor_end_expression(sv2017::For_end_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFor_variable_declaration_var_assign(sv2017::For_variable_declaration_var_assignContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFor_variable_declaration(sv2017::For_variable_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFor_step(sv2017::For_stepContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLoop_variables(sv2017::Loop_variablesContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitJump_statement(sv2017::Jump_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWait_statement(sv2017::Wait_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitName_of_instance(sv2017::Name_of_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChecker_instantiation(sv2017::Checker_instantiationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_checker_port_connections(sv2017::List_of_checker_port_connectionsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOrdered_checker_port_connection(sv2017::Ordered_checker_port_connectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamed_checker_port_connection(sv2017::Named_checker_port_connectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProcedural_assertion_statement(sv2017::Procedural_assertion_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConcurrent_assertion_statement(sv2017::Concurrent_assertion_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssertion_item(sv2017::Assertion_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConcurrent_assertion_item(sv2017::Concurrent_assertion_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImmediate_assertion_statement(sv2017::Immediate_assertion_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSimple_immediate_assertion_statement(sv2017::Simple_immediate_assertion_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSimple_immediate_assert_statement(sv2017::Simple_immediate_assert_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSimple_immediate_assume_statement(sv2017::Simple_immediate_assume_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSimple_immediate_cover_statement(sv2017::Simple_immediate_cover_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeferred_immediate_assertion_statement(sv2017::Deferred_immediate_assertion_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimitive_delay(sv2017::Primitive_delayContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeferred_immediate_assert_statement(sv2017::Deferred_immediate_assert_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeferred_immediate_assume_statement(sv2017::Deferred_immediate_assume_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDeferred_immediate_cover_statement(sv2017::Deferred_immediate_cover_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitWeight_specification(sv2017::Weight_specificationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProduction_item(sv2017::Production_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRs_code_block(sv2017::Rs_code_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRandsequence_statement(sv2017::Randsequence_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRs_prod(sv2017::Rs_prodContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRs_if_else(sv2017::Rs_if_elseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRs_repeat(sv2017::Rs_repeatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRs_case(sv2017::Rs_caseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRs_case_item(sv2017::Rs_case_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRs_rule(sv2017::Rs_ruleContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRs_production_list(sv2017::Rs_production_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProduction(sv2017::ProductionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTf_item_declaration(sv2017::Tf_item_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTf_port_list(sv2017::Tf_port_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTf_port_item(sv2017::Tf_port_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTf_port_direction(sv2017::Tf_port_directionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTf_port_declaration(sv2017::Tf_port_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_tf_variable_identifiers_item(sv2017::List_of_tf_variable_identifiers_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_tf_variable_identifiers(sv2017::List_of_tf_variable_identifiersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpect_property_statement(sv2017::Expect_property_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlock_item_declaration(sv2017::Block_item_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParam_assignment(sv2017::Param_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_assignment(sv2017::Type_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_type_assignments(sv2017::List_of_type_assignmentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_param_assignments(sv2017::List_of_param_assignmentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParameter_declaration(sv2017::Parameter_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_declaration(sv2017::Type_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNet_type_declaration(sv2017::Net_type_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLet_declaration(sv2017::Let_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLet_port_list(sv2017::Let_port_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLet_port_item(sv2017::Let_port_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLet_formal_type(sv2017::Let_formal_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_import_declaration(sv2017::Package_import_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_import_item(sv2017::Package_import_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_list_of_arguments(sv2017::Property_list_of_argumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_actual_arg(sv2017::Property_actual_argContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_formal_type(sv2017::Property_formal_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_formal_type(sv2017::Sequence_formal_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_instance(sv2017::Property_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_spec(sv2017::Property_specContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_expr(sv2017::Property_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_case_item(sv2017::Property_case_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBit_select(sv2017::Bit_selectContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier_with_bit_select(sv2017::Identifier_with_bit_selectContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_or_class_scoped_hier_id_with_select(sv2017::Package_or_class_scoped_hier_id_with_selectContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_or_class_scoped_path_item(sv2017::Package_or_class_scoped_path_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_or_class_scoped_path(sv2017::Package_or_class_scoped_pathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHierarchical_identifier(sv2017::Hierarchical_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_or_class_scoped_id(sv2017::Package_or_class_scoped_idContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelect(sv2017::SelectContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEvent_expression_item(sv2017::Event_expression_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEvent_expression(sv2017::Event_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBoolean_abbrev(sv2017::Boolean_abbrevContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_abbrev(sv2017::Sequence_abbrevContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConsecutive_repetition(sv2017::Consecutive_repetitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNon_consecutive_repetition(sv2017::Non_consecutive_repetitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGoto_repetition(sv2017::Goto_repetitionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCycle_delay_const_range_expression(sv2017::Cycle_delay_const_range_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_instance(sv2017::Sequence_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_expr(sv2017::Sequence_exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_match_item(sv2017::Sequence_match_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_assignment(sv2017::Operator_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_actual_arg(sv2017::Sequence_actual_argContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDist_weight(sv2017::Dist_weightContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClocking_declaration(sv2017::Clocking_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClocking_item(sv2017::Clocking_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_clocking_decl_assign(sv2017::List_of_clocking_decl_assignContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClocking_decl_assign(sv2017::Clocking_decl_assignContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDefault_skew(sv2017::Default_skewContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClocking_direction(sv2017::Clocking_directionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClocking_skew(sv2017::Clocking_skewContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClocking_event(sv2017::Clocking_eventContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCycle_delay_range(sv2017::Cycle_delay_rangeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression_or_dist(sv2017::Expression_or_distContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCovergroup_declaration(sv2017::Covergroup_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCover_cross(sv2017::Cover_crossContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier_list_2plus(sv2017::Identifier_list_2plusContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCross_body(sv2017::Cross_bodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCross_body_item(sv2017::Cross_body_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBins_selection_or_option(sv2017::Bins_selection_or_optionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBins_selection(sv2017::Bins_selectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelect_expression(sv2017::Select_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSelect_condition(sv2017::Select_conditionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBins_expression(sv2017::Bins_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCovergroup_range_list(sv2017::Covergroup_range_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCovergroup_value_range(sv2017::Covergroup_value_rangeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCovergroup_expression(sv2017::Covergroup_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCoverage_spec_or_option(sv2017::Coverage_spec_or_optionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCoverage_option(sv2017::Coverage_optionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCoverage_spec(sv2017::Coverage_specContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCover_point(sv2017::Cover_pointContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBins_or_empty(sv2017::Bins_or_emptyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBins_or_options(sv2017::Bins_or_optionsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTrans_list(sv2017::Trans_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTrans_set(sv2017::Trans_setContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTrans_range_list(sv2017::Trans_range_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRepeat_range(sv2017::Repeat_rangeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCoverage_event(sv2017::Coverage_eventContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBlock_event_expression(sv2017::Block_event_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHierarchical_btf_identifier(sv2017::Hierarchical_btf_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssertion_variable_declaration(sv2017::Assertion_variable_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDist_item(sv2017::Dist_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitValue_range(sv2017::Value_rangeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttribute_instance(sv2017::Attribute_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAttr_spec(sv2017::Attr_specContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_new(sv2017::Class_newContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParam_expression(sv2017::Param_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstant_param_expression(sv2017::Constant_param_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUnpacked_dimension(sv2017::Unpacked_dimensionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPacked_dimension(sv2017::Packed_dimensionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariable_dimension(sv2017::Variable_dimensionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_union(sv2017::Struct_unionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnum_base_type(sv2017::Enum_base_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_type_primitive(sv2017::Data_type_primitiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_type(sv2017::Data_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_type_or_implicit(sv2017::Data_type_or_implicitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitImplicit_data_type(sv2017::Implicit_data_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_list_of_arguments_named_item(sv2017::Sequence_list_of_arguments_named_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_list_of_arguments(sv2017::Sequence_list_of_argumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_arguments_named_item(sv2017::List_of_arguments_named_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_arguments(sv2017::List_of_argumentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimary_literal(sv2017::Primary_literalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitType_reference(sv2017::Type_referenceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_scope(sv2017::Package_scopeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPs_identifier(sv2017::Ps_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_parameter_value_assignments(sv2017::List_of_parameter_value_assignmentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParameter_value_assignment(sv2017::Parameter_value_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_type(sv2017::Class_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_scope(sv2017::Class_scopeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFirst_range_identifier(sv2017::First_range_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSecond_range_identifier(sv2017::Second_range_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRange_expression(sv2017::Range_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstant_range_expression(sv2017::Constant_range_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstant_mintypmax_expression(sv2017::Constant_mintypmax_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMintypmax_expression(sv2017::Mintypmax_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryLit(sv2017::PrimaryLitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryRepl(sv2017::PrimaryReplContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryRandomize(sv2017::PrimaryRandomizeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryAssig(sv2017::PrimaryAssigContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryBitSelect(sv2017::PrimaryBitSelectContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryTfCall(sv2017::PrimaryTfCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryTypeRef(sv2017::PrimaryTypeRefContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryCallArrayMethodNoArgs(sv2017::PrimaryCallArrayMethodNoArgsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryCast(sv2017::PrimaryCastContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryPar(sv2017::PrimaryParContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryCall(sv2017::PrimaryCallContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryRandomize2(sv2017::PrimaryRandomize2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryDot(sv2017::PrimaryDotContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryStreaming_concatenation(sv2017::PrimaryStreaming_concatenationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryPath(sv2017::PrimaryPathContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryIndex(sv2017::PrimaryIndexContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryCallWith(sv2017::PrimaryCallWithContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryConcat(sv2017::PrimaryConcatContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPrimaryCast2(sv2017::PrimaryCast2Context *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCast_separator(sv2017::Cast_separatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstant_expression(sv2017::Constant_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInc_or_dec_expressionPre(sv2017::Inc_or_dec_expressionPreContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInc_or_dec_expressionPost(sv2017::Inc_or_dec_expressionPostContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExpression(sv2017::ExpressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_power(sv2017::Operator_powerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_bitwise_and(sv2017::Operator_bitwise_andContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOperator_bitwise_or(sv2017::Operator_bitwise_orContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReplication(sv2017::ReplicationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReplication_size(sv2017::Replication_sizeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReplication_value(sv2017::Replication_valueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConcatenation(sv2017::ConcatenationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDynamic_array_new(sv2017::Dynamic_array_newContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConst_or_range_expression(sv2017::Const_or_range_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariable_decl_assignment(sv2017::Variable_decl_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignment_pattern_variable_lvalue(sv2017::Assignment_pattern_variable_lvalueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStream_operator(sv2017::Stream_operatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSlice_size(sv2017::Slice_sizeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStreaming_concatenation(sv2017::Streaming_concatenationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStream_concatenation(sv2017::Stream_concatenationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStream_expression(sv2017::Stream_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArray_range_expression(sv2017::Array_range_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRange_separator(sv2017::Range_separatorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOpen_range_list(sv2017::Open_range_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPattern(sv2017::PatternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignment_pattern(sv2017::Assignment_patternContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReplication_assignment(sv2017::Replication_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStructure_pattern_key(sv2017::Structure_pattern_keyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitArray_pattern_key(sv2017::Array_pattern_keyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignment_pattern_key(sv2017::Assignment_pattern_keyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStruct_union_member(sv2017::Struct_union_memberContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_type_or_void(sv2017::Data_type_or_voidContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnum_name_declaration(sv2017::Enum_name_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignment_pattern_expression(sv2017::Assignment_pattern_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAssignment_pattern_expression_type(sv2017::Assignment_pattern_expression_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNet_lvalue(sv2017::Net_lvalueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVariable_lvalue(sv2017::Variable_lvalueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSolve_before_list(sv2017::Solve_before_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstraint_block_item(sv2017::Constraint_block_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstraint_expression(sv2017::Constraint_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUniqueness_constraint(sv2017::Uniqueness_constraintContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstraint_set(sv2017::Constraint_setContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRandomize_call(sv2017::Randomize_callContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_header_common(sv2017::Module_header_commonContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_declaration(sv2017::Module_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_keyword(sv2017::Module_keywordContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNet_port_type(sv2017::Net_port_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitVar_data_type(sv2017::Var_data_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNet_or_var_data_type(sv2017::Net_or_var_data_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_defparam_assignments(sv2017::List_of_defparam_assignmentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_net_decl_assignments(sv2017::List_of_net_decl_assignmentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_specparam_assignments(sv2017::List_of_specparam_assignmentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_variable_decl_assignments(sv2017::List_of_variable_decl_assignmentsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_variable_identifiers_item(sv2017::List_of_variable_identifiers_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_variable_identifiers(sv2017::List_of_variable_identifiersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_variable_port_identifiers(sv2017::List_of_variable_port_identifiersContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDefparam_assignment(sv2017::Defparam_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNet_decl_assignment(sv2017::Net_decl_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecparam_assignment(sv2017::Specparam_assignmentContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitError_limit_value(sv2017::Error_limit_valueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitReject_limit_value(sv2017::Reject_limit_valueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPulse_control_specparam(sv2017::Pulse_control_specparamContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier_doted_index_at_end(sv2017::Identifier_doted_index_at_endContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecify_terminal_descriptor(sv2017::Specify_terminal_descriptorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecify_input_terminal_descriptor(sv2017::Specify_input_terminal_descriptorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecify_output_terminal_descriptor(sv2017::Specify_output_terminal_descriptorContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecify_item(sv2017::Specify_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPulsestyle_declaration(sv2017::Pulsestyle_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitShowcancelled_declaration(sv2017::Showcancelled_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPath_declaration(sv2017::Path_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSimple_path_declaration(sv2017::Simple_path_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPath_delay_value(sv2017::Path_delay_valueContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_path_outputs(sv2017::List_of_path_outputsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_path_inputs(sv2017::List_of_path_inputsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_paths(sv2017::List_of_pathsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_path_delay_expressions(sv2017::List_of_path_delay_expressionsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitT_path_delay_expression(sv2017::T_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTrise_path_delay_expression(sv2017::Trise_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTfall_path_delay_expression(sv2017::Tfall_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTz_path_delay_expression(sv2017::Tz_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitT01_path_delay_expression(sv2017::T01_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitT10_path_delay_expression(sv2017::T10_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitT0z_path_delay_expression(sv2017::T0z_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTz1_path_delay_expression(sv2017::Tz1_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitT1z_path_delay_expression(sv2017::T1z_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTz0_path_delay_expression(sv2017::Tz0_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitT0x_path_delay_expression(sv2017::T0x_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTx1_path_delay_expression(sv2017::Tx1_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitT1x_path_delay_expression(sv2017::T1x_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTx0_path_delay_expression(sv2017::Tx0_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTxz_path_delay_expression(sv2017::Txz_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTzx_path_delay_expression(sv2017::Tzx_path_delay_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParallel_path_description(sv2017::Parallel_path_descriptionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFull_path_description(sv2017::Full_path_descriptionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIdentifier_list(sv2017::Identifier_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecparam_declaration(sv2017::Specparam_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEdge_sensitive_path_declaration(sv2017::Edge_sensitive_path_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParallel_edge_sensitive_path_description(sv2017::Parallel_edge_sensitive_path_descriptionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFull_edge_sensitive_path_description(sv2017::Full_edge_sensitive_path_descriptionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_source_expression(sv2017::Data_source_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitData_declaration(sv2017::Data_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_path_expression(sv2017::Module_path_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitState_dependent_path_declaration(sv2017::State_dependent_path_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_export_declaration(sv2017::Package_export_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGenvar_declaration(sv2017::Genvar_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNet_declaration(sv2017::Net_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParameter_port_list(sv2017::Parameter_port_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParameter_port_declaration(sv2017::Parameter_port_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_port_declarations_ansi_item(sv2017::List_of_port_declarations_ansi_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_port_declarations(sv2017::List_of_port_declarationsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNonansi_port_declaration(sv2017::Nonansi_port_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNonansi_port(sv2017::Nonansi_portContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNonansi_port__expr(sv2017::Nonansi_port__exprContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPort_identifier(sv2017::Port_identifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnsi_port_declaration(sv2017::Ansi_port_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSystem_timing_check(sv2017::System_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_setup_timing_check(sv2017::Dolar_setup_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_hold_timing_check(sv2017::Dolar_hold_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_setuphold_timing_check(sv2017::Dolar_setuphold_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_recovery_timing_check(sv2017::Dolar_recovery_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_removal_timing_check(sv2017::Dolar_removal_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_recrem_timing_check(sv2017::Dolar_recrem_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_skew_timing_check(sv2017::Dolar_skew_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_timeskew_timing_check(sv2017::Dolar_timeskew_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_fullskew_timing_check(sv2017::Dolar_fullskew_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_period_timing_check(sv2017::Dolar_period_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_width_timing_check(sv2017::Dolar_width_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDolar_nochange_timing_check(sv2017::Dolar_nochange_timing_checkContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTimecheck_condition(sv2017::Timecheck_conditionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitControlled_reference_event(sv2017::Controlled_reference_eventContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDelayed_reference(sv2017::Delayed_referenceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnd_edge_offset(sv2017::End_edge_offsetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEvent_based_flag(sv2017::Event_based_flagContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNotifier(sv2017::NotifierContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitRemain_active_flag(sv2017::Remain_active_flagContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTimestamp_condition(sv2017::Timestamp_conditionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitStart_edge_offset(sv2017::Start_edge_offsetContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitThreshold(sv2017::ThresholdContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTiming_check_limit(sv2017::Timing_check_limitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTiming_check_event(sv2017::Timing_check_eventContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTiming_check_condition(sv2017::Timing_check_conditionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitScalar_timing_check_condition(sv2017::Scalar_timing_check_conditionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitControlled_timing_check_event(sv2017::Controlled_timing_check_eventContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_data_type_or_implicit(sv2017::Function_data_type_or_implicitContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExtern_tf_declaration(sv2017::Extern_tf_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_declaration(sv2017::Function_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUntyped_function_declaration(sv2017::Untyped_function_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTask_prototype(sv2017::Task_prototypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitFunction_prototype(sv2017::Function_prototypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDpi_import_export(sv2017::Dpi_import_exportContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDpi_function_import_property(sv2017::Dpi_function_import_propertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDpi_task_import_property(sv2017::Dpi_task_import_propertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTask_and_function_declaration_common(sv2017::Task_and_function_declaration_commonContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitTask_declaration(sv2017::Task_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitMethod_prototype(sv2017::Method_prototypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitExtern_constraint_declaration(sv2017::Extern_constraint_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstraint_block(sv2017::Constraint_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChecker_port_list(sv2017::Checker_port_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChecker_port_item(sv2017::Checker_port_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChecker_port_direction(sv2017::Checker_port_directionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChecker_declaration(sv2017::Checker_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_declaration(sv2017::Class_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAlways_construct(sv2017::Always_constructContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterface_class_type(sv2017::Interface_class_typeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterface_class_declaration(sv2017::Interface_class_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterface_class_item(sv2017::Interface_class_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInterface_class_method(sv2017::Interface_class_methodContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_declaration(sv2017::Package_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPackage_item(sv2017::Package_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProgram_declaration(sv2017::Program_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProgram_header(sv2017::Program_headerContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProgram_item(sv2017::Program_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNon_port_program_item(sv2017::Non_port_program_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnonymous_program(sv2017::Anonymous_programContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitAnonymous_program_item(sv2017::Anonymous_program_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_declaration(sv2017::Sequence_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_port_list(sv2017::Sequence_port_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSequence_port_item(sv2017::Sequence_port_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_declaration(sv2017::Property_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_port_list(sv2017::Property_port_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProperty_port_item(sv2017::Property_port_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitContinuous_assign(sv2017::Continuous_assignContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitChecker_or_generate_item(sv2017::Checker_or_generate_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstraint_prototype(sv2017::Constraint_prototypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_constraint(sv2017::Class_constraintContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConstraint_declaration(sv2017::Constraint_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_constructor_declaration(sv2017::Class_constructor_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_property(sv2017::Class_propertyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_method(sv2017::Class_methodContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_constructor_prototype(sv2017::Class_constructor_prototypeContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitClass_item(sv2017::Class_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitParameter_override(sv2017::Parameter_overrideContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGate_instantiation(sv2017::Gate_instantiationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnable_gate_or_mos_switch_or_cmos_switch_instance(sv2017::Enable_gate_or_mos_switch_or_cmos_switch_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitN_input_gate_instance(sv2017::N_input_gate_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitN_output_gate_instance(sv2017::N_output_gate_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPass_switch_instance(sv2017::Pass_switch_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPass_enable_switch_instance(sv2017::Pass_enable_switch_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPull_gate_instance(sv2017::Pull_gate_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPulldown_strength(sv2017::Pulldown_strengthContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPullup_strength(sv2017::Pullup_strengthContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitEnable_terminal(sv2017::Enable_terminalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInout_terminal(sv2017::Inout_terminalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInput_terminal(sv2017::Input_terminalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOutput_terminal(sv2017::Output_terminalContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_instantiation(sv2017::Udp_instantiationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_instance(sv2017::Udp_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUdp_instance_body(sv2017::Udp_instance_bodyContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitHierarchical_instance(sv2017::Hierarchical_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitList_of_port_connections(sv2017::List_of_port_connectionsContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitOrdered_port_connection(sv2017::Ordered_port_connectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNamed_port_connection(sv2017::Named_port_connectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPort_expression_connection(sv2017::Port_expression_connectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPort_concatenation_connection(sv2017::Port_concatenation_connectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConcatenation_item(sv2017::Concatenation_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitPort_replication_connection(sv2017::Port_replication_connectionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBind_directive(sv2017::Bind_directiveContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBind_target_instance(sv2017::Bind_target_instanceContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBind_target_instance_list(sv2017::Bind_target_instance_listContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitBind_instantiation(sv2017::Bind_instantiationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfig_declaration(sv2017::Config_declarationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitDesign_statement(sv2017::Design_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConfig_rule_statement(sv2017::Config_rule_statementContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInst_clause(sv2017::Inst_clauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitInst_name(sv2017::Inst_nameContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCell_clause(sv2017::Cell_clauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLiblist_clause(sv2017::Liblist_clauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitUse_clause(sv2017::Use_clauseContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitNet_alias(sv2017::Net_aliasContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitSpecify_block(sv2017::Specify_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGenerate_region(sv2017::Generate_regionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGenvar_expression(sv2017::Genvar_expressionContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitLoop_generate_construct(sv2017::Loop_generate_constructContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGenvar_initialization(sv2017::Genvar_initializationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGenvar_iteration(sv2017::Genvar_iterationContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitConditional_generate_construct(sv2017::Conditional_generate_constructContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitIf_generate_construct(sv2017::If_generate_constructContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCase_generate_construct(sv2017::Case_generate_constructContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitCase_generate_item(sv2017::Case_generate_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGenerate_begin_end_block(sv2017::Generate_begin_end_blockContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitGenerate_item(sv2017::Generate_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitProgram_generate_item(sv2017::Program_generate_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_or_generate_or_interface_or_checker_item(sv2017::Module_or_generate_or_interface_or_checker_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_or_generate_or_interface_item(sv2017::Module_or_generate_or_interface_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_or_generate_item(sv2017::Module_or_generate_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitElaboration_system_task(sv2017::Elaboration_system_taskContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_item_item(sv2017::Module_item_itemContext *ctx) override {
    return visitChildren(ctx);
  }

  virtual std::any visitModule_item(sv2017::Module_itemContext *ctx) override {
    return visitChildren(ctx);
  }


};

}  // namespace mgp_sv
