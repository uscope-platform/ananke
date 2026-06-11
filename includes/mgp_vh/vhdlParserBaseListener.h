
// Generated from /Users/filss/git/makefile_gen/grammars/vhdlParser.g4 by ANTLR 4.13.1

#pragma once


#include "antlr4-runtime.h"
#include "vhdlParserListener.h"


namespace mgp_vh {

/**
 * This class provides an empty implementation of vhdlParserListener,
 * which can be extended to create a listener which only needs to handle a subset
 * of the available methods.
 */
class  vhdlParserBaseListener : public vhdlParserListener {
public:

  virtual void enterAny_keyword(vhdlParser::Any_keywordContext * /*ctx*/) override { }
  virtual void exitAny_keyword(vhdlParser::Any_keywordContext * /*ctx*/) override { }

  virtual void enterName_literal(vhdlParser::Name_literalContext * /*ctx*/) override { }
  virtual void exitName_literal(vhdlParser::Name_literalContext * /*ctx*/) override { }

  virtual void enterName(vhdlParser::NameContext * /*ctx*/) override { }
  virtual void exitName(vhdlParser::NameContext * /*ctx*/) override { }

  virtual void enterName_slice_part(vhdlParser::Name_slice_partContext * /*ctx*/) override { }
  virtual void exitName_slice_part(vhdlParser::Name_slice_partContext * /*ctx*/) override { }

  virtual void enterName_attribute_part(vhdlParser::Name_attribute_partContext * /*ctx*/) override { }
  virtual void exitName_attribute_part(vhdlParser::Name_attribute_partContext * /*ctx*/) override { }

  virtual void enterAttribute_name(vhdlParser::Attribute_nameContext * /*ctx*/) override { }
  virtual void exitAttribute_name(vhdlParser::Attribute_nameContext * /*ctx*/) override { }

  virtual void enterSuffix(vhdlParser::SuffixContext * /*ctx*/) override { }
  virtual void exitSuffix(vhdlParser::SuffixContext * /*ctx*/) override { }

  virtual void enterExplicit_range(vhdlParser::Explicit_rangeContext * /*ctx*/) override { }
  virtual void exitExplicit_range(vhdlParser::Explicit_rangeContext * /*ctx*/) override { }

  virtual void enterSelected_name(vhdlParser::Selected_nameContext * /*ctx*/) override { }
  virtual void exitSelected_name(vhdlParser::Selected_nameContext * /*ctx*/) override { }

  virtual void enterEntity_declaration(vhdlParser::Entity_declarationContext * /*ctx*/) override { }
  virtual void exitEntity_declaration(vhdlParser::Entity_declarationContext * /*ctx*/) override { }

  virtual void enterEntity_declarative_item(vhdlParser::Entity_declarative_itemContext * /*ctx*/) override { }
  virtual void exitEntity_declarative_item(vhdlParser::Entity_declarative_itemContext * /*ctx*/) override { }

  virtual void enterEntity_statement(vhdlParser::Entity_statementContext * /*ctx*/) override { }
  virtual void exitEntity_statement(vhdlParser::Entity_statementContext * /*ctx*/) override { }

  virtual void enterArchitecture_body(vhdlParser::Architecture_bodyContext * /*ctx*/) override { }
  virtual void exitArchitecture_body(vhdlParser::Architecture_bodyContext * /*ctx*/) override { }

  virtual void enterBlock_declarative_item(vhdlParser::Block_declarative_itemContext * /*ctx*/) override { }
  virtual void exitBlock_declarative_item(vhdlParser::Block_declarative_itemContext * /*ctx*/) override { }

  virtual void enterConfiguration_declaration(vhdlParser::Configuration_declarationContext * /*ctx*/) override { }
  virtual void exitConfiguration_declaration(vhdlParser::Configuration_declarationContext * /*ctx*/) override { }

  virtual void enterConfiguration_declarative_item(vhdlParser::Configuration_declarative_itemContext * /*ctx*/) override { }
  virtual void exitConfiguration_declarative_item(vhdlParser::Configuration_declarative_itemContext * /*ctx*/) override { }

  virtual void enterBlock_configuration(vhdlParser::Block_configurationContext * /*ctx*/) override { }
  virtual void exitBlock_configuration(vhdlParser::Block_configurationContext * /*ctx*/) override { }

  virtual void enterBlock_specification(vhdlParser::Block_specificationContext * /*ctx*/) override { }
  virtual void exitBlock_specification(vhdlParser::Block_specificationContext * /*ctx*/) override { }

  virtual void enterGenerate_specification(vhdlParser::Generate_specificationContext * /*ctx*/) override { }
  virtual void exitGenerate_specification(vhdlParser::Generate_specificationContext * /*ctx*/) override { }

  virtual void enterConfiguration_item(vhdlParser::Configuration_itemContext * /*ctx*/) override { }
  virtual void exitConfiguration_item(vhdlParser::Configuration_itemContext * /*ctx*/) override { }

  virtual void enterComponent_configuration(vhdlParser::Component_configurationContext * /*ctx*/) override { }
  virtual void exitComponent_configuration(vhdlParser::Component_configurationContext * /*ctx*/) override { }

  virtual void enterSubprogram_declaration(vhdlParser::Subprogram_declarationContext * /*ctx*/) override { }
  virtual void exitSubprogram_declaration(vhdlParser::Subprogram_declarationContext * /*ctx*/) override { }

  virtual void enterSubprogram_specification(vhdlParser::Subprogram_specificationContext * /*ctx*/) override { }
  virtual void exitSubprogram_specification(vhdlParser::Subprogram_specificationContext * /*ctx*/) override { }

  virtual void enterProcedure_specification(vhdlParser::Procedure_specificationContext * /*ctx*/) override { }
  virtual void exitProcedure_specification(vhdlParser::Procedure_specificationContext * /*ctx*/) override { }

  virtual void enterFunction_specification(vhdlParser::Function_specificationContext * /*ctx*/) override { }
  virtual void exitFunction_specification(vhdlParser::Function_specificationContext * /*ctx*/) override { }

  virtual void enterSubprogram_header(vhdlParser::Subprogram_headerContext * /*ctx*/) override { }
  virtual void exitSubprogram_header(vhdlParser::Subprogram_headerContext * /*ctx*/) override { }

  virtual void enterDesignator(vhdlParser::DesignatorContext * /*ctx*/) override { }
  virtual void exitDesignator(vhdlParser::DesignatorContext * /*ctx*/) override { }

  virtual void enterOperator_symbol(vhdlParser::Operator_symbolContext * /*ctx*/) override { }
  virtual void exitOperator_symbol(vhdlParser::Operator_symbolContext * /*ctx*/) override { }

  virtual void enterFormal_parameter_list(vhdlParser::Formal_parameter_listContext * /*ctx*/) override { }
  virtual void exitFormal_parameter_list(vhdlParser::Formal_parameter_listContext * /*ctx*/) override { }

  virtual void enterSubprogram_body(vhdlParser::Subprogram_bodyContext * /*ctx*/) override { }
  virtual void exitSubprogram_body(vhdlParser::Subprogram_bodyContext * /*ctx*/) override { }

  virtual void enterSubprogram_kind(vhdlParser::Subprogram_kindContext * /*ctx*/) override { }
  virtual void exitSubprogram_kind(vhdlParser::Subprogram_kindContext * /*ctx*/) override { }

