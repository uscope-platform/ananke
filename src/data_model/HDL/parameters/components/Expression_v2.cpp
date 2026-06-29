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

#include <sstream>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

#include "data_model/HDL/parameters/components/Expression_v2.hpp"

CEREAL_REGISTER_TYPE(Expression_v2)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Parameter_value_base, Expression_v2)

std::shared_ptr<Parameter_value_base> Expression_v2::unwrap(Expression_v2 expr) {
    if (expr.lhs && !expr.rhs && expr.operation == none) {
        return expr.lhs;
    }
    return std::make_shared<Expression_v2>(expr);
}

std::string Expression_v2::print() const {
    if (!lhs && !rhs) return "";
    if (operation == none && lhs && !rhs) return lhs->print();

    auto op_str = [](expression_operator op) -> std::string {
        if (op_to_str.contains(op)) return op_to_str.at(op);
        return "???";
    };

    bool is_unary = operation == logic_neg || operation == bitwise_neg;
    is_unary |= !rhs && (operation == add || operation == subtract);

    if (is_unary) {
        auto operand = lhs ? lhs : rhs;
        return op_str(operation) + (operand ? operand->print() : "");
    }

    auto needs_parens = [&](const std::shared_ptr<Parameter_value_base> &side, bool is_lhs_side) {
        if (!side->is<Expression_v2>()) return false;
        auto inner_op = static_cast<const Expression_v2 &>(*side).get_operation();
        if (inner_op == none) return false;
        if (!op_precedence.contains(inner_op) || !op_precedence.contains(operation)) return true;
        if (is_lhs_side)
            return op_precedence.at(inner_op) < op_precedence.at(operation);
        else
            return op_precedence.at(inner_op) <= op_precedence.at(operation);
    };

    std::ostringstream oss;
    if (lhs) {
        if (needs_parens(lhs, true))
            oss << "(" << lhs->print() << ")";
        else
            oss << lhs->print();
    }
    oss << op_str(operation);
    if (rhs) {
        if (needs_parens(rhs, false))
            oss << "(" << rhs->print() << ")";
        else
            oss << rhs->print();
    }
    return oss.str();
}

bool Expression_v2::isEqual(const Parameter_value_base &other) const {
    auto other_exp = dynamic_cast<const Expression_v2*>(&other);
    if (!other_exp) return false;
    if (lhs && other_exp->lhs) {
        if (!(*lhs == *other_exp->lhs)) return false;
    } else if (lhs || other_exp->lhs) {
        return false;
    }
    if (rhs && other_exp->rhs) {
        if (!(*rhs == *other_exp->rhs)) return false;
    } else if (rhs || other_exp->rhs) {
        return false;
    }
    return operation == other_exp->operation;
}

void Expression_v2::set_container_sizes(const resolved_type &s,
    const std::map<qualified_identifier, resolved_parameter> &context) {
    current_size = 1;
    for (auto &size:s.packed_sizes) current_size *= size;
    resolved_type r;
    r.packed_sizes.push_back(64);
    if (lhs) lhs->set_container_sizes(r);
    if (rhs) rhs->set_container_sizes(r);
}

void Expression_v2::propagate_expression(const qualified_identifier &constant_id,
    const std::shared_ptr<Parameter_value_base> &value) {
    if (lhs && !Token::try_replace_identifier(lhs, constant_id, value)) {
        lhs->propagate_expression(constant_id, value);
    }
    if (rhs && !Token::try_replace_identifier(rhs, constant_id, value)) {
        rhs->propagate_expression(constant_id, value);
    }
}

bool operator==(const Expression_v2 &lhs, const Expression_v2 &rhs) {
    if (lhs.lhs && rhs.lhs) {
        if (!(*lhs.lhs == *rhs.lhs)) return false;
    } else if (lhs.lhs || rhs.lhs) {
        return false;
    }
    if (lhs.rhs && rhs.rhs) {
        if (!(*lhs.rhs == *rhs.rhs)) return false;
    } else if (lhs.rhs || rhs.rhs) {
        return false;
    }
    return lhs.operation == rhs.operation;
}

int64_t Expression_v2::get_size() {
    auto expression_value = evaluate({});
    if(expression_value.has_value()) {
        if(expression_value.value().is_integer())
            return expression_value.value().get_integer().get_size();
    }
    return 0;
}

std::set<qualified_identifier> Expression_v2::get_dependencies() const {
    std::set<qualified_identifier> deps;
    if (lhs) {
        auto lhs_deps = lhs->get_dependencies();
        deps.insert(lhs_deps.begin(), lhs_deps.end());
    }
    if (rhs) {
        auto rhs_deps = rhs->get_dependencies();
        deps.insert(rhs_deps.begin(), rhs_deps.end());
    }
    return deps;
}

std::optional<resolved_parameter> Expression_v2::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    std::optional<resolved_parameter> r_val, l_val;
    resolved_parameter ret_val;

    if (operation == none) {
        if (lhs && !rhs) return lhs->evaluate(context);
        return std::nullopt;
    }

    if (lhs) l_val = lhs->evaluate(context);
    if (rhs) r_val = rhs->evaluate(context);
    if (operation == logic_neg || operation == bitwise_neg) {
        resolved_parameter operand = 0;
        if (l_val.has_value()) operand = l_val.value();
        auto res = evaluate_unary_expression(operand);
        if (std::holds_alternative<double>(res)) ret_val = std::get<double>(res);
        else ret_val = std::get<hdl_integer>(res);
    } else {
        resolved_parameter operand_a = 0;
        resolved_parameter operand_b = 0;
        if (l_val) operand_a = l_val.value();
        if (r_val) {
            operand_b = r_val.value();
        } else {
            if (operation == subtract) {
                operand_b = operand_a;
                operand_a = 0;
            }
        }
        auto res = evaluate_binary_expression(operand_a, operand_b);
        if (std::holds_alternative<double>(res)) ret_val = std::get<double>(res);
        else ret_val = std::get<hdl_integer>(res);
    }
    return  ret_val;
}

