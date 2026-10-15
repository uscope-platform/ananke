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

#include <algorithm>
#include <cctype>
#include <map>
#include <regex>

#include "frontend/analysis/system_verilog/sv_parsing_helpers.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/types/hdl_type.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/types/resolved_type.hpp"

namespace {

// Only literal bounds (no identifiers of any kind) are trusted: anything
// else needs elaboration context this checker does not have, and an
// unevaluated identifier must never become a made-up width.
bool bounds_are_literal(const std::vector<dimension_t> &dims) {
    for (const auto &dim : dims) {
        for (const auto *bound : {dim.first_bound.get(), dim.second_bound.get()}) {
            if (!bound) return false;
            if (!bound->get_dependencies().data.empty()) return false;
        }
    }
    return true;
}

std::optional<uint64_t> resolve_base_width(const std::shared_ptr<hdl_type> &base) {
    if (!base) return 32; // LRM 6.19: the default enum base type is int.
    if (!base->is<HDL_simple_type>()) return std::nullopt;
    if (!bounds_are_literal(base->as<HDL_simple_type>().get_packed_dimensions())) {
        return std::nullopt;
    }
    auto resolved = base->evaluate_type({});
    if (!resolved.has_value()) return std::nullopt;
    return packed_width(resolved.value());
}

// 2-state base (or the default int base): x/z member values are illegal
// there. Anything else, including unresolvable bases, is given the benefit
// of the doubt.
bool is_two_state_base(const std::shared_ptr<hdl_type> &base) {
    if (!base) return true;
    if (!base->is<HDL_simple_type>()) return false;
    static const std::set<std::string> two_state = {
        "bit", "byte", "shortint", "int", "longint"};
    return two_state.contains(base->as<HDL_simple_type>().get_type_name());
}

void record_violation(
    const std::string &file, unsigned int line, std::vector<semantic_violation> &violations,
    semantic_error_code code, const std::string &message) {
    spdlog::error("{}:{}: error: {}", file, line, message);
    violations.push_back(semantic_violation{code, message});
}

} // namespace

bool sv_semantic_checker::check_class_method(
    const std::string &file, unsigned int line, const std::string &method_name) {
    if (opts_.disable_semantic_checks) return false;
    static const std::set<std::string> builtin_class_methods = {
        "randomize", "rand_mode", "constraint_mode"};
    if (builtin_class_methods.contains(method_name)) {
        semantic_violation violation{
            semantic_error_code::builtin_method_override,
            "method " + method_name + "() is built-in and cannot be overridden"};
        spdlog::error("{}:{}: error: {}", file, line, violation.message);
        violations_.push_back(std::move(violation));
        return true;
    }
    return false;
}

bool sv_semantic_checker::check_pure_virtual_method(
    const std::string &file, unsigned int line, const std::string &class_name,
    bool class_is_virtual) {
    if (opts_.disable_semantic_checks) return false;
    if (class_is_virtual) return false;
    record_violation(
        file, line, violations_, semantic_error_code::pure_virtual_in_non_virtual_class,
        "pure virtual method declared in non-virtual class '" + class_name +
            "'; pure virtual methods are only allowed in virtual classes");
    return true;
}

int sv_semantic_checker::exit_code_for(semantic_error_code code) {
    switch (code) {
    case semantic_error_code::builtin_method_override:
    case semantic_error_code::enum_value_size_mismatch:
    case semantic_error_code::enum_unknown_in_two_state:
    case semantic_error_code::pure_virtual_in_non_virtual_class:
    case semantic_error_code::enum_assignment_type_mismatch:
    case semantic_error_code::timescale_precision_exceeds_unit:
    case semantic_error_code::undefined_identifier:
    case semantic_error_code::unresolved_forward_typedef:
    case semantic_error_code::conflicting_declaration:
    case semantic_error_code::end_label_mismatch:
    case semantic_error_code::invalid_type_use:
        return 56;
    }
    return 56;
}

