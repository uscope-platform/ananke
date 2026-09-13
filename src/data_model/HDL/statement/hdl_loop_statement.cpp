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

#include "data_model/HDL/statement/hdl_loop_statement.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"

#include <algorithm>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_loop_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_loop_statement)

parameter_deps_t hdl_loop_statement::get_dependencies() const {
    parameter_deps_t deps;
    if (init) deps.merge(init->get_dependencies());
    if (end_condition) deps.merge(end_condition->get_dependencies());
    if (iteration) deps.merge(iteration->get_dependencies());
    for (const auto& stmt : loop_body)
        if (stmt) deps.merge(stmt->get_dependencies());
    return deps;
}

void hdl_loop_statement::propagate_function(const hdl_function_def_ptr &def) {
    if (init) init->propagate_function(def);
    if (end_condition) end_condition->propagate_function(def);
    if (iteration) iteration->propagate_function(def);
    for (auto &stmt : loop_body)
        if (stmt) stmt->propagate_function(def);
}

bool hdl_loop_statement::equals(const hdl_statement_base &other) const {
    const auto& rhs = static_cast<const hdl_loop_statement&>(other);

    bool res = static_cast<bool>(init) == static_cast<bool>(rhs.init)
               && static_cast<bool>(end_condition) == static_cast<bool>(rhs.end_condition)
               && static_cast<bool>(iteration) == static_cast<bool>(rhs.iteration);
    if (init && rhs.init) res &= *init == *rhs.init;
    if (end_condition && rhs.end_condition) res &= *end_condition == *rhs.end_condition;
    if (iteration && rhs.iteration) res &= *iteration == *rhs.iteration;
    res &= std::ranges::equal(loop_body, rhs.loop_body,
        [](const auto& a, const auto& b) { if (!a || !b) return a == b; return *a == *b; });
    return res;
}

std::string hdl_loop_statement::print() const {
    std::ostringstream oss;
    oss << "for( " << (init ? init->to_string() : "") << " ; "
        << (end_condition ? end_condition->print() : "") << " ; "
        << (iteration ? iteration->print() : "") << " ) begin\n";
    for (auto &stmt:loop_body) {
        if (stmt) oss << stmt->print() << "\n";
    }
    oss << "end";
    return oss.str();
}

void PrintTo(const hdl_loop_statement& s, std::ostream* os) {
    *os << s.print();
}
