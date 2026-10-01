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

#ifndef ANANKE_VHDL_VISITOR_HPP
#define ANANKE_VHDL_VISITOR_HPP

#include <string>
#include <utility>
#include <vector>
#include <stack>
#include <optional>
#include <unordered_map>

#include "data_model/HDL/statement/hdl_instance_statement.hpp"
#include "data_model/HDL/factories/HDL_modules_factory.hpp"
#include "data_model/HDL/factories/HDL_packages_factory.hpp"
#include "data_model/HDL/factories/HDL_instances_factory.hpp"
#include "data_model/HDL/factories/HDL_parameters_factory.hpp"
#include "data_model/HDL/factories/HDL_loops_factory.hpp"
#include "data_model/HDL/factories/HDL_conditionals_factory.hpp"
#include "data_model/HDL/parameters/components/HDL_builtin_function.hpp"
#include "frontend/analysis/vhdl/vhdl_type_engine.hpp"

#include "mgp_vh/vhdlParserBaseListener.h"
#include "mgp_vh/vhdlParser.h"


class vhdl_visitor : public mgp_vh::vhdlParserBaseListener {

public:
    explicit vhdl_visitor(std::string p);
    void enterEntity_declaration(mgp_vh::vhdlParser::Entity_declarationContext *ctx) override;
    void exitEntity_declaration(mgp_vh::vhdlParser::Entity_declarationContext *ctx) override;
    void enterArchitecture_body(mgp_vh::vhdlParser::Architecture_bodyContext *ctx) override;
    void exitArchitecture_body(mgp_vh::vhdlParser::Architecture_bodyContext *ctx) override;
    void exitConcurrent_statement(mgp_vh::vhdlParser::Concurrent_statementContext *ctx) override;

    void enterGeneric_clause(mgp_vh::vhdlParser::Generic_clauseContext *ctx) override;
    void exitGeneric_clause(mgp_vh::vhdlParser::Generic_clauseContext *ctx) override;
    void enterPort_clause(mgp_vh::vhdlParser::Port_clauseContext *ctx) override;
    void exitPort_clause(mgp_vh::vhdlParser::Port_clauseContext *ctx) override;
    void enterInterface_constant_declaration(mgp_vh::vhdlParser::Interface_constant_declarationContext *ctx) override;
    void exitInterface_constant_declaration(mgp_vh::vhdlParser::Interface_constant_declarationContext *ctx) override;
    void enterInterface_signal_declaration(mgp_vh::vhdlParser::Interface_signal_declarationContext *ctx) override;
    void exitInterface_signal_declaration(mgp_vh::vhdlParser::Interface_signal_declarationContext *ctx) override;
    void enterSubtype_indication(mgp_vh::vhdlParser::Subtype_indicationContext *ctx) override;
    void exitSubtype_indication(mgp_vh::vhdlParser::Subtype_indicationContext *ctx) override;

