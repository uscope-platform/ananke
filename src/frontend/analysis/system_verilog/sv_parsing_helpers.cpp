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

#include <map>
#include <optional>
#include <regex>
#include <set>

namespace {

    bool is_hex_digit(const char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    // Helper: UNSIGNED_NUMBER per lexer: DECIMAL_DIGIT (UNDERSCORE|DECIMAL_DIGIT)*
    // i.e. first char must be a digit, rest digits or '_', with at least one digit.
    bool is_unsigned_number_text(const std::string_view &s) {
        if (s.empty() || s[0] < '0' || s[0] > '9') return false;
        for (char c : s) {
            if ((c < '0' || c > '9') && c != '_') return false;
        }
        return true;
    }

    // \d+(\.\d+)? with '_' separators allowed
    bool is_time_number(const std::string_view &s) {
        if (s.empty()) return false;
        size_t int_end = s.find_first_not_of("0123456789_");
        if (int_end == std::string_view::npos) return is_unsigned_number_text(s);
        if (int_end == 0 || s[int_end] != '.') return false;
        auto int_part = s.substr(0, int_end);
        if (!is_unsigned_number_text(int_part)) return false;
        auto frac = s.substr(int_end + 1);
        if (frac.empty()) return false;
        return is_unsigned_number_text(frac);
    }

    // \d+(\.\d+)?(s|ms|us|ns|ps|fs)
    bool is_time_literal(const std::string_view &s) {
        static const std::string_view units[] = {"fs", "ps", "ns", "us", "ms", "s"};
        for (const auto &unit : units) {
            if (s.size() > unit.size() && s.ends_with(unit))
                return is_time_number(s.substr(0, s.size() - unit.size()));
        }
        return false;
    }

    // ^[+\-]?(\d+\.\d*|\.\d+)([eE][+\-]?\d+)?$ | ^[+\-]?\d+[eE][+\-]?\d+$ with '_' separators
    bool is_real_literal(const std::string_view &s) {
        size_t i = 0;
        if (i < s.size() && (s[i] == '+' || s[i] == '-')) i++;
        if (i >= s.size()) return false;

        size_t int_end = s.find_first_not_of("0123456789_", i);
        bool has_int = int_end != i;
        if (has_int && !is_unsigned_number_text(s.substr(i, int_end == std::string_view::npos ? int_end : int_end - i))) return false;
        if (int_end == std::string_view::npos) return false;

        if (s[int_end] == '.') {
            size_t frac_start = int_end + 1;
            size_t frac_end = s.find_first_not_of("0123456789_", frac_start);
            size_t frac_len = (frac_end == std::string_view::npos ? s.size() : frac_end) - frac_start;
            bool has_frac = frac_len != 0;
            if (has_frac && !is_unsigned_number_text(s.substr(frac_start, frac_len))) return false;
            if (!has_int) {
                // ".\d+" form: need at least one fractional digit
                if (!has_frac) return false;
            }
            if (frac_end == std::string_view::npos) return has_int || has_frac;
            if (s[frac_end] != 'e' && s[frac_end] != 'E') return false;
            int_end = frac_end;
        } else if (s[int_end] == 'e' || s[int_end] == 'E') {
            if (!has_int) return false;
        } else if (s[int_end] == '_') {
            return false;
        } else {
            return false;
        }

        size_t exp_digits = int_end + 1;
        if (exp_digits < s.size() && (s[exp_digits] == '+' || s[exp_digits] == '-')) exp_digits++;
        if (exp_digits >= s.size()) return false;
        return is_unsigned_number_text(s.substr(exp_digits));
    }

    // ^\d[\d_]*$ (UNSIGNED_NUMBER per lexer, '_' separators allowed)
    bool is_unsigned_integer(const std::string_view &s) {
        return is_unsigned_number_text(s);
    }

    bool is_unbased_unsized_literal(const std::string_view &s) {
        return s == "'0" || s == "'1" || s == "'x" || s == "'X" ||
               s == "'z" || s == "'Z" || s == "'?";
    }

    static bool is_based_digit(char c, char base) {
        if (c == '_' || c == '?') return true;
        if (c == 'x' || c == 'X' || c == 'z' || c == 'Z') return true;
        switch (base) {
            case 'b': case 'B': return c == '0' || c == '1';
            case 'o': case 'O': return c >= '0' && c <= '7';
            case 'd': case 'D': return c >= '0' && c <= '9';
            case 'h': case 'H': return is_hex_digit(c);
            default: return false;
        }
    }

    // ^\d[\d_]*'(s|S)?(h|d|o|b|H|D|O|B)([0-9a-fA-FxXzZ?_]+)  (prefix match)
    // NOTE: the lexer allows whitespace after the apostrophe, around the
    // optional sign flag and after the base (e.g. reggen emits `1'h 0`),
    // so whitespace is ignored for classification. Identifiers never
    // contain whitespace, so stripping cannot misclassify them.
    bool is_sized_literal(const std::string_view &s) {
        std::string stripped;
        std::string_view view = s;
        if (s.find_first_of(" \t\n\r\f\v") != std::string_view::npos) {
            stripped.reserve(s.size());
            for (char c : s) {
                if (c != ' ' && c != '\t' && c != '\n' && c != '\r' && c != '\f' && c != '\v')
                    stripped.push_back(c);
            }
            view = stripped;
        }
        size_t int_end = view.find_first_not_of("0123456789_");
        if (int_end == std::string_view::npos || int_end >= view.size() || view[int_end] != '\'') return false;
        if (int_end != 0 && !is_unsigned_number_text(view.substr(0, int_end))) return false;
        size_t i = int_end + 1;
        if (i < view.size() && (view[i] == 's' || view[i] == 'S')) i++;
        if (i >= view.size()) return false;
        char base = view[i];
        if (base != 'h' && base != 'H' && base != 'd' && base != 'D' &&
            base != 'o' && base != 'O' && base != 'b' && base != 'B') return false;
        if (i + 1 >= view.size()) return false;
        // At least one valid digit, rest must all be valid digits/'_' (prefix match
        // requires the first digit char to be valid; trailing chars were previously
        // unchecked, keep prefix semantics but reject obvious identifiers).
        if (!is_based_digit(view[i + 1], base)) return false;
        return true;
    }

}

namespace sv_parsing_helpers {

