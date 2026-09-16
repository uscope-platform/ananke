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
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include "frontend/analysis/vhdl/vhdl_visitor.hpp"

#include <algorithm>
#include <cctype>
#include <charconv>
#include <string>
#include <map>

#include <spdlog/spdlog.h>

#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/parameters/components/token/Real_token.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/components/token/String_token.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"

namespace {

    bool is_extended_identifier(const std::string &s) {
        return s.size() >= 2 && s.front() == '\\' && s.back() == '\\';
    }

    // Canonical form of a VHDL identifier: extended identifiers keep their
    // case and lose their backslash delimiters, all others are lower-cased.
    std::string canon(const std::string &s) {
        if (is_extended_identifier(s)) return s.substr(1, s.size() - 2);
        std::string ret = s;
        std::transform(ret.begin(), ret.end(), ret.begin(),
                       [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
        return ret;
    }

    // Leftmost identifier of a (possibly selected) name: for `pkg.CONST` the
    // recursive `name` rule nests the base, so walk down to the innermost
    // `name_literal`.
    std::string selected_name_base(mgp_vh::vhdlParser::NameContext *nm) {
        while (nm) {
            if (nm->name_literal() && nm->name_literal()->identifier())
                return nm->name_literal()->identifier()->getText();
            if (!nm->name()) break;
            nm = nm->name();
        }
        return "";
    }

}

vhdl_visitor::vhdl_visitor(std::string p) {
    path = std::move(p);
    // Stamp declarations as VHDL. Builtin classification intentionally keeps
    // the legacy union table: VHDL relies on both tables (e.g. `clog2` only
    // exists in the SV table, `minimum` only in the VHDL one).
    modules_factory.set_language(hdl_language::vhdl);
    params_factory.set_language(hdl_language::vhdl);
}


void vhdl_visitor::enterEntity_declaration(mgp_vh::vhdlParser::Entity_declarationContext *ctx) {
    std::string module_name = canon(ctx->identifier()[0]->getText());
    size_t line_number = ctx->getStart()->getLine();
    modules_factory.new_module(module_name,module, line_number);
    current_entity_name = module_name;
    in_entity_declaration = true;
}

void vhdl_visitor::exitArchitecture_body(mgp_vh::vhdlParser::Architecture_bodyContext *ctx) {
    std::string entity_name = canon(ctx->name()->getText());
    std::string arch_name = canon(ctx->identifier(0)->getText());

    auto arch_res = modules_factory.get_module();
    arch_res->set_architecture(arch_name);
    for (auto &stmt : statement_map[entity_name + "::" + arch_name])
        arch_res->add_statement(stmt);

    // Copy the entity's interface into the architecture resource (self-contained).
    for (auto &item : entities) {
        if (item->is<hdl_resource_statement>() && item->as<hdl_resource_statement>().getName() == entity_name) {
            auto &entity = item->as<hdl_resource_statement>();
            arch_res->set_ports(entity.get_port_specs());
            arch_res->set_parameters(entity.get_parameters());
            break;
        }
    }
    last_arch_of[entity_name] = arch_name;
    entities.push_back(arch_res);
}


void vhdl_visitor::exitEntity_declaration(mgp_vh::vhdlParser::Entity_declarationContext *ctx) {
    entities.push_back(modules_factory.get_module());
    in_entity_declaration = false;
}

void vhdl_visitor::enterConcurrent_statement(mgp_vh::vhdlParser::Concurrent_statementContext *ctx) {
    // Start the instance dependency before its generic/port maps are walked, so
    // the maps can attach to a live dependency (mirrors sv_visitor).
    if (ctx->component_instantiation_statement() && ctx->label()) {
        auto module_name = instantiated_module_name(ctx->component_instantiation_statement());
        if (!module_name.empty())
            deps_factory.new_dependency(canon(ctx->label()->getText()), module_name, module);
    }
}

void vhdl_visitor::exitConcurrent_statement(mgp_vh::vhdlParser::Concurrent_statementContext *ctx) {
    if (deps_factory.is_valid_dependency()) {
        auto dep = deps_factory.get_dependency();
        if (ctx->component_instantiation_statement())
            dep->set_architecture(instantiated_module_arch(ctx->component_instantiation_statement()));
        if (!generate_stack.empty() && (generate_stack.back() == "if" || generate_stack.back() == "case"))
            conditionals_factory.add_statement(dep);
        else if (!generate_stack.empty() && generate_stack.back() == "for")
            loops_factory.add_statement(dep);
        else
            statement_map[current_architecture].push_back(dep);
    }
}

std::string vhdl_visitor::instantiated_module_name(
        mgp_vh::vhdlParser::Component_instantiation_statementContext *ctx) {
    auto unit = ctx->instantiated_unit();
    if (!unit || !unit->name()) return "";
    // The grammar absorbs an architecture selector into the name
    // (`entity work.foo(rtl)` parses `work.foo(rtl)` as one name), so strip a
    // trailing `(…)` before resolving the module name.
    std::string text = unit->name()->getText();
    auto paren = text.rfind('(');
    if (paren != std::string::npos && text.back() == ')')
        text = text.substr(0, paren);
    std::string module_name;
    auto dot = text.rfind('.');
    module_name = dot == std::string::npos ? text : text.substr(dot + 1);
    return canon(module_name);
}

std::string vhdl_visitor::instantiated_module_arch(
        mgp_vh::vhdlParser::Component_instantiation_statementContext *ctx) {
    auto unit = ctx->instantiated_unit();
    if (!unit) return "";
    // `entity work.foo(rtl)` — the grammar usually folds `(rtl)` into the name,
    // so extract the trailing selector from the name text.
    if (unit->name()) {
        auto text = unit->name()->getText();
        auto paren = text.rfind('(');
        if (paren != std::string::npos && text.back() == ')')
            return canon(text.substr(paren + 1, text.size() - paren - 2));
    }
    if (unit->identifier())
        return canon(unit->identifier()->getText());
    return "";
}

void vhdl_visitor::enterGeneric_map_aspect(mgp_vh::vhdlParser::Generic_map_aspectContext *ctx) {
    in_instance_generic_map = true;
    positional_gen_index = 0;
}

void vhdl_visitor::exitGeneric_map_aspect(mgp_vh::vhdlParser::Generic_map_aspectContext *ctx) {
    in_instance_generic_map = false;
}

void vhdl_visitor::enterPort_map_aspect(mgp_vh::vhdlParser::Port_map_aspectContext *ctx) {
    in_instance_port_map = true;
    positional_port_index = 0;
    positional_port_placeholder.clear();
}

void vhdl_visitor::exitPort_map_aspect(mgp_vh::vhdlParser::Port_map_aspectContext *ctx) {
    in_instance_port_map = false;
}

void vhdl_visitor::enterAssociation_element(mgp_vh::vhdlParser::Association_elementContext *ctx) {
    if (in_name_selector) return;   // association element inside an index (`sig(0)`)
    if (in_instance_generic_map) {
        auto actual = ctx->actual_part();
        bool is_open = actual && actual->actual_designator() && actual->actual_designator()->KW_OPEN();
        if (is_open) { positional_gen_index++; return; }
        if (!ctx->formal_part()) {
            // Positional: record with a placeholder name, resolved post-parse
            // against the target entity's generic order.
            params_factory.start_instance_parameter_assignment("@gen" + std::to_string(positional_gen_index++));
            params_factory.start_param_override();
            instance_override_active = true;
            return;
        }
        auto formal = ctx->formal_part()->name(0);
        if (!formal) return;
        params_factory.start_instance_parameter_assignment(canon(formal->getText()));
        params_factory.start_param_override();
        instance_override_active = true;
    } else if (in_instance_port_map) {
        auto actual = ctx->actual_part();
        bool is_open = actual && actual->actual_designator() && actual->actual_designator()->KW_OPEN();
        if (is_open) { positional_port_index++; return; }
        if (!ctx->formal_part()) {
            // Positional: record with a placeholder name, resolved post-parse
            // against the target entity's port order.
            positional_port_placeholder = "@port" + std::to_string(positional_port_index++);
            deps_factory.start_port();
            return;
        }
        deps_factory.start_port();
    }
}

void vhdl_visitor::exitAssociation_element(mgp_vh::vhdlParser::Association_elementContext *ctx) {
    if (in_name_selector) return;   // association element inside an index (`sig(0)`)
    if (instance_override_active) {
        params_factory.stop_param_override();
        auto param = params_factory.get_parameter();
        // The override carries only the actual value; the formal type is resolved
        // downstream. Drop the factory's stale current_type so it doesn't leak in.
        if (param) param->set_type(std::make_shared<HDL_simple_type>());
        if (deps_factory.is_valid_dependency())
            deps_factory.add_parameter(param);
        instance_override_active = false;
    } else if (deps_factory.is_in_port()) {
        deps_factory.stop_port();
        auto formal = ctx->formal_part() ? ctx->formal_part()->name(0) : nullptr;
        if (formal)
            deps_factory.add_port(canon(formal->getText()));
        else if (!positional_port_placeholder.empty()) {
            deps_factory.add_port(positional_port_placeholder);
            positional_port_placeholder.clear();
        }
    }
}

void vhdl_visitor::enterAssociation_list(mgp_vh::vhdlParser::Association_listContext *ctx) {
    // An indexed name in a port actual (`sig(0)`) uses the association_list rule;
    // the enclosing map's list is entered before any port is started.
    if (in_instance_port_map && deps_factory.is_in_port() && !in_name_selector) {
        in_name_selector = true;
        pending_index = true;
        pending_index_parts.clear();
    }
}

void vhdl_visitor::exitAssociation_list(mgp_vh::vhdlParser::Association_listContext *ctx) {
    if (in_name_selector && !pending_slice) in_name_selector = false;
}

void vhdl_visitor::enterName_slice_part(mgp_vh::vhdlParser::Name_slice_partContext *ctx) {
    if (!(in_instance_port_map && deps_factory.is_in_port())) return;
    in_name_selector = true;
    pending_slice = true;
    pending_slice_first.reset();
    pending_slice_second.reset();
    pending_slice_dir.clear();
}

void vhdl_visitor::exitName_slice_part(mgp_vh::vhdlParser::Name_slice_partContext *ctx) {
    if (in_name_selector) in_name_selector = false;
}

void vhdl_visitor::enterDirection(mgp_vh::vhdlParser::DirectionContext *ctx) {
    if (in_name_selector && in_instance_port_map && deps_factory.is_in_port())
        pending_slice_dir = ctx->KW_TO() ? "to" : "downto";
    if (in_loop_range) {
        // The `to`/`downto` splits the loop range: the first bound is the loop
        // init, the second becomes the end bound.
        generate_loop_dir = ctx->KW_TO() ? "to" : "downto";
        auto init_param = params_factory.get_parameter();
        params_factory.stop_param_assignment();
        loops_factory.set_loop_init(*init_param);
        params_factory.start_param_assignment();
        params_factory.new_parameter("genvar_end_bound");
    }
    if (in_case_range_choice) {
        // The direction splits a case range choice into its two bounds.
        auto first = params_factory.get_parameter();
        params_factory.stop_param_assignment();
        if (first->get_expression()) case_choice_exprs.push_back(first->get_expression());
        params_factory.start_param_assignment();
        params_factory.new_parameter("gen_case_choice");
    }
}

void vhdl_visitor::enterFor_generate_statement(mgp_vh::vhdlParser::For_generate_statementContext *ctx) {
    loops_factory.new_loop();
    generate_stack.emplace_back("for");
    in_generate_loop = true;
}

void vhdl_visitor::exitFor_generate_statement(mgp_vh::vhdlParser::For_generate_statementContext *ctx) {
    if (!in_generate_loop) return;
    in_generate_loop = false;
    auto loop = loops_factory.get_loop_statement();
    generate_stack.pop_back();
    if (!generate_stack.empty() && generate_stack.back() == "if")
        conditionals_factory.add_statement(loop);
    else
        statement_map[current_architecture].push_back(loop);
}

void vhdl_visitor::enterIf_generate_statement(mgp_vh::vhdlParser::If_generate_statementContext *ctx) {
    if (!conditionals_factory.is_active())
        conditionals_factory.new_conditional();
    else
        conditionals_factory.push_nested();
    generate_stack.emplace_back("if");
}

void vhdl_visitor::exitIf_generate_statement(mgp_vh::vhdlParser::If_generate_statementContext *ctx) {
    if (generate_stack.empty() || generate_stack.back() != "if") return;
    auto stmt = conditionals_factory.get_conditional();
    if (stmt.is_empty()) {
        generate_stack.pop_back();
        return;
    }
    auto ptr = std::make_shared<hdl_conditional_statement>(stmt);
    generate_stack.pop_back();
    if (!generate_stack.empty() && generate_stack.back() == "for")
        loops_factory.add_statement(ptr);
    else
        statement_map[current_architecture].push_back(ptr);
}

void vhdl_visitor::enterCondition(mgp_vh::vhdlParser::ConditionContext *ctx) {
    if (generate_stack.empty() || generate_stack.back() != "if") return;
    // The first condition sets the initial branch; elsif conditions add branches.
    if (conditionals_factory.has_condition())
        conditionals_factory.add_branch();
    in_generate_condition = true;
    params_factory.start_param_assignment();
    params_factory.new_parameter("gen_if_cond");
}

void vhdl_visitor::exitCondition(mgp_vh::vhdlParser::ConditionContext *ctx) {
    if (!in_generate_condition) return;
    auto param = params_factory.get_parameter();
    params_factory.stop_param_assignment();
    in_generate_condition = false;
    conditionals_factory.set_condition(param->get_expression());
}

void vhdl_visitor::enterGenerate_statement_body(mgp_vh::vhdlParser::Generate_statement_bodyContext *ctx) {
    // Only an if-generate's own bodies drive the conditional branch tracking.
    if (!generate_stack.empty() && generate_stack.back() == "if")
        conditionals_factory.enter_body_item();
}

void vhdl_visitor::exitGenerate_statement_body(mgp_vh::vhdlParser::Generate_statement_bodyContext *ctx) {
    if (!generate_stack.empty() && generate_stack.back() == "if")
        conditionals_factory.exit_body_item();
}

void vhdl_visitor::enterGenerate_statement_body_with_begin_end(mgp_vh::vhdlParser::Generate_statement_body_with_begin_endContext *ctx) {
    if (!generate_stack.empty() && generate_stack.back() == "case")
        conditionals_factory.enter_body_item();
}

void vhdl_visitor::exitGenerate_statement_body_with_begin_end(mgp_vh::vhdlParser::Generate_statement_body_with_begin_endContext *ctx) {
    if (!generate_stack.empty() && generate_stack.back() == "case")
        conditionals_factory.exit_body_item();
}

void vhdl_visitor::enterCase_generate_statement(mgp_vh::vhdlParser::Case_generate_statementContext *ctx) {
    if (!conditionals_factory.is_active())
        conditionals_factory.new_conditional();
    else
        conditionals_factory.push_nested();
    generate_stack.push_back("case");
    in_generate_case = true;
    in_case_selector = true;
    case_alternatives_seen = 0;
    params_factory.start_param_assignment();
    params_factory.new_parameter("gen_case_sel");
}

void vhdl_visitor::exitCase_generate_statement(mgp_vh::vhdlParser::Case_generate_statementContext *ctx) {
    if (generate_stack.empty() || generate_stack.back() != "case") return;
    in_generate_case = false;
    auto stmt = conditionals_factory.get_conditional();
    if (stmt.is_empty()) {
        generate_stack.pop_back();
        return;
    }
    auto ptr = std::make_shared<hdl_conditional_statement>(stmt);
    generate_stack.pop_back();
    if (!generate_stack.empty() && generate_stack.back() == "for")
        loops_factory.add_statement(ptr);
    else
        statement_map[current_architecture].push_back(ptr);
}

void vhdl_visitor::enterCase_generate_alternative(mgp_vh::vhdlParser::Case_generate_alternativeContext *ctx) {
    if (!in_generate_case) return;
    if (case_alternatives_seen == 0 && in_case_selector) {
        // The case selector is the expression routed before the first alternative.
        auto sel = params_factory.get_parameter();
        params_factory.stop_param_assignment();
        in_case_selector = false;
        case_selector_expr = sel->get_expression();
    }
    case_alternatives_seen++;
    in_case_else_alternative = false;
    in_case_range_choice = false;
    case_choice_is_range = false;
    case_choice_exprs.clear();
    bool has_others = false;
    if (ctx->choices()) {
        for (auto *ch : ctx->choices()->choice()) {
            if (ch->KW_OTHERS()) { has_others = true; break; }
        }
    }
    if (has_others) {
        in_case_else_alternative = true;
        return;
    }
    conditionals_factory.add_branch();
    in_case_choices = true;
}

void vhdl_visitor::exitCase_generate_alternative(mgp_vh::vhdlParser::Case_generate_alternativeContext *ctx) {
    if (!in_generate_case) return;
    if (in_case_choices) {
        in_case_choices = false;
        conditionals_factory.set_condition(build_case_condition(case_selector_expr, case_choice_exprs));
    }
    in_case_else_alternative = false;
}

void vhdl_visitor::enterChoice(mgp_vh::vhdlParser::ChoiceContext *ctx) {
    if (in_case_choices && !ctx->KW_OTHERS()) {
        if (ctx->discrete_range()) {
            in_case_range_choice = true;
            case_choice_is_range = true;
        }
        params_factory.start_param_assignment();
        params_factory.new_parameter("gen_case_choice");
    }
}

void vhdl_visitor::exitChoice(mgp_vh::vhdlParser::ChoiceContext *ctx) {
    if (!in_case_choices || ctx->KW_OTHERS()) return;
    if (in_case_range_choice) {
        // A range choice (`1 to 3`) routes its two bounds through the same
        // param; the direction splits them.
        auto second = params_factory.get_parameter();
        params_factory.stop_param_assignment();
        if (second->get_expression()) case_choice_exprs.push_back(second->get_expression());
        in_case_range_choice = false;
        return;
    }
    auto c = params_factory.get_parameter();
    params_factory.stop_param_assignment();
    if (c->get_expression()) case_choice_exprs.push_back(c->get_expression());
}

std::shared_ptr<Expression_base> vhdl_visitor::build_case_condition(
        const std::shared_ptr<Expression_base> &selector,
        const std::vector<std::shared_ptr<Expression_base>> &choices) {
    if (!selector || choices.empty()) return nullptr;
    if (case_choice_is_range && choices.size() >= 2) {
        // `when A to B` → SEL in [A, B] → (SEL >= A) AND (SEL <= B).
        auto lo = std::make_shared<Expression_v2>();
        lo->set_lhs(selector);
        lo->set_rhs(choices[0]);
        lo->set_operation(Expression_v2::greater_equal);
        auto hi = std::make_shared<Expression_v2>();
        hi->set_lhs(selector);
        hi->set_rhs(choices[1]);
        hi->set_operation(Expression_v2::less_equal);
        auto range_cond = std::make_shared<Expression_v2>();
        range_cond->set_lhs(lo);
        range_cond->set_rhs(hi);
        range_cond->set_operation(Expression_v2::logical_and);
        return range_cond;
    }
    std::shared_ptr<Expression_base> cond;
    for (auto &ch : choices) {
        auto eq = std::make_shared<Expression_v2>();
        eq->set_lhs(selector);
        eq->set_rhs(ch);
        eq->set_operation(Expression_v2::equal);
        if (!cond) {
            cond = eq;
        } else {
            auto or_expr = std::make_shared<Expression_v2>();
            or_expr->set_lhs(cond);
            or_expr->set_rhs(eq);
            or_expr->set_operation(Expression_v2::logical_or);
            cond = or_expr;
        }
    }
    return cond;
}

void vhdl_visitor::enterParameter_specification(mgp_vh::vhdlParser::Parameter_specificationContext *ctx) {
    if (!in_generate_loop) return;
    generate_loop_var = canon(ctx->identifier()->getText());
    in_loop_range = true;
    loops_factory.set_phase(HDL_loops_factory::init);
    params_factory.start_param_assignment();
    params_factory.new_parameter(generate_loop_var);
}

void vhdl_visitor::exitParameter_specification(mgp_vh::vhdlParser::Parameter_specificationContext *ctx) {
    if (!in_generate_loop) return;
    auto end_param = params_factory.get_parameter();
    params_factory.stop_param_assignment();
    in_loop_range = false;

    // VHDL `for i in A to B generate` maps onto the shared loop model as:
    // init = A, end condition = `i <= B` (to) / `i >= B` (downto), iteration = `i +/- 1`.
    auto loop_var = std::make_shared<Identifier_token>(qualified_identifier(generate_loop_var));
    auto end_cond = std::make_shared<Expression_v2>();
    end_cond->set_lhs(loop_var);
    if (end_param->get_expression()) end_cond->set_rhs(end_param->get_expression());
    end_cond->set_operation(generate_loop_dir == "to" ? Expression_v2::less_equal : Expression_v2::greater_equal);

    auto iter = std::make_shared<Expression_v2>();
    iter->set_lhs(loop_var);
    iter->set_rhs(std::make_shared<Numeric_token>("1"));
    iter->set_operation(generate_loop_dir == "to" ? Expression_v2::add : Expression_v2::subtract);

    loops_factory.add_expression(*end_cond);
    loops_factory.set_phase(HDL_loops_factory::step);
    loops_factory.add_expression(*iter);
    loops_factory.set_phase(HDL_loops_factory::body);
}

void vhdl_visitor::route_port_slice(const std::string &base, const std::string &first,
                                    const std::string &dir, const std::string &second) {
    if (!deps_factory.is_valid_dependency() || !deps_factory.is_in_port()) return;
    deps_factory.start_scalar_net(base);
    deps_factory.start_array_range();
    deps_factory.start_expression(false);
    deps_factory.add_connection_element(first);
    deps_factory.stop_expression(false);
    deps_factory.advance_array_range_phase(dir == "to" ? "+" : "-");
    deps_factory.start_expression(false);
    deps_factory.add_connection_element(second);
    deps_factory.stop_expression(false);
    deps_factory.stop_array_range();
}

void vhdl_visitor::route_port_index(const std::string &base, const std::vector<std::string> &idx) {
    if (!deps_factory.is_valid_dependency() || !deps_factory.is_in_port()) return;
    deps_factory.start_scalar_net(base);
    deps_factory.start_bit_selection();
    for (auto &i : idx) deps_factory.add_connection_element(i);
    deps_factory.stop_bit_selection();
}

void vhdl_visitor::clear_pending_selector() {
    pending_slice = false;
    pending_index = false;
    pending_slice_first.reset();
    pending_slice_second.reset();
    pending_slice_dir.clear();
    pending_index_parts.clear();
}

void vhdl_visitor::route_port_actual(mgp_vh::vhdlParser::Numeric_literalContext *ctx) {
    if (in_name_selector) {
        // A bound/index inside a slice (`sig(7 downto 0)`) or index (`sig(0)`):
        // capture it for the enclosing name rather than routing it as a net.
        if (pending_slice) {
            if (!pending_slice_first.has_value()) pending_slice_first = canon(ctx->getText());
            else if (!pending_slice_second.has_value()) pending_slice_second = canon(ctx->getText());
        } else if (pending_index) {
            pending_index_parts.push_back(canon(ctx->getText()));
        }
        return;
    }
    // The base of an indexed/sliced name is the text up to the first `(` (the
    // name_literal lives on the innermost `name` child, not the outer one).
    auto full_text = ctx->getText();
    auto paren = full_text.find('(');
    std::string base = paren == std::string::npos ? full_text : full_text.substr(0, paren);
    if (pending_slice && pending_slice_first.has_value() && pending_slice_second.has_value()) {
        route_port_slice(canon(base), *pending_slice_first, pending_slice_dir, *pending_slice_second);
        clear_pending_selector();
        return;
    }
    if (pending_index) {
        route_port_index(canon(base), pending_index_parts);
        clear_pending_selector();
        return;
    }
    route_port_connection(canon(ctx->getText()));
}

void vhdl_visitor::route_port_connection(const std::string &text) {
    if (!deps_factory.is_valid_dependency() || !deps_factory.is_in_port()) return;
    deps_factory.add_connection_element(canon(text));
    deps_factory.start_scalar_net(canon(text));
}

void vhdl_visitor::enterArchitecture_body(mgp_vh::vhdlParser::Architecture_bodyContext *ctx) {
    // Real architecture name (`architecture rtl of foo` → identifier(0) = rtl,
    // name = foo). Each architecture becomes its own implementation resource;
    // statements are bucketed per (entity, arch) so same-named architectures of
    // different entities don't collide.
    std::string entity_name = canon(ctx->name()->getText());
    std::string arch_name = canon(ctx->identifier(0)->getText());
    current_architecture = entity_name + "::" + arch_name;
    size_t line_number = ctx->getStart()->getLine();
    modules_factory.new_module(entity_name, module, line_number);
}

void vhdl_visitor::enterGeneric_clause(mgp_vh::vhdlParser::Generic_clauseContext *ctx) {
    in_generic_clause = true;
}

void vhdl_visitor::exitGeneric_clause(mgp_vh::vhdlParser::Generic_clauseContext *ctx) {
    in_generic_clause = false;
}

void vhdl_visitor::enterInterface_constant_declaration(
        mgp_vh::vhdlParser::Interface_constant_declarationContext *ctx) {
    if (in_generic_clause)
        start_generic(ctx->identifier_list(), ctx->subtype_indication());
}

void vhdl_visitor::exitInterface_constant_declaration(
        mgp_vh::vhdlParser::Interface_constant_declarationContext *ctx) {
    if (in_generic_clause)
        finalize_generic(ctx->identifier_list());
}

void vhdl_visitor::enterInterface_signal_declaration(
        mgp_vh::vhdlParser::Interface_signal_declarationContext *ctx) {
    // A generic declared without the `constant` keyword (the common form,
    // e.g. `N : integer := 8`) is parsed as an interface signal declaration;
    // ports use the same rule. Distinguish by the enclosing clause.
    if (in_generic_clause)
        start_generic(ctx->identifier_list(), ctx->subtype_indication());
}

void vhdl_visitor::exitInterface_signal_declaration(
        mgp_vh::vhdlParser::Interface_signal_declarationContext *ctx) {
    if (in_generic_clause)
        finalize_generic(ctx->identifier_list());
    else if (in_port_clause)
        finalize_port(ctx);
}

void vhdl_visitor::enterPort_clause(mgp_vh::vhdlParser::Port_clauseContext *ctx) {
    in_port_clause = true;
}

void vhdl_visitor::exitPort_clause(mgp_vh::vhdlParser::Port_clauseContext *ctx) {
    in_port_clause = false;
}

void vhdl_visitor::finalize_port(mgp_vh::vhdlParser::Interface_signal_declarationContext *ctx) {
    if (!ctx->identifier_list()) return;
    auto dir = raw_port;
    if (ctx->signal_mode()) {
        if (ctx->signal_mode()->KW_IN()) dir = input_port;
        else if (ctx->signal_mode()->KW_OUT()) dir = output_port;
        else if (ctx->signal_mode()->KW_INOUT()) dir = inout_port;
        else if (ctx->signal_mode()->KW_BUFFER()) dir = output_port;   // treated as output
        else if (ctx->signal_mode()->KW_LINKAGE()) dir = raw_port;
    }
    for (auto *id : ctx->identifier_list()->identifier()) {
        HDL_port port;
        port.direction = dir;
        auto name = canon(id->getText());
        modules_factory.add_port(name, port);
        if (in_entity_declaration)
            entity_port_order[current_entity_name].push_back(name);
    }
}

void vhdl_visitor::enterSubtype_indication(mgp_vh::vhdlParser::Subtype_indicationContext *ctx) {
    if (in_generic_clause || in_type_declaration || in_subtype_declaration || in_constant_declaration) {
        in_subtype_indication = true;
        type_engine.start_type_resolution();
    }
}

void vhdl_visitor::exitSubtype_indication(mgp_vh::vhdlParser::Subtype_indicationContext *ctx) {
    if (!(in_generic_clause || in_type_declaration || in_subtype_declaration || in_constant_declaration)) return;
    in_subtype_indication = false;
    auto result = type_engine.finish_type_resolution();
    if (in_type_declaration) {
        if (in_record_element) {
            pending_resolved_type = result;
        } else if (decl_context == vhdl_type_kind::array) {
            type_engine.set_array_element_type(result);
        }
    } else {
        pending_resolved_type = result;
    }
}

void vhdl_visitor::start_generic(mgp_vh::vhdlParser::Identifier_listContext *ids,
                                 mgp_vh::vhdlParser::Subtype_indicationContext *type) {
    if (!ids || ids->identifier().empty()) return;
    // The type is resolved by the type engine during the subtype walk; set a
    // provisional type now and override it in finalize_generic.
    params_factory.set_type(make_generic_type(type));
    params_factory.start_param_assignment();
    params_factory.new_parameter(canon(ids->identifier(0)->getText()));
}

void vhdl_visitor::finalize_generic(mgp_vh::vhdlParser::Identifier_listContext *ids) {
    if (!ids || ids->identifier().empty()) return;
    params_factory.stop_param_assignment();

    // The default expression (built by the listener callbacks) belongs to the
    // first name; the remaining names in the list share the same type/default,
    // so clone it.
    auto base = params_factory.get_parameter();
    if (pending_resolved_type)
        base->set_type(pending_resolved_type);
    pending_resolved_type = nullptr;
    modules_factory.add_parameter(base);
    for (size_t i = 1; i < ids->identifier().size(); i++) {
        auto clone = std::make_shared<HDL_parameter>(*base);
        clone->set_name(canon(ids->identifier(i)->getText()));
        modules_factory.add_parameter(clone);
    }
    if (in_entity_declaration) {
        for (auto *id : ids->identifier())
            entity_generic_order[current_entity_name].push_back(canon(id->getText()));
    }
}

std::shared_ptr<hdl_type> vhdl_visitor::make_generic_type(
        mgp_vh::vhdlParser::Subtype_indicationContext *type) {
    auto t = std::make_shared<HDL_simple_type>();
    if (type && type->type_mark() && type->type_mark()->name() &&
        type->type_mark()->name()->name_literal() &&
        type->type_mark()->name()->name_literal()->identifier()) {
        std::string type_name = canon(type->type_mark()->name()->name_literal()->identifier()->getText());
        t->set_type_name(type_name);
        // integer/natural/positive are signed integer-like scalars.
        if (type_name == "integer" || type_name == "natural" || type_name == "positive")
            t->set_signed(true);
        if (type_name == "real")
            t->set_real(true);
    }
    return t;
}

bool vhdl_visitor::has_expr_operator(mgp_vh::vhdlParser::ExpressionContext *ctx) {
    return ctx && (ctx->shift_operator() || ctx->relational_operator() || ctx->logical_operator());
}

bool vhdl_visitor::simple_is_nonleaf(mgp_vh::vhdlParser::Simple_expressionContext *ctx) {
    return ctx && (ctx->DOUBLESTAR() || ctx->KW_ABS() || ctx->KW_NOT() || ctx->logical_operator()
        || ctx->sign() || ctx->multiplying_operator() || ctx->adding_operator());
}

std::optional<Expression_v2::expression_operator> vhdl_visitor::simple_expression_op(
        mgp_vh::vhdlParser::Simple_expressionContext *ctx) {
    if (!ctx || !simple_is_nonleaf(ctx)) return std::nullopt;

    if (ctx->DOUBLESTAR()) return Expression_v2::power;
    if (ctx->KW_ABS()) return Expression_v2::abs_value;
    if (ctx->KW_NOT()) return Expression_v2::bitwise_neg;
    if (ctx->logical_operator()) {
        // VHDL allows a logical operator as a unary reduction over a vector.
        if (ctx->logical_operator()->KW_AND() || ctx->logical_operator()->KW_NAND())
            return Expression_v2::reduction_and;
        if (ctx->logical_operator()->KW_OR() || ctx->logical_operator()->KW_NOR())
            return Expression_v2::reduction_or;
        return Expression_v2::reduction_xor;
    }
    if (ctx->multiplying_operator()) {
        if (ctx->multiplying_operator()->DIV()) return Expression_v2::divide;
        if (ctx->multiplying_operator()->KW_MOD()) return Expression_v2::v_mod;   // floor mod
        if (ctx->multiplying_operator()->KW_REM()) return Expression_v2::modulo;  // truncated rem
        return Expression_v2::multiply;
    }
    if (ctx->adding_operator()) {
        if (ctx->adding_operator()->PLUS()) return Expression_v2::add;
        if (ctx->adding_operator()->MINUS()) return Expression_v2::subtract;
        spdlog::warn("VHDL concatenation (&) in generic default not supported, ignored");
        return std::nullopt;
    }
    if (ctx->sign() && ctx->sign()->MINUS()) return Expression_v2::subtract;
    return std::nullopt;
}

void vhdl_visitor::enterExpression(mgp_vh::vhdlParser::ExpressionContext *ctx) {
    if (is_in_generic_expression() && (has_expr_operator(ctx) || ctx->COND_OP()))
        params_factory.start_expression_new(true);
}

void vhdl_visitor::exitExpression(mgp_vh::vhdlParser::ExpressionContext *ctx) {
    if (!is_in_generic_expression()) return;

    auto op = Expression_v2::none;
    if (ctx->COND_OP()) {
        // VHDL `??` condition operator: normalize a condition to 0/1.
        op = Expression_v2::condition_op;
    } else if (ctx->shift_operator()) {
        if (ctx->shift_operator()->KW_SLL()) op = Expression_v2::logic_shift_left;
        else if (ctx->shift_operator()->KW_SRL()) op = Expression_v2::logic_shift_right;
        else if (ctx->shift_operator()->KW_SLA()) op = Expression_v2::arithmetic_shift_left;
        else if (ctx->shift_operator()->KW_SRA()) op = Expression_v2::arithmetic_shift_right;
        else if (ctx->shift_operator()->KW_ROL()) op = Expression_v2::rotate_left;
        else if (ctx->shift_operator()->KW_ROR()) op = Expression_v2::rotate_right;
    } else if (ctx->relational_operator()) {
        if (ctx->relational_operator()->EQ()) op = Expression_v2::equal;
        else if (ctx->relational_operator()->NE()) op = Expression_v2::not_equal;
        else if (ctx->relational_operator()->LT()) op = Expression_v2::less;
        else if (ctx->relational_operator()->CONASGN()) op = Expression_v2::less_equal; // VHDL <=
        else if (ctx->relational_operator()->GT()) op = Expression_v2::greater;
        else if (ctx->relational_operator()->GE()) op = Expression_v2::greater_equal;
    } else if (ctx->logical_operator()) {
        if (ctx->logical_operator()->KW_AND()) op = Expression_v2::bitwise_and;
        else if (ctx->logical_operator()->KW_OR()) op = Expression_v2::bitwise_or;
        else if (ctx->logical_operator()->KW_XOR()) op = Expression_v2::bitwise_xor;
        else if (ctx->logical_operator()->KW_XNOR()) op = Expression_v2::bitwise_xnor;
    }
    if (op != Expression_v2::none)
        params_factory.set_operation(op);
    if (op != Expression_v2::none)
        params_factory.stop_expression_new(true);
}

void vhdl_visitor::enterSimple_expression(mgp_vh::vhdlParser::Simple_expressionContext *ctx) {
    // Range bound inside a type constraint: route into the type engine.
    if (type_engine.in_range()) {
        type_engine.start_bound_expression(simple_is_nonleaf(ctx));
        return;
    }
    // An unambiguous aggregate primary is handled by enter/exitAggregate; skip
    // bracketing it here so the aggregate elements reach level 0 independently.
    if (is_in_generic_expression() && !simple_is_aggregate(ctx))
        params_factory.start_expression_new(simple_is_nonleaf(ctx));
}

void vhdl_visitor::exitSimple_expression(mgp_vh::vhdlParser::Simple_expressionContext *ctx) {
    // Range bound inside a type constraint: route into the type engine.
    if (type_engine.in_range()) {
        if (auto op = simple_expression_op(ctx); op.has_value())
            type_engine.set_range_operation(op.value());
        type_engine.stop_bound_expression(simple_is_nonleaf(ctx));
        return;
    }
    if (!is_in_generic_expression() || simple_is_aggregate(ctx)) return;

    if (auto op = simple_expression_op(ctx); op.has_value())
        params_factory.set_operation(op.value());
    params_factory.stop_expression_new(simple_is_nonleaf(ctx));
}

void vhdl_visitor::enterNumeric_literal(mgp_vh::vhdlParser::Numeric_literalContext *ctx) {
    if (!is_in_generic_expression() || !params_factory.is_component_relevant()) return;
    if (!ctx->name() || !is_function_call(ctx->name())) return;

    // A VHDL function call such as `log2(N)`. The factory classifies it: known
    // builtins become an HDL_builtin_function, anything else a user function.
    auto call_name = extract_call_name(ctx->name());
    if (!is_vhdl_builtin(call_name))
        spdlog::warn("Unsupported VHDL function '{}' in generic default, ignored", call_name);
    params_factory.start_function_call(call_name);
}

void vhdl_visitor::enterActual_part(mgp_vh::vhdlParser::Actual_partContext *ctx) {
    // `name ( actual_designator )` on the connection side is an indexed name
    // (`sig(0)`); in a generic default it is a function call.
    if (in_instance_port_map && deps_factory.is_in_port() && ctx->name() && ctx->LPAREN()) {
        in_name_selector = true;
        pending_index = true;
        pending_index_base = canon(ctx->name()->getText());
        pending_index_parts.clear();
        return;
    }
    if (!is_in_generic_expression() || !params_factory.is_component_relevant()) return;
    // A nested function call parsed as `name ( actual_designator )` (e.g. the
    // `log2(N)` inside `ceil(log2(N))`) arrives here rather than through the
    // `name(association_list)` rule.
    if (ctx->name() && ctx->LPAREN() && ctx->name()->name_literal() &&
        ctx->name()->name_literal()->identifier()) {
        auto call_name = canon(ctx->name()->name_literal()->identifier()->getText());
        if (!is_vhdl_builtin(call_name))
            spdlog::warn("Unsupported VHDL function '{}' in generic default, ignored", call_name);
        params_factory.start_function_call(call_name);
    }
}

void vhdl_visitor::exitActual_part(mgp_vh::vhdlParser::Actual_partContext *ctx) {
    if (in_instance_port_map && deps_factory.is_in_port() && ctx->name() && ctx->LPAREN()) {
        in_name_selector = false;
        route_port_index(pending_index_base, pending_index_parts);
        clear_pending_selector();
        return;
    }
    if (!is_in_generic_expression() || !params_factory.is_component_relevant()) return;
    if (ctx->name() && ctx->LPAREN() && ctx->name()->name_literal() &&
        ctx->name()->name_literal()->identifier()) {
        params_factory.stop_function_call();
    }
}

void vhdl_visitor::enterQualified_expression(
        mgp_vh::vhdlParser::Qualified_expressionContext *ctx) {
    if (!is_in_generic_expression() || !params_factory.is_component_relevant()) return;

    // VHDL qualified expression / type conversion: `type'(expr)`. The content
    // is parenthesized, so it is an expression-sized cast.
    params_factory.start_cast(true);

    if (ctx->type_mark() && ctx->type_mark()->name()) {
        auto nm = ctx->type_mark()->name();
        std::string type_name;
        if (nm->name_literal() && nm->name_literal()->identifier())
            type_name = canon(nm->name_literal()->identifier()->getText());
        else if (nm->suffix())
            type_name = canon(nm->suffix()->getText()); // pkg.type
        if (!type_name.empty())
            params_factory.set_cast_type(type_name);
    }
}

void vhdl_visitor::exitQualified_expression(
        mgp_vh::vhdlParser::Qualified_expressionContext *ctx) {
    if (!is_in_generic_expression() || !params_factory.is_component_relevant()) return;
    params_factory.stop_cast();
}

void vhdl_visitor::exitType_mark(mgp_vh::vhdlParser::Type_markContext *ctx) {
    if (!type_engine.active()) return;
    // A type_mark is a `name`, which may include a slice/constraint suffix such
    // as `std_logic_vector(7 downto 0)`; walk down to the leaf identifier.
    auto nm = ctx->name();
    while (nm) {
        if (nm->name_literal() && nm->name_literal()->identifier()) {
            type_engine.set_type_mark(canon(nm->name_literal()->identifier()->getText()));
            return;
        }
        if (nm->suffix()) {
            type_engine.set_type_mark(canon(nm->suffix()->getText())); // pkg.type
            return;
        }
        nm = nm->name();
    }
}

void vhdl_visitor::enterIndex_constraint(mgp_vh::vhdlParser::Index_constraintContext *ctx) {
    type_engine.start_array_constraint();
}

void vhdl_visitor::exitIndex_constraint(mgp_vh::vhdlParser::Index_constraintContext *ctx) {
    type_engine.stop_array_constraint();
}

void vhdl_visitor::enterExplicit_range(mgp_vh::vhdlParser::Explicit_rangeContext *ctx) {
    type_engine.start_range();
}

void vhdl_visitor::exitExplicit_range(mgp_vh::vhdlParser::Explicit_rangeContext *ctx) {
    bool descending = ctx->direction() && ctx->direction()->KW_DOWNTO() != nullptr;
    type_engine.stop_range(descending);
}

void vhdl_visitor::enterType_declaration(mgp_vh::vhdlParser::Type_declarationContext *ctx) {
    in_type_declaration = true;
    decl_context = vhdl_type_kind::none;
    declared_type_name.clear();
    if (ctx->full_type_declaration() && ctx->full_type_declaration()->identifier()) {
        declared_type_name = canon(ctx->full_type_declaration()->identifier()->getText());
        type_engine.start_type_declaration(declared_type_name);
    }
}

void vhdl_visitor::exitType_declaration(mgp_vh::vhdlParser::Type_declarationContext *ctx) {
    if (!in_type_declaration) return;
    auto t = type_engine.finish_type_declaration();
    if (t && !declared_type_name.empty())
        modules_factory.add_typedef(declared_type_name, t);
    in_type_declaration = false;
    decl_context = vhdl_type_kind::none;
    declared_type_name.clear();
}

void vhdl_visitor::enterType_definition(mgp_vh::vhdlParser::Type_definitionContext *ctx) {
    if (!in_type_declaration) return;
    vhdl_type_kind kind = vhdl_type_kind::none;
    if (ctx->scalar_type_definition() && ctx->scalar_type_definition()->enumeration_type_definition())
        kind = vhdl_type_kind::enumeration;
    else if (ctx->scalar_type_definition())
        kind = vhdl_type_kind::scalar;
    else if (ctx->composite_type_definition() && ctx->composite_type_definition()->array_type_definition())
        kind = vhdl_type_kind::array;
    else if (ctx->composite_type_definition() && ctx->composite_type_definition()->record_type_definition())
        kind = vhdl_type_kind::record;
    decl_context = kind;
    type_engine.set_declaration_kind(kind);
}

void vhdl_visitor::enterEnumeration_literal(mgp_vh::vhdlParser::Enumeration_literalContext *ctx) {
    if (!in_type_declaration || decl_context != vhdl_type_kind::enumeration) return;
    if (ctx->identifier())
        type_engine.add_enum_literal(canon(ctx->identifier()->getText()));
    else if (ctx->CHARACTER_LITERAL())
        type_engine.add_enum_literal(ctx->CHARACTER_LITERAL()->getText());
}

void vhdl_visitor::enterElement_declaration(mgp_vh::vhdlParser::Element_declarationContext *ctx) {
    if (!in_type_declaration || decl_context != vhdl_type_kind::record) return;
    in_record_element = true;
    std::vector<std::string> names;
    if (ctx->identifier_list()) {
        for (auto *id : ctx->identifier_list()->identifier())
            names.push_back(canon(id->getText()));
    }
    type_engine.begin_record_element(names);
}

void vhdl_visitor::exitElement_declaration(mgp_vh::vhdlParser::Element_declarationContext *ctx) {
    if (!in_type_declaration || decl_context != vhdl_type_kind::record || !in_record_element) return;
    in_record_element = false;
    type_engine.end_record_element(pending_resolved_type);
    pending_resolved_type = nullptr;
}

void vhdl_visitor::enterSubtype_declaration(mgp_vh::vhdlParser::Subtype_declarationContext *ctx) {
    in_subtype_declaration = true;
    subtype_decl_name = canon(ctx->identifier()->getText());
}

void vhdl_visitor::exitSubtype_declaration(mgp_vh::vhdlParser::Subtype_declarationContext *ctx) {
    if (!in_subtype_declaration) return;
    if (pending_resolved_type) {
        type_engine.register_type(subtype_decl_name, pending_resolved_type);
        modules_factory.add_typedef(subtype_decl_name, pending_resolved_type);
    }
    pending_resolved_type = nullptr;
    in_subtype_declaration = false;
    subtype_decl_name.clear();
}

void vhdl_visitor::enterPackage_declaration(mgp_vh::vhdlParser::Package_declarationContext *ctx) {
    std::string package_name = canon(ctx->identifier()[0]->getText());
    size_t line_number = ctx->getStart()->getLine();
    modules_factory.new_module(package_name, package, line_number);
    in_package_declaration = true;
}

void vhdl_visitor::exitPackage_declaration(mgp_vh::vhdlParser::Package_declarationContext *ctx) {
    if (!in_package_declaration) return;
    entities.push_back(modules_factory.get_module());
    in_package_declaration = false;
}

void vhdl_visitor::enterConstant_declaration(mgp_vh::vhdlParser::Constant_declarationContext *ctx) {
    if (!in_package_declaration) return;
    in_constant_declaration = true;
    // Same shape as a generic: identifier_list : type := default.
    start_generic(ctx->identifier_list(), ctx->subtype_indication());
}

void vhdl_visitor::exitConstant_declaration(mgp_vh::vhdlParser::Constant_declarationContext *ctx) {
    if (!in_constant_declaration) return;
    finalize_generic(ctx->identifier_list());
    in_constant_declaration = false;
}

void vhdl_visitor::enterUse_clause(mgp_vh::vhdlParser::Use_clauseContext *ctx) {
    // `use work.pkg.all` / `use work.pkg.item` / `use pkg` — imports are
    // file-level statements, collected on the hdl_file.
    for (auto *sn : ctx->selected_name()) {
        // Split the dotted name into [library, package, (all|item)].
        std::vector<std::string> segments;
        segments.push_back(canon(sn->identifier()->getText()));
        for (auto *suf : sn->suffix()) {
            if (suf->KW_ALL()) segments.push_back("all");
            else if (suf->name_literal() && suf->name_literal()->identifier())
                segments.push_back(canon(suf->name_literal()->identifier()->getText()));
            else
                segments.push_back(canon(suf->getText()));
        }
        auto stmt = std::make_shared<hdl_import_stmt>();
        size_t n = segments.size();
        if (n >= 2 && segments.back() == "all") {
            stmt->set_wildcard(true);
            stmt->set_package(segments[n - 2]);
            if (n >= 3) stmt->set_library(segments[n - 3]);
        } else if (n >= 3) {
            stmt->set_item(segments.back());
            stmt->set_package(segments[n - 2]);
            stmt->set_library(segments[n - 3]);
        } else {
            stmt->set_package(segments.back());
            if (n == 2) stmt->set_library(segments[0]);
        }
        file_imports.push_back(stmt);
    }
}

void vhdl_visitor::exitNumeric_literal(mgp_vh::vhdlParser::Numeric_literalContext *ctx) {
    // Range bound inside a type constraint: route into the type engine.
    if (type_engine.in_range()) {
        type_engine.add_range_component(make_numeric_value(ctx));
        return;
    }
    if (in_instance_port_map && deps_factory.is_in_port()) {
        route_port_actual(ctx);
        return;
    }
    if (!is_in_generic_expression() || !params_factory.is_component_relevant()) return;

    // A function call: finalize it (the argument expressions have been routed
    // into the function factory by the intervening listener callbacks).
    if (ctx->name() && is_function_call(ctx->name())) {
        params_factory.stop_function_call();
        return;
    }

    // A bare identifier: a reference to another generic/constant (possibly a
    // package-qualified selected name like `pkg.CONST`).
    if (ctx->name()) {
        std::string base = selected_name_base(ctx->name());
        if (!base.empty()) {
            auto suffix = ctx->name()->suffix();
            if (suffix && suffix->name_literal() && suffix->name_literal()->identifier()) {
                params_factory.add_component(std::make_shared<Identifier_token>(
                    qualified_identifier(canon(base), canon(suffix->name_literal()->identifier()->getText()))));
            } else {
                params_factory.add_component(std::make_shared<Identifier_token>(qualified_identifier(canon(base))));
            }
            return;
        }
    }

    // A character literal ('a') or string literal ("...") also arrive through
    // the `name` rule.
    if (ctx->name() && ctx->name()->name_literal()) {
        auto lit = ctx->name()->name_literal();
        if (lit->CHARACTER_LITERAL()) {
            params_factory.add_component(make_character_value(lit->CHARACTER_LITERAL()->getText()));
            return;
        }
        if (lit->operator_symbol() && lit->operator_symbol()->STRING_LITERAL()) {
            auto str = lit->operator_symbol()->STRING_LITERAL()->getText();
            params_factory.add_component(std::make_shared<String_token>(str));
            return;
        }
    }

    params_factory.add_component(make_vhdl_value(ctx->getText()));
}

std::shared_ptr<Expression_base> vhdl_visitor::make_numeric_value(
        mgp_vh::vhdlParser::Numeric_literalContext *ctx) {
    if (!ctx) return nullptr;
    // A bare identifier: a reference to another generic/constant (possibly a
    // package-qualified selected name like `pkg.CONST`).
    if (ctx->name()) {
        std::string base = selected_name_base(ctx->name());
        if (!base.empty()) {
            auto suffix = ctx->name()->suffix();
            if (suffix && suffix->name_literal() && suffix->name_literal()->identifier())
                return std::make_shared<Identifier_token>(
                    qualified_identifier(canon(base), canon(suffix->name_literal()->identifier()->getText())));
            return std::make_shared<Identifier_token>(qualified_identifier(canon(base)));
        }
    }
    // Character/string literals also arrive through the `name` rule.
    if (ctx->name() && ctx->name()->name_literal()) {
        auto lit = ctx->name()->name_literal();
        if (lit->CHARACTER_LITERAL())
            return make_character_value(lit->CHARACTER_LITERAL()->getText());
        if (lit->operator_symbol() && lit->operator_symbol()->STRING_LITERAL())
            return std::make_shared<String_token>(lit->operator_symbol()->STRING_LITERAL()->getText());
    }
    return make_vhdl_value(ctx->getText());
}

bool vhdl_visitor::is_function_call(mgp_vh::vhdlParser::NameContext *nm) {
    return nm && nm->association_list() != nullptr;
}

std::string vhdl_visitor::extract_call_name(mgp_vh::vhdlParser::NameContext *nm) {
    if (!nm) return "";
    auto base = nm->name(); // the prefix (function) name
    if (!base) return "";
    if (base->name_literal() && base->name_literal()->identifier())
        return canon(base->name_literal()->identifier()->getText());
    if (base->suffix())
        return canon(base->suffix()->getText()); // qualified: pkg.func
    if (base->name())
        return extract_call_name(base);
    return "";
}

bool vhdl_visitor::is_vhdl_builtin(const std::string &name) {
    return HDL_builtin_function::from_vhdl(name).has_value();
}

void vhdl_visitor::exitPrimary(mgp_vh::vhdlParser::PrimaryContext *ctx) {
    if (ctx->BIT_STRING_LITERAL()) {
        // Treat a bit string literal as a sized numeric value (e.g. x"FF").
        if (type_engine.in_range()) {
            type_engine.add_range_component(make_vhdl_value(ctx->BIT_STRING_LITERAL()->getText()));
            return;
        }
        if (!is_in_generic_expression() || !params_factory.is_component_relevant()) return;
        params_factory.add_component(make_vhdl_value(ctx->BIT_STRING_LITERAL()->getText()));
    }
}

void vhdl_visitor::enterAggregate(mgp_vh::vhdlParser::AggregateContext *ctx) {
    // `(expr)` is ambiguous between a parenthesized expression and a
    // single-element aggregate. Only treat it as an aggregate when it is
    // unambiguous: multiple elements, or an element with `choices`/`others`.
    if (is_in_generic_expression() && is_aggregate(ctx))
        params_factory.start_initialization_list();
}

void vhdl_visitor::exitAggregate(mgp_vh::vhdlParser::AggregateContext *ctx) {
    if (!is_in_generic_expression() || !is_aggregate(ctx)) return;

    // An aggregate using `others` sets the default-initialization flag.
    bool default_assignment = false;
    for (auto *el : ctx->element_association()) {
        if (el->choices()) {
            for (auto *c : el->choices()->choice()) {
                if (c->KW_OTHERS()) { default_assignment = true; break; }
            }
        }
        if (default_assignment) break;
    }
    params_factory.stop_initialization_list(default_assignment);
}

void vhdl_visitor::enterChoices(mgp_vh::vhdlParser::ChoicesContext *ctx) {
    // Aggregate choices suppress expression routing; case-generate choices route.
    if (!in_case_choices)
        in_aggregate_choices = true;
}

void vhdl_visitor::exitChoices(mgp_vh::vhdlParser::ChoicesContext *ctx) {
    if (!in_case_choices)
        in_aggregate_choices = false;
}

bool vhdl_visitor::is_aggregate(mgp_vh::vhdlParser::AggregateContext *ctx) {
    if (ctx->element_association().size() > 1) return true;
    // Single element: unambiguous aggregate only if it has `choices`
    // (e.g. `(0 => a)` or `(others => a)`).
    if (!ctx->element_association().empty())
        return ctx->element_association(0)->choices() != nullptr;
    return false;
}

bool vhdl_visitor::simple_is_aggregate(mgp_vh::vhdlParser::Simple_expressionContext *ctx) {
    return ctx && ctx->primary(0) && ctx->primary(0)->aggregate() &&
           is_aggregate(ctx->primary(0)->aggregate());
}

bool vhdl_visitor::is_in_generic_expression() const {
    return (in_generic_clause || in_instance_generic_map || in_loop_range || in_generate_condition
            || in_case_selector || in_case_choices || in_constant_declaration) && !in_subtype_indication && !in_aggregate_choices;
}

std::shared_ptr<Expression_base> vhdl_visitor::make_character_value(const std::string &text) {
    // A VHDL character literal like 'a' is represented by its ASCII value.
    // The text includes the surrounding apostrophes.
    if (text.size() >= 3 && text.front() == '\'' && text.back() == '\'') {
        char c = text[1];
        return std::make_shared<Numeric_token>(std::to_string(static_cast<int>(static_cast<unsigned char>(c))));
    }
    spdlog::warn("Malformed VHDL character literal '{}' in generic default, ignored", text);
    return nullptr;
}

std::shared_ptr<Expression_base> vhdl_visitor::make_vhdl_value(const std::string &text) {
    if (text.empty()) return nullptr;

    // Bit-string literal, e.g. x"FF", b"1010", o"17", d"255",
    // 8x"1F", ub"1101", sx"FF".
    if (text.find('"') != std::string::npos) {
        size_t quote_open = text.find('"');
        std::string prefix = text.substr(0, quote_open);
        size_t quote_close = text.rfind('"');
        if (quote_close > quote_open) {
            std::string digits = text.substr(quote_open + 1, quote_close - quote_open - 1);
            // prefix = [width](u|s)?base
            int64_t width = -1;
            bool signed_number = false;
            size_t i = 0;
            // Optional integer width.
            while (i < prefix.size() && std::isdigit(static_cast<unsigned char>(prefix[i]))) i++;
            if (i > 0) {
                std::from_chars(prefix.data(), prefix.data() + i, width, 10);
            }
            if (i < prefix.size() && (prefix[i] == 'u' || prefix[i] == 's')) {
                signed_number = prefix[i] == 's';
                i++;
            }
            if (i < prefix.size()) {
                int base = 10;
                char base_char = std::tolower(static_cast<unsigned char>(prefix[i]));
                if (base_char == 'b') base = 2;
                else if (base_char == 'o') base = 8;
                else if (base_char == 'x') base = 16;
                else if (base_char == 'd') base = 10;

                int64_t value = 0;
                bool valid = true;
                for (char c : digits) {
                    if (c == '_') continue;
                    int d;
                    if (c >= '0' && c <= '9') d = c - '0';
                    else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
                    else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
                    else { valid = false; break; }
                    if (d >= base) { valid = false; break; }
                    value = value * base + d;
                }
                if (valid) {
                    auto tok = std::make_shared<Numeric_token>(std::to_string(value));
                    if (width > 0) {
                        tok->set_binary_size(width);
                        tok->set_sized_explicit(true);
                        if (signed_number) {
                            // Interpret as a two's-complement signed value of `width` bits.
                            auto v = value;
                            if (v & (int64_t(1) << (width - 1))) v -= (int64_t(1) << width);
                            tok = std::make_shared<Numeric_token>(std::to_string(v));
                        }
                    }
                    return tok;
                }
            }
        }
        spdlog::warn("Malformed VHDL bit-string literal '{}' in generic default, ignored", text);
        return nullptr;
    }

    // VHDL based literal, e.g. 16#FF# or 2#1010#.
    size_t hash_pos = text.find('#');
    if (hash_pos != std::string::npos && text.size() > hash_pos + 1 && text.back() == '#') {
        std::string base_str = text.substr(0, hash_pos);
        std::string digits = text.substr(hash_pos + 1, text.size() - hash_pos - 2);
        int base = 10;
        std::from_chars(base_str.data(), base_str.data() + base_str.size(), base, 10);
        // Reject base > 16 or invalid digits.
        if (base >= 2 && base <= 16 && !digits.empty()) {
            int64_t value = 0;
            for (char c : digits) {
                if (c == '_') continue;
                int d = 0;
                if (c >= '0' && c <= '9') d = c - '0';
                else if (c >= 'a' && c <= 'f') d = c - 'a' + 10;
                else if (c >= 'A' && c <= 'F') d = c - 'A' + 10;
                else { d = -1; break; }
                if (d >= base) { d = -1; break; }
                value = value * base + d;
            }
            auto tok = std::make_shared<Numeric_token>(std::to_string(value));
            return tok;
        }
    }

    // Real literal (contains '.', or exponent without a base prefix).
    bool is_real = text.find('.') != std::string::npos;
    if (!is_real) {
        // exponent like 1e3 or 1E-3
        size_t epos = text.find_first_of("eE");
        is_real = epos != std::string::npos;
    }
    if (is_real) {
        std::string cleaned = text;
        cleaned.erase(std::remove(cleaned.begin(), cleaned.end(), '_'), cleaned.end());
        return std::make_shared<Real_token>(cleaned);
    }

    // Plain integer (underscores are handled by Numeric_token).
    return std::make_shared<Numeric_token>(text);
}

void vhdl_visitor::attach_default_architectures() {
    // The entity resource (architecture "") is the default implementation: it
    // carries the last-declared architecture's statements and typedefs.
    for (auto &item : entities) {
        if (!item->is<hdl_resource_statement>()) continue;
        auto &res = item->as<hdl_resource_statement>();
        if (!res.get_architecture().empty()) continue;
        auto it = last_arch_of.find(res.getName());
        if (it == last_arch_of.end()) continue;
        for (auto &a : entities) {
            if (!a->is<hdl_resource_statement>()) continue;
            auto &arch = a->as<hdl_resource_statement>();
            if (arch.getName() != res.getName() || arch.get_architecture() != it->second) continue;
            for (auto &stmt : arch.get_statements()) res.add_statement(stmt);
            for (auto &[tname, ttype] : arch.get_typedefs()) res.add_typedef(tname, ttype);
            break;
        }
    }
}

void vhdl_visitor::resolve_positional_maps() {
    for (auto &e : entities) {
        if (!e->is<hdl_resource_statement>()) continue;
        for (auto &stmt : e->as<hdl_resource_statement>().get_statements())
            resolve_positional_in_statement(stmt);
    }
}

void vhdl_visitor::resolve_positional_in_statement(const std::shared_ptr<hdl_statement_base> &stmt) {
    if (auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(stmt)) {
        resolve_positional_instance(inst);
    } else if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmt)) {
        for (auto &s : loop->get_body()) resolve_positional_in_statement(s);
    } else if (auto cond = std::dynamic_pointer_cast<hdl_conditional_statement>(stmt)) {
        for (auto &b : cond->get_branches())
            for (auto &s : b.body) resolve_positional_in_statement(s);
        for (auto &s : cond->get_else_body()) resolve_positional_in_statement(s);
    }
}