  virtual void enterSubprogram_instantiation_declaration(vhdlParser::Subprogram_instantiation_declarationContext * /*ctx*/) override { }
  virtual void exitSubprogram_instantiation_declaration(vhdlParser::Subprogram_instantiation_declarationContext * /*ctx*/) override { }

  virtual void enterSignature(vhdlParser::SignatureContext * /*ctx*/) override { }
  virtual void exitSignature(vhdlParser::SignatureContext * /*ctx*/) override { }

  virtual void enterPackage_declaration(vhdlParser::Package_declarationContext * /*ctx*/) override { }
  virtual void exitPackage_declaration(vhdlParser::Package_declarationContext * /*ctx*/) override { }

  virtual void enterPackage_declarative_item(vhdlParser::Package_declarative_itemContext * /*ctx*/) override { }
  virtual void exitPackage_declarative_item(vhdlParser::Package_declarative_itemContext * /*ctx*/) override { }

  virtual void enterPackage_body(vhdlParser::Package_bodyContext * /*ctx*/) override { }
  virtual void exitPackage_body(vhdlParser::Package_bodyContext * /*ctx*/) override { }

  virtual void enterPackage_instantiation_declaration(vhdlParser::Package_instantiation_declarationContext * /*ctx*/) override { }
  virtual void exitPackage_instantiation_declaration(vhdlParser::Package_instantiation_declarationContext * /*ctx*/) override { }

  virtual void enterScalar_type_definition(vhdlParser::Scalar_type_definitionContext * /*ctx*/) override { }
  virtual void exitScalar_type_definition(vhdlParser::Scalar_type_definitionContext * /*ctx*/) override { }

  virtual void enterRange_constraint(vhdlParser::Range_constraintContext * /*ctx*/) override { }
  virtual void exitRange_constraint(vhdlParser::Range_constraintContext * /*ctx*/) override { }

  virtual void enterRange(vhdlParser::RangeContext * /*ctx*/) override { }
  virtual void exitRange(vhdlParser::RangeContext * /*ctx*/) override { }

  virtual void enterDirection(vhdlParser::DirectionContext * /*ctx*/) override { }
  virtual void exitDirection(vhdlParser::DirectionContext * /*ctx*/) override { }

  virtual void enterEnumeration_type_definition(vhdlParser::Enumeration_type_definitionContext * /*ctx*/) override { }
  virtual void exitEnumeration_type_definition(vhdlParser::Enumeration_type_definitionContext * /*ctx*/) override { }

  virtual void enterEnumeration_literal(vhdlParser::Enumeration_literalContext * /*ctx*/) override { }
  virtual void exitEnumeration_literal(vhdlParser::Enumeration_literalContext * /*ctx*/) override { }

  virtual void enterInteger_type_definition(vhdlParser::Integer_type_definitionContext * /*ctx*/) override { }
  virtual void exitInteger_type_definition(vhdlParser::Integer_type_definitionContext * /*ctx*/) override { }

  virtual void enterPhysical_type_definition(vhdlParser::Physical_type_definitionContext * /*ctx*/) override { }
  virtual void exitPhysical_type_definition(vhdlParser::Physical_type_definitionContext * /*ctx*/) override { }

  virtual void enterPrimary_unit_declaration(vhdlParser::Primary_unit_declarationContext * /*ctx*/) override { }
  virtual void exitPrimary_unit_declaration(vhdlParser::Primary_unit_declarationContext * /*ctx*/) override { }

  virtual void enterSecondary_unit_declaration(vhdlParser::Secondary_unit_declarationContext * /*ctx*/) override { }
  virtual void exitSecondary_unit_declaration(vhdlParser::Secondary_unit_declarationContext * /*ctx*/) override { }

  virtual void enterFloating_type_definition(vhdlParser::Floating_type_definitionContext * /*ctx*/) override { }
  virtual void exitFloating_type_definition(vhdlParser::Floating_type_definitionContext * /*ctx*/) override { }

  virtual void enterComposite_type_definition(vhdlParser::Composite_type_definitionContext * /*ctx*/) override { }
  virtual void exitComposite_type_definition(vhdlParser::Composite_type_definitionContext * /*ctx*/) override { }

  virtual void enterArray_type_definition(vhdlParser::Array_type_definitionContext * /*ctx*/) override { }
  virtual void exitArray_type_definition(vhdlParser::Array_type_definitionContext * /*ctx*/) override { }

  virtual void enterUnbounded_array_definition(vhdlParser::Unbounded_array_definitionContext * /*ctx*/) override { }
  virtual void exitUnbounded_array_definition(vhdlParser::Unbounded_array_definitionContext * /*ctx*/) override { }

  virtual void enterConstrained_array_definition(vhdlParser::Constrained_array_definitionContext * /*ctx*/) override { }
  virtual void exitConstrained_array_definition(vhdlParser::Constrained_array_definitionContext * /*ctx*/) override { }

  virtual void enterIndex_subtype_definition(vhdlParser::Index_subtype_definitionContext * /*ctx*/) override { }
  virtual void exitIndex_subtype_definition(vhdlParser::Index_subtype_definitionContext * /*ctx*/) override { }

  virtual void enterArray_constraint(vhdlParser::Array_constraintContext * /*ctx*/) override { }
  virtual void exitArray_constraint(vhdlParser::Array_constraintContext * /*ctx*/) override { }

  virtual void enterArray_element_constraint(vhdlParser::Array_element_constraintContext * /*ctx*/) override { }
  virtual void exitArray_element_constraint(vhdlParser::Array_element_constraintContext * /*ctx*/) override { }

  virtual void enterIndex_constraint(vhdlParser::Index_constraintContext * /*ctx*/) override { }
  virtual void exitIndex_constraint(vhdlParser::Index_constraintContext * /*ctx*/) override { }

  virtual void enterDiscrete_range(vhdlParser::Discrete_rangeContext * /*ctx*/) override { }
  virtual void exitDiscrete_range(vhdlParser::Discrete_rangeContext * /*ctx*/) override { }

  virtual void enterRecord_type_definition(vhdlParser::Record_type_definitionContext * /*ctx*/) override { }
  virtual void exitRecord_type_definition(vhdlParser::Record_type_definitionContext * /*ctx*/) override { }

  virtual void enterElement_declaration(vhdlParser::Element_declarationContext * /*ctx*/) override { }
  virtual void exitElement_declaration(vhdlParser::Element_declarationContext * /*ctx*/) override { }

  virtual void enterIdentifier_list(vhdlParser::Identifier_listContext * /*ctx*/) override { }
  virtual void exitIdentifier_list(vhdlParser::Identifier_listContext * /*ctx*/) override { }

  virtual void enterElement_subtype_definition(vhdlParser::Element_subtype_definitionContext * /*ctx*/) override { }
  virtual void exitElement_subtype_definition(vhdlParser::Element_subtype_definitionContext * /*ctx*/) override { }

  virtual void enterRecord_constraint(vhdlParser::Record_constraintContext * /*ctx*/) override { }
  virtual void exitRecord_constraint(vhdlParser::Record_constraintContext * /*ctx*/) override { }