    qualified_identifier parse_qualified_identifier(mgp_sv::sv2017::Package_or_class_scoped_pathContext *ctx) {
        auto items = ctx->package_or_class_scoped_path_item();
        auto d_colon = ctx->DOUBLE_COLON();

        if (d_colon.empty()) {
            return qualified_identifier(ctx->getText());
        }

        std::vector<std::string> prefix;
        std::string name;

        unsigned int first_item = 0;

        if (ctx->KW_DOLAR_UNIT()) {
            prefix.push_back("$unit");
            first_item = 0;
        } else if (ctx->KW_DOLAR_ROOT()) {
            prefix.push_back("$root");
            first_item = 0;
        } else if (ctx->implicit_class_handle()) {
            prefix.push_back(ctx->implicit_class_handle()->getText());
            first_item = 0;
        }

        unsigned int n_segments = items.size();
        for (unsigned int i = first_item; i < n_segments - 1; ++i) {
            prefix.push_back(items[i]->identifier()->getText());
        }
        name = items[n_segments - 1]->identifier()->getText();

        qualified_identifier qi(name);
        if (!prefix.empty()) {
            qi.set_package_prefix(prefix);
        }
        return qi;
    }

    std::shared_ptr<Expression_base> make_value(const std::string &s) {
        if (is_time_literal(s)) {
            return std::make_shared<Time_token>(s);
        }
        if (is_real_literal(s)) {
            return std::make_shared<Real_token>(s);
        }
        if (is_unbased_unsized_literal(s)) {
            return std::make_shared<Numeric_token>(s);
        }
        if (is_unsigned_integer(s) || is_sized_literal(s)) {
            return std::make_shared<Numeric_token>(s);
        }
        if (s.starts_with("\"")) {
            return std::make_shared<String_token>(s);
        }
        return std::make_shared<Identifier_token>(qualified_identifier(s));
    }

}

namespace {

using namespace mgp_sv;

struct type_info {
    bool explicit_type = false;
    bool vector_type = true;
    std::string range;
};

std::string join_dimensions(const std::vector<sv2017::Packed_dimensionContext *> &dims) {
    std::string out;
    for (auto *d : dims) out += d->getText();
    return out;
}

std::string join_dimensions(const std::vector<sv2017::Variable_dimensionContext *> &dims) {
    std::string out;
    for (auto *d : dims) out += d->getText();
    return out;
}

// `integer`, `real`, enums, structs, named types and strings take no
// packed range; only net/reg/logic/bit (or no type at all) do.
type_info describe_type(sv2017::Data_type_or_implicitContext *dti) {
    type_info info;
    if (!dti) return info;
    if (auto *implicit = dti->implicit_data_type()) {
        info.range = join_dimensions(implicit->packed_dimension());
        return info;
    }
    auto *type = dti->data_type();
    if (!type) return info;
    info.explicit_type = true;
    auto *primitive = type->data_type_primitive();
    info.vector_type = primitive && primitive->integer_type() && primitive->integer_type()->integer_vector_type();
    if (info.vector_type) info.range = join_dimensions(type->variable_dimension());
    return info;
}

void add_declaration(
    std::vector<scope_declaration> &out, sv2017::IdentifierContext *id, bool is_port, const type_info &type) {
    if (!id) return;
    scope_declaration d;
    d.name = id->getText();
    d.line = id->getStart()->getLine();
    d.is_port = is_port;
    d.explicit_type = is_port && type.explicit_type;
    d.vector_type = type.vector_type;
    d.range = type.range;
    out.push_back(std::move(d));
}

// A scope the declarations inside belong to someone else.
bool opens_nested_scope(antlr4::tree::ParseTree *node) {
    return dynamic_cast<sv2017::Task_declarationContext *>(node) ||
           dynamic_cast<sv2017::Function_declarationContext *>(node) ||
           dynamic_cast<sv2017::Class_declarationContext *>(node) ||
           dynamic_cast<sv2017::Module_declarationContext *>(node) ||
           dynamic_cast<sv2017::Interface_declarationContext *>(node) ||
           dynamic_cast<sv2017::Program_declarationContext *>(node) ||
           dynamic_cast<sv2017::Checker_declarationContext *>(node) ||
           dynamic_cast<sv2017::Seq_blockContext *>(node) ||
           dynamic_cast<sv2017::Par_blockContext *>(node) ||
           dynamic_cast<sv2017::Generate_begin_end_blockContext *>(node) ||
           dynamic_cast<sv2017::Loop_generate_constructContext *>(node) ||
           dynamic_cast<sv2017::Conditional_generate_constructContext *>(node) ||
           dynamic_cast<sv2017::Always_constructContext *>(node) ||
           dynamic_cast<sv2017::Initial_constructContext *>(node) ||
           dynamic_cast<sv2017::Final_constructContext *>(node) ||
           dynamic_cast<sv2017::Specify_blockContext *>(node) ||
           dynamic_cast<sv2017::Covergroup_declarationContext *>(node) ||
           dynamic_cast<sv2017::Rs_code_blockContext *>(node);
}

void collect(antlr4::tree::ParseTree *node, std::vector<scope_declaration> &out) {
    if (auto *port = dynamic_cast<sv2017::Nonansi_port_declarationContext *>(node)) {
        type_info type;
        if (auto *t = port->net_or_var_data_type()) {
            type = describe_type(t->data_type_or_implicit());
            if (t->net_type() || t->KW_VAR() || t->KW_INTERCONNECT()) type.explicit_type = true;
            if (auto *implicit = t->implicit_data_type()) type.range = join_dimensions(implicit->packed_dimension());
        } else if (auto *t = port->net_port_type()) {
            type = describe_type(t->data_type_or_implicit());
            if (t->net_type() || t->KW_INTERCONNECT()) type.explicit_type = true;
            if (auto *implicit = t->implicit_data_type()) type.range = join_dimensions(implicit->packed_dimension());
        } else if (port->var_data_type()) {
            type.explicit_type = true;
        } else if (port->KW_INPUT() == nullptr && port->KW_OUTPUT() == nullptr && port->KW_INOUT() == nullptr &&
                   port->KW_REF() == nullptr) {
            return; // interface port
        }
        if (auto *ids = port->list_of_variable_identifiers()) {
            for (auto *item : ids->list_of_variable_identifiers_item()) add_declaration(out, item->identifier(), true, type);
        } else if (auto *pids = port->list_of_variable_port_identifiers()) {
            for (auto *item : pids->list_of_tf_variable_identifiers()->list_of_tf_variable_identifiers_item()) {
                add_declaration(out, item->identifier(), true, type);
            }
        }
        return;
    }
    if (auto *tf = dynamic_cast<sv2017::Tf_port_declarationContext *>(node)) {
        auto type = describe_type(tf->data_type_or_implicit());
        if (tf->KW_VAR()) type.explicit_type = true;
        for (auto *item : tf->list_of_tf_variable_identifiers()->list_of_tf_variable_identifiers_item()) {
            add_declaration(out, item->identifier(), true, type);
        }
        return;
    }
    if (auto *net = dynamic_cast<sv2017::Net_declarationContext *>(node)) {
        if (net->KW_INTERCONNECT() || !net->list_of_net_decl_assignments()) return;
        type_info type;
        if (net->net_type()) type = describe_type(net->data_type_or_implicit());
        else type.vector_type = false; // user-defined nettype
        for (auto *a : net->list_of_net_decl_assignments()->net_decl_assignment()) {
            add_declaration(out, a->identifier(), false, type);
        }
        return;
    }
    if (auto *data = dynamic_cast<sv2017::Data_declarationContext *>(node)) {
        if (auto *list = data->list_of_variable_decl_assignments()) {
            auto type = describe_type(data->data_type_or_implicit());
            for (auto *a : list->variable_decl_assignment()) add_declaration(out, a->identifier(), false, type);
        }
        return;
    }
    for (auto *child : node->children) {
        if (!opens_nested_scope(child)) collect(child, out);
    }
}

} // namespace

namespace {

using namespace mgp_sv;

bool is_colon(antlr4::tree::ParseTree *n) {
    return n->getTreeType() == antlr4::tree::ParseTreeType::TERMINAL &&
           static_cast<antlr4::tree::TerminalNode *>(n)->getSymbol()->getType() == sv2017::COLON;
}

sv2017::IdentifierContext *as_identifier(antlr4::tree::ParseTree *n) {
    if (n->getTreeType() != antlr4::tree::ParseTreeType::RULE) return nullptr;
    auto *rule = static_cast<antlr4::ParserRuleContext *>(n);
    return rule->getRuleIndex() == sv2017::RuleIdentifier ? static_cast<sv2017::IdentifierContext *>(rule) : nullptr;
}

std::string text_of(sv2017::IdentifierContext *id) { return id ? id->getText() : std::string(); }

// First `identifier` that is a direct child: the declared name for rules whose name is not in a header rule.
std::string first_direct_identifier(antlr4::ParserRuleContext *ctx) {
    for (auto *child : ctx->children) {
        if (auto *id = as_identifier(child)) return id->getText();
    }
    return {};
}

// `... KW_END* : label` at the tail of the rule.
void read_tail_label(antlr4::ParserRuleContext *ctx, end_label_info &info) {
    const auto n = ctx->children.size();
    if (n < 3) return;
    auto *id = as_identifier(ctx->children[n - 1]);
    if (id && is_colon(ctx->children[n - 2])) {
        info.has_end = true;
        info.end = id->getText();
        info.line = id->getStart()->getLine();
    }
}

// begin / fork: `KW : name` right after the opening keyword.
std::string label_after_first(antlr4::ParserRuleContext *ctx, size_t keyword_index) {
    if (ctx->children.size() > keyword_index + 2 && is_colon(ctx->children[keyword_index + 1])) {
        return text_of(as_identifier(ctx->children[keyword_index + 2]));
    }
    return {};
}

}  // namespace

namespace {

// Unqualified named type of a data_type (`T` in `T x`), empty otherwise.
std::string plain_type_name(sv2017::Data_typeContext *dt) {
    if (!dt) return {};
    auto *path = dt->package_or_class_scoped_path();
    if (!path) return {};
    auto text = path->getText();
    return text.find("::") == std::string::npos && text.find('#') == std::string::npos &&
                   text.find('.') == std::string::npos ? text : std::string();
}

typedef_shape::kind_t dimension_kind(const std::vector<sv2017::Variable_dimensionContext *> &dims) {
    using kind_t = typedef_shape::kind_t;
    kind_t kind = kind_t::other;
    for (auto *d : dims) {
        const auto text = d->getText();
        if (text == "[]") return kind_t::dynamic_array;
        if (text.starts_with("[$")) return kind_t::queue;
        kind = kind_t::unpacked_array;
    }
    return kind;
}

std::optional<int> literal_range_width(const std::string &text) {
    static const std::regex range(R"(\[([0-9]+):([0-9]+)\])");
    std::smatch m;
    if (!std::regex_match(text, m, range)) return std::nullopt;
    const long a = std::stol(m[1]), b = std::stol(m[2]);
    return static_cast<int>((a > b ? a - b : b - a) + 1);
}

}  // namespace

namespace {

void collect_locals(antlr4::tree::ParseTree *node, std::set<std::string> &locals) {
    if (node->getTreeType() != antlr4::tree::ParseTreeType::RULE) return;
    auto *ctx = static_cast<antlr4::ParserRuleContext *>(node);
    switch (ctx->getRuleIndex()) {
    case sv2017::RuleData_declaration: {
        auto *d = static_cast<sv2017::Data_declarationContext *>(ctx);
        // An explicit `static` local is not automatic.
        if (d->lifetime() && d->lifetime()->KW_STATIC()) return;
        if (auto *list = d->list_of_variable_decl_assignments()) {
            for (auto *a : list->variable_decl_assignment()) if (a->identifier()) locals.insert(a->identifier()->getText());
        }
        return;
    }
    case sv2017::RuleTf_port_item: {
        auto *p = static_cast<sv2017::Tf_port_itemContext *>(ctx);
        if (p->identifier()) locals.insert(p->identifier()->getText());
        return;
    }
    case sv2017::RuleList_of_tf_variable_identifiers_item: {
        auto *p = static_cast<sv2017::List_of_tf_variable_identifiers_itemContext *>(ctx);
        if (p->identifier()) locals.insert(p->identifier()->getText());
        return;
    }
    case sv2017::RuleTask_declaration:
    case sv2017::RuleFunction_declaration:
        // handled by the caller for the root; nested tasks are separate scopes
        break;
    default:
        break;
    }
    for (auto *child : node->children) collect_locals(child, locals);
}

// Any identifier in the subtree that is one of `locals`.
std::string first_local_in(antlr4::tree::ParseTree *node, const std::set<std::string> &locals) {
    if (node->getTreeType() != antlr4::tree::ParseTreeType::RULE) return {};
    auto *ctx = static_cast<antlr4::ParserRuleContext *>(node);
    if (ctx->getRuleIndex() == sv2017::RuleIdentifier) {
        auto text = ctx->getText();
        return locals.contains(text) ? text : std::string();
    }
    for (auto *child : node->children) {
        if (auto found = first_local_in(child, locals); !found.empty()) return found;
    }
    return {};
}

void find_misuse(antlr4::tree::ParseTree *node, const std::set<std::string> &locals, std::vector<automatic_misuse> &out) {
    if (node->getTreeType() != antlr4::tree::ParseTreeType::RULE) return;
    auto *ctx = static_cast<antlr4::ParserRuleContext *>(node);
    switch (ctx->getRuleIndex()) {
    case sv2017::RuleNonblocking_assignment: {
        auto *nb = static_cast<sv2017::Nonblocking_assignmentContext *>(ctx);
        // The written variable is the first identifier of the l-value (`s.x`, `a[i]` write `s`, `a`).
        if (nb->variable_lvalue()) {
            if (auto name = first_local_in(nb->variable_lvalue()->children.empty() ? node : nb->variable_lvalue()->children[0], locals);
                !name.empty()) {
                out.push_back({name, "written by a nonblocking assignment", static_cast<unsigned int>(ctx->getStart()->getLine())});
                return;
            }
        }
        if (nb->delay_or_event_control()) {
            if (auto name = first_local_in(nb->delay_or_event_control(), locals); !name.empty()) {
                out.push_back({name, "used in the event control of a nonblocking assignment", static_cast<unsigned int>(ctx->getStart()->getLine())});
                return;
            }
        }
        break;
    }
    case sv2017::RuleProcedural_continuous_assignment: {
        auto *pc = static_cast<sv2017::Procedural_continuous_assignmentContext *>(ctx);
        if (pc->variable_assignment()) {
            if (auto name = first_local_in(pc->variable_assignment(), locals); !name.empty()) {
                out.push_back({name, "used in a procedural continuous assignment", static_cast<unsigned int>(ctx->getStart()->getLine())});
                return;
            }
        }
        break;
    }
    case sv2017::RuleTask_declaration:
    case sv2017::RuleFunction_declaration:
        return; // own scope (the root is passed to the children directly)
    default:
        break;
    }
    for (auto *child : node->children) find_misuse(child, locals, out);
}

}  // namespace

namespace sv_parsing_helpers {
    std::vector<const void *> enclosing_scopes(antlr4::tree::ParseTree *node) {
        std::vector<const void *> scopes;
        for (auto *p = node->parent; p; p = p->parent) {
            if (p->getTreeType() != antlr4::tree::ParseTreeType::RULE) continue;
            switch (static_cast<antlr4::ParserRuleContext *>(p)->getRuleIndex()) {
            case sv2017::RuleModule_declaration: case sv2017::RuleInterface_declaration:
            case sv2017::RuleProgram_declaration: case sv2017::RulePackage_declaration:
            case sv2017::RuleClass_declaration: case sv2017::RuleTask_declaration:
            case sv2017::RuleFunction_declaration: case sv2017::RuleSeq_block: case sv2017::RulePar_block:
            case sv2017::RuleGenerate_begin_end_block: case sv2017::RuleChecker_declaration:
                scopes.push_back(p);
                break;
            default:
                if (p->parent == nullptr) scopes.push_back(p);
            }
        }
        return scopes;
    }

