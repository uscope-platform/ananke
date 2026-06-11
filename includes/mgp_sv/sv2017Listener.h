
// Generated from /Users/filss/git/makefile_gen/grammars/sv2017.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "sv2017.h"


namespace mgp_sv {

/**
 * This interface defines an abstract listener for a parse tree produced by sv2017.
 */
class  sv2017Listener : public antlr4::tree::ParseTreeListener {
public:

  virtual void enterSource_text(sv2017::Source_textContext *ctx) = 0;
  virtual void exitSource_text(sv2017::Source_textContext *ctx) = 0;

  virtual void enterDescription(sv2017::DescriptionContext *ctx) = 0;
  virtual void exitDescription(sv2017::DescriptionContext *ctx) = 0;

  virtual void enterAssignment_operator(sv2017::Assignment_operatorContext *ctx) = 0;
  virtual void exitAssignment_operator(sv2017::Assignment_operatorContext *ctx) = 0;

  virtual void enterEdge_identifier(sv2017::Edge_identifierContext *ctx) = 0;
  virtual void exitEdge_identifier(sv2017::Edge_identifierContext *ctx) = 0;

  virtual void enterIdentifier(sv2017::IdentifierContext *ctx) = 0;
  virtual void exitIdentifier(sv2017::IdentifierContext *ctx) = 0;

  virtual void enterInteger_type(sv2017::Integer_typeContext *ctx) = 0;
  virtual void exitInteger_type(sv2017::Integer_typeContext *ctx) = 0;

  virtual void enterInteger_atom_type(sv2017::Integer_atom_typeContext *ctx) = 0;
  virtual void exitInteger_atom_type(sv2017::Integer_atom_typeContext *ctx) = 0;

  virtual void enterInteger_vector_type(sv2017::Integer_vector_typeContext *ctx) = 0;
  virtual void exitInteger_vector_type(sv2017::Integer_vector_typeContext *ctx) = 0;

  virtual void enterNon_integer_type(sv2017::Non_integer_typeContext *ctx) = 0;
  virtual void exitNon_integer_type(sv2017::Non_integer_typeContext *ctx) = 0;

  virtual void enterNet_type(sv2017::Net_typeContext *ctx) = 0;
  virtual void exitNet_type(sv2017::Net_typeContext *ctx) = 0;

  virtual void enterUnary_module_path_operator(sv2017::Unary_module_path_operatorContext *ctx) = 0;
  virtual void exitUnary_module_path_operator(sv2017::Unary_module_path_operatorContext *ctx) = 0;

  virtual void enterUnary_operator(sv2017::Unary_operatorContext *ctx) = 0;
  virtual void exitUnary_operator(sv2017::Unary_operatorContext *ctx) = 0;

  virtual void enterInc_or_dec_operator(sv2017::Inc_or_dec_operatorContext *ctx) = 0;
  virtual void exitInc_or_dec_operator(sv2017::Inc_or_dec_operatorContext *ctx) = 0;

  virtual void enterImplicit_class_handle(sv2017::Implicit_class_handleContext *ctx) = 0;
  virtual void exitImplicit_class_handle(sv2017::Implicit_class_handleContext *ctx) = 0;

  virtual void enterIntegral_number(sv2017::Integral_numberContext *ctx) = 0;
  virtual void exitIntegral_number(sv2017::Integral_numberContext *ctx) = 0;

  virtual void enterReal_number(sv2017::Real_numberContext *ctx) = 0;
  virtual void exitReal_number(sv2017::Real_numberContext *ctx) = 0;

  virtual void enterAny_system_tf_identifier(sv2017::Any_system_tf_identifierContext *ctx) = 0;
  virtual void exitAny_system_tf_identifier(sv2017::Any_system_tf_identifierContext *ctx) = 0;

  virtual void enterSigning(sv2017::SigningContext *ctx) = 0;
  virtual void exitSigning(sv2017::SigningContext *ctx) = 0;

  virtual void enterNumber(sv2017::NumberContext *ctx) = 0;
  virtual void exitNumber(sv2017::NumberContext *ctx) = 0;

  virtual void enterTimeunits_declaration(sv2017::Timeunits_declarationContext *ctx) = 0;
  virtual void exitTimeunits_declaration(sv2017::Timeunits_declarationContext *ctx) = 0;

  virtual void enterLifetime(sv2017::LifetimeContext *ctx) = 0;
  virtual void exitLifetime(sv2017::LifetimeContext *ctx) = 0;

  virtual void enterPort_direction(sv2017::Port_directionContext *ctx) = 0;
  virtual void exitPort_direction(sv2017::Port_directionContext *ctx) = 0;

  virtual void enterAlways_keyword(sv2017::Always_keywordContext *ctx) = 0;
  virtual void exitAlways_keyword(sv2017::Always_keywordContext *ctx) = 0;

  virtual void enterJoin_keyword(sv2017::Join_keywordContext *ctx) = 0;
  virtual void exitJoin_keyword(sv2017::Join_keywordContext *ctx) = 0;

  virtual void enterUnique_priority(sv2017::Unique_priorityContext *ctx) = 0;
  virtual void exitUnique_priority(sv2017::Unique_priorityContext *ctx) = 0;

  virtual void enterDrive_strength(sv2017::Drive_strengthContext *ctx) = 0;
  virtual void exitDrive_strength(sv2017::Drive_strengthContext *ctx) = 0;

  virtual void enterStrength0(sv2017::Strength0Context *ctx) = 0;
  virtual void exitStrength0(sv2017::Strength0Context *ctx) = 0;

  virtual void enterStrength1(sv2017::Strength1Context *ctx) = 0;
  virtual void exitStrength1(sv2017::Strength1Context *ctx) = 0;

  virtual void enterCharge_strength(sv2017::Charge_strengthContext *ctx) = 0;
  virtual void exitCharge_strength(sv2017::Charge_strengthContext *ctx) = 0;

  virtual void enterSequence_lvar_port_direction(sv2017::Sequence_lvar_port_directionContext *ctx) = 0;
  virtual void exitSequence_lvar_port_direction(sv2017::Sequence_lvar_port_directionContext *ctx) = 0;

  virtual void enterBins_keyword(sv2017::Bins_keywordContext *ctx) = 0;
  virtual void exitBins_keyword(sv2017::Bins_keywordContext *ctx) = 0;

  virtual void enterClass_item_qualifier(sv2017::Class_item_qualifierContext *ctx) = 0;
  virtual void exitClass_item_qualifier(sv2017::Class_item_qualifierContext *ctx) = 0;

  virtual void enterRandom_qualifier(sv2017::Random_qualifierContext *ctx) = 0;
  virtual void exitRandom_qualifier(sv2017::Random_qualifierContext *ctx) = 0;

  virtual void enterProperty_qualifier(sv2017::Property_qualifierContext *ctx) = 0;
  virtual void exitProperty_qualifier(sv2017::Property_qualifierContext *ctx) = 0;

  virtual void enterMethod_qualifier(sv2017::Method_qualifierContext *ctx) = 0;
  virtual void exitMethod_qualifier(sv2017::Method_qualifierContext *ctx) = 0;

  virtual void enterConstraint_prototype_qualifier(sv2017::Constraint_prototype_qualifierContext *ctx) = 0;
  virtual void exitConstraint_prototype_qualifier(sv2017::Constraint_prototype_qualifierContext *ctx) = 0;

  virtual void enterCmos_switchtype(sv2017::Cmos_switchtypeContext *ctx) = 0;
  virtual void exitCmos_switchtype(sv2017::Cmos_switchtypeContext *ctx) = 0;

  virtual void enterEnable_gatetype(sv2017::Enable_gatetypeContext *ctx) = 0;
  virtual void exitEnable_gatetype(sv2017::Enable_gatetypeContext *ctx) = 0;

  virtual void enterMos_switchtype(sv2017::Mos_switchtypeContext *ctx) = 0;
  virtual void exitMos_switchtype(sv2017::Mos_switchtypeContext *ctx) = 0;

  virtual void enterN_input_gatetype(sv2017::N_input_gatetypeContext *ctx) = 0;
  virtual void exitN_input_gatetype(sv2017::N_input_gatetypeContext *ctx) = 0;

  virtual void enterN_output_gatetype(sv2017::N_output_gatetypeContext *ctx) = 0;
  virtual void exitN_output_gatetype(sv2017::N_output_gatetypeContext *ctx) = 0;

  virtual void enterPass_en_switchtype(sv2017::Pass_en_switchtypeContext *ctx) = 0;
  virtual void exitPass_en_switchtype(sv2017::Pass_en_switchtypeContext *ctx) = 0;

  virtual void enterPass_switchtype(sv2017::Pass_switchtypeContext *ctx) = 0;
  virtual void exitPass_switchtype(sv2017::Pass_switchtypeContext *ctx) = 0;

  virtual void enterAny_implication(sv2017::Any_implicationContext *ctx) = 0;
  virtual void exitAny_implication(sv2017::Any_implicationContext *ctx) = 0;

  virtual void enterTiming_check_event_control(sv2017::Timing_check_event_controlContext *ctx) = 0;
  virtual void exitTiming_check_event_control(sv2017::Timing_check_event_controlContext *ctx) = 0;

  virtual void enterImport_export(sv2017::Import_exportContext *ctx) = 0;
  virtual void exitImport_export(sv2017::Import_exportContext *ctx) = 0;

  virtual void enterArray_method_name(sv2017::Array_method_nameContext *ctx) = 0;
  virtual void exitArray_method_name(sv2017::Array_method_nameContext *ctx) = 0;

  virtual void enterOperator_mul_div_mod(sv2017::Operator_mul_div_modContext *ctx) = 0;
  virtual void exitOperator_mul_div_mod(sv2017::Operator_mul_div_modContext *ctx) = 0;

  virtual void enterOperator_plus_minus(sv2017::Operator_plus_minusContext *ctx) = 0;
  virtual void exitOperator_plus_minus(sv2017::Operator_plus_minusContext *ctx) = 0;

  virtual void enterOperator_shift(sv2017::Operator_shiftContext *ctx) = 0;
  virtual void exitOperator_shift(sv2017::Operator_shiftContext *ctx) = 0;

  virtual void enterOperator_cmp(sv2017::Operator_cmpContext *ctx) = 0;
  virtual void exitOperator_cmp(sv2017::Operator_cmpContext *ctx) = 0;

  virtual void enterOperator_eq_neq(sv2017::Operator_eq_neqContext *ctx) = 0;
  virtual void exitOperator_eq_neq(sv2017::Operator_eq_neqContext *ctx) = 0;

  virtual void enterOperator_xor(sv2017::Operator_xorContext *ctx) = 0;
  virtual void exitOperator_xor(sv2017::Operator_xorContext *ctx) = 0;

  virtual void enterOperator_impl(sv2017::Operator_implContext *ctx) = 0;
  virtual void exitOperator_impl(sv2017::Operator_implContext *ctx) = 0;

  virtual void enterUdp_nonansi_declaration(sv2017::Udp_nonansi_declarationContext *ctx) = 0;
  virtual void exitUdp_nonansi_declaration(sv2017::Udp_nonansi_declarationContext *ctx) = 0;

  virtual void enterUdp_ansi_declaration(sv2017::Udp_ansi_declarationContext *ctx) = 0;
  virtual void exitUdp_ansi_declaration(sv2017::Udp_ansi_declarationContext *ctx) = 0;

  virtual void enterUdp_declaration(sv2017::Udp_declarationContext *ctx) = 0;
  virtual void exitUdp_declaration(sv2017::Udp_declarationContext *ctx) = 0;

  virtual void enterUdp_declaration_port_list(sv2017::Udp_declaration_port_listContext *ctx) = 0;
  virtual void exitUdp_declaration_port_list(sv2017::Udp_declaration_port_listContext *ctx) = 0;

  virtual void enterUdp_port_declaration(sv2017::Udp_port_declarationContext *ctx) = 0;
  virtual void exitUdp_port_declaration(sv2017::Udp_port_declarationContext *ctx) = 0;

  virtual void enterUdp_output_declaration(sv2017::Udp_output_declarationContext *ctx) = 0;
  virtual void exitUdp_output_declaration(sv2017::Udp_output_declarationContext *ctx) = 0;

  virtual void enterUdp_input_declaration(sv2017::Udp_input_declarationContext *ctx) = 0;
  virtual void exitUdp_input_declaration(sv2017::Udp_input_declarationContext *ctx) = 0;

  virtual void enterUdp_reg_declaration(sv2017::Udp_reg_declarationContext *ctx) = 0;
  virtual void exitUdp_reg_declaration(sv2017::Udp_reg_declarationContext *ctx) = 0;

  virtual void enterUdp_body(sv2017::Udp_bodyContext *ctx) = 0;
  virtual void exitUdp_body(sv2017::Udp_bodyContext *ctx) = 0;

  virtual void enterCombinational_body(sv2017::Combinational_bodyContext *ctx) = 0;
  virtual void exitCombinational_body(sv2017::Combinational_bodyContext *ctx) = 0;

  virtual void enterCombinational_entry(sv2017::Combinational_entryContext *ctx) = 0;
  virtual void exitCombinational_entry(sv2017::Combinational_entryContext *ctx) = 0;

  virtual void enterSequential_body(sv2017::Sequential_bodyContext *ctx) = 0;
  virtual void exitSequential_body(sv2017::Sequential_bodyContext *ctx) = 0;

  virtual void enterUdp_initial_statement(sv2017::Udp_initial_statementContext *ctx) = 0;
  virtual void exitUdp_initial_statement(sv2017::Udp_initial_statementContext *ctx) = 0;

  virtual void enterSequential_entry(sv2017::Sequential_entryContext *ctx) = 0;
  virtual void exitSequential_entry(sv2017::Sequential_entryContext *ctx) = 0;

  virtual void enterSeq_input_list(sv2017::Seq_input_listContext *ctx) = 0;
  virtual void exitSeq_input_list(sv2017::Seq_input_listContext *ctx) = 0;

  virtual void enterLevel_input_list(sv2017::Level_input_listContext *ctx) = 0;
  virtual void exitLevel_input_list(sv2017::Level_input_listContext *ctx) = 0;

