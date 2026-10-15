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

#include "frontend/analysis/system_verilog/sv_semantic_checker.hpp"

#include <spdlog/spdlog.h>

#include <cctype>
#include <functional>
#include <optional>
#include <regex>
#include <tuple>

namespace {

void record_violation(
    const std::string &file, unsigned int line, std::vector<semantic_violation> &violations, const std::string &message) {
    spdlog::error("{}:{}: error: {}", file, line, message);
    violations.push_back(semantic_violation{semantic_error_code::invalid_constant_expression, message});
}

bool is_id_start(char c) { return std::isalpha(static_cast<unsigned char>(c)) || c == '_'; }
bool is_id_char(char c) { return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '$'; }

size_t skip_parens(const std::string &s, size_t open) {
    int depth = 0;
    for (size_t i = open; i < s.size(); ++i) {
        if (s[i] == '(') ++depth;
        else if (s[i] == ')' && --depth == 0) return i + 1;
    }
    return s.size();
}

// Identifiers used as values in an expression: not function calls, casts, members, scoped names, system names
// or the digits of based literals.
std::vector<std::string> identifiers_in(const std::string &s) {
    std::vector<std::string> out;
    size_t i = 0;
    while (i < s.size()) {
        const char c = s[i];
        if (c == '"') {
            ++i;
            while (i < s.size() && s[i] != '"') i += (s[i] == '\\') ? 2 : 1;
            ++i;
            continue;
        }
        if (std::isdigit(static_cast<unsigned char>(c)) || c == '\'') {
            // number, sized/based literal or unbased literal: swallow every alphanumeric of it
            ++i;
            while (i < s.size() && (is_id_char(s[i]) || s[i] == '\'' || s[i] == '?')) ++i;
            continue;
        }
        if (is_id_start(c)) {
            size_t j = i;
            while (j < s.size() && is_id_char(s[j])) ++j;
            const bool member = i > 0 && (s[i - 1] == '.' || s[i - 1] == ':');
            const bool call = j < s.size() && (s[j] == '(' || s[j] == '\'' || (s[j] == ':' && j + 1 < s.size() && s[j + 1] == ':'));
            if (!member && !call) out.push_back(s.substr(i, j - i));
            i = j;
            // Arguments of a call (`f(x)`) are not judged: constant functions and queries such as $bits(x) take objects.
            if (call && s[j] == '(') i = skip_parens(s, j);
            continue;
        }
        if (c == '$') {  // system function name and its arguments
            ++i;
            while (i < s.size() && is_id_char(s[i])) ++i;
            if (i < s.size() && s[i] == '(') i = skip_parens(s, i);
            continue;
        }
        ++i;
    }
    return out;
}

// Splits `a:b` at the first top-level ':' that is not part of `+:` / `-:` / `::`; false when there is none.
bool split_range(const std::string &s, std::string &a, std::string &b) {
    int depth = 0;
    bool in_string = false;
    for (size_t i = 0; i < s.size(); ++i) {
        const char c = s[i];
        if (c == '"') in_string = !in_string;
        if (in_string) continue;
        if (c == '(' || c == '[' || c == '{') ++depth;
        else if (c == ')' || c == ']' || c == '}') --depth;
        else if (c == '?') return false;  // ternary: not modeled
        else if (c == ':' && i + 1 < s.size() && s[i + 1] == ':') ++i;  // scope operator
        else if (c == ':' && depth == 0) {
            if (i > 0 && (s[i - 1] == '+' || s[i - 1] == '-')) return false;
            a = s.substr(0, i);
            b = s.substr(i + 1);
            return true;
        }
    }
    return false;
}

struct evaluator {
    const std::string &s;
    std::function<std::optional<std::tuple<bool, bool, bool, long long>>(const std::string &)> resolve;  // known, xz, real, value
    size_t i = 0;
    struct result { bool known = false, xz = false, real = false; long long value = 0; };

    static result unknown() { return {}; }
    result combine(result a, result b, long long v, bool ok) {
        result r;
        r.xz = a.xz || b.xz;
        r.real = a.real || b.real;
        r.known = ok && a.known && b.known && !r.xz && !r.real;
        r.value = r.known ? v : 0;
        return r;
    }
    bool eat(const char *op) {
        const size_t n = std::char_traits<char>::length(op);
        if (s.compare(i, n, op) == 0) { i += n; return true; }
        return false;
    }