    std::vector<automatic_misuse> find_automatic_misuse(antlr4::ParserRuleContext *scope) {
        std::vector<automatic_misuse> out;
        bool automatic = false;
        if (scope->getRuleIndex() == sv2017::RuleTask_declaration) {
            auto *l = static_cast<sv2017::Task_declarationContext *>(scope)->lifetime();
            automatic = l && l->KW_AUTOMATIC();
        } else if (scope->getRuleIndex() == sv2017::RuleFunction_declaration) {
            auto *l = static_cast<sv2017::Function_declarationContext *>(scope)->lifetime();
            automatic = l && l->KW_AUTOMATIC();
        }
        if (!automatic) return out;
        std::set<std::string> locals;
        for (auto *child : scope->children) collect_locals(child, locals);
        if (locals.empty()) return out;
        for (auto *child : scope->children) find_misuse(child, locals, out);
        return out;
    }

    int positional_pattern_elements(const std::string &text) {
        if (!text.starts_with("'{") || !text.ends_with("}")) return -1;
        std::string body = text.substr(2, text.size() - 3);
        if (body.empty()) return -1;
        std::vector<std::string> elements;
        std::string current;
        int depth = 0;
        bool in_string = false;
        for (char c : body) {
            if (c == '"') in_string = !in_string;
            if (!in_string) {
                if (c == '(' || c == '[' || c == '{') ++depth;
                else if (c == ')' || c == ']' || c == '}') --depth;
                else if (c == ',' && depth == 0) { elements.push_back(current); current.clear(); continue; }
            }
            current += c;
        }
        elements.push_back(current);
        static const std::regex replication(R"(^[^{}]+\{.*\}$)");
        for (const auto &e : elements) {
            if (e.empty()) return -1;
            // `key: value` (named) and `N{...}` (replication) patterns are not positional.
            int d = 0;
            bool str = false;
            for (char c : e) {
                if (c == '"') str = !str;
                if (str) continue;
                if (c == '(' || c == '[' || c == '{') ++d;
                else if (c == ')' || c == ']' || c == '}') --d;
                else if ((c == ':' || c == '?') && d == 0) return -1;
            }
            if (elements.size() == 1 && std::regex_match(e, replication)) return -1;
        }
        return static_cast<int>(elements.size());
    }