  virtual void enterEdge_input_list(sv2017::Edge_input_listContext *ctx) = 0;
  virtual void exitEdge_input_list(sv2017::Edge_input_listContext *ctx) = 0;

  virtual void enterEdge_indicator(sv2017::Edge_indicatorContext *ctx) = 0;
  virtual void exitEdge_indicator(sv2017::Edge_indicatorContext *ctx) = 0;

  virtual void enterCurrent_state(sv2017::Current_stateContext *ctx) = 0;
  virtual void exitCurrent_state(sv2017::Current_stateContext *ctx) = 0;

  virtual void enterNext_state(sv2017::Next_stateContext *ctx) = 0;
  virtual void exitNext_state(sv2017::Next_stateContext *ctx) = 0;

  virtual void enterInterface_declaration(sv2017::Interface_declarationContext *ctx) = 0;
  virtual void exitInterface_declaration(sv2017::Interface_declarationContext *ctx) = 0;

  virtual void enterInterface_header(sv2017::Interface_headerContext *ctx) = 0;
  virtual void exitInterface_header(sv2017::Interface_headerContext *ctx) = 0;

  virtual void enterInterface_item(sv2017::Interface_itemContext *ctx) = 0;
  virtual void exitInterface_item(sv2017::Interface_itemContext *ctx) = 0;

  virtual void enterModport_declaration(sv2017::Modport_declarationContext *ctx) = 0;
  virtual void exitModport_declaration(sv2017::Modport_declarationContext *ctx) = 0;

  virtual void enterModport_item(sv2017::Modport_itemContext *ctx) = 0;
  virtual void exitModport_item(sv2017::Modport_itemContext *ctx) = 0;

  virtual void enterModport_ports_declaration(sv2017::Modport_ports_declarationContext *ctx) = 0;
  virtual void exitModport_ports_declaration(sv2017::Modport_ports_declarationContext *ctx) = 0;

  virtual void enterModport_clocking_declaration(sv2017::Modport_clocking_declarationContext *ctx) = 0;
  virtual void exitModport_clocking_declaration(sv2017::Modport_clocking_declarationContext *ctx) = 0;

  virtual void enterModport_simple_ports_declaration(sv2017::Modport_simple_ports_declarationContext *ctx) = 0;
  virtual void exitModport_simple_ports_declaration(sv2017::Modport_simple_ports_declarationContext *ctx) = 0;

  virtual void enterModport_simple_port(sv2017::Modport_simple_portContext *ctx) = 0;
  virtual void exitModport_simple_port(sv2017::Modport_simple_portContext *ctx) = 0;

  virtual void enterModport_tf_ports_declaration(sv2017::Modport_tf_ports_declarationContext *ctx) = 0;
  virtual void exitModport_tf_ports_declaration(sv2017::Modport_tf_ports_declarationContext *ctx) = 0;

  virtual void enterModport_tf_port(sv2017::Modport_tf_portContext *ctx) = 0;
  virtual void exitModport_tf_port(sv2017::Modport_tf_portContext *ctx) = 0;

  virtual void enterStatement_or_null(sv2017::Statement_or_nullContext *ctx) = 0;
  virtual void exitStatement_or_null(sv2017::Statement_or_nullContext *ctx) = 0;

  virtual void enterInitial_construct(sv2017::Initial_constructContext *ctx) = 0;
  virtual void exitInitial_construct(sv2017::Initial_constructContext *ctx) = 0;

  virtual void enterDefault_clocking_or_dissable_construct(sv2017::Default_clocking_or_dissable_constructContext *ctx) = 0;
  virtual void exitDefault_clocking_or_dissable_construct(sv2017::Default_clocking_or_dissable_constructContext *ctx) = 0;

  virtual void enterStatement(sv2017::StatementContext *ctx) = 0;
  virtual void exitStatement(sv2017::StatementContext *ctx) = 0;

  virtual void enterStatement_item(sv2017::Statement_itemContext *ctx) = 0;
  virtual void exitStatement_item(sv2017::Statement_itemContext *ctx) = 0;

  virtual void enterCycle_delay(sv2017::Cycle_delayContext *ctx) = 0;
  virtual void exitCycle_delay(sv2017::Cycle_delayContext *ctx) = 0;

  virtual void enterClocking_drive(sv2017::Clocking_driveContext *ctx) = 0;
  virtual void exitClocking_drive(sv2017::Clocking_driveContext *ctx) = 0;

  virtual void enterClockvar_expression(sv2017::Clockvar_expressionContext *ctx) = 0;
  virtual void exitClockvar_expression(sv2017::Clockvar_expressionContext *ctx) = 0;

  virtual void enterFinal_construct(sv2017::Final_constructContext *ctx) = 0;
  virtual void exitFinal_construct(sv2017::Final_constructContext *ctx) = 0;

  virtual void enterBlocking_assignment(sv2017::Blocking_assignmentContext *ctx) = 0;
  virtual void exitBlocking_assignment(sv2017::Blocking_assignmentContext *ctx) = 0;

  virtual void enterProcedural_timing_control_statement(sv2017::Procedural_timing_control_statementContext *ctx) = 0;
  virtual void exitProcedural_timing_control_statement(sv2017::Procedural_timing_control_statementContext *ctx) = 0;

  virtual void enterProcedural_timing_control(sv2017::Procedural_timing_controlContext *ctx) = 0;
  virtual void exitProcedural_timing_control(sv2017::Procedural_timing_controlContext *ctx) = 0;

  virtual void enterEvent_control(sv2017::Event_controlContext *ctx) = 0;
  virtual void exitEvent_control(sv2017::Event_controlContext *ctx) = 0;

  virtual void enterDelay_or_event_control(sv2017::Delay_or_event_controlContext *ctx) = 0;
  virtual void exitDelay_or_event_control(sv2017::Delay_or_event_controlContext *ctx) = 0;

  virtual void enterDelay3(sv2017::Delay3Context *ctx) = 0;
  virtual void exitDelay3(sv2017::Delay3Context *ctx) = 0;

  virtual void enterDelay2(sv2017::Delay2Context *ctx) = 0;
  virtual void exitDelay2(sv2017::Delay2Context *ctx) = 0;

  virtual void enterDelay_value(sv2017::Delay_valueContext *ctx) = 0;
  virtual void exitDelay_value(sv2017::Delay_valueContext *ctx) = 0;

  virtual void enterDelay_control(sv2017::Delay_controlContext *ctx) = 0;
  virtual void exitDelay_control(sv2017::Delay_controlContext *ctx) = 0;

  virtual void enterNonblocking_assignment(sv2017::Nonblocking_assignmentContext *ctx) = 0;
  virtual void exitNonblocking_assignment(sv2017::Nonblocking_assignmentContext *ctx) = 0;

  virtual void enterProcedural_continuous_assignment(sv2017::Procedural_continuous_assignmentContext *ctx) = 0;
  virtual void exitProcedural_continuous_assignment(sv2017::Procedural_continuous_assignmentContext *ctx) = 0;

  virtual void enterVariable_assignment(sv2017::Variable_assignmentContext *ctx) = 0;
  virtual void exitVariable_assignment(sv2017::Variable_assignmentContext *ctx) = 0;

  virtual void enterAction_block(sv2017::Action_blockContext *ctx) = 0;
  virtual void exitAction_block(sv2017::Action_blockContext *ctx) = 0;

  virtual void enterSeq_block(sv2017::Seq_blockContext *ctx) = 0;
  virtual void exitSeq_block(sv2017::Seq_blockContext *ctx) = 0;

  virtual void enterPar_block(sv2017::Par_blockContext *ctx) = 0;
  virtual void exitPar_block(sv2017::Par_blockContext *ctx) = 0;

  virtual void enterCase_statement(sv2017::Case_statementContext *ctx) = 0;
  virtual void exitCase_statement(sv2017::Case_statementContext *ctx) = 0;

  virtual void enterCase_keyword(sv2017::Case_keywordContext *ctx) = 0;
  virtual void exitCase_keyword(sv2017::Case_keywordContext *ctx) = 0;

  virtual void enterCase_item(sv2017::Case_itemContext *ctx) = 0;
  virtual void exitCase_item(sv2017::Case_itemContext *ctx) = 0;

  virtual void enterCase_pattern_item(sv2017::Case_pattern_itemContext *ctx) = 0;
  virtual void exitCase_pattern_item(sv2017::Case_pattern_itemContext *ctx) = 0;

  virtual void enterCase_inside_item(sv2017::Case_inside_itemContext *ctx) = 0;
  virtual void exitCase_inside_item(sv2017::Case_inside_itemContext *ctx) = 0;

  virtual void enterRandcase_statement(sv2017::Randcase_statementContext *ctx) = 0;
  virtual void exitRandcase_statement(sv2017::Randcase_statementContext *ctx) = 0;

  virtual void enterRandcase_item(sv2017::Randcase_itemContext *ctx) = 0;
  virtual void exitRandcase_item(sv2017::Randcase_itemContext *ctx) = 0;

  virtual void enterCond_predicate(sv2017::Cond_predicateContext *ctx) = 0;
  virtual void exitCond_predicate(sv2017::Cond_predicateContext *ctx) = 0;

  virtual void enterConditional_statement(sv2017::Conditional_statementContext *ctx) = 0;
  virtual void exitConditional_statement(sv2017::Conditional_statementContext *ctx) = 0;

  virtual void enterSubroutine_call_statement(sv2017::Subroutine_call_statementContext *ctx) = 0;
  virtual void exitSubroutine_call_statement(sv2017::Subroutine_call_statementContext *ctx) = 0;

  virtual void enterDisable_statement(sv2017::Disable_statementContext *ctx) = 0;
  virtual void exitDisable_statement(sv2017::Disable_statementContext *ctx) = 0;

  virtual void enterEvent_trigger(sv2017::Event_triggerContext *ctx) = 0;
  virtual void exitEvent_trigger(sv2017::Event_triggerContext *ctx) = 0;

  virtual void enterLoop_statement(sv2017::Loop_statementContext *ctx) = 0;
  virtual void exitLoop_statement(sv2017::Loop_statementContext *ctx) = 0;

  virtual void enterList_of_variable_assignments(sv2017::List_of_variable_assignmentsContext *ctx) = 0;
  virtual void exitList_of_variable_assignments(sv2017::List_of_variable_assignmentsContext *ctx) = 0;

  virtual void enterFor_initialization(sv2017::For_initializationContext *ctx) = 0;
  virtual void exitFor_initialization(sv2017::For_initializationContext *ctx) = 0;

  virtual void enterFor_end_expression(sv2017::For_end_expressionContext *ctx) = 0;
  virtual void exitFor_end_expression(sv2017::For_end_expressionContext *ctx) = 0;

  virtual void enterFor_variable_declaration_var_assign(sv2017::For_variable_declaration_var_assignContext *ctx) = 0;
  virtual void exitFor_variable_declaration_var_assign(sv2017::For_variable_declaration_var_assignContext *ctx) = 0;

  virtual void enterFor_variable_declaration(sv2017::For_variable_declarationContext *ctx) = 0;
  virtual void exitFor_variable_declaration(sv2017::For_variable_declarationContext *ctx) = 0;

  virtual void enterFor_step(sv2017::For_stepContext *ctx) = 0;
  virtual void exitFor_step(sv2017::For_stepContext *ctx) = 0;

  virtual void enterLoop_variables(sv2017::Loop_variablesContext *ctx) = 0;
  virtual void exitLoop_variables(sv2017::Loop_variablesContext *ctx) = 0;

  virtual void enterJump_statement(sv2017::Jump_statementContext *ctx) = 0;
  virtual void exitJump_statement(sv2017::Jump_statementContext *ctx) = 0;

  virtual void enterWait_statement(sv2017::Wait_statementContext *ctx) = 0;
  virtual void exitWait_statement(sv2017::Wait_statementContext *ctx) = 0;

  virtual void enterName_of_instance(sv2017::Name_of_instanceContext *ctx) = 0;
  virtual void exitName_of_instance(sv2017::Name_of_instanceContext *ctx) = 0;

  virtual void enterChecker_instantiation(sv2017::Checker_instantiationContext *ctx) = 0;
  virtual void exitChecker_instantiation(sv2017::Checker_instantiationContext *ctx) = 0;

  virtual void enterList_of_checker_port_connections(sv2017::List_of_checker_port_connectionsContext *ctx) = 0;
  virtual void exitList_of_checker_port_connections(sv2017::List_of_checker_port_connectionsContext *ctx) = 0;

  virtual void enterOrdered_checker_port_connection(sv2017::Ordered_checker_port_connectionContext *ctx) = 0;
  virtual void exitOrdered_checker_port_connection(sv2017::Ordered_checker_port_connectionContext *ctx) = 0;

  virtual void enterNamed_checker_port_connection(sv2017::Named_checker_port_connectionContext *ctx) = 0;
  virtual void exitNamed_checker_port_connection(sv2017::Named_checker_port_connectionContext *ctx) = 0;

  virtual void enterProcedural_assertion_statement(sv2017::Procedural_assertion_statementContext *ctx) = 0;
  virtual void exitProcedural_assertion_statement(sv2017::Procedural_assertion_statementContext *ctx) = 0;

  virtual void enterConcurrent_assertion_statement(sv2017::Concurrent_assertion_statementContext *ctx) = 0;
  virtual void exitConcurrent_assertion_statement(sv2017::Concurrent_assertion_statementContext *ctx) = 0;

  virtual void enterAssertion_item(sv2017::Assertion_itemContext *ctx) = 0;
  virtual void exitAssertion_item(sv2017::Assertion_itemContext *ctx) = 0;

  virtual void enterConcurrent_assertion_item(sv2017::Concurrent_assertion_itemContext *ctx) = 0;
  virtual void exitConcurrent_assertion_item(sv2017::Concurrent_assertion_itemContext *ctx) = 0;

  virtual void enterImmediate_assertion_statement(sv2017::Immediate_assertion_statementContext *ctx) = 0;
  virtual void exitImmediate_assertion_statement(sv2017::Immediate_assertion_statementContext *ctx) = 0;

  virtual void enterSimple_immediate_assertion_statement(sv2017::Simple_immediate_assertion_statementContext *ctx) = 0;
  virtual void exitSimple_immediate_assertion_statement(sv2017::Simple_immediate_assertion_statementContext *ctx) = 0;