bool sv_semantic_checker::check_enum_value(
    const std::string &file, unsigned int line, const std::string &member_name,
    int64_t literal_size, bool sized_explicit, bool has_unknown_digits,
    const std::shared_ptr<hdl_type> &base_type) {
    if (opts_.disable_semantic_checks) return false;
    if (!sized_explicit) return false;
    bool fired = false;
    if (has_unknown_digits && is_two_state_base(base_type)) {
        record_violation(
            file, line, violations_, semantic_error_code::enum_unknown_in_two_state,
            "enum member '" + member_name +
                "' assigns x/z to an enum with 2-state base type");
        fired = true;
    }
    if (literal_size >= 0) {
        auto width = resolve_base_width(base_type);
        if (width.has_value() && static_cast<uint64_t>(literal_size) != *width) {
            record_violation(
                file, line, violations_, semantic_error_code::enum_value_size_mismatch,
                "enum member '" + member_name + "' has a sized literal of size " +
                    std::to_string(literal_size) + " but the enum base type has width " +
                    std::to_string(*width));
            fired = true;
        }
    }
    return fired;
}

void sv_semantic_checker::note_typedef(const std::string &name, bool is_enum) {
    if (is_enum) enum_typedefs_.insert(name);
    else enum_typedefs_.erase(name);
}

void sv_semantic_checker::note_variable(
    const std::string &name, const std::string &type_name, bool is_anonymous_enum,
    bool has_dimensions) {
    const bool is_enum = is_anonymous_enum || enum_typedefs_.contains(type_name);
    if (is_enum && !has_dimensions) enum_variables_.insert(name);
    else enum_variables_.erase(name);
}

bool sv_semantic_checker::check_assignment(
    const std::string &file, unsigned int line, const std::string &lhs,
    const std::string &rhs_text) {
    if (opts_.disable_semantic_checks) return false;
    if (!enum_variables_.contains(lhs) || rhs_text.empty()) return false;
    if (!std::isdigit(static_cast<unsigned char>(rhs_text[0])) && rhs_text[0] != '\'') return false;
    if (!std::dynamic_pointer_cast<Numeric_token>(sv_parsing_helpers::make_value(rhs_text))) {
        return false;
    }
    record_violation(
        file, line, violations_, semantic_error_code::enum_assignment_type_mismatch,
        "cannot assign integral literal '" + rhs_text + "' to enum variable '" + lhs +
            "' without a cast; enum types are strongly typed");
    return true;
}

bool sv_semantic_checker::check_timescale(
    const std::string &file, unsigned int line, uint64_t unit_fs, uint64_t precision_fs,
    std::string_view directive) {
    if (opts_.disable_semantic_checks) return false;
    if (precision_fs <= unit_fs) return false;
    record_violation(
        file, line, violations_, semantic_error_code::timescale_precision_exceeds_unit,
        "invalid " + std::string(directive) +
            ": time precision shall not be larger than the time unit");
    return true;
}

void sv_semantic_checker::begin_unit_scope() {
    unit_declared_.clear();
    unit_resolved_.clear();
    unit_forwards_.clear();
    unit_wildcard_import_ = false;
    pending_unit_names_.clear();
}

void sv_semantic_checker::declare_unit_name(const std::string &name) {
    unit_declared_.insert(name);
    unit_resolved_.insert(name);
}

void sv_semantic_checker::declare_unit_forward(const std::string &name, unsigned int line) {
    unit_declared_.insert(name);
    unit_forwards_.emplace_back(name, line);
}

bool sv_semantic_checker::check_unit_reference(
    const std::string &file, unsigned int line, const std::string &name) {
    if (opts_.disable_semantic_checks || unit_wildcard_import_) return false;
    if (unit_declared_.contains(name)) return false;
    if (opts_.defer_unit_name_errors) {
        pending_unit_names_.push_back({name, line, false});
        return false;
    }
    record_violation(
        file, line, violations_, semantic_error_code::undefined_identifier,
        "'" + name + "' is not declared");
    return true;
}

bool sv_semantic_checker::finish_unit_scope(const std::string &file) {
    if (opts_.disable_semantic_checks) return false;
    bool fired = false;
    for (const auto &[name, line] : unit_forwards_) {
        if (unit_resolved_.contains(name)) continue;
        if (opts_.defer_unit_name_errors) {
            pending_unit_names_.push_back({name, line, true});
            continue;
        }
        record_violation(
            file, line, violations_, semantic_error_code::unresolved_forward_typedef,
            "forward typedef '" + name + "' is never resolved to a data type");
        fired = true;
    }
    return fired;
}

namespace {

bool is_literal_range(const std::string &range) {
    static const std::regex literal(R"((\[[0-9]+:[0-9]+\])+)");
    return std::regex_match(range, literal);
}

} // namespace