  virtual void enterRecord_element_constraint(vhdlParser::Record_element_constraintContext * /*ctx*/) override { }
  virtual void exitRecord_element_constraint(vhdlParser::Record_element_constraintContext * /*ctx*/) override { }

  virtual void enterAccess_type_definition(vhdlParser::Access_type_definitionContext * /*ctx*/) override { }
  virtual void exitAccess_type_definition(vhdlParser::Access_type_definitionContext * /*ctx*/) override { }

  virtual void enterIncomplete_type_declaration(vhdlParser::Incomplete_type_declarationContext * /*ctx*/) override { }
  virtual void exitIncomplete_type_declaration(vhdlParser::Incomplete_type_declarationContext * /*ctx*/) override { }

  virtual void enterFile_type_definition(vhdlParser::File_type_definitionContext * /*ctx*/) override { }
  virtual void exitFile_type_definition(vhdlParser::File_type_definitionContext * /*ctx*/) override { }

  virtual void enterProtected_type_definition(vhdlParser::Protected_type_definitionContext * /*ctx*/) override { }
  virtual void exitProtected_type_definition(vhdlParser::Protected_type_definitionContext * /*ctx*/) override { }

  virtual void enterProtected_type_declaration(vhdlParser::Protected_type_declarationContext * /*ctx*/) override { }
  virtual void exitProtected_type_declaration(vhdlParser::Protected_type_declarationContext * /*ctx*/) override { }

  virtual void enterProtected_type_declarative_item(vhdlParser::Protected_type_declarative_itemContext * /*ctx*/) override { }
  virtual void exitProtected_type_declarative_item(vhdlParser::Protected_type_declarative_itemContext * /*ctx*/) override { }

  virtual void enterProtected_type_body(vhdlParser::Protected_type_bodyContext * /*ctx*/) override { }
  virtual void exitProtected_type_body(vhdlParser::Protected_type_bodyContext * /*ctx*/) override { }

  virtual void enterType_declaration(vhdlParser::Type_declarationContext * /*ctx*/) override { }
  virtual void exitType_declaration(vhdlParser::Type_declarationContext * /*ctx*/) override { }

  virtual void enterFull_type_declaration(vhdlParser::Full_type_declarationContext * /*ctx*/) override { }
  virtual void exitFull_type_declaration(vhdlParser::Full_type_declarationContext * /*ctx*/) override { }

  virtual void enterType_definition(vhdlParser::Type_definitionContext * /*ctx*/) override { }
  virtual void exitType_definition(vhdlParser::Type_definitionContext * /*ctx*/) override { }

  virtual void enterSubtype_declaration(vhdlParser::Subtype_declarationContext * /*ctx*/) override { }
  virtual void exitSubtype_declaration(vhdlParser::Subtype_declarationContext * /*ctx*/) override { }

  virtual void enterSubtype_indication(vhdlParser::Subtype_indicationContext * /*ctx*/) override { }
  virtual void exitSubtype_indication(vhdlParser::Subtype_indicationContext * /*ctx*/) override { }

  virtual void enterResolution_indication(vhdlParser::Resolution_indicationContext * /*ctx*/) override { }
  virtual void exitResolution_indication(vhdlParser::Resolution_indicationContext * /*ctx*/) override { }

  virtual void enterElement_resolution(vhdlParser::Element_resolutionContext * /*ctx*/) override { }
  virtual void exitElement_resolution(vhdlParser::Element_resolutionContext * /*ctx*/) override { }

  virtual void enterArray_element_resolution(vhdlParser::Array_element_resolutionContext * /*ctx*/) override { }
  virtual void exitArray_element_resolution(vhdlParser::Array_element_resolutionContext * /*ctx*/) override { }

  virtual void enterRecord_resolution(vhdlParser::Record_resolutionContext * /*ctx*/) override { }
  virtual void exitRecord_resolution(vhdlParser::Record_resolutionContext * /*ctx*/) override { }

  virtual void enterRecord_element_resolution(vhdlParser::Record_element_resolutionContext * /*ctx*/) override { }
  virtual void exitRecord_element_resolution(vhdlParser::Record_element_resolutionContext * /*ctx*/) override { }

  virtual void enterType_mark(vhdlParser::Type_markContext * /*ctx*/) override { }
  virtual void exitType_mark(vhdlParser::Type_markContext * /*ctx*/) override { }

  virtual void enterConstraint(vhdlParser::ConstraintContext * /*ctx*/) override { }
  virtual void exitConstraint(vhdlParser::ConstraintContext * /*ctx*/) override { }

  virtual void enterElement_constraint(vhdlParser::Element_constraintContext * /*ctx*/) override { }
  virtual void exitElement_constraint(vhdlParser::Element_constraintContext * /*ctx*/) override { }

  virtual void enterObject_declaration(vhdlParser::Object_declarationContext * /*ctx*/) override { }
  virtual void exitObject_declaration(vhdlParser::Object_declarationContext * /*ctx*/) override { }

  virtual void enterConstant_declaration(vhdlParser::Constant_declarationContext * /*ctx*/) override { }
  virtual void exitConstant_declaration(vhdlParser::Constant_declarationContext * /*ctx*/) override { }

  virtual void enterSignal_declaration(vhdlParser::Signal_declarationContext * /*ctx*/) override { }
  virtual void exitSignal_declaration(vhdlParser::Signal_declarationContext * /*ctx*/) override { }

  virtual void enterSignal_kind(vhdlParser::Signal_kindContext * /*ctx*/) override { }
  virtual void exitSignal_kind(vhdlParser::Signal_kindContext * /*ctx*/) override { }

  virtual void enterVariable_declaration(vhdlParser::Variable_declarationContext * /*ctx*/) override { }
  virtual void exitVariable_declaration(vhdlParser::Variable_declarationContext * /*ctx*/) override { }

  virtual void enterFile_declaration(vhdlParser::File_declarationContext * /*ctx*/) override { }
  virtual void exitFile_declaration(vhdlParser::File_declarationContext * /*ctx*/) override { }

  virtual void enterFile_open_information(vhdlParser::File_open_informationContext * /*ctx*/) override { }
  virtual void exitFile_open_information(vhdlParser::File_open_informationContext * /*ctx*/) override { }

  virtual void enterFile_logical_name(vhdlParser::File_logical_nameContext * /*ctx*/) override { }
  virtual void exitFile_logical_name(vhdlParser::File_logical_nameContext * /*ctx*/) override { }

