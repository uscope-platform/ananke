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

#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include <spdlog/spdlog.h>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Concatenation)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, Concatenation)

namespace {
// Sizing derivation shared by evaluate/resolve when an incoming container
// type is present. It replaces the old set_container_sizes pass (deleted
// with it): same branch structure and per-component narrowing, but computed
// into locals instead of mutating members.
struct concat_expected_sizing {
    bool packing = false;
    std::vector<uint64_t> unpacked_dimension;
    std::vector<bool> unpacked_ascending;
    int64_t container_size = 0;
    std::vector<struct_member_resolved_type> fields_sizes;
    std::vector<std::optional<resolved_type>> child_sizing;
};

concat_expected_sizing derive_concat_sizing(const resolved_type &s, size_t n_components) {
    concat_expected_sizing d;
    d.child_sizing.resize(n_components);
    if (s.struct_sizes.empty()) {
        resolved_type content_sizes;
        d.unpacked_dimension = s.unpacked_sizes;
        d.unpacked_ascending = s.unpacked_ascending;
        if (!s.packed_sizes.empty() || !s.unpacked_sizes.empty()) {
            if (!s.unpacked_sizes.empty()) {
                if (s.unpacked_sizes.size() > 1) {
                    content_sizes.unpacked_sizes.insert(content_sizes.unpacked_sizes.end(), s.unpacked_sizes.begin(), s.unpacked_sizes.end() - 1);
                    if (s.unpacked_ascending.size() == s.unpacked_sizes.size()) {
                        content_sizes.unpacked_ascending.insert(content_sizes.unpacked_ascending.end(), s.unpacked_ascending.begin(), s.unpacked_ascending.end() - 1);
                    }
                }
                content_sizes.packed_sizes = s.packed_sizes;
                content_sizes.packed_ascending = s.packed_ascending;
                d.container_size = static_cast<int64_t>(s.unpacked_sizes.back());
                d.packing = false;
            } else {
                d.container_size = static_cast<int64_t>(packed_width(s));
                d.packing = true;
                content_sizes.packed_sizes.insert(content_sizes.packed_sizes.end(), s.packed_sizes.begin(), s.packed_sizes.end());
                content_sizes.packed_ascending.insert(content_sizes.packed_ascending.end(), s.packed_ascending.begin(), s.packed_ascending.end());
            }
            for (auto &cs : d.child_sizing) cs = content_sizes;
        } else {
            d.container_size = 32;
            d.packing = true;
        }
    } else {
        d.packing = s.packed_struct;
        d.container_size = static_cast<int64_t>(packed_width(s));
        d.fields_sizes = s.struct_sizes;
        if (!s.unpacked_sizes.empty()) {
            d.packing = false;
            resolved_type element;
            element.packed_sizes.push_back(packed_width(s));
            element.packed_ascending.push_back(false);
            element.packed_struct = true;
            element.struct_sizes = s.struct_sizes;
            d.unpacked_dimension = s.unpacked_sizes;
            d.unpacked_ascending = s.unpacked_ascending;
            for (auto &cs : d.child_sizing) cs = element;
            d.fields_sizes.clear();
            return d;
        }
        size_t n = std::min(s.struct_sizes.size(), n_components);
        for (size_t i = 0; i < n; i++) {
            resolved_type rt;
            rt.packed_sizes = s.struct_sizes[i].packed_sizes;
            rt.unpacked_sizes = s.struct_sizes[i].unpacked_sizes;
            rt.unpacked_ascending = s.struct_sizes[i].unpacked_ascending;
            rt.struct_sizes = s.struct_sizes[i].members;
            if (!s.struct_sizes[i].members.empty()) {
                rt.packed_struct = true;
            }
            d.child_sizing[i] = rt;
        }
    }
    return d;
}
}

Concatenation::Concatenation(const Concatenation &other) {

    components = other.components;
    default_initialization = other.default_initialization;
    component_keys = other.component_keys;
}

