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


#include "data_model/HDL/parameters/components/Expression.hpp"
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Expression)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Parameter_value_base, Expression)


Expression::Expression(const std::initializer_list<Token> &list) {
    components.reserve(list.size());

    for (const auto &token : list) {
        // Copy the token onto the heap inside a shared_ptr
        components.push_back(std::make_shared<Token>(token));
    }
}

std::string Expression::print() const {
    std::string ret_val;
    for(auto &item:components){
        auto val = item->as<Token>().get_value();
        if (val.has_value()) {
            if(item->as<Token>().is_array()) {
                auto arr = val.value().get_int_array();;
                ret_val += "{xxxxxxx}";
            } else if(item->as<Token>().is_numeric()){
                if( val.value().is_integer())
                    ret_val += std::to_string(val.value().get_integer());
                else if(val.value().is_real()) {
                    ret_val += std::to_string(val.value().get_real());
                }
            } else if(item->as<Token>().is_identifier()){
                if(!item->as<Token>().get_package_prefix().empty()){
                    ret_val += item->as<Token>().get_package_prefix() + "::";
                }
                ret_val += val->get_string();
            } else if(item->as<Token>().is_operator()) {
                ret_val += item->as<Token>().print();
            }
        }
    }
    return ret_val;
}

Expression Expression::to_rpm() const {
    Expression rpn_exp;
    std::stack<std::shared_ptr<Parameter_value_base>> shunting_stack;

    if(components.empty()){
        return {};
    }
    if(rpn){
        Expression expr;
        expr.components = components;
        expr.rpn = rpn;
        return expr;
    }

    for(auto item:components){
        if (item->as<Token>().is_numeric()) {
            rpn_exp.push_back(item->as<Token>());
        } else if(item->as<Token>().is_operator()){
            while (
                    !shunting_stack.empty() &&
                    !shunting_stack.top()->as<Token>().is_parenthesis()  &&
                    (
                        shunting_stack.top()->as<Token>().is_function() ||
                        shunting_stack.top()->as<Token>().get_operator_precedence()<item->as<Token>().get_operator_precedence() ||
                        shunting_stack.top()->as<Token>().get_operator_precedence()==item->as<Token>().get_operator_precedence() &&
                        !shunting_stack.top()->as<Token>().is_right_associative()
                    )
            ){
                rpn_exp.push_back(shunting_stack.top()->as<Token>());
                shunting_stack.pop();
            }
            shunting_stack.push(item);
        } else if(item->as<Token>().get_value().value().get_string() == "(" || item->as<Token>().is_function()){
            shunting_stack.push(item);
        } else if(item->as<Token>().get_value().value().get_string()  == ")"){
            while (!shunting_stack.empty() && !shunting_stack.top()->as<Token>().is_parenthesis()) {
                rpn_exp.push_back(shunting_stack.top()->as<Token>());
                shunting_stack.pop();
            }
            if (!shunting_stack.empty()) shunting_stack.pop();
        } else {
            rpn_exp.push_back(item->as<Token>());
        }
    }

    while(!shunting_stack.empty()){
        rpn_exp.push_back(shunting_stack.top()->as<Token>());
        shunting_stack.pop();
    }
    rpn_exp.rpn = true;
    return rpn_exp;
}