  virtual void enterInterface_declaration(vhdlParser::Interface_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_declaration(vhdlParser::Interface_declarationContext * /*ctx*/) override { }

  virtual void enterInterface_object_declaration(vhdlParser::Interface_object_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_object_declaration(vhdlParser::Interface_object_declarationContext * /*ctx*/) override { }

  virtual void enterInterface_constant_declaration(vhdlParser::Interface_constant_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_constant_declaration(vhdlParser::Interface_constant_declarationContext * /*ctx*/) override { }

  virtual void enterInterface_signal_declaration(vhdlParser::Interface_signal_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_signal_declaration(vhdlParser::Interface_signal_declarationContext * /*ctx*/) override { }

  virtual void enterInterface_variable_declaration(vhdlParser::Interface_variable_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_variable_declaration(vhdlParser::Interface_variable_declarationContext * /*ctx*/) override { }

  virtual void enterInterface_file_declaration(vhdlParser::Interface_file_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_file_declaration(vhdlParser::Interface_file_declarationContext * /*ctx*/) override { }

  virtual void enterSignal_mode(vhdlParser::Signal_modeContext * /*ctx*/) override { }
  virtual void exitSignal_mode(vhdlParser::Signal_modeContext * /*ctx*/) override { }

  virtual void enterInterface_type_declaration(vhdlParser::Interface_type_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_type_declaration(vhdlParser::Interface_type_declarationContext * /*ctx*/) override { }

  virtual void enterInterface_incomplete_type_declaration(vhdlParser::Interface_incomplete_type_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_incomplete_type_declaration(vhdlParser::Interface_incomplete_type_declarationContext * /*ctx*/) override { }

  virtual void enterInterface_subprogram_declaration(vhdlParser::Interface_subprogram_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_subprogram_declaration(vhdlParser::Interface_subprogram_declarationContext * /*ctx*/) override { }

  virtual void enterInterface_subprogram_specification(vhdlParser::Interface_subprogram_specificationContext * /*ctx*/) override { }
  virtual void exitInterface_subprogram_specification(vhdlParser::Interface_subprogram_specificationContext * /*ctx*/) override { }

  virtual void enterInterface_procedure_specification(vhdlParser::Interface_procedure_specificationContext * /*ctx*/) override { }
  virtual void exitInterface_procedure_specification(vhdlParser::Interface_procedure_specificationContext * /*ctx*/) override { }

  virtual void enterInterface_function_specification(vhdlParser::Interface_function_specificationContext * /*ctx*/) override { }
  virtual void exitInterface_function_specification(vhdlParser::Interface_function_specificationContext * /*ctx*/) override { }

  virtual void enterInterface_subprogram_default(vhdlParser::Interface_subprogram_defaultContext * /*ctx*/) override { }
  virtual void exitInterface_subprogram_default(vhdlParser::Interface_subprogram_defaultContext * /*ctx*/) override { }

  virtual void enterInterface_package_declaration(vhdlParser::Interface_package_declarationContext * /*ctx*/) override { }
  virtual void exitInterface_package_declaration(vhdlParser::Interface_package_declarationContext * /*ctx*/) override { }

  virtual void enterInterface_package_generic_map_aspect(vhdlParser::Interface_package_generic_map_aspectContext * /*ctx*/) override { }
  virtual void exitInterface_package_generic_map_aspect(vhdlParser::Interface_package_generic_map_aspectContext * /*ctx*/) override { }

  virtual void enterInterface_list(vhdlParser::Interface_listContext * /*ctx*/) override { }
  virtual void exitInterface_list(vhdlParser::Interface_listContext * /*ctx*/) override { }

  virtual void enterInterface_element(vhdlParser::Interface_elementContext * /*ctx*/) override { }
  virtual void exitInterface_element(vhdlParser::Interface_elementContext * /*ctx*/) override { }

  virtual void enterGeneric_clause(vhdlParser::Generic_clauseContext * /*ctx*/) override { }
  virtual void exitGeneric_clause(vhdlParser::Generic_clauseContext * /*ctx*/) override { }

  virtual void enterGeneric_list(vhdlParser::Generic_listContext * /*ctx*/) override { }
  virtual void exitGeneric_list(vhdlParser::Generic_listContext * /*ctx*/) override { }

  virtual void enterPort_clause(vhdlParser::Port_clauseContext * /*ctx*/) override { }
  virtual void exitPort_clause(vhdlParser::Port_clauseContext * /*ctx*/) override { }

  virtual void enterPort_list(vhdlParser::Port_listContext * /*ctx*/) override { }
  virtual void exitPort_list(vhdlParser::Port_listContext * /*ctx*/) override { }

  virtual void enterAssociation_list(vhdlParser::Association_listContext * /*ctx*/) override { }
  virtual void exitAssociation_list(vhdlParser::Association_listContext * /*ctx*/) override { }

  virtual void enterAssociation_element(vhdlParser::Association_elementContext * /*ctx*/) override { }
  virtual void exitAssociation_element(vhdlParser::Association_elementContext * /*ctx*/) override { }

  virtual void enterFormal_part(vhdlParser::Formal_partContext * /*ctx*/) override { }
  virtual void exitFormal_part(vhdlParser::Formal_partContext * /*ctx*/) override { }

  virtual void enterActual_part(vhdlParser::Actual_partContext * /*ctx*/) override { }
  virtual void exitActual_part(vhdlParser::Actual_partContext * /*ctx*/) override { }

  virtual void enterActual_designator(vhdlParser::Actual_designatorContext * /*ctx*/) override { }
  virtual void exitActual_designator(vhdlParser::Actual_designatorContext * /*ctx*/) override { }

  virtual void enterGeneric_map_aspect(vhdlParser::Generic_map_aspectContext * /*ctx*/) override { }
  virtual void exitGeneric_map_aspect(vhdlParser::Generic_map_aspectContext * /*ctx*/) override { }

  virtual void enterPort_map_aspect(vhdlParser::Port_map_aspectContext * /*ctx*/) override { }
  virtual void exitPort_map_aspect(vhdlParser::Port_map_aspectContext * /*ctx*/) override { }

  virtual void enterAlias_declaration(vhdlParser::Alias_declarationContext * /*ctx*/) override { }
  virtual void exitAlias_declaration(vhdlParser::Alias_declarationContext * /*ctx*/) override { }

  virtual void enterAlias_designator(vhdlParser::Alias_designatorContext * /*ctx*/) override { }
  virtual void exitAlias_designator(vhdlParser::Alias_designatorContext * /*ctx*/) override { }

  virtual void enterAttribute_declaration(vhdlParser::Attribute_declarationContext * /*ctx*/) override { }
  virtual void exitAttribute_declaration(vhdlParser::Attribute_declarationContext * /*ctx*/) override { }

  virtual void enterComponent_declaration(vhdlParser::Component_declarationContext * /*ctx*/) override { }
  virtual void exitComponent_declaration(vhdlParser::Component_declarationContext * /*ctx*/) override { }

  virtual void enterGroup_template_declaration(vhdlParser::Group_template_declarationContext * /*ctx*/) override { }
  virtual void exitGroup_template_declaration(vhdlParser::Group_template_declarationContext * /*ctx*/) override { }

  virtual void enterEntity_class_entry_list(vhdlParser::Entity_class_entry_listContext * /*ctx*/) override { }
  virtual void exitEntity_class_entry_list(vhdlParser::Entity_class_entry_listContext * /*ctx*/) override { }

  virtual void enterEntity_class_entry(vhdlParser::Entity_class_entryContext * /*ctx*/) override { }
  virtual void exitEntity_class_entry(vhdlParser::Entity_class_entryContext * /*ctx*/) override { }

  virtual void enterGroup_declaration(vhdlParser::Group_declarationContext * /*ctx*/) override { }
  virtual void exitGroup_declaration(vhdlParser::Group_declarationContext * /*ctx*/) override { }

  virtual void enterGroup_constituent_list(vhdlParser::Group_constituent_listContext * /*ctx*/) override { }
  virtual void exitGroup_constituent_list(vhdlParser::Group_constituent_listContext * /*ctx*/) override { }

  virtual void enterGroup_constituent(vhdlParser::Group_constituentContext * /*ctx*/) override { }
  virtual void exitGroup_constituent(vhdlParser::Group_constituentContext * /*ctx*/) override { }

  virtual void enterAttribute_specification(vhdlParser::Attribute_specificationContext * /*ctx*/) override { }
  virtual void exitAttribute_specification(vhdlParser::Attribute_specificationContext * /*ctx*/) override { }

  virtual void enterEntity_specification(vhdlParser::Entity_specificationContext * /*ctx*/) override { }
  virtual void exitEntity_specification(vhdlParser::Entity_specificationContext * /*ctx*/) override { }

  virtual void enterEntity_class(vhdlParser::Entity_classContext * /*ctx*/) override { }
  virtual void exitEntity_class(vhdlParser::Entity_classContext * /*ctx*/) override { }

  virtual void enterEntity_name_list(vhdlParser::Entity_name_listContext * /*ctx*/) override { }
  virtual void exitEntity_name_list(vhdlParser::Entity_name_listContext * /*ctx*/) override { }

  virtual void enterEntity_designator(vhdlParser::Entity_designatorContext * /*ctx*/) override { }
  virtual void exitEntity_designator(vhdlParser::Entity_designatorContext * /*ctx*/) override { }

  virtual void enterEntity_tag(vhdlParser::Entity_tagContext * /*ctx*/) override { }
  virtual void exitEntity_tag(vhdlParser::Entity_tagContext * /*ctx*/) override { }

  virtual void enterConfiguration_specification(vhdlParser::Configuration_specificationContext * /*ctx*/) override { }
  virtual void exitConfiguration_specification(vhdlParser::Configuration_specificationContext * /*ctx*/) override { }

  virtual void enterSimple_configuration_specification(vhdlParser::Simple_configuration_specificationContext * /*ctx*/) override { }
  virtual void exitSimple_configuration_specification(vhdlParser::Simple_configuration_specificationContext * /*ctx*/) override { }

  virtual void enterCompound_configuration_specification(vhdlParser::Compound_configuration_specificationContext * /*ctx*/) override { }
  virtual void exitCompound_configuration_specification(vhdlParser::Compound_configuration_specificationContext * /*ctx*/) override { }

  virtual void enterComponent_specification(vhdlParser::Component_specificationContext * /*ctx*/) override { }
  virtual void exitComponent_specification(vhdlParser::Component_specificationContext * /*ctx*/) override { }

  virtual void enterInstantiation_list(vhdlParser::Instantiation_listContext * /*ctx*/) override { }
  virtual void exitInstantiation_list(vhdlParser::Instantiation_listContext * /*ctx*/) override { }

  virtual void enterBinding_indication(vhdlParser::Binding_indicationContext * /*ctx*/) override { }
  virtual void exitBinding_indication(vhdlParser::Binding_indicationContext * /*ctx*/) override { }

  virtual void enterEntity_aspect(vhdlParser::Entity_aspectContext * /*ctx*/) override { }
  virtual void exitEntity_aspect(vhdlParser::Entity_aspectContext * /*ctx*/) override { }

  virtual void enterVerification_unit_binding_indication(vhdlParser::Verification_unit_binding_indicationContext * /*ctx*/) override { }
  virtual void exitVerification_unit_binding_indication(vhdlParser::Verification_unit_binding_indicationContext * /*ctx*/) override { }

  virtual void enterVerification_unit_list(vhdlParser::Verification_unit_listContext * /*ctx*/) override { }
  virtual void exitVerification_unit_list(vhdlParser::Verification_unit_listContext * /*ctx*/) override { }

  virtual void enterDisconnection_specification(vhdlParser::Disconnection_specificationContext * /*ctx*/) override { }
  virtual void exitDisconnection_specification(vhdlParser::Disconnection_specificationContext * /*ctx*/) override { }

  virtual void enterGuarded_signal_specification(vhdlParser::Guarded_signal_specificationContext * /*ctx*/) override { }
  virtual void exitGuarded_signal_specification(vhdlParser::Guarded_signal_specificationContext * /*ctx*/) override { }

  virtual void enterSignal_list(vhdlParser::Signal_listContext * /*ctx*/) override { }
  virtual void exitSignal_list(vhdlParser::Signal_listContext * /*ctx*/) override { }

  virtual void enterAttribute_designator(vhdlParser::Attribute_designatorContext * /*ctx*/) override { }
  virtual void exitAttribute_designator(vhdlParser::Attribute_designatorContext * /*ctx*/) override { }

  virtual void enterExternal_name(vhdlParser::External_nameContext * /*ctx*/) override { }
  virtual void exitExternal_name(vhdlParser::External_nameContext * /*ctx*/) override { }

  virtual void enterExternal_pathname(vhdlParser::External_pathnameContext * /*ctx*/) override { }
  virtual void exitExternal_pathname(vhdlParser::External_pathnameContext * /*ctx*/) override { }

  virtual void enterPackage_pathname(vhdlParser::Package_pathnameContext * /*ctx*/) override { }
  virtual void exitPackage_pathname(vhdlParser::Package_pathnameContext * /*ctx*/) override { }

  virtual void enterAbsolute_pathname(vhdlParser::Absolute_pathnameContext * /*ctx*/) override { }
  virtual void exitAbsolute_pathname(vhdlParser::Absolute_pathnameContext * /*ctx*/) override { }

  virtual void enterRelative_pathname(vhdlParser::Relative_pathnameContext * /*ctx*/) override { }
  virtual void exitRelative_pathname(vhdlParser::Relative_pathnameContext * /*ctx*/) override { }

  virtual void enterPartial_pathname(vhdlParser::Partial_pathnameContext * /*ctx*/) override { }
  virtual void exitPartial_pathname(vhdlParser::Partial_pathnameContext * /*ctx*/) override { }

  virtual void enterPathname_element(vhdlParser::Pathname_elementContext * /*ctx*/) override { }
  virtual void exitPathname_element(vhdlParser::Pathname_elementContext * /*ctx*/) override { }

  virtual void enterExpression(vhdlParser::ExpressionContext * /*ctx*/) override { }
  virtual void exitExpression(vhdlParser::ExpressionContext * /*ctx*/) override { }

  virtual void enterSimple_expression(vhdlParser::Simple_expressionContext * /*ctx*/) override { }
  virtual void exitSimple_expression(vhdlParser::Simple_expressionContext * /*ctx*/) override { }

  virtual void enterPrimary(vhdlParser::PrimaryContext * /*ctx*/) override { }
  virtual void exitPrimary(vhdlParser::PrimaryContext * /*ctx*/) override { }

  virtual void enterLogical_operator(vhdlParser::Logical_operatorContext * /*ctx*/) override { }
  virtual void exitLogical_operator(vhdlParser::Logical_operatorContext * /*ctx*/) override { }

  virtual void enterRelational_operator(vhdlParser::Relational_operatorContext * /*ctx*/) override { }
  virtual void exitRelational_operator(vhdlParser::Relational_operatorContext * /*ctx*/) override { }

  virtual void enterShift_operator(vhdlParser::Shift_operatorContext * /*ctx*/) override { }
  virtual void exitShift_operator(vhdlParser::Shift_operatorContext * /*ctx*/) override { }

  virtual void enterAdding_operator(vhdlParser::Adding_operatorContext * /*ctx*/) override { }
  virtual void exitAdding_operator(vhdlParser::Adding_operatorContext * /*ctx*/) override { }

  virtual void enterSign(vhdlParser::SignContext * /*ctx*/) override { }
  virtual void exitSign(vhdlParser::SignContext * /*ctx*/) override { }

  virtual void enterMultiplying_operator(vhdlParser::Multiplying_operatorContext * /*ctx*/) override { }
  virtual void exitMultiplying_operator(vhdlParser::Multiplying_operatorContext * /*ctx*/) override { }

  virtual void enterMiscellaneous_operator(vhdlParser::Miscellaneous_operatorContext * /*ctx*/) override { }
  virtual void exitMiscellaneous_operator(vhdlParser::Miscellaneous_operatorContext * /*ctx*/) override { }

  virtual void enterNumeric_literal(vhdlParser::Numeric_literalContext * /*ctx*/) override { }
  virtual void exitNumeric_literal(vhdlParser::Numeric_literalContext * /*ctx*/) override { }

  virtual void enterPhysical_literal(vhdlParser::Physical_literalContext * /*ctx*/) override { }
  virtual void exitPhysical_literal(vhdlParser::Physical_literalContext * /*ctx*/) override { }

  virtual void enterAggregate(vhdlParser::AggregateContext * /*ctx*/) override { }
  virtual void exitAggregate(vhdlParser::AggregateContext * /*ctx*/) override { }

  virtual void enterElement_association(vhdlParser::Element_associationContext * /*ctx*/) override { }
  virtual void exitElement_association(vhdlParser::Element_associationContext * /*ctx*/) override { }

  virtual void enterChoices(vhdlParser::ChoicesContext * /*ctx*/) override { }
  virtual void exitChoices(vhdlParser::ChoicesContext * /*ctx*/) override { }

  virtual void enterChoice(vhdlParser::ChoiceContext * /*ctx*/) override { }
  virtual void exitChoice(vhdlParser::ChoiceContext * /*ctx*/) override { }

  virtual void enterQualified_expression(vhdlParser::Qualified_expressionContext * /*ctx*/) override { }
  virtual void exitQualified_expression(vhdlParser::Qualified_expressionContext * /*ctx*/) override { }

  virtual void enterAllocator(vhdlParser::AllocatorContext * /*ctx*/) override { }
  virtual void exitAllocator(vhdlParser::AllocatorContext * /*ctx*/) override { }

  virtual void enterSequence_of_statements(vhdlParser::Sequence_of_statementsContext * /*ctx*/) override { }
  virtual void exitSequence_of_statements(vhdlParser::Sequence_of_statementsContext * /*ctx*/) override { }

  virtual void enterSequential_statement(vhdlParser::Sequential_statementContext * /*ctx*/) override { }
  virtual void exitSequential_statement(vhdlParser::Sequential_statementContext * /*ctx*/) override { }

  virtual void enterWait_statement(vhdlParser::Wait_statementContext * /*ctx*/) override { }
  virtual void exitWait_statement(vhdlParser::Wait_statementContext * /*ctx*/) override { }

  virtual void enterSensitivity_clause(vhdlParser::Sensitivity_clauseContext * /*ctx*/) override { }
  virtual void exitSensitivity_clause(vhdlParser::Sensitivity_clauseContext * /*ctx*/) override { }

  virtual void enterSensitivity_list(vhdlParser::Sensitivity_listContext * /*ctx*/) override { }
  virtual void exitSensitivity_list(vhdlParser::Sensitivity_listContext * /*ctx*/) override { }

  virtual void enterCondition_clause(vhdlParser::Condition_clauseContext * /*ctx*/) override { }
  virtual void exitCondition_clause(vhdlParser::Condition_clauseContext * /*ctx*/) override { }

  virtual void enterCondition(vhdlParser::ConditionContext * /*ctx*/) override { }
  virtual void exitCondition(vhdlParser::ConditionContext * /*ctx*/) override { }

  virtual void enterTimeout_clause(vhdlParser::Timeout_clauseContext * /*ctx*/) override { }
  virtual void exitTimeout_clause(vhdlParser::Timeout_clauseContext * /*ctx*/) override { }

  virtual void enterAssertion_statement(vhdlParser::Assertion_statementContext * /*ctx*/) override { }
  virtual void exitAssertion_statement(vhdlParser::Assertion_statementContext * /*ctx*/) override { }

  virtual void enterAssertion(vhdlParser::AssertionContext * /*ctx*/) override { }
  virtual void exitAssertion(vhdlParser::AssertionContext * /*ctx*/) override { }

  virtual void enterReport_statement(vhdlParser::Report_statementContext * /*ctx*/) override { }
  virtual void exitReport_statement(vhdlParser::Report_statementContext * /*ctx*/) override { }

  virtual void enterSignal_assignment_statement(vhdlParser::Signal_assignment_statementContext * /*ctx*/) override { }
  virtual void exitSignal_assignment_statement(vhdlParser::Signal_assignment_statementContext * /*ctx*/) override { }

  virtual void enterSimple_signal_assignment(vhdlParser::Simple_signal_assignmentContext * /*ctx*/) override { }
  virtual void exitSimple_signal_assignment(vhdlParser::Simple_signal_assignmentContext * /*ctx*/) override { }

  virtual void enterSimple_waveform_assignment(vhdlParser::Simple_waveform_assignmentContext * /*ctx*/) override { }
  virtual void exitSimple_waveform_assignment(vhdlParser::Simple_waveform_assignmentContext * /*ctx*/) override { }

  virtual void enterSimple_force_assignment(vhdlParser::Simple_force_assignmentContext * /*ctx*/) override { }
  virtual void exitSimple_force_assignment(vhdlParser::Simple_force_assignmentContext * /*ctx*/) override { }

  virtual void enterSimple_release_assignment(vhdlParser::Simple_release_assignmentContext * /*ctx*/) override { }
  virtual void exitSimple_release_assignment(vhdlParser::Simple_release_assignmentContext * /*ctx*/) override { }

  virtual void enterForce_mode(vhdlParser::Force_modeContext * /*ctx*/) override { }
  virtual void exitForce_mode(vhdlParser::Force_modeContext * /*ctx*/) override { }

  virtual void enterDelay_mechanism(vhdlParser::Delay_mechanismContext * /*ctx*/) override { }
  virtual void exitDelay_mechanism(vhdlParser::Delay_mechanismContext * /*ctx*/) override { }

  virtual void enterTarget(vhdlParser::TargetContext * /*ctx*/) override { }
  virtual void exitTarget(vhdlParser::TargetContext * /*ctx*/) override { }

  virtual void enterWaveform(vhdlParser::WaveformContext * /*ctx*/) override { }
  virtual void exitWaveform(vhdlParser::WaveformContext * /*ctx*/) override { }

  virtual void enterWaveform_element(vhdlParser::Waveform_elementContext * /*ctx*/) override { }
  virtual void exitWaveform_element(vhdlParser::Waveform_elementContext * /*ctx*/) override { }

  virtual void enterConditional_signal_assignment(vhdlParser::Conditional_signal_assignmentContext * /*ctx*/) override { }
  virtual void exitConditional_signal_assignment(vhdlParser::Conditional_signal_assignmentContext * /*ctx*/) override { }

  virtual void enterConditional_waveform_assignment(vhdlParser::Conditional_waveform_assignmentContext * /*ctx*/) override { }
  virtual void exitConditional_waveform_assignment(vhdlParser::Conditional_waveform_assignmentContext * /*ctx*/) override { }

  virtual void enterConditional_waveforms(vhdlParser::Conditional_waveformsContext * /*ctx*/) override { }
  virtual void exitConditional_waveforms(vhdlParser::Conditional_waveformsContext * /*ctx*/) override { }

  virtual void enterConditional_force_assignment(vhdlParser::Conditional_force_assignmentContext * /*ctx*/) override { }
  virtual void exitConditional_force_assignment(vhdlParser::Conditional_force_assignmentContext * /*ctx*/) override { }

  virtual void enterConditional_expressions(vhdlParser::Conditional_expressionsContext * /*ctx*/) override { }
  virtual void exitConditional_expressions(vhdlParser::Conditional_expressionsContext * /*ctx*/) override { }

  virtual void enterSelected_signal_assignment(vhdlParser::Selected_signal_assignmentContext * /*ctx*/) override { }
  virtual void exitSelected_signal_assignment(vhdlParser::Selected_signal_assignmentContext * /*ctx*/) override { }

  virtual void enterSelected_waveform_assignment(vhdlParser::Selected_waveform_assignmentContext * /*ctx*/) override { }
  virtual void exitSelected_waveform_assignment(vhdlParser::Selected_waveform_assignmentContext * /*ctx*/) override { }

  virtual void enterSelected_waveforms(vhdlParser::Selected_waveformsContext * /*ctx*/) override { }
  virtual void exitSelected_waveforms(vhdlParser::Selected_waveformsContext * /*ctx*/) override { }

  virtual void enterSelected_force_assignment(vhdlParser::Selected_force_assignmentContext * /*ctx*/) override { }
  virtual void exitSelected_force_assignment(vhdlParser::Selected_force_assignmentContext * /*ctx*/) override { }

  virtual void enterSelected_expressions(vhdlParser::Selected_expressionsContext * /*ctx*/) override { }
  virtual void exitSelected_expressions(vhdlParser::Selected_expressionsContext * /*ctx*/) override { }

  virtual void enterVariable_assignment_statement(vhdlParser::Variable_assignment_statementContext * /*ctx*/) override { }
  virtual void exitVariable_assignment_statement(vhdlParser::Variable_assignment_statementContext * /*ctx*/) override { }

  virtual void enterSimple_variable_assignment(vhdlParser::Simple_variable_assignmentContext * /*ctx*/) override { }
  virtual void exitSimple_variable_assignment(vhdlParser::Simple_variable_assignmentContext * /*ctx*/) override { }

  virtual void enterConditional_variable_assignment(vhdlParser::Conditional_variable_assignmentContext * /*ctx*/) override { }
  virtual void exitConditional_variable_assignment(vhdlParser::Conditional_variable_assignmentContext * /*ctx*/) override { }

  virtual void enterSelected_variable_assignment(vhdlParser::Selected_variable_assignmentContext * /*ctx*/) override { }
  virtual void exitSelected_variable_assignment(vhdlParser::Selected_variable_assignmentContext * /*ctx*/) override { }

  virtual void enterProcedure_call_statement(vhdlParser::Procedure_call_statementContext * /*ctx*/) override { }
  virtual void exitProcedure_call_statement(vhdlParser::Procedure_call_statementContext * /*ctx*/) override { }

  virtual void enterProcedure_call(vhdlParser::Procedure_callContext * /*ctx*/) override { }
  virtual void exitProcedure_call(vhdlParser::Procedure_callContext * /*ctx*/) override { }

  virtual void enterIf_statement(vhdlParser::If_statementContext * /*ctx*/) override { }
  virtual void exitIf_statement(vhdlParser::If_statementContext * /*ctx*/) override { }

  virtual void enterCase_statement(vhdlParser::Case_statementContext * /*ctx*/) override { }
  virtual void exitCase_statement(vhdlParser::Case_statementContext * /*ctx*/) override { }

  virtual void enterCase_statement_alternative(vhdlParser::Case_statement_alternativeContext * /*ctx*/) override { }
  virtual void exitCase_statement_alternative(vhdlParser::Case_statement_alternativeContext * /*ctx*/) override { }

  virtual void enterLoop_statement(vhdlParser::Loop_statementContext * /*ctx*/) override { }
  virtual void exitLoop_statement(vhdlParser::Loop_statementContext * /*ctx*/) override { }

  virtual void enterIteration_scheme(vhdlParser::Iteration_schemeContext * /*ctx*/) override { }
  virtual void exitIteration_scheme(vhdlParser::Iteration_schemeContext * /*ctx*/) override { }

  virtual void enterParameter_specification(vhdlParser::Parameter_specificationContext * /*ctx*/) override { }
  virtual void exitParameter_specification(vhdlParser::Parameter_specificationContext * /*ctx*/) override { }

  virtual void enterNext_statement(vhdlParser::Next_statementContext * /*ctx*/) override { }
  virtual void exitNext_statement(vhdlParser::Next_statementContext * /*ctx*/) override { }

  virtual void enterExit_statement(vhdlParser::Exit_statementContext * /*ctx*/) override { }
  virtual void exitExit_statement(vhdlParser::Exit_statementContext * /*ctx*/) override { }

  virtual void enterReturn_statement(vhdlParser::Return_statementContext * /*ctx*/) override { }
  virtual void exitReturn_statement(vhdlParser::Return_statementContext * /*ctx*/) override { }

  virtual void enterNull_statement(vhdlParser::Null_statementContext * /*ctx*/) override { }
  virtual void exitNull_statement(vhdlParser::Null_statementContext * /*ctx*/) override { }

  virtual void enterConcurrent_statement_with_optional_label(vhdlParser::Concurrent_statement_with_optional_labelContext * /*ctx*/) override { }
  virtual void exitConcurrent_statement_with_optional_label(vhdlParser::Concurrent_statement_with_optional_labelContext * /*ctx*/) override { }

  virtual void enterConcurrent_statement(vhdlParser::Concurrent_statementContext * /*ctx*/) override { }
  virtual void exitConcurrent_statement(vhdlParser::Concurrent_statementContext * /*ctx*/) override { }

  virtual void enterBlock_statement(vhdlParser::Block_statementContext * /*ctx*/) override { }
  virtual void exitBlock_statement(vhdlParser::Block_statementContext * /*ctx*/) override { }

  virtual void enterBlock_header(vhdlParser::Block_headerContext * /*ctx*/) override { }
  virtual void exitBlock_header(vhdlParser::Block_headerContext * /*ctx*/) override { }

  virtual void enterProcess_statement(vhdlParser::Process_statementContext * /*ctx*/) override { }
  virtual void exitProcess_statement(vhdlParser::Process_statementContext * /*ctx*/) override { }

  virtual void enterProcess_sensitivity_list(vhdlParser::Process_sensitivity_listContext * /*ctx*/) override { }
  virtual void exitProcess_sensitivity_list(vhdlParser::Process_sensitivity_listContext * /*ctx*/) override { }

  virtual void enterProcess_or_package_declarative_item(vhdlParser::Process_or_package_declarative_itemContext * /*ctx*/) override { }
  virtual void exitProcess_or_package_declarative_item(vhdlParser::Process_or_package_declarative_itemContext * /*ctx*/) override { }

  virtual void enterProcess_declarative_item(vhdlParser::Process_declarative_itemContext * /*ctx*/) override { }
  virtual void exitProcess_declarative_item(vhdlParser::Process_declarative_itemContext * /*ctx*/) override { }

  virtual void enterConcurrent_procedure_call_statement(vhdlParser::Concurrent_procedure_call_statementContext * /*ctx*/) override { }
  virtual void exitConcurrent_procedure_call_statement(vhdlParser::Concurrent_procedure_call_statementContext * /*ctx*/) override { }

  virtual void enterConcurrent_assertion_statement(vhdlParser::Concurrent_assertion_statementContext * /*ctx*/) override { }
  virtual void exitConcurrent_assertion_statement(vhdlParser::Concurrent_assertion_statementContext * /*ctx*/) override { }

  virtual void enterConcurrent_signal_assignment_statement(vhdlParser::Concurrent_signal_assignment_statementContext * /*ctx*/) override { }
  virtual void exitConcurrent_signal_assignment_statement(vhdlParser::Concurrent_signal_assignment_statementContext * /*ctx*/) override { }

  virtual void enterConcurrent_signal_assignment_any(vhdlParser::Concurrent_signal_assignment_anyContext * /*ctx*/) override { }
  virtual void exitConcurrent_signal_assignment_any(vhdlParser::Concurrent_signal_assignment_anyContext * /*ctx*/) override { }

  virtual void enterConcurrent_selected_signal_assignment(vhdlParser::Concurrent_selected_signal_assignmentContext * /*ctx*/) override { }
  virtual void exitConcurrent_selected_signal_assignment(vhdlParser::Concurrent_selected_signal_assignmentContext * /*ctx*/) override { }

  virtual void enterComponent_instantiation_statement(vhdlParser::Component_instantiation_statementContext * /*ctx*/) override { }
  virtual void exitComponent_instantiation_statement(vhdlParser::Component_instantiation_statementContext * /*ctx*/) override { }

  virtual void enterInstantiated_unit(vhdlParser::Instantiated_unitContext * /*ctx*/) override { }
  virtual void exitInstantiated_unit(vhdlParser::Instantiated_unitContext * /*ctx*/) override { }

  virtual void enterGenerate_statement(vhdlParser::Generate_statementContext * /*ctx*/) override { }
  virtual void exitGenerate_statement(vhdlParser::Generate_statementContext * /*ctx*/) override { }

  virtual void enterFor_generate_statement(vhdlParser::For_generate_statementContext * /*ctx*/) override { }
  virtual void exitFor_generate_statement(vhdlParser::For_generate_statementContext * /*ctx*/) override { }

  virtual void enterIf_generate_statement(vhdlParser::If_generate_statementContext * /*ctx*/) override { }
  virtual void exitIf_generate_statement(vhdlParser::If_generate_statementContext * /*ctx*/) override { }

  virtual void enterCase_generate_statement(vhdlParser::Case_generate_statementContext * /*ctx*/) override { }
  virtual void exitCase_generate_statement(vhdlParser::Case_generate_statementContext * /*ctx*/) override { }

  virtual void enterCase_generate_alternative(vhdlParser::Case_generate_alternativeContext * /*ctx*/) override { }
  virtual void exitCase_generate_alternative(vhdlParser::Case_generate_alternativeContext * /*ctx*/) override { }

  virtual void enterGenerate_statement_body_with_begin_end(vhdlParser::Generate_statement_body_with_begin_endContext * /*ctx*/) override { }
  virtual void exitGenerate_statement_body_with_begin_end(vhdlParser::Generate_statement_body_with_begin_endContext * /*ctx*/) override { }

  virtual void enterGenerate_statement_body(vhdlParser::Generate_statement_bodyContext * /*ctx*/) override { }
  virtual void exitGenerate_statement_body(vhdlParser::Generate_statement_bodyContext * /*ctx*/) override { }

  virtual void enterLabel(vhdlParser::LabelContext * /*ctx*/) override { }
  virtual void exitLabel(vhdlParser::LabelContext * /*ctx*/) override { }

  virtual void enterUse_clause(vhdlParser::Use_clauseContext * /*ctx*/) override { }
  virtual void exitUse_clause(vhdlParser::Use_clauseContext * /*ctx*/) override { }

  virtual void enterDesign_file(vhdlParser::Design_fileContext * /*ctx*/) override { }
  virtual void exitDesign_file(vhdlParser::Design_fileContext * /*ctx*/) override { }

  virtual void enterDesign_unit(vhdlParser::Design_unitContext * /*ctx*/) override { }
  virtual void exitDesign_unit(vhdlParser::Design_unitContext * /*ctx*/) override { }

  virtual void enterLibrary_unit(vhdlParser::Library_unitContext * /*ctx*/) override { }
  virtual void exitLibrary_unit(vhdlParser::Library_unitContext * /*ctx*/) override { }

  virtual void enterPrimary_unit(vhdlParser::Primary_unitContext * /*ctx*/) override { }
  virtual void exitPrimary_unit(vhdlParser::Primary_unitContext * /*ctx*/) override { }

  virtual void enterSecondary_unit(vhdlParser::Secondary_unitContext * /*ctx*/) override { }
  virtual void exitSecondary_unit(vhdlParser::Secondary_unitContext * /*ctx*/) override { }

  virtual void enterLibrary_clause(vhdlParser::Library_clauseContext * /*ctx*/) override { }
  virtual void exitLibrary_clause(vhdlParser::Library_clauseContext * /*ctx*/) override { }

  virtual void enterLogical_name_list(vhdlParser::Logical_name_listContext * /*ctx*/) override { }
  virtual void exitLogical_name_list(vhdlParser::Logical_name_listContext * /*ctx*/) override { }

  virtual void enterContext_declaration(vhdlParser::Context_declarationContext * /*ctx*/) override { }
  virtual void exitContext_declaration(vhdlParser::Context_declarationContext * /*ctx*/) override { }

  virtual void enterContext_clause(vhdlParser::Context_clauseContext * /*ctx*/) override { }
  virtual void exitContext_clause(vhdlParser::Context_clauseContext * /*ctx*/) override { }

  virtual void enterContext_item(vhdlParser::Context_itemContext * /*ctx*/) override { }
  virtual void exitContext_item(vhdlParser::Context_itemContext * /*ctx*/) override { }

  virtual void enterContext_reference(vhdlParser::Context_referenceContext * /*ctx*/) override { }
  virtual void exitContext_reference(vhdlParser::Context_referenceContext * /*ctx*/) override { }

  virtual void enterIdentifier(vhdlParser::IdentifierContext * /*ctx*/) override { }
  virtual void exitIdentifier(vhdlParser::IdentifierContext * /*ctx*/) override { }


  virtual void enterEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void exitEveryRule(antlr4::ParserRuleContext * /*ctx*/) override { }
  virtual void visitTerminal(antlr4::tree::TerminalNode * /*node*/) override { }
  virtual void visitErrorNode(antlr4::tree::ErrorNode * /*node*/) override { }

};

}  // namespace mgp_vh
