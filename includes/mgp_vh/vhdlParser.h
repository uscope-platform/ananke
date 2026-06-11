
// Generated from /Users/filss/git/makefile_gen/grammars/vhdlParser.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"


namespace mgp_vh {


class  vhdlParser : public antlr4::Parser {
public:
  enum {
    KW_PROCESS = 1, KW_CONTEXT = 2, KW_POSTPONED = 3, KW_LINKAGE = 4, KW_COMPONENT = 5, 
    KW_ABS = 6, KW_DEFAULT = 7, KW_THEN = 8, KW_BLOCK = 9, KW_REM = 10, 
    KW_INERTIAL = 11, KW_NEXT = 12, KW_ENTITY = 13, KW_ON = 14, KW_GROUP = 15, 
    KW_XNOR = 16, KW_FILE = 17, KW_PURE = 18, KW_GUARDED = 19, KW_GENERIC = 20, 
    KW_RANGE = 21, KW_ELSE = 22, KW_USE = 23, KW_SHARED = 24, KW_MOD = 25, 
    KW_LOOP = 26, KW_RECORD = 27, KW_SIGNAL = 28, KW_REJECT = 29, KW_BEGIN = 30, 
    KW_SLA = 31, KW_DISCONNECT = 32, KW_OF = 33, KW_PROCEDURE = 34, KW_SRL = 35, 
    KW_VUNIT = 36, KW_ATTRIBUTE = 37, KW_VARIABLE = 38, KW_PROPERTY = 39, 
    KW_UNAFFECTED = 40, KW_XOR = 41, KW_REGISTER = 42, KW_SUBTYPE = 43, 
    KW_TO = 44, KW_NEW = 45, KW_REPORT = 46, KW_CONSTANT = 47, KW_BUFFER = 48, 
    KW_BODY = 49, KW_AFTER = 50, KW_TRANSPORT = 51, KW_FUNCTION = 52, KW_END = 53, 
    KW_SELECT = 54, KW_OR = 55, KW_LIBRARY = 56, KW_ELSIF = 57, KW_SLL = 58, 
    KW_MAP = 59, KW_SRA = 60, KW_PROTECTED = 61, KW_DOWNTO = 62, KW_LABEL = 63, 
    KW_ALL = 64, KW_ALIAS = 65, KW_GENERATE = 66, KW_NOR = 67, KW_IN = 68, 
    KW_RELEASE = 69, KW_EXIT = 70, KW_RETURN = 71, KW_WITH = 72, KW_UNTIL = 73, 
    KW_AND = 74, KW_INOUT = 75, KW_WAIT = 76, KW_NAND = 77, KW_ARRAY = 78, 
    KW_FORCE = 79, KW_WHILE = 80, KW_IMPURE = 81, KW_PACKAGE = 82, KW_UNITS = 83, 
    KW_ASSERT = 84, KW_PARAMETER = 85, KW_SEVERITY = 86, KW_LITERAL = 87, 
    KW_FOR = 88, KW_ROR = 89, KW_IF = 90, KW_OUT = 91, KW_ROL = 92, KW_IS = 93, 
    KW_SEQUENCE = 94, KW_OTHERS = 95, KW_TYPE = 96, KW_CASE = 97, KW_NOT = 98, 
    KW_CONFIGURATION = 99, KW_OPEN = 100, KW_ARCHITECTURE = 101, KW_BUS = 102, 
    KW_ACCESS = 103, KW_WHEN = 104, KW_PORT = 105, KW_NULL = 106, BASIC_IDENTIFIER = 107, 
    EXTENDED_IDENTIFIER = 108, DECIMAL_LITERAL = 109, INTEGER = 110, BASED_LITERAL = 111, 
    CHARACTER_LITERAL = 112, STRING_LITERAL = 113, BIT_STRING_LITERAL = 114, 
    COMMENT = 115, BLOCK_COMMENT = 116, TAB = 117, SPACE = 118, NEWLINE = 119, 
    CR = 120, SPACE_CHARACTER = 121, DBLQUOTE = 122, UNDERSCORE = 123, DIGIT = 124, 
    SEMI = 125, LPAREN = 126, RPAREN = 127, LSQUARE_BR = 128, RSQUARE_BR = 129, 
    APOSTROPHE = 130, SHIFT_LEFT = 131, SHIFT_RIGHT = 132, AT = 133, HASHTAG = 134, 
    COMMA = 135, DOT = 136, QUESTIONMARK = 137, COLON = 138, EQ = 139, NE = 140, 
    LT = 141, GT = 142, GE = 143, EQ_MATCH = 144, NE_MATCH = 145, LT_MATCH = 146, 
    LE_MATCH = 147, GT_MATCH = 148, GE_MATCH = 149, PLUS = 150, MINUS = 151, 
    AMPERSAND = 152, BAR = 153, BACKSLASH = 154, MUL = 155, DIV = 156, DOUBLESTAR = 157, 
    CONASGN = 158, GRAVE_ACCENT = 159, UP = 160, VARASGN = 161, BOX = 162, 
    ARROW = 163, COND_OP = 164
  };

  enum {
    RuleAny_keyword = 0, RuleName_literal = 1, RuleName = 2, RuleName_slice_part = 3, 
    RuleName_attribute_part = 4, RuleAttribute_name = 5, RuleSuffix = 6, 
    RuleExplicit_range = 7, RuleSelected_name = 8, RuleEntity_declaration = 9, 
    RuleEntity_declarative_item = 10, RuleEntity_statement = 11, RuleArchitecture_body = 12, 
    RuleBlock_declarative_item = 13, RuleConfiguration_declaration = 14, 
    RuleConfiguration_declarative_item = 15, RuleBlock_configuration = 16, 
    RuleBlock_specification = 17, RuleGenerate_specification = 18, RuleConfiguration_item = 19, 
    RuleComponent_configuration = 20, RuleSubprogram_declaration = 21, RuleSubprogram_specification = 22, 
    RuleProcedure_specification = 23, RuleFunction_specification = 24, RuleSubprogram_header = 25, 
    RuleDesignator = 26, RuleOperator_symbol = 27, RuleFormal_parameter_list = 28, 
    RuleSubprogram_body = 29, RuleSubprogram_kind = 30, RuleSubprogram_instantiation_declaration = 31, 
    RuleSignature = 32, RulePackage_declaration = 33, RulePackage_declarative_item = 34, 
    RulePackage_body = 35, RulePackage_instantiation_declaration = 36, RuleScalar_type_definition = 37, 
    RuleRange_constraint = 38, RuleRange = 39, RuleDirection = 40, RuleEnumeration_type_definition = 41, 
    RuleEnumeration_literal = 42, RuleInteger_type_definition = 43, RulePhysical_type_definition = 44, 
    RulePrimary_unit_declaration = 45, RuleSecondary_unit_declaration = 46, 
    RuleFloating_type_definition = 47, RuleComposite_type_definition = 48, 
    RuleArray_type_definition = 49, RuleUnbounded_array_definition = 50, 
    RuleConstrained_array_definition = 51, RuleIndex_subtype_definition = 52, 
    RuleArray_constraint = 53, RuleArray_element_constraint = 54, RuleIndex_constraint = 55, 
    RuleDiscrete_range = 56, RuleRecord_type_definition = 57, RuleElement_declaration = 58, 
    RuleIdentifier_list = 59, RuleElement_subtype_definition = 60, RuleRecord_constraint = 61, 
    RuleRecord_element_constraint = 62, RuleAccess_type_definition = 63, 
    RuleIncomplete_type_declaration = 64, RuleFile_type_definition = 65, 
    RuleProtected_type_definition = 66, RuleProtected_type_declaration = 67, 
    RuleProtected_type_declarative_item = 68, RuleProtected_type_body = 69, 
    RuleType_declaration = 70, RuleFull_type_declaration = 71, RuleType_definition = 72, 
    RuleSubtype_declaration = 73, RuleSubtype_indication = 74, RuleResolution_indication = 75, 
    RuleElement_resolution = 76, RuleArray_element_resolution = 77, RuleRecord_resolution = 78, 
    RuleRecord_element_resolution = 79, RuleType_mark = 80, RuleConstraint = 81, 
    RuleElement_constraint = 82, RuleObject_declaration = 83, RuleConstant_declaration = 84, 
    RuleSignal_declaration = 85, RuleSignal_kind = 86, RuleVariable_declaration = 87, 
    RuleFile_declaration = 88, RuleFile_open_information = 89, RuleFile_logical_name = 90, 
    RuleInterface_declaration = 91, RuleInterface_object_declaration = 92, 
    RuleInterface_constant_declaration = 93, RuleInterface_signal_declaration = 94, 
    RuleInterface_variable_declaration = 95, RuleInterface_file_declaration = 96, 
    RuleSignal_mode = 97, RuleInterface_type_declaration = 98, RuleInterface_incomplete_type_declaration = 99, 
    RuleInterface_subprogram_declaration = 100, RuleInterface_subprogram_specification = 101, 
    RuleInterface_procedure_specification = 102, RuleInterface_function_specification = 103, 
    RuleInterface_subprogram_default = 104, RuleInterface_package_declaration = 105, 
    RuleInterface_package_generic_map_aspect = 106, RuleInterface_list = 107, 
    RuleInterface_element = 108, RuleGeneric_clause = 109, RuleGeneric_list = 110, 
    RulePort_clause = 111, RulePort_list = 112, RuleAssociation_list = 113, 
    RuleAssociation_element = 114, RuleFormal_part = 115, RuleActual_part = 116, 
    RuleActual_designator = 117, RuleGeneric_map_aspect = 118, RulePort_map_aspect = 119, 
    RuleAlias_declaration = 120, RuleAlias_designator = 121, RuleAttribute_declaration = 122, 
    RuleComponent_declaration = 123, RuleGroup_template_declaration = 124, 
    RuleEntity_class_entry_list = 125, RuleEntity_class_entry = 126, RuleGroup_declaration = 127, 
    RuleGroup_constituent_list = 128, RuleGroup_constituent = 129, RuleAttribute_specification = 130, 
    RuleEntity_specification = 131, RuleEntity_class = 132, RuleEntity_name_list = 133, 
    RuleEntity_designator = 134, RuleEntity_tag = 135, RuleConfiguration_specification = 136, 
    RuleSimple_configuration_specification = 137, RuleCompound_configuration_specification = 138, 
    RuleComponent_specification = 139, RuleInstantiation_list = 140, RuleBinding_indication = 141, 
    RuleEntity_aspect = 142, RuleVerification_unit_binding_indication = 143, 
    RuleVerification_unit_list = 144, RuleDisconnection_specification = 145, 
    RuleGuarded_signal_specification = 146, RuleSignal_list = 147, RuleAttribute_designator = 148, 
    RuleExternal_name = 149, RuleExternal_pathname = 150, RulePackage_pathname = 151, 
    RuleAbsolute_pathname = 152, RuleRelative_pathname = 153, RulePartial_pathname = 154, 
    RulePathname_element = 155, RuleExpression = 156, RuleSimple_expression = 157, 
    RulePrimary = 158, RuleLogical_operator = 159, RuleRelational_operator = 160, 
    RuleShift_operator = 161, RuleAdding_operator = 162, RuleSign = 163, 
    RuleMultiplying_operator = 164, RuleMiscellaneous_operator = 165, RuleNumeric_literal = 166, 
    RulePhysical_literal = 167, RuleAggregate = 168, RuleElement_association = 169, 
    RuleChoices = 170, RuleChoice = 171, RuleQualified_expression = 172, 
    RuleAllocator = 173, RuleSequence_of_statements = 174, RuleSequential_statement = 175, 
    RuleWait_statement = 176, RuleSensitivity_clause = 177, RuleSensitivity_list = 178, 
    RuleCondition_clause = 179, RuleCondition = 180, RuleTimeout_clause = 181, 
    RuleAssertion_statement = 182, RuleAssertion = 183, RuleReport_statement = 184, 
    RuleSignal_assignment_statement = 185, RuleSimple_signal_assignment = 186, 
    RuleSimple_waveform_assignment = 187, RuleSimple_force_assignment = 188, 
    RuleSimple_release_assignment = 189, RuleForce_mode = 190, RuleDelay_mechanism = 191, 
    RuleTarget = 192, RuleWaveform = 193, RuleWaveform_element = 194, RuleConditional_signal_assignment = 195, 
    RuleConditional_waveform_assignment = 196, RuleConditional_waveforms = 197, 
    RuleConditional_force_assignment = 198, RuleConditional_expressions = 199, 
    RuleSelected_signal_assignment = 200, RuleSelected_waveform_assignment = 201, 
    RuleSelected_waveforms = 202, RuleSelected_force_assignment = 203, RuleSelected_expressions = 204, 
    RuleVariable_assignment_statement = 205, RuleSimple_variable_assignment = 206, 
    RuleConditional_variable_assignment = 207, RuleSelected_variable_assignment = 208, 
    RuleProcedure_call_statement = 209, RuleProcedure_call = 210, RuleIf_statement = 211, 
    RuleCase_statement = 212, RuleCase_statement_alternative = 213, RuleLoop_statement = 214, 
    RuleIteration_scheme = 215, RuleParameter_specification = 216, RuleNext_statement = 217, 
    RuleExit_statement = 218, RuleReturn_statement = 219, RuleNull_statement = 220, 
    RuleConcurrent_statement_with_optional_label = 221, RuleConcurrent_statement = 222, 
    RuleBlock_statement = 223, RuleBlock_header = 224, RuleProcess_statement = 225, 
    RuleProcess_sensitivity_list = 226, RuleProcess_or_package_declarative_item = 227, 
    RuleProcess_declarative_item = 228, RuleConcurrent_procedure_call_statement = 229, 
    RuleConcurrent_assertion_statement = 230, RuleConcurrent_signal_assignment_statement = 231, 
    RuleConcurrent_signal_assignment_any = 232, RuleConcurrent_selected_signal_assignment = 233, 
    RuleComponent_instantiation_statement = 234, RuleInstantiated_unit = 235, 
    RuleGenerate_statement = 236, RuleFor_generate_statement = 237, RuleIf_generate_statement = 238, 
    RuleCase_generate_statement = 239, RuleCase_generate_alternative = 240, 
    RuleGenerate_statement_body_with_begin_end = 241, RuleGenerate_statement_body = 242, 
    RuleLabel = 243, RuleUse_clause = 244, RuleDesign_file = 245, RuleDesign_unit = 246, 
    RuleLibrary_unit = 247, RulePrimary_unit = 248, RuleSecondary_unit = 249, 
    RuleLibrary_clause = 250, RuleLogical_name_list = 251, RuleContext_declaration = 252, 
    RuleContext_clause = 253, RuleContext_item = 254, RuleContext_reference = 255, 
    RuleIdentifier = 256
  };

