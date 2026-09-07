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

#include "data_model/HDL/statement/hdl_conditional_statement.hpp"

#include <algorithm>
#include <sstream>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_conditional_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_conditional_statement)

void hdl_conditional_statement::add_branch(const std::shared_ptr<Expression_base>& cond) {
    branches.push_back({cond, {}});
}

void hdl_conditional_statement::set_condition(const std::shared_ptr<Expression_base>& e) {
    if (branches.empty())
        branches.push_back({e, {}});
    else
        branches.back().condition = e;
}

void hdl_conditional_statement::add_to_branch(const std::shared_ptr<hdl_statement_base>& s) {
    if (branches.empty())
        branches.push_back({nullptr, {s}});
    else
        branches.back().body.push_back(s);
}

void hdl_conditional_statement::add_to_else(const std::shared_ptr<hdl_statement_base>& s) {
    else_body.push_back(s);
}

parameter_deps_t hdl_conditional_statement::get_dependencies() const {
    parameter_deps_t deps;
    for (const auto &b : branches) {
        if (b.condition)
            deps.merge(b.condition->get_dependencies());
        for (const auto &stmt : b.body)
            deps.merge(stmt->get_dependencies());
    }
    for (const auto &stmt : else_body)
        deps.merge(stmt->get_dependencies());
    return deps;
}

void hdl_conditional_statement::propagate_function(const hdl_function_def_ptr &def) {
    for (auto &b : branches) {
        if (b.condition)
            b.condition->propagate_function(def);
        for (auto &stmt : b.body)
            if (stmt) stmt->propagate_function(def);
    }
    for (auto &stmt : else_body)
        if (stmt) stmt->propagate_function(def);
}

std::unique_ptr<hdl_statement_base> hdl_conditional_statement::clone() const {
    auto c = std::make_unique<hdl_conditional_statement>();
    for (const auto &b : branches) {
        hdl_conditional_branch cb;
        cb.condition = b.condition;
        for (const auto &s : b.body)
            cb.body.push_back(s->clone());
        c->branches.push_back(std::move(cb));
    }
    for (const auto &s : else_body)
        c->else_body.push_back(s->clone());
    return c;
}

bool hdl_conditional_statement::equals(const hdl_statement_base& other) const {
    const auto& rhs = static_cast<const hdl_conditional_statement&>(other);

    if (branches.size() != rhs.branches.size()) return false;
    for (size_t i = 0; i < branches.size(); i++) {
        bool cond_eq = (!branches[i].condition && !rhs.branches[i].condition) ||
                       (branches[i].condition && rhs.branches[i].condition &&
                        *branches[i].condition == *rhs.branches[i].condition);
        if (!cond_eq) return false;
        if (!std::ranges::equal(branches[i].body, rhs.branches[i].body,
            [](const auto& a, const auto& b) { return *a == *b; }))
            return false;
    }

    return std::ranges::equal(else_body, rhs.else_body,
        [](const auto& a, const auto& b) { return *a == *b; });
}

std::string hdl_conditional_statement::print() const {
    std::ostringstream oss;
    for (size_t i = 0; i < branches.size(); i++) {
        oss << (i == 0 ? "if (" : "else if (");
        if (branches[i].condition) oss << branches[i].condition->print();
        oss << ") begin\n";
        for (auto &stmt : branches[i].body)
            oss << stmt->print() << "\n";
        oss << "end ";
    }
    if (!else_body.empty()) {
        oss << "else begin\n";
        for (auto &stmt : else_body)
            oss << stmt->print() << "\n";
        oss << "end";
    }
    return oss.str();
}

void PrintTo(const hdl_conditional_statement& s, std::ostream* os) {
    *os << s.print();
}

bool hdl_conditional_statement::is_empty() const {
    for (const auto &b : branches)
        if (!b.body.empty()) return false;
    return else_body.empty();
}

void hdl_conditional_statement::flatten() {
    if (else_body.size() != 1) return;
    auto inner = std::dynamic_pointer_cast<hdl_conditional_statement>(else_body[0]);
    if (!inner) return;

    inner->flatten();

    branches.reserve(branches.size() + inner->branches.size());
    for (auto &b : inner->branches)
        branches.push_back(std::move(b));
    else_body = std::move(inner->else_body);
}
