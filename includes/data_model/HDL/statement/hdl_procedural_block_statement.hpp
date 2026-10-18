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

#ifndef ANANKE_HDL_PROCEDURAL_BLOCK_STATEMENT_HPP
#define ANANKE_HDL_PROCEDURAL_BLOCK_STATEMENT_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

enum class procedural_block_kind_t : uint8_t {
    always_comb,
    always_star,   // always @* / always @(*)
    always_latch,
    always_ff,
    always,        // plain always with an explicit sensitivity list
    initial
};

enum class sensitivity_edge_t : uint8_t {
    level,
    posedge,
    negedge
};

struct sensitivity_item {
    sensitivity_edge_t edge = sensitivity_edge_t::level;
    std::shared_ptr<Expression_base> signal;

    template<class Archive>
    void serialize(Archive & ar) { ar(edge, signal); }
};

// An always/initial block. Only combinational blocks are produced by the
// frontend for now (see is_combinational()), but every kind is representable
// so later sequential tracking does not change the cache schema again.
// The body holds hdl_assignment_statement / hdl_conditional_statement / loop
// statements, the same containers used by function bodies.
class hdl_procedural_block_statement : public hdl_statement_base {
public:
    hdl_procedural_block_statement() = default;
    explicit hdl_procedural_block_statement(procedural_block_kind_t k) : kind(k) {}

    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_def_ptr &def) override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_kind(procedural_block_kind_t k) { kind = k; }
    [[nodiscard]] procedural_block_kind_t get_kind() const { return kind; }

    // always_comb, always @*, or a plain always whose sensitivity list is
    // non-empty and level-only.
    [[nodiscard]] bool is_combinational() const;

    void add_sensitivity(sensitivity_edge_t edge, const std::shared_ptr<Expression_base> &signal) {
        sensitivity.push_back({edge, signal});
    }
    [[nodiscard]] const std::vector<sensitivity_item> &get_sensitivity() const { return sensitivity; }

    void add_statement(const std::shared_ptr<hdl_statement_base> &s) { body.push_back(s); }
    [[nodiscard]] const hdl_statement_body &get_body() const { return body; }

    void set_line_n(int l) { line_n = l; }
    [[nodiscard]] int get_line_n() const { return line_n; }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(kind, sensitivity, body, line_n);
    }
private:
    procedural_block_kind_t kind = procedural_block_kind_t::always_comb;
    std::vector<sensitivity_item> sensitivity;
    hdl_statement_body body;
    int line_n = 0;
};

#endif //ANANKE_HDL_PROCEDURAL_BLOCK_STATEMENT_HPP