  explicit vhdlParser(antlr4::TokenStream *input);

  vhdlParser(antlr4::TokenStream *input, const antlr4::atn::ParserATNSimulatorOptions &options);

  ~vhdlParser() override;

  std::string getGrammarFileName() const override;

  const antlr4::atn::ATN& getATN() const override;

  const std::vector<std::string>& getRuleNames() const override;

  const antlr4::dfa::Vocabulary& getVocabulary() const override;

  antlr4::atn::SerializedATNView getSerializedATN() const override;


  class Any_keywordContext;
  class Name_literalContext;
  class NameContext;
  class Name_slice_partContext;
  class Name_attribute_partContext;
  class Attribute_nameContext;
  class SuffixContext;
  class Explicit_rangeContext;
  class Selected_nameContext;
  class Entity_declarationContext;
  class Entity_declarative_itemContext;
  class Entity_statementContext;
  class Architecture_bodyContext;
  class Block_declarative_itemContext;
  class Configuration_declarationContext;
  class Configuration_declarative_itemContext;
  class Block_configurationContext;
  class Block_specificationContext;
  class Generate_specificationContext;
  class Configuration_itemContext;
  class Component_configurationContext;
  class Subprogram_declarationContext;
  class Subprogram_specificationContext;
  class Procedure_specificationContext;
  class Function_specificationContext;
  class Subprogram_headerContext;
  class DesignatorContext;
  class Operator_symbolContext;
  class Formal_parameter_listContext;
  class Subprogram_bodyContext;
  class Subprogram_kindContext;
  class Subprogram_instantiation_declarationContext;
  class SignatureContext;
  class Package_declarationContext;
  class Package_declarative_itemContext;
  class Package_bodyContext;
  class Package_instantiation_declarationContext;
  class Scalar_type_definitionContext;
  class Range_constraintContext;
  class RangeContext;
  class DirectionContext;
  class Enumeration_type_definitionContext;
  class Enumeration_literalContext;
  class Integer_type_definitionContext;
  class Physical_type_definitionContext;
  class Primary_unit_declarationContext;
  class Secondary_unit_declarationContext;
  class Floating_type_definitionContext;
  class Composite_type_definitionContext;
  class Array_type_definitionContext;
  class Unbounded_array_definitionContext;
  class Constrained_array_definitionContext;
  class Index_subtype_definitionContext;
  class Array_constraintContext;
  class Array_element_constraintContext;
  class Index_constraintContext;
  class Discrete_rangeContext;
  class Record_type_definitionContext;
  class Element_declarationContext;
  class Identifier_listContext;
  class Element_subtype_definitionContext;
  class Record_constraintContext;
  class Record_element_constraintContext;
  class Access_type_definitionContext;
  class Incomplete_type_declarationContext;
  class File_type_definitionContext;
  class Protected_type_definitionContext;
  class Protected_type_declarationContext;
  class Protected_type_declarative_itemContext;
  class Protected_type_bodyContext;
  class Type_declarationContext;
  class Full_type_declarationContext;
  class Type_definitionContext;
  class Subtype_declarationContext;
  class Subtype_indicationContext;
  class Resolution_indicationContext;
  class Element_resolutionContext;
  class Array_element_resolutionContext;
  class Record_resolutionContext;
  class Record_element_resolutionContext;
  class Type_markContext;
  class ConstraintContext;
  class Element_constraintContext;
  class Object_declarationContext;
  class Constant_declarationContext;
  class Signal_declarationContext;
  class Signal_kindContext;
  class Variable_declarationContext;
  class File_declarationContext;
  class File_open_informationContext;
  class File_logical_nameContext;
  class Interface_declarationContext;
  class Interface_object_declarationContext;
  class Interface_constant_declarationContext;
  class Interface_signal_declarationContext;
  class Interface_variable_declarationContext;
  class Interface_file_declarationContext;
  class Signal_modeContext;
  class Interface_type_declarationContext;
  class Interface_incomplete_type_declarationContext;
  class Interface_subprogram_declarationContext;
  class Interface_subprogram_specificationContext;
  class Interface_procedure_specificationContext;
  class Interface_function_specificationContext;
  class Interface_subprogram_defaultContext;
  class Interface_package_declarationContext;
  class Interface_package_generic_map_aspectContext;
  class Interface_listContext;
  class Interface_elementContext;
  class Generic_clauseContext;
  class Generic_listContext;
  class Port_clauseContext;
  class Port_listContext;
  class Association_listContext;
  class Association_elementContext;
  class Formal_partContext;
  class Actual_partContext;
  class Actual_designatorContext;
  class Generic_map_aspectContext;
  class Port_map_aspectContext;
  class Alias_declarationContext;
  class Alias_designatorContext;
  class Attribute_declarationContext;
  class Component_declarationContext;
  class Group_template_declarationContext;
  class Entity_class_entry_listContext;
  class Entity_class_entryContext;
  class Group_declarationContext;
  class Group_constituent_listContext;
  class Group_constituentContext;
  class Attribute_specificationContext;
  class Entity_specificationContext;
  class Entity_classContext;
  class Entity_name_listContext;
  class Entity_designatorContext;
  class Entity_tagContext;
  class Configuration_specificationContext;
  class Simple_configuration_specificationContext;
  class Compound_configuration_specificationContext;
  class Component_specificationContext;
  class Instantiation_listContext;
  class Binding_indicationContext;
  class Entity_aspectContext;
  class Verification_unit_binding_indicationContext;
  class Verification_unit_listContext;
  class Disconnection_specificationContext;
  class Guarded_signal_specificationContext;
  class Signal_listContext;
  class Attribute_designatorContext;
  class External_nameContext;
  class External_pathnameContext;
  class Package_pathnameContext;
  class Absolute_pathnameContext;
  class Relative_pathnameContext;
  class Partial_pathnameContext;
  class Pathname_elementContext;
  class ExpressionContext;
  class Simple_expressionContext;
  class PrimaryContext;
  class Logical_operatorContext;
  class Relational_operatorContext;
  class Shift_operatorContext;
  class Adding_operatorContext;
  class SignContext;
  class Multiplying_operatorContext;
  class Miscellaneous_operatorContext;
  class Numeric_literalContext;
  class Physical_literalContext;
  class AggregateContext;
  class Element_associationContext;
  class ChoicesContext;
  class ChoiceContext;
  class Qualified_expressionContext;
  class AllocatorContext;
  class Sequence_of_statementsContext;
  class Sequential_statementContext;
  class Wait_statementContext;
  class Sensitivity_clauseContext;
  class Sensitivity_listContext;
  class Condition_clauseContext;
  class ConditionContext;
  class Timeout_clauseContext;
  class Assertion_statementContext;
  class AssertionContext;
  class Report_statementContext;
  class Signal_assignment_statementContext;
  class Simple_signal_assignmentContext;
  class Simple_waveform_assignmentContext;
  class Simple_force_assignmentContext;
  class Simple_release_assignmentContext;
  class Force_modeContext;
  class Delay_mechanismContext;
  class TargetContext;
  class WaveformContext;
  class Waveform_elementContext;
  class Conditional_signal_assignmentContext;
  class Conditional_waveform_assignmentContext;
  class Conditional_waveformsContext;
  class Conditional_force_assignmentContext;
  class Conditional_expressionsContext;
  class Selected_signal_assignmentContext;
  class Selected_waveform_assignmentContext;
  class Selected_waveformsContext;
  class Selected_force_assignmentContext;
  class Selected_expressionsContext;
  class Variable_assignment_statementContext;
  class Simple_variable_assignmentContext;
  class Conditional_variable_assignmentContext;
  class Selected_variable_assignmentContext;
  class Procedure_call_statementContext;
  class Procedure_callContext;
  class If_statementContext;
  class Case_statementContext;
  class Case_statement_alternativeContext;
  class Loop_statementContext;
  class Iteration_schemeContext;
  class Parameter_specificationContext;
  class Next_statementContext;
  class Exit_statementContext;
  class Return_statementContext;
  class Null_statementContext;
  class Concurrent_statement_with_optional_labelContext;
  class Concurrent_statementContext;
  class Block_statementContext;
  class Block_headerContext;
  class Process_statementContext;
  class Process_sensitivity_listContext;
  class Process_or_package_declarative_itemContext;
  class Process_declarative_itemContext;
  class Concurrent_procedure_call_statementContext;
  class Concurrent_assertion_statementContext;
  class Concurrent_signal_assignment_statementContext;
  class Concurrent_signal_assignment_anyContext;
  class Concurrent_selected_signal_assignmentContext;
  class Component_instantiation_statementContext;
  class Instantiated_unitContext;
  class Generate_statementContext;
  class For_generate_statementContext;
  class If_generate_statementContext;
  class Case_generate_statementContext;
  class Case_generate_alternativeContext;
  class Generate_statement_body_with_begin_endContext;
  class Generate_statement_bodyContext;
  class LabelContext;
  class Use_clauseContext;
  class Design_fileContext;
  class Design_unitContext;
  class Library_unitContext;
  class Primary_unitContext;
  class Secondary_unitContext;
  class Library_clauseContext;
  class Logical_name_listContext;
  class Context_declarationContext;
  class Context_clauseContext;
  class Context_itemContext;
  class Context_referenceContext;
  class IdentifierContext; 

  class  Any_keywordContext : public antlr4::ParserRuleContext {
  public:
    Any_keywordContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PROCESS();
    antlr4::tree::TerminalNode *KW_CONTEXT();
    antlr4::tree::TerminalNode *KW_POSTPONED();
    antlr4::tree::TerminalNode *KW_LINKAGE();
    antlr4::tree::TerminalNode *KW_COMPONENT();
    antlr4::tree::TerminalNode *KW_ABS();
    antlr4::tree::TerminalNode *KW_DEFAULT();
    antlr4::tree::TerminalNode *KW_THEN();
    antlr4::tree::TerminalNode *KW_BLOCK();
    antlr4::tree::TerminalNode *KW_REM();
    antlr4::tree::TerminalNode *KW_INERTIAL();
    antlr4::tree::TerminalNode *KW_NEXT();
    antlr4::tree::TerminalNode *KW_ENTITY();
    antlr4::tree::TerminalNode *KW_ON();
    antlr4::tree::TerminalNode *KW_GROUP();
    antlr4::tree::TerminalNode *KW_XNOR();
    antlr4::tree::TerminalNode *KW_FILE();
    antlr4::tree::TerminalNode *KW_PURE();
    antlr4::tree::TerminalNode *KW_GUARDED();
    antlr4::tree::TerminalNode *KW_GENERIC();
    antlr4::tree::TerminalNode *KW_RANGE();
    antlr4::tree::TerminalNode *KW_ELSE();
    antlr4::tree::TerminalNode *KW_USE();
    antlr4::tree::TerminalNode *KW_SHARED();
    antlr4::tree::TerminalNode *KW_MOD();
    antlr4::tree::TerminalNode *KW_LOOP();
    antlr4::tree::TerminalNode *KW_RECORD();
    antlr4::tree::TerminalNode *KW_SIGNAL();
    antlr4::tree::TerminalNode *KW_REJECT();
    antlr4::tree::TerminalNode *KW_BEGIN();
    antlr4::tree::TerminalNode *KW_SLA();
    antlr4::tree::TerminalNode *KW_DISCONNECT();
    antlr4::tree::TerminalNode *KW_OF();
    antlr4::tree::TerminalNode *KW_PROCEDURE();
    antlr4::tree::TerminalNode *KW_SRL();
    antlr4::tree::TerminalNode *KW_VUNIT();
    antlr4::tree::TerminalNode *KW_ATTRIBUTE();
    antlr4::tree::TerminalNode *KW_VARIABLE();
    antlr4::tree::TerminalNode *KW_PROPERTY();
    antlr4::tree::TerminalNode *KW_UNAFFECTED();
    antlr4::tree::TerminalNode *KW_XOR();
    antlr4::tree::TerminalNode *KW_REGISTER();
    antlr4::tree::TerminalNode *KW_SUBTYPE();
    antlr4::tree::TerminalNode *KW_TO();
    antlr4::tree::TerminalNode *KW_NEW();
    antlr4::tree::TerminalNode *KW_REPORT();
    antlr4::tree::TerminalNode *KW_CONSTANT();
    antlr4::tree::TerminalNode *KW_BUFFER();
    antlr4::tree::TerminalNode *KW_BODY();
    antlr4::tree::TerminalNode *KW_AFTER();
    antlr4::tree::TerminalNode *KW_TRANSPORT();
    antlr4::tree::TerminalNode *KW_FUNCTION();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *KW_SELECT();
    antlr4::tree::TerminalNode *KW_OR();
    antlr4::tree::TerminalNode *KW_LIBRARY();
    antlr4::tree::TerminalNode *KW_ELSIF();
    antlr4::tree::TerminalNode *KW_SLL();
    antlr4::tree::TerminalNode *KW_MAP();
    antlr4::tree::TerminalNode *KW_SRA();
    antlr4::tree::TerminalNode *KW_PROTECTED();
    antlr4::tree::TerminalNode *KW_DOWNTO();
    antlr4::tree::TerminalNode *KW_LABEL();
    antlr4::tree::TerminalNode *KW_ALL();
    antlr4::tree::TerminalNode *KW_ALIAS();
    antlr4::tree::TerminalNode *KW_GENERATE();
    antlr4::tree::TerminalNode *KW_NOR();
    antlr4::tree::TerminalNode *KW_IN();
    antlr4::tree::TerminalNode *KW_RELEASE();
    antlr4::tree::TerminalNode *KW_EXIT();
    antlr4::tree::TerminalNode *KW_RETURN();
    antlr4::tree::TerminalNode *KW_WITH();
    antlr4::tree::TerminalNode *KW_UNTIL();
    antlr4::tree::TerminalNode *KW_AND();
    antlr4::tree::TerminalNode *KW_INOUT();
    antlr4::tree::TerminalNode *KW_WAIT();
    antlr4::tree::TerminalNode *KW_NAND();
    antlr4::tree::TerminalNode *KW_ARRAY();
    antlr4::tree::TerminalNode *KW_FORCE();
    antlr4::tree::TerminalNode *KW_WHILE();
    antlr4::tree::TerminalNode *KW_IMPURE();
    antlr4::tree::TerminalNode *KW_PACKAGE();
    antlr4::tree::TerminalNode *KW_UNITS();
    antlr4::tree::TerminalNode *KW_ASSERT();
    antlr4::tree::TerminalNode *KW_PARAMETER();
    antlr4::tree::TerminalNode *KW_SEVERITY();
    antlr4::tree::TerminalNode *KW_LITERAL();
    antlr4::tree::TerminalNode *KW_FOR();
    antlr4::tree::TerminalNode *KW_ROR();
    antlr4::tree::TerminalNode *KW_IF();
    antlr4::tree::TerminalNode *KW_OUT();
    antlr4::tree::TerminalNode *KW_ROL();
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_SEQUENCE();
    antlr4::tree::TerminalNode *KW_OTHERS();
    antlr4::tree::TerminalNode *KW_TYPE();
    antlr4::tree::TerminalNode *KW_CASE();
    antlr4::tree::TerminalNode *KW_NOT();
    antlr4::tree::TerminalNode *KW_CONFIGURATION();
    antlr4::tree::TerminalNode *KW_OPEN();
    antlr4::tree::TerminalNode *KW_ARCHITECTURE();
    antlr4::tree::TerminalNode *KW_BUS();
    antlr4::tree::TerminalNode *KW_ACCESS();
    antlr4::tree::TerminalNode *KW_WHEN();
    antlr4::tree::TerminalNode *KW_PORT();
    antlr4::tree::TerminalNode *KW_NULL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Any_keywordContext* any_keyword();

