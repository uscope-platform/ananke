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

#ifndef ANANKE_HDL_CONTINUOUS_ASSIGNMENT_HPP
#define ANANKE_HDL_CONTINUOUS_ASSIGNMENT_HPP

#include <memory>
#include <string>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/statement/hdl_lvalue.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

// Module-level `assign` (or VHDL concurrent assignment): a persistent driver
// of a net, with no evaluation order. Deliberately not an
// hdl_assignment_statement, which the function evaluator interprets as a
// sequential write. `assign a = x, b = y;` is stored as one statement per pair.
class hdl_continuous_assignment : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_def_ptr &def) override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_lvalue(const hdl_lvalue &l) { lvalue = l; }
    [[nodiscard]] const hdl_lvalue &get_lvalue() const { return lvalue; }
    [[nodiscard]] hdl_lvalue &get_lvalue() { return lvalue; }

    void set_value(const std::shared_ptr<Expression_base>& v) { value = v; }
    [[nodiscard]] std::shared_ptr<Expression_base> get_value() const { return value; }

    void set_line_n(int l) { line_n = l; }
    [[nodiscard]] int get_line_n() const { return line_n; }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(lvalue, value, line_n);
    }
private:
    hdl_lvalue lvalue;
    std::shared_ptr<Expression_base> value;
    int line_n = 0;
};

#endif //ANANKE_HDL_CONTINUOUS_ASSIGNMENT_HPP
