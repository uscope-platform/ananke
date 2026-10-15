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

namespace sv_parsing_helpers {
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