  class  Name_literalContext : public antlr4::ParserRuleContext {
  public:
    Name_literalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Operator_symbolContext *operator_symbol();
    antlr4::tree::TerminalNode *CHARACTER_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Name_literalContext* name_literal();

  class  NameContext : public antlr4::ParserRuleContext {
  public:
    NameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Name_literalContext *name_literal();
    External_nameContext *external_name();
    NameContext *name();
    Name_slice_partContext *name_slice_part();
    Name_attribute_partContext *name_attribute_part();
    Association_listContext *association_list();
    antlr4::tree::TerminalNode *DOT();
    SuffixContext *suffix();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  NameContext* name();
  NameContext* name(int precedence);
  class  Name_slice_partContext : public antlr4::ParserRuleContext {
  public:
    Name_slice_partContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    Explicit_rangeContext *explicit_range();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Name_slice_partContext* name_slice_part();

  class  Name_attribute_partContext : public antlr4::ParserRuleContext {
  public:
    Name_attribute_partContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *APOSTROPHE();
    Attribute_designatorContext *attribute_designator();
    SignatureContext *signature();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Name_attribute_partContext* name_attribute_part();

  class  Attribute_nameContext : public antlr4::ParserRuleContext {
  public:
    Attribute_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    Name_attribute_partContext *name_attribute_part();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Attribute_nameContext* attribute_name();

  class  SuffixContext : public antlr4::ParserRuleContext {
  public:
    SuffixContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Name_literalContext *name_literal();
    antlr4::tree::TerminalNode *KW_ALL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SuffixContext* suffix();

  class  Explicit_rangeContext : public antlr4::ParserRuleContext {
  public:
    Explicit_rangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Simple_expressionContext *> simple_expression();
    Simple_expressionContext* simple_expression(size_t i);
    DirectionContext *direction();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Explicit_rangeContext* explicit_range();