    result primary() {
        if (i >= s.size()) return unknown();
        if (s[i] == '(') {
            ++i;
            auto v = additive();
            if (i < s.size() && s[i] == ')') ++i; else return unknown();
            return v;
        }
        if (s[i] == '-') { ++i; auto v = primary(); v.value = -v.value; return v; }
        if (s[i] == '+') { ++i; return primary(); }
        if (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '\'') return number();
        if (is_id_start(s[i])) {
            size_t j = i;
            while (j < s.size() && is_id_char(s[j])) ++j;
            const auto name = s.substr(i, j - i);
            i = j;
            if (i < s.size() && (s[i] == '(' || s[i] == '\'' || s[i] == '.' || s[i] == '[' || s[i] == ':')) { skip_rest(); return unknown(); }
            if (auto r = resolve(name)) {
                result out;
                std::tie(out.known, out.xz, out.real, out.value) = *r;
                return out;
            }
            return unknown();
        }
        skip_rest();
        return unknown();
    }
    void skip_rest() { i = s.size() + 1; }  // poisons the parse: everything after is unknown

    result number() {
        size_t j = i;
        std::string size_digits;
        while (j < s.size() && (std::isdigit(static_cast<unsigned char>(s[j])) || s[j] == '_')) {
            if (s[j] != '_') size_digits += s[j];
            ++j;
        }
        if (j < s.size() && (s[j] == '.' || ((s[j] == 'e' || s[j] == 'E') && !size_digits.empty()))) {
            // real literal
            while (j < s.size() && (std::isdigit(static_cast<unsigned char>(s[j])) || s[j] == '.' || s[j] == 'e' || s[j] == 'E' ||
                                     ((s[j] == '+' || s[j] == '-') && (s[j - 1] == 'e' || s[j - 1] == 'E')) || s[j] == '_')) ++j;
            i = j;
            result r;
            r.real = true;
            return r;
        }
        if (j < s.size() && s[j] == '\'') {
            ++j;
            if (j < s.size() && (s[j] == 's' || s[j] == 'S')) ++j;
            if (j >= s.size()) { skip_rest(); return unknown(); }
            const char base_c = static_cast<char>(std::tolower(static_cast<unsigned char>(s[j])));
            int base = base_c == 'b' ? 2 : base_c == 'o' ? 8 : base_c == 'd' ? 10 : base_c == 'h' ? 16 : 0;
            if (!base) {  // '0 '1 'x 'z
                ++j;
                i = j;
                result r;
                r.xz = base_c == 'x' || base_c == 'z';
                return r;
            }
            ++j;
            result r;
            bool any = false, overflow = false;
            long long v = 0;
            while (j < s.size() && (std::isalnum(static_cast<unsigned char>(s[j])) || s[j] == '_' || s[j] == '?')) {
                const char c = static_cast<char>(std::tolower(static_cast<unsigned char>(s[j++])));
                if (c == '_') continue;
                any = true;
                if (c == 'x' || c == 'z' || c == '?') { r.xz = true; continue; }
                int d = std::isdigit(static_cast<unsigned char>(c)) ? c - '0' : (c >= 'a' && c <= 'f') ? 10 + c - 'a' : 99;
                if (d >= base) { skip_rest(); return unknown(); }
                if (v > (1LL << 55)) overflow = true; else v = v * base + d;
            }
            i = j;
            if (!any) { skip_rest(); return unknown(); }
            r.known = !r.xz && !overflow;
            r.value = r.known ? v : 0;
            return r;
        }
        if (size_digits.empty() || size_digits.size() > 17) { skip_rest(); return unknown(); }
        i = j;
        result r;
        r.known = true;
        r.value = std::stoll(size_digits);
        return r;
    }

