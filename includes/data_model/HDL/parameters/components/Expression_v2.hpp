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


#ifndef ANANKE_EXPRESSION_V2_HPP
#define ANANKE_EXPRESSION_V2_HPP

#include <map>
#include <spdlog/spdlog.h>

#include "Expression_base.hpp"


class Expression_v2 : public Expression_base {
public:

    enum expression_operator {
        none,
        logic_neg,
        bitwise_neg,
        reduction_and,
        reduction_nand,
        reduction_or,
        reduction_nor,
        reduction_xor,
        reduction_xnor,
        power,
        multiply,
        divide,
        modulo,
        add,
        subtract,
        logic_shift_left,
        logic_shift_right,
        arithmetic_shift_left,
        arithmetic_shift_right,
        rotate_left,
        rotate_right,
        v_mod,
        greater,
        greater_equal,
        less,
        less_equal,
        equal,
        not_equal,
        case_equal,
        case_not_equal,
        wildcard_equal,
        wildcard_not_equal,
        bitwise_and,
        bitwise_xor,
        bitwise_xnor,
        bitwise_or,
        logical_and,
        logical_or,
        abs_value,
        condition_op
    };

    inline static const std::map<expression_operator, std::string> op_to_str = {
        {logic_neg, "!"},
        {bitwise_neg, "~"},
        {reduction_and, "&"},
        {reduction_nand, "~&"},
        {reduction_or, "|"},
        {reduction_nor, "~|"},
        {reduction_xor, "^"},
        {reduction_xnor, "~^"},
        {power, "**"},
        {multiply, "*"},
        {divide, "/"},
        {modulo, "%"},
        {add, "+"},
        {subtract, "-"},
        {logic_shift_left, "<<"},
        {logic_shift_right, ">>"},
        {arithmetic_shift_left, "<<<"},
        {arithmetic_shift_right, ">>>"},
        {rotate_left, "rol"},
        {rotate_right, "ror"},
        {v_mod, "mod"},
        {greater, ">"},
        {greater_equal, ">="},
        {less, "<"},
        {less_equal, "<="},
        {equal, "=="},
        {not_equal, "!="},
        {case_equal, "==="},
        {case_not_equal, "!=="},
        {wildcard_equal, "==?"},
        {wildcard_not_equal, "!=?"},
        {bitwise_and, "&"},
        {bitwise_xor, "^"},
        {bitwise_xnor, "~^"},
        {bitwise_or, "|"},
        {logical_and, "&&"},
        {logical_or, "||"},
        {abs_value, "abs"},
        {condition_op, "??"}
    };

    inline static const std::map<expression_operator, int> op_precedence = {
        {power, 10},
        {multiply, 8}, {divide, 8}, {modulo, 8},
        {add, 7}, {subtract, 7},
        {logic_shift_left, 6}, {logic_shift_right, 6},
        {arithmetic_shift_left, 6}, {arithmetic_shift_right, 6},
        {rotate_left, 6}, {rotate_right, 6},
        {v_mod, 8},
        {greater, 5}, {greater_equal, 5}, {less, 5}, {less_equal, 5},
        {equal, 4}, {not_equal, 4}, {case_equal, 4}, {case_not_equal, 4},
        {wildcard_equal, 4}, {wildcard_not_equal, 4},
        {bitwise_and, 3},
        {bitwise_xor, 2}, {bitwise_xnor, 2},
        {bitwise_or, 1},
        {logical_and, 0}, {logical_or, 0}
    };

    Expression_v2() = default;
    static std::shared_ptr<Expression_base> unwrap(Expression_v2 expr);
    void set_lhs(const std::shared_ptr<Expression_base> &comp){lhs = comp;}
    void set_rhs(const std::shared_ptr<Expression_base> &comp) {rhs = comp;}
    std::shared_ptr<Expression_base> get_lhs(){return lhs;}
    std::shared_ptr<Expression_base> get_rhs(){return rhs;}
    [[nodiscard]] std::shared_ptr<const Expression_base> get_lhs() const {return lhs;}
    [[nodiscard]] std::shared_ptr<const Expression_base> get_rhs() const {return rhs;}
    void set_operation(const expression_operator &op) {operation = op;}
    [[nodiscard]] expression_operator get_operation() const {return operation;}

    friend bool operator==(const Expression_v2 &lhs, const Expression_v2 &rhs);
    std::string print() const override;
    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;
    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const override;
    std::expected<resolved_parameter, solver_errors> evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) override;
    parameter_deps_t get_dependencies()const override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Expression_base> &value) override;
    void propagate_function(const hdl_function_statement &def) override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(lhs, rhs, operation);
    }

private:

    bool isEqual(const Expression_base& other) const override;
    std::variant<hdl_integer, double> evaluate_binary_expression(resolved_parameter op_a, resolved_parameter op_b);
    std::variant<hdl_integer, double> evaluate_rotate(
        const std::map<qualified_identifier, resolved_parameter> &context,
        resolved_parameter op_a, resolved_parameter op_b,
        const std::optional<resolved_type> &operand_sizing = std::nullopt);
    std::variant<hdl_integer, double> evaluate_unary_expression(resolved_parameter operand);

    std::shared_ptr<Expression_base> lhs;
    std::shared_ptr<Expression_base> rhs;
    expression_operator operation = none;
};




#endif //ANANKE_EXPRESSION_V2_HPP