bool sv_semantic_checker::check_scope_declarations(
    const std::string &file, const std::vector<scope_declaration> &declarations, bool is_tf_scope) {
    if (opts_.disable_semantic_checks) return false;

    struct entry {
        std::vector<const scope_declaration *> ports;
        std::vector<const scope_declaration *> objects;
    };
    std::map<std::string, entry> by_name;
    for (const auto &d : declarations) {
        (d.is_port ? by_name[d.name].ports : by_name[d.name].objects).push_back(&d);
    }

    bool fired = false;
    auto report = [&](unsigned int line, const std::string &message) {
        record_violation(file, line, violations_, semantic_error_code::conflicting_declaration, message);
        fired = true;
    };
    for (const auto &decl : declarations) {
        const auto &e = by_name[decl.name];
        // Reported once per name, on the declaration that causes the conflict.
        const bool is_first_report = (decl.is_port ? e.ports : e.objects).front() == &decl;
        if (is_first_report) {
            if (e.ports.size() > 1) {
                report(e.ports[1]->line, "port '" + decl.name + "' is declared more than once");
                continue;
            }
            if (e.objects.size() > 1) {
                report(e.objects[1]->line, "'" + decl.name + "' is already declared in this scope");
                continue;
            }
            if (e.ports.empty() || e.objects.empty()) continue;
            const auto &port = *e.ports.front();
            const auto &object = *e.objects.front();
            const unsigned int line = std::max(port.line, object.line);
            if (port.explicit_type) {
                report(line, "'" + decl.name + "' is a port declared with a type and cannot be declared again");
            } else if (!object.vector_type && !port.range.empty()) {
                report(line, "port '" + decl.name + "' has packed dimensions " + port.range +
                                 " which conflict with its non-vector type declaration");
            } else if (object.vector_type && port.range.empty() != object.range.empty() &&
                       !(is_tf_scope && port.range.empty())) {
                report(line, "packed dimensions of '" + decl.name + "' differ between its port declaration and its type declaration");
            } else if (object.vector_type && port.range != object.range &&
                       is_literal_range(port.range) && is_literal_range(object.range)) {
                report(line, "packed dimensions " + port.range + " of port '" + decl.name +
                                 "' do not match its type declaration " + object.range);
            }
        }
    }
    return fired;
}

bool sv_semantic_checker::check_end_label(const std::string &file, const end_label_info &info) {
    if (opts_.disable_semantic_checks || !info.has_end || info.end == info.name) return false;
    if (info.name.empty()) {
        record_violation(
            file, info.line, violations_, semantic_error_code::end_label_mismatch,
            "end label '" + info.end + "' on an unnamed " + info.kind + " block");
    } else {
        record_violation(
            file, info.line, violations_, semantic_error_code::end_label_mismatch,
            "end label '" + info.end + "' does not match the " + info.kind + " name '" + info.name + "'");
    }
    return true;
}

namespace {

using kind_t = typedef_shape::kind_t;

const char *kind_description(kind_t kind) {
    switch (kind) {
    case kind_t::enum_type: return "an enum";
    case kind_t::struct_type: return "a struct";
    case kind_t::union_type: return "a union";
    case kind_t::real_type: return "a real type";
    case kind_t::string_type: return "a string";
    case kind_t::chandle_or_event: return "a chandle or event";
    case kind_t::class_type: return "a class";
    case kind_t::unpacked_array: return "an unpacked array";
    case kind_t::dynamic_array: return "a dynamic array";
    case kind_t::queue: return "a queue";
    default: return "";
    }
}

const char *kind_noun(kind_t kind) {
    switch (kind) {
    case kind_t::string_type: return "string";
    case kind_t::chandle_or_event: return "chandle/event";
    case kind_t::unpacked_array: return "unpacked array";
    case kind_t::dynamic_array: return "dynamic array";
    case kind_t::queue: return "queue";
    default: return "non-net";
    }
}

bool is_non_integral(kind_t kind) {
    switch (kind) {
    case kind_t::real_type: case kind_t::string_type: case kind_t::chandle_or_event: case kind_t::class_type:
    case kind_t::unpacked_array: case kind_t::dynamic_array: case kind_t::queue:
        return true;
    default:
        return false;
    }
}

// Small constant evaluator for enum member values. Returns nullopt when the
// text uses anything it does not model (identifiers that are not earlier
// members, function calls, ...); `unknown_digits` reports an x/z literal.
struct enum_expr_eval {
    const std::string &s;
    const std::map<std::string, __int128> &names;
    size_t i = 0;
    bool unknown_digits = false;
    bool non_constant = false;
    bool failed = false;