    result multiplicative() {
        auto lhs = primary();
        while (i < s.size()) {
            if (s[i] == '*' && !(i + 1 < s.size() && s[i + 1] == '*')) {
                ++i; auto rhs = primary(); lhs = combine(lhs, rhs, lhs.value * rhs.value, true);
            } else if (s[i] == '/') {
                ++i; auto rhs = primary(); lhs = combine(lhs, rhs, rhs.value ? lhs.value / rhs.value : 0, rhs.value != 0);
            } else if (s[i] == '%') {
                ++i; auto rhs = primary(); lhs = combine(lhs, rhs, rhs.value ? lhs.value % rhs.value : 0, rhs.value != 0);
            } else break;
        }
        return lhs;
    }
    result additive() {
        auto lhs = multiplicative();
        while (i < s.size()) {
            if (s[i] == '+') { ++i; auto rhs = multiplicative(); lhs = combine(lhs, rhs, lhs.value + rhs.value, true); }
            else if (s[i] == '-') { ++i; auto rhs = multiplicative(); lhs = combine(lhs, rhs, lhs.value - rhs.value, true); }
            else if (s.compare(i, 2, "<<") == 0) { i += 2; auto rhs = multiplicative(); lhs = combine(lhs, rhs, (rhs.value >= 0 && rhs.value < 62) ? (lhs.value << rhs.value) : 0, rhs.value >= 0 && rhs.value < 62); }
            else if (s.compare(i, 2, ">>") == 0) { i += 2; auto rhs = multiplicative(); lhs = combine(lhs, rhs, (rhs.value >= 0 && rhs.value < 62) ? (lhs.value >> rhs.value) : 0, rhs.value >= 0 && rhs.value < 62); }
            else break;
        }
        return lhs;
    }
    result run() {
        auto r = additive();
        if (i != s.size()) return unknown();  // trailing operators we do not model
        return r;
    }
};

}  // namespace

const sv_semantic_checker::name_entry *sv_semantic_checker::lookup_name(
    const scope_chain &chain, const std::string &name) const {
    for (const void *scope : chain) {
        auto it = names_.find({scope, name});
        if (it != names_.end()) return &it->second;
    }
    return nullptr;
}

sv_semantic_checker::constant_value sv_semantic_checker::evaluate_constant(
    const std::string &text, const scope_chain &chain) const {
    evaluator ev{text, [&](const std::string &name) -> std::optional<std::tuple<bool, bool, bool, long long>> {
                     const auto *entry = lookup_name(chain, name);
                     if (!entry || entry->variable) return std::nullopt;
                     return std::make_tuple(entry->value.known, entry->value.xz, entry->value.real, entry->value.value);
                 }};
    auto r = ev.run();
    constant_value out;
    out.known = r.known;
    out.xz = r.xz;
    out.real = r.real;
    out.value = r.value;
    return out;
}

void sv_semantic_checker::declare_constant_name(
    const scope_chain &chain, const std::string &name, const std::string &value_text, bool is_real) {
    if (chain.empty()) return;
    name_entry entry;
    if (!value_text.empty()) entry.value = evaluate_constant(value_text, chain);
    if (is_real) { entry.value.real = true; entry.value.known = false; }
    names_[{chain.front(), name}] = entry;
}

void sv_semantic_checker::declare_variable_name(const scope_chain &chain, const std::string &name, bool plain_vector) {
    if (chain.empty()) return;
    name_entry entry;
    entry.variable = true;
    entry.plain_vector = plain_vector;
    names_[{chain.front(), name}] = entry;
}

bool sv_semantic_checker::check_constant_use(
    const std::string &file, unsigned int line, const std::string &text, const scope_chain &chain,
    const std::string &what) {
    if (opts_.disable_semantic_checks) return false;
    for (const auto &id : identifiers_in(text)) {
        const auto *entry = lookup_name(chain, id);
        if (entry && entry->variable) {
            record_violation(file, line, violations_, "'" + id + "' is not a constant but is used in " + what);
            return true;
        }
    }
    return false;
}

bool sv_semantic_checker::check_dimension(
    const std::string &file, unsigned int line, const std::string &inner, bool packed, const scope_chain &chain,
    bool judge_values) {
    if (opts_.disable_semantic_checks) return false;
    std::string a, b;
    const bool is_range = split_range(inner, a, b);
    if (packed && !is_range) {
        // `[]`, `[$]`, `[0]`, `[-1]`: a packed dimension needs both bounds. A single non-literal size
        // (`reg [N-1:0][W] x`, common in real testbenches) is not an LRM form either, but simulators take it as
        // `[W-1:0]`, so only strict mode rejects it.
        static const std::regex literal_size(R"(-?[0-9]+)");
        const bool nonsense = inner.empty() || inner[0] == '$' || std::regex_match(inner, literal_size);
        const bool sloppy_size = !nonsense && inner.find('?') == std::string::npos;
        if (nonsense || (sloppy_size && opts_.strict)) {
            record_violation(file, line, violations_, "invalid packed dimension [" + inner + "]; expected [msb:lsb]");
            return true;
        }
        return false;
    }
    if (!is_range) {
        // unpacked size `[N]`: N constant (a type name or `*` is not an identifier in the table)
        if (inner.empty() || inner[0] == '$') return false;
        return check_constant_use(file, line, inner, chain, "an array dimension");
    }
    if (a.starts_with('$') || b.starts_with('$') || a.empty()) return false;  // queue bound
    bool fired = check_constant_use(file, line, a, chain, "a dimension bound");
    fired = check_constant_use(file, line, b, chain, "a dimension bound") || fired;
    for (const auto *bound : {&a, &b}) {
        if (judge_values && evaluate_constant(*bound, chain).xz) {
            record_violation(file, line, violations_, "dimension bound '" + *bound + "' is x or z");
            fired = true;
            break;
        }
    }
    return fired;
}