  virtual void enterSimple_immediate_assert_statement(sv2017::Simple_immediate_assert_statementContext *ctx) = 0;
  virtual void exitSimple_immediate_assert_statement(sv2017::Simple_immediate_assert_statementContext *ctx) = 0;

  virtual void enterSimple_immediate_assume_statement(sv2017::Simple_immediate_assume_statementContext *ctx) = 0;
  virtual void exitSimple_immediate_assume_statement(sv2017::Simple_immediate_assume_statementContext *ctx) = 0;

  virtual void enterSimple_immediate_cover_statement(sv2017::Simple_immediate_cover_statementContext *ctx) = 0;
  virtual void exitSimple_immediate_cover_statement(sv2017::Simple_immediate_cover_statementContext *ctx) = 0;

  virtual void enterDeferred_immediate_assertion_statement(sv2017::Deferred_immediate_assertion_statementContext *ctx) = 0;
  virtual void exitDeferred_immediate_assertion_statement(sv2017::Deferred_immediate_assertion_statementContext *ctx) = 0;

  virtual void enterPrimitive_delay(sv2017::Primitive_delayContext *ctx) = 0;
  virtual void exitPrimitive_delay(sv2017::Primitive_delayContext *ctx) = 0;

  virtual void enterDeferred_immediate_assert_statement(sv2017::Deferred_immediate_assert_statementContext *ctx) = 0;
  virtual void exitDeferred_immediate_assert_statement(sv2017::Deferred_immediate_assert_statementContext *ctx) = 0;

  virtual void enterDeferred_immediate_assume_statement(sv2017::Deferred_immediate_assume_statementContext *ctx) = 0;
  virtual void exitDeferred_immediate_assume_statement(sv2017::Deferred_immediate_assume_statementContext *ctx) = 0;

  virtual void enterDeferred_immediate_cover_statement(sv2017::Deferred_immediate_cover_statementContext *ctx) = 0;
  virtual void exitDeferred_immediate_cover_statement(sv2017::Deferred_immediate_cover_statementContext *ctx) = 0;

  virtual void enterWeight_specification(sv2017::Weight_specificationContext *ctx) = 0;
  virtual void exitWeight_specification(sv2017::Weight_specificationContext *ctx) = 0;

  virtual void enterProduction_item(sv2017::Production_itemContext *ctx) = 0;
  virtual void exitProduction_item(sv2017::Production_itemContext *ctx) = 0;

  virtual void enterRs_code_block(sv2017::Rs_code_blockContext *ctx) = 0;
  virtual void exitRs_code_block(sv2017::Rs_code_blockContext *ctx) = 0;

  virtual void enterRandsequence_statement(sv2017::Randsequence_statementContext *ctx) = 0;
  virtual void exitRandsequence_statement(sv2017::Randsequence_statementContext *ctx) = 0;

  virtual void enterRs_prod(sv2017::Rs_prodContext *ctx) = 0;
  virtual void exitRs_prod(sv2017::Rs_prodContext *ctx) = 0;

  virtual void enterRs_if_else(sv2017::Rs_if_elseContext *ctx) = 0;
  virtual void exitRs_if_else(sv2017::Rs_if_elseContext *ctx) = 0;

  virtual void enterRs_repeat(sv2017::Rs_repeatContext *ctx) = 0;
  virtual void exitRs_repeat(sv2017::Rs_repeatContext *ctx) = 0;

  virtual void enterRs_case(sv2017::Rs_caseContext *ctx) = 0;
  virtual void exitRs_case(sv2017::Rs_caseContext *ctx) = 0;

  virtual void enterRs_case_item(sv2017::Rs_case_itemContext *ctx) = 0;
  virtual void exitRs_case_item(sv2017::Rs_case_itemContext *ctx) = 0;

  virtual void enterRs_rule(sv2017::Rs_ruleContext *ctx) = 0;
  virtual void exitRs_rule(sv2017::Rs_ruleContext *ctx) = 0;

  virtual void enterRs_production_list(sv2017::Rs_production_listContext *ctx) = 0;
  virtual void exitRs_production_list(sv2017::Rs_production_listContext *ctx) = 0;

  virtual void enterProduction(sv2017::ProductionContext *ctx) = 0;
  virtual void exitProduction(sv2017::ProductionContext *ctx) = 0;

  virtual void enterTf_item_declaration(sv2017::Tf_item_declarationContext *ctx) = 0;
  virtual void exitTf_item_declaration(sv2017::Tf_item_declarationContext *ctx) = 0;

  virtual void enterTf_port_list(sv2017::Tf_port_listContext *ctx) = 0;
  virtual void exitTf_port_list(sv2017::Tf_port_listContext *ctx) = 0;

  virtual void enterTf_port_item(sv2017::Tf_port_itemContext *ctx) = 0;
  virtual void exitTf_port_item(sv2017::Tf_port_itemContext *ctx) = 0;

  virtual void enterTf_port_direction(sv2017::Tf_port_directionContext *ctx) = 0;
  virtual void exitTf_port_direction(sv2017::Tf_port_directionContext *ctx) = 0;

  virtual void enterTf_port_declaration(sv2017::Tf_port_declarationContext *ctx) = 0;
  virtual void exitTf_port_declaration(sv2017::Tf_port_declarationContext *ctx) = 0;

  virtual void enterList_of_tf_variable_identifiers_item(sv2017::List_of_tf_variable_identifiers_itemContext *ctx) = 0;
  virtual void exitList_of_tf_variable_identifiers_item(sv2017::List_of_tf_variable_identifiers_itemContext *ctx) = 0;

  virtual void enterList_of_tf_variable_identifiers(sv2017::List_of_tf_variable_identifiersContext *ctx) = 0;
  virtual void exitList_of_tf_variable_identifiers(sv2017::List_of_tf_variable_identifiersContext *ctx) = 0;

  virtual void enterExpect_property_statement(sv2017::Expect_property_statementContext *ctx) = 0;
  virtual void exitExpect_property_statement(sv2017::Expect_property_statementContext *ctx) = 0;

  virtual void enterBlock_item_declaration(sv2017::Block_item_declarationContext *ctx) = 0;
  virtual void exitBlock_item_declaration(sv2017::Block_item_declarationContext *ctx) = 0;

  virtual void enterParam_assignment(sv2017::Param_assignmentContext *ctx) = 0;
  virtual void exitParam_assignment(sv2017::Param_assignmentContext *ctx) = 0;

  virtual void enterType_assignment(sv2017::Type_assignmentContext *ctx) = 0;
  virtual void exitType_assignment(sv2017::Type_assignmentContext *ctx) = 0;

  virtual void enterList_of_type_assignments(sv2017::List_of_type_assignmentsContext *ctx) = 0;
  virtual void exitList_of_type_assignments(sv2017::List_of_type_assignmentsContext *ctx) = 0;

  virtual void enterList_of_param_assignments(sv2017::List_of_param_assignmentsContext *ctx) = 0;
  virtual void exitList_of_param_assignments(sv2017::List_of_param_assignmentsContext *ctx) = 0;

