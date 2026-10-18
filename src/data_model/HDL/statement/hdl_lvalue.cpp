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

#include "data_model/HDL/statement/hdl_lvalue.hpp"

#include <algorithm>
#include <sstream>

parameter_deps_t hdl_lvalue::get_dependencies() const {
    parameter_deps_t deps;
    for (const auto &idx : indices) {
        if (idx) deps.merge(idx->get_dependencies());
    }
    return deps;
}

void hdl_lvalue::propagate_function(const hdl_function_def_ptr &def) {
    for (const auto &idx : indices) {
        if (idx) idx->propagate_function(def);
    }
}

static bool same_index(const std::shared_ptr<Expression_base> &a,
                       const std::shared_ptr<Expression_base> &b) {
    return (!a && !b) || (a && b && *a == *b);
}

bool hdl_lvalue::equals(const hdl_lvalue &rhs) const {
    if (targets.size() != rhs.targets.size()) return false;
    for (size_t i = 0; i < targets.size(); ++i) {
        if (!(targets[i] == rhs.targets[i])) return false;
    }
    // A missing slot and an explicit null both mean "whole variable": the
    // factory always populates parallel slots, hand-built statements often
    // omit them. Same for member bounds (absent ≡ all-absent).
    size_t n_indices = std::max(indices.size(), rhs.indices.size());
    for (size_t i = 0; i < n_indices; ++i) {
        std::shared_ptr<Expression_base> a = i < indices.size() ? indices[i] : nullptr;
        std::shared_ptr<Expression_base> b = i < rhs.indices.size() ? rhs.indices[i] : nullptr;
        if (!same_index(a, b)) return false;
    }
    size_t n_selects = std::max(member_selects.size(), rhs.member_selects.size());
    for (size_t i = 0; i < n_selects; ++i) {
        concat_member_select a = i < member_selects.size() ? member_selects[i] : concat_member_select{};
        concat_member_select b = i < rhs.member_selects.size() ? rhs.member_selects[i] : concat_member_select{};
        if (a.has_bounds() != b.has_bounds() || a.hi != b.hi || a.lo != b.lo) return false;
        if (a.dynamic_select != b.dynamic_select) return false;
    }
    return true;
}

std::string hdl_lvalue::print() const {
    auto print_bounds = [this](size_t i, std::ostringstream &out) {
        if (i < member_selects.size() && member_selects[i].has_bounds())
            out << "[" << member_selects[i].hi << ":" << member_selects[i].lo << "]";
    };
    std::ostringstream oss;
    if (targets.size() == 1) {
        oss << targets.front().print();
        if (!indices.empty() && indices.front()) oss << "[" << indices.front()->print() << "]";
        print_bounds(0, oss);
    } else if (!targets.empty()) {
        oss << "{";
        for (size_t i = 0; i < targets.size(); ++i) {
            if (i != 0) oss << ", ";
            oss << targets[i].print();
            if (i < indices.size() && indices[i]) oss << "[" << indices[i]->print() << "]";
            print_bounds(i, oss);
        }
        oss << "}";
    }
    return oss.str();
}
