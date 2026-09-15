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

#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <spdlog/spdlog.h>

#include "data_model/HDL/types/HDL_enum_type.hpp"
#include "data_model/HDL/types/hdl_type.hpp"

CEREAL_REGISTER_TYPE(HDL_enum_type)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_type, HDL_enum_type)

parameter_deps_t HDL_enum_type::get_dependencies(){
    parameter_deps_t result;
    if (base_type) result.merge(base_type->get_dependencies());
    for (auto &dim : unpacked_dimensions) {
        if (dim.first_bound) result.merge(dim.first_bound->get_dependencies());
        if (dim.second_bound) result.merge(dim.second_bound->get_dependencies());
    }
    return result;
}

std::optional<resolved_type> HDL_enum_type::evaluate_type(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    // Dim bounds live in the typedef's defining scope: when reached from
    // another context, only package-qualified copies exist (e.g.
    // fpnew_pkg::NUM_FP_FORMATS), while the bounds are bare. Mirror the
    // HDL_simple_type overlay: synthesize unambiguous bare aliases for both
    // the base eval and the dim bound evals.
    auto dim_needs_scope = false;
    for (const auto &dim : unpacked_dimensions) {
        for (const auto *bound : {dim.first_bound.get(), dim.second_bound.get()}) {
            if (!bound) continue;
            for (const auto &dep : bound->get_dependencies().data) {
                if (dep.get_package_prefix().empty() && dep.get_instance().empty()) dim_needs_scope = true;
            }
        }
    }
    const auto eval_ctx_opt = dim_needs_scope ? overlay_unambiguous_scope(context) : std::nullopt;
    const auto &eval_ctx = eval_ctx_opt ? *eval_ctx_opt : context;

    std::optional<resolved_type> base_result;
    if (base_type) {
        base_result = base_type->evaluate_type(eval_ctx);
        // Array dims ride on top of the base; an unevaluable base poisons
        // the whole reference even when they would have carried the info.
        if (!base_result) return std::nullopt;
    } else {
        resolved_type rt;
        rt.packed_sizes.push_back(32);
        rt.packed_ascending.push_back(true);
        base_result = rt;
    }
    for (auto &dim : unpacked_dimensions) {
        if (!dim.first_bound || !dim.second_bound) return std::nullopt;
        auto f_b = dim.first_bound->evaluate(eval_ctx);
        auto s_b = dim.second_bound->evaluate(eval_ctx);
        if (!(f_b.has_value() && s_b.has_value()) || !f_b.value().is_integer() || !s_b.value().is_integer())
            return std::nullopt;
        auto first = f_b.value().get_integer().to_wide();
        auto second = s_b.value().get_integer().to_wide();
        auto span = first > second ? (first - second) : (second - first);
        if (span > 1'000'000) {
            // Corrupted bound arithmetic (e.g. the defining context was not
            // resolvable) must never materialize as mdarray dimensions.
            spdlog::warn("enum dimension evaluates to {} cells, refusing to build", span.str());
            return std::nullopt;
        }
        hdl_integer diff;
        diff.set_value(span + 1);
        base_result->unpacked_sizes.push_back(diff.get_value());
        base_result->unpacked_ascending.push_back(f_b.value().get_integer() < s_b.value().get_integer());
        base_result->unpacked_left.push_back(f_b.value().get_integer().get_value());
        base_result->unpacked_right.push_back(s_b.value().get_integer().get_value());
    }
    return base_result;
}

std::string HDL_enum_type::to_print() const{
    std::string result = "enum {";
    for (size_t i = 0; i < members.size(); ++i) {
        if (i > 0) result += ", ";
        result += members[i].name;
    }
    result += "}";
    if (base_type) {
        result += " (base: " + base_type->to_print() + ")";
    }
    return result;
}