bool Concatenation::reorder_by_member_names(const std::vector<std::string> &member_names) {
    if (component_keys.empty() || default_initialization) return true;
    if (component_keys.size() != components.size()) {
        spdlog::warn("Struct literal mixes keyed and positional members; packing positionally");
        return false;
    }
    if (component_keys.size() > member_names.size()) {
        spdlog::warn("Struct literal has {} keyed members for {} struct members; packing positionally",
                     component_keys.size(), member_names.size());
        return false;
    }
    // Missing members default to zero (normal for partial keyed literals).
    auto zero = std::make_shared<Numeric_token>("0");
    std::vector<std::shared_ptr<Expression_base>> ordered(member_names.size(), zero);
    std::vector<bool> placed(member_names.size(), false);
    for (size_t i = 0; i < component_keys.size(); ++i) {
        size_t pos = 0;
        bool found = false;
        for (; pos < member_names.size(); ++pos) {
            if (member_names[pos] == component_keys[i]) { found = true; break; }
        }
        if (!found) {
            spdlog::warn("Struct literal key '{}' is not a struct member; packing positionally",
                         component_keys[i]);
            return false;
        }
        if (placed[pos]) {
            spdlog::warn("Struct literal key '{}' appears twice; packing positionally", component_keys[i]);
            return false;
        }
        placed[pos] = true;
        ordered[pos] = components[i];
    }
    components = std::move(ordered);
    component_keys.clear();
    return true;
}

Concatenation::Concatenation(Concatenation &&other) noexcept {
    components = other.components;
    default_initialization = other.default_initialization;
    component_keys = std::move(other.component_keys);
}

parameter_deps_t Concatenation::get_dependencies() const{
    parameter_deps_t result;
    for (auto &comp:components) {
        result.merge(comp->get_dependencies());
    }
    return result;
}

void Concatenation::propagate_function(const hdl_function_def_ptr &def) {
    for (auto &comp:components) {
        comp->propagate_function(def);
    }
}