    void enterExpression(mgp_vh::vhdlParser::ExpressionContext *ctx) override;
    void exitExpression(mgp_vh::vhdlParser::ExpressionContext *ctx) override;
    void enterSimple_expression(mgp_vh::vhdlParser::Simple_expressionContext *ctx) override;
    void exitSimple_expression(mgp_vh::vhdlParser::Simple_expressionContext *ctx) override;
    void enterNumeric_literal(mgp_vh::vhdlParser::Numeric_literalContext *ctx) override;
    void exitNumeric_literal(mgp_vh::vhdlParser::Numeric_literalContext *ctx) override;
    void exitPrimary(mgp_vh::vhdlParser::PrimaryContext *ctx) override;
    void enterAggregate(mgp_vh::vhdlParser::AggregateContext *ctx) override;
    void exitAggregate(mgp_vh::vhdlParser::AggregateContext *ctx) override;
    void enterChoices(mgp_vh::vhdlParser::ChoicesContext *ctx) override;
    void exitChoices(mgp_vh::vhdlParser::ChoicesContext *ctx) override;
    void enterActual_part(mgp_vh::vhdlParser::Actual_partContext *ctx) override;
    void exitActual_part(mgp_vh::vhdlParser::Actual_partContext *ctx) override;
    void enterQualified_expression(mgp_vh::vhdlParser::Qualified_expressionContext *ctx) override;
    void exitQualified_expression(mgp_vh::vhdlParser::Qualified_expressionContext *ctx) override;
    void exitType_mark(mgp_vh::vhdlParser::Type_markContext *ctx) override;
    void enterIndex_constraint(mgp_vh::vhdlParser::Index_constraintContext *ctx) override;
    void exitIndex_constraint(mgp_vh::vhdlParser::Index_constraintContext *ctx) override;
    void enterExplicit_range(mgp_vh::vhdlParser::Explicit_rangeContext *ctx) override;
    void exitExplicit_range(mgp_vh::vhdlParser::Explicit_rangeContext *ctx) override;
    void enterType_declaration(mgp_vh::vhdlParser::Type_declarationContext *ctx) override;
    void exitType_declaration(mgp_vh::vhdlParser::Type_declarationContext *ctx) override;
    void enterType_definition(mgp_vh::vhdlParser::Type_definitionContext *ctx) override;
    void enterEnumeration_literal(mgp_vh::vhdlParser::Enumeration_literalContext *ctx) override;
    void enterElement_declaration(mgp_vh::vhdlParser::Element_declarationContext *ctx) override;
    void exitElement_declaration(mgp_vh::vhdlParser::Element_declarationContext *ctx) override;
    void enterSubtype_declaration(mgp_vh::vhdlParser::Subtype_declarationContext *ctx) override;
    void exitSubtype_declaration(mgp_vh::vhdlParser::Subtype_declarationContext *ctx) override;
    void enterPackage_declaration(mgp_vh::vhdlParser::Package_declarationContext *ctx) override;
    void exitPackage_declaration(mgp_vh::vhdlParser::Package_declarationContext *ctx) override;
    void enterConstant_declaration(mgp_vh::vhdlParser::Constant_declarationContext *ctx) override;
    void exitConstant_declaration(mgp_vh::vhdlParser::Constant_declarationContext *ctx) override;
    void enterUse_clause(mgp_vh::vhdlParser::Use_clauseContext *ctx) override;
    void enterConcurrent_statement(mgp_vh::vhdlParser::Concurrent_statementContext *ctx) override;
    void enterGeneric_map_aspect(mgp_vh::vhdlParser::Generic_map_aspectContext *ctx) override;
    void exitGeneric_map_aspect(mgp_vh::vhdlParser::Generic_map_aspectContext *ctx) override;
    void enterPort_map_aspect(mgp_vh::vhdlParser::Port_map_aspectContext *ctx) override;
    void exitPort_map_aspect(mgp_vh::vhdlParser::Port_map_aspectContext *ctx) override;
    void enterAssociation_element(mgp_vh::vhdlParser::Association_elementContext *ctx) override;
    void exitAssociation_element(mgp_vh::vhdlParser::Association_elementContext *ctx) override;
    void enterAssociation_list(mgp_vh::vhdlParser::Association_listContext *ctx) override;
    void exitAssociation_list(mgp_vh::vhdlParser::Association_listContext *ctx) override;
    void enterName_slice_part(mgp_vh::vhdlParser::Name_slice_partContext *ctx) override;
    void exitName_slice_part(mgp_vh::vhdlParser::Name_slice_partContext *ctx) override;
    void enterDirection(mgp_vh::vhdlParser::DirectionContext *ctx) override;
    void enterFor_generate_statement(mgp_vh::vhdlParser::For_generate_statementContext *ctx) override;
    void exitFor_generate_statement(mgp_vh::vhdlParser::For_generate_statementContext *ctx) override;
    void enterParameter_specification(mgp_vh::vhdlParser::Parameter_specificationContext *ctx) override;
    void exitParameter_specification(mgp_vh::vhdlParser::Parameter_specificationContext *ctx) override;
    void enterIf_generate_statement(mgp_vh::vhdlParser::If_generate_statementContext *ctx) override;
    void exitIf_generate_statement(mgp_vh::vhdlParser::If_generate_statementContext *ctx) override;
    void enterCondition(mgp_vh::vhdlParser::ConditionContext *ctx) override;
    void exitCondition(mgp_vh::vhdlParser::ConditionContext *ctx) override;
    void enterGenerate_statement_body(mgp_vh::vhdlParser::Generate_statement_bodyContext *ctx) override;
    void exitGenerate_statement_body(mgp_vh::vhdlParser::Generate_statement_bodyContext *ctx) override;
    void enterCase_generate_statement(mgp_vh::vhdlParser::Case_generate_statementContext *ctx) override;
    void exitCase_generate_statement(mgp_vh::vhdlParser::Case_generate_statementContext *ctx) override;
    void enterCase_generate_alternative(mgp_vh::vhdlParser::Case_generate_alternativeContext *ctx) override;
    void exitCase_generate_alternative(mgp_vh::vhdlParser::Case_generate_alternativeContext *ctx) override;
    void enterChoice(mgp_vh::vhdlParser::ChoiceContext *ctx) override;
    void exitChoice(mgp_vh::vhdlParser::ChoiceContext *ctx) override;
    void enterGenerate_statement_body_with_begin_end(mgp_vh::vhdlParser::Generate_statement_body_with_begin_endContext *ctx) override;
    void exitGenerate_statement_body_with_begin_end(mgp_vh::vhdlParser::Generate_statement_body_with_begin_endContext *ctx) override;

