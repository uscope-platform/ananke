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

#include "frontend/analysis/system_verilog/sv_parsing_helpers.hpp"
#include "frontend/analysis/system_verilog/sv_visitor.hpp"

using sv_parsing_helpers::enclosing_scopes;

namespace {

unsigned int line_of(antlr4::ParserRuleContext *ctx) { return static_cast<unsigned int>(ctx->getStart()->getLine()); }

// `[inner]` -> `inner`
std::string inner_text(antlr4::ParserRuleContext *ctx) {
    auto text = ctx->getText();
    if (text.size() >= 2 && text.front() == '[' && text.back() == ']') return text.substr(1, text.size() - 2);
    return text;
}

bool has_queue_dimension(const std::string &text) { return text.starts_with("[$") || text == "[]"; }

// Zero replications are only legal next to other operands of a concatenation.
bool zero_replication_allowed(antlr4::ParserRuleContext *ctx) {
    for (auto *p = ctx->parent; p; p = p->parent) {
        if (p->getTreeType() != antlr4::tree::ParseTreeType::RULE) return false;
        auto *rule = static_cast<antlr4::ParserRuleContext *>(p);
        switch (rule->getRuleIndex()) {
        case sv2017::RulePrimary: case sv2017::RuleExpression: case sv2017::RuleConstant_expression:
        case sv2017::RuleMintypmax_expression: case sv2017::RuleParam_expression:
        case sv2017::RuleConstant_param_expression:
            continue;
        case sv2017::RuleConcatenation:
            return static_cast<sv2017::ConcatenationContext *>(rule)->expression().size() > 1;
        default:
            return false;
        }
    }
    return false;
}

// Inside an if/case generate: a width that is zero or negative may belong to a branch that is never elaborated.
bool in_conditional_generate(antlr4::ParserRuleContext *ctx) {
    for (auto *p = ctx->parent; p; p = p->parent) {
        if (p->getTreeType() != antlr4::tree::ParseTreeType::RULE) continue;
        const auto rule = static_cast<antlr4::ParserRuleContext *>(p)->getRuleIndex();
        if (rule == sv2017::RuleIf_generate_construct || rule == sv2017::RuleCase_generate_construct) return true;
    }
    return false;
}

// A `[..]` that the parser read as a dimension inside an expression is really an index (`$display(v[i])`
// parses its argument as a data type first).
bool inside_expression(antlr4::ParserRuleContext *ctx) {
    for (auto *p = ctx->parent; p; p = p->parent) {
        if (p->getTreeType() == antlr4::tree::ParseTreeType::RULE &&
            static_cast<antlr4::ParserRuleContext *>(p)->getRuleIndex() == sv2017::RulePrimary) {
            return true;
        }
    }
    return false;
}

}  // namespace