  class  Selected_nameContext : public antlr4::ParserRuleContext {
  public:
    Selected_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);
    std::vector<SuffixContext *> suffix();
    SuffixContext* suffix(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Selected_nameContext* selected_name();

  class  Entity_declarationContext : public antlr4::ParserRuleContext {
  public:
    Entity_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_ENTITY();
    antlr4::tree::TerminalNode* KW_ENTITY(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    Generic_clauseContext *generic_clause();
    Port_clauseContext *port_clause();
    std::vector<Entity_declarative_itemContext *> entity_declarative_item();
    Entity_declarative_itemContext* entity_declarative_item(size_t i);
    antlr4::tree::TerminalNode *KW_BEGIN();
    std::vector<Entity_statementContext *> entity_statement();
    Entity_statementContext* entity_statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_declarationContext* entity_declaration();

  class  Entity_declarative_itemContext : public antlr4::ParserRuleContext {
  public:
    Entity_declarative_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Signal_declarationContext *signal_declaration();
    Process_declarative_itemContext *process_declarative_item();
    Disconnection_specificationContext *disconnection_specification();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_declarative_itemContext* entity_declarative_item();

  class  Entity_statementContext : public antlr4::ParserRuleContext {
  public:
    Entity_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Concurrent_assertion_statementContext *concurrent_assertion_statement();
    Concurrent_procedure_call_statementContext *concurrent_procedure_call_statement();
    Process_statementContext *process_statement();
    LabelContext *label();
    antlr4::tree::TerminalNode *COLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_statementContext* entity_statement();

  class  Architecture_bodyContext : public antlr4::ParserRuleContext {
  public:
    Architecture_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_ARCHITECTURE();
    antlr4::tree::TerminalNode* KW_ARCHITECTURE(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_OF();
    NameContext *name();
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_BEGIN();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Block_declarative_itemContext *> block_declarative_item();
    Block_declarative_itemContext* block_declarative_item(size_t i);
    std::vector<Concurrent_statementContext *> concurrent_statement();
    Concurrent_statementContext* concurrent_statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Architecture_bodyContext* architecture_body();

  class  Block_declarative_itemContext : public antlr4::ParserRuleContext {
  public:
    Block_declarative_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Entity_declarative_itemContext *entity_declarative_item();
    Component_declarationContext *component_declaration();
    Configuration_specificationContext *configuration_specification();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Block_declarative_itemContext* block_declarative_item();

  class  Configuration_declarationContext : public antlr4::ParserRuleContext {
  public:
    Configuration_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_CONFIGURATION();
    antlr4::tree::TerminalNode* KW_CONFIGURATION(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_OF();
    NameContext *name();
    antlr4::tree::TerminalNode *KW_IS();
    Block_configurationContext *block_configuration();
    antlr4::tree::TerminalNode *KW_END();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    std::vector<Configuration_declarative_itemContext *> configuration_declarative_item();
    Configuration_declarative_itemContext* configuration_declarative_item(size_t i);
    std::vector<Verification_unit_binding_indicationContext *> verification_unit_binding_indication();
    Verification_unit_binding_indicationContext* verification_unit_binding_indication(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Configuration_declarationContext* configuration_declaration();

  class  Configuration_declarative_itemContext : public antlr4::ParserRuleContext {
  public:
    Configuration_declarative_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Use_clauseContext *use_clause();
    Attribute_specificationContext *attribute_specification();
    Group_declarationContext *group_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Configuration_declarative_itemContext* configuration_declarative_item();

  class  Block_configurationContext : public antlr4::ParserRuleContext {
  public:
    Block_configurationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_FOR();
    antlr4::tree::TerminalNode* KW_FOR(size_t i);
    Block_specificationContext *block_specification();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Use_clauseContext *> use_clause();
    Use_clauseContext* use_clause(size_t i);
    std::vector<Configuration_itemContext *> configuration_item();
    Configuration_itemContext* configuration_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Block_configurationContext* block_configuration();

  class  Block_specificationContext : public antlr4::ParserRuleContext {
  public:
    Block_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LabelContext *label();
    antlr4::tree::TerminalNode *LPAREN();
    Generate_specificationContext *generate_specification();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Block_specificationContext* block_specification();

  class  Generate_specificationContext : public antlr4::ParserRuleContext {
  public:
    Generate_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Discrete_rangeContext *discrete_range();
    ExpressionContext *expression();
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generate_specificationContext* generate_specification();

  class  Configuration_itemContext : public antlr4::ParserRuleContext {
  public:
    Configuration_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Block_configurationContext *block_configuration();
    Component_configurationContext *component_configuration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Configuration_itemContext* configuration_item();

  class  Component_configurationContext : public antlr4::ParserRuleContext {
  public:
    Component_configurationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_FOR();
    antlr4::tree::TerminalNode* KW_FOR(size_t i);
    Component_specificationContext *component_specification();
    antlr4::tree::TerminalNode *KW_END();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    Binding_indicationContext *binding_indication();
    std::vector<Verification_unit_binding_indicationContext *> verification_unit_binding_indication();
    Verification_unit_binding_indicationContext* verification_unit_binding_indication(size_t i);
    Block_configurationContext *block_configuration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Component_configurationContext* component_configuration();

  class  Subprogram_declarationContext : public antlr4::ParserRuleContext {
  public:
    Subprogram_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Subprogram_specificationContext *subprogram_specification();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Subprogram_declarationContext* subprogram_declaration();

  class  Subprogram_specificationContext : public antlr4::ParserRuleContext {
  public:
    Subprogram_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Procedure_specificationContext *procedure_specification();
    Function_specificationContext *function_specification();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Subprogram_specificationContext* subprogram_specification();

  class  Procedure_specificationContext : public antlr4::ParserRuleContext {
  public:
    Procedure_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PROCEDURE();
    DesignatorContext *designator();
    Subprogram_headerContext *subprogram_header();
    antlr4::tree::TerminalNode *LPAREN();
    Formal_parameter_listContext *formal_parameter_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_PARAMETER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Procedure_specificationContext* procedure_specification();

  class  Function_specificationContext : public antlr4::ParserRuleContext {
  public:
    Function_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FUNCTION();
    DesignatorContext *designator();
    antlr4::tree::TerminalNode *KW_RETURN();
    Type_markContext *type_mark();
    Subprogram_headerContext *subprogram_header();
    antlr4::tree::TerminalNode *LPAREN();
    Formal_parameter_listContext *formal_parameter_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_PURE();
    antlr4::tree::TerminalNode *KW_IMPURE();
    antlr4::tree::TerminalNode *KW_PARAMETER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Function_specificationContext* function_specification();

  class  Subprogram_headerContext : public antlr4::ParserRuleContext {
  public:
    Subprogram_headerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_GENERIC();
    antlr4::tree::TerminalNode *LPAREN();
    Generic_listContext *generic_list();
    antlr4::tree::TerminalNode *RPAREN();
    Generic_map_aspectContext *generic_map_aspect();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Subprogram_headerContext* subprogram_header();

  class  DesignatorContext : public antlr4::ParserRuleContext {
  public:
    DesignatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Operator_symbolContext *operator_symbol();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DesignatorContext* designator();

  class  Operator_symbolContext : public antlr4::ParserRuleContext {
  public:
    Operator_symbolContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *STRING_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Operator_symbolContext* operator_symbol();

  class  Formal_parameter_listContext : public antlr4::ParserRuleContext {
  public:
    Formal_parameter_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Interface_listContext *interface_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Formal_parameter_listContext* formal_parameter_list();

  class  Subprogram_bodyContext : public antlr4::ParserRuleContext {
  public:
    Subprogram_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Subprogram_specificationContext *subprogram_specification();
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_BEGIN();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Process_declarative_itemContext *> process_declarative_item();
    Process_declarative_itemContext* process_declarative_item(size_t i);
    std::vector<Sequential_statementContext *> sequential_statement();
    Sequential_statementContext* sequential_statement(size_t i);
    Subprogram_kindContext *subprogram_kind();
    DesignatorContext *designator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Subprogram_bodyContext* subprogram_body();

  class  Subprogram_kindContext : public antlr4::ParserRuleContext {
  public:
    Subprogram_kindContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PROCEDURE();
    antlr4::tree::TerminalNode *KW_FUNCTION();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Subprogram_kindContext* subprogram_kind();

  class  Subprogram_instantiation_declarationContext : public antlr4::ParserRuleContext {
  public:
    Subprogram_instantiation_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Subprogram_kindContext *subprogram_kind();
    DesignatorContext *designator();
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_NEW();
    NameContext *name();
    antlr4::tree::TerminalNode *SEMI();
    SignatureContext *signature();
    Generic_map_aspectContext *generic_map_aspect();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Subprogram_instantiation_declarationContext* subprogram_instantiation_declaration();

  class  SignatureContext : public antlr4::ParserRuleContext {
  public:
    SignatureContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LSQUARE_BR();
    antlr4::tree::TerminalNode *RSQUARE_BR();
    std::vector<Type_markContext *> type_mark();
    Type_markContext* type_mark(size_t i);
    antlr4::tree::TerminalNode *KW_RETURN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SignatureContext* signature();

  class  Package_declarationContext : public antlr4::ParserRuleContext {
  public:
    Package_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_PACKAGE();
    antlr4::tree::TerminalNode* KW_PACKAGE(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_END();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    Generic_clauseContext *generic_clause();
    std::vector<Package_declarative_itemContext *> package_declarative_item();
    Package_declarative_itemContext* package_declarative_item(size_t i);
    Generic_map_aspectContext *generic_map_aspect();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_declarationContext* package_declaration();

  class  Package_declarative_itemContext : public antlr4::ParserRuleContext {
  public:
    Package_declarative_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Process_or_package_declarative_itemContext *process_or_package_declarative_item();
    Signal_declarationContext *signal_declaration();
    Component_declarationContext *component_declaration();
    Disconnection_specificationContext *disconnection_specification();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_declarative_itemContext* package_declarative_item();

  class  Package_bodyContext : public antlr4::ParserRuleContext {
  public:
    Package_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_PACKAGE();
    antlr4::tree::TerminalNode* KW_PACKAGE(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_BODY();
    antlr4::tree::TerminalNode* KW_BODY(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Process_declarative_itemContext *> process_declarative_item();
    Process_declarative_itemContext* process_declarative_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_bodyContext* package_body();

  class  Package_instantiation_declarationContext : public antlr4::ParserRuleContext {
  public:
    Package_instantiation_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PACKAGE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_NEW();
    NameContext *name();
    antlr4::tree::TerminalNode *SEMI();
    Generic_map_aspectContext *generic_map_aspect();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_instantiation_declarationContext* package_instantiation_declaration();

  class  Scalar_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Scalar_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Enumeration_type_definitionContext *enumeration_type_definition();
    Integer_type_definitionContext *integer_type_definition();
    Floating_type_definitionContext *floating_type_definition();
    Physical_type_definitionContext *physical_type_definition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Scalar_type_definitionContext* scalar_type_definition();

  class  Range_constraintContext : public antlr4::ParserRuleContext {
  public:
    Range_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_RANGE();
    RangeContext *range();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Range_constraintContext* range_constraint();

  class  RangeContext : public antlr4::ParserRuleContext {
  public:
    RangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Attribute_nameContext *attribute_name();
    Explicit_rangeContext *explicit_range();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  RangeContext* range();

  class  DirectionContext : public antlr4::ParserRuleContext {
  public:
    DirectionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TO();
    antlr4::tree::TerminalNode *KW_DOWNTO();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  DirectionContext* direction();

  class  Enumeration_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Enumeration_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Enumeration_literalContext *> enumeration_literal();
    Enumeration_literalContext* enumeration_literal(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enumeration_type_definitionContext* enumeration_type_definition();

  class  Enumeration_literalContext : public antlr4::ParserRuleContext {
  public:
    Enumeration_literalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *CHARACTER_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Enumeration_literalContext* enumeration_literal();

  class  Integer_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Integer_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Range_constraintContext *range_constraint();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Integer_type_definitionContext* integer_type_definition();

  class  Physical_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Physical_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Range_constraintContext *range_constraint();
    std::vector<antlr4::tree::TerminalNode *> KW_UNITS();
    antlr4::tree::TerminalNode* KW_UNITS(size_t i);
    Primary_unit_declarationContext *primary_unit_declaration();
    antlr4::tree::TerminalNode *KW_END();
    std::vector<Secondary_unit_declarationContext *> secondary_unit_declaration();
    Secondary_unit_declarationContext* secondary_unit_declaration(size_t i);
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Physical_type_definitionContext* physical_type_definition();

  class  Primary_unit_declarationContext : public antlr4::ParserRuleContext {
  public:
    Primary_unit_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Primary_unit_declarationContext* primary_unit_declaration();

  class  Secondary_unit_declarationContext : public antlr4::ParserRuleContext {
  public:
    Secondary_unit_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *EQ();
    Physical_literalContext *physical_literal();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Secondary_unit_declarationContext* secondary_unit_declaration();

  class  Floating_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Floating_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Range_constraintContext *range_constraint();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Floating_type_definitionContext* floating_type_definition();

  class  Composite_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Composite_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Array_type_definitionContext *array_type_definition();
    Record_type_definitionContext *record_type_definition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Composite_type_definitionContext* composite_type_definition();

  class  Array_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Array_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Unbounded_array_definitionContext *unbounded_array_definition();
    Constrained_array_definitionContext *constrained_array_definition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Array_type_definitionContext* array_type_definition();

  class  Unbounded_array_definitionContext : public antlr4::ParserRuleContext {
  public:
    Unbounded_array_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ARRAY();
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Index_subtype_definitionContext *> index_subtype_definition();
    Index_subtype_definitionContext* index_subtype_definition(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_OF();
    Subtype_indicationContext *subtype_indication();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Unbounded_array_definitionContext* unbounded_array_definition();

  class  Constrained_array_definitionContext : public antlr4::ParserRuleContext {
  public:
    Constrained_array_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ARRAY();
    Index_constraintContext *index_constraint();
    antlr4::tree::TerminalNode *KW_OF();
    Subtype_indicationContext *subtype_indication();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constrained_array_definitionContext* constrained_array_definition();

  class  Index_subtype_definitionContext : public antlr4::ParserRuleContext {
  public:
    Index_subtype_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_markContext *type_mark();
    antlr4::tree::TerminalNode *KW_RANGE();
    antlr4::tree::TerminalNode *BOX();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Index_subtype_definitionContext* index_subtype_definition();

  class  Array_constraintContext : public antlr4::ParserRuleContext {
  public:
    Array_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Index_constraintContext *index_constraint();
    Array_element_constraintContext *array_element_constraint();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *KW_OPEN();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Array_constraintContext* array_constraint();

  class  Array_element_constraintContext : public antlr4::ParserRuleContext {
  public:
    Array_element_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Element_constraintContext *element_constraint();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Array_element_constraintContext* array_element_constraint();

  class  Index_constraintContext : public antlr4::ParserRuleContext {
  public:
    Index_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Discrete_rangeContext *> discrete_range();
    Discrete_rangeContext* discrete_range(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Index_constraintContext* index_constraint();

  class  Discrete_rangeContext : public antlr4::ParserRuleContext {
  public:
    Discrete_rangeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Subtype_indicationContext *subtype_indication();
    RangeContext *range();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Discrete_rangeContext* discrete_range();

  class  Record_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Record_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_RECORD();
    antlr4::tree::TerminalNode* KW_RECORD(size_t i);
    antlr4::tree::TerminalNode *KW_END();
    std::vector<Element_declarationContext *> element_declaration();
    Element_declarationContext* element_declaration(size_t i);
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Record_type_definitionContext* record_type_definition();

  class  Element_declarationContext : public antlr4::ParserRuleContext {
  public:
    Element_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *COLON();
    Element_subtype_definitionContext *element_subtype_definition();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Element_declarationContext* element_declaration();

  class  Identifier_listContext : public antlr4::ParserRuleContext {
  public:
    Identifier_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Identifier_listContext* identifier_list();

  class  Element_subtype_definitionContext : public antlr4::ParserRuleContext {
  public:
    Element_subtype_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Subtype_indicationContext *subtype_indication();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Element_subtype_definitionContext* element_subtype_definition();

  class  Record_constraintContext : public antlr4::ParserRuleContext {
  public:
    Record_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Record_element_constraintContext *> record_element_constraint();
    Record_element_constraintContext* record_element_constraint(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Record_constraintContext* record_constraint();

  class  Record_element_constraintContext : public antlr4::ParserRuleContext {
  public:
    Record_element_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Element_constraintContext *element_constraint();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Record_element_constraintContext* record_element_constraint();

  class  Access_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Access_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ACCESS();
    Subtype_indicationContext *subtype_indication();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Access_type_definitionContext* access_type_definition();

  class  Incomplete_type_declarationContext : public antlr4::ParserRuleContext {
  public:
    Incomplete_type_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TYPE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Incomplete_type_declarationContext* incomplete_type_declaration();

  class  File_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    File_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FILE();
    antlr4::tree::TerminalNode *KW_OF();
    Type_markContext *type_mark();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  File_type_definitionContext* file_type_definition();

  class  Protected_type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Protected_type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Protected_type_declarationContext *protected_type_declaration();
    Protected_type_bodyContext *protected_type_body();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Protected_type_definitionContext* protected_type_definition();

  class  Protected_type_declarationContext : public antlr4::ParserRuleContext {
  public:
    Protected_type_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_PROTECTED();
    antlr4::tree::TerminalNode* KW_PROTECTED(size_t i);
    antlr4::tree::TerminalNode *KW_END();
    std::vector<Protected_type_declarative_itemContext *> protected_type_declarative_item();
    Protected_type_declarative_itemContext* protected_type_declarative_item(size_t i);
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Protected_type_declarationContext* protected_type_declaration();

  class  Protected_type_declarative_itemContext : public antlr4::ParserRuleContext {
  public:
    Protected_type_declarative_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Subprogram_declarationContext *subprogram_declaration();
    Subprogram_instantiation_declarationContext *subprogram_instantiation_declaration();
    Attribute_specificationContext *attribute_specification();
    Use_clauseContext *use_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Protected_type_declarative_itemContext* protected_type_declarative_item();

  class  Protected_type_bodyContext : public antlr4::ParserRuleContext {
  public:
    Protected_type_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_PROTECTED();
    antlr4::tree::TerminalNode* KW_PROTECTED(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_BODY();
    antlr4::tree::TerminalNode* KW_BODY(size_t i);
    antlr4::tree::TerminalNode *KW_END();
    std::vector<Process_declarative_itemContext *> process_declarative_item();
    Process_declarative_itemContext* process_declarative_item(size_t i);
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Protected_type_bodyContext* protected_type_body();

  class  Type_declarationContext : public antlr4::ParserRuleContext {
  public:
    Type_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Full_type_declarationContext *full_type_declaration();
    Incomplete_type_declarationContext *incomplete_type_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_declarationContext* type_declaration();

  class  Full_type_declarationContext : public antlr4::ParserRuleContext {
  public:
    Full_type_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TYPE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_IS();
    Type_definitionContext *type_definition();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Full_type_declarationContext* full_type_declaration();

  class  Type_definitionContext : public antlr4::ParserRuleContext {
  public:
    Type_definitionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Scalar_type_definitionContext *scalar_type_definition();
    Composite_type_definitionContext *composite_type_definition();
    Access_type_definitionContext *access_type_definition();
    File_type_definitionContext *file_type_definition();
    Protected_type_definitionContext *protected_type_definition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_definitionContext* type_definition();

  class  Subtype_declarationContext : public antlr4::ParserRuleContext {
  public:
    Subtype_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SUBTYPE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_IS();
    Subtype_indicationContext *subtype_indication();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Subtype_declarationContext* subtype_declaration();

  class  Subtype_indicationContext : public antlr4::ParserRuleContext {
  public:
    Subtype_indicationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_markContext *type_mark();
    Resolution_indicationContext *resolution_indication();
    ConstraintContext *constraint();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Subtype_indicationContext* subtype_indication();

  class  Resolution_indicationContext : public antlr4::ParserRuleContext {
  public:
    Resolution_indicationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    antlr4::tree::TerminalNode *LPAREN();
    Element_resolutionContext *element_resolution();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Resolution_indicationContext* resolution_indication();

  class  Element_resolutionContext : public antlr4::ParserRuleContext {
  public:
    Element_resolutionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Array_element_resolutionContext *array_element_resolution();
    Record_resolutionContext *record_resolution();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Element_resolutionContext* element_resolution();

  class  Array_element_resolutionContext : public antlr4::ParserRuleContext {
  public:
    Array_element_resolutionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Resolution_indicationContext *resolution_indication();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Array_element_resolutionContext* array_element_resolution();

  class  Record_resolutionContext : public antlr4::ParserRuleContext {
  public:
    Record_resolutionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Record_element_resolutionContext *> record_element_resolution();
    Record_element_resolutionContext* record_element_resolution(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Record_resolutionContext* record_resolution();

  class  Record_element_resolutionContext : public antlr4::ParserRuleContext {
  public:
    Record_element_resolutionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Resolution_indicationContext *resolution_indication();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Record_element_resolutionContext* record_element_resolution();

  class  Type_markContext : public antlr4::ParserRuleContext {
  public:
    Type_markContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Type_markContext* type_mark();

  class  ConstraintContext : public antlr4::ParserRuleContext {
  public:
    ConstraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Range_constraintContext *range_constraint();
    Element_constraintContext *element_constraint();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConstraintContext* constraint();

  class  Element_constraintContext : public antlr4::ParserRuleContext {
  public:
    Element_constraintContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Array_constraintContext *array_constraint();
    Record_constraintContext *record_constraint();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Element_constraintContext* element_constraint();

  class  Object_declarationContext : public antlr4::ParserRuleContext {
  public:
    Object_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Constant_declarationContext *constant_declaration();
    Signal_declarationContext *signal_declaration();
    Variable_declarationContext *variable_declaration();
    File_declarationContext *file_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Object_declarationContext* object_declaration();

  class  Constant_declarationContext : public antlr4::ParserRuleContext {
  public:
    Constant_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONSTANT();
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *VARASGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Constant_declarationContext* constant_declaration();

  class  Signal_declarationContext : public antlr4::ParserRuleContext {
  public:
    Signal_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SIGNAL();
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();
    antlr4::tree::TerminalNode *SEMI();
    Signal_kindContext *signal_kind();
    antlr4::tree::TerminalNode *VARASGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Signal_declarationContext* signal_declaration();

  class  Signal_kindContext : public antlr4::ParserRuleContext {
  public:
    Signal_kindContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_REGISTER();
    antlr4::tree::TerminalNode *KW_BUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Signal_kindContext* signal_kind();

  class  Variable_declarationContext : public antlr4::ParserRuleContext {
  public:
    Variable_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_VARIABLE();
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_SHARED();
    antlr4::tree::TerminalNode *VARASGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Variable_declarationContext* variable_declaration();

  class  File_declarationContext : public antlr4::ParserRuleContext {
  public:
    File_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FILE();
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();
    antlr4::tree::TerminalNode *SEMI();
    File_open_informationContext *file_open_information();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  File_declarationContext* file_declaration();

  class  File_open_informationContext : public antlr4::ParserRuleContext {
  public:
    File_open_informationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IS();
    File_logical_nameContext *file_logical_name();
    antlr4::tree::TerminalNode *KW_OPEN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_IN();
    antlr4::tree::TerminalNode *KW_OUT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  File_open_informationContext* file_open_information();

  class  File_logical_nameContext : public antlr4::ParserRuleContext {
  public:
    File_logical_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  File_logical_nameContext* file_logical_name();

  class  Interface_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Interface_object_declarationContext *interface_object_declaration();
    Interface_type_declarationContext *interface_type_declaration();
    Interface_subprogram_declarationContext *interface_subprogram_declaration();
    Interface_package_declarationContext *interface_package_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_declarationContext* interface_declaration();

  class  Interface_object_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_object_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Interface_constant_declarationContext *interface_constant_declaration();
    Interface_signal_declarationContext *interface_signal_declaration();
    Interface_variable_declarationContext *interface_variable_declaration();
    Interface_file_declarationContext *interface_file_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_object_declarationContext* interface_object_declaration();

  class  Interface_constant_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_constant_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONSTANT();
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();
    antlr4::tree::TerminalNode *KW_IN();
    antlr4::tree::TerminalNode *VARASGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_constant_declarationContext* interface_constant_declaration();

  class  Interface_signal_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_signal_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();
    antlr4::tree::TerminalNode *KW_SIGNAL();
    Signal_modeContext *signal_mode();
    antlr4::tree::TerminalNode *KW_BUS();
    antlr4::tree::TerminalNode *VARASGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_signal_declarationContext* interface_signal_declaration();

  class  Interface_variable_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_variable_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_VARIABLE();
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();
    Signal_modeContext *signal_mode();
    antlr4::tree::TerminalNode *VARASGN();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_variable_declarationContext* interface_variable_declaration();

  class  Interface_file_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_file_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FILE();
    Identifier_listContext *identifier_list();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_file_declarationContext* interface_file_declaration();

  class  Signal_modeContext : public antlr4::ParserRuleContext {
  public:
    Signal_modeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IN();
    antlr4::tree::TerminalNode *KW_OUT();
    antlr4::tree::TerminalNode *KW_INOUT();
    antlr4::tree::TerminalNode *KW_BUFFER();
    antlr4::tree::TerminalNode *KW_LINKAGE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Signal_modeContext* signal_mode();

  class  Interface_type_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_type_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Interface_incomplete_type_declarationContext *interface_incomplete_type_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_type_declarationContext* interface_type_declaration();

  class  Interface_incomplete_type_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_incomplete_type_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TYPE();
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_incomplete_type_declarationContext* interface_incomplete_type_declaration();

  class  Interface_subprogram_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_subprogram_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Interface_subprogram_specificationContext *interface_subprogram_specification();
    antlr4::tree::TerminalNode *KW_IS();
    Interface_subprogram_defaultContext *interface_subprogram_default();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_subprogram_declarationContext* interface_subprogram_declaration();

  class  Interface_subprogram_specificationContext : public antlr4::ParserRuleContext {
  public:
    Interface_subprogram_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Interface_procedure_specificationContext *interface_procedure_specification();
    Interface_function_specificationContext *interface_function_specification();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_subprogram_specificationContext* interface_subprogram_specification();

  class  Interface_procedure_specificationContext : public antlr4::ParserRuleContext {
  public:
    Interface_procedure_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PROCEDURE();
    DesignatorContext *designator();
    antlr4::tree::TerminalNode *LPAREN();
    Formal_parameter_listContext *formal_parameter_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_PARAMETER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_procedure_specificationContext* interface_procedure_specification();

  class  Interface_function_specificationContext : public antlr4::ParserRuleContext {
  public:
    Interface_function_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FUNCTION();
    DesignatorContext *designator();
    antlr4::tree::TerminalNode *KW_RETURN();
    Type_markContext *type_mark();
    antlr4::tree::TerminalNode *LPAREN();
    Formal_parameter_listContext *formal_parameter_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_PURE();
    antlr4::tree::TerminalNode *KW_IMPURE();
    antlr4::tree::TerminalNode *KW_PARAMETER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_function_specificationContext* interface_function_specification();

  class  Interface_subprogram_defaultContext : public antlr4::ParserRuleContext {
  public:
    Interface_subprogram_defaultContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    antlr4::tree::TerminalNode *BOX();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_subprogram_defaultContext* interface_subprogram_default();

  class  Interface_package_declarationContext : public antlr4::ParserRuleContext {
  public:
    Interface_package_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PACKAGE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_NEW();
    NameContext *name();
    Interface_package_generic_map_aspectContext *interface_package_generic_map_aspect();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_package_declarationContext* interface_package_declaration();

  class  Interface_package_generic_map_aspectContext : public antlr4::ParserRuleContext {
  public:
    Interface_package_generic_map_aspectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Generic_map_aspectContext *generic_map_aspect();
    antlr4::tree::TerminalNode *KW_GENERIC();
    antlr4::tree::TerminalNode *KW_MAP();
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *BOX();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_DEFAULT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_package_generic_map_aspectContext* interface_package_generic_map_aspect();

  class  Interface_listContext : public antlr4::ParserRuleContext {
  public:
    Interface_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Interface_elementContext *> interface_element();
    Interface_elementContext* interface_element(size_t i);
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_listContext* interface_list();

  class  Interface_elementContext : public antlr4::ParserRuleContext {
  public:
    Interface_elementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Interface_declarationContext *interface_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Interface_elementContext* interface_element();

  class  Generic_clauseContext : public antlr4::ParserRuleContext {
  public:
    Generic_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_GENERIC();
    antlr4::tree::TerminalNode *LPAREN();
    Generic_listContext *generic_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generic_clauseContext* generic_clause();

  class  Generic_listContext : public antlr4::ParserRuleContext {
  public:
    Generic_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Interface_listContext *interface_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generic_listContext* generic_list();

  class  Port_clauseContext : public antlr4::ParserRuleContext {
  public:
    Port_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PORT();
    antlr4::tree::TerminalNode *LPAREN();
    Port_listContext *port_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Port_clauseContext* port_clause();

  class  Port_listContext : public antlr4::ParserRuleContext {
  public:
    Port_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Interface_listContext *interface_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Port_listContext* port_list();

  class  Association_listContext : public antlr4::ParserRuleContext {
  public:
    Association_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Association_elementContext *> association_element();
    Association_elementContext* association_element(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Association_listContext* association_list();

  class  Association_elementContext : public antlr4::ParserRuleContext {
  public:
    Association_elementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Actual_partContext *actual_part();
    Formal_partContext *formal_part();
    antlr4::tree::TerminalNode *ARROW();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Association_elementContext* association_element();

  class  Formal_partContext : public antlr4::ParserRuleContext {
  public:
    Formal_partContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<NameContext *> name();
    NameContext* name(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Formal_partContext* formal_part();

  class  Actual_partContext : public antlr4::ParserRuleContext {
  public:
    Actual_partContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    antlr4::tree::TerminalNode *LPAREN();
    Actual_designatorContext *actual_designator();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Actual_partContext* actual_part();

  class  Actual_designatorContext : public antlr4::ParserRuleContext {
  public:
    Actual_designatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_INERTIAL();
    Subtype_indicationContext *subtype_indication();
    antlr4::tree::TerminalNode *KW_OPEN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Actual_designatorContext* actual_designator();

  class  Generic_map_aspectContext : public antlr4::ParserRuleContext {
  public:
    Generic_map_aspectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_GENERIC();
    antlr4::tree::TerminalNode *KW_MAP();
    Association_listContext *association_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generic_map_aspectContext* generic_map_aspect();

  class  Port_map_aspectContext : public antlr4::ParserRuleContext {
  public:
    Port_map_aspectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_PORT();
    antlr4::tree::TerminalNode *KW_MAP();
    Association_listContext *association_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Port_map_aspectContext* port_map_aspect();

  class  Alias_declarationContext : public antlr4::ParserRuleContext {
  public:
    Alias_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ALIAS();
    Alias_designatorContext *alias_designator();
    antlr4::tree::TerminalNode *KW_IS();
    NameContext *name();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();
    SignatureContext *signature();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Alias_declarationContext* alias_declaration();

  class  Alias_designatorContext : public antlr4::ParserRuleContext {
  public:
    Alias_designatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *CHARACTER_LITERAL();
    Operator_symbolContext *operator_symbol();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Alias_designatorContext* alias_designator();

  class  Attribute_declarationContext : public antlr4::ParserRuleContext {
  public:
    Attribute_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ATTRIBUTE();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *COLON();
    Type_markContext *type_mark();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Attribute_declarationContext* attribute_declaration();

  class  Component_declarationContext : public antlr4::ParserRuleContext {
  public:
    Component_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_COMPONENT();
    antlr4::tree::TerminalNode* KW_COMPONENT(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_IS();
    Generic_clauseContext *generic_clause();
    Port_clauseContext *port_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Component_declarationContext* component_declaration();

  class  Group_template_declarationContext : public antlr4::ParserRuleContext {
  public:
    Group_template_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_GROUP();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *LPAREN();
    Entity_class_entry_listContext *entity_class_entry_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Group_template_declarationContext* group_template_declaration();

  class  Entity_class_entry_listContext : public antlr4::ParserRuleContext {
  public:
    Entity_class_entry_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Entity_class_entryContext *> entity_class_entry();
    Entity_class_entryContext* entity_class_entry(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_class_entry_listContext* entity_class_entry_list();

  class  Entity_class_entryContext : public antlr4::ParserRuleContext {
  public:
    Entity_class_entryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Entity_classContext *entity_class();
    antlr4::tree::TerminalNode *BOX();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_class_entryContext* entity_class_entry();

  class  Group_declarationContext : public antlr4::ParserRuleContext {
  public:
    Group_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_GROUP();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *COLON();
    NameContext *name();
    antlr4::tree::TerminalNode *LPAREN();
    Group_constituent_listContext *group_constituent_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Group_declarationContext* group_declaration();

  class  Group_constituent_listContext : public antlr4::ParserRuleContext {
  public:
    Group_constituent_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Group_constituentContext *> group_constituent();
    Group_constituentContext* group_constituent(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Group_constituent_listContext* group_constituent_list();

  class  Group_constituentContext : public antlr4::ParserRuleContext {
  public:
    Group_constituentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Group_constituentContext* group_constituent();

  class  Attribute_specificationContext : public antlr4::ParserRuleContext {
  public:
    Attribute_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ATTRIBUTE();
    Attribute_designatorContext *attribute_designator();
    antlr4::tree::TerminalNode *KW_OF();
    Entity_specificationContext *entity_specification();
    antlr4::tree::TerminalNode *KW_IS();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Attribute_specificationContext* attribute_specification();

  class  Entity_specificationContext : public antlr4::ParserRuleContext {
  public:
    Entity_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Entity_name_listContext *entity_name_list();
    antlr4::tree::TerminalNode *COLON();
    Entity_classContext *entity_class();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_specificationContext* entity_specification();

  class  Entity_classContext : public antlr4::ParserRuleContext {
  public:
    Entity_classContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ENTITY();
    antlr4::tree::TerminalNode *KW_ARCHITECTURE();
    antlr4::tree::TerminalNode *KW_CONFIGURATION();
    antlr4::tree::TerminalNode *KW_PROCEDURE();
    antlr4::tree::TerminalNode *KW_FUNCTION();
    antlr4::tree::TerminalNode *KW_PACKAGE();
    antlr4::tree::TerminalNode *KW_TYPE();
    antlr4::tree::TerminalNode *KW_SUBTYPE();
    antlr4::tree::TerminalNode *KW_CONSTANT();
    antlr4::tree::TerminalNode *KW_SIGNAL();
    antlr4::tree::TerminalNode *KW_VARIABLE();
    antlr4::tree::TerminalNode *KW_COMPONENT();
    antlr4::tree::TerminalNode *KW_LABEL();
    antlr4::tree::TerminalNode *KW_LITERAL();
    antlr4::tree::TerminalNode *KW_UNITS();
    antlr4::tree::TerminalNode *KW_GROUP();
    antlr4::tree::TerminalNode *KW_FILE();
    antlr4::tree::TerminalNode *KW_PROPERTY();
    antlr4::tree::TerminalNode *KW_SEQUENCE();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_classContext* entity_class();

  class  Entity_name_listContext : public antlr4::ParserRuleContext {
  public:
    Entity_name_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Entity_designatorContext *> entity_designator();
    Entity_designatorContext* entity_designator(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *KW_OTHERS();
    antlr4::tree::TerminalNode *KW_ALL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_name_listContext* entity_name_list();

  class  Entity_designatorContext : public antlr4::ParserRuleContext {
  public:
    Entity_designatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Entity_tagContext *entity_tag();
    SignatureContext *signature();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_designatorContext* entity_designator();

  class  Entity_tagContext : public antlr4::ParserRuleContext {
  public:
    Entity_tagContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *CHARACTER_LITERAL();
    Operator_symbolContext *operator_symbol();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_tagContext* entity_tag();

  class  Configuration_specificationContext : public antlr4::ParserRuleContext {
  public:
    Configuration_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Simple_configuration_specificationContext *simple_configuration_specification();
    Compound_configuration_specificationContext *compound_configuration_specification();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Configuration_specificationContext* configuration_specification();

  class  Simple_configuration_specificationContext : public antlr4::ParserRuleContext {
  public:
    Simple_configuration_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_FOR();
    antlr4::tree::TerminalNode* KW_FOR(size_t i);
    Component_specificationContext *component_specification();
    Binding_indicationContext *binding_indication();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    antlr4::tree::TerminalNode *KW_END();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_configuration_specificationContext* simple_configuration_specification();

  class  Compound_configuration_specificationContext : public antlr4::ParserRuleContext {
  public:
    Compound_configuration_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_FOR();
    antlr4::tree::TerminalNode* KW_FOR(size_t i);
    Component_specificationContext *component_specification();
    Binding_indicationContext *binding_indication();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    antlr4::tree::TerminalNode *KW_END();
    std::vector<Verification_unit_binding_indicationContext *> verification_unit_binding_indication();
    Verification_unit_binding_indicationContext* verification_unit_binding_indication(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Compound_configuration_specificationContext* compound_configuration_specification();

  class  Component_specificationContext : public antlr4::ParserRuleContext {
  public:
    Component_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Instantiation_listContext *instantiation_list();
    antlr4::tree::TerminalNode *COLON();
    NameContext *name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Component_specificationContext* component_specification();

  class  Instantiation_listContext : public antlr4::ParserRuleContext {
  public:
    Instantiation_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<LabelContext *> label();
    LabelContext* label(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *KW_OTHERS();
    antlr4::tree::TerminalNode *KW_ALL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Instantiation_listContext* instantiation_list();

  class  Binding_indicationContext : public antlr4::ParserRuleContext {
  public:
    Binding_indicationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_USE();
    Entity_aspectContext *entity_aspect();
    Generic_map_aspectContext *generic_map_aspect();
    Port_map_aspectContext *port_map_aspect();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Binding_indicationContext* binding_indication();

  class  Entity_aspectContext : public antlr4::ParserRuleContext {
  public:
    Entity_aspectContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ENTITY();
    NameContext *name();
    antlr4::tree::TerminalNode *LPAREN();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_CONFIGURATION();
    antlr4::tree::TerminalNode *KW_OPEN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Entity_aspectContext* entity_aspect();

  class  Verification_unit_binding_indicationContext : public antlr4::ParserRuleContext {
  public:
    Verification_unit_binding_indicationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_USE();
    antlr4::tree::TerminalNode *KW_VUNIT();
    Verification_unit_listContext *verification_unit_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Verification_unit_binding_indicationContext* verification_unit_binding_indication();

  class  Verification_unit_listContext : public antlr4::ParserRuleContext {
  public:
    Verification_unit_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<NameContext *> name();
    NameContext* name(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Verification_unit_listContext* verification_unit_list();

  class  Disconnection_specificationContext : public antlr4::ParserRuleContext {
  public:
    Disconnection_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_DISCONNECT();
    Guarded_signal_specificationContext *guarded_signal_specification();
    antlr4::tree::TerminalNode *KW_AFTER();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Disconnection_specificationContext* disconnection_specification();

  class  Guarded_signal_specificationContext : public antlr4::ParserRuleContext {
  public:
    Guarded_signal_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Signal_listContext *signal_list();
    antlr4::tree::TerminalNode *COLON();
    Type_markContext *type_mark();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Guarded_signal_specificationContext* guarded_signal_specification();

  class  Signal_listContext : public antlr4::ParserRuleContext {
  public:
    Signal_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<NameContext *> name();
    NameContext* name(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *KW_OTHERS();
    antlr4::tree::TerminalNode *KW_ALL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Signal_listContext* signal_list();

  class  Attribute_designatorContext : public antlr4::ParserRuleContext {
  public:
    Attribute_designatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    Any_keywordContext *any_keyword();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Attribute_designatorContext* attribute_designator();

  class  External_nameContext : public antlr4::ParserRuleContext {
  public:
    External_nameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *SHIFT_LEFT();
    External_pathnameContext *external_pathname();
    antlr4::tree::TerminalNode *COLON();
    Subtype_indicationContext *subtype_indication();
    antlr4::tree::TerminalNode *SHIFT_RIGHT();
    antlr4::tree::TerminalNode *KW_VARIABLE();
    antlr4::tree::TerminalNode *KW_CONSTANT();
    antlr4::tree::TerminalNode *KW_SIGNAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  External_nameContext* external_name();

  class  External_pathnameContext : public antlr4::ParserRuleContext {
  public:
    External_pathnameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Package_pathnameContext *package_pathname();
    Absolute_pathnameContext *absolute_pathname();
    Relative_pathnameContext *relative_pathname();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  External_pathnameContext* external_pathname();

  class  Package_pathnameContext : public antlr4::ParserRuleContext {
  public:
    Package_pathnameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *AT();
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Package_pathnameContext* package_pathname();

  class  Absolute_pathnameContext : public antlr4::ParserRuleContext {
  public:
    Absolute_pathnameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOT();
    Partial_pathnameContext *partial_pathname();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Absolute_pathnameContext* absolute_pathname();

  class  Relative_pathnameContext : public antlr4::ParserRuleContext {
  public:
    Relative_pathnameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Partial_pathnameContext *partial_pathname();
    std::vector<antlr4::tree::TerminalNode *> UP();
    antlr4::tree::TerminalNode* UP(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Relative_pathnameContext* relative_pathname();

  class  Partial_pathnameContext : public antlr4::ParserRuleContext {
  public:
    Partial_pathnameContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    std::vector<Pathname_elementContext *> pathname_element();
    Pathname_elementContext* pathname_element(size_t i);
    std::vector<antlr4::tree::TerminalNode *> DOT();
    antlr4::tree::TerminalNode* DOT(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Partial_pathnameContext* partial_pathname();

  class  Pathname_elementContext : public antlr4::ParserRuleContext {
  public:
    Pathname_elementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LabelContext *label();
    antlr4::tree::TerminalNode *LPAREN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *RPAREN();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Pathname_elementContext* pathname_element();

  class  ExpressionContext : public antlr4::ParserRuleContext {
  public:
    ExpressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *COND_OP();
    PrimaryContext *primary();
    Simple_expressionContext *simple_expression();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    Shift_operatorContext *shift_operator();
    Relational_operatorContext *relational_operator();
    Logical_operatorContext *logical_operator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ExpressionContext* expression();
  ExpressionContext* expression(int precedence);
  class  Simple_expressionContext : public antlr4::ParserRuleContext {
  public:
    Simple_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<PrimaryContext *> primary();
    PrimaryContext* primary(size_t i);
    antlr4::tree::TerminalNode *DOUBLESTAR();
    std::vector<Simple_expressionContext *> simple_expression();
    Simple_expressionContext* simple_expression(size_t i);
    antlr4::tree::TerminalNode *KW_ABS();
    antlr4::tree::TerminalNode *KW_NOT();
    Logical_operatorContext *logical_operator();
    SignContext *sign();
    Multiplying_operatorContext *multiplying_operator();
    Adding_operatorContext *adding_operator();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_expressionContext* simple_expression();
  Simple_expressionContext* simple_expression(int precedence);
  class  PrimaryContext : public antlr4::ParserRuleContext {
  public:
    PrimaryContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Numeric_literalContext *numeric_literal();
    antlr4::tree::TerminalNode *BIT_STRING_LITERAL();
    antlr4::tree::TerminalNode *KW_NULL();
    AllocatorContext *allocator();
    AggregateContext *aggregate();
    Qualified_expressionContext *qualified_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  PrimaryContext* primary();

  class  Logical_operatorContext : public antlr4::ParserRuleContext {
  public:
    Logical_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_AND();
    antlr4::tree::TerminalNode *KW_OR();
    antlr4::tree::TerminalNode *KW_NAND();
    antlr4::tree::TerminalNode *KW_NOR();
    antlr4::tree::TerminalNode *KW_XOR();
    antlr4::tree::TerminalNode *KW_XNOR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Logical_operatorContext* logical_operator();

  class  Relational_operatorContext : public antlr4::ParserRuleContext {
  public:
    Relational_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EQ();
    antlr4::tree::TerminalNode *NE();
    antlr4::tree::TerminalNode *LT();
    antlr4::tree::TerminalNode *CONASGN();
    antlr4::tree::TerminalNode *GT();
    antlr4::tree::TerminalNode *GE();
    antlr4::tree::TerminalNode *EQ_MATCH();
    antlr4::tree::TerminalNode *NE_MATCH();
    antlr4::tree::TerminalNode *LT_MATCH();
    antlr4::tree::TerminalNode *LE_MATCH();
    antlr4::tree::TerminalNode *GT_MATCH();
    antlr4::tree::TerminalNode *GE_MATCH();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Relational_operatorContext* relational_operator();

  class  Shift_operatorContext : public antlr4::ParserRuleContext {
  public:
    Shift_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_SLL();
    antlr4::tree::TerminalNode *KW_SRL();
    antlr4::tree::TerminalNode *KW_SLA();
    antlr4::tree::TerminalNode *KW_SRA();
    antlr4::tree::TerminalNode *KW_ROL();
    antlr4::tree::TerminalNode *KW_ROR();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Shift_operatorContext* shift_operator();

  class  Adding_operatorContext : public antlr4::ParserRuleContext {
  public:
    Adding_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();
    antlr4::tree::TerminalNode *AMPERSAND();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Adding_operatorContext* adding_operator();

  class  SignContext : public antlr4::ParserRuleContext {
  public:
    SignContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *PLUS();
    antlr4::tree::TerminalNode *MINUS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  SignContext* sign();

  class  Multiplying_operatorContext : public antlr4::ParserRuleContext {
  public:
    Multiplying_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *MUL();
    antlr4::tree::TerminalNode *DIV();
    antlr4::tree::TerminalNode *KW_MOD();
    antlr4::tree::TerminalNode *KW_REM();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Multiplying_operatorContext* multiplying_operator();

  class  Miscellaneous_operatorContext : public antlr4::ParserRuleContext {
  public:
    Miscellaneous_operatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DOUBLESTAR();
    antlr4::tree::TerminalNode *KW_ABS();
    antlr4::tree::TerminalNode *KW_NOT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Miscellaneous_operatorContext* miscellaneous_operator();

  class  Numeric_literalContext : public antlr4::ParserRuleContext {
  public:
    Numeric_literalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *DECIMAL_LITERAL();
    NameContext *name();
    antlr4::tree::TerminalNode *BASED_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Numeric_literalContext* numeric_literal();

  class  Physical_literalContext : public antlr4::ParserRuleContext {
  public:
    Physical_literalContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    antlr4::tree::TerminalNode *DECIMAL_LITERAL();
    antlr4::tree::TerminalNode *BASED_LITERAL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Physical_literalContext* physical_literal();

  class  AggregateContext : public antlr4::ParserRuleContext {
  public:
    AggregateContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *LPAREN();
    std::vector<Element_associationContext *> element_association();
    Element_associationContext* element_association(size_t i);
    antlr4::tree::TerminalNode *RPAREN();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AggregateContext* aggregate();

  class  Element_associationContext : public antlr4::ParserRuleContext {
  public:
    Element_associationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();
    ChoicesContext *choices();
    antlr4::tree::TerminalNode *ARROW();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Element_associationContext* element_association();

  class  ChoicesContext : public antlr4::ParserRuleContext {
  public:
    ChoicesContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ChoiceContext *> choice();
    ChoiceContext* choice(size_t i);
    std::vector<antlr4::tree::TerminalNode *> BAR();
    antlr4::tree::TerminalNode* BAR(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ChoicesContext* choices();

  class  ChoiceContext : public antlr4::ParserRuleContext {
  public:
    ChoiceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Discrete_rangeContext *discrete_range();
    Simple_expressionContext *simple_expression();
    antlr4::tree::TerminalNode *KW_OTHERS();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ChoiceContext* choice();

  class  Qualified_expressionContext : public antlr4::ParserRuleContext {
  public:
    Qualified_expressionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Type_markContext *type_mark();
    antlr4::tree::TerminalNode *APOSTROPHE();
    AggregateContext *aggregate();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Qualified_expressionContext* qualified_expression();

  class  AllocatorContext : public antlr4::ParserRuleContext {
  public:
    AllocatorContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_NEW();
    Subtype_indicationContext *subtype_indication();
    Qualified_expressionContext *qualified_expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AllocatorContext* allocator();

  class  Sequence_of_statementsContext : public antlr4::ParserRuleContext {
  public:
    Sequence_of_statementsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Sequential_statementContext *> sequential_statement();
    Sequential_statementContext* sequential_statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequence_of_statementsContext* sequence_of_statements();

  class  Sequential_statementContext : public antlr4::ParserRuleContext {
  public:
    Sequential_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Wait_statementContext *wait_statement();
    Assertion_statementContext *assertion_statement();
    Report_statementContext *report_statement();
    Signal_assignment_statementContext *signal_assignment_statement();
    Variable_assignment_statementContext *variable_assignment_statement();
    Procedure_call_statementContext *procedure_call_statement();
    If_statementContext *if_statement();
    Case_statementContext *case_statement();
    Loop_statementContext *loop_statement();
    Next_statementContext *next_statement();
    Exit_statementContext *exit_statement();
    Return_statementContext *return_statement();
    Null_statementContext *null_statement();
    LabelContext *label();
    antlr4::tree::TerminalNode *COLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sequential_statementContext* sequential_statement();

  class  Wait_statementContext : public antlr4::ParserRuleContext {
  public:
    Wait_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_WAIT();
    antlr4::tree::TerminalNode *SEMI();
    Sensitivity_clauseContext *sensitivity_clause();
    Condition_clauseContext *condition_clause();
    Timeout_clauseContext *timeout_clause();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Wait_statementContext* wait_statement();

  class  Sensitivity_clauseContext : public antlr4::ParserRuleContext {
  public:
    Sensitivity_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ON();
    Sensitivity_listContext *sensitivity_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sensitivity_clauseContext* sensitivity_clause();

  class  Sensitivity_listContext : public antlr4::ParserRuleContext {
  public:
    Sensitivity_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<NameContext *> name();
    NameContext* name(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Sensitivity_listContext* sensitivity_list();

  class  Condition_clauseContext : public antlr4::ParserRuleContext {
  public:
    Condition_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_UNTIL();
    ConditionContext *condition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Condition_clauseContext* condition_clause();

  class  ConditionContext : public antlr4::ParserRuleContext {
  public:
    ConditionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  ConditionContext* condition();

  class  Timeout_clauseContext : public antlr4::ParserRuleContext {
  public:
    Timeout_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FOR();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Timeout_clauseContext* timeout_clause();

  class  Assertion_statementContext : public antlr4::ParserRuleContext {
  public:
    Assertion_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    AssertionContext *assertion();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Assertion_statementContext* assertion_statement();

  class  AssertionContext : public antlr4::ParserRuleContext {
  public:
    AssertionContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ASSERT();
    ConditionContext *condition();
    antlr4::tree::TerminalNode *KW_REPORT();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *KW_SEVERITY();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  AssertionContext* assertion();

  class  Report_statementContext : public antlr4::ParserRuleContext {
  public:
    Report_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_REPORT();
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_SEVERITY();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Report_statementContext* report_statement();

  class  Signal_assignment_statementContext : public antlr4::ParserRuleContext {
  public:
    Signal_assignment_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Simple_signal_assignmentContext *simple_signal_assignment();
    Conditional_signal_assignmentContext *conditional_signal_assignment();
    Selected_signal_assignmentContext *selected_signal_assignment();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Signal_assignment_statementContext* signal_assignment_statement();

  class  Simple_signal_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Simple_signal_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Simple_waveform_assignmentContext *simple_waveform_assignment();
    Simple_force_assignmentContext *simple_force_assignment();
    Simple_release_assignmentContext *simple_release_assignment();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_signal_assignmentContext* simple_signal_assignment();

  class  Simple_waveform_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Simple_waveform_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TargetContext *target();
    antlr4::tree::TerminalNode *CONASGN();
    WaveformContext *waveform();
    antlr4::tree::TerminalNode *SEMI();
    Delay_mechanismContext *delay_mechanism();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_waveform_assignmentContext* simple_waveform_assignment();

  class  Simple_force_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Simple_force_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TargetContext *target();
    antlr4::tree::TerminalNode *CONASGN();
    antlr4::tree::TerminalNode *KW_FORCE();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *SEMI();
    Force_modeContext *force_mode();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_force_assignmentContext* simple_force_assignment();

  class  Simple_release_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Simple_release_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TargetContext *target();
    antlr4::tree::TerminalNode *CONASGN();
    antlr4::tree::TerminalNode *KW_RELEASE();
    antlr4::tree::TerminalNode *SEMI();
    Force_modeContext *force_mode();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_release_assignmentContext* simple_release_assignment();

  class  Force_modeContext : public antlr4::ParserRuleContext {
  public:
    Force_modeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IN();
    antlr4::tree::TerminalNode *KW_OUT();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Force_modeContext* force_mode();

  class  Delay_mechanismContext : public antlr4::ParserRuleContext {
  public:
    Delay_mechanismContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_TRANSPORT();
    antlr4::tree::TerminalNode *KW_INERTIAL();
    antlr4::tree::TerminalNode *KW_REJECT();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Delay_mechanismContext* delay_mechanism();

  class  TargetContext : public antlr4::ParserRuleContext {
  public:
    TargetContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    AggregateContext *aggregate();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  TargetContext* target();

  class  WaveformContext : public antlr4::ParserRuleContext {
  public:
    WaveformContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Waveform_elementContext *> waveform_element();
    Waveform_elementContext* waveform_element(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);
    antlr4::tree::TerminalNode *KW_UNAFFECTED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  WaveformContext* waveform();

  class  Waveform_elementContext : public antlr4::ParserRuleContext {
  public:
    Waveform_elementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    antlr4::tree::TerminalNode *KW_AFTER();
    antlr4::tree::TerminalNode *KW_NULL();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Waveform_elementContext* waveform_element();

  class  Conditional_signal_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Conditional_signal_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Conditional_waveform_assignmentContext *conditional_waveform_assignment();
    Conditional_force_assignmentContext *conditional_force_assignment();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Conditional_signal_assignmentContext* conditional_signal_assignment();

  class  Conditional_waveform_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Conditional_waveform_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TargetContext *target();
    antlr4::tree::TerminalNode *CONASGN();
    Conditional_waveformsContext *conditional_waveforms();
    antlr4::tree::TerminalNode *SEMI();
    Delay_mechanismContext *delay_mechanism();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Conditional_waveform_assignmentContext* conditional_waveform_assignment();

  class  Conditional_waveformsContext : public antlr4::ParserRuleContext {
  public:
    Conditional_waveformsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<WaveformContext *> waveform();
    WaveformContext* waveform(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_WHEN();
    antlr4::tree::TerminalNode* KW_WHEN(size_t i);
    std::vector<ConditionContext *> condition();
    ConditionContext* condition(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_ELSE();
    antlr4::tree::TerminalNode* KW_ELSE(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Conditional_waveformsContext* conditional_waveforms();

  class  Conditional_force_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Conditional_force_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TargetContext *target();
    antlr4::tree::TerminalNode *CONASGN();
    antlr4::tree::TerminalNode *KW_FORCE();
    Conditional_expressionsContext *conditional_expressions();
    antlr4::tree::TerminalNode *SEMI();
    Force_modeContext *force_mode();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Conditional_force_assignmentContext* conditional_force_assignment();

  class  Conditional_expressionsContext : public antlr4::ParserRuleContext {
  public:
    Conditional_expressionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_WHEN();
    antlr4::tree::TerminalNode* KW_WHEN(size_t i);
    std::vector<ConditionContext *> condition();
    ConditionContext* condition(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_ELSE();
    antlr4::tree::TerminalNode* KW_ELSE(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Conditional_expressionsContext* conditional_expressions();

  class  Selected_signal_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Selected_signal_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Selected_waveform_assignmentContext *selected_waveform_assignment();
    Selected_force_assignmentContext *selected_force_assignment();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Selected_signal_assignmentContext* selected_signal_assignment();

  class  Selected_waveform_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Selected_waveform_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_WITH();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_SELECT();
    TargetContext *target();
    antlr4::tree::TerminalNode *CONASGN();
    Selected_waveformsContext *selected_waveforms();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *QUESTIONMARK();
    Delay_mechanismContext *delay_mechanism();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Selected_waveform_assignmentContext* selected_waveform_assignment();

  class  Selected_waveformsContext : public antlr4::ParserRuleContext {
  public:
    Selected_waveformsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<WaveformContext *> waveform();
    WaveformContext* waveform(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_WHEN();
    antlr4::tree::TerminalNode* KW_WHEN(size_t i);
    std::vector<ChoicesContext *> choices();
    ChoicesContext* choices(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Selected_waveformsContext* selected_waveforms();

  class  Selected_force_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Selected_force_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_WITH();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_SELECT();
    TargetContext *target();
    antlr4::tree::TerminalNode *CONASGN();
    antlr4::tree::TerminalNode *KW_FORCE();
    Selected_expressionsContext *selected_expressions();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *QUESTIONMARK();
    Force_modeContext *force_mode();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Selected_force_assignmentContext* selected_force_assignment();

  class  Selected_expressionsContext : public antlr4::ParserRuleContext {
  public:
    Selected_expressionsContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<ExpressionContext *> expression();
    ExpressionContext* expression(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_WHEN();
    antlr4::tree::TerminalNode* KW_WHEN(size_t i);
    std::vector<ChoicesContext *> choices();
    ChoicesContext* choices(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Selected_expressionsContext* selected_expressions();

  class  Variable_assignment_statementContext : public antlr4::ParserRuleContext {
  public:
    Variable_assignment_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Simple_variable_assignmentContext *simple_variable_assignment();
    Conditional_variable_assignmentContext *conditional_variable_assignment();
    Selected_variable_assignmentContext *selected_variable_assignment();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Variable_assignment_statementContext* variable_assignment_statement();

  class  Simple_variable_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Simple_variable_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TargetContext *target();
    antlr4::tree::TerminalNode *VARASGN();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Simple_variable_assignmentContext* simple_variable_assignment();

  class  Conditional_variable_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Conditional_variable_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TargetContext *target();
    antlr4::tree::TerminalNode *VARASGN();
    Conditional_expressionsContext *conditional_expressions();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Conditional_variable_assignmentContext* conditional_variable_assignment();

  class  Selected_variable_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Selected_variable_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_WITH();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_SELECT();
    TargetContext *target();
    antlr4::tree::TerminalNode *VARASGN();
    Selected_expressionsContext *selected_expressions();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *QUESTIONMARK();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Selected_variable_assignmentContext* selected_variable_assignment();

  class  Procedure_call_statementContext : public antlr4::ParserRuleContext {
  public:
    Procedure_call_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Procedure_callContext *procedure_call();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Procedure_call_statementContext* procedure_call_statement();

  class  Procedure_callContext : public antlr4::ParserRuleContext {
  public:
    Procedure_callContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Procedure_callContext* procedure_call();

  class  If_statementContext : public antlr4::ParserRuleContext {
  public:
    If_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_IF();
    antlr4::tree::TerminalNode* KW_IF(size_t i);
    std::vector<ConditionContext *> condition();
    ConditionContext* condition(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_THEN();
    antlr4::tree::TerminalNode* KW_THEN(size_t i);
    std::vector<Sequence_of_statementsContext *> sequence_of_statements();
    Sequence_of_statementsContext* sequence_of_statements(size_t i);
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_ELSE();
    std::vector<antlr4::tree::TerminalNode *> KW_ELSIF();
    antlr4::tree::TerminalNode* KW_ELSIF(size_t i);
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  If_statementContext* if_statement();

  class  Case_statementContext : public antlr4::ParserRuleContext {
  public:
    Case_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_CASE();
    antlr4::tree::TerminalNode* KW_CASE(size_t i);
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_IS();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> QUESTIONMARK();
    antlr4::tree::TerminalNode* QUESTIONMARK(size_t i);
    std::vector<Case_statement_alternativeContext *> case_statement_alternative();
    Case_statement_alternativeContext* case_statement_alternative(size_t i);
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_statementContext* case_statement();

  class  Case_statement_alternativeContext : public antlr4::ParserRuleContext {
  public:
    Case_statement_alternativeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_WHEN();
    ChoicesContext *choices();
    antlr4::tree::TerminalNode *ARROW();
    Sequence_of_statementsContext *sequence_of_statements();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_statement_alternativeContext* case_statement_alternative();

  class  Loop_statementContext : public antlr4::ParserRuleContext {
  public:
    Loop_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_LOOP();
    antlr4::tree::TerminalNode* KW_LOOP(size_t i);
    Sequence_of_statementsContext *sequence_of_statements();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    Iteration_schemeContext *iteration_scheme();
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Loop_statementContext* loop_statement();

  class  Iteration_schemeContext : public antlr4::ParserRuleContext {
  public:
    Iteration_schemeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_WHILE();
    ConditionContext *condition();
    antlr4::tree::TerminalNode *KW_FOR();
    Parameter_specificationContext *parameter_specification();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Iteration_schemeContext* iteration_scheme();

  class  Parameter_specificationContext : public antlr4::ParserRuleContext {
  public:
    Parameter_specificationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *KW_IN();
    Discrete_rangeContext *discrete_range();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Parameter_specificationContext* parameter_specification();

  class  Next_statementContext : public antlr4::ParserRuleContext {
  public:
    Next_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_NEXT();
    antlr4::tree::TerminalNode *SEMI();
    LabelContext *label();
    antlr4::tree::TerminalNode *KW_WHEN();
    ConditionContext *condition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Next_statementContext* next_statement();

  class  Exit_statementContext : public antlr4::ParserRuleContext {
  public:
    Exit_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_EXIT();
    antlr4::tree::TerminalNode *SEMI();
    LabelContext *label();
    antlr4::tree::TerminalNode *KW_WHEN();
    ConditionContext *condition();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Exit_statementContext* exit_statement();

  class  Return_statementContext : public antlr4::ParserRuleContext {
  public:
    Return_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_RETURN();
    antlr4::tree::TerminalNode *SEMI();
    ExpressionContext *expression();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Return_statementContext* return_statement();

  class  Null_statementContext : public antlr4::ParserRuleContext {
  public:
    Null_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_NULL();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Null_statementContext* null_statement();

  class  Concurrent_statement_with_optional_labelContext : public antlr4::ParserRuleContext {
  public:
    Concurrent_statement_with_optional_labelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Process_statementContext *process_statement();
    Concurrent_procedure_call_statementContext *concurrent_procedure_call_statement();
    Concurrent_assertion_statementContext *concurrent_assertion_statement();
    Concurrent_signal_assignment_statementContext *concurrent_signal_assignment_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concurrent_statement_with_optional_labelContext* concurrent_statement_with_optional_label();

  class  Concurrent_statementContext : public antlr4::ParserRuleContext {
  public:
    Concurrent_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    LabelContext *label();
    antlr4::tree::TerminalNode *COLON();
    Block_statementContext *block_statement();
    Component_instantiation_statementContext *component_instantiation_statement();
    Generate_statementContext *generate_statement();
    Concurrent_statement_with_optional_labelContext *concurrent_statement_with_optional_label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concurrent_statementContext* concurrent_statement();

  class  Block_statementContext : public antlr4::ParserRuleContext {
  public:
    Block_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_BLOCK();
    antlr4::tree::TerminalNode* KW_BLOCK(size_t i);
    Block_headerContext *block_header();
    antlr4::tree::TerminalNode *KW_BEGIN();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *LPAREN();
    ConditionContext *condition();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_IS();
    std::vector<Block_declarative_itemContext *> block_declarative_item();
    Block_declarative_itemContext* block_declarative_item(size_t i);
    std::vector<Concurrent_statementContext *> concurrent_statement();
    Concurrent_statementContext* concurrent_statement(size_t i);
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Block_statementContext* block_statement();

  class  Block_headerContext : public antlr4::ParserRuleContext {
  public:
    Block_headerContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Generic_clauseContext *generic_clause();
    Port_clauseContext *port_clause();
    Generic_map_aspectContext *generic_map_aspect();
    std::vector<antlr4::tree::TerminalNode *> SEMI();
    antlr4::tree::TerminalNode* SEMI(size_t i);
    Port_map_aspectContext *port_map_aspect();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Block_headerContext* block_header();

  class  Process_statementContext : public antlr4::ParserRuleContext {
  public:
    Process_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_PROCESS();
    antlr4::tree::TerminalNode* KW_PROCESS(size_t i);
    antlr4::tree::TerminalNode *KW_BEGIN();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> KW_POSTPONED();
    antlr4::tree::TerminalNode* KW_POSTPONED(size_t i);
    antlr4::tree::TerminalNode *LPAREN();
    Process_sensitivity_listContext *process_sensitivity_list();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_IS();
    std::vector<Process_declarative_itemContext *> process_declarative_item();
    Process_declarative_itemContext* process_declarative_item(size_t i);
    std::vector<Sequential_statementContext *> sequential_statement();
    Sequential_statementContext* sequential_statement(size_t i);
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Process_statementContext* process_statement();

  class  Process_sensitivity_listContext : public antlr4::ParserRuleContext {
  public:
    Process_sensitivity_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_ALL();
    Sensitivity_listContext *sensitivity_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Process_sensitivity_listContext* process_sensitivity_list();

  class  Process_or_package_declarative_itemContext : public antlr4::ParserRuleContext {
  public:
    Process_or_package_declarative_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Subprogram_declarationContext *subprogram_declaration();
    Subprogram_instantiation_declarationContext *subprogram_instantiation_declaration();
    Package_declarationContext *package_declaration();
    Package_instantiation_declarationContext *package_instantiation_declaration();
    Type_declarationContext *type_declaration();
    Subtype_declarationContext *subtype_declaration();
    Constant_declarationContext *constant_declaration();
    Variable_declarationContext *variable_declaration();
    File_declarationContext *file_declaration();
    Alias_declarationContext *alias_declaration();
    Attribute_declarationContext *attribute_declaration();
    Attribute_specificationContext *attribute_specification();
    Use_clauseContext *use_clause();
    Group_template_declarationContext *group_template_declaration();
    Group_declarationContext *group_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Process_or_package_declarative_itemContext* process_or_package_declarative_item();

  class  Process_declarative_itemContext : public antlr4::ParserRuleContext {
  public:
    Process_declarative_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Process_or_package_declarative_itemContext *process_or_package_declarative_item();
    Subprogram_bodyContext *subprogram_body();
    Package_bodyContext *package_body();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Process_declarative_itemContext* process_declarative_item();

  class  Concurrent_procedure_call_statementContext : public antlr4::ParserRuleContext {
  public:
    Concurrent_procedure_call_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Procedure_call_statementContext *procedure_call_statement();
    antlr4::tree::TerminalNode *KW_POSTPONED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concurrent_procedure_call_statementContext* concurrent_procedure_call_statement();

  class  Concurrent_assertion_statementContext : public antlr4::ParserRuleContext {
  public:
    Concurrent_assertion_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Assertion_statementContext *assertion_statement();
    antlr4::tree::TerminalNode *KW_POSTPONED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concurrent_assertion_statementContext* concurrent_assertion_statement();

  class  Concurrent_signal_assignment_statementContext : public antlr4::ParserRuleContext {
  public:
    Concurrent_signal_assignment_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Concurrent_signal_assignment_anyContext *concurrent_signal_assignment_any();
    Concurrent_selected_signal_assignmentContext *concurrent_selected_signal_assignment();
    antlr4::tree::TerminalNode *KW_POSTPONED();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concurrent_signal_assignment_statementContext* concurrent_signal_assignment_statement();

  class  Concurrent_signal_assignment_anyContext : public antlr4::ParserRuleContext {
  public:
    Concurrent_signal_assignment_anyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    TargetContext *target();
    antlr4::tree::TerminalNode *CONASGN();
    antlr4::tree::TerminalNode *SEMI();
    WaveformContext *waveform();
    Conditional_waveformsContext *conditional_waveforms();
    antlr4::tree::TerminalNode *KW_GUARDED();
    Delay_mechanismContext *delay_mechanism();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concurrent_signal_assignment_anyContext* concurrent_signal_assignment_any();

  class  Concurrent_selected_signal_assignmentContext : public antlr4::ParserRuleContext {
  public:
    Concurrent_selected_signal_assignmentContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_WITH();
    ExpressionContext *expression();
    antlr4::tree::TerminalNode *KW_SELECT();
    TargetContext *target();
    antlr4::tree::TerminalNode *CONASGN();
    Selected_waveformsContext *selected_waveforms();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *QUESTIONMARK();
    antlr4::tree::TerminalNode *KW_GUARDED();
    Delay_mechanismContext *delay_mechanism();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Concurrent_selected_signal_assignmentContext* concurrent_selected_signal_assignment();

  class  Component_instantiation_statementContext : public antlr4::ParserRuleContext {
  public:
    Component_instantiation_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Instantiated_unitContext *instantiated_unit();
    antlr4::tree::TerminalNode *SEMI();
    Generic_map_aspectContext *generic_map_aspect();
    Port_map_aspectContext *port_map_aspect();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Component_instantiation_statementContext* component_instantiation_statement();

  class  Instantiated_unitContext : public antlr4::ParserRuleContext {
  public:
    Instantiated_unitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    NameContext *name();
    antlr4::tree::TerminalNode *KW_COMPONENT();
    antlr4::tree::TerminalNode *KW_ENTITY();
    antlr4::tree::TerminalNode *LPAREN();
    IdentifierContext *identifier();
    antlr4::tree::TerminalNode *RPAREN();
    antlr4::tree::TerminalNode *KW_CONFIGURATION();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Instantiated_unitContext* instantiated_unit();

  class  Generate_statementContext : public antlr4::ParserRuleContext {
  public:
    Generate_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    For_generate_statementContext *for_generate_statement();
    If_generate_statementContext *if_generate_statement();
    Case_generate_statementContext *case_generate_statement();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generate_statementContext* generate_statement();

  class  For_generate_statementContext : public antlr4::ParserRuleContext {
  public:
    For_generate_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_FOR();
    Parameter_specificationContext *parameter_specification();
    std::vector<antlr4::tree::TerminalNode *> KW_GENERATE();
    antlr4::tree::TerminalNode* KW_GENERATE(size_t i);
    Generate_statement_bodyContext *generate_statement_body();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  For_generate_statementContext* for_generate_statement();

  class  If_generate_statementContext : public antlr4::ParserRuleContext {
  public:
    If_generate_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_IF();
    std::vector<ConditionContext *> condition();
    ConditionContext* condition(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_GENERATE();
    antlr4::tree::TerminalNode* KW_GENERATE(size_t i);
    std::vector<Generate_statement_bodyContext *> generate_statement_body();
    Generate_statement_bodyContext* generate_statement_body(size_t i);
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    antlr4::tree::TerminalNode *KW_ELSE();
    std::vector<LabelContext *> label();
    LabelContext* label(size_t i);
    std::vector<antlr4::tree::TerminalNode *> COLON();
    antlr4::tree::TerminalNode* COLON(size_t i);
    std::vector<antlr4::tree::TerminalNode *> KW_ELSIF();
    antlr4::tree::TerminalNode* KW_ELSIF(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  If_generate_statementContext* if_generate_statement();

  class  Case_generate_statementContext : public antlr4::ParserRuleContext {
  public:
    Case_generate_statementContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CASE();
    ExpressionContext *expression();
    std::vector<antlr4::tree::TerminalNode *> KW_GENERATE();
    antlr4::tree::TerminalNode* KW_GENERATE(size_t i);
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Case_generate_alternativeContext *> case_generate_alternative();
    Case_generate_alternativeContext* case_generate_alternative(size_t i);
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_generate_statementContext* case_generate_statement();

  class  Case_generate_alternativeContext : public antlr4::ParserRuleContext {
  public:
    Case_generate_alternativeContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_WHEN();
    ChoicesContext *choices();
    antlr4::tree::TerminalNode *ARROW();
    Generate_statement_body_with_begin_endContext *generate_statement_body_with_begin_end();
    LabelContext *label();
    antlr4::tree::TerminalNode *COLON();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Case_generate_alternativeContext* case_generate_alternative();

  class  Generate_statement_body_with_begin_endContext : public antlr4::ParserRuleContext {
  public:
    Generate_statement_body_with_begin_endContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BEGIN();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();
    std::vector<Block_declarative_itemContext *> block_declarative_item();
    Block_declarative_itemContext* block_declarative_item(size_t i);
    std::vector<Concurrent_statementContext *> concurrent_statement();
    Concurrent_statementContext* concurrent_statement(size_t i);
    LabelContext *label();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generate_statement_body_with_begin_endContext* generate_statement_body_with_begin_end();

  class  Generate_statement_bodyContext : public antlr4::ParserRuleContext {
  public:
    Generate_statement_bodyContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_BEGIN();
    std::vector<Block_declarative_itemContext *> block_declarative_item();
    Block_declarative_itemContext* block_declarative_item(size_t i);
    std::vector<Concurrent_statementContext *> concurrent_statement();
    Concurrent_statementContext* concurrent_statement(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Generate_statement_bodyContext* generate_statement_body();

  class  LabelContext : public antlr4::ParserRuleContext {
  public:
    LabelContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    IdentifierContext *identifier();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  LabelContext* label();

  class  Use_clauseContext : public antlr4::ParserRuleContext {
  public:
    Use_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_USE();
    std::vector<Selected_nameContext *> selected_name();
    Selected_nameContext* selected_name(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Use_clauseContext* use_clause();

  class  Design_fileContext : public antlr4::ParserRuleContext {
  public:
    Design_fileContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *EOF();
    std::vector<Design_unitContext *> design_unit();
    Design_unitContext* design_unit(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Design_fileContext* design_file();

  class  Design_unitContext : public antlr4::ParserRuleContext {
  public:
    Design_unitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Context_clauseContext *context_clause();
    Library_unitContext *library_unit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Design_unitContext* design_unit();

  class  Library_unitContext : public antlr4::ParserRuleContext {
  public:
    Library_unitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Primary_unitContext *primary_unit();
    Secondary_unitContext *secondary_unit();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Library_unitContext* library_unit();

  class  Primary_unitContext : public antlr4::ParserRuleContext {
  public:
    Primary_unitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Entity_declarationContext *entity_declaration();
    Configuration_declarationContext *configuration_declaration();
    Package_declarationContext *package_declaration();
    Package_instantiation_declarationContext *package_instantiation_declaration();
    Context_declarationContext *context_declaration();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Primary_unitContext* primary_unit();

  class  Secondary_unitContext : public antlr4::ParserRuleContext {
  public:
    Secondary_unitContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Architecture_bodyContext *architecture_body();
    Package_bodyContext *package_body();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Secondary_unitContext* secondary_unit();

  class  Library_clauseContext : public antlr4::ParserRuleContext {
  public:
    Library_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_LIBRARY();
    Logical_name_listContext *logical_name_list();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Library_clauseContext* library_clause();

  class  Logical_name_listContext : public antlr4::ParserRuleContext {
  public:
    Logical_name_listContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Identifier_listContext *identifier_list();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Logical_name_listContext* logical_name_list();

  class  Context_declarationContext : public antlr4::ParserRuleContext {
  public:
    Context_declarationContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<antlr4::tree::TerminalNode *> KW_CONTEXT();
    antlr4::tree::TerminalNode* KW_CONTEXT(size_t i);
    std::vector<IdentifierContext *> identifier();
    IdentifierContext* identifier(size_t i);
    antlr4::tree::TerminalNode *KW_IS();
    Context_clauseContext *context_clause();
    antlr4::tree::TerminalNode *KW_END();
    antlr4::tree::TerminalNode *SEMI();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Context_declarationContext* context_declaration();

  class  Context_clauseContext : public antlr4::ParserRuleContext {
  public:
    Context_clauseContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    std::vector<Context_itemContext *> context_item();
    Context_itemContext* context_item(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Context_clauseContext* context_clause();

  class  Context_itemContext : public antlr4::ParserRuleContext {
  public:
    Context_itemContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    Library_clauseContext *library_clause();
    Use_clauseContext *use_clause();
    Context_referenceContext *context_reference();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Context_itemContext* context_item();

  class  Context_referenceContext : public antlr4::ParserRuleContext {
  public:
    Context_referenceContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *KW_CONTEXT();
    std::vector<Selected_nameContext *> selected_name();
    Selected_nameContext* selected_name(size_t i);
    antlr4::tree::TerminalNode *SEMI();
    std::vector<antlr4::tree::TerminalNode *> COMMA();
    antlr4::tree::TerminalNode* COMMA(size_t i);

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  Context_referenceContext* context_reference();

  class  IdentifierContext : public antlr4::ParserRuleContext {
  public:
    IdentifierContext(antlr4::ParserRuleContext *parent, size_t invokingState);
    virtual size_t getRuleIndex() const override;
    antlr4::tree::TerminalNode *BASIC_IDENTIFIER();
    antlr4::tree::TerminalNode *EXTENDED_IDENTIFIER();

    virtual void enterRule(antlr4::tree::ParseTreeListener *listener) override;
    virtual void exitRule(antlr4::tree::ParseTreeListener *listener) override;

    virtual std::any accept(antlr4::tree::ParseTreeVisitor *visitor) override;
   
  };

  IdentifierContext* identifier();


  bool sempred(antlr4::RuleContext *_localctx, size_t ruleIndex, size_t predicateIndex) override;

  bool nameSempred(NameContext *_localctx, size_t predicateIndex);
  bool expressionSempred(ExpressionContext *_localctx, size_t predicateIndex);
  bool simple_expressionSempred(Simple_expressionContext *_localctx, size_t predicateIndex);
  bool conditional_waveformsSempred(Conditional_waveformsContext *_localctx, size_t predicateIndex);
  bool conditional_expressionsSempred(Conditional_expressionsContext *_localctx, size_t predicateIndex);
  bool if_statementSempred(If_statementContext *_localctx, size_t predicateIndex);
  bool if_generate_statementSempred(If_generate_statementContext *_localctx, size_t predicateIndex);

  // By default the static state used to implement the parser is lazily initialized during the first
  // call to the constructor. You can call this function if you wish to initialize the static state
  // ahead of time.
  static void initialize();

private:
};

}  // namespace mgp_vh