    int first_literal_dimension(mgp_sv::sv2017::Variable_dimensionContext *dim) {
        auto w = literal_range_width(dim->getText());
        return w ? *w : -1;
    }

    typedef_shape describe_typedef(sv2017::Type_declarationContext *ctx) {
        using kind_t = typedef_shape::kind_t;
        typedef_shape shape;
        if (ctx->identifier().empty()) return shape;
        shape.name = ctx->identifier().back()->getText();
        shape.line = ctx->getStart()->getLine();
        auto *dt = ctx->data_type();
        if (!dt) {
            // `typedef T;`, `typedef enum T;`, `typedef class C;`
            shape.kind = ctx->getText().find("class") != std::string::npos ? kind_t::class_type : kind_t::forward;
            return shape;
        }
        if (dt->KW_STRING()) shape.kind = kind_t::string_type;
        else if (dt->KW_CHANDLE() || dt->KW_EVENT()) shape.kind = kind_t::chandle_or_event;
        else if (dt->KW_ENUM()) {
            shape.kind = kind_t::enum_type;
            if (auto *eb = dt->enum_base_type(); eb && eb->package_or_class_scoped_id()) {
                const auto name = eb->package_or_class_scoped_id()->getText();
                if (name.find("::") == std::string::npos) shape.depends.push_back(name);
            }
        } else if (dt->struct_union()) {
            shape.kind = dt->struct_union()->KW_UNION() ? kind_t::union_type : kind_t::struct_type;
            shape.member_count = 0;
            for (auto *member : dt->struct_union_member()) {
                if (member->list_of_variable_decl_assignments()) {
                    shape.member_count += static_cast<int>(member->list_of_variable_decl_assignments()->variable_decl_assignment().size());
                }
            }
            for (auto *member : dt->struct_union_member()) {
                auto *mdt = member->data_type_or_void() ? member->data_type_or_void()->data_type() : nullptr;
                const auto name = plain_type_name(mdt);
                if (name.empty() || !member->list_of_variable_decl_assignments()) continue;
                // By-value members only: a queue or dynamic array of the type is fine.
                for (auto *decl : member->list_of_variable_decl_assignments()->variable_decl_assignment()) {
                    if (decl->variable_dimension().empty() && (!mdt || mdt->variable_dimension().empty())) {
                        shape.depends.push_back(name);
                        break;
                    }
                }
            }
        } else if (auto *primitive = dt->data_type_primitive()) {
            if (primitive->non_integer_type()) shape.kind = kind_t::real_type;
            else if (primitive->integer_type()) {
                shape.is_atom = primitive->integer_type()->integer_atom_type() != nullptr;
                shape.packed_dims = static_cast<int>(dt->variable_dimension().size());
            }
        } else if (auto name = plain_type_name(dt); !name.empty()) {
            shape.depends.push_back(name);
            shape.packed_dims = static_cast<int>(dt->variable_dimension().size());
        }
        if (auto kind = dimension_kind(ctx->variable_dimension()); kind != kind_t::other) shape.kind = kind;
        return shape;
    }