bool sv_semantic_checker::check_part_select(
    const std::string &file, unsigned int line, const std::string &object, const std::string &separator,
    const std::string &first, const std::string &second, const scope_chain &chain, bool judge_values) {
    if (opts_.disable_semantic_checks) return false;
    bool fired = false;
    if (separator == ":") {
        // Only fixed-size integral objects: queues and dynamic arrays slice with variable bounds.
        std::string root;
        for (char c : object) { if (!is_id_char(c)) break; root += c; }
        const auto *entry = root.empty() ? nullptr : lookup_name(chain, root);
        if (!entry || !entry->variable || !entry->plain_vector) return false;
        fired = check_constant_use(file, line, first, chain, "a part-select bound");
        fired = check_constant_use(file, line, second, chain, "a part-select bound") || fired;
        for (const auto *bound : {&first, &second}) {
            if (judge_values && evaluate_constant(*bound, chain).xz) {
                record_violation(file, line, violations_, "part-select bound '" + *bound + "' is x or z");
                fired = true;
                break;
            }
        }
        return fired;
    }
    // `+:` / `-:`: the width must be a positive constant.
    fired = check_constant_use(file, line, second, chain, "an indexed part-select width");
    auto width = evaluate_constant(second, chain);
    if (judge_values && (width.xz || (width.known && width.value <= 0))) {
        record_violation(file, line, violations_, "indexed part-select width '" + second + "' must be a positive defined constant");
        fired = true;
    }
    return fired;
}

bool sv_semantic_checker::check_replication(
    const std::string &file, unsigned int line, const std::string &count, bool zero_allowed, const scope_chain &chain,
    bool judge_values) {
    if (opts_.disable_semantic_checks || !judge_values) return false;
    auto v = evaluate_constant(count, chain);
    if (v.xz) {
        record_violation(file, line, violations_, "replication count '" + count + "' is x or z");
        return true;
    }
    if (v.known && v.value < 0) {
        record_violation(file, line, violations_, "replication count '" + count + "' is negative");
        return true;
    }
    if (v.known && v.value == 0 && !zero_allowed) {
        record_violation(file, line, violations_, "zero replication '{" + count + "{...}}' must be an operand of a concatenation with other operands");
        return true;
    }
    // A variable count is legal for string replication (`{n{"-"}}`), so only the value is judged.
    return false;
}

bool sv_semantic_checker::check_size_cast(
    const std::string &file, unsigned int line, const std::string &width, const scope_chain &chain, bool judge_values) {
    if (opts_.disable_semantic_checks || !judge_values) return false;
    auto v = evaluate_constant(width, chain);
    if (v.xz || (v.known && v.value <= 0)) {
        record_violation(file, line, violations_, "size cast width '" + width + "' must be a positive defined constant");
        return true;
    }
    return false;
}

bool sv_semantic_checker::check_concatenation_operands(
    const std::string &file, unsigned int line, const std::vector<std::string> &operands, const scope_chain &chain) {
    if (opts_.disable_semantic_checks) return false;
    for (const auto &op : operands) {
        if (op.empty()) continue;
        if (evaluate_constant(op, chain).real) {
            record_violation(file, line, violations_, "real operand '" + op + "' in a concatenation");
            return true;
        }
    }
    return false;
}

bool sv_semantic_checker::check_net_lvalue(
    const std::string &file, unsigned int line, const std::string &lvalue, const scope_chain &chain) {
    if (opts_.disable_semantic_checks) return false;
    // Index expressions of the target: the contents of each top-level `[...]` group without a range.
    int depth = 0;
    size_t start = 0;
    for (size_t i = 0; i < lvalue.size(); ++i) {
        if (lvalue[i] == '[') { if (depth++ == 0) start = i + 1; }
        else if (lvalue[i] == ']' && --depth == 0) {
            const auto index = lvalue.substr(start, i - start);
            std::string a, b;
            if (!split_range(index, a, b) && index.find(':') == std::string::npos) {
                if (check_constant_use(file, line, index, chain, "a continuous assignment target index")) return true;
            }
        }
    }
    return false;
}
