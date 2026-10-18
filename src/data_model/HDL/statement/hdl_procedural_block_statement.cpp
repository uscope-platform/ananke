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

#include "data_model/HDL/statement/hdl_procedural_block_statement.hpp"

#include <algorithm>
#include <sstream>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/vector.hpp>

CEREAL_REGISTER_TYPE(hdl_procedural_block_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_procedural_block_statement)

bool hdl_procedural_block_statement::is_combinational() const {
    switch (kind) {
        case procedural_block_kind_t::always_comb:
        case procedural_block_kind_t::always_star:
            return true;
        case procedural_block_kind_t::always:
            return !sensitivity.empty() &&
                   std::ranges::all_of(sensitivity, [](const sensitivity_item &i) {
                       return i.edge == sensitivity_edge_t::level;
                   });
        default:
            return false;
    }
}

parameter_deps_t hdl_procedural_block_statement::get_dependencies() const {
    parameter_deps_t deps;
    for (const auto &s : sensitivity)
        if (s.signal) deps.merge(s.signal->get_dependencies());
    merge_body_deps(deps, body);
    return deps;
}

void hdl_procedural_block_statement::propagate_function(const hdl_function_def_ptr &def) {
    for (auto &s : sensitivity)
        if (s.signal) s.signal->propagate_function(def);
    propagate_body_functions(body, def);
}

bool hdl_procedural_block_statement::equals(const hdl_statement_base& other) const {
    const auto& rhs = static_cast<const hdl_procedural_block_statement&>(other);
    if (kind != rhs.kind) return false;
    auto same_expr = [](const auto &a, const auto &b) { return (!a && !b) || (a && b && *a == *b); };
    if (!std::ranges::equal(sensitivity, rhs.sensitivity,
        [&](const sensitivity_item &a, const sensitivity_item &b) {
            return a.edge == b.edge && same_expr(a.signal, b.signal);
        })) return false;
    return std::ranges::equal(body, rhs.body, same_expr);
}

std::string hdl_procedural_block_statement::print() const {
    std::ostringstream oss;
    switch (kind) {
        case procedural_block_kind_t::always_comb:  oss << "always_comb"; break;
        case procedural_block_kind_t::always_latch: oss << "always_latch"; break;
        case procedural_block_kind_t::always_ff:    oss << "always_ff"; break;
        case procedural_block_kind_t::initial:      oss << "initial"; break;
        case procedural_block_kind_t::always_star:  oss << "always @*"; break;
        case procedural_block_kind_t::always:       oss << "always"; break;
    }
    if (!sensitivity.empty()) {
        oss << " @(";
        for (size_t i = 0; i < sensitivity.size(); ++i) {
            if (i != 0) oss << " or ";
            if (sensitivity[i].edge == sensitivity_edge_t::posedge) oss << "posedge ";
            if (sensitivity[i].edge == sensitivity_edge_t::negedge) oss << "negedge ";
            if (sensitivity[i].signal) oss << sensitivity[i].signal->print();
        }
        oss << ")";
    }
    oss << " begin\n";
    for (const auto &s : body)
        if (s) oss << s->print() << "\n";
    oss << "end";
    return oss.str();
}