    enum_info describe_enum(sv2017::Data_typeContext *ctx) {
        enum_info info;
        info.line = ctx->getStart()->getLine();
        if (auto *eb = ctx->enum_base_type()) {
            if (auto *atom = eb->integer_atom_type()) {
                static const std::map<std::string, std::pair<int, bool>> atoms = {
                    {"byte", {8, true}}, {"shortint", {16, true}}, {"int", {32, true}},
                    {"longint", {64, true}}, {"integer", {32, true}}, {"time", {64, false}}};
                auto it = atoms.find(atom->getText());
                info.width = it == atoms.end() ? 0 : it->second.first;
                info.is_signed = it == atoms.end() ? true : it->second.second;
                info.two_state = atom->getText() != "integer" && atom->getText() != "time";
                if (eb->signing()) info.is_signed = eb->signing()->getText() == "signed";
            } else if (auto *vec = eb->integer_vector_type()) {
                info.is_signed = eb->signing() && eb->signing()->getText() == "signed";
                info.two_state = vec->getText() == "bit";
                info.width = 1;
                if (auto *dim = eb->variable_dimension()) {
                    auto w = literal_range_width(dim->getText());
                    info.width = w ? *w : 0;
                }
            } else if (auto *scoped = eb->package_or_class_scoped_id()) {
                const auto name = scoped->getText();
                if (name.find("::") == std::string::npos) info.base_name = name;
                info.has_base_dimension = eb->variable_dimension() != nullptr;
                info.width = 0;
            } else {
                info.width = 0; // `enum [3:0] {...}`
            }
        }
        for (auto *e : ctx->enum_name_declaration()) {
            if (!e->identifier()) continue;
            enum_member_info m;
            m.name = e->identifier()->getText();
            m.line = e->getStart()->getLine();
            if (!e->integral_number().empty()) {
                const auto &nums = e->integral_number();
                try {
                    const long a = std::stol(nums[0]->getText());
                    const long b = nums.size() > 1 ? std::stol(nums[1]->getText()) : -1;
                    m.count = nums.size() > 1 ? static_cast<int>((a > b ? a - b : b - a) + 1) : static_cast<int>(a);
                } catch (...) { m.count = 1; }
            }
            if (e->expression()) m.expression = e->expression()->getText();
            info.members.push_back(std::move(m));
        }
        return info;
    }