  virtual void enterLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) = 0;
  virtual void exitLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) = 0;

  virtual void enterParameter_declaration(sv2017::Parameter_declarationContext *ctx) = 0;
  virtual void exitParameter_declaration(sv2017::Parameter_declarationContext *ctx) = 0;

  virtual void enterType_declaration(sv2017::Type_declarationContext *ctx) = 0;
  virtual void exitType_declaration(sv2017::Type_declarationContext *ctx) = 0;

  virtual void enterNet_type_declaration(sv2017::Net_type_declarationContext *ctx) = 0;
  virtual void exitNet_type_declaration(sv2017::Net_type_declarationContext *ctx) = 0;

  virtual void enterLet_declaration(sv2017::Let_declarationContext *ctx) = 0;
  virtual void exitLet_declaration(sv2017::Let_declarationContext *ctx) = 0;

  virtual void enterLet_port_list(sv2017::Let_port_listContext *ctx) = 0;
  virtual void exitLet_port_list(sv2017::Let_port_listContext *ctx) = 0;

  virtual void enterLet_port_item(sv2017::Let_port_itemContext *ctx) = 0;
  virtual void exitLet_port_item(sv2017::Let_port_itemContext *ctx) = 0;

  virtual void enterLet_formal_type(sv2017::Let_formal_typeContext *ctx) = 0;
  virtual void exitLet_formal_type(sv2017::Let_formal_typeContext *ctx) = 0;

  virtual void enterPackage_import_declaration(sv2017::Package_import_declarationContext *ctx) = 0;
  virtual void exitPackage_import_declaration(sv2017::Package_import_declarationContext *ctx) = 0;

  virtual void enterPackage_import_item(sv2017::Package_import_itemContext *ctx) = 0;
  virtual void exitPackage_import_item(sv2017::Package_import_itemContext *ctx) = 0;

  virtual void enterProperty_list_of_arguments(sv2017::Property_list_of_argumentsContext *ctx) = 0;
  virtual void exitProperty_list_of_arguments(sv2017::Property_list_of_argumentsContext *ctx) = 0;

  virtual void enterProperty_actual_arg(sv2017::Property_actual_argContext *ctx) = 0;
  virtual void exitProperty_actual_arg(sv2017::Property_actual_argContext *ctx) = 0;

  virtual void enterProperty_formal_type(sv2017::Property_formal_typeContext *ctx) = 0;
  virtual void exitProperty_formal_type(sv2017::Property_formal_typeContext *ctx) = 0;

  virtual void enterSequence_formal_type(sv2017::Sequence_formal_typeContext *ctx) = 0;
  virtual void exitSequence_formal_type(sv2017::Sequence_formal_typeContext *ctx) = 0;

  virtual void enterProperty_instance(sv2017::Property_instanceContext *ctx) = 0;
  virtual void exitProperty_instance(sv2017::Property_instanceContext *ctx) = 0;

  virtual void enterProperty_spec(sv2017::Property_specContext *ctx) = 0;
  virtual void exitProperty_spec(sv2017::Property_specContext *ctx) = 0;

  virtual void enterProperty_expr(sv2017::Property_exprContext *ctx) = 0;
  virtual void exitProperty_expr(sv2017::Property_exprContext *ctx) = 0;

  virtual void enterProperty_case_item(sv2017::Property_case_itemContext *ctx) = 0;
  virtual void exitProperty_case_item(sv2017::Property_case_itemContext *ctx) = 0;

  virtual void enterBit_select(sv2017::Bit_selectContext *ctx) = 0;
  virtual void exitBit_select(sv2017::Bit_selectContext *ctx) = 0;

  virtual void enterIdentifier_with_bit_select(sv2017::Identifier_with_bit_selectContext *ctx) = 0;
  virtual void exitIdentifier_with_bit_select(sv2017::Identifier_with_bit_selectContext *ctx) = 0;

  virtual void enterPackage_or_class_scoped_hier_id_with_select(sv2017::Package_or_class_scoped_hier_id_with_selectContext *ctx) = 0;
  virtual void exitPackage_or_class_scoped_hier_id_with_select(sv2017::Package_or_class_scoped_hier_id_with_selectContext *ctx) = 0;

  virtual void enterPackage_or_class_scoped_path_item(sv2017::Package_or_class_scoped_path_itemContext *ctx) = 0;
  virtual void exitPackage_or_class_scoped_path_item(sv2017::Package_or_class_scoped_path_itemContext *ctx) = 0;

  virtual void enterPackage_or_class_scoped_path(sv2017::Package_or_class_scoped_pathContext *ctx) = 0;
  virtual void exitPackage_or_class_scoped_path(sv2017::Package_or_class_scoped_pathContext *ctx) = 0;

  virtual void enterHierarchical_identifier(sv2017::Hierarchical_identifierContext *ctx) = 0;
  virtual void exitHierarchical_identifier(sv2017::Hierarchical_identifierContext *ctx) = 0;

  virtual void enterPackage_or_class_scoped_id(sv2017::Package_or_class_scoped_idContext *ctx) = 0;
  virtual void exitPackage_or_class_scoped_id(sv2017::Package_or_class_scoped_idContext *ctx) = 0;

  virtual void enterSelect(sv2017::SelectContext *ctx) = 0;
  virtual void exitSelect(sv2017::SelectContext *ctx) = 0;

  virtual void enterEvent_expression_item(sv2017::Event_expression_itemContext *ctx) = 0;
  virtual void exitEvent_expression_item(sv2017::Event_expression_itemContext *ctx) = 0;

  virtual void enterEvent_expression(sv2017::Event_expressionContext *ctx) = 0;
  virtual void exitEvent_expression(sv2017::Event_expressionContext *ctx) = 0;

  virtual void enterBoolean_abbrev(sv2017::Boolean_abbrevContext *ctx) = 0;
  virtual void exitBoolean_abbrev(sv2017::Boolean_abbrevContext *ctx) = 0;

  virtual void enterSequence_abbrev(sv2017::Sequence_abbrevContext *ctx) = 0;
  virtual void exitSequence_abbrev(sv2017::Sequence_abbrevContext *ctx) = 0;

  virtual void enterConsecutive_repetition(sv2017::Consecutive_repetitionContext *ctx) = 0;
  virtual void exitConsecutive_repetition(sv2017::Consecutive_repetitionContext *ctx) = 0;

  virtual void enterNon_consecutive_repetition(sv2017::Non_consecutive_repetitionContext *ctx) = 0;
  virtual void exitNon_consecutive_repetition(sv2017::Non_consecutive_repetitionContext *ctx) = 0;

  virtual void enterGoto_repetition(sv2017::Goto_repetitionContext *ctx) = 0;
  virtual void exitGoto_repetition(sv2017::Goto_repetitionContext *ctx) = 0;

  virtual void enterCycle_delay_const_range_expression(sv2017::Cycle_delay_const_range_expressionContext *ctx) = 0;
  virtual void exitCycle_delay_const_range_expression(sv2017::Cycle_delay_const_range_expressionContext *ctx) = 0;

  virtual void enterSequence_instance(sv2017::Sequence_instanceContext *ctx) = 0;
  virtual void exitSequence_instance(sv2017::Sequence_instanceContext *ctx) = 0;

  virtual void enterSequence_expr(sv2017::Sequence_exprContext *ctx) = 0;
  virtual void exitSequence_expr(sv2017::Sequence_exprContext *ctx) = 0;

  virtual void enterSequence_match_item(sv2017::Sequence_match_itemContext *ctx) = 0;
  virtual void exitSequence_match_item(sv2017::Sequence_match_itemContext *ctx) = 0;

  virtual void enterOperator_assignment(sv2017::Operator_assignmentContext *ctx) = 0;
  virtual void exitOperator_assignment(sv2017::Operator_assignmentContext *ctx) = 0;

  virtual void enterSequence_actual_arg(sv2017::Sequence_actual_argContext *ctx) = 0;
  virtual void exitSequence_actual_arg(sv2017::Sequence_actual_argContext *ctx) = 0;

  virtual void enterDist_weight(sv2017::Dist_weightContext *ctx) = 0;
  virtual void exitDist_weight(sv2017::Dist_weightContext *ctx) = 0;

  virtual void enterClocking_declaration(sv2017::Clocking_declarationContext *ctx) = 0;
  virtual void exitClocking_declaration(sv2017::Clocking_declarationContext *ctx) = 0;

  virtual void enterClocking_item(sv2017::Clocking_itemContext *ctx) = 0;
  virtual void exitClocking_item(sv2017::Clocking_itemContext *ctx) = 0;

  virtual void enterList_of_clocking_decl_assign(sv2017::List_of_clocking_decl_assignContext *ctx) = 0;
  virtual void exitList_of_clocking_decl_assign(sv2017::List_of_clocking_decl_assignContext *ctx) = 0;

  virtual void enterClocking_decl_assign(sv2017::Clocking_decl_assignContext *ctx) = 0;
  virtual void exitClocking_decl_assign(sv2017::Clocking_decl_assignContext *ctx) = 0;

  virtual void enterDefault_skew(sv2017::Default_skewContext *ctx) = 0;
  virtual void exitDefault_skew(sv2017::Default_skewContext *ctx) = 0;

  virtual void enterClocking_direction(sv2017::Clocking_directionContext *ctx) = 0;
  virtual void exitClocking_direction(sv2017::Clocking_directionContext *ctx) = 0;

  virtual void enterClocking_skew(sv2017::Clocking_skewContext *ctx) = 0;
  virtual void exitClocking_skew(sv2017::Clocking_skewContext *ctx) = 0;

  virtual void enterClocking_event(sv2017::Clocking_eventContext *ctx) = 0;
  virtual void exitClocking_event(sv2017::Clocking_eventContext *ctx) = 0;

  virtual void enterCycle_delay_range(sv2017::Cycle_delay_rangeContext *ctx) = 0;
  virtual void exitCycle_delay_range(sv2017::Cycle_delay_rangeContext *ctx) = 0;

  virtual void enterExpression_or_dist(sv2017::Expression_or_distContext *ctx) = 0;
  virtual void exitExpression_or_dist(sv2017::Expression_or_distContext *ctx) = 0;

  virtual void enterCovergroup_declaration(sv2017::Covergroup_declarationContext *ctx) = 0;
  virtual void exitCovergroup_declaration(sv2017::Covergroup_declarationContext *ctx) = 0;

  virtual void enterCover_cross(sv2017::Cover_crossContext *ctx) = 0;
  virtual void exitCover_cross(sv2017::Cover_crossContext *ctx) = 0;

  virtual void enterIdentifier_list_2plus(sv2017::Identifier_list_2plusContext *ctx) = 0;
  virtual void exitIdentifier_list_2plus(sv2017::Identifier_list_2plusContext *ctx) = 0;

  virtual void enterCross_body(sv2017::Cross_bodyContext *ctx) = 0;
  virtual void exitCross_body(sv2017::Cross_bodyContext *ctx) = 0;

  virtual void enterCross_body_item(sv2017::Cross_body_itemContext *ctx) = 0;
  virtual void exitCross_body_item(sv2017::Cross_body_itemContext *ctx) = 0;

  virtual void enterBins_selection_or_option(sv2017::Bins_selection_or_optionContext *ctx) = 0;
  virtual void exitBins_selection_or_option(sv2017::Bins_selection_or_optionContext *ctx) = 0;

  virtual void enterBins_selection(sv2017::Bins_selectionContext *ctx) = 0;
  virtual void exitBins_selection(sv2017::Bins_selectionContext *ctx) = 0;

  virtual void enterSelect_expression(sv2017::Select_expressionContext *ctx) = 0;
  virtual void exitSelect_expression(sv2017::Select_expressionContext *ctx) = 0;

  virtual void enterSelect_condition(sv2017::Select_conditionContext *ctx) = 0;
  virtual void exitSelect_condition(sv2017::Select_conditionContext *ctx) = 0;

  virtual void enterBins_expression(sv2017::Bins_expressionContext *ctx) = 0;
  virtual void exitBins_expression(sv2017::Bins_expressionContext *ctx) = 0;

  virtual void enterCovergroup_range_list(sv2017::Covergroup_range_listContext *ctx) = 0;
  virtual void exitCovergroup_range_list(sv2017::Covergroup_range_listContext *ctx) = 0;

  virtual void enterCovergroup_value_range(sv2017::Covergroup_value_rangeContext *ctx) = 0;
  virtual void exitCovergroup_value_range(sv2017::Covergroup_value_rangeContext *ctx) = 0;

  virtual void enterCovergroup_expression(sv2017::Covergroup_expressionContext *ctx) = 0;
  virtual void exitCovergroup_expression(sv2017::Covergroup_expressionContext *ctx) = 0;

  virtual void enterCoverage_spec_or_option(sv2017::Coverage_spec_or_optionContext *ctx) = 0;
  virtual void exitCoverage_spec_or_option(sv2017::Coverage_spec_or_optionContext *ctx) = 0;

  virtual void enterCoverage_option(sv2017::Coverage_optionContext *ctx) = 0;
  virtual void exitCoverage_option(sv2017::Coverage_optionContext *ctx) = 0;

  virtual void enterCoverage_spec(sv2017::Coverage_specContext *ctx) = 0;
  virtual void exitCoverage_spec(sv2017::Coverage_specContext *ctx) = 0;

  virtual void enterCover_point(sv2017::Cover_pointContext *ctx) = 0;
  virtual void exitCover_point(sv2017::Cover_pointContext *ctx) = 0;

  virtual void enterBins_or_empty(sv2017::Bins_or_emptyContext *ctx) = 0;
  virtual void exitBins_or_empty(sv2017::Bins_or_emptyContext *ctx) = 0;

  virtual void enterBins_or_options(sv2017::Bins_or_optionsContext *ctx) = 0;
  virtual void exitBins_or_options(sv2017::Bins_or_optionsContext *ctx) = 0;

  virtual void enterTrans_list(sv2017::Trans_listContext *ctx) = 0;
  virtual void exitTrans_list(sv2017::Trans_listContext *ctx) = 0;

  virtual void enterTrans_set(sv2017::Trans_setContext *ctx) = 0;
  virtual void exitTrans_set(sv2017::Trans_setContext *ctx) = 0;

  virtual void enterTrans_range_list(sv2017::Trans_range_listContext *ctx) = 0;
  virtual void exitTrans_range_list(sv2017::Trans_range_listContext *ctx) = 0;

  virtual void enterRepeat_range(sv2017::Repeat_rangeContext *ctx) = 0;
  virtual void exitRepeat_range(sv2017::Repeat_rangeContext *ctx) = 0;

  virtual void enterCoverage_event(sv2017::Coverage_eventContext *ctx) = 0;
  virtual void exitCoverage_event(sv2017::Coverage_eventContext *ctx) = 0;

  virtual void enterBlock_event_expression(sv2017::Block_event_expressionContext *ctx) = 0;
  virtual void exitBlock_event_expression(sv2017::Block_event_expressionContext *ctx) = 0;

  virtual void enterHierarchical_btf_identifier(sv2017::Hierarchical_btf_identifierContext *ctx) = 0;
  virtual void exitHierarchical_btf_identifier(sv2017::Hierarchical_btf_identifierContext *ctx) = 0;

  virtual void enterAssertion_variable_declaration(sv2017::Assertion_variable_declarationContext *ctx) = 0;
  virtual void exitAssertion_variable_declaration(sv2017::Assertion_variable_declarationContext *ctx) = 0;

  virtual void enterDist_item(sv2017::Dist_itemContext *ctx) = 0;
  virtual void exitDist_item(sv2017::Dist_itemContext *ctx) = 0;

  virtual void enterValue_range(sv2017::Value_rangeContext *ctx) = 0;
  virtual void exitValue_range(sv2017::Value_rangeContext *ctx) = 0;

  virtual void enterAttribute_instance(sv2017::Attribute_instanceContext *ctx) = 0;
  virtual void exitAttribute_instance(sv2017::Attribute_instanceContext *ctx) = 0;

  virtual void enterAttr_spec(sv2017::Attr_specContext *ctx) = 0;
  virtual void exitAttr_spec(sv2017::Attr_specContext *ctx) = 0;

  virtual void enterClass_new(sv2017::Class_newContext *ctx) = 0;
  virtual void exitClass_new(sv2017::Class_newContext *ctx) = 0;

  virtual void enterParam_expression(sv2017::Param_expressionContext *ctx) = 0;
  virtual void exitParam_expression(sv2017::Param_expressionContext *ctx) = 0;

  virtual void enterConstant_param_expression(sv2017::Constant_param_expressionContext *ctx) = 0;
  virtual void exitConstant_param_expression(sv2017::Constant_param_expressionContext *ctx) = 0;

  virtual void enterUnpacked_dimension(sv2017::Unpacked_dimensionContext *ctx) = 0;
  virtual void exitUnpacked_dimension(sv2017::Unpacked_dimensionContext *ctx) = 0;

  virtual void enterPacked_dimension(sv2017::Packed_dimensionContext *ctx) = 0;
  virtual void exitPacked_dimension(sv2017::Packed_dimensionContext *ctx) = 0;

  virtual void enterVariable_dimension(sv2017::Variable_dimensionContext *ctx) = 0;
  virtual void exitVariable_dimension(sv2017::Variable_dimensionContext *ctx) = 0;

  virtual void enterStruct_union(sv2017::Struct_unionContext *ctx) = 0;
  virtual void exitStruct_union(sv2017::Struct_unionContext *ctx) = 0;

  virtual void enterEnum_base_type(sv2017::Enum_base_typeContext *ctx) = 0;
  virtual void exitEnum_base_type(sv2017::Enum_base_typeContext *ctx) = 0;

  virtual void enterData_type_primitive(sv2017::Data_type_primitiveContext *ctx) = 0;
  virtual void exitData_type_primitive(sv2017::Data_type_primitiveContext *ctx) = 0;

  virtual void enterData_type(sv2017::Data_typeContext *ctx) = 0;
  virtual void exitData_type(sv2017::Data_typeContext *ctx) = 0;

  virtual void enterData_type_or_implicit(sv2017::Data_type_or_implicitContext *ctx) = 0;
  virtual void exitData_type_or_implicit(sv2017::Data_type_or_implicitContext *ctx) = 0;

  virtual void enterImplicit_data_type(sv2017::Implicit_data_typeContext *ctx) = 0;
  virtual void exitImplicit_data_type(sv2017::Implicit_data_typeContext *ctx) = 0;

  virtual void enterSequence_list_of_arguments_named_item(sv2017::Sequence_list_of_arguments_named_itemContext *ctx) = 0;
  virtual void exitSequence_list_of_arguments_named_item(sv2017::Sequence_list_of_arguments_named_itemContext *ctx) = 0;

  virtual void enterSequence_list_of_arguments(sv2017::Sequence_list_of_argumentsContext *ctx) = 0;
  virtual void exitSequence_list_of_arguments(sv2017::Sequence_list_of_argumentsContext *ctx) = 0;

  virtual void enterList_of_arguments_named_item(sv2017::List_of_arguments_named_itemContext *ctx) = 0;
  virtual void exitList_of_arguments_named_item(sv2017::List_of_arguments_named_itemContext *ctx) = 0;

  virtual void enterList_of_arguments(sv2017::List_of_argumentsContext *ctx) = 0;
  virtual void exitList_of_arguments(sv2017::List_of_argumentsContext *ctx) = 0;

  virtual void enterPrimary_literal(sv2017::Primary_literalContext *ctx) = 0;
  virtual void exitPrimary_literal(sv2017::Primary_literalContext *ctx) = 0;

  virtual void enterType_reference(sv2017::Type_referenceContext *ctx) = 0;
  virtual void exitType_reference(sv2017::Type_referenceContext *ctx) = 0;

  virtual void enterPackage_scope(sv2017::Package_scopeContext *ctx) = 0;
  virtual void exitPackage_scope(sv2017::Package_scopeContext *ctx) = 0;

  virtual void enterPs_identifier(sv2017::Ps_identifierContext *ctx) = 0;
  virtual void exitPs_identifier(sv2017::Ps_identifierContext *ctx) = 0;

  virtual void enterList_of_parameter_value_assignments(sv2017::List_of_parameter_value_assignmentsContext *ctx) = 0;
  virtual void exitList_of_parameter_value_assignments(sv2017::List_of_parameter_value_assignmentsContext *ctx) = 0;

  virtual void enterParameter_value_assignment(sv2017::Parameter_value_assignmentContext *ctx) = 0;
  virtual void exitParameter_value_assignment(sv2017::Parameter_value_assignmentContext *ctx) = 0;

  virtual void enterClass_type(sv2017::Class_typeContext *ctx) = 0;
  virtual void exitClass_type(sv2017::Class_typeContext *ctx) = 0;

  virtual void enterClass_scope(sv2017::Class_scopeContext *ctx) = 0;
  virtual void exitClass_scope(sv2017::Class_scopeContext *ctx) = 0;

  virtual void enterFirst_range_identifier(sv2017::First_range_identifierContext *ctx) = 0;
  virtual void exitFirst_range_identifier(sv2017::First_range_identifierContext *ctx) = 0;

  virtual void enterSecond_range_identifier(sv2017::Second_range_identifierContext *ctx) = 0;
  virtual void exitSecond_range_identifier(sv2017::Second_range_identifierContext *ctx) = 0;

  virtual void enterRange_expression(sv2017::Range_expressionContext *ctx) = 0;
  virtual void exitRange_expression(sv2017::Range_expressionContext *ctx) = 0;

  virtual void enterConstant_range_expression(sv2017::Constant_range_expressionContext *ctx) = 0;
  virtual void exitConstant_range_expression(sv2017::Constant_range_expressionContext *ctx) = 0;

  virtual void enterConstant_mintypmax_expression(sv2017::Constant_mintypmax_expressionContext *ctx) = 0;
  virtual void exitConstant_mintypmax_expression(sv2017::Constant_mintypmax_expressionContext *ctx) = 0;

  virtual void enterMintypmax_expression(sv2017::Mintypmax_expressionContext *ctx) = 0;
  virtual void exitMintypmax_expression(sv2017::Mintypmax_expressionContext *ctx) = 0;

  virtual void enterNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *ctx) = 0;
  virtual void exitNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *ctx) = 0;

  virtual void enterPrimaryLit(sv2017::PrimaryLitContext *ctx) = 0;
  virtual void exitPrimaryLit(sv2017::PrimaryLitContext *ctx) = 0;

  virtual void enterPrimaryRepl(sv2017::PrimaryReplContext *ctx) = 0;
  virtual void exitPrimaryRepl(sv2017::PrimaryReplContext *ctx) = 0;

  virtual void enterPrimaryRandomize(sv2017::PrimaryRandomizeContext *ctx) = 0;
  virtual void exitPrimaryRandomize(sv2017::PrimaryRandomizeContext *ctx) = 0;

  virtual void enterPrimaryAssig(sv2017::PrimaryAssigContext *ctx) = 0;
  virtual void exitPrimaryAssig(sv2017::PrimaryAssigContext *ctx) = 0;

  virtual void enterPrimaryBitSelect(sv2017::PrimaryBitSelectContext *ctx) = 0;
  virtual void exitPrimaryBitSelect(sv2017::PrimaryBitSelectContext *ctx) = 0;

  virtual void enterPrimaryTfCall(sv2017::PrimaryTfCallContext *ctx) = 0;
  virtual void exitPrimaryTfCall(sv2017::PrimaryTfCallContext *ctx) = 0;

  virtual void enterPrimaryTypeRef(sv2017::PrimaryTypeRefContext *ctx) = 0;
  virtual void exitPrimaryTypeRef(sv2017::PrimaryTypeRefContext *ctx) = 0;

  virtual void enterPrimaryCallArrayMethodNoArgs(sv2017::PrimaryCallArrayMethodNoArgsContext *ctx) = 0;
  virtual void exitPrimaryCallArrayMethodNoArgs(sv2017::PrimaryCallArrayMethodNoArgsContext *ctx) = 0;

  virtual void enterPrimaryCast(sv2017::PrimaryCastContext *ctx) = 0;
  virtual void exitPrimaryCast(sv2017::PrimaryCastContext *ctx) = 0;

  virtual void enterPrimaryPar(sv2017::PrimaryParContext *ctx) = 0;
  virtual void exitPrimaryPar(sv2017::PrimaryParContext *ctx) = 0;

  virtual void enterPrimaryCall(sv2017::PrimaryCallContext *ctx) = 0;
  virtual void exitPrimaryCall(sv2017::PrimaryCallContext *ctx) = 0;

  virtual void enterPrimaryRandomize2(sv2017::PrimaryRandomize2Context *ctx) = 0;
  virtual void exitPrimaryRandomize2(sv2017::PrimaryRandomize2Context *ctx) = 0;

  virtual void enterPrimaryDot(sv2017::PrimaryDotContext *ctx) = 0;
  virtual void exitPrimaryDot(sv2017::PrimaryDotContext *ctx) = 0;

  virtual void enterPrimaryStreaming_concatenation(sv2017::PrimaryStreaming_concatenationContext *ctx) = 0;
  virtual void exitPrimaryStreaming_concatenation(sv2017::PrimaryStreaming_concatenationContext *ctx) = 0;

  virtual void enterPrimaryPath(sv2017::PrimaryPathContext *ctx) = 0;
  virtual void exitPrimaryPath(sv2017::PrimaryPathContext *ctx) = 0;

  virtual void enterPrimaryIndex(sv2017::PrimaryIndexContext *ctx) = 0;
  virtual void exitPrimaryIndex(sv2017::PrimaryIndexContext *ctx) = 0;

  virtual void enterPrimaryCallWith(sv2017::PrimaryCallWithContext *ctx) = 0;
  virtual void exitPrimaryCallWith(sv2017::PrimaryCallWithContext *ctx) = 0;

  virtual void enterPrimaryConcat(sv2017::PrimaryConcatContext *ctx) = 0;
  virtual void exitPrimaryConcat(sv2017::PrimaryConcatContext *ctx) = 0;

  virtual void enterPrimaryCast2(sv2017::PrimaryCast2Context *ctx) = 0;
  virtual void exitPrimaryCast2(sv2017::PrimaryCast2Context *ctx) = 0;

  virtual void enterCast_separator(sv2017::Cast_separatorContext *ctx) = 0;
  virtual void exitCast_separator(sv2017::Cast_separatorContext *ctx) = 0;

  virtual void enterConstant_expression(sv2017::Constant_expressionContext *ctx) = 0;
  virtual void exitConstant_expression(sv2017::Constant_expressionContext *ctx) = 0;

  virtual void enterInc_or_dec_expressionPre(sv2017::Inc_or_dec_expressionPreContext *ctx) = 0;
  virtual void exitInc_or_dec_expressionPre(sv2017::Inc_or_dec_expressionPreContext *ctx) = 0;

  virtual void enterInc_or_dec_expressionPost(sv2017::Inc_or_dec_expressionPostContext *ctx) = 0;
  virtual void exitInc_or_dec_expressionPost(sv2017::Inc_or_dec_expressionPostContext *ctx) = 0;

  virtual void enterExpression(sv2017::ExpressionContext *ctx) = 0;
  virtual void exitExpression(sv2017::ExpressionContext *ctx) = 0;

  virtual void enterOperator_bitwise_and(sv2017::Operator_bitwise_andContext *ctx) = 0;
  virtual void exitOperator_bitwise_and(sv2017::Operator_bitwise_andContext *ctx) = 0;

  virtual void enterOperator_bitwise_or(sv2017::Operator_bitwise_orContext *ctx) = 0;
  virtual void exitOperator_bitwise_or(sv2017::Operator_bitwise_orContext *ctx) = 0;

  virtual void enterReplication(sv2017::ReplicationContext *ctx) = 0;
  virtual void exitReplication(sv2017::ReplicationContext *ctx) = 0;

  virtual void enterReplication_size(sv2017::Replication_sizeContext *ctx) = 0;
  virtual void exitReplication_size(sv2017::Replication_sizeContext *ctx) = 0;

  virtual void enterReplication_value(sv2017::Replication_valueContext *ctx) = 0;
  virtual void exitReplication_value(sv2017::Replication_valueContext *ctx) = 0;

  virtual void enterConcatenation(sv2017::ConcatenationContext *ctx) = 0;
  virtual void exitConcatenation(sv2017::ConcatenationContext *ctx) = 0;

  virtual void enterDynamic_array_new(sv2017::Dynamic_array_newContext *ctx) = 0;
  virtual void exitDynamic_array_new(sv2017::Dynamic_array_newContext *ctx) = 0;

  virtual void enterConst_or_range_expression(sv2017::Const_or_range_expressionContext *ctx) = 0;
  virtual void exitConst_or_range_expression(sv2017::Const_or_range_expressionContext *ctx) = 0;

  virtual void enterVariable_decl_assignment(sv2017::Variable_decl_assignmentContext *ctx) = 0;
  virtual void exitVariable_decl_assignment(sv2017::Variable_decl_assignmentContext *ctx) = 0;

  virtual void enterAssignment_pattern_variable_lvalue(sv2017::Assignment_pattern_variable_lvalueContext *ctx) = 0;
  virtual void exitAssignment_pattern_variable_lvalue(sv2017::Assignment_pattern_variable_lvalueContext *ctx) = 0;

  virtual void enterStream_operator(sv2017::Stream_operatorContext *ctx) = 0;
  virtual void exitStream_operator(sv2017::Stream_operatorContext *ctx) = 0;

  virtual void enterSlice_size(sv2017::Slice_sizeContext *ctx) = 0;
  virtual void exitSlice_size(sv2017::Slice_sizeContext *ctx) = 0;

  virtual void enterStreaming_concatenation(sv2017::Streaming_concatenationContext *ctx) = 0;
  virtual void exitStreaming_concatenation(sv2017::Streaming_concatenationContext *ctx) = 0;

  virtual void enterStream_concatenation(sv2017::Stream_concatenationContext *ctx) = 0;
  virtual void exitStream_concatenation(sv2017::Stream_concatenationContext *ctx) = 0;

  virtual void enterStream_expression(sv2017::Stream_expressionContext *ctx) = 0;
  virtual void exitStream_expression(sv2017::Stream_expressionContext *ctx) = 0;

  virtual void enterArray_range_expression(sv2017::Array_range_expressionContext *ctx) = 0;
  virtual void exitArray_range_expression(sv2017::Array_range_expressionContext *ctx) = 0;

  virtual void enterRange_separator(sv2017::Range_separatorContext *ctx) = 0;
  virtual void exitRange_separator(sv2017::Range_separatorContext *ctx) = 0;

  virtual void enterOpen_range_list(sv2017::Open_range_listContext *ctx) = 0;
  virtual void exitOpen_range_list(sv2017::Open_range_listContext *ctx) = 0;

  virtual void enterPattern(sv2017::PatternContext *ctx) = 0;
  virtual void exitPattern(sv2017::PatternContext *ctx) = 0;

  virtual void enterAssignment_pattern(sv2017::Assignment_patternContext *ctx) = 0;
  virtual void exitAssignment_pattern(sv2017::Assignment_patternContext *ctx) = 0;

  virtual void enterReplication_assignment(sv2017::Replication_assignmentContext *ctx) = 0;
  virtual void exitReplication_assignment(sv2017::Replication_assignmentContext *ctx) = 0;

  virtual void enterStructure_pattern_key(sv2017::Structure_pattern_keyContext *ctx) = 0;
  virtual void exitStructure_pattern_key(sv2017::Structure_pattern_keyContext *ctx) = 0;

  virtual void enterArray_pattern_key(sv2017::Array_pattern_keyContext *ctx) = 0;
  virtual void exitArray_pattern_key(sv2017::Array_pattern_keyContext *ctx) = 0;

  virtual void enterAssignment_pattern_key(sv2017::Assignment_pattern_keyContext *ctx) = 0;
  virtual void exitAssignment_pattern_key(sv2017::Assignment_pattern_keyContext *ctx) = 0;

  virtual void enterStruct_union_member(sv2017::Struct_union_memberContext *ctx) = 0;
  virtual void exitStruct_union_member(sv2017::Struct_union_memberContext *ctx) = 0;

  virtual void enterData_type_or_void(sv2017::Data_type_or_voidContext *ctx) = 0;
  virtual void exitData_type_or_void(sv2017::Data_type_or_voidContext *ctx) = 0;

  virtual void enterEnum_name_declaration(sv2017::Enum_name_declarationContext *ctx) = 0;
  virtual void exitEnum_name_declaration(sv2017::Enum_name_declarationContext *ctx) = 0;

  virtual void enterAssignment_pattern_expression(sv2017::Assignment_pattern_expressionContext *ctx) = 0;
  virtual void exitAssignment_pattern_expression(sv2017::Assignment_pattern_expressionContext *ctx) = 0;

  virtual void enterAssignment_pattern_expression_type(sv2017::Assignment_pattern_expression_typeContext *ctx) = 0;
  virtual void exitAssignment_pattern_expression_type(sv2017::Assignment_pattern_expression_typeContext *ctx) = 0;

  virtual void enterNet_lvalue(sv2017::Net_lvalueContext *ctx) = 0;
  virtual void exitNet_lvalue(sv2017::Net_lvalueContext *ctx) = 0;

  virtual void enterVariable_lvalue(sv2017::Variable_lvalueContext *ctx) = 0;
  virtual void exitVariable_lvalue(sv2017::Variable_lvalueContext *ctx) = 0;

  virtual void enterSolve_before_list(sv2017::Solve_before_listContext *ctx) = 0;
  virtual void exitSolve_before_list(sv2017::Solve_before_listContext *ctx) = 0;

  virtual void enterConstraint_block_item(sv2017::Constraint_block_itemContext *ctx) = 0;
  virtual void exitConstraint_block_item(sv2017::Constraint_block_itemContext *ctx) = 0;

  virtual void enterConstraint_expression(sv2017::Constraint_expressionContext *ctx) = 0;
  virtual void exitConstraint_expression(sv2017::Constraint_expressionContext *ctx) = 0;

  virtual void enterUniqueness_constraint(sv2017::Uniqueness_constraintContext *ctx) = 0;
  virtual void exitUniqueness_constraint(sv2017::Uniqueness_constraintContext *ctx) = 0;

  virtual void enterConstraint_set(sv2017::Constraint_setContext *ctx) = 0;
  virtual void exitConstraint_set(sv2017::Constraint_setContext *ctx) = 0;

  virtual void enterRandomize_call(sv2017::Randomize_callContext *ctx) = 0;
  virtual void exitRandomize_call(sv2017::Randomize_callContext *ctx) = 0;

  virtual void enterModule_header_common(sv2017::Module_header_commonContext *ctx) = 0;
  virtual void exitModule_header_common(sv2017::Module_header_commonContext *ctx) = 0;

  virtual void enterModule_declaration(sv2017::Module_declarationContext *ctx) = 0;
  virtual void exitModule_declaration(sv2017::Module_declarationContext *ctx) = 0;

  virtual void enterModule_keyword(sv2017::Module_keywordContext *ctx) = 0;
  virtual void exitModule_keyword(sv2017::Module_keywordContext *ctx) = 0;

  virtual void enterNet_port_type(sv2017::Net_port_typeContext *ctx) = 0;
  virtual void exitNet_port_type(sv2017::Net_port_typeContext *ctx) = 0;

  virtual void enterVar_data_type(sv2017::Var_data_typeContext *ctx) = 0;
  virtual void exitVar_data_type(sv2017::Var_data_typeContext *ctx) = 0;

  virtual void enterNet_or_var_data_type(sv2017::Net_or_var_data_typeContext *ctx) = 0;
  virtual void exitNet_or_var_data_type(sv2017::Net_or_var_data_typeContext *ctx) = 0;

  virtual void enterList_of_defparam_assignments(sv2017::List_of_defparam_assignmentsContext *ctx) = 0;
  virtual void exitList_of_defparam_assignments(sv2017::List_of_defparam_assignmentsContext *ctx) = 0;

  virtual void enterList_of_net_decl_assignments(sv2017::List_of_net_decl_assignmentsContext *ctx) = 0;
  virtual void exitList_of_net_decl_assignments(sv2017::List_of_net_decl_assignmentsContext *ctx) = 0;

  virtual void enterList_of_specparam_assignments(sv2017::List_of_specparam_assignmentsContext *ctx) = 0;
  virtual void exitList_of_specparam_assignments(sv2017::List_of_specparam_assignmentsContext *ctx) = 0;

  virtual void enterList_of_variable_decl_assignments(sv2017::List_of_variable_decl_assignmentsContext *ctx) = 0;
  virtual void exitList_of_variable_decl_assignments(sv2017::List_of_variable_decl_assignmentsContext *ctx) = 0;

  virtual void enterList_of_variable_identifiers_item(sv2017::List_of_variable_identifiers_itemContext *ctx) = 0;
  virtual void exitList_of_variable_identifiers_item(sv2017::List_of_variable_identifiers_itemContext *ctx) = 0;

  virtual void enterList_of_variable_identifiers(sv2017::List_of_variable_identifiersContext *ctx) = 0;
  virtual void exitList_of_variable_identifiers(sv2017::List_of_variable_identifiersContext *ctx) = 0;

  virtual void enterList_of_variable_port_identifiers(sv2017::List_of_variable_port_identifiersContext *ctx) = 0;
  virtual void exitList_of_variable_port_identifiers(sv2017::List_of_variable_port_identifiersContext *ctx) = 0;

  virtual void enterDefparam_assignment(sv2017::Defparam_assignmentContext *ctx) = 0;
  virtual void exitDefparam_assignment(sv2017::Defparam_assignmentContext *ctx) = 0;

  virtual void enterNet_decl_assignment(sv2017::Net_decl_assignmentContext *ctx) = 0;
  virtual void exitNet_decl_assignment(sv2017::Net_decl_assignmentContext *ctx) = 0;

  virtual void enterSpecparam_assignment(sv2017::Specparam_assignmentContext *ctx) = 0;
  virtual void exitSpecparam_assignment(sv2017::Specparam_assignmentContext *ctx) = 0;

  virtual void enterError_limit_value(sv2017::Error_limit_valueContext *ctx) = 0;
  virtual void exitError_limit_value(sv2017::Error_limit_valueContext *ctx) = 0;

  virtual void enterReject_limit_value(sv2017::Reject_limit_valueContext *ctx) = 0;
  virtual void exitReject_limit_value(sv2017::Reject_limit_valueContext *ctx) = 0;

  virtual void enterPulse_control_specparam(sv2017::Pulse_control_specparamContext *ctx) = 0;
  virtual void exitPulse_control_specparam(sv2017::Pulse_control_specparamContext *ctx) = 0;

  virtual void enterIdentifier_doted_index_at_end(sv2017::Identifier_doted_index_at_endContext *ctx) = 0;
  virtual void exitIdentifier_doted_index_at_end(sv2017::Identifier_doted_index_at_endContext *ctx) = 0;

  virtual void enterSpecify_terminal_descriptor(sv2017::Specify_terminal_descriptorContext *ctx) = 0;
  virtual void exitSpecify_terminal_descriptor(sv2017::Specify_terminal_descriptorContext *ctx) = 0;

  virtual void enterSpecify_input_terminal_descriptor(sv2017::Specify_input_terminal_descriptorContext *ctx) = 0;
  virtual void exitSpecify_input_terminal_descriptor(sv2017::Specify_input_terminal_descriptorContext *ctx) = 0;

  virtual void enterSpecify_output_terminal_descriptor(sv2017::Specify_output_terminal_descriptorContext *ctx) = 0;
  virtual void exitSpecify_output_terminal_descriptor(sv2017::Specify_output_terminal_descriptorContext *ctx) = 0;

  virtual void enterSpecify_item(sv2017::Specify_itemContext *ctx) = 0;
  virtual void exitSpecify_item(sv2017::Specify_itemContext *ctx) = 0;

  virtual void enterPulsestyle_declaration(sv2017::Pulsestyle_declarationContext *ctx) = 0;
  virtual void exitPulsestyle_declaration(sv2017::Pulsestyle_declarationContext *ctx) = 0;

  virtual void enterShowcancelled_declaration(sv2017::Showcancelled_declarationContext *ctx) = 0;
  virtual void exitShowcancelled_declaration(sv2017::Showcancelled_declarationContext *ctx) = 0;

  virtual void enterPath_declaration(sv2017::Path_declarationContext *ctx) = 0;
  virtual void exitPath_declaration(sv2017::Path_declarationContext *ctx) = 0;

  virtual void enterSimple_path_declaration(sv2017::Simple_path_declarationContext *ctx) = 0;
  virtual void exitSimple_path_declaration(sv2017::Simple_path_declarationContext *ctx) = 0;

  virtual void enterPath_delay_value(sv2017::Path_delay_valueContext *ctx) = 0;
  virtual void exitPath_delay_value(sv2017::Path_delay_valueContext *ctx) = 0;

  virtual void enterList_of_path_outputs(sv2017::List_of_path_outputsContext *ctx) = 0;
  virtual void exitList_of_path_outputs(sv2017::List_of_path_outputsContext *ctx) = 0;

  virtual void enterList_of_path_inputs(sv2017::List_of_path_inputsContext *ctx) = 0;
  virtual void exitList_of_path_inputs(sv2017::List_of_path_inputsContext *ctx) = 0;

  virtual void enterList_of_paths(sv2017::List_of_pathsContext *ctx) = 0;
  virtual void exitList_of_paths(sv2017::List_of_pathsContext *ctx) = 0;

  virtual void enterList_of_path_delay_expressions(sv2017::List_of_path_delay_expressionsContext *ctx) = 0;
  virtual void exitList_of_path_delay_expressions(sv2017::List_of_path_delay_expressionsContext *ctx) = 0;

  virtual void enterT_path_delay_expression(sv2017::T_path_delay_expressionContext *ctx) = 0;
  virtual void exitT_path_delay_expression(sv2017::T_path_delay_expressionContext *ctx) = 0;

  virtual void enterTrise_path_delay_expression(sv2017::Trise_path_delay_expressionContext *ctx) = 0;
  virtual void exitTrise_path_delay_expression(sv2017::Trise_path_delay_expressionContext *ctx) = 0;

  virtual void enterTfall_path_delay_expression(sv2017::Tfall_path_delay_expressionContext *ctx) = 0;
  virtual void exitTfall_path_delay_expression(sv2017::Tfall_path_delay_expressionContext *ctx) = 0;

  virtual void enterTz_path_delay_expression(sv2017::Tz_path_delay_expressionContext *ctx) = 0;
  virtual void exitTz_path_delay_expression(sv2017::Tz_path_delay_expressionContext *ctx) = 0;

  virtual void enterT01_path_delay_expression(sv2017::T01_path_delay_expressionContext *ctx) = 0;
  virtual void exitT01_path_delay_expression(sv2017::T01_path_delay_expressionContext *ctx) = 0;

  virtual void enterT10_path_delay_expression(sv2017::T10_path_delay_expressionContext *ctx) = 0;
  virtual void exitT10_path_delay_expression(sv2017::T10_path_delay_expressionContext *ctx) = 0;

  virtual void enterT0z_path_delay_expression(sv2017::T0z_path_delay_expressionContext *ctx) = 0;
  virtual void exitT0z_path_delay_expression(sv2017::T0z_path_delay_expressionContext *ctx) = 0;

  virtual void enterTz1_path_delay_expression(sv2017::Tz1_path_delay_expressionContext *ctx) = 0;
  virtual void exitTz1_path_delay_expression(sv2017::Tz1_path_delay_expressionContext *ctx) = 0;

  virtual void enterT1z_path_delay_expression(sv2017::T1z_path_delay_expressionContext *ctx) = 0;
  virtual void exitT1z_path_delay_expression(sv2017::T1z_path_delay_expressionContext *ctx) = 0;

  virtual void enterTz0_path_delay_expression(sv2017::Tz0_path_delay_expressionContext *ctx) = 0;
  virtual void exitTz0_path_delay_expression(sv2017::Tz0_path_delay_expressionContext *ctx) = 0;

  virtual void enterT0x_path_delay_expression(sv2017::T0x_path_delay_expressionContext *ctx) = 0;
  virtual void exitT0x_path_delay_expression(sv2017::T0x_path_delay_expressionContext *ctx) = 0;

  virtual void enterTx1_path_delay_expression(sv2017::Tx1_path_delay_expressionContext *ctx) = 0;
  virtual void exitTx1_path_delay_expression(sv2017::Tx1_path_delay_expressionContext *ctx) = 0;

  virtual void enterT1x_path_delay_expression(sv2017::T1x_path_delay_expressionContext *ctx) = 0;
  virtual void exitT1x_path_delay_expression(sv2017::T1x_path_delay_expressionContext *ctx) = 0;

  virtual void enterTx0_path_delay_expression(sv2017::Tx0_path_delay_expressionContext *ctx) = 0;
  virtual void exitTx0_path_delay_expression(sv2017::Tx0_path_delay_expressionContext *ctx) = 0;

  virtual void enterTxz_path_delay_expression(sv2017::Txz_path_delay_expressionContext *ctx) = 0;
  virtual void exitTxz_path_delay_expression(sv2017::Txz_path_delay_expressionContext *ctx) = 0;

  virtual void enterTzx_path_delay_expression(sv2017::Tzx_path_delay_expressionContext *ctx) = 0;
  virtual void exitTzx_path_delay_expression(sv2017::Tzx_path_delay_expressionContext *ctx) = 0;

  virtual void enterParallel_path_description(sv2017::Parallel_path_descriptionContext *ctx) = 0;
  virtual void exitParallel_path_description(sv2017::Parallel_path_descriptionContext *ctx) = 0;

  virtual void enterFull_path_description(sv2017::Full_path_descriptionContext *ctx) = 0;
  virtual void exitFull_path_description(sv2017::Full_path_descriptionContext *ctx) = 0;

  virtual void enterIdentifier_list(sv2017::Identifier_listContext *ctx) = 0;
  virtual void exitIdentifier_list(sv2017::Identifier_listContext *ctx) = 0;

  virtual void enterSpecparam_declaration(sv2017::Specparam_declarationContext *ctx) = 0;
  virtual void exitSpecparam_declaration(sv2017::Specparam_declarationContext *ctx) = 0;

  virtual void enterEdge_sensitive_path_declaration(sv2017::Edge_sensitive_path_declarationContext *ctx) = 0;
  virtual void exitEdge_sensitive_path_declaration(sv2017::Edge_sensitive_path_declarationContext *ctx) = 0;

  virtual void enterParallel_edge_sensitive_path_description(sv2017::Parallel_edge_sensitive_path_descriptionContext *ctx) = 0;
  virtual void exitParallel_edge_sensitive_path_description(sv2017::Parallel_edge_sensitive_path_descriptionContext *ctx) = 0;

  virtual void enterFull_edge_sensitive_path_description(sv2017::Full_edge_sensitive_path_descriptionContext *ctx) = 0;
  virtual void exitFull_edge_sensitive_path_description(sv2017::Full_edge_sensitive_path_descriptionContext *ctx) = 0;

  virtual void enterData_source_expression(sv2017::Data_source_expressionContext *ctx) = 0;
  virtual void exitData_source_expression(sv2017::Data_source_expressionContext *ctx) = 0;

  virtual void enterData_declaration(sv2017::Data_declarationContext *ctx) = 0;
  virtual void exitData_declaration(sv2017::Data_declarationContext *ctx) = 0;

  virtual void enterModule_path_expression(sv2017::Module_path_expressionContext *ctx) = 0;
  virtual void exitModule_path_expression(sv2017::Module_path_expressionContext *ctx) = 0;

  virtual void enterState_dependent_path_declaration(sv2017::State_dependent_path_declarationContext *ctx) = 0;
  virtual void exitState_dependent_path_declaration(sv2017::State_dependent_path_declarationContext *ctx) = 0;

  virtual void enterPackage_export_declaration(sv2017::Package_export_declarationContext *ctx) = 0;
  virtual void exitPackage_export_declaration(sv2017::Package_export_declarationContext *ctx) = 0;

  virtual void enterGenvar_declaration(sv2017::Genvar_declarationContext *ctx) = 0;
  virtual void exitGenvar_declaration(sv2017::Genvar_declarationContext *ctx) = 0;

  virtual void enterNet_declaration(sv2017::Net_declarationContext *ctx) = 0;
  virtual void exitNet_declaration(sv2017::Net_declarationContext *ctx) = 0;

  virtual void enterParameter_port_list(sv2017::Parameter_port_listContext *ctx) = 0;
  virtual void exitParameter_port_list(sv2017::Parameter_port_listContext *ctx) = 0;

  virtual void enterParameter_port_declaration(sv2017::Parameter_port_declarationContext *ctx) = 0;
  virtual void exitParameter_port_declaration(sv2017::Parameter_port_declarationContext *ctx) = 0;

  virtual void enterList_of_port_declarations_ansi_item(sv2017::List_of_port_declarations_ansi_itemContext *ctx) = 0;
  virtual void exitList_of_port_declarations_ansi_item(sv2017::List_of_port_declarations_ansi_itemContext *ctx) = 0;

  virtual void enterList_of_port_declarations(sv2017::List_of_port_declarationsContext *ctx) = 0;
  virtual void exitList_of_port_declarations(sv2017::List_of_port_declarationsContext *ctx) = 0;

  virtual void enterNonansi_port_declaration(sv2017::Nonansi_port_declarationContext *ctx) = 0;
  virtual void exitNonansi_port_declaration(sv2017::Nonansi_port_declarationContext *ctx) = 0;

  virtual void enterNonansi_port(sv2017::Nonansi_portContext *ctx) = 0;
  virtual void exitNonansi_port(sv2017::Nonansi_portContext *ctx) = 0;

  virtual void enterNonansi_port__expr(sv2017::Nonansi_port__exprContext *ctx) = 0;
  virtual void exitNonansi_port__expr(sv2017::Nonansi_port__exprContext *ctx) = 0;

  virtual void enterPort_identifier(sv2017::Port_identifierContext *ctx) = 0;
  virtual void exitPort_identifier(sv2017::Port_identifierContext *ctx) = 0;

  virtual void enterAnsi_port_declaration(sv2017::Ansi_port_declarationContext *ctx) = 0;
  virtual void exitAnsi_port_declaration(sv2017::Ansi_port_declarationContext *ctx) = 0;

  virtual void enterSystem_timing_check(sv2017::System_timing_checkContext *ctx) = 0;
  virtual void exitSystem_timing_check(sv2017::System_timing_checkContext *ctx) = 0;

  virtual void enterDolar_setup_timing_check(sv2017::Dolar_setup_timing_checkContext *ctx) = 0;
  virtual void exitDolar_setup_timing_check(sv2017::Dolar_setup_timing_checkContext *ctx) = 0;

  virtual void enterDolar_hold_timing_check(sv2017::Dolar_hold_timing_checkContext *ctx) = 0;
  virtual void exitDolar_hold_timing_check(sv2017::Dolar_hold_timing_checkContext *ctx) = 0;

  virtual void enterDolar_setuphold_timing_check(sv2017::Dolar_setuphold_timing_checkContext *ctx) = 0;
  virtual void exitDolar_setuphold_timing_check(sv2017::Dolar_setuphold_timing_checkContext *ctx) = 0;

  virtual void enterDolar_recovery_timing_check(sv2017::Dolar_recovery_timing_checkContext *ctx) = 0;
  virtual void exitDolar_recovery_timing_check(sv2017::Dolar_recovery_timing_checkContext *ctx) = 0;

  virtual void enterDolar_removal_timing_check(sv2017::Dolar_removal_timing_checkContext *ctx) = 0;
  virtual void exitDolar_removal_timing_check(sv2017::Dolar_removal_timing_checkContext *ctx) = 0;

  virtual void enterDolar_recrem_timing_check(sv2017::Dolar_recrem_timing_checkContext *ctx) = 0;
  virtual void exitDolar_recrem_timing_check(sv2017::Dolar_recrem_timing_checkContext *ctx) = 0;

  virtual void enterDolar_skew_timing_check(sv2017::Dolar_skew_timing_checkContext *ctx) = 0;
  virtual void exitDolar_skew_timing_check(sv2017::Dolar_skew_timing_checkContext *ctx) = 0;

  virtual void enterDolar_timeskew_timing_check(sv2017::Dolar_timeskew_timing_checkContext *ctx) = 0;
  virtual void exitDolar_timeskew_timing_check(sv2017::Dolar_timeskew_timing_checkContext *ctx) = 0;

  virtual void enterDolar_fullskew_timing_check(sv2017::Dolar_fullskew_timing_checkContext *ctx) = 0;
  virtual void exitDolar_fullskew_timing_check(sv2017::Dolar_fullskew_timing_checkContext *ctx) = 0;

  virtual void enterDolar_period_timing_check(sv2017::Dolar_period_timing_checkContext *ctx) = 0;
  virtual void exitDolar_period_timing_check(sv2017::Dolar_period_timing_checkContext *ctx) = 0;

  virtual void enterDolar_width_timing_check(sv2017::Dolar_width_timing_checkContext *ctx) = 0;
  virtual void exitDolar_width_timing_check(sv2017::Dolar_width_timing_checkContext *ctx) = 0;

  virtual void enterDolar_nochange_timing_check(sv2017::Dolar_nochange_timing_checkContext *ctx) = 0;
  virtual void exitDolar_nochange_timing_check(sv2017::Dolar_nochange_timing_checkContext *ctx) = 0;

  virtual void enterTimecheck_condition(sv2017::Timecheck_conditionContext *ctx) = 0;
  virtual void exitTimecheck_condition(sv2017::Timecheck_conditionContext *ctx) = 0;

  virtual void enterControlled_reference_event(sv2017::Controlled_reference_eventContext *ctx) = 0;
  virtual void exitControlled_reference_event(sv2017::Controlled_reference_eventContext *ctx) = 0;

  virtual void enterDelayed_reference(sv2017::Delayed_referenceContext *ctx) = 0;
  virtual void exitDelayed_reference(sv2017::Delayed_referenceContext *ctx) = 0;

  virtual void enterEnd_edge_offset(sv2017::End_edge_offsetContext *ctx) = 0;
  virtual void exitEnd_edge_offset(sv2017::End_edge_offsetContext *ctx) = 0;

  virtual void enterEvent_based_flag(sv2017::Event_based_flagContext *ctx) = 0;
  virtual void exitEvent_based_flag(sv2017::Event_based_flagContext *ctx) = 0;

  virtual void enterNotifier(sv2017::NotifierContext *ctx) = 0;
  virtual void exitNotifier(sv2017::NotifierContext *ctx) = 0;

  virtual void enterRemain_active_flag(sv2017::Remain_active_flagContext *ctx) = 0;
  virtual void exitRemain_active_flag(sv2017::Remain_active_flagContext *ctx) = 0;

  virtual void enterTimestamp_condition(sv2017::Timestamp_conditionContext *ctx) = 0;
  virtual void exitTimestamp_condition(sv2017::Timestamp_conditionContext *ctx) = 0;

  virtual void enterStart_edge_offset(sv2017::Start_edge_offsetContext *ctx) = 0;
  virtual void exitStart_edge_offset(sv2017::Start_edge_offsetContext *ctx) = 0;

  virtual void enterThreshold(sv2017::ThresholdContext *ctx) = 0;
  virtual void exitThreshold(sv2017::ThresholdContext *ctx) = 0;

  virtual void enterTiming_check_limit(sv2017::Timing_check_limitContext *ctx) = 0;
  virtual void exitTiming_check_limit(sv2017::Timing_check_limitContext *ctx) = 0;

  virtual void enterTiming_check_event(sv2017::Timing_check_eventContext *ctx) = 0;
  virtual void exitTiming_check_event(sv2017::Timing_check_eventContext *ctx) = 0;

  virtual void enterTiming_check_condition(sv2017::Timing_check_conditionContext *ctx) = 0;
  virtual void exitTiming_check_condition(sv2017::Timing_check_conditionContext *ctx) = 0;

  virtual void enterScalar_timing_check_condition(sv2017::Scalar_timing_check_conditionContext *ctx) = 0;
  virtual void exitScalar_timing_check_condition(sv2017::Scalar_timing_check_conditionContext *ctx) = 0;

  virtual void enterControlled_timing_check_event(sv2017::Controlled_timing_check_eventContext *ctx) = 0;
  virtual void exitControlled_timing_check_event(sv2017::Controlled_timing_check_eventContext *ctx) = 0;

  virtual void enterFunction_data_type_or_implicit(sv2017::Function_data_type_or_implicitContext *ctx) = 0;
  virtual void exitFunction_data_type_or_implicit(sv2017::Function_data_type_or_implicitContext *ctx) = 0;

  virtual void enterExtern_tf_declaration(sv2017::Extern_tf_declarationContext *ctx) = 0;
  virtual void exitExtern_tf_declaration(sv2017::Extern_tf_declarationContext *ctx) = 0;

  virtual void enterFunction_declaration(sv2017::Function_declarationContext *ctx) = 0;
  virtual void exitFunction_declaration(sv2017::Function_declarationContext *ctx) = 0;

  virtual void enterUntyped_function_declaration(sv2017::Untyped_function_declarationContext *ctx) = 0;
  virtual void exitUntyped_function_declaration(sv2017::Untyped_function_declarationContext *ctx) = 0;

  virtual void enterTask_prototype(sv2017::Task_prototypeContext *ctx) = 0;
  virtual void exitTask_prototype(sv2017::Task_prototypeContext *ctx) = 0;

  virtual void enterFunction_prototype(sv2017::Function_prototypeContext *ctx) = 0;
  virtual void exitFunction_prototype(sv2017::Function_prototypeContext *ctx) = 0;

  virtual void enterDpi_import_export(sv2017::Dpi_import_exportContext *ctx) = 0;
  virtual void exitDpi_import_export(sv2017::Dpi_import_exportContext *ctx) = 0;

  virtual void enterDpi_function_import_property(sv2017::Dpi_function_import_propertyContext *ctx) = 0;
  virtual void exitDpi_function_import_property(sv2017::Dpi_function_import_propertyContext *ctx) = 0;

  virtual void enterDpi_task_import_property(sv2017::Dpi_task_import_propertyContext *ctx) = 0;
  virtual void exitDpi_task_import_property(sv2017::Dpi_task_import_propertyContext *ctx) = 0;

  virtual void enterTask_and_function_declaration_common(sv2017::Task_and_function_declaration_commonContext *ctx) = 0;
  virtual void exitTask_and_function_declaration_common(sv2017::Task_and_function_declaration_commonContext *ctx) = 0;

  virtual void enterTask_declaration(sv2017::Task_declarationContext *ctx) = 0;
  virtual void exitTask_declaration(sv2017::Task_declarationContext *ctx) = 0;

  virtual void enterMethod_prototype(sv2017::Method_prototypeContext *ctx) = 0;
  virtual void exitMethod_prototype(sv2017::Method_prototypeContext *ctx) = 0;

  virtual void enterExtern_constraint_declaration(sv2017::Extern_constraint_declarationContext *ctx) = 0;
  virtual void exitExtern_constraint_declaration(sv2017::Extern_constraint_declarationContext *ctx) = 0;

  virtual void enterConstraint_block(sv2017::Constraint_blockContext *ctx) = 0;
  virtual void exitConstraint_block(sv2017::Constraint_blockContext *ctx) = 0;

  virtual void enterChecker_port_list(sv2017::Checker_port_listContext *ctx) = 0;
  virtual void exitChecker_port_list(sv2017::Checker_port_listContext *ctx) = 0;

  virtual void enterChecker_port_item(sv2017::Checker_port_itemContext *ctx) = 0;
  virtual void exitChecker_port_item(sv2017::Checker_port_itemContext *ctx) = 0;

  virtual void enterChecker_port_direction(sv2017::Checker_port_directionContext *ctx) = 0;
  virtual void exitChecker_port_direction(sv2017::Checker_port_directionContext *ctx) = 0;

  virtual void enterChecker_declaration(sv2017::Checker_declarationContext *ctx) = 0;
  virtual void exitChecker_declaration(sv2017::Checker_declarationContext *ctx) = 0;

  virtual void enterClass_declaration(sv2017::Class_declarationContext *ctx) = 0;
  virtual void exitClass_declaration(sv2017::Class_declarationContext *ctx) = 0;

  virtual void enterAlways_construct(sv2017::Always_constructContext *ctx) = 0;
  virtual void exitAlways_construct(sv2017::Always_constructContext *ctx) = 0;

  virtual void enterInterface_class_type(sv2017::Interface_class_typeContext *ctx) = 0;
  virtual void exitInterface_class_type(sv2017::Interface_class_typeContext *ctx) = 0;

  virtual void enterInterface_class_declaration(sv2017::Interface_class_declarationContext *ctx) = 0;
  virtual void exitInterface_class_declaration(sv2017::Interface_class_declarationContext *ctx) = 0;

  virtual void enterInterface_class_item(sv2017::Interface_class_itemContext *ctx) = 0;
  virtual void exitInterface_class_item(sv2017::Interface_class_itemContext *ctx) = 0;

  virtual void enterInterface_class_method(sv2017::Interface_class_methodContext *ctx) = 0;
  virtual void exitInterface_class_method(sv2017::Interface_class_methodContext *ctx) = 0;

  virtual void enterPackage_declaration(sv2017::Package_declarationContext *ctx) = 0;
  virtual void exitPackage_declaration(sv2017::Package_declarationContext *ctx) = 0;

  virtual void enterPackage_item(sv2017::Package_itemContext *ctx) = 0;
  virtual void exitPackage_item(sv2017::Package_itemContext *ctx) = 0;

  virtual void enterProgram_declaration(sv2017::Program_declarationContext *ctx) = 0;
  virtual void exitProgram_declaration(sv2017::Program_declarationContext *ctx) = 0;

  virtual void enterProgram_header(sv2017::Program_headerContext *ctx) = 0;
  virtual void exitProgram_header(sv2017::Program_headerContext *ctx) = 0;

  virtual void enterProgram_item(sv2017::Program_itemContext *ctx) = 0;
  virtual void exitProgram_item(sv2017::Program_itemContext *ctx) = 0;

  virtual void enterNon_port_program_item(sv2017::Non_port_program_itemContext *ctx) = 0;
  virtual void exitNon_port_program_item(sv2017::Non_port_program_itemContext *ctx) = 0;

  virtual void enterAnonymous_program(sv2017::Anonymous_programContext *ctx) = 0;
  virtual void exitAnonymous_program(sv2017::Anonymous_programContext *ctx) = 0;

  virtual void enterAnonymous_program_item(sv2017::Anonymous_program_itemContext *ctx) = 0;
  virtual void exitAnonymous_program_item(sv2017::Anonymous_program_itemContext *ctx) = 0;

  virtual void enterSequence_declaration(sv2017::Sequence_declarationContext *ctx) = 0;
  virtual void exitSequence_declaration(sv2017::Sequence_declarationContext *ctx) = 0;

  virtual void enterSequence_port_list(sv2017::Sequence_port_listContext *ctx) = 0;
  virtual void exitSequence_port_list(sv2017::Sequence_port_listContext *ctx) = 0;

  virtual void enterSequence_port_item(sv2017::Sequence_port_itemContext *ctx) = 0;
  virtual void exitSequence_port_item(sv2017::Sequence_port_itemContext *ctx) = 0;

  virtual void enterProperty_declaration(sv2017::Property_declarationContext *ctx) = 0;
  virtual void exitProperty_declaration(sv2017::Property_declarationContext *ctx) = 0;

  virtual void enterProperty_port_list(sv2017::Property_port_listContext *ctx) = 0;
  virtual void exitProperty_port_list(sv2017::Property_port_listContext *ctx) = 0;

  virtual void enterProperty_port_item(sv2017::Property_port_itemContext *ctx) = 0;
  virtual void exitProperty_port_item(sv2017::Property_port_itemContext *ctx) = 0;

  virtual void enterContinuous_assign(sv2017::Continuous_assignContext *ctx) = 0;
  virtual void exitContinuous_assign(sv2017::Continuous_assignContext *ctx) = 0;

  virtual void enterChecker_or_generate_item(sv2017::Checker_or_generate_itemContext *ctx) = 0;
  virtual void exitChecker_or_generate_item(sv2017::Checker_or_generate_itemContext *ctx) = 0;

  virtual void enterConstraint_prototype(sv2017::Constraint_prototypeContext *ctx) = 0;
  virtual void exitConstraint_prototype(sv2017::Constraint_prototypeContext *ctx) = 0;

  virtual void enterClass_constraint(sv2017::Class_constraintContext *ctx) = 0;
  virtual void exitClass_constraint(sv2017::Class_constraintContext *ctx) = 0;

  virtual void enterConstraint_declaration(sv2017::Constraint_declarationContext *ctx) = 0;
  virtual void exitConstraint_declaration(sv2017::Constraint_declarationContext *ctx) = 0;

  virtual void enterClass_constructor_declaration(sv2017::Class_constructor_declarationContext *ctx) = 0;
  virtual void exitClass_constructor_declaration(sv2017::Class_constructor_declarationContext *ctx) = 0;

  virtual void enterClass_property(sv2017::Class_propertyContext *ctx) = 0;
  virtual void exitClass_property(sv2017::Class_propertyContext *ctx) = 0;

  virtual void enterClass_method(sv2017::Class_methodContext *ctx) = 0;
  virtual void exitClass_method(sv2017::Class_methodContext *ctx) = 0;

  virtual void enterClass_constructor_prototype(sv2017::Class_constructor_prototypeContext *ctx) = 0;
  virtual void exitClass_constructor_prototype(sv2017::Class_constructor_prototypeContext *ctx) = 0;

  virtual void enterClass_item(sv2017::Class_itemContext *ctx) = 0;
  virtual void exitClass_item(sv2017::Class_itemContext *ctx) = 0;

  virtual void enterParameter_override(sv2017::Parameter_overrideContext *ctx) = 0;
  virtual void exitParameter_override(sv2017::Parameter_overrideContext *ctx) = 0;

  virtual void enterGate_instantiation(sv2017::Gate_instantiationContext *ctx) = 0;
  virtual void exitGate_instantiation(sv2017::Gate_instantiationContext *ctx) = 0;

  virtual void enterEnable_gate_or_mos_switch_or_cmos_switch_instance(sv2017::Enable_gate_or_mos_switch_or_cmos_switch_instanceContext *ctx) = 0;
  virtual void exitEnable_gate_or_mos_switch_or_cmos_switch_instance(sv2017::Enable_gate_or_mos_switch_or_cmos_switch_instanceContext *ctx) = 0;

  virtual void enterN_input_gate_instance(sv2017::N_input_gate_instanceContext *ctx) = 0;
  virtual void exitN_input_gate_instance(sv2017::N_input_gate_instanceContext *ctx) = 0;

  virtual void enterN_output_gate_instance(sv2017::N_output_gate_instanceContext *ctx) = 0;
  virtual void exitN_output_gate_instance(sv2017::N_output_gate_instanceContext *ctx) = 0;

  virtual void enterPass_switch_instance(sv2017::Pass_switch_instanceContext *ctx) = 0;
  virtual void exitPass_switch_instance(sv2017::Pass_switch_instanceContext *ctx) = 0;

  virtual void enterPass_enable_switch_instance(sv2017::Pass_enable_switch_instanceContext *ctx) = 0;
  virtual void exitPass_enable_switch_instance(sv2017::Pass_enable_switch_instanceContext *ctx) = 0;

  virtual void enterPull_gate_instance(sv2017::Pull_gate_instanceContext *ctx) = 0;
  virtual void exitPull_gate_instance(sv2017::Pull_gate_instanceContext *ctx) = 0;

  virtual void enterPulldown_strength(sv2017::Pulldown_strengthContext *ctx) = 0;
  virtual void exitPulldown_strength(sv2017::Pulldown_strengthContext *ctx) = 0;

  virtual void enterPullup_strength(sv2017::Pullup_strengthContext *ctx) = 0;
  virtual void exitPullup_strength(sv2017::Pullup_strengthContext *ctx) = 0;

  virtual void enterEnable_terminal(sv2017::Enable_terminalContext *ctx) = 0;
  virtual void exitEnable_terminal(sv2017::Enable_terminalContext *ctx) = 0;

  virtual void enterInout_terminal(sv2017::Inout_terminalContext *ctx) = 0;
  virtual void exitInout_terminal(sv2017::Inout_terminalContext *ctx) = 0;

  virtual void enterInput_terminal(sv2017::Input_terminalContext *ctx) = 0;
  virtual void exitInput_terminal(sv2017::Input_terminalContext *ctx) = 0;

  virtual void enterOutput_terminal(sv2017::Output_terminalContext *ctx) = 0;
  virtual void exitOutput_terminal(sv2017::Output_terminalContext *ctx) = 0;

  virtual void enterUdp_instantiation(sv2017::Udp_instantiationContext *ctx) = 0;
  virtual void exitUdp_instantiation(sv2017::Udp_instantiationContext *ctx) = 0;

  virtual void enterUdp_instance(sv2017::Udp_instanceContext *ctx) = 0;
  virtual void exitUdp_instance(sv2017::Udp_instanceContext *ctx) = 0;

  virtual void enterUdp_instance_body(sv2017::Udp_instance_bodyContext *ctx) = 0;
  virtual void exitUdp_instance_body(sv2017::Udp_instance_bodyContext *ctx) = 0;

  virtual void enterModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *ctx) = 0;
  virtual void exitModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *ctx) = 0;

  virtual void enterHierarchical_instance(sv2017::Hierarchical_instanceContext *ctx) = 0;
  virtual void exitHierarchical_instance(sv2017::Hierarchical_instanceContext *ctx) = 0;

  virtual void enterList_of_port_connections(sv2017::List_of_port_connectionsContext *ctx) = 0;
  virtual void exitList_of_port_connections(sv2017::List_of_port_connectionsContext *ctx) = 0;

  virtual void enterOrdered_port_connection(sv2017::Ordered_port_connectionContext *ctx) = 0;
  virtual void exitOrdered_port_connection(sv2017::Ordered_port_connectionContext *ctx) = 0;

  virtual void enterNamed_port_connection(sv2017::Named_port_connectionContext *ctx) = 0;
  virtual void exitNamed_port_connection(sv2017::Named_port_connectionContext *ctx) = 0;

  virtual void enterPort_expression_connection(sv2017::Port_expression_connectionContext *ctx) = 0;
  virtual void exitPort_expression_connection(sv2017::Port_expression_connectionContext *ctx) = 0;

  virtual void enterPort_concatenation_connection(sv2017::Port_concatenation_connectionContext *ctx) = 0;
  virtual void exitPort_concatenation_connection(sv2017::Port_concatenation_connectionContext *ctx) = 0;

  virtual void enterConcatenation_item(sv2017::Concatenation_itemContext *ctx) = 0;
  virtual void exitConcatenation_item(sv2017::Concatenation_itemContext *ctx) = 0;

  virtual void enterPort_replication_connection(sv2017::Port_replication_connectionContext *ctx) = 0;
  virtual void exitPort_replication_connection(sv2017::Port_replication_connectionContext *ctx) = 0;

  virtual void enterBind_directive(sv2017::Bind_directiveContext *ctx) = 0;
  virtual void exitBind_directive(sv2017::Bind_directiveContext *ctx) = 0;

  virtual void enterBind_target_instance(sv2017::Bind_target_instanceContext *ctx) = 0;
  virtual void exitBind_target_instance(sv2017::Bind_target_instanceContext *ctx) = 0;

  virtual void enterBind_target_instance_list(sv2017::Bind_target_instance_listContext *ctx) = 0;
  virtual void exitBind_target_instance_list(sv2017::Bind_target_instance_listContext *ctx) = 0;

  virtual void enterBind_instantiation(sv2017::Bind_instantiationContext *ctx) = 0;
  virtual void exitBind_instantiation(sv2017::Bind_instantiationContext *ctx) = 0;

  virtual void enterConfig_declaration(sv2017::Config_declarationContext *ctx) = 0;
  virtual void exitConfig_declaration(sv2017::Config_declarationContext *ctx) = 0;

  virtual void enterDesign_statement(sv2017::Design_statementContext *ctx) = 0;
  virtual void exitDesign_statement(sv2017::Design_statementContext *ctx) = 0;

  virtual void enterConfig_rule_statement(sv2017::Config_rule_statementContext *ctx) = 0;
  virtual void exitConfig_rule_statement(sv2017::Config_rule_statementContext *ctx) = 0;

  virtual void enterInst_clause(sv2017::Inst_clauseContext *ctx) = 0;
  virtual void exitInst_clause(sv2017::Inst_clauseContext *ctx) = 0;

  virtual void enterInst_name(sv2017::Inst_nameContext *ctx) = 0;
  virtual void exitInst_name(sv2017::Inst_nameContext *ctx) = 0;

  virtual void enterCell_clause(sv2017::Cell_clauseContext *ctx) = 0;
  virtual void exitCell_clause(sv2017::Cell_clauseContext *ctx) = 0;

  virtual void enterLiblist_clause(sv2017::Liblist_clauseContext *ctx) = 0;
  virtual void exitLiblist_clause(sv2017::Liblist_clauseContext *ctx) = 0;

  virtual void enterUse_clause(sv2017::Use_clauseContext *ctx) = 0;
  virtual void exitUse_clause(sv2017::Use_clauseContext *ctx) = 0;

  virtual void enterNet_alias(sv2017::Net_aliasContext *ctx) = 0;
  virtual void exitNet_alias(sv2017::Net_aliasContext *ctx) = 0;

  virtual void enterSpecify_block(sv2017::Specify_blockContext *ctx) = 0;
  virtual void exitSpecify_block(sv2017::Specify_blockContext *ctx) = 0;

  virtual void enterGenerate_region(sv2017::Generate_regionContext *ctx) = 0;
  virtual void exitGenerate_region(sv2017::Generate_regionContext *ctx) = 0;

  virtual void enterGenvar_expression(sv2017::Genvar_expressionContext *ctx) = 0;
  virtual void exitGenvar_expression(sv2017::Genvar_expressionContext *ctx) = 0;

  virtual void enterLoop_generate_construct(sv2017::Loop_generate_constructContext *ctx) = 0;
  virtual void exitLoop_generate_construct(sv2017::Loop_generate_constructContext *ctx) = 0;

  virtual void enterGenvar_initialization(sv2017::Genvar_initializationContext *ctx) = 0;
  virtual void exitGenvar_initialization(sv2017::Genvar_initializationContext *ctx) = 0;

  virtual void enterGenvar_iteration(sv2017::Genvar_iterationContext *ctx) = 0;
  virtual void exitGenvar_iteration(sv2017::Genvar_iterationContext *ctx) = 0;

  virtual void enterConditional_generate_construct(sv2017::Conditional_generate_constructContext *ctx) = 0;
  virtual void exitConditional_generate_construct(sv2017::Conditional_generate_constructContext *ctx) = 0;

  virtual void enterIf_generate_construct(sv2017::If_generate_constructContext *ctx) = 0;
  virtual void exitIf_generate_construct(sv2017::If_generate_constructContext *ctx) = 0;

  virtual void enterCase_generate_construct(sv2017::Case_generate_constructContext *ctx) = 0;
  virtual void exitCase_generate_construct(sv2017::Case_generate_constructContext *ctx) = 0;

  virtual void enterCase_generate_item(sv2017::Case_generate_itemContext *ctx) = 0;
  virtual void exitCase_generate_item(sv2017::Case_generate_itemContext *ctx) = 0;

  virtual void enterGenerate_begin_end_block(sv2017::Generate_begin_end_blockContext *ctx) = 0;
  virtual void exitGenerate_begin_end_block(sv2017::Generate_begin_end_blockContext *ctx) = 0;

  virtual void enterGenerate_item(sv2017::Generate_itemContext *ctx) = 0;
  virtual void exitGenerate_item(sv2017::Generate_itemContext *ctx) = 0;

  virtual void enterProgram_generate_item(sv2017::Program_generate_itemContext *ctx) = 0;
  virtual void exitProgram_generate_item(sv2017::Program_generate_itemContext *ctx) = 0;

  virtual void enterModule_or_generate_or_interface_or_checker_item(sv2017::Module_or_generate_or_interface_or_checker_itemContext *ctx) = 0;
  virtual void exitModule_or_generate_or_interface_or_checker_item(sv2017::Module_or_generate_or_interface_or_checker_itemContext *ctx) = 0;

  virtual void enterModule_or_generate_or_interface_item(sv2017::Module_or_generate_or_interface_itemContext *ctx) = 0;
  virtual void exitModule_or_generate_or_interface_item(sv2017::Module_or_generate_or_interface_itemContext *ctx) = 0;

  virtual void enterModule_or_generate_item(sv2017::Module_or_generate_itemContext *ctx) = 0;
  virtual void exitModule_or_generate_item(sv2017::Module_or_generate_itemContext *ctx) = 0;

  virtual void enterElaboration_system_task(sv2017::Elaboration_system_taskContext *ctx) = 0;
  virtual void exitElaboration_system_task(sv2017::Elaboration_system_taskContext *ctx) = 0;

  virtual void enterModule_item_item(sv2017::Module_item_itemContext *ctx) = 0;
  virtual void exitModule_item_item(sv2017::Module_item_itemContext *ctx) = 0;

  virtual void enterModule_item(sv2017::Module_itemContext *ctx) = 0;
  virtual void exitModule_item(sv2017::Module_itemContext *ctx) = 0;


};

}  // namespace mgp_sv
