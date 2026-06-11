
// Generated from /Users/filss/git/makefile_gen/grammars/sv2017.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "sv2017.h"


namespace mgp_sv {

/**
 * This class defines an abstract visitor for a parse tree
 * produced by sv2017.
 */
class  sv2017Visitor : public antlr4::tree::AbstractParseTreeVisitor {
public:

  /**
   * Visit parse trees produced by sv2017.
   */
    virtual std::any visitSource_text(sv2017::Source_textContext *context) = 0;

    virtual std::any visitDescription(sv2017::DescriptionContext *context) = 0;

    virtual std::any visitAssignment_operator(sv2017::Assignment_operatorContext *context) = 0;

    virtual std::any visitEdge_identifier(sv2017::Edge_identifierContext *context) = 0;

    virtual std::any visitIdentifier(sv2017::IdentifierContext *context) = 0;

    virtual std::any visitInteger_type(sv2017::Integer_typeContext *context) = 0;

    virtual std::any visitInteger_atom_type(sv2017::Integer_atom_typeContext *context) = 0;

    virtual std::any visitInteger_vector_type(sv2017::Integer_vector_typeContext *context) = 0;

    virtual std::any visitNon_integer_type(sv2017::Non_integer_typeContext *context) = 0;

    virtual std::any visitNet_type(sv2017::Net_typeContext *context) = 0;

    virtual std::any visitUnary_module_path_operator(sv2017::Unary_module_path_operatorContext *context) = 0;

    virtual std::any visitUnary_operator(sv2017::Unary_operatorContext *context) = 0;

    virtual std::any visitInc_or_dec_operator(sv2017::Inc_or_dec_operatorContext *context) = 0;

    virtual std::any visitImplicit_class_handle(sv2017::Implicit_class_handleContext *context) = 0;

    virtual std::any visitIntegral_number(sv2017::Integral_numberContext *context) = 0;

    virtual std::any visitReal_number(sv2017::Real_numberContext *context) = 0;

    virtual std::any visitAny_system_tf_identifier(sv2017::Any_system_tf_identifierContext *context) = 0;

    virtual std::any visitSigning(sv2017::SigningContext *context) = 0;

    virtual std::any visitNumber(sv2017::NumberContext *context) = 0;

    virtual std::any visitTimeunits_declaration(sv2017::Timeunits_declarationContext *context) = 0;

    virtual std::any visitLifetime(sv2017::LifetimeContext *context) = 0;

    virtual std::any visitPort_direction(sv2017::Port_directionContext *context) = 0;

    virtual std::any visitAlways_keyword(sv2017::Always_keywordContext *context) = 0;

    virtual std::any visitJoin_keyword(sv2017::Join_keywordContext *context) = 0;

    virtual std::any visitUnique_priority(sv2017::Unique_priorityContext *context) = 0;

    virtual std::any visitDrive_strength(sv2017::Drive_strengthContext *context) = 0;

    virtual std::any visitStrength0(sv2017::Strength0Context *context) = 0;

    virtual std::any visitStrength1(sv2017::Strength1Context *context) = 0;

    virtual std::any visitCharge_strength(sv2017::Charge_strengthContext *context) = 0;

    virtual std::any visitSequence_lvar_port_direction(sv2017::Sequence_lvar_port_directionContext *context) = 0;

    virtual std::any visitBins_keyword(sv2017::Bins_keywordContext *context) = 0;

    virtual std::any visitClass_item_qualifier(sv2017::Class_item_qualifierContext *context) = 0;

    virtual std::any visitRandom_qualifier(sv2017::Random_qualifierContext *context) = 0;

    virtual std::any visitProperty_qualifier(sv2017::Property_qualifierContext *context) = 0;

    virtual std::any visitMethod_qualifier(sv2017::Method_qualifierContext *context) = 0;

    virtual std::any visitConstraint_prototype_qualifier(sv2017::Constraint_prototype_qualifierContext *context) = 0;

    virtual std::any visitCmos_switchtype(sv2017::Cmos_switchtypeContext *context) = 0;

    virtual std::any visitEnable_gatetype(sv2017::Enable_gatetypeContext *context) = 0;

    virtual std::any visitMos_switchtype(sv2017::Mos_switchtypeContext *context) = 0;

    virtual std::any visitN_input_gatetype(sv2017::N_input_gatetypeContext *context) = 0;

    virtual std::any visitN_output_gatetype(sv2017::N_output_gatetypeContext *context) = 0;

    virtual std::any visitPass_en_switchtype(sv2017::Pass_en_switchtypeContext *context) = 0;

    virtual std::any visitPass_switchtype(sv2017::Pass_switchtypeContext *context) = 0;

    virtual std::any visitAny_implication(sv2017::Any_implicationContext *context) = 0;

    virtual std::any visitTiming_check_event_control(sv2017::Timing_check_event_controlContext *context) = 0;

    virtual std::any visitImport_export(sv2017::Import_exportContext *context) = 0;

    virtual std::any visitArray_method_name(sv2017::Array_method_nameContext *context) = 0;

    virtual std::any visitOperator_mul_div_mod(sv2017::Operator_mul_div_modContext *context) = 0;

    virtual std::any visitOperator_plus_minus(sv2017::Operator_plus_minusContext *context) = 0;

    virtual std::any visitOperator_shift(sv2017::Operator_shiftContext *context) = 0;

    virtual std::any visitOperator_cmp(sv2017::Operator_cmpContext *context) = 0;

    virtual std::any visitOperator_eq_neq(sv2017::Operator_eq_neqContext *context) = 0;

    virtual std::any visitOperator_xor(sv2017::Operator_xorContext *context) = 0;

    virtual std::any visitOperator_impl(sv2017::Operator_implContext *context) = 0;

    virtual std::any visitUdp_nonansi_declaration(sv2017::Udp_nonansi_declarationContext *context) = 0;

    virtual std::any visitUdp_ansi_declaration(sv2017::Udp_ansi_declarationContext *context) = 0;

    virtual std::any visitUdp_declaration(sv2017::Udp_declarationContext *context) = 0;

    virtual std::any visitUdp_declaration_port_list(sv2017::Udp_declaration_port_listContext *context) = 0;

    virtual std::any visitUdp_port_declaration(sv2017::Udp_port_declarationContext *context) = 0;

    virtual std::any visitUdp_output_declaration(sv2017::Udp_output_declarationContext *context) = 0;

    virtual std::any visitUdp_input_declaration(sv2017::Udp_input_declarationContext *context) = 0;

    virtual std::any visitUdp_reg_declaration(sv2017::Udp_reg_declarationContext *context) = 0;

    virtual std::any visitUdp_body(sv2017::Udp_bodyContext *context) = 0;

    virtual std::any visitCombinational_body(sv2017::Combinational_bodyContext *context) = 0;

    virtual std::any visitCombinational_entry(sv2017::Combinational_entryContext *context) = 0;

    virtual std::any visitSequential_body(sv2017::Sequential_bodyContext *context) = 0;

    virtual std::any visitUdp_initial_statement(sv2017::Udp_initial_statementContext *context) = 0;

    virtual std::any visitSequential_entry(sv2017::Sequential_entryContext *context) = 0;

    virtual std::any visitSeq_input_list(sv2017::Seq_input_listContext *context) = 0;

