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
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"

CEREAL_REGISTER_TYPE(Expression_v2)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, Expression_v2)

namespace {
    // Container width of an expression operand, derived from its resolved type.
    // This is the deterministic width used for rotate/shift semantics (unsized
    // integer literals model a 32-bit container).
    uint64_t expr_width(const std::shared_ptr<Expression_base> &e,
                        const std::map<qualified_identifier, resolved_parameter> &context,
                        const std::optional<resolved_type> &child_sizing = std::nullopt) {
        if (!e) return 0;
        auto t = e->resolve_expression_type(context, child_sizing);
        if (t && !t->is_real && !t->packed_sizes.empty()) {
            uint64_t w = 1;
            for (auto ps : t->packed_sizes) w *= ps;
            if (w > 0) return w;
        }
        auto v = e->evaluate(context, child_sizing);
        if (v && v->is_integer()) return v->get_integer().get_size();
        return 64;
    }
}

std::shared_ptr<Expression_base> Expression_v2::unwrap(Expression_v2 expr) {
    if (!expr.lhs && !expr.rhs && expr.operation == none) {
        return nullptr;
    }
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

    auto needs_parens = [&](const std::shared_ptr<Expression_base> &side, bool is_lhs_side) {
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

bool Expression_v2::isEqual(const Expression_base &other) const {
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
    resolved_type r;
    r.packed_sizes.push_back(64);
    if (lhs) lhs->set_container_sizes(r);
    if (rhs) rhs->set_container_sizes(r);
}

void Expression_v2::propagate_expression(const qualified_identifier &constant_id,
    const std::shared_ptr<Expression_base> &value) {
    if (lhs && lhs->is<Identifier_token>() && lhs->as<Identifier_token>().get_value() == constant_id) {
        lhs = value;
    } else if (lhs) {
        lhs->propagate_expression(constant_id, value);
    }
    if (rhs && rhs->is<Identifier_token>() && rhs->as<Identifier_token>().get_value() == constant_id) {
        rhs = value;
    } else if (rhs) {
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

void Expression_v2::propagate_function(const hdl_function_def_ptr &def) {
    if (lhs) lhs->propagate_function(def);
    if (rhs) rhs->propagate_function(def);
}

parameter_deps_t Expression_v2::get_dependencies() const {
    parameter_deps_t deps;
    if (lhs) {
        deps.merge(lhs->get_dependencies());
    }
    if (rhs) {
        deps.merge(rhs->get_dependencies());
    }
    return deps;
}

std::optional<resolved_type> Expression_v2::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type) const {
    // Mirror of set_container_sizes: when this node was reached with a
    // container type, operands were unconditionally sized as 64-bit packed.
    // With no expected type the operands keep their own (default) sizing.
    std::optional<resolved_type> operand_sizing;
    if (expected_type) {
        resolved_type r;
        r.packed_sizes.push_back(64);
        operand_sizing = r;
    }
    if (operation == none) {
        if (lhs && !rhs) return lhs ? lhs->resolve_expression_type(context, operand_sizing) : std::nullopt;
        return std::nullopt;
    }

    auto lhs_t = lhs ? lhs->resolve_expression_type(context, operand_sizing) : std::nullopt;
    auto rhs_t = rhs ? rhs->resolve_expression_type(context, operand_sizing) : std::nullopt;

    auto width_of = [](const std::optional<resolved_type> &t) -> uint64_t {
        if (!t) return 0;
        uint64_t w = 1;
        for (auto ps : t->packed_sizes) w *= ps;
        return w;
    };

    if ((lhs_t && lhs_t->is_real) || (rhs_t && rhs_t->is_real)) {
        resolved_type result;
        result.is_real = true;
        result.packed_sizes.push_back(64);
        result.packed_ascending.push_back(false);
        result.packed_left.push_back(63);
        result.packed_right.push_back(0);
        return result;
    }

    uint64_t w_a = width_of(lhs_t);
    uint64_t w_b = width_of(rhs_t);

    uint64_t width = std::max(w_a, w_b);
    switch (operation) {
        case logic_neg:
        case reduction_and: case reduction_nand:
        case reduction_or: case reduction_nor:
        case reduction_xor: case reduction_xnor:
        case greater: case greater_equal: case less: case less_equal:
        case equal: case not_equal:
        case case_equal: case case_not_equal:
        case wildcard_equal: case wildcard_not_equal:
        case logical_and: case logical_or:
            width = 1;
            break;
        case bitwise_neg:
            width = w_a;
            break;
        case logic_shift_left: case logic_shift_right:
        case arithmetic_shift_left: case arithmetic_shift_right:
        case power:
            width = w_a;
            break;
        default:
            width = std::max(w_a, w_b);
            break;
    }
    if (width == 0) width = 32;

    resolved_type result;
    result.packed_sizes.push_back(width);
    result.packed_ascending.push_back(false);
    result.packed_left.push_back(static_cast<int64_t>(width) - 1);
    result.packed_right.push_back(0);
    return result;
}

std::expected<resolved_parameter, solver_errors> Expression_v2::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type) {

    std::expected<resolved_parameter, solver_errors> r_val, l_val;
    resolved_parameter ret_val;

    // Mirror of set_container_sizes: when reached with a container type the
    // operands were unconditionally sized as 64-bit packed; otherwise they
    // keep their own (default) sizing.
    std::optional<resolved_type> operand_sizing;
    if (expected_type) {
        resolved_type r;
        r.packed_sizes.push_back(64);
        operand_sizing = r;
    }

    if (operation == none) {
        if (lhs && !rhs) return lhs->evaluate(context, operand_sizing);
        return std::unexpected{missing_arguments};
    }

    if (lhs) l_val = lhs->evaluate(context, operand_sizing);
    if (rhs) r_val = rhs->evaluate(context, operand_sizing);
    if (operation == logic_neg || operation == bitwise_neg ||
        operation == reduction_and || operation == reduction_nand ||
        operation == reduction_or || operation == reduction_nor ||
        operation == reduction_xor || operation == reduction_xnor ||
        operation == abs_value || operation == condition_op) {
        resolved_parameter operand = 0;
        if (l_val.has_value()) operand = l_val.value();
        auto res = evaluate_unary_expression(operand);
        if (std::holds_alternative<double>(res)) ret_val = std::get<double>(res);
        else ret_val = std::get<hdl_integer>(res);
    } else {
        resolved_parameter operand_a = 0;
        resolved_parameter operand_b = 0;
        if (l_val) operand_a = l_val.value();
        if (rhs && r_val) {
            operand_b = r_val.value();
        } else {
            if (operation == subtract) {
                operand_b = operand_a;
                operand_a = 0;
            }
        }
        std::variant<hdl_integer, double> res;
        if (operation == rotate_left || operation == rotate_right) {
            res = evaluate_rotate(context, operand_a, operand_b, operand_sizing);
        } else {
            res = evaluate_binary_expression(operand_a, operand_b);
        }
        if (std::holds_alternative<double>(res)) ret_val = std::get<double>(res);
        else ret_val = std::get<hdl_integer>(res);
    }

    // Self-determined result-width truncation: the result of an expression is
    // truncated to the maximum operand width (1 bit for comparisons/logic, the
    // left operand width for shifts and unary ops), so wide values do not leak
    // past their container.
    if (ret_val.is_integer()) {
        // Operand widths mirror the sized state: the operands were sized as
        // 64-bit packed when this node was reached with a container type.
        auto operand_width = [&](const std::shared_ptr<Expression_base> &e) -> uint64_t {
            if (!e) return 0;
            auto t = e->resolve_expression_type(context, operand_sizing);
            if (t && !t->is_real) {
                uint64_t w = 1;
                for (auto ps : t->packed_sizes) w *= ps;
                if (w > 0) return w;
            }
            auto v = e->evaluate(context, operand_sizing);
            if (v && v->is_integer()) return v->get_integer().get_size();
            return 64;
        };
        uint64_t w_a = operand_width(lhs);
        uint64_t w_b = operand_width(rhs);
        uint64_t width = 1;
        switch (operation) {
            case logic_neg:
            case reduction_and: case reduction_nand:
            case reduction_or: case reduction_nor:
            case reduction_xor: case reduction_xnor:
            case condition_op:
            case greater: case greater_equal: case less: case less_equal:
            case equal: case not_equal: case case_equal: case case_not_equal:
            case wildcard_equal: case wildcard_not_equal:
            case logical_and: case logical_or:
                width = 1;
                break;
            case bitwise_neg:
            case abs_value:
            case logic_shift_left: case logic_shift_right:
            case arithmetic_shift_left: case arithmetic_shift_right:
            case rotate_left: case rotate_right:
            case power:
                width = w_a;
                break;
            default:
                width = std::max(w_a, w_b);
                break;
        }
        auto v = ret_val.get_integer();
        if (v.get_size() > width) {
            if (v.get_signed())
                ret_val = v.sign_extend(static_cast<int64_t>(width));
            else
                ret_val = v.truncate_to(static_cast<int64_t>(width));
        }
    }
    return  ret_val;
}

std::variant<hdl_integer, double> Expression_v2::evaluate_rotate(
    const std::map<qualified_identifier, resolved_parameter> &context,
    resolved_parameter op_a, resolved_parameter op_b,
    const std::optional<resolved_type> &operand_sizing) {
    if (!op_a.is_integer() || !op_b.is_integer()) {
        spdlog::warn("The rotate operator is only defined between integers");
        return 0;
    }

    // Container width of the left operand, derived from its resolved type.
    // Unsized integer literals model a 32-bit container, which makes the
    // rotate deterministic (vs. the natural bit length of the value).
    uint64_t width = expr_width(lhs, context, operand_sizing);
    if (width <= 0) width = 64;

    auto i_a = op_a.get_integer();
    auto i_b = op_b.get_integer();
    int64_t amount = i_b.get_value() % static_cast<int64_t>(width);
    if (amount < 0) amount += static_cast<int64_t>(width);
    wide_integer mask = (wide_integer(1) << width) - 1;
    wide_integer v = i_a.to_wide() & mask;
    wide_integer res;
    if (amount == 0) {
        res = v;
    } else if (operation == rotate_left) {
        res = ((v << amount) | (v >> (width - amount))) & mask;
    } else {
        res = ((v >> amount) | (v << (width - amount))) & mask;
    }
    hdl_integer result;
    result.set_value(res);
    result.set_signed(i_a.get_signed());
    return result;
}

std::variant<hdl_integer, double> Expression_v2::evaluate_binary_expression(resolved_parameter op_a, resolved_parameter op_b) {

    if(operation ==  equal || operation == case_equal || operation == wildcard_equal){
        if (op_a.is_integer() && op_b.is_integer())
            return op_a.get_integer() == op_b.get_integer();
        if (op_a.is_string() && op_b.is_string())
            return op_a.get_string() == op_b.get_string();
        // Mixed int/real or any real comparison: compare as doubles.
        if ((op_a.is_integer() || op_a.is_real()) && (op_b.is_integer() || op_b.is_real())) {
            double a = op_a.is_real() ? op_a.get_real() : op_a.get_integer().to_double();
            double b = op_b.is_real() ? op_b.get_real() : op_b.get_integer().to_double();
            return a == b;
        }
        spdlog::warn("Attempted comparison between incompatible operand types");
        return 0;
    }
    if(operation ==  not_equal || operation == case_not_equal || operation == wildcard_not_equal){
        if (op_a.is_integer() && op_b.is_integer())
            return op_a.get_integer() != op_b.get_integer();
        if (op_a.is_string() && op_b.is_string())
            return op_a.get_string() != op_b.get_string();
        // Mixed int/real or any real comparison: compare as doubles.
        if ((op_a.is_integer() || op_a.is_real()) && (op_b.is_integer() || op_b.is_real())) {
            double a = op_a.is_real() ? op_a.get_real() : op_a.get_integer().to_double();
            double b = op_b.is_real() ? op_b.get_real() : op_b.get_integer().to_double();
            return a != b;
        }
        spdlog::warn("Attempted comparison between incompatible operand types");
        return 0;
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
    else d_a = op_a.get_integer().to_double();
    if(op_b.is_real())  d_b = op_b.get_real();
    else d_b = op_b.get_integer().to_double();
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
        else spdlog::warn("The modulus operator is only defined between integers");
        result_d = 0;
    }else if(operation == v_mod){
        // VHDL `mod` is a floor-style modulo: the result takes the sign of the
        // divisor. C++ `%` is truncated; adjust so the remainder matches the
        // divisor's sign.
        if(int_exec) {
            auto rem = i_a % i_b;
            auto rem_v = rem.get_value();
            auto div_v = i_b.get_value();
            if (rem_v != 0 && ((rem_v < 0) != (div_v < 0)))
                rem_v += div_v;
            result_i = hdl_integer(rem_v);
        } else {
            spdlog::warn("The mod operator is only defined between integers");
            result_d = 0;
        }
    }else if(operation == logic_shift_left){
        if(int_exec) result_i =i_a << i_b;
        else spdlog::warn("The shift operator is only defined between integers");
        result_d = 0;
    }else if(operation == logic_shift_right){
        if(int_exec) {
            uint64_t operand_size = i_a.get_size();
            hdl_integer u_a = i_a;
            if(operand_size > 0) {
                u_a = i_a.truncate_to(static_cast<int64_t>(operand_size));
            }
            int64_t shift = i_b.get_value();
            return u_a >> hdl_integer(shift);
        }
        spdlog::warn("The shift operator is only defined between integers");
        return 0;
    } else if(operation == arithmetic_shift_left){
        if(int_exec) result_i =i_a << i_b;
        else spdlog::warn("The shift operator is only defined between integers");
        result_d = 0;
    } else if(operation == arithmetic_shift_right){
        if(int_exec) result_i =i_a >> i_b;
        else spdlog::warn("The shift operator is only defined between integers");
        result_d = 0;
    } else if(operation == rotate_left || operation == rotate_right){
        // Rotate is handled in Expression_v2::evaluate where the operand
        // expression (and thus its container width) is available.
        spdlog::warn("Rotate operator evaluated without container width");
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
        if (!int_exec) { spdlog::warn("Bitwise operator with real operand"); result_d = 0; }
        else result_i = i_a & i_b;
    } else if(operation == bitwise_or){
        if (!int_exec) { spdlog::warn("Bitwise operator with real operand"); result_d = 0; }
        else result_i = i_a | i_b;
    } else if(operation == bitwise_xor){
        if (!int_exec) { spdlog::warn("Bitwise operator with real operand"); result_d = 0; }
        else result_i = i_a ^ i_b;
    } else if(operation == bitwise_xnor){
        if (!int_exec) { spdlog::warn("Bitwise operator with real operand"); result_d = 0; }
        else result_i = ~(i_a ^ i_b);
    } else if(operation == logical_and){
        if(int_exec) result_i =i_a && i_b;
        result_d = d_a && d_b;
    } else if(operation == logical_or){
        if(int_exec)  result_i = i_a || i_b;
        result_d = d_a || d_b;
    } else {
        return unsupported;
    }

    if (int_exec) {
        result_i.set_signed(output_signed);
        return result_i;
    }
    return result_d;
    
}

std::variant<hdl_integer, double> Expression_v2::evaluate_unary_expression(resolved_parameter operand) {

    if (operation == logic_neg) {
        if (operand.is_real()) return operand.get_real() == 0.0 ? hdl_integer(1) : hdl_integer(0);
        if (operand.is_integer()) return operand.get_integer() == 0 ? hdl_integer(1) : hdl_integer(0);
        spdlog::warn("Attempted evaluation of operand of unsupported type");
        return 0;
    }

    if (operation == abs_value) {
        if (operand.is_real()) return std::abs(operand.get_real());
        if (operand.is_integer()) {
            auto v = operand.get_integer().to_wide();
            hdl_integer res;
            res.set_value(v < 0 ? -v : v);
            res.set_signed(operand.get_integer().get_signed());
            return res;
        }
        spdlog::warn("Attempted evaluation of operand of unsupported type");
        return 0;
    }

    if (operation == condition_op) {
        // VHDL `??` condition operator: normalize a condition to 0/1.
        if (operand.is_real()) return operand.get_real() != 0.0 ? hdl_integer(1) : hdl_integer(0);
        if (operand.is_integer()) return operand.get_integer() != 0 ? hdl_integer(1) : hdl_integer(0);
        spdlog::warn("Attempted evaluation of operand of unsupported type");
        return 0;
    }

    if (!operand.is_integer()) {
        spdlog::warn("Attempted evaluation of operand of unsupported type");
        return 0;
    }

    auto int_op = operand.get_integer();

    if (operation == bitwise_neg) {
        auto width = int_op.get_size();
        wide_integer negated = -int_op.to_wide() - 1;
        hdl_integer res;
        res.set_value(negated);
        res = res.truncate_to(static_cast<int64_t>(width));
        return res;
    }

    if (operation == reduction_and) {
        wide_integer v = int_op.to_wide();
        while (v != 0) {
            if ((v & 1) == 0) return static_cast<hdl_integer>(0);
            v >>= 1;
        }
        // Value is 0, or all bits are 1. A zero value has a zero bit, so AND-reduce is 0.
        return static_cast<hdl_integer>(int_op.to_wide() != 0);
    }
    if (operation == reduction_nand) {
        wide_integer v = int_op.to_wide();
        while (v != 0) {
            if ((v & 1) == 0) return static_cast<hdl_integer>(1);
            v >>= 1;
        }
        return static_cast<hdl_integer>(int_op.to_wide() == 0);
    }
    if (operation == reduction_or) {
        return static_cast<hdl_integer>(int_op.to_wide() != 0);
    }
    if (operation == reduction_nor) {
        return static_cast<hdl_integer>(int_op.to_wide() == 0);
    }

    // Reduction XOR / XNOR: parity of the set bits, across the full wide value.
    if (operation == reduction_xor || operation == reduction_xnor) {
        wide_integer v = int_op.to_wide();
        bool parity = false;
        while (v != 0) {
            parity = !parity;
            v &= (v - 1);
        }
        if (operation == reduction_xnor) parity = !parity;
        return static_cast<hdl_integer>(parity ? 1 : 0);
    }

    throw unsupported;
}