    void skip() { while (i < s.size() && std::isspace(static_cast<unsigned char>(s[i]))) ++i; }

    std::optional<__int128> primary() {
        skip();
        if (i >= s.size()) { failed = true; return std::nullopt; }
        if (s[i] == '(') {
            ++i;
            auto v = sum();
            skip();
            if (i < s.size() && s[i] == ')') ++i; else failed = true;
            return v;
        }
        if (s[i] == '-') { ++i; auto v = primary(); if (v) return -*v; return std::nullopt; }
        if (s[i] == '+') { ++i; return primary(); }
        if (s[i] == '$') {
            size_t j = i + 1;
            while (j < s.size() && (std::isalnum(static_cast<unsigned char>(s[j])) || s[j] == '_')) ++j;
            const auto name = s.substr(i, j - i);
            static const std::set<std::string> runtime = {"$time", "$stime", "$realtime", "$random", "$urandom", "$urandom_range"};
            if (runtime.contains(name)) non_constant = true;
            failed = true;
            return std::nullopt;
        }
        if (std::isalpha(static_cast<unsigned char>(s[i])) || s[i] == '_') {
            size_t j = i;
            while (j < s.size() && (std::isalnum(static_cast<unsigned char>(s[j])) || s[j] == '_')) ++j;
            auto it = names.find(s.substr(i, j - i));
            i = j;
            if (it == names.end()) { failed = true; return std::nullopt; }
            return it->second;
        }
        if (std::isdigit(static_cast<unsigned char>(s[i])) || s[i] == '\'') return number();
        failed = true;
        return std::nullopt;
    }

    std::optional<__int128> number() {
        size_t j = i;
        std::string size_digits;
        while (j < s.size() && (std::isdigit(static_cast<unsigned char>(s[j])) || s[j] == '_')) {
            if (s[j] != '_') size_digits += s[j];
            ++j;
        }
        if (j < s.size() && s[j] == '\'') {
            ++j;
            if (j < s.size() && (s[j] == 's' || s[j] == 'S')) ++j;
            if (j >= s.size()) { failed = true; return std::nullopt; }
            const char base_c = static_cast<char>(std::tolower(static_cast<unsigned char>(s[j++])));
            int base = base_c == 'b' ? 2 : base_c == 'o' ? 8 : base_c == 'd' ? 10 : base_c == 'h' ? 16 : 0;
            if (!base) { failed = true; return std::nullopt; }
            __int128 value = 0;
            bool any = false, overflow = false;
            while (j < s.size() && (std::isalnum(static_cast<unsigned char>(s[j])) || s[j] == '_' || s[j] == '?')) {
                const char c = static_cast<char>(std::tolower(static_cast<unsigned char>(s[j++])));
                if (c == '_') continue;
                if (c == 'x' || c == 'z' || c == '?') { unknown_digits = true; any = true; continue; }
                int d = std::isdigit(static_cast<unsigned char>(c)) ? c - '0' : (c >= 'a' && c <= 'f') ? 10 + c - 'a' : 99;
                if (d >= base) { failed = true; return std::nullopt; }
                value = value * base + d;
                if (value > (static_cast<__int128>(1) << 100)) overflow = true;
                any = true;
            }
            i = j;
            if (!any || overflow) { failed = true; return std::nullopt; }
            if (unknown_digits) { failed = true; return std::nullopt; }
            return value;
        }
        if (size_digits.empty() || size_digits.size() > 18) { failed = true; return std::nullopt; }
        i = j;
        return static_cast<__int128>(std::stoll(size_digits));
    }

    std::optional<__int128> product() {
        auto lhs = primary();
        while (true) {
            skip();
            if (i < s.size() && s[i] == '*' && (i + 1 >= s.size() || s[i + 1] != '*')) {
                ++i;
                auto rhs = primary();
                if (lhs && rhs) lhs = *lhs * *rhs; else lhs = std::nullopt;
            } else break;
        }
        return lhs;
    }

    std::optional<__int128> sum() {
        auto lhs = product();
        while (true) {
            skip();
            if (i < s.size() && (s[i] == '+' || s[i] == '-')) {
                const char op = s[i++];
                auto rhs = product();
                if (lhs && rhs) lhs = op == '+' ? *lhs + *rhs : *lhs - *rhs; else lhs = std::nullopt;
            } else break;
        }
        return lhs;
    }

