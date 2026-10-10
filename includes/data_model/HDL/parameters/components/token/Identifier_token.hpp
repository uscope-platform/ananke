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

#ifndef ANANKE_IDENTIFIER_TOKEN_HPP
#define ANANKE_IDENTIFIER_TOKEN_HPP

#include <bitset>
#include <cstdint>
#include "data_model/HDL/parameters/common/qualified_identifier.hpp"

#include <cereal/types/vector.hpp>
#include <cereal/types/variant.hpp>

#include "data_model/HDL/parameters/components/Expression_base.hpp"

class hdl_type;

class Identifier_token :public Expression_base{
public:
    Identifier_token() = default;
    Identifier_token(const Identifier_token &c);

    explicit Identifier_token(const qualified_identifier &q_i);

    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_def_ptr &def) override;
    std::expected<resolved_parameter, solver_errors> evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) override;

    bool is_subscripted() const {return !array_index.empty();}

    std::string print() const override;

    friend bool operator==(const Identifier_token &lhs, const Identifier_token &rhs);

    void set_array_index(const std::vector<std::shared_ptr<Expression_base>> &v) {array_index = v;}
    void add_array_index(const std::shared_ptr<Expression_base> &a_i) {array_index.push_back(a_i);}
    std::vector<std::shared_ptr<Expression_base>> get_array_index() {return array_index;}

    // Literal `[msb:lsb]` range read (`x[7:4]`), sliced at evaluation.
    // Absent (-1) means no range: single-bit selects ride array_index as
    // before, everything else reads whole. Bounds normalize here so all
    // consumers see hi >= lo.
    void set_range(int64_t hi, int64_t lo) {
        if (hi < lo) { range_hi = lo; range_lo = hi; }
        else { range_hi = hi; range_lo = lo; }
    }
    bool has_range() const { return range_hi >= 0 && range_lo >= 0; }

    qualified_identifier get_value() const {return id;}

    void set_expression_type(const std::shared_ptr<hdl_type> &t) { expression_type = t; }
    std::shared_ptr<hdl_type> get_expression_type() const { return expression_type; }
    // Marks an identifier that stands in for a *type* (e.g. the argument of
    // $bits(logic [7:0])) rather than a readable value: it carries no data
    // dependency, the type itself rides along as expression_type.
    void set_type_placeholder(bool v) { type_placeholder = v; }
    bool is_type_placeholder() const { return type_placeholder; }

    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(id, array_index, type_placeholder, expression_type, range_hi, range_lo);
    }

private:
    std::string print_index(const std::vector<std::shared_ptr<Expression_base>> &index) const;

    bool isEqual(const Expression_base& other) const override;

    qualified_identifier id;
    std::vector<std::shared_ptr<Expression_base>> array_index;
    int64_t range_hi = -1;
    int64_t range_lo = -1;
    std::shared_ptr<hdl_type> expression_type;
    bool type_placeholder = false;
};

#endif //ANANKE_IDENTIFIER_TOKEN_HPP