std::optional<resolved_parameter> Expression::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    if(components.empty()) return std::nullopt;
    if (components.size() == 1) {
        return components[0]->as<Token>().evaluate(context);
    }

    auto expr_stack = to_rpm();

    std::stack<std::shared_ptr<Parameter_value_base>> evaluator_stack;
    for(auto & i : expr_stack.components){
        if(i->as<Token>().is_numeric()) {
            evaluator_stack.push(i);
        } else if (i->as<Token>().is_identifier()) {
            auto resolved = i->as<Token>().evaluate(context);
            if (!resolved.has_value()) return std::nullopt;
            if (resolved.value().is_integer()) {
                evaluator_stack.emplace(std::make_shared<Token>(resolved.value().get_integer(), 0));
            } else if (resolved.value().is_real()) {
                evaluator_stack.emplace(std::make_shared<Token>(resolved.value().get_real(), 0));
            } else if (resolved.value().is_string()) {
                evaluator_stack.emplace(std::make_shared<Token>(resolved.value().get_string(), Token::string));
            } else {
                return std::nullopt;
            }
        } else if (i->as<Token>().is_string()) {
            evaluator_stack.push(i);
        } else {
            if (i->as<Token>().is_function()) {
                auto resolved = i->as<Token>().evaluate(context);
                if (!resolved.has_value()) return std::nullopt;
                if (resolved.value().is_integer()) {
                    evaluator_stack.emplace(std::make_shared<Token>(std::variant<hdl_integer, double>(resolved.value().get_integer()), 0));
                } else if (resolved.value().is_real()) {
                    evaluator_stack.emplace(std::make_shared<Token>(std::variant<hdl_integer, double>(resolved.value().get_real()), 0));
                } else if (resolved.value().is_int_array()) {
                    Token t;
                    t.set_value(resolved.value());
                    evaluator_stack.push(std::make_shared<Token>(t));
                } else {
                    return std::nullopt;
                }
                continue;
            }
            std::variant<hdl_integer, double> result;
            if (!i->as<Token>().is_operator()) return std::nullopt;
            if(i->as<Token>().get_operator_type() == Token::unary_operator){
                auto op = evaluator_stack.top()->as<Token>().get_value();
                result = evaluate_unary_expression(op.value(), i->as<Token>().get_operation());
                evaluator_stack.pop();
            } else if(i->as<Token>().get_operator_type() == Token::binary_operator){
                resolved_parameter op_a;
                auto op_b = evaluator_stack.top()->as<Token>().get_value();
                evaluator_stack.pop();
                if(expr_stack.components.size()==2)
                    op_a = 0;
                else {
                    op_a = evaluator_stack.top()->as<Token>().get_value().value();
                    evaluator_stack.pop();
                }
                result = evaluate_binary_expression(op_a, op_b.value(), i->as<Token>().get_operation());
            }
            evaluator_stack.emplace(std::make_shared<Token>(result, Token::number));
        }
    }

    if (evaluator_stack.empty())throw std::runtime_error("Evaluation of an empty expression");
    return evaluator_stack.top()->as<Token>().get_value();

}

int64_t Expression::get_size() {
    if (components.size() == 1) {
        return components[0]->as<Token>().get_binary_size();
    }

    auto expression_value = evaluate({});
    if(expression_value.has_value()) {
        if(expression_value.value().is_integer())
            return expression_value.value().get_integer().get_size();
    }
    return 0;
}

