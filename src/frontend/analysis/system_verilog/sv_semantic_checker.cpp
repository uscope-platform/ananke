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

int sv_semantic_checker::exit_code_for(semantic_error_code code) {
    switch (code) {
    case semantic_error_code::builtin_method_override:
    case semantic_error_code::enum_value_size_mismatch:
    case semantic_error_code::enum_unknown_in_two_state:
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