void vhdl_visitor::resolve_positional_instance(const std::shared_ptr<hdl_instance_statement> &inst) {
    auto type = inst->get_type();

    auto pit = entity_port_order.find(type);
    if (pit != entity_port_order.end()) {
        auto old_ports = inst->get_ports();
        std::unordered_map<std::string, std::vector<HDL_net>> new_ports;
        bool changed = false;
        for (auto &[k, v] : old_ports) {
            if (k.rfind("@port", 0) == 0) {
                int idx = std::stoi(k.substr(5));
                if (idx < static_cast<int>(pit->second.size())) {
                    new_ports[pit->second[idx]] = v;
                    changed = true;
                } else {
                    spdlog::warn("VHDL positional port map index {} out of range for '{}', dropped", idx, type);
                }
            } else {
                new_ports[k] = v;
            }
        }
        if (changed) inst->set_ports(new_ports);
    }

    auto git = entity_generic_order.find(type);
    if (git != entity_generic_order.end()) {
        Parameters_map new_params = inst->get_parameters();
        std::vector<std::pair<std::string, int>> placeholders;
        for (auto &[k, v] : new_params)
            if (k.rfind("@gen", 0) == 0) placeholders.push_back({k, std::stoi(k.substr(4))});
        bool changed = false;
        for (auto &[k, idx] : placeholders) {
            auto param = new_params.const_get(k);
            new_params.erase(k);
            if (idx < static_cast<int>(git->second.size())) {
                auto renamed = std::make_shared<HDL_parameter>(*param);
                renamed->set_name(git->second[idx]);
                new_params.insert(renamed);
                changed = true;
            } else {
                spdlog::warn("VHDL positional generic map index {} out of range for '{}', dropped", idx, type);
            }
        }
        if (changed) inst->set_parameters(new_params);
    }
}