std::variant<hdl_integer, double> Expression::evaluate_binary_expression(resolved_parameter op_a, resolved_parameter op_b, Token::sv_operators operation) {

    if(operation ==  Token::equal){
        return op_a == op_b;
    }
    if(operation ==  Token::not_equal){
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

    if(op_a.is_real())  d_a = op_a.get_real();
    else d_a = static_cast<double>(op_a.get_integer().get_value());
    if(op_b.is_real())  d_b = op_b.get_real();
    else d_b = static_cast<double>(op_b.get_integer().get_value());
    if(op_a.is_integer()) i_a =  op_a.get_integer();
    if(op_b.is_integer()) i_b =  op_b.get_integer();
    if(operation == Token::add){
        if(int_exec) return i_a + i_b;
        return d_a + d_b;
    }
    if(operation == Token::subtract){
        if(int_exec) return i_a - i_b;
        return d_a - d_b;
    }
    if(operation == Token::multiply){
        if(int_exec) return i_a * i_b;
        return d_a * d_b;
    }
    if(operation == Token::power){
        if(int_exec) return std::pow(i_a, i_b);
        return std::pow(d_a, d_b);
    }
    if(operation == Token::divide){
        if(int_exec) return i_a / i_b;
        return d_a / d_b;
    }
    if(operation == Token::modulo){
        if(int_exec) return i_a % i_b;
        spdlog::warn("The modulus operator is only defined between integers");
        return 0;
    }
    if(operation == Token::logic_shift_left){
        if(int_exec) return i_a << i_b;
        spdlog::warn("The shift operator is only defined between integers");
        return 0;
    }
    if(operation == Token::logic_shift_right){
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
    }
    if(operation == Token::arithmetic_shift_left){
        if(int_exec) return i_a << i_b;
        spdlog::warn("The shift operator is only defined between integers");
        return 0;
    }

    if(operation == Token::arithmetic_shift_right){
        if(int_exec) return i_a >> i_b;
        spdlog::warn("The shift operator is only defined between integers");
        return 0;
    }

    if(operation == Token::greater){
        if(int_exec) return i_a > i_b;
        return d_a > d_b;
    }

    if(operation == Token::greater_equal){
        if(int_exec) return i_a >= i_b;
        return d_a >= d_b;

    }
    if(operation == Token::less){
        if(int_exec) return i_a < i_b;
        return d_a < d_b;
    }
    if(operation == Token::less_equal){
        if(int_exec) return i_a <= i_b;
        return d_a <= d_b;
    }
    if(operation == Token::bitwise_and){
        return i_a & i_b;
    }
    if(operation == Token::bitwise_or){
        return i_a | i_b;
    }
    if(operation == Token::bitwise_xor){
        return i_a ^ i_b;
    }
    if(operation == Token::bitwise_xnor){
        return ~(i_a ^ i_b);
    }
    if(operation == Token::logical_and){
        if(int_exec) return i_a && i_b;
        return static_cast<double>(d_a && d_b);
    }
    if(operation == Token::logical_or){
        if(int_exec)  return i_a || i_b;
        return static_cast<double>(d_a || d_b);
    }
    throw std::runtime_error("Error: Attempted evaluation of an unsupported binary expression expression ");
}

std::variant<hdl_integer, double> Expression::evaluate_unary_expression(resolved_parameter operand, Token::sv_operators operation) {

    if( !operand.is_integer() || operand.is_real()) {
        spdlog::warn("Attempted evaluation of operand of unsupported type");
        return  0;
    }
    const bool int_exec = operand.is_integer();

    hdl_integer int_op;
    if(int_exec) int_op = operand.get_integer();
    double double_op = 0;
    if(operand.is_real()) double_op = operand.get_real();
    if(operation == Token::logic_neg){
        if(int_exec) return !int_op;
        return double_op != 0 ? 1 : 0;
    }

    if(operation ==  Token::bitwise_neg){
        return ~operand.get_integer();
    }
    throw std::runtime_error("Error: Attempted evaluation of an unsupported unary expression ");
}


void Expression::add_index(const std::shared_ptr<Parameter_value_base> &idx) {
    if (!components.empty()) components.back()->as<Token>().add_array_index(idx);
}

void Expression::set_container_sizes(const resolved_type &s,
    const std::map<qualified_identifier, resolved_parameter> &context) {
    current_size = 1;
    for (auto &size:s.packed_sizes) current_size *= size;
}

Expression Expression::clone()  const{
    Expression ret;
    ret.components = components;
    ret.rpn = rpn;
    return ret;
}

std::set<qualified_identifier> Expression::get_dependencies()const {
    std::set<qualified_identifier> result;
    for (auto &comp:components) {
        auto deps = comp->as<Token>().get_dependencies();
        result.insert(deps.begin(), deps.end());
    }
    return result;
}

void Expression::propagate_expression(const qualified_identifier &constant_id,
    const std::shared_ptr<Parameter_value_base> &value) {
    std::vector<std::shared_ptr<Parameter_value_base>> new_expr;

    for (auto & component : components) {
        if (component->as<Token>().is_identifier()) {
            if (component->as<Token>().get_value().value().get_string() == constant_id.name) {
                if (value->is<Expression>()) {
                    auto expr = value->as<Expression>();
                    if (expr.components.size() == 1) {
                        new_expr.push_back(expr.components[0]);
                        continue;
                    }
                }
            }
        }
        new_expr.push_back(component);
    }
    components = new_expr;
}

void Expression::propagate_function(const HDL_function_def &def) {
    for (auto & component : components) {
        component->as<Token>().propagate_function(def);
    }
}

bool operator==(const Expression &lhs, const Expression &rhs) {
    bool ret = true;
    ret &= lhs.rpn == rhs.rpn;
    if (lhs.components.size() != rhs.components.size()) return false;
    for (int i = 0; i< lhs.components.size(); i++) {
        ret &= *lhs.components[i] == *rhs.components[i];
    }
    return ret;
}