void sv_visitor::check_constant_context(antlr4::ParserRuleContext *ctx) {
    auto fired = [this](bool violation) { if (violation) had_error = true; };
    const bool judge_values = !in_conditional_generate(ctx);
    switch (ctx->getRuleIndex()) {
    case sv2017::RuleParam_assignment: {
        auto *pa = static_cast<sv2017::Param_assignmentContext *>(ctx);
        if (!pa->identifier()) break;
        const auto chain = enclosing_scopes(ctx);
        std::string value;
        if (pa->constant_param_expression() && pa->unpacked_dimension().empty()) {
            value = pa->constant_param_expression()->getText();
        }
        bool is_real = false;
        bool is_local = false;
        int hops = 0;
        for (auto *p = ctx->parent; p && hops < 4; p = p->parent, ++hops) {
            if (p->getTreeType() != antlr4::tree::ParseTreeType::RULE) break;
            auto *rule = static_cast<antlr4::ParserRuleContext *>(p);
            antlr4::ParserRuleContext *type = nullptr;
            if (rule->getRuleIndex() == sv2017::RuleParameter_declaration) {
                type = static_cast<sv2017::Parameter_declarationContext *>(rule)->data_type_or_implicit();
            } else if (rule->getRuleIndex() == sv2017::RuleLocal_parameter_declaration) {
                is_local = true;
                type = static_cast<sv2017::Local_parameter_declarationContext *>(rule)->data_type_or_implicit();
            } else if (rule->getRuleIndex() == sv2017::RuleParameter_port_declaration) {
                type = static_cast<sv2017::Parameter_port_declarationContext *>(rule)->data_type();
                if (!type) continue;  // `localparam`/`parameter` declaration nested inside
            } else {
                continue;
            }
            if (type) is_real = type->getText().find("real") != std::string::npos;
            break;
        }
        // A `parameter` of a module, interface, program or class can be overridden at instantiation, so its
        // default is not its value. Only localparams and package/unit parameters are final.
        bool definitive = is_local;
        if (!definitive && !chain.empty()) {
            auto *scope = static_cast<antlr4::tree::ParseTree *>(const_cast<void *>(chain.front()));
            definitive = scope->getTreeType() == antlr4::tree::ParseTreeType::RULE &&
                         (static_cast<antlr4::ParserRuleContext *>(scope)->getRuleIndex() == sv2017::RulePackage_declaration ||
                          scope->parent == nullptr);
        }
        if (!value.empty()) {
            fired(semantic_checker.check_constant_use(current_file, line_of(ctx), value, chain, "a parameter value"));
        }
        if (!definitive) value.clear();
        semantic_checker.declare_constant_name(chain, pa->identifier()->getText(), value, is_real);
        break;
    }
    case sv2017::RuleGenvar_declaration: {
        auto *g = static_cast<sv2017::Genvar_declarationContext *>(ctx);
        const auto chain = enclosing_scopes(ctx);
        if (g->identifier_list()) {
            for (auto *id : g->identifier_list()->identifier()) semantic_checker.declare_constant_name(chain, id->getText(), "", false);
        }
        break;
    }
    case sv2017::RuleGenvar_initialization: {
        auto *g = static_cast<sv2017::Genvar_initializationContext *>(ctx);
        if (g->identifier()) semantic_checker.declare_constant_name(enclosing_scopes(ctx), g->identifier()->getText(), "", false);
        break;
    }
    case sv2017::RuleEnum_name_declaration: {
        auto *e = static_cast<sv2017::Enum_name_declarationContext *>(ctx);
        if (e->identifier()) semantic_checker.declare_constant_name(enclosing_scopes(ctx), e->identifier()->getText(), "", false);
        break;
    }
    case sv2017::RuleAttr_spec: {
        auto *a = static_cast<sv2017::Attr_specContext *>(ctx);
        // `clocking_decl_assign` reuses attr_spec for `output sig = expr;`: only real attributes `(* name = value *)`.
        const bool is_attribute = ctx->parent && ctx->parent->getTreeType() == antlr4::tree::ParseTreeType::RULE &&
                                  static_cast<antlr4::ParserRuleContext *>(ctx->parent)->getRuleIndex() == sv2017::RuleAttribute_instance;
        if (is_attribute && a->expression()) {
            fired(semantic_checker.check_constant_use(
                current_file, line_of(ctx), a->expression()->getText(), enclosing_scopes(ctx), "an attribute value"));
        }
        break;
    }
    case sv2017::RulePacked_dimension:
        if (inside_expression(ctx)) break;
        fired(semantic_checker.check_dimension(current_file, line_of(ctx), inner_text(ctx), true, enclosing_scopes(ctx), judge_values));
        break;
    case sv2017::RuleUnpacked_dimension:
        if (inside_expression(ctx)) break;
        fired(semantic_checker.check_dimension(current_file, line_of(ctx), inner_text(ctx), false, enclosing_scopes(ctx), judge_values));
        break;
    case sv2017::RuleVariable_dimension: {
        if (inside_expression(ctx)) break;
        // Dimensions written on a data type are packed; on a declarator they are unpacked.
        bool packed = false;
        if (ctx->parent && ctx->parent->getTreeType() == antlr4::tree::ParseTreeType::RULE) {
            auto *parent = static_cast<antlr4::ParserRuleContext *>(ctx->parent);
            if (parent->getRuleIndex() == sv2017::RuleEnum_base_type) packed = true;
            else if (parent->getRuleIndex() == sv2017::RuleData_type) {
                auto *primitive = static_cast<sv2017::Data_typeContext *>(parent)->data_type_primitive();
                packed = primitive && primitive->integer_type();
            }
        }
        fired(semantic_checker.check_dimension(current_file, line_of(ctx), inner_text(ctx), packed, enclosing_scopes(ctx), judge_values));
        break;
    }
    case sv2017::RuleArray_range_expression: {
        auto *r = static_cast<sv2017::Array_range_expressionContext *>(ctx);
        auto *parent = ctx->parent;
        if (r->expression().size() != 2 || !r->range_separator() || !parent ||
            parent->getTreeType() != antlr4::tree::ParseTreeType::RULE ||
            static_cast<antlr4::ParserRuleContext *>(parent)->getRuleIndex() != sv2017::RulePrimary ||
            parent->children.size() != 4) {
            break;
        }
        fired(semantic_checker.check_part_select(
            current_file, line_of(ctx), parent->children[0]->getText(), r->range_separator()->getText(),
            r->expression(0)->getText(), r->expression(1)->getText(), enclosing_scopes(ctx), judge_values));
        break;
    }
    case sv2017::RulePackage_or_class_scoped_hier_id_with_select: {
        // l-value part-select: `name[a:b]`, `name[a+:w]`
        auto *s = static_cast<sv2017::Package_or_class_scoped_hier_id_with_selectContext *>(ctx);
        if (!s->COLON() || s->expression().size() != 2 || !s->package_or_class_scoped_path()) break;
        const std::string separator = (s->operator_plus_minus() ? s->operator_plus_minus()->getText() : "") + ":";
        fired(semantic_checker.check_part_select(
            current_file, line_of(ctx), s->package_or_class_scoped_path()->getText(), separator,
            s->expression(0)->getText(), s->expression(1)->getText(), enclosing_scopes(ctx), judge_values));
        break;
    }
    case sv2017::RuleReplication: {
        auto *r = static_cast<sv2017::ReplicationContext *>(ctx);
        if (!r->replication_size()) break;
        fired(semantic_checker.check_replication(
            current_file, line_of(ctx), r->replication_size()->getText(), zero_replication_allowed(ctx),
            enclosing_scopes(ctx), judge_values));
        break;
    }
    case sv2017::RulePrimary:
        // `N'(expr)`: primary cast_separator expression ')'
        if (ctx->children.size() == 4 && ctx->children[1]->getTreeType() == antlr4::tree::ParseTreeType::RULE &&
            static_cast<antlr4::ParserRuleContext *>(ctx->children[1])->getRuleIndex() == sv2017::RuleCast_separator) {
            fired(semantic_checker.check_size_cast(
                current_file, line_of(ctx), ctx->children[0]->getText(), enclosing_scopes(ctx), judge_values));
        }
        break;
    case sv2017::RuleConcatenation: {
        // `{1.0, 2.0}` is a queue/array literal in assignments, so only parameter values are judged.
        bool in_parameter = false;
        for (auto *p = ctx->parent; p && !in_parameter; p = p->parent) {
            in_parameter = p->getTreeType() == antlr4::tree::ParseTreeType::RULE &&
                           static_cast<antlr4::ParserRuleContext *>(p)->getRuleIndex() == sv2017::RuleParam_assignment;
        }
        if (!in_parameter) break;
        auto *c = static_cast<sv2017::ConcatenationContext *>(ctx);
        std::vector<std::string> operands;
        for (auto *e : c->expression()) operands.push_back(e->getText());
        if (!operands.empty()) {
            fired(semantic_checker.check_concatenation_operands(current_file, line_of(ctx), operands, enclosing_scopes(ctx)));
        }
        break;
    }
    case sv2017::RuleContinuous_assign: {
        auto *a = static_cast<sv2017::Continuous_assignContext *>(ctx);
        if (!a->list_of_variable_assignments()) break;
        const auto chain = enclosing_scopes(ctx);
        for (auto *v : a->list_of_variable_assignments()->variable_assignment()) {
            if (v->variable_lvalue()) {
                fired(semantic_checker.check_net_lvalue(current_file, line_of(v), v->variable_lvalue()->getText(), chain));
            }
        }
        break;
    }
    // ---- name tables ----
    case sv2017::RuleData_declaration: {
        auto *d = static_cast<sv2017::Data_declarationContext *>(ctx);
        auto *list = d->list_of_variable_decl_assignments();
        if (!list) break;
        const auto chain = enclosing_scopes(ctx);
        auto *dti = d->data_type_or_implicit();
        auto *dt = dti ? dti->data_type() : nullptr;
        auto *primitive = dt ? dt->data_type_primitive() : nullptr;
        const bool integral = !dt || (primitive && primitive->integer_type());
        for (auto *decl : list->variable_decl_assignment()) {
            if (!decl->identifier()) continue;
            bool queue = false;
            for (auto *dim : decl->variable_dimension()) queue = queue || has_queue_dimension(dim->getText()) || dim->getText().find("string") != std::string::npos;
            if (d->KW_CONST()) semantic_checker.declare_constant_name(chain, decl->identifier()->getText(), "", false);
            else semantic_checker.declare_variable_name(chain, decl->identifier()->getText(), integral && !queue);
        }
        break;
    }
    case sv2017::RuleNet_declaration: {
        auto *n = static_cast<sv2017::Net_declarationContext *>(ctx);
        const auto chain = enclosing_scopes(ctx);
        if (n->list_of_net_decl_assignments()) {
            for (auto *a : n->list_of_net_decl_assignments()->net_decl_assignment()) {
                if (!a->identifier()) continue;
                bool queue = false;
                for (auto *dim : a->unpacked_dimension()) queue = queue || has_queue_dimension(dim->getText());
                semantic_checker.declare_variable_name(chain, a->identifier()->getText(), !queue);
            }
        } else {
            for (auto *id : n->identifier()) semantic_checker.declare_variable_name(chain, id->getText(), false);
        }
        break;
    }
    case sv2017::RuleAnsi_port_declaration: {
        auto *p = static_cast<sv2017::Ansi_port_declarationContext *>(ctx);
        if (!p->port_identifier() || !p->identifier().empty()) break;  // interface port / explicit `.name(expr)`
        bool queue = false;
        for (auto *dim : p->variable_dimension()) queue = queue || has_queue_dimension(dim->getText());
        semantic_checker.declare_variable_name(enclosing_scopes(ctx), p->port_identifier()->getText(), !queue);
        break;
    }
    case sv2017::RuleList_of_variable_identifiers_item: {
        auto *i = static_cast<sv2017::List_of_variable_identifiers_itemContext *>(ctx);
        if (i->identifier()) semantic_checker.declare_variable_name(enclosing_scopes(ctx), i->identifier()->getText(), true);
        break;
    }
    case sv2017::RuleList_of_tf_variable_identifiers_item: {
        auto *i = static_cast<sv2017::List_of_tf_variable_identifiers_itemContext *>(ctx);
        if (i->identifier()) semantic_checker.declare_variable_name(enclosing_scopes(ctx), i->identifier()->getText(), false);
        break;
    }
    case sv2017::RuleTf_port_item: {
        // Ports of a task or function body only: DPI imports and extern/pure prototypes have no body scope,
        // and their formals must not leak into the enclosing module or class.
        auto *i = static_cast<sv2017::Tf_port_itemContext *>(ctx);
        const auto chain = enclosing_scopes(ctx);
        if (!i->identifier() || chain.empty()) break;
        auto *scope = static_cast<antlr4::tree::ParseTree *>(const_cast<void *>(chain.front()));
        if (scope->getTreeType() != antlr4::tree::ParseTreeType::RULE) break;
        const auto rule = static_cast<antlr4::ParserRuleContext *>(scope)->getRuleIndex();
        if (rule != sv2017::RuleTask_declaration && rule != sv2017::RuleFunction_declaration) break;
        // A prototype nested in a task/function declaration cannot occur, but a DPI import sits in the module.
        semantic_checker.declare_variable_name(chain, i->identifier()->getText(), false);
        break;
    }
    default:
        break;
    }
}