    std::vector<packed_member_info> describe_packed_members(sv2017::Data_typeContext *ctx) {
        std::vector<packed_member_info> out;
        for (auto *member : ctx->struct_union_member()) {
            auto *list = member->list_of_variable_decl_assignments();
            auto *mdt = member->data_type_or_void() ? member->data_type_or_void()->data_type() : nullptr;
            if (!list) continue;
            std::string builtin;
            std::string type_name;
            if (mdt) {
                if (mdt->KW_STRING()) builtin = "string";
                else if (mdt->KW_CHANDLE()) builtin = "chandle";
                else if (mdt->KW_EVENT()) builtin = "event";
                else if (mdt->data_type_primitive() && mdt->data_type_primitive()->non_integer_type()) builtin = "real";
                else if (mdt->struct_union() && !mdt->struct_union()->KW_UNION() && !mdt->KW_PACKED()) builtin = "unpacked struct";
                else if (mdt->struct_union() && mdt->struct_union()->KW_UNION() && !mdt->KW_PACKED()) builtin = "unpacked union";
                else type_name = plain_type_name(mdt);
            }
            for (auto *decl : list->variable_decl_assignment()) {
                packed_member_info m;
                m.name = decl->identifier() ? decl->identifier()->getText() : "";
                m.line = decl->getStart()->getLine();
                m.has_unpacked_dimension = !decl->variable_dimension().empty();
                m.has_default = decl->expression() != nullptr;
                m.type_name = type_name;
                m.builtin_issue = builtin;
                out.push_back(std::move(m));
            }
        }
        return out;
    }