    virtual std::any visitLevel_input_list(sv2017::Level_input_listContext *context) = 0;

    virtual std::any visitEdge_input_list(sv2017::Edge_input_listContext *context) = 0;

    virtual std::any visitEdge_indicator(sv2017::Edge_indicatorContext *context) = 0;

    virtual std::any visitCurrent_state(sv2017::Current_stateContext *context) = 0;

    virtual std::any visitNext_state(sv2017::Next_stateContext *context) = 0;

    virtual std::any visitInterface_declaration(sv2017::Interface_declarationContext *context) = 0;

    virtual std::any visitInterface_header(sv2017::Interface_headerContext *context) = 0;

    virtual std::any visitInterface_item(sv2017::Interface_itemContext *context) = 0;

    virtual std::any visitModport_declaration(sv2017::Modport_declarationContext *context) = 0;

    virtual std::any visitModport_item(sv2017::Modport_itemContext *context) = 0;

    virtual std::any visitModport_ports_declaration(sv2017::Modport_ports_declarationContext *context) = 0;

    virtual std::any visitModport_clocking_declaration(sv2017::Modport_clocking_declarationContext *context) = 0;

    virtual std::any visitModport_simple_ports_declaration(sv2017::Modport_simple_ports_declarationContext *context) = 0;

    virtual std::any visitModport_simple_port(sv2017::Modport_simple_portContext *context) = 0;

    virtual std::any visitModport_tf_ports_declaration(sv2017::Modport_tf_ports_declarationContext *context) = 0;

    virtual std::any visitModport_tf_port(sv2017::Modport_tf_portContext *context) = 0;

    virtual std::any visitStatement_or_null(sv2017::Statement_or_nullContext *context) = 0;

    virtual std::any visitInitial_construct(sv2017::Initial_constructContext *context) = 0;

    virtual std::any visitDefault_clocking_or_dissable_construct(sv2017::Default_clocking_or_dissable_constructContext *context) = 0;

    virtual std::any visitStatement(sv2017::StatementContext *context) = 0;

    virtual std::any visitStatement_item(sv2017::Statement_itemContext *context) = 0;

    virtual std::any visitCycle_delay(sv2017::Cycle_delayContext *context) = 0;

    virtual std::any visitClocking_drive(sv2017::Clocking_driveContext *context) = 0;

    virtual std::any visitClockvar_expression(sv2017::Clockvar_expressionContext *context) = 0;

    virtual std::any visitFinal_construct(sv2017::Final_constructContext *context) = 0;

    virtual std::any visitBlocking_assignment(sv2017::Blocking_assignmentContext *context) = 0;

    virtual std::any visitProcedural_timing_control_statement(sv2017::Procedural_timing_control_statementContext *context) = 0;

    virtual std::any visitProcedural_timing_control(sv2017::Procedural_timing_controlContext *context) = 0;

    virtual std::any visitEvent_control(sv2017::Event_controlContext *context) = 0;

    virtual std::any visitDelay_or_event_control(sv2017::Delay_or_event_controlContext *context) = 0;

    virtual std::any visitDelay3(sv2017::Delay3Context *context) = 0;

    virtual std::any visitDelay2(sv2017::Delay2Context *context) = 0;

    virtual std::any visitDelay_value(sv2017::Delay_valueContext *context) = 0;

    virtual std::any visitDelay_control(sv2017::Delay_controlContext *context) = 0;

    virtual std::any visitNonblocking_assignment(sv2017::Nonblocking_assignmentContext *context) = 0;

    virtual std::any visitProcedural_continuous_assignment(sv2017::Procedural_continuous_assignmentContext *context) = 0;

    virtual std::any visitVariable_assignment(sv2017::Variable_assignmentContext *context) = 0;

    virtual std::any visitAction_block(sv2017::Action_blockContext *context) = 0;

    virtual std::any visitSeq_block(sv2017::Seq_blockContext *context) = 0;

    virtual std::any visitPar_block(sv2017::Par_blockContext *context) = 0;

    virtual std::any visitCase_statement(sv2017::Case_statementContext *context) = 0;

    virtual std::any visitCase_keyword(sv2017::Case_keywordContext *context) = 0;

    virtual std::any visitCase_item(sv2017::Case_itemContext *context) = 0;

    virtual std::any visitCase_pattern_item(sv2017::Case_pattern_itemContext *context) = 0;

    virtual std::any visitCase_inside_item(sv2017::Case_inside_itemContext *context) = 0;

    virtual std::any visitRandcase_statement(sv2017::Randcase_statementContext *context) = 0;

    virtual std::any visitRandcase_item(sv2017::Randcase_itemContext *context) = 0;

    virtual std::any visitCond_predicate(sv2017::Cond_predicateContext *context) = 0;

    virtual std::any visitConditional_statement(sv2017::Conditional_statementContext *context) = 0;

    virtual std::any visitSubroutine_call_statement(sv2017::Subroutine_call_statementContext *context) = 0;

    virtual std::any visitDisable_statement(sv2017::Disable_statementContext *context) = 0;

    virtual std::any visitEvent_trigger(sv2017::Event_triggerContext *context) = 0;

    virtual std::any visitLoop_statement(sv2017::Loop_statementContext *context) = 0;

    virtual std::any visitList_of_variable_assignments(sv2017::List_of_variable_assignmentsContext *context) = 0;

    virtual std::any visitFor_initialization(sv2017::For_initializationContext *context) = 0;

    virtual std::any visitFor_end_expression(sv2017::For_end_expressionContext *context) = 0;

    virtual std::any visitFor_variable_declaration_var_assign(sv2017::For_variable_declaration_var_assignContext *context) = 0;

    virtual std::any visitFor_variable_declaration(sv2017::For_variable_declarationContext *context) = 0;

    virtual std::any visitFor_step(sv2017::For_stepContext *context) = 0;

    virtual std::any visitLoop_variables(sv2017::Loop_variablesContext *context) = 0;

    virtual std::any visitJump_statement(sv2017::Jump_statementContext *context) = 0;

    virtual std::any visitWait_statement(sv2017::Wait_statementContext *context) = 0;

    virtual std::any visitName_of_instance(sv2017::Name_of_instanceContext *context) = 0;

    virtual std::any visitChecker_instantiation(sv2017::Checker_instantiationContext *context) = 0;

    virtual std::any visitList_of_checker_port_connections(sv2017::List_of_checker_port_connectionsContext *context) = 0;

    virtual std::any visitOrdered_checker_port_connection(sv2017::Ordered_checker_port_connectionContext *context) = 0;

    virtual std::any visitNamed_checker_port_connection(sv2017::Named_checker_port_connectionContext *context) = 0;

    virtual std::any visitProcedural_assertion_statement(sv2017::Procedural_assertion_statementContext *context) = 0;

    virtual std::any visitConcurrent_assertion_statement(sv2017::Concurrent_assertion_statementContext *context) = 0;

    virtual std::any visitAssertion_item(sv2017::Assertion_itemContext *context) = 0;

    virtual std::any visitConcurrent_assertion_item(sv2017::Concurrent_assertion_itemContext *context) = 0;

    virtual std::any visitImmediate_assertion_statement(sv2017::Immediate_assertion_statementContext *context) = 0;

