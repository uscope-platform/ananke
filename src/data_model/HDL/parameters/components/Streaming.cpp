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

#include "data_model/HDL/parameters/components/Streaming.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"

#include <sstream>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Streaming)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, Streaming)

parameter_deps_t Streaming::get_dependencies() const {
    parameter_deps_t result;
    for (const auto &comp : components) result.merge(comp->get_dependencies());
    if (slice_size) result.merge(slice_size->get_dependencies());
    return result;
}
void Streaming::propagate_function(const hdl_function_def_ptr &def) {
    for (auto &comp : components) comp->propagate_function(def);
    if (slice_size) slice_size->propagate_function(def);
}

std::expected<resolved_parameter, solver_errors> Streaming::evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type) {
    if (components.empty()) return std::unexpected{missing_arguments};

    // Mirror of set_container_sizes: the incoming container type was
    // forwarded to components and slice size unchanged.
    // Evaluate all components; use declared literal width when available.
    std::vector<hdl_integer> values;
    std::vector<int64_t> widths;
    int64_t total_width = 0;
    for (const auto &comp : components) {
        auto v = comp->evaluate(context, expected_type);
        if (!v.has_value() || !v.value().is_integer()) return std::unexpected{missing_value};
        auto raw = v.value().get_integer();
        int64_t w = 0;
        auto comp_t = comp->resolve_expression_type(context, expected_type);
        if (comp_t) w = static_cast<int64_t>(packed_width(*comp_t));
        if (w <= 0) w = raw.get_size();
        widths.push_back(w);
        values.push_back(raw);
        total_width += w;
    }

    // Build P = MSB-first concatenation: components[0] is the most significant.
    hdl_integer P = 0;
    int64_t shift = total_width;
    for (size_t i = 0; i < values.size(); i++) {
        shift -= widths[i];
        auto masked = values[i].truncate_to(widths[i]);
        P = P | (masked << hdl_integer(shift));
    }

    // Determine slice size (default 1).
    int64_t slice = 1;
    if (slice_size) {
        auto s = slice_size->evaluate(context, expected_type);
        if (s.has_value() && s.value().is_integer() && s.value().get_integer().get_value() > 0)
            slice = s.value().get_integer().get_value();
    }

    hdl_integer result = 0;
    if (direction == right) {
        // Right-to-left streaming (>>): left-to-right order, slice_size ignored, no re-ordering.
        result = P;
    } else {
        // Left-to-right streaming (<<): slice into blocks starting from the right-most bit,
        // then reverse the block order, preserving bits within each block.
        std::vector<hdl_integer> blocks;      // blocks[0] = right-most (LSB) block
        std::vector<int64_t> block_widths;
        for (int64_t off = 0; off < total_width; off += slice) {
            int64_t w = std::min<int64_t>(slice, total_width - off);
            blocks.push_back((P >> hdl_integer(off)).truncate_to(w));
            block_widths.push_back(w);
        }
        int64_t out_shift = 0;
        for (int64_t i = static_cast<int64_t>(blocks.size()) - 1; i >= 0; i--) {
            result = result | (blocks[i] << hdl_integer(out_shift));
            out_shift += block_widths[i];
        }
    }

    return resolved_parameter(result);
}

std::string Streaming::print() const {
    std::ostringstream oss;
    oss << "{";
    if (direction == left) oss << "<<";
    else oss << ">>";
    if (slice_size) oss << slice_size->print();
    oss << "{";
    for (size_t i = 0; i < components.size(); i++) {
        if (i > 0) oss << ", ";
        oss << components[i]->print();
    }
    oss << "}}";
    return oss.str();
}

std::optional<resolved_type> Streaming::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type) const {
    uint64_t total_bits = 0;
    for (const auto &comp : components) {
        // Mirror of set_container_sizes: the incoming container type was
        // forwarded to components unchanged.
        auto comp_t = comp->resolve_expression_type(context, expected_type);
        if (!comp_t) return std::nullopt;
        if (comp_t->is_real) {
            resolved_type result;
            result.is_real = true;
            return result;
        }
        for (auto ps : comp_t->packed_sizes) total_bits += ps;
    }
    resolved_type result;
    result.packed_sizes.push_back(total_bits);
    result.packed_ascending.push_back(false);
    result.packed_left.push_back(static_cast<int64_t>(total_bits) - 1);
    result.packed_right.push_back(0);
    return result;
}

bool operator==(const Streaming &lhs, const Streaming &rhs) {
    if (lhs.direction != rhs.direction) return false;
    if (lhs.components.size() != rhs.components.size()) return false;
    for (size_t i = 0; i < lhs.components.size(); i++) {
        if (!(*lhs.components[i] == *rhs.components[i])) return false;
    }
    if (lhs.slice_size && rhs.slice_size) {
        if (!(*lhs.slice_size == *rhs.slice_size)) return false;
    } else if (lhs.slice_size || rhs.slice_size) {
        return false;
    }
    return true;
}

bool Streaming::isEqual(const Expression_base &other) const {
    const auto &rhs = static_cast<const Streaming &>(other);
    return *this == rhs;
}
