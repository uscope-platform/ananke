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

#ifndef ANANKE_HDL_LVALUE_HPP
#define ANANKE_HDL_LVALUE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

// Literal `[msb:lsb]` bounds captured from a concatenation member's select
// syntax at parse time. Range selects vanish before storage everywhere else
// in the pipeline, so without this the unpacker could not recover them.
// (-1, -1) means "no literal bounds" (bare, single-bit or dynamic member).
// `dynamic_select` marks a present-but-non-literal select (part-selects,
// parametric bounds): the member is never width-resolved, not even via
// declaration lookup, so it cannot be mistaken for a bare member.
struct concat_member_select {
    int64_t hi = -1;
    int64_t lo = -1;
    bool dynamic_select = false;
    [[nodiscard]] bool has_bounds() const { return hi >= 0 && lo >= 0; }
    template<class Archive> void serialize(Archive & ar) { ar(hi, lo, dynamic_select); }
};

// The write side of an assignment, shared by every statement that drives a
// target (function/procedural assignments, continuous assigns).
//
// Write targets are parallel with indices (an index entry may be null).
// Concatenation lvalues record every member; every other producer records
// exactly one. The factory always populates both vectors.
class hdl_lvalue {
public:
    void set_target(const std::string& n) { targets = {qualified_identifier(n)}; }
    void set_target(const qualified_identifier& q) { targets = {q}; }
    void set_targets(const std::vector<qualified_identifier>& qs) { targets = qs; }
    [[nodiscard]] const std::vector<qualified_identifier> &get_targets() const { return targets; }

    void set_index(const std::shared_ptr<Expression_base>& idx) { indices = {idx}; }
    void set_indices(const std::vector<std::shared_ptr<Expression_base>>& idxs) { indices = idxs; }
    [[nodiscard]] const std::vector<std::shared_ptr<Expression_base>> &get_indices() const { return indices; }

    // Per-member literal bounds, parallel to targets. Only concatenation
    // assignments populate it (see concat_member_select); everyone else
    // leaves it empty, which compares equal to all-absent.
    void set_member_selects(const std::vector<concat_member_select> &sels) { member_selects = sels; }
    [[nodiscard]] const std::vector<concat_member_select> &get_member_selects() const { return member_selects; }

    // Dependencies of the index expressions (the targets themselves are writes).
    [[nodiscard]] parameter_deps_t get_dependencies() const;
    void propagate_function(const hdl_function_def_ptr &def);
    [[nodiscard]] bool equals(const hdl_lvalue &other) const;
    [[nodiscard]] std::string print() const;

    friend bool operator==(const hdl_lvalue &a, const hdl_lvalue &b) { return a.equals(b); }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(targets, indices, member_selects);
    }
private:
    std::vector<qualified_identifier> targets;
    std::vector<std::shared_ptr<Expression_base>> indices;
    std::vector<concat_member_select> member_selects;
};

#endif //ANANKE_HDL_LVALUE_HPP