    std::optional<__int128> run() {
        auto v = sum();
        skip();
        if (i != s.size()) failed = true;
        return failed ? std::nullopt : v;
    }
};

} // namespace

bool sv_semantic_checker::check_typedef(const std::string &file, const typedef_shape &shape) {
    if (opts_.disable_semantic_checks) return false;
    // A forward declaration never replaces the real one.
    if (shape.kind == kind_t::forward) {
        typedefs_.try_emplace(shape.name, shape);
        return false;
    }
    typedefs_[shape.name] = shape;
    // Depth-first search for a path from the typedef back to itself.
    std::set<std::string> visited;
    std::vector<std::string> stack(shape.depends.begin(), shape.depends.end());
    while (!stack.empty()) {
        auto name = stack.back();
        stack.pop_back();
        if (name == shape.name) {
            record_violation(
                file, shape.line, violations_, semantic_error_code::invalid_type_use,
                "type '" + shape.name + "' contains or aliases itself (circular type definition)");
            return true;
        }
        if (!visited.insert(name).second) continue;
        auto it = typedefs_.find(name);
        if (it != typedefs_.end()) stack.insert(stack.end(), it->second.depends.begin(), it->second.depends.end());
    }
    return false;
}

bool sv_semantic_checker::check_packed_members(
    const std::string &file, const std::string &aggregate, const std::vector<packed_member_info> &members) {
    if (opts_.disable_semantic_checks) return false;
    bool fired = false;
    for (const auto &m : members) {
        std::string issue;
        if (m.has_unpacked_dimension) issue = "has an unpacked, dynamic or queue dimension";
        else if (m.has_default) issue = "has a default value";
        else if (!m.builtin_issue.empty()) issue = "has " + m.builtin_issue + " type";
        else if (!m.type_name.empty()) {
            if (auto it = typedefs_.find(m.type_name); it != typedefs_.end() && is_non_integral(it->second.kind)) {
                issue = std::string("has type '") + m.type_name + "' which is " + kind_description(it->second.kind);
            } else if (class_names_.contains(m.type_name)) {
                issue = "has class type '" + m.type_name + "'";
            }
        }
        if (issue.empty()) continue;
        record_violation(
            file, m.line, violations_, semantic_error_code::invalid_type_use,
            "member '" + m.name + "' of a packed " + aggregate + " " + issue + "; packed members must be packed integral types");
        fired = true;
    }
    return fired;
}

bool sv_semantic_checker::check_net_type(
    const std::string &file, unsigned int line, const std::string &net_name, bool is_string,
    const std::string &type_name, bool has_queue_dimension) {
    if (opts_.disable_semantic_checks) return false;
    std::string issue;
    if (is_string) issue = "string";
    else if (has_queue_dimension) issue = "queue";
    else if (!type_name.empty()) {
        if (class_names_.contains(type_name)) issue = "class";
        else if (auto it = typedefs_.find(type_name); it != typedefs_.end() && is_non_integral(it->second.kind) &&
                                                      it->second.kind != kind_t::real_type) {
            issue = kind_noun(it->second.kind);
        }
    }
    if (issue.empty()) return false;
    record_violation(
        file, line, violations_, semantic_error_code::invalid_type_use,
        "net '" + net_name + "' cannot have " + issue + " type; nets take 4-state integral types");
    return true;
}