std::expected<resolved_parameter, solver_errors> Concatenation::evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type){
    std::expected<resolved_parameter, solver_errors> result;
    auto concat_size = components.size();
    // Sizing comes only from the incoming container type (see
    // derive_concat_sizing); without one the construction defaults apply.
    concat_expected_sizing cur;
    cur.child_sizing.resize(components.size());
    if (expected_type) cur = derive_concat_sizing(*expected_type, components.size());
    // A struct literal with more or fewer components than the type has
    // members silently scrambles (positional assembly) or drops data.
    // `default:` fills the rest by design and is exempt.
    if (!cur.fields_sizes.empty() && !default_initialization &&
        cur.fields_sizes.size() != concat_size) {
        spdlog::warn("Struct literal has {} components for {} members; packing positionally",
                     concat_size, cur.fields_sizes.size());
    }
    if (cur.packing) {
        std::vector<int64_t> sizes;
        std::vector<hdl_integer> values;
        for (int i = 0;i<concat_size; i++) {
            int src = concat_size-i-1;
            auto value_opt = components[src]->evaluate(context, cur.child_sizing[src]);
            if (!value_opt.has_value()) return std::unexpected{missing_value};
            auto raw_value = value_opt.value();
            const bool member_array = !cur.fields_sizes.empty() && (size_t)src < cur.fields_sizes.size()
                && !cur.fields_sizes[src].unpacked_sizes.empty();
            if (!raw_value.is_integer()) {
                if (!raw_value.is_int_array() || !member_array) return std::unexpected{wrong_type};
                int64_t piece = 1;
                for (auto &ps : cur.fields_sizes[src].packed_sizes) piece *= ps;
                if (piece <= 0) return std::unexpected{wrong_type};
                const auto flat_data = raw_value.get_int_array().get_data();
                std::vector<hdl_integer> flat;
                for (auto &l2 : flat_data)
                    for (auto &l1 : l2)
                        for (auto &v : l1) flat.push_back(v);
                if (flat.empty()) return std::unexpected{missing_value};
                for (auto it = flat.rbegin(); it != flat.rend(); ++it) {
                    values.push_back(*it);
                    sizes.push_back(piece);
                }
                continue;
            }
            values.push_back(raw_value.get_integer());

            if (!cur.fields_sizes.empty() && (size_t)src < cur.fields_sizes.size()) {
                int64_t w = 1;
                for (auto &ps : cur.fields_sizes[src].packed_sizes) w *= ps;
                sizes.push_back(w);
            } else {
                auto comp_t = components[src]->resolve_expression_type(context, cur.child_sizing[src]);
                int64_t w = comp_t ? static_cast<int64_t>(packed_width(*comp_t)) : 0;
                if (w <= 0) w = raw_value.get_integer().get_size();
                sizes.push_back(w);
            }
        }
        result = pack_values(values, sizes);
        result = result->get_integer().truncate_to(cur.container_size);
    } else {
        if (components.empty())return std::unexpected{missing_value};

        bool reverse_order = cur.unpacked_ascending.empty() || !cur.unpacked_ascending.back();

        auto v = components[0]->evaluate(context, cur.child_sizing[0]);
        if (!v.has_value()) return std::unexpected{missing_value};
        if (v.value().is_string()) {
            mdarray<std::string> result_string;
            for (int64_t i = 0;i<concat_size; i++) {
                int64_t idx = reverse_order ? concat_size - i - 1 : i;
                auto value_opt = components[idx]->evaluate(context, cur.child_sizing[idx]);
                if (!value_opt.has_value()) return std::unexpected{missing_value};
                if (!value_opt.value().is_string()) {
                    spdlog::warn("Concatenating mixed string and non-string components, defaulting to 0");
                    return std::unexpected{wrong_type};
                }
                mdarray<std::string> to_concat;
                to_concat.set_value(0,value_opt.value().get_string());
                auto concat_res = mdarray<std::string>::concatenate(result_string, to_concat);
                if (!concat_res.has_value()) {
                    spdlog::warn("Concatenation of arrays with incompatible shapes, defaulting to empty");
                    return std::unexpected{missing_value};
                }
                result_string = concat_res.value();
            }
            result = result_string;
        } else {
            mdarray<hdl_integer> result_array;
            for (int64_t i = 0;i<concat_size; i++) {
                int64_t idx = reverse_order ? concat_size - i - 1 : i;
                auto value_opt = components[idx]->evaluate(context, cur.child_sizing[idx]);
                if (!value_opt.has_value()) return std::unexpected{missing_value};
                if (value_opt.value().is_integer()) {
                    mdarray<hdl_integer> to_concat;
                    to_concat.set_value(0,value_opt.value().get_integer());
                    auto concat_res = mdarray<hdl_integer>::concatenate(result_array, to_concat);
                    if (!concat_res.has_value()) {
                        spdlog::warn("Concatenation of arrays with incompatible shapes, defaulting to empty");
                        return std::unexpected{missing_value};
                    }
                    result_array = concat_res.value();
                } else if (value_opt.value().is_int_array()) {
                    auto array_res = value_opt.value().get_int_array();
                    auto concat_res = cur.unpacked_dimension.size() == 1
                        ? mdarray<hdl_integer>::concatenate(result_array, array_res)
                        : mdarray<hdl_integer>::stack(result_array, array_res);
                    if (!concat_res.has_value()) {
                        spdlog::warn("Concatenation of arrays with incompatible shapes, defaulting to empty");
                        return std::unexpected{missing_value};
                    }
                    result_array = concat_res.value();
                } else {
                    spdlog::warn("Concatenating unsupported component type, defaulting to 0");
                    return std::unexpected{wrong_type};
                }
            }
            result = result_array;
        }
    }
    if (default_initialization) {
        if (!result.has_value()) return result;
        auto dims = cur.unpacked_dimension;
        while (dims.size()<3) dims.insert(dims.begin(), 1);
        bool zero_dim = false;
        for (auto d : dims) if (d == 0) zero_dim = true;
        if (zero_dim) {
            spdlog::warn("Array default initialization with a zero dimension is not supported, defaulting to 0");
            return std::unexpected{missing_value};
        }
        if(result.value().is_int_array()) {
            auto val = result.value().get_int_array().get_scalar();
            if (!val) return std::unexpected{missing_arguments};
            mdarray result_array = {dims, val.value()};
            return result_array;
        }
        if(result.value().is_string_array()) {
            auto val = result.value().get_string_array().get_scalar();
            if (!val) return std::unexpected{missing_arguments};
            mdarray result_array = {dims, val.value()};
            return result_array;
        }
    }
    return result;
}

