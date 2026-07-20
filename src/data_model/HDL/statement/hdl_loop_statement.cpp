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

#include <algorithm>

parameter_deps_t hdl_loop_statement::get_dependencies() const {
    parameter_deps_t deps;
    deps.merge(init->get_dependencies());
    deps.merge(end_condition->get_dependencies());
    deps.merge(iteration->get_dependencies());
    for (const auto& stmt : loop_body)
        deps.merge(stmt->get_dependencies());
    return deps;
}

std::unique_ptr<hdl_statement_base> hdl_loop_statement::clone() const {
    auto c = std::make_unique<hdl_loop_statement>();
    c->init = std::make_shared<HDL_parameter>(*init);
    c->end_condition = end_condition;
    c->iteration = iteration;
    for (const auto& s : loop_body)
        c->loop_body.push_back(s->clone());
    return c;
}

bool hdl_loop_statement::equals(const hdl_statement_base &other) const {
    const auto& rhs = static_cast<const hdl_loop_statement&>(other);

    bool res = *init == *rhs.init;
    res &= *end_condition == *rhs.end_condition;
    res &= *iteration == *rhs.iteration;
    res &= std::ranges::equal(loop_body, rhs.loop_body,
        [](const auto& a, const auto& b) { return *a == *b; });
    return res;
}

std::string hdl_loop_statement::print() const {
    std::ostringstream oss;
    oss << "for( "<< init->to_string() << " ; " << end_condition->print()  << " ; " << iteration->print()<< " ) begin\n";
    for (auto &stmt:loop_body) {
        oss << stmt->print() << "\n";
    }
    oss << "end";
    return oss.str();
}

void PrintTo(const hdl_loop_statement& s, std::ostream* os) {
    *os << s.print();
}