bool sv_semantic_checker::check_enum(const std::string &file, const enum_info &info) {
    if (opts_.disable_semantic_checks) return false;
    bool fired = false;
    auto report = [&](unsigned int line, const std::string &message) {
        record_violation(file, line, violations_, semantic_error_code::invalid_type_use, message);
        fired = true;
    };
    if (!info.base_name.empty()) {
        if (auto it = typedefs_.find(info.base_name); it != typedefs_.end()) {
            const auto &base = it->second;
            if (base.kind == kind_t::enum_type || base.kind == kind_t::struct_type || base.kind == kind_t::union_type ||
                is_non_integral(base.kind)) {
                report(info.line, "enum base type '" + info.base_name + "' is " + kind_description(base.kind) +
                                      "; an enum base must be an integral type");
            } else if (base.packed_dims >= 2) {
                report(info.line, "enum base type '" + info.base_name + "' has multiple packed dimensions");
            } else if (info.has_base_dimension && (base.packed_dims > 0 || base.is_atom)) {
                report(info.line, "enum base type '" + info.base_name + "' already has packed dimensions");
            }
        }
        return fired;
    }
    if (info.width <= 0) return fired;

    std::map<std::string, __int128> names;
    std::map<__int128, std::string> seen_values;
    std::optional<__int128> next = 0;
    const __int128 one = 1;
    const __int128 lo = info.is_signed ? -(one << (info.width - 1)) : 0;
    const __int128 hi = info.is_signed ? (one << (info.width - 1)) - 1 : (one << info.width) - 1;
    for (const auto &m : info.members) {
        std::optional<__int128> value = next;
        if (m.expression) {
            enum_expr_eval eval{*m.expression, names};
            value = eval.run();
            if (eval.non_constant) {
                report(m.line, "enum member '" + m.name + "' value '" + *m.expression + "' is not a constant expression");
                value = std::nullopt;
            } else if (eval.unknown_digits) {
                // Sized literals are judged by check_enum_value; an unsized one is only seen here.
                static const std::regex unsized(R"((^|[^0-9a-zA-Z_])')");
                if (info.two_state && std::regex_search(*m.expression, unsized)) {
                    record_violation(
                        file, m.line, violations_, semantic_error_code::enum_unknown_in_two_state,
                        "enum member '" + m.name + "' assigns x/z to an enum with 2-state base type");
                    fired = true;
                }
                if (false) {
                    report(m.line, "enum member '" + m.name + "' assigns x/z to an enum with 2-state base type");
                }
                value = std::nullopt;
            }
        }
        for (int k = 0; k < m.count; ++k) {
            if (!value) { next = std::nullopt; break; }
            __int128 v = *value + k;
            // A sized literal wraps at its own width (`-4'sd1` is 4'b1111), so
            // only the pattern at the base width is compared.
            if (m.expression && m.count == 1) {
                static const std::regex sized(R"([0-9]\s*')");
                if (std::regex_search(*m.expression, sized)) {
                    const __int128 modulus = one << info.width;
                    v = ((v % modulus) + modulus) % modulus;
                    if (info.is_signed && v > hi) v -= modulus;
                }
            }
            const std::string member = m.count > 1 ? m.name + std::to_string(k) : m.name;
            if (v < lo || v > hi) {
                report(m.line, "enum member '" + member + "' value " + std::to_string(static_cast<long long>(v)) +
                                   " does not fit the " + std::to_string(info.width) + "-bit " +
                                   (info.is_signed ? "signed" : "unsigned") + " base type");
            } else if (auto dup = seen_values.find(v); dup != seen_values.end()) {
                report(m.line, "enum member '" + member + "' has the same value as '" + dup->second + "'");
            } else {
                seen_values.emplace(v, member);
            }
            names[member] = v;
            next = v + 1;
        }
        if (m.count == 1 && value) names[m.name] = *value;
        if (!value) next = std::nullopt;
    }
    return fired;
}

bool sv_semantic_checker::check_assignment_pattern(
    const std::string &file, unsigned int line, const std::string &name, int elements, int expected, bool scalar) {
    if (opts_.disable_semantic_checks || elements < 0) return false;
    if (scalar) {
        record_violation(
            file, line, violations_, semantic_error_code::invalid_type_use,
            "assignment pattern used to initialise scalar '" + name + "'");
        return true;
    }
    if (expected >= 0 && elements != expected) {
        record_violation(
            file, line, violations_, semantic_error_code::invalid_type_use,
            "assignment pattern for '" + name + "' has " + std::to_string(elements) + " elements but the type has " +
                std::to_string(expected));
        return true;
    }
    return false;
}

bool sv_semantic_checker::check_foreach(
    const std::string &file, unsigned int line, const std::string &name, int loop_variables) {
    if (opts_.disable_semantic_checks) return false;
    auto it = array_dims_.find(name);
    if (it == array_dims_.end() || loop_variables <= it->second) return false;
    record_violation(
        file, line, violations_, semantic_error_code::invalid_type_use,
        "foreach over '" + name + "' has " + std::to_string(loop_variables) + " loop variables but the array has " +
            std::to_string(it->second) + " dimension" + (it->second == 1 ? "" : "s"));
    return true;
}

bool sv_semantic_checker::check_automatic_use(const std::string &file, const std::vector<automatic_misuse> &misuses) {
    if (opts_.disable_semantic_checks || misuses.empty()) return false;
    for (const auto &m : misuses) {
        record_violation(
            file, m.line, violations_, semantic_error_code::invalid_type_use,
            "automatic variable '" + m.variable + "' is " + m.how);
    }
    return true;
}