std::variant<hdl_integer, double> Expression_v2::evaluate_binary_expression(resolved_parameter op_a, resolved_parameter op_b) {

    if(operation ==  equal){
        return op_a == op_b;
    }
    if(operation ==  not_equal){
        return op_a != op_b;
    }

    bool supported_a = (op_a.is_integer() || op_a.is_real() );
    bool supported_b = (op_b.is_integer() || op_b.is_real() );
    if(  !supported_a || !supported_b) {
        spdlog::warn("Attempted evaluation of operand of unsupported type");
        return  0;
    }
    bool int_exec = op_a.is_integer() && op_b.is_integer();
    double d_a, d_b;
    hdl_integer i_a = 0;
    hdl_integer i_b = 0;
    bool output_signed = false;
    if (int_exec) {
        output_signed = op_a.get_integer().get_signed() || op_b.get_integer().get_signed();
    }
    if(op_a.is_real())  d_a = op_a.get_real();
    else d_a = static_cast<double>(op_a.get_integer().get_value());
    if(op_b.is_real())  d_b = op_b.get_real();
    else d_b = static_cast<double>(op_b.get_integer().get_value());
    if(op_a.is_integer()) i_a =  op_a.get_integer();
    if(op_b.is_integer()) i_b =  op_b.get_integer();
    
    hdl_integer result_i;
    double result_d;
    if(operation == add){
        if(int_exec) result_i = i_a + i_b;
        else result_d = d_a + d_b;
    }else if(operation == subtract){
        if(int_exec) result_i =i_a - i_b;
         else result_d = d_a - d_b;
    }else if(operation == multiply){
        if(int_exec) result_i =i_a * i_b;
         else result_d = d_a * d_b;
    }else if(operation == power){
        if(int_exec) result_i =std::pow(i_a, i_b);
        result_d = std::pow(d_a, d_b);
    }else if(operation == divide){
        if(int_exec) result_i =i_a / i_b;
        else result_d = d_a / d_b;
    }else if(operation == modulo){
        if(int_exec) result_i =i_a % i_b;
        spdlog::warn("The modulus operator is only defined between integers");
        result_d = 0;
    }else if(operation == logic_shift_left){
        if(int_exec) result_i =i_a << i_b;
        spdlog::warn("The shift operator is only defined between integers");
        result_d = 0;
    }else if(operation == logic_shift_right){
        if(int_exec) {
            uint64_t u_a = static_cast<uint64_t>(i_a.get_value());
            if(current_size > 0 && current_size < 64) {
                u_a &= (1ULL << current_size) - 1;
            }
            uint64_t shift = static_cast<uint64_t>(i_b.get_value());
            if(shift >= 64) return hdl_integer(0);
            return hdl_integer(static_cast<int64_t>(u_a >> shift));
        }
        spdlog::warn("The shift operator is only defined between integers");
        return 0;
    } else if(operation == arithmetic_shift_left){
        if(int_exec) result_i =i_a << i_b;
        spdlog::warn("The shift operator is only defined between integers");
        result_d = 0;
    } else if(operation == arithmetic_shift_right){
        if(int_exec) result_i =i_a >> i_b;
        spdlog::warn("The shift operator is only defined between integers");
        result_d = 0;
    }else if(operation == greater){
        if(int_exec) result_i =i_a > i_b;
        else result_d = d_a > d_b;
    }else if(operation == greater_equal){
        if(int_exec) result_i =i_a >= i_b;
        else result_d = d_a >= d_b;
    } else if(operation == less){
        if(int_exec) result_i =i_a < i_b;
        else result_d = d_a < d_b;
    } else if(operation == less_equal){
        if(int_exec) result_i =i_a <= i_b;
        else result_d = d_a <= d_b;
    } else if(operation == bitwise_and){
        result_i = i_a & i_b;
    } else if(operation == bitwise_or){
        result_i = i_a | i_b;
    } else if(operation == bitwise_xor){
        result_i = i_a ^ i_b;
    } else if(operation == bitwise_xnor){
        result_i = ~(i_a ^ i_b);
    } else if(operation == logical_and){
        if(int_exec) result_i =i_a && i_b;
        result_d = d_a && d_b;
    } else if(operation == logical_or){
        if(int_exec)  result_i = i_a || i_b;
        result_d = d_a || d_b;
    } else {
        throw std::runtime_error("Error: Attempted evaluation of an unsupported binary expression expression ");
    }

    if (int_exec) {
        result_i.set_signed(output_signed);
        return result_i;
    }
    return result_d;
    
}

std::variant<hdl_integer, double> Expression_v2::evaluate_unary_expression(resolved_parameter operand) {

    if( !operand.is_integer() || operand.is_real()) {
        spdlog::warn("Attempted evaluation of operand of unsupported type");
        return  0;
    }
    const bool int_exec = operand.is_integer();

    hdl_integer int_op;
    if(int_exec) int_op = operand.get_integer();
    double double_op = 0;
    if(operand.is_real()) double_op = operand.get_real();
    if(operation == logic_neg){
        if(int_exec) return !int_op;
        return double_op != 0 ? 1 : 0;
    }

    if(operation ==  bitwise_neg){
        return ~operand.get_integer();
    }
    throw std::runtime_error("Error: Attempted evaluation of an unsupported unary expression ");
}