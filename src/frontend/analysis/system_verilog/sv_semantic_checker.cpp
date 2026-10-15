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