    std::vector<std::shared_ptr<hdl_statement_base>> get_entities() {
        resolve_positional_maps();
        attach_default_architectures();
        return entities;
    }
    std::vector<std::shared_ptr<hdl_import_stmt>> get_imports() {return file_imports;}
private:
    void start_generic(mgp_vh::vhdlParser::Identifier_listContext *ids,
                       mgp_vh::vhdlParser::Subtype_indicationContext *type);
    void finalize_generic(mgp_vh::vhdlParser::Identifier_listContext *ids);
    void finalize_port(mgp_vh::vhdlParser::Interface_signal_declarationContext *ctx);
    static std::string instantiated_module_name(mgp_vh::vhdlParser::Component_instantiation_statementContext *ctx);
    static std::string instantiated_module_arch(mgp_vh::vhdlParser::Component_instantiation_statementContext *ctx);
    void route_port_connection(const std::string &text);
    void route_port_slice(const std::string &base, const std::string &first,
                          const std::string &dir, const std::string &second);
    void route_port_index(const std::string &base, const std::vector<std::string> &idx);
    void route_port_actual(mgp_vh::vhdlParser::Numeric_literalContext *ctx);
    void clear_pending_selector();
    void resolve_positional_maps();
    void attach_default_architectures();
    void resolve_positional_in_statement(const std::shared_ptr<hdl_statement_base> &stmt);
    void resolve_positional_instance(const std::shared_ptr<hdl_instance_statement> &inst);
    std::shared_ptr<Expression_base> build_case_condition(
        const std::shared_ptr<Expression_base> &selector,
        const std::vector<std::shared_ptr<Expression_base>> &choices);
    std::shared_ptr<hdl_type> make_generic_type(mgp_vh::vhdlParser::Subtype_indicationContext *type);
    std::shared_ptr<Expression_base> make_vhdl_value(const std::string &text);
    std::shared_ptr<Expression_base> make_character_value(const std::string &text);
    std::shared_ptr<Expression_base> make_numeric_value(mgp_vh::vhdlParser::Numeric_literalContext *ctx);
    bool is_in_generic_expression() const;
    static bool is_aggregate(mgp_vh::vhdlParser::AggregateContext *ctx);
    static bool simple_is_aggregate(mgp_vh::vhdlParser::Simple_expressionContext *ctx);
    static bool has_expr_operator(mgp_vh::vhdlParser::ExpressionContext *ctx);
    static bool simple_is_nonleaf(mgp_vh::vhdlParser::Simple_expressionContext *ctx);
    static std::optional<Expression_v2::expression_operator> simple_expression_op(
        mgp_vh::vhdlParser::Simple_expressionContext *ctx);
    static bool is_function_call(mgp_vh::vhdlParser::NameContext *nm);
    static std::string extract_call_name(mgp_vh::vhdlParser::NameContext *nm);
    static bool is_vhdl_builtin(const std::string &name);

    std::string path;
    std::vector<std::shared_ptr<hdl_statement_base>>  entities;
    std::unordered_map<std::string, std::vector<std::shared_ptr<hdl_statement_base>>> statement_map;
    std::map<std::string, std::string> last_arch_of;
    std::string current_architecture;
    bool in_generic_clause = false;
    bool in_port_clause = false;
    bool in_subtype_indication = false;
    bool in_aggregate_choices = false;
    bool in_type_declaration = false;
    bool in_subtype_declaration = false;
    bool in_record_element = false;
    bool in_instance_generic_map = false;
    bool instance_override_active = false;
    bool in_instance_port_map = false;
    bool in_name_selector = false;
    bool pending_slice = false;
    bool pending_index = false;
    bool in_generate_loop = false;
    bool in_loop_range = false;
    bool in_generate_condition = false;
    bool in_generate_case = false;
    bool in_case_selector = false;
    bool in_case_choices = false;
    bool in_case_else_alternative = false;
    bool in_case_range_choice = false;
    bool case_choice_is_range = false;
    bool in_entity_declaration = false;
    bool in_package_declaration = false;
    bool in_constant_declaration = false;
    std::vector<std::shared_ptr<hdl_import_stmt>> file_imports;
    int positional_gen_index = 0;
    int positional_port_index = 0;
    std::string positional_port_placeholder;
    std::string current_entity_name;
    std::unordered_map<std::string, std::vector<std::string>> entity_port_order;
    std::unordered_map<std::string, std::vector<std::string>> entity_generic_order;
    int case_alternatives_seen = 0;
    std::shared_ptr<Expression_base> case_selector_expr;
    std::vector<std::shared_ptr<Expression_base>> case_choice_exprs;
    std::vector<std::string> generate_stack;
    std::string generate_loop_var;
    std::string generate_loop_dir;
    std::optional<std::string> pending_slice_first;
    std::optional<std::string> pending_slice_second;
    std::string pending_slice_dir;
    std::string pending_index_base;
    std::vector<std::string> pending_index_parts;
    vhdl_type_kind decl_context = vhdl_type_kind::none;
    std::string declared_type_name;
    std::string subtype_decl_name;
    std::shared_ptr<hdl_type> pending_resolved_type;

    HDL_modules_factory modules_factory;
    HDL_packages_factory packages_factory;
    void route_resource_statement(const std::shared_ptr<hdl_statement_base> &s);
    void route_resource_typedef(const std::string &name, const std::shared_ptr<hdl_type> &type);
    HDL_parameters_factory params_factory;
    HDL_instances_factory deps_factory;
    HDL_loops_factory loops_factory;
    HDL_conditionals_factory conditionals_factory;
    vhdl_type_engine type_engine;
};


#endif //ANANKE_VHDL_VISITOR_HPP