std::string Concatenation::print()  const{
    std::ostringstream oss;
    oss << "{";
    for (int i = 0; i< components.size(); i++) {
        if (!component_keys.empty() && i < (int)component_keys.size() && !component_keys[i].empty())
            oss << component_keys[i] << ": ";
        oss << components[i]->print();
        if (components.size() == 1) break;
        if (i<components.size()-1) oss <<", ";
    }
    oss <<"}\n";
    return oss.str();
}

std::optional<resolved_type> Concatenation::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type) const {
    // Sizing comes only from the incoming container type (see
    // derive_concat_sizing); without one the construction defaults apply.
    bool packing_l = false;
    std::vector<uint64_t> unpacked_dimension_l;
    std::vector<bool> unpacked_ascending_l;
    std::vector<std::optional<resolved_type>> child_sizing(components.size());
    if (expected_type) {
        auto derived = derive_concat_sizing(*expected_type, components.size());
        packing_l = derived.packing;
        unpacked_dimension_l = std::move(derived.unpacked_dimension);
        unpacked_ascending_l = std::move(derived.unpacked_ascending);
        child_sizing = std::move(derived.child_sizing);
    }
    resolved_type result;
    uint64_t total_bits = 0;
    bool all_real = true;
    for (size_t ci = 0; ci < components.size(); ci++) {
        auto comp_t = components[ci]->resolve_expression_type(context, child_sizing[ci]);
        if (!comp_t) return std::nullopt;
        if (!comp_t->is_real) all_real = false;
        for (auto ps : comp_t->packed_sizes) total_bits += ps;
    }
    if (all_real && !components.empty()) {
        result.is_real = true;
        return result;
    }
    if (packing_l || unpacked_dimension_l.empty()) {
        result.packed_sizes.push_back(total_bits);
        result.packed_ascending.push_back(false);
        result.packed_left.push_back(static_cast<int64_t>(total_bits) - 1);
        result.packed_right.push_back(0);
        return result;
    }
    if (!unpacked_dimension_l.empty()) {
        result.unpacked_sizes = unpacked_dimension_l;
        result.unpacked_ascending = unpacked_ascending_l;
        result.unpacked_left.clear();
        result.unpacked_right.clear();
        for (size_t i = 0; i < unpacked_dimension_l.size(); i++) {
            bool asc = i < unpacked_ascending_l.size() ? unpacked_ascending_l[i] : false;
            int64_t sz = static_cast<int64_t>(unpacked_dimension_l[i]);
            result.unpacked_left.push_back(asc ? 0 : sz - 1);
            result.unpacked_right.push_back(asc ? sz - 1 : 0);
        }
    } else {
        result.unpacked_sizes.push_back(components.size());
        result.unpacked_ascending.push_back(true);
        result.unpacked_left.push_back(0);
        result.unpacked_right.push_back(static_cast<int64_t>(components.size()) - 1);
    }
    if (total_bits > 0) {
        result.packed_sizes.push_back(total_bits);
        result.packed_ascending.push_back(false);
        result.packed_left.push_back(static_cast<int64_t>(total_bits) - 1);
        result.packed_right.push_back(0);
    }
    return result;
}

