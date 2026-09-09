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


#include <sstream>

#include "data_model/HDL/types/HDL_simple_type.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(HDL_simple_type)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_type, HDL_simple_type)

void HDL_simple_type::add_dimension(const dimension_t &d) {
    if(d.packed){
        packed_dimensions.push_back(d);
    } else{
        unpacked_dimensions.push_back(d);
    }
}

void HDL_simple_type::set_packed_dimensions(const std::vector<dimension_t> &d) {
    packed_dimensions.insert(packed_dimensions.end(), d.begin(), d.end());
}

void HDL_simple_type::set_unpacked_dimensions(const std::vector<dimension_t> &d) {
    unpacked_dimensions.insert(unpacked_dimensions.end(), d.begin(), d.end());
}

parameter_deps_t HDL_simple_type::get_dependencies() {
    parameter_deps_t result;
    for (auto &dim:unpacked_dimensions) {
        result.merge(dim.first_bound->get_dependencies());
        result.merge(dim.second_bound->get_dependencies());
    }

    for (auto &dim:packed_dimensions) {
        result.merge(dim.first_bound->get_dependencies());
        result.merge(dim.second_bound->get_dependencies());
    }
    return result;
}

std::optional<resolved_type> HDL_simple_type::evaluate_type(const std::map<qualified_identifier, resolved_parameter> &context) {
    // Skip the scope overlay entirely when no dimension bound references a
    // bare identifier: any added bare keys would go unread, so the overlay
    // is provably the identity here (see hdl_type).
    auto dim_needs_scope = [](const auto &dims) {
        for (const auto &dim : dims) {
            for (const auto *bound : {dim.first_bound.get(), dim.second_bound.get()}) {
                if (!bound) continue;
                for (const auto &dep : bound->get_dependencies().data) {
                    if (dep.get_package_prefix().empty() && dep.get_instance().empty()) return true;
                }
            }
        }
        return false;
    };
    // Dimensions live in the typedef's own package scope: unambiguous
    // package-qualified values resolve as bare here (see hdl_type).
    const bool needs_scope =
        dim_needs_scope(unpacked_dimensions) || dim_needs_scope(packed_dimensions);
    const auto overlaid =
        needs_scope ? overlay_unambiguous_scope(context) : std::nullopt;
    const auto &eval_ctx = overlaid ? *overlaid : context;
    resolved_type result;
    result.is_real = is_real;
    auto span_size = [](const hdl_integer &f_b, const hdl_integer &s_b) -> hdl_integer {
        // Width of a range = |first - second| + 1, computed without relying on
        // std::abs(hdl_integer), which is not found by qualified lookup.
        auto a = f_b.to_wide();
        auto b = s_b.to_wide();
        auto span = a > b ? (a - b) : (b - a);
        hdl_integer diff;
        diff.set_value(span + 1);
        return diff;
    };
    for (auto &dim: unpacked_dimensions) {
        auto f_b = dim.first_bound->evaluate(eval_ctx);
        auto s_b = dim.second_bound->evaluate(eval_ctx);
        if (!(f_b.has_value() && s_b.has_value()) || !f_b.value().is_integer() || !s_b.value().is_integer()) return std::nullopt;
        auto diff = span_size(f_b.value().get_integer(), s_b.value().get_integer());
        result.unpacked_sizes.push_back(diff.get_value());
        result.unpacked_ascending.push_back(f_b.value().get_integer() < s_b.value().get_integer());
        result.unpacked_left.push_back(f_b.value().get_integer().get_value());
        result.unpacked_right.push_back(s_b.value().get_integer().get_value());
    }
    for (auto &dim: packed_dimensions) {
        auto f_b = dim.first_bound->evaluate(eval_ctx);
        auto s_b = dim.second_bound->evaluate(eval_ctx);
        if (!(f_b.has_value() && s_b.has_value()) || !f_b.value().is_integer() || !s_b.value().is_integer()) return std::nullopt;
        auto diff = span_size(f_b.value().get_integer(), s_b.value().get_integer());
        result.packed_sizes.push_back(diff.get_value());
        result.packed_ascending.push_back(f_b.value().get_integer() < s_b.value().get_integer());
        result.packed_left.push_back(f_b.value().get_integer().get_value());
        result.packed_right.push_back(s_b.value().get_integer().get_value());
    }

    return result;
}

std::string HDL_simple_type::to_print() const {
    std::string result;
    if (is_implicit || is_signed || is_real || !is_scalar()) {
        result += " (";
        bool first = true;
        if (is_implicit) { result += "implicit"; first = false; }
        if (is_signed) { if (!first) result += ","; result += "signed"; first = false; }
        if (is_real) { if (!first) result += ","; result += "real"; first = false; }
        if (!is_scalar()) { if (!first) result += ","; result += "nonscalar"; }
        result += ")";
    }

    for(const auto &item : packed_dimensions){
        result += "[" + item.first_bound->print() + ":" + item.second_bound->print()+ "]";
    }
    for(const auto &item : unpacked_dimensions){
        result += "[" + item.first_bound->print() + ":" + item.second_bound->print()+ "]";
    }
    return result;
}