    end_label_info end_labels(antlr4::ParserRuleContext *ctx) {
        end_label_info info;
        switch (ctx->getRuleIndex()) {
        case sv2017::RuleSeq_block:
            info.kind = "begin";
            info.name = label_after_first(ctx, 0);
            break;
        case sv2017::RulePar_block:
            info.kind = "fork";
            info.name = label_after_first(ctx, 0);
            break;
        case sv2017::RuleGenerate_begin_end_block: {
            info.kind = "generate begin";
            if (!ctx->children.empty() && as_identifier(ctx->children[0])) {
                info.name = text_of(as_identifier(ctx->children[0]));
            } else {
                info.name = label_after_first(ctx, 0);
            }
            break;
        }
        case sv2017::RuleModule_declaration: {
            info.kind = "module";
            auto *m = static_cast<sv2017::Module_declarationContext *>(ctx);
            if (auto *h = m->module_header_common()) info.name = text_of(h->identifier());
            break;
        }
        case sv2017::RuleInterface_declaration: {
            info.kind = "interface";
            auto *m = static_cast<sv2017::Interface_declarationContext *>(ctx);
            info.name = m->interface_header() ? text_of(m->interface_header()->identifier()) : first_direct_identifier(ctx);
            break;
        }
        case sv2017::RuleProgram_declaration: {
            info.kind = "program";
            auto *m = static_cast<sv2017::Program_declarationContext *>(ctx);
            info.name = m->program_header() ? text_of(m->program_header()->identifier()) : first_direct_identifier(ctx);
            break;
        }
        case sv2017::RuleUdp_declaration: {
            info.kind = "primitive";
            auto *m = static_cast<sv2017::Udp_declarationContext *>(ctx);
            if (m->udp_ansi_declaration()) info.name = text_of(m->udp_ansi_declaration()->identifier());
            else if (m->udp_nonansi_declaration()) info.name = text_of(m->udp_nonansi_declaration()->identifier());
            else info.name = first_direct_identifier(ctx);
            break;
        }
        case sv2017::RulePackage_declaration:
            info.kind = "package";
            info.name = first_direct_identifier(ctx);
            break;
        case sv2017::RuleClass_declaration:
        case sv2017::RuleInterface_class_declaration:
            info.kind = "class";
            info.name = first_direct_identifier(ctx);
            break;
        case sv2017::RuleChecker_declaration:
            info.kind = "checker";
            info.name = first_direct_identifier(ctx);
            break;
        case sv2017::RuleTask_declaration: {
            info.kind = "task";
            auto *common = static_cast<sv2017::Task_declarationContext *>(ctx)->task_and_function_declaration_common();
            if (common && !common->identifier().empty()) info.name = common->identifier().back()->getText();
            break;
        }
        case sv2017::RuleFunction_declaration: {
            info.kind = "function";
            auto *u = static_cast<sv2017::Function_declarationContext *>(ctx)->untyped_function_declaration();
            auto *common = u ? u->task_and_function_declaration_common() : nullptr;
            if (common && !common->identifier().empty()) info.name = common->identifier().back()->getText();
            break;
        }
        default:
            return info;
        }
        read_tail_label(ctx, info);
        return info;
    }

    std::vector<scope_declaration> collect_scope_declarations(antlr4::ParserRuleContext *scope) {
        std::vector<scope_declaration> out;
        if (scope) {
            for (auto *child : scope->children) {
                if (!opens_nested_scope(child)) collect(child, out);
            }
        }
        return out;
    }
}