    virtual std::any visitSimple_immediate_assertion_statement(sv2017::Simple_immediate_assertion_statementContext *context) = 0;

    virtual std::any visitSimple_immediate_assert_statement(sv2017::Simple_immediate_assert_statementContext *context) = 0;

    virtual std::any visitSimple_immediate_assume_statement(sv2017::Simple_immediate_assume_statementContext *context) = 0;

    virtual std::any visitSimple_immediate_cover_statement(sv2017::Simple_immediate_cover_statementContext *context) = 0;

    virtual std::any visitDeferred_immediate_assertion_statement(sv2017::Deferred_immediate_assertion_statementContext *context) = 0;

    virtual std::any visitPrimitive_delay(sv2017::Primitive_delayContext *context) = 0;

    virtual std::any visitDeferred_immediate_assert_statement(sv2017::Deferred_immediate_assert_statementContext *context) = 0;

    virtual std::any visitDeferred_immediate_assume_statement(sv2017::Deferred_immediate_assume_statementContext *context) = 0;

    virtual std::any visitDeferred_immediate_cover_statement(sv2017::Deferred_immediate_cover_statementContext *context) = 0;

    virtual std::any visitWeight_specification(sv2017::Weight_specificationContext *context) = 0;

    virtual std::any visitProduction_item(sv2017::Production_itemContext *context) = 0;

    virtual std::any visitRs_code_block(sv2017::Rs_code_blockContext *context) = 0;

    virtual std::any visitRandsequence_statement(sv2017::Randsequence_statementContext *context) = 0;

    virtual std::any visitRs_prod(sv2017::Rs_prodContext *context) = 0;

    virtual std::any visitRs_if_else(sv2017::Rs_if_elseContext *context) = 0;

    virtual std::any visitRs_repeat(sv2017::Rs_repeatContext *context) = 0;

    virtual std::any visitRs_case(sv2017::Rs_caseContext *context) = 0;

    virtual std::any visitRs_case_item(sv2017::Rs_case_itemContext *context) = 0;

    virtual std::any visitRs_rule(sv2017::Rs_ruleContext *context) = 0;

    virtual std::any visitRs_production_list(sv2017::Rs_production_listContext *context) = 0;

    virtual std::any visitProduction(sv2017::ProductionContext *context) = 0;

    virtual std::any visitTf_item_declaration(sv2017::Tf_item_declarationContext *context) = 0;

    virtual std::any visitTf_port_list(sv2017::Tf_port_listContext *context) = 0;

    virtual std::any visitTf_port_item(sv2017::Tf_port_itemContext *context) = 0;

    virtual std::any visitTf_port_direction(sv2017::Tf_port_directionContext *context) = 0;

    virtual std::any visitTf_port_declaration(sv2017::Tf_port_declarationContext *context) = 0;

    virtual std::any visitList_of_tf_variable_identifiers_item(sv2017::List_of_tf_variable_identifiers_itemContext *context) = 0;

    virtual std::any visitList_of_tf_variable_identifiers(sv2017::List_of_tf_variable_identifiersContext *context) = 0;

    virtual std::any visitExpect_property_statement(sv2017::Expect_property_statementContext *context) = 0;

    virtual std::any visitBlock_item_declaration(sv2017::Block_item_declarationContext *context) = 0;

    virtual std::any visitParam_assignment(sv2017::Param_assignmentContext *context) = 0;

    virtual std::any visitType_assignment(sv2017::Type_assignmentContext *context) = 0;

    virtual std::any visitList_of_type_assignments(sv2017::List_of_type_assignmentsContext *context) = 0;

    virtual std::any visitList_of_param_assignments(sv2017::List_of_param_assignmentsContext *context) = 0;

    virtual std::any visitLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *context) = 0;

    virtual std::any visitParameter_declaration(sv2017::Parameter_declarationContext *context) = 0;

    virtual std::any visitType_declaration(sv2017::Type_declarationContext *context) = 0;

    virtual std::any visitNet_type_declaration(sv2017::Net_type_declarationContext *context) = 0;

    virtual std::any visitLet_declaration(sv2017::Let_declarationContext *context) = 0;

    virtual std::any visitLet_port_list(sv2017::Let_port_listContext *context) = 0;

    virtual std::any visitLet_port_item(sv2017::Let_port_itemContext *context) = 0;

    virtual std::any visitLet_formal_type(sv2017::Let_formal_typeContext *context) = 0;

    virtual std::any visitPackage_import_declaration(sv2017::Package_import_declarationContext *context) = 0;

    virtual std::any visitPackage_import_item(sv2017::Package_import_itemContext *context) = 0;

    virtual std::any visitProperty_list_of_arguments(sv2017::Property_list_of_argumentsContext *context) = 0;

    virtual std::any visitProperty_actual_arg(sv2017::Property_actual_argContext *context) = 0;

    virtual std::any visitProperty_formal_type(sv2017::Property_formal_typeContext *context) = 0;

    virtual std::any visitSequence_formal_type(sv2017::Sequence_formal_typeContext *context) = 0;

    virtual std::any visitProperty_instance(sv2017::Property_instanceContext *context) = 0;

    virtual std::any visitProperty_spec(sv2017::Property_specContext *context) = 0;

    virtual std::any visitProperty_expr(sv2017::Property_exprContext *context) = 0;

    virtual std::any visitProperty_case_item(sv2017::Property_case_itemContext *context) = 0;

    virtual std::any visitBit_select(sv2017::Bit_selectContext *context) = 0;

    virtual std::any visitIdentifier_with_bit_select(sv2017::Identifier_with_bit_selectContext *context) = 0;

    virtual std::any visitPackage_or_class_scoped_hier_id_with_select(sv2017::Package_or_class_scoped_hier_id_with_selectContext *context) = 0;

    virtual std::any visitPackage_or_class_scoped_path_item(sv2017::Package_or_class_scoped_path_itemContext *context) = 0;

    virtual std::any visitPackage_or_class_scoped_path(sv2017::Package_or_class_scoped_pathContext *context) = 0;

    virtual std::any visitHierarchical_identifier(sv2017::Hierarchical_identifierContext *context) = 0;

    virtual std::any visitPackage_or_class_scoped_id(sv2017::Package_or_class_scoped_idContext *context) = 0;

    virtual std::any visitSelect(sv2017::SelectContext *context) = 0;

    virtual std::any visitEvent_expression_item(sv2017::Event_expression_itemContext *context) = 0;

    virtual std::any visitEvent_expression(sv2017::Event_expressionContext *context) = 0;

    virtual std::any visitBoolean_abbrev(sv2017::Boolean_abbrevContext *context) = 0;

    virtual std::any visitSequence_abbrev(sv2017::Sequence_abbrevContext *context) = 0;

    virtual std::any visitConsecutive_repetition(sv2017::Consecutive_repetitionContext *context) = 0;

    virtual std::any visitNon_consecutive_repetition(sv2017::Non_consecutive_repetitionContext *context) = 0;

    virtual std::any visitGoto_repetition(sv2017::Goto_repetitionContext *context) = 0;

    virtual std::any visitCycle_delay_const_range_expression(sv2017::Cycle_delay_const_range_expressionContext *context) = 0;

