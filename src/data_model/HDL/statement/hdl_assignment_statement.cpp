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

#include "data_model/HDL/statement/hdl_assignment_statement.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/string.hpp>
#include <cereal/types/memory.hpp>

#include <algorithm>

CEREAL_REGISTER_TYPE(hdl_assignment_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_assignment_statement)

parameter_deps_t hdl_assignment_statement::get_dependencies() const {
    parameter_deps_t deps;
    if (value) deps.merge(value->get_dependencies());
    for (const auto &idx : indices) {
        if (idx) deps.merge(idx->get_dependencies());
    }
    return deps;
}

void hdl_assignment_statement::propagate_function(const hdl_function_def_ptr &def) {
    if (value) value->propagate_function(def);
    for (const auto &idx : indices) {
        if (idx) idx->propagate_function(def);
    }
}

static bool same_index(const std::shared_ptr<Expression_base> &a,
                       const std::shared_ptr<Expression_base> &b) {
    return (!a && !b) || (a && b && *a == *b);
}

bool hdl_assignment_statement::equals(const hdl_statement_base& other) const {
    const auto& rhs = static_cast<const hdl_assignment_statement&>(other);
    if (targets.size() != rhs.targets.size()) return false;
    for (size_t i = 0; i < targets.size(); ++i) {
        if (!(targets[i] == rhs.targets[i])) return false;
    }
    // A missing slot and an explicit null both mean "whole variable": the
    // factory always populates parallel slots, hand-built statements often
    // omit them.
    size_t n_indices = std::max(indices.size(), rhs.indices.size());
    for (size_t i = 0; i < n_indices; ++i) {
        std::shared_ptr<Expression_base> a = i < indices.size() ? indices[i] : nullptr;
        std::shared_ptr<Expression_base> b = i < rhs.indices.size() ? rhs.indices[i] : nullptr;
        if (!same_index(a, b)) return false;
    }
    bool res = (!value && !rhs.value) || (value && rhs.value && *value == *rhs.value);
    return res;
}

std::string hdl_assignment_statement::print() const {
    std::ostringstream oss;
    if (targets.size() == 1) {
        oss << targets.front().print();
        if (!indices.empty() && indices.front()) oss << "[" << indices.front()->print() << "]";
    } else if (!targets.empty()) {
        oss << "{";
        for (size_t i = 0; i < targets.size(); ++i) {
            if (i != 0) oss << ", ";
            oss << targets[i].print();
            if (i < indices.size() && indices[i]) oss << "[" << indices[i]->print() << "]";
        }
        oss << "}";
    }
    oss << " = ";
    if (value) oss << value->print();
    return oss.str();
}
