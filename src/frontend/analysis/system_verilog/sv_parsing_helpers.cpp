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
    bool is_sized_literal(const std::string_view &s) {
        size_t int_end = s.find_first_not_of("0123456789_");
        if (int_end == std::string_view::npos || int_end >= s.size() || s[int_end] != '\'') return false;
        if (int_end != 0 && !is_unsigned_number_text(s.substr(0, int_end))) return false;
        size_t i = int_end + 1;
        if (i < s.size() && (s[i] == 's' || s[i] == 'S')) i++;
        if (i >= s.size()) return false;
        char base = s[i];
        if (base != 'h' && base != 'H' && base != 'd' && base != 'D' &&
            base != 'o' && base != 'O' && base != 'b' && base != 'B') return false;
        if (i + 1 >= s.size()) return false;
        // At least one valid digit, rest must all be valid digits/'_' (prefix match
        // requires the first digit char to be valid; trailing chars were previously
        // unchecked, keep prefix semantics but reject obvious identifiers).
        if (!is_based_digit(s[i + 1], base)) return false;
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