    virtual std::any visitSequence_instance(sv2017::Sequence_instanceContext *context) = 0;

    virtual std::any visitSequence_expr(sv2017::Sequence_exprContext *context) = 0;

    virtual std::any visitSequence_match_item(sv2017::Sequence_match_itemContext *context) = 0;

    virtual std::any visitOperator_assignment(sv2017::Operator_assignmentContext *context) = 0;

    virtual std::any visitSequence_actual_arg(sv2017::Sequence_actual_argContext *context) = 0;

    virtual std::any visitDist_weight(sv2017::Dist_weightContext *context) = 0;

    virtual std::any visitClocking_declaration(sv2017::Clocking_declarationContext *context) = 0;

    virtual std::any visitClocking_item(sv2017::Clocking_itemContext *context) = 0;

    virtual std::any visitList_of_clocking_decl_assign(sv2017::List_of_clocking_decl_assignContext *context) = 0;

    virtual std::any visitClocking_decl_assign(sv2017::Clocking_decl_assignContext *context) = 0;

    virtual std::any visitDefault_skew(sv2017::Default_skewContext *context) = 0;

    virtual std::any visitClocking_direction(sv2017::Clocking_directionContext *context) = 0;

    virtual std::any visitClocking_skew(sv2017::Clocking_skewContext *context) = 0;

    virtual std::any visitClocking_event(sv2017::Clocking_eventContext *context) = 0;

    virtual std::any visitCycle_delay_range(sv2017::Cycle_delay_rangeContext *context) = 0;

    virtual std::any visitExpression_or_dist(sv2017::Expression_or_distContext *context) = 0;

    virtual std::any visitCovergroup_declaration(sv2017::Covergroup_declarationContext *context) = 0;

    virtual std::any visitCover_cross(sv2017::Cover_crossContext *context) = 0;

    virtual std::any visitIdentifier_list_2plus(sv2017::Identifier_list_2plusContext *context) = 0;

    virtual std::any visitCross_body(sv2017::Cross_bodyContext *context) = 0;

    virtual std::any visitCross_body_item(sv2017::Cross_body_itemContext *context) = 0;

    virtual std::any visitBins_selection_or_option(sv2017::Bins_selection_or_optionContext *context) = 0;

    virtual std::any visitBins_selection(sv2017::Bins_selectionContext *context) = 0;

    virtual std::any visitSelect_expression(sv2017::Select_expressionContext *context) = 0;

    virtual std::any visitSelect_condition(sv2017::Select_conditionContext *context) = 0;

    virtual std::any visitBins_expression(sv2017::Bins_expressionContext *context) = 0;

    virtual std::any visitCovergroup_range_list(sv2017::Covergroup_range_listContext *context) = 0;

    virtual std::any visitCovergroup_value_range(sv2017::Covergroup_value_rangeContext *context) = 0;

    virtual std::any visitCovergroup_expression(sv2017::Covergroup_expressionContext *context) = 0;

    virtual std::any visitCoverage_spec_or_option(sv2017::Coverage_spec_or_optionContext *context) = 0;

    virtual std::any visitCoverage_option(sv2017::Coverage_optionContext *context) = 0;

    virtual std::any visitCoverage_spec(sv2017::Coverage_specContext *context) = 0;

    virtual std::any visitCover_point(sv2017::Cover_pointContext *context) = 0;

    virtual std::any visitBins_or_empty(sv2017::Bins_or_emptyContext *context) = 0;

    virtual std::any visitBins_or_options(sv2017::Bins_or_optionsContext *context) = 0;

    virtual std::any visitTrans_list(sv2017::Trans_listContext *context) = 0;

    virtual std::any visitTrans_set(sv2017::Trans_setContext *context) = 0;

    virtual std::any visitTrans_range_list(sv2017::Trans_range_listContext *context) = 0;

    virtual std::any visitRepeat_range(sv2017::Repeat_rangeContext *context) = 0;

    virtual std::any visitCoverage_event(sv2017::Coverage_eventContext *context) = 0;

    virtual std::any visitBlock_event_expression(sv2017::Block_event_expressionContext *context) = 0;

    virtual std::any visitHierarchical_btf_identifier(sv2017::Hierarchical_btf_identifierContext *context) = 0;

    virtual std::any visitAssertion_variable_declaration(sv2017::Assertion_variable_declarationContext *context) = 0;

    virtual std::any visitDist_item(sv2017::Dist_itemContext *context) = 0;

    virtual std::any visitValue_range(sv2017::Value_rangeContext *context) = 0;

    virtual std::any visitAttribute_instance(sv2017::Attribute_instanceContext *context) = 0;

    virtual std::any visitAttr_spec(sv2017::Attr_specContext *context) = 0;

    virtual std::any visitClass_new(sv2017::Class_newContext *context) = 0;

    virtual std::any visitParam_expression(sv2017::Param_expressionContext *context) = 0;

    virtual std::any visitConstant_param_expression(sv2017::Constant_param_expressionContext *context) = 0;

    virtual std::any visitUnpacked_dimension(sv2017::Unpacked_dimensionContext *context) = 0;

    virtual std::any visitPacked_dimension(sv2017::Packed_dimensionContext *context) = 0;

    virtual std::any visitVariable_dimension(sv2017::Variable_dimensionContext *context) = 0;

    virtual std::any visitStruct_union(sv2017::Struct_unionContext *context) = 0;

    virtual std::any visitEnum_base_type(sv2017::Enum_base_typeContext *context) = 0;

    virtual std::any visitData_type_primitive(sv2017::Data_type_primitiveContext *context) = 0;

    virtual std::any visitData_type(sv2017::Data_typeContext *context) = 0;

    virtual std::any visitData_type_or_implicit(sv2017::Data_type_or_implicitContext *context) = 0;

    virtual std::any visitImplicit_data_type(sv2017::Implicit_data_typeContext *context) = 0;

    virtual std::any visitSequence_list_of_arguments_named_item(sv2017::Sequence_list_of_arguments_named_itemContext *context) = 0;

    virtual std::any visitSequence_list_of_arguments(sv2017::Sequence_list_of_argumentsContext *context) = 0;

    virtual std::any visitList_of_arguments_named_item(sv2017::List_of_arguments_named_itemContext *context) = 0;

    virtual std::any visitList_of_arguments(sv2017::List_of_argumentsContext *context) = 0;

    virtual std::any visitPrimary_literal(sv2017::Primary_literalContext *context) = 0;

    virtual std::any visitType_reference(sv2017::Type_referenceContext *context) = 0;

    virtual std::any visitPackage_scope(sv2017::Package_scopeContext *context) = 0;

    virtual std::any visitPs_identifier(sv2017::Ps_identifierContext *context) = 0;

    virtual std::any visitList_of_parameter_value_assignments(sv2017::List_of_parameter_value_assignmentsContext *context) = 0;

    virtual std::any visitParameter_value_assignment(sv2017::Parameter_value_assignmentContext *context) = 0;

    virtual std::any visitClass_type(sv2017::Class_typeContext *context) = 0;

    virtual std::any visitClass_scope(sv2017::Class_scopeContext *context) = 0;

    virtual std::any visitFirst_range_identifier(sv2017::First_range_identifierContext *context) = 0;

