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

#ifndef ANANKE_HDL_ASSIGNMENT_STATEMENT_HPP
#define ANANKE_HDL_ASSIGNMENT_STATEMENT_HPP

#include <memory>
#include <string>
#include <vector>
#include <cstdint>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/statement/hdl_lvalue.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

// Sequential (blocking) assignment: function bodies and procedural blocks.
// Continuous `assign` statements are a separate type, hdl_continuous_assignment.
class hdl_assignment_statement : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_def_ptr &def) override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    // The lvalue accessors below forward to the embedded hdl_lvalue.
    void set_target(const std::string& n) { lvalue.set_target(n); }
    void set_target(const qualified_identifier& q) { lvalue.set_target(q); }
    void set_targets(const std::vector<qualified_identifier>& qs) { lvalue.set_targets(qs); }
    const std::vector<qualified_identifier> &get_targets() const { return lvalue.get_targets(); }

    void set_index(const std::shared_ptr<Expression_base>& idx) { lvalue.set_index(idx); }
    void set_indices(const std::vector<std::shared_ptr<Expression_base>>& idxs) { lvalue.set_indices(idxs); }
    const std::vector<std::shared_ptr<Expression_base>> &get_indices() const { return lvalue.get_indices(); }

    void set_member_selects(const std::vector<concat_member_select> &sels) { lvalue.set_member_selects(sels); }
    const std::vector<concat_member_select> &get_member_selects() const { return lvalue.get_member_selects(); }

    void set_lvalue(const hdl_lvalue &l) { lvalue = l; }
    const hdl_lvalue &get_lvalue() const { return lvalue; }

    void set_value(const std::shared_ptr<Expression_base>& v) { value = v; }
    std::shared_ptr<Expression_base> get_value() const { return value; }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(lvalue, value);
    }
private:
    hdl_lvalue lvalue;
    std::shared_ptr<Expression_base> value;
};

#endif //ANANKE_HDL_ASSIGNMENT_STATEMENT_HPP
