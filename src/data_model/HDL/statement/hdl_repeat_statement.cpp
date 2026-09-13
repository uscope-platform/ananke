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

#include "data_model/HDL/statement/hdl_repeat_statement.hpp"

#include <sstream>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_repeat_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_repeat_statement)

parameter_deps_t hdl_repeat_statement::get_dependencies() const {
    parameter_deps_t deps;
    if (count) deps.merge(count->get_dependencies());
    for (const auto& stmt : loop_body)
        if (stmt) deps.merge(stmt->get_dependencies());
    return deps;
}

void hdl_repeat_statement::propagate_function(const hdl_function_def_ptr &def) {
    if (count) count->propagate_function(def);
    for (auto &stmt : loop_body)
        if (stmt) stmt->propagate_function(def);
}

bool hdl_repeat_statement::equals(const hdl_statement_base &other) const {
    const auto& rhs = static_cast<const hdl_repeat_statement&>(other);

    if (static_cast<bool>(count) != static_cast<bool>(rhs.count)) return false;
    if (count && rhs.count) {
        if (!(*count == *rhs.count)) return false;
    }
    return std::equal(loop_body.begin(), loop_body.end(), rhs.loop_body.begin(), rhs.loop_body.end(),
        [](const auto& a, const auto& b) { if (!a || !b) return a == b; return *a == *b; });
}

std::string hdl_repeat_statement::print() const {
    std::ostringstream oss;
    oss << "repeat( " << (count ? count->print() : "") << " ) begin\n";
    for (auto &stmt:loop_body) {
        if (stmt) oss << stmt->print() << "\n";
    }
    oss << "end";
    return oss.str();
}

void PrintTo(const hdl_repeat_statement& s, std::ostream* os) {
    *os << s.print();
}