    virtual std::any visitSecond_range_identifier(sv2017::Second_range_identifierContext *context) = 0;

    virtual std::any visitRange_expression(sv2017::Range_expressionContext *context) = 0;

    virtual std::any visitConstant_range_expression(sv2017::Constant_range_expressionContext *context) = 0;

    virtual std::any visitConstant_mintypmax_expression(sv2017::Constant_mintypmax_expressionContext *context) = 0;

    virtual std::any visitMintypmax_expression(sv2017::Mintypmax_expressionContext *context) = 0;

    virtual std::any visitNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *context) = 0;

    virtual std::any visitPrimaryLit(sv2017::PrimaryLitContext *context) = 0;

    virtual std::any visitPrimaryRepl(sv2017::PrimaryReplContext *context) = 0;

    virtual std::any visitPrimaryRandomize(sv2017::PrimaryRandomizeContext *context) = 0;

    virtual std::any visitPrimaryAssig(sv2017::PrimaryAssigContext *context) = 0;

    virtual std::any visitPrimaryBitSelect(sv2017::PrimaryBitSelectContext *context) = 0;

    virtual std::any visitPrimaryTfCall(sv2017::PrimaryTfCallContext *context) = 0;

    virtual std::any visitPrimaryTypeRef(sv2017::PrimaryTypeRefContext *context) = 0;

    virtual std::any visitPrimaryCallArrayMethodNoArgs(sv2017::PrimaryCallArrayMethodNoArgsContext *context) = 0;

    virtual std::any visitPrimaryCast(sv2017::PrimaryCastContext *context) = 0;

    virtual std::any visitPrimaryPar(sv2017::PrimaryParContext *context) = 0;

    virtual std::any visitPrimaryCall(sv2017::PrimaryCallContext *context) = 0;

    virtual std::any visitPrimaryRandomize2(sv2017::PrimaryRandomize2Context *context) = 0;

    virtual std::any visitPrimaryDot(sv2017::PrimaryDotContext *context) = 0;

    virtual std::any visitPrimaryStreaming_concatenation(sv2017::PrimaryStreaming_concatenationContext *context) = 0;

    virtual std::any visitPrimaryPath(sv2017::PrimaryPathContext *context) = 0;

    virtual std::any visitPrimaryIndex(sv2017::PrimaryIndexContext *context) = 0;

    virtual std::any visitPrimaryCallWith(sv2017::PrimaryCallWithContext *context) = 0;

    virtual std::any visitPrimaryConcat(sv2017::PrimaryConcatContext *context) = 0;

    virtual std::any visitPrimaryCast2(sv2017::PrimaryCast2Context *context) = 0;

    virtual std::any visitCast_separator(sv2017::Cast_separatorContext *context) = 0;

    virtual std::any visitConstant_expression(sv2017::Constant_expressionContext *context) = 0;

    virtual std::any visitInc_or_dec_expressionPre(sv2017::Inc_or_dec_expressionPreContext *context) = 0;

    virtual std::any visitInc_or_dec_expressionPost(sv2017::Inc_or_dec_expressionPostContext *context) = 0;

    virtual std::any visitExpression(sv2017::ExpressionContext *context) = 0;

    virtual std::any visitOperator_bitwise_and(sv2017::Operator_bitwise_andContext *context) = 0;

    virtual std::any visitOperator_bitwise_or(sv2017::Operator_bitwise_orContext *context) = 0;

    virtual std::any visitReplication(sv2017::ReplicationContext *context) = 0;

    virtual std::any visitReplication_size(sv2017::Replication_sizeContext *context) = 0;

    virtual std::any visitReplication_value(sv2017::Replication_valueContext *context) = 0;

    virtual std::any visitConcatenation(sv2017::ConcatenationContext *context) = 0;

    virtual std::any visitDynamic_array_new(sv2017::Dynamic_array_newContext *context) = 0;

    virtual std::any visitConst_or_range_expression(sv2017::Const_or_range_expressionContext *context) = 0;

    virtual std::any visitVariable_decl_assignment(sv2017::Variable_decl_assignmentContext *context) = 0;

    virtual std::any visitAssignment_pattern_variable_lvalue(sv2017::Assignment_pattern_variable_lvalueContext *context) = 0;

    virtual std::any visitStream_operator(sv2017::Stream_operatorContext *context) = 0;

    virtual std::any visitSlice_size(sv2017::Slice_sizeContext *context) = 0;

    virtual std::any visitStreaming_concatenation(sv2017::Streaming_concatenationContext *context) = 0;

    virtual std::any visitStream_concatenation(sv2017::Stream_concatenationContext *context) = 0;

    virtual std::any visitStream_expression(sv2017::Stream_expressionContext *context) = 0;

    virtual std::any visitArray_range_expression(sv2017::Array_range_expressionContext *context) = 0;

    virtual std::any visitRange_separator(sv2017::Range_separatorContext *context) = 0;

    virtual std::any visitOpen_range_list(sv2017::Open_range_listContext *context) = 0;

    virtual std::any visitPattern(sv2017::PatternContext *context) = 0;

    virtual std::any visitAssignment_pattern(sv2017::Assignment_patternContext *context) = 0;

    virtual std::any visitReplication_assignment(sv2017::Replication_assignmentContext *context) = 0;

    virtual std::any visitStructure_pattern_key(sv2017::Structure_pattern_keyContext *context) = 0;

    virtual std::any visitArray_pattern_key(sv2017::Array_pattern_keyContext *context) = 0;

    virtual std::any visitAssignment_pattern_key(sv2017::Assignment_pattern_keyContext *context) = 0;

    virtual std::any visitStruct_union_member(sv2017::Struct_union_memberContext *context) = 0;

    virtual std::any visitData_type_or_void(sv2017::Data_type_or_voidContext *context) = 0;

    virtual std::any visitEnum_name_declaration(sv2017::Enum_name_declarationContext *context) = 0;

    virtual std::any visitAssignment_pattern_expression(sv2017::Assignment_pattern_expressionContext *context) = 0;

    virtual std::any visitAssignment_pattern_expression_type(sv2017::Assignment_pattern_expression_typeContext *context) = 0;

    virtual std::any visitNet_lvalue(sv2017::Net_lvalueContext *context) = 0;

    virtual std::any visitVariable_lvalue(sv2017::Variable_lvalueContext *context) = 0;

    virtual std::any visitSolve_before_list(sv2017::Solve_before_listContext *context) = 0;

    virtual std::any visitConstraint_block_item(sv2017::Constraint_block_itemContext *context) = 0;

    virtual std::any visitConstraint_expression(sv2017::Constraint_expressionContext *context) = 0;

    virtual std::any visitUniqueness_constraint(sv2017::Uniqueness_constraintContext *context) = 0;

    virtual std::any visitConstraint_set(sv2017::Constraint_setContext *context) = 0;

    virtual std::any visitRandomize_call(sv2017::Randomize_callContext *context) = 0;

    virtual std::any visitModule_header_common(sv2017::Module_header_commonContext *context) = 0;

    virtual std::any visitModule_declaration(sv2017::Module_declarationContext *context) = 0;

    virtual std::any visitModule_keyword(sv2017::Module_keywordContext *context) = 0;

    virtual std::any visitNet_port_type(sv2017::Net_port_typeContext *context) = 0;

    virtual std::any visitVar_data_type(sv2017::Var_data_typeContext *context) = 0;

    virtual std::any visitNet_or_var_data_type(sv2017::Net_or_var_data_typeContext *context) = 0;

    virtual std::any visitList_of_defparam_assignments(sv2017::List_of_defparam_assignmentsContext *context) = 0;

    virtual std::any visitList_of_net_decl_assignments(sv2017::List_of_net_decl_assignmentsContext *context) = 0;

    virtual std::any visitList_of_specparam_assignments(sv2017::List_of_specparam_assignmentsContext *context) = 0;

    virtual std::any visitList_of_variable_decl_assignments(sv2017::List_of_variable_decl_assignmentsContext *context) = 0;

    virtual std::any visitList_of_variable_identifiers_item(sv2017::List_of_variable_identifiers_itemContext *context) = 0;

    virtual std::any visitList_of_variable_identifiers(sv2017::List_of_variable_identifiersContext *context) = 0;

    virtual std::any visitList_of_variable_port_identifiers(sv2017::List_of_variable_port_identifiersContext *context) = 0;

    virtual std::any visitDefparam_assignment(sv2017::Defparam_assignmentContext *context) = 0;

    virtual std::any visitNet_decl_assignment(sv2017::Net_decl_assignmentContext *context) = 0;

    virtual std::any visitSpecparam_assignment(sv2017::Specparam_assignmentContext *context) = 0;

    virtual std::any visitError_limit_value(sv2017::Error_limit_valueContext *context) = 0;

    virtual std::any visitReject_limit_value(sv2017::Reject_limit_valueContext *context) = 0;

    virtual std::any visitPulse_control_specparam(sv2017::Pulse_control_specparamContext *context) = 0;

    virtual std::any visitIdentifier_doted_index_at_end(sv2017::Identifier_doted_index_at_endContext *context) = 0;

    virtual std::any visitSpecify_terminal_descriptor(sv2017::Specify_terminal_descriptorContext *context) = 0;

    virtual std::any visitSpecify_input_terminal_descriptor(sv2017::Specify_input_terminal_descriptorContext *context) = 0;

    virtual std::any visitSpecify_output_terminal_descriptor(sv2017::Specify_output_terminal_descriptorContext *context) = 0;

    virtual std::any visitSpecify_item(sv2017::Specify_itemContext *context) = 0;

    virtual std::any visitPulsestyle_declaration(sv2017::Pulsestyle_declarationContext *context) = 0;

    virtual std::any visitShowcancelled_declaration(sv2017::Showcancelled_declarationContext *context) = 0;

    virtual std::any visitPath_declaration(sv2017::Path_declarationContext *context) = 0;

    virtual std::any visitSimple_path_declaration(sv2017::Simple_path_declarationContext *context) = 0;

    virtual std::any visitPath_delay_value(sv2017::Path_delay_valueContext *context) = 0;

    virtual std::any visitList_of_path_outputs(sv2017::List_of_path_outputsContext *context) = 0;

    virtual std::any visitList_of_path_inputs(sv2017::List_of_path_inputsContext *context) = 0;

    virtual std::any visitList_of_paths(sv2017::List_of_pathsContext *context) = 0;

    virtual std::any visitList_of_path_delay_expressions(sv2017::List_of_path_delay_expressionsContext *context) = 0;

    virtual std::any visitT_path_delay_expression(sv2017::T_path_delay_expressionContext *context) = 0;

    virtual std::any visitTrise_path_delay_expression(sv2017::Trise_path_delay_expressionContext *context) = 0;

    virtual std::any visitTfall_path_delay_expression(sv2017::Tfall_path_delay_expressionContext *context) = 0;

    virtual std::any visitTz_path_delay_expression(sv2017::Tz_path_delay_expressionContext *context) = 0;

    virtual std::any visitT01_path_delay_expression(sv2017::T01_path_delay_expressionContext *context) = 0;

    virtual std::any visitT10_path_delay_expression(sv2017::T10_path_delay_expressionContext *context) = 0;

    virtual std::any visitT0z_path_delay_expression(sv2017::T0z_path_delay_expressionContext *context) = 0;

    virtual std::any visitTz1_path_delay_expression(sv2017::Tz1_path_delay_expressionContext *context) = 0;

    virtual std::any visitT1z_path_delay_expression(sv2017::T1z_path_delay_expressionContext *context) = 0;

    virtual std::any visitTz0_path_delay_expression(sv2017::Tz0_path_delay_expressionContext *context) = 0;

    virtual std::any visitT0x_path_delay_expression(sv2017::T0x_path_delay_expressionContext *context) = 0;

    virtual std::any visitTx1_path_delay_expression(sv2017::Tx1_path_delay_expressionContext *context) = 0;

    virtual std::any visitT1x_path_delay_expression(sv2017::T1x_path_delay_expressionContext *context) = 0;

    virtual std::any visitTx0_path_delay_expression(sv2017::Tx0_path_delay_expressionContext *context) = 0;

    virtual std::any visitTxz_path_delay_expression(sv2017::Txz_path_delay_expressionContext *context) = 0;

    virtual std::any visitTzx_path_delay_expression(sv2017::Tzx_path_delay_expressionContext *context) = 0;

    virtual std::any visitParallel_path_description(sv2017::Parallel_path_descriptionContext *context) = 0;

    virtual std::any visitFull_path_description(sv2017::Full_path_descriptionContext *context) = 0;

    virtual std::any visitIdentifier_list(sv2017::Identifier_listContext *context) = 0;

    virtual std::any visitSpecparam_declaration(sv2017::Specparam_declarationContext *context) = 0;

    virtual std::any visitEdge_sensitive_path_declaration(sv2017::Edge_sensitive_path_declarationContext *context) = 0;

    virtual std::any visitParallel_edge_sensitive_path_description(sv2017::Parallel_edge_sensitive_path_descriptionContext *context) = 0;

    virtual std::any visitFull_edge_sensitive_path_description(sv2017::Full_edge_sensitive_path_descriptionContext *context) = 0;

    virtual std::any visitData_source_expression(sv2017::Data_source_expressionContext *context) = 0;

    virtual std::any visitData_declaration(sv2017::Data_declarationContext *context) = 0;

    virtual std::any visitModule_path_expression(sv2017::Module_path_expressionContext *context) = 0;

    virtual std::any visitState_dependent_path_declaration(sv2017::State_dependent_path_declarationContext *context) = 0;

    virtual std::any visitPackage_export_declaration(sv2017::Package_export_declarationContext *context) = 0;

    virtual std::any visitGenvar_declaration(sv2017::Genvar_declarationContext *context) = 0;

    virtual std::any visitNet_declaration(sv2017::Net_declarationContext *context) = 0;

    virtual std::any visitParameter_port_list(sv2017::Parameter_port_listContext *context) = 0;

    virtual std::any visitParameter_port_declaration(sv2017::Parameter_port_declarationContext *context) = 0;

    virtual std::any visitList_of_port_declarations_ansi_item(sv2017::List_of_port_declarations_ansi_itemContext *context) = 0;

    virtual std::any visitList_of_port_declarations(sv2017::List_of_port_declarationsContext *context) = 0;

    virtual std::any visitNonansi_port_declaration(sv2017::Nonansi_port_declarationContext *context) = 0;

    virtual std::any visitNonansi_port(sv2017::Nonansi_portContext *context) = 0;

    virtual std::any visitNonansi_port__expr(sv2017::Nonansi_port__exprContext *context) = 0;

    virtual std::any visitPort_identifier(sv2017::Port_identifierContext *context) = 0;

    virtual std::any visitAnsi_port_declaration(sv2017::Ansi_port_declarationContext *context) = 0;

    virtual std::any visitSystem_timing_check(sv2017::System_timing_checkContext *context) = 0;

    virtual std::any visitDolar_setup_timing_check(sv2017::Dolar_setup_timing_checkContext *context) = 0;

    virtual std::any visitDolar_hold_timing_check(sv2017::Dolar_hold_timing_checkContext *context) = 0;

    virtual std::any visitDolar_setuphold_timing_check(sv2017::Dolar_setuphold_timing_checkContext *context) = 0;

    virtual std::any visitDolar_recovery_timing_check(sv2017::Dolar_recovery_timing_checkContext *context) = 0;

    virtual std::any visitDolar_removal_timing_check(sv2017::Dolar_removal_timing_checkContext *context) = 0;

    virtual std::any visitDolar_recrem_timing_check(sv2017::Dolar_recrem_timing_checkContext *context) = 0;

    virtual std::any visitDolar_skew_timing_check(sv2017::Dolar_skew_timing_checkContext *context) = 0;

    virtual std::any visitDolar_timeskew_timing_check(sv2017::Dolar_timeskew_timing_checkContext *context) = 0;

    virtual std::any visitDolar_fullskew_timing_check(sv2017::Dolar_fullskew_timing_checkContext *context) = 0;

    virtual std::any visitDolar_period_timing_check(sv2017::Dolar_period_timing_checkContext *context) = 0;

    virtual std::any visitDolar_width_timing_check(sv2017::Dolar_width_timing_checkContext *context) = 0;

    virtual std::any visitDolar_nochange_timing_check(sv2017::Dolar_nochange_timing_checkContext *context) = 0;

    virtual std::any visitTimecheck_condition(sv2017::Timecheck_conditionContext *context) = 0;

    virtual std::any visitControlled_reference_event(sv2017::Controlled_reference_eventContext *context) = 0;

    virtual std::any visitDelayed_reference(sv2017::Delayed_referenceContext *context) = 0;

    virtual std::any visitEnd_edge_offset(sv2017::End_edge_offsetContext *context) = 0;

    virtual std::any visitEvent_based_flag(sv2017::Event_based_flagContext *context) = 0;

    virtual std::any visitNotifier(sv2017::NotifierContext *context) = 0;

    virtual std::any visitRemain_active_flag(sv2017::Remain_active_flagContext *context) = 0;

    virtual std::any visitTimestamp_condition(sv2017::Timestamp_conditionContext *context) = 0;

    virtual std::any visitStart_edge_offset(sv2017::Start_edge_offsetContext *context) = 0;

    virtual std::any visitThreshold(sv2017::ThresholdContext *context) = 0;

    virtual std::any visitTiming_check_limit(sv2017::Timing_check_limitContext *context) = 0;

    virtual std::any visitTiming_check_event(sv2017::Timing_check_eventContext *context) = 0;

    virtual std::any visitTiming_check_condition(sv2017::Timing_check_conditionContext *context) = 0;

    virtual std::any visitScalar_timing_check_condition(sv2017::Scalar_timing_check_conditionContext *context) = 0;

    virtual std::any visitControlled_timing_check_event(sv2017::Controlled_timing_check_eventContext *context) = 0;

    virtual std::any visitFunction_data_type_or_implicit(sv2017::Function_data_type_or_implicitContext *context) = 0;

    virtual std::any visitExtern_tf_declaration(sv2017::Extern_tf_declarationContext *context) = 0;

    virtual std::any visitFunction_declaration(sv2017::Function_declarationContext *context) = 0;

    virtual std::any visitUntyped_function_declaration(sv2017::Untyped_function_declarationContext *context) = 0;

    virtual std::any visitTask_prototype(sv2017::Task_prototypeContext *context) = 0;

    virtual std::any visitFunction_prototype(sv2017::Function_prototypeContext *context) = 0;

    virtual std::any visitDpi_import_export(sv2017::Dpi_import_exportContext *context) = 0;

    virtual std::any visitDpi_function_import_property(sv2017::Dpi_function_import_propertyContext *context) = 0;

    virtual std::any visitDpi_task_import_property(sv2017::Dpi_task_import_propertyContext *context) = 0;

    virtual std::any visitTask_and_function_declaration_common(sv2017::Task_and_function_declaration_commonContext *context) = 0;

    virtual std::any visitTask_declaration(sv2017::Task_declarationContext *context) = 0;

    virtual std::any visitMethod_prototype(sv2017::Method_prototypeContext *context) = 0;

    virtual std::any visitExtern_constraint_declaration(sv2017::Extern_constraint_declarationContext *context) = 0;

    virtual std::any visitConstraint_block(sv2017::Constraint_blockContext *context) = 0;

    virtual std::any visitChecker_port_list(sv2017::Checker_port_listContext *context) = 0;

    virtual std::any visitChecker_port_item(sv2017::Checker_port_itemContext *context) = 0;

    virtual std::any visitChecker_port_direction(sv2017::Checker_port_directionContext *context) = 0;

    virtual std::any visitChecker_declaration(sv2017::Checker_declarationContext *context) = 0;

    virtual std::any visitClass_declaration(sv2017::Class_declarationContext *context) = 0;

    virtual std::any visitAlways_construct(sv2017::Always_constructContext *context) = 0;

    virtual std::any visitInterface_class_type(sv2017::Interface_class_typeContext *context) = 0;

    virtual std::any visitInterface_class_declaration(sv2017::Interface_class_declarationContext *context) = 0;

    virtual std::any visitInterface_class_item(sv2017::Interface_class_itemContext *context) = 0;

    virtual std::any visitInterface_class_method(sv2017::Interface_class_methodContext *context) = 0;

    virtual std::any visitPackage_declaration(sv2017::Package_declarationContext *context) = 0;

    virtual std::any visitPackage_item(sv2017::Package_itemContext *context) = 0;

    virtual std::any visitProgram_declaration(sv2017::Program_declarationContext *context) = 0;

    virtual std::any visitProgram_header(sv2017::Program_headerContext *context) = 0;

    virtual std::any visitProgram_item(sv2017::Program_itemContext *context) = 0;

    virtual std::any visitNon_port_program_item(sv2017::Non_port_program_itemContext *context) = 0;

    virtual std::any visitAnonymous_program(sv2017::Anonymous_programContext *context) = 0;

    virtual std::any visitAnonymous_program_item(sv2017::Anonymous_program_itemContext *context) = 0;

    virtual std::any visitSequence_declaration(sv2017::Sequence_declarationContext *context) = 0;

    virtual std::any visitSequence_port_list(sv2017::Sequence_port_listContext *context) = 0;

    virtual std::any visitSequence_port_item(sv2017::Sequence_port_itemContext *context) = 0;

    virtual std::any visitProperty_declaration(sv2017::Property_declarationContext *context) = 0;

    virtual std::any visitProperty_port_list(sv2017::Property_port_listContext *context) = 0;

    virtual std::any visitProperty_port_item(sv2017::Property_port_itemContext *context) = 0;

    virtual std::any visitContinuous_assign(sv2017::Continuous_assignContext *context) = 0;

    virtual std::any visitChecker_or_generate_item(sv2017::Checker_or_generate_itemContext *context) = 0;

    virtual std::any visitConstraint_prototype(sv2017::Constraint_prototypeContext *context) = 0;

    virtual std::any visitClass_constraint(sv2017::Class_constraintContext *context) = 0;

    virtual std::any visitConstraint_declaration(sv2017::Constraint_declarationContext *context) = 0;

    virtual std::any visitClass_constructor_declaration(sv2017::Class_constructor_declarationContext *context) = 0;

    virtual std::any visitClass_property(sv2017::Class_propertyContext *context) = 0;

    virtual std::any visitClass_method(sv2017::Class_methodContext *context) = 0;

    virtual std::any visitClass_constructor_prototype(sv2017::Class_constructor_prototypeContext *context) = 0;

    virtual std::any visitClass_item(sv2017::Class_itemContext *context) = 0;

    virtual std::any visitParameter_override(sv2017::Parameter_overrideContext *context) = 0;

    virtual std::any visitGate_instantiation(sv2017::Gate_instantiationContext *context) = 0;

    virtual std::any visitEnable_gate_or_mos_switch_or_cmos_switch_instance(sv2017::Enable_gate_or_mos_switch_or_cmos_switch_instanceContext *context) = 0;

    virtual std::any visitN_input_gate_instance(sv2017::N_input_gate_instanceContext *context) = 0;

    virtual std::any visitN_output_gate_instance(sv2017::N_output_gate_instanceContext *context) = 0;

    virtual std::any visitPass_switch_instance(sv2017::Pass_switch_instanceContext *context) = 0;

    virtual std::any visitPass_enable_switch_instance(sv2017::Pass_enable_switch_instanceContext *context) = 0;

    virtual std::any visitPull_gate_instance(sv2017::Pull_gate_instanceContext *context) = 0;

    virtual std::any visitPulldown_strength(sv2017::Pulldown_strengthContext *context) = 0;

    virtual std::any visitPullup_strength(sv2017::Pullup_strengthContext *context) = 0;

    virtual std::any visitEnable_terminal(sv2017::Enable_terminalContext *context) = 0;

    virtual std::any visitInout_terminal(sv2017::Inout_terminalContext *context) = 0;

    virtual std::any visitInput_terminal(sv2017::Input_terminalContext *context) = 0;

    virtual std::any visitOutput_terminal(sv2017::Output_terminalContext *context) = 0;

    virtual std::any visitUdp_instantiation(sv2017::Udp_instantiationContext *context) = 0;

    virtual std::any visitUdp_instance(sv2017::Udp_instanceContext *context) = 0;

    virtual std::any visitUdp_instance_body(sv2017::Udp_instance_bodyContext *context) = 0;

    virtual std::any visitModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *context) = 0;

    virtual std::any visitHierarchical_instance(sv2017::Hierarchical_instanceContext *context) = 0;

    virtual std::any visitList_of_port_connections(sv2017::List_of_port_connectionsContext *context) = 0;

    virtual std::any visitOrdered_port_connection(sv2017::Ordered_port_connectionContext *context) = 0;

    virtual std::any visitNamed_port_connection(sv2017::Named_port_connectionContext *context) = 0;

    virtual std::any visitPort_expression_connection(sv2017::Port_expression_connectionContext *context) = 0;

    virtual std::any visitPort_concatenation_connection(sv2017::Port_concatenation_connectionContext *context) = 0;

    virtual std::any visitConcatenation_item(sv2017::Concatenation_itemContext *context) = 0;

    virtual std::any visitPort_replication_connection(sv2017::Port_replication_connectionContext *context) = 0;

    virtual std::any visitBind_directive(sv2017::Bind_directiveContext *context) = 0;

    virtual std::any visitBind_target_instance(sv2017::Bind_target_instanceContext *context) = 0;

    virtual std::any visitBind_target_instance_list(sv2017::Bind_target_instance_listContext *context) = 0;

    virtual std::any visitBind_instantiation(sv2017::Bind_instantiationContext *context) = 0;

    virtual std::any visitConfig_declaration(sv2017::Config_declarationContext *context) = 0;

    virtual std::any visitDesign_statement(sv2017::Design_statementContext *context) = 0;

    virtual std::any visitConfig_rule_statement(sv2017::Config_rule_statementContext *context) = 0;

    virtual std::any visitInst_clause(sv2017::Inst_clauseContext *context) = 0;

    virtual std::any visitInst_name(sv2017::Inst_nameContext *context) = 0;

    virtual std::any visitCell_clause(sv2017::Cell_clauseContext *context) = 0;

    virtual std::any visitLiblist_clause(sv2017::Liblist_clauseContext *context) = 0;

    virtual std::any visitUse_clause(sv2017::Use_clauseContext *context) = 0;

    virtual std::any visitNet_alias(sv2017::Net_aliasContext *context) = 0;

    virtual std::any visitSpecify_block(sv2017::Specify_blockContext *context) = 0;

    virtual std::any visitGenerate_region(sv2017::Generate_regionContext *context) = 0;

    virtual std::any visitGenvar_expression(sv2017::Genvar_expressionContext *context) = 0;

    virtual std::any visitLoop_generate_construct(sv2017::Loop_generate_constructContext *context) = 0;

    virtual std::any visitGenvar_initialization(sv2017::Genvar_initializationContext *context) = 0;

    virtual std::any visitGenvar_iteration(sv2017::Genvar_iterationContext *context) = 0;

    virtual std::any visitConditional_generate_construct(sv2017::Conditional_generate_constructContext *context) = 0;

    virtual std::any visitIf_generate_construct(sv2017::If_generate_constructContext *context) = 0;

    virtual std::any visitCase_generate_construct(sv2017::Case_generate_constructContext *context) = 0;

    virtual std::any visitCase_generate_item(sv2017::Case_generate_itemContext *context) = 0;

    virtual std::any visitGenerate_begin_end_block(sv2017::Generate_begin_end_blockContext *context) = 0;

    virtual std::any visitGenerate_item(sv2017::Generate_itemContext *context) = 0;

    virtual std::any visitProgram_generate_item(sv2017::Program_generate_itemContext *context) = 0;

    virtual std::any visitModule_or_generate_or_interface_or_checker_item(sv2017::Module_or_generate_or_interface_or_checker_itemContext *context) = 0;

    virtual std::any visitModule_or_generate_or_interface_item(sv2017::Module_or_generate_or_interface_itemContext *context) = 0;

    virtual std::any visitModule_or_generate_item(sv2017::Module_or_generate_itemContext *context) = 0;

    virtual std::any visitElaboration_system_task(sv2017::Elaboration_system_taskContext *context) = 0;

    virtual std::any visitModule_item_item(sv2017::Module_item_itemContext *context) = 0;

    virtual std::any visitModule_item(sv2017::Module_itemContext *context) = 0;


};

}  // namespace mgp_sv
