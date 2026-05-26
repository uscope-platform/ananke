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


std::string Expression::print() const {
    std::string ret_val;
    for(auto &item:components){
        auto val = item.get_value();
        if (val.has_value()) {
            if(item.is_array()) {
                auto arr = val.value().get_int_array();;
                ret_val += "{xxxxxxx}";
            } else if(item.is_numeric()){
                if( val.value().is_integer())
                    ret_val += std::to_string(val.value().get_integer());
                else if(val.value().is_real()) {
                    ret_val += std::to_string(val.value().get_real());
                }
            } else if(item.is_identifier()){
                if(!item.get_package_prefix().empty()){
                    ret_val += item.get_package_prefix() + "::";
                }
                ret_val += val->get_string();
            } else if(item.is_operator()) {
                ret_val += val->get_string();
            }
        }
    }
    return ret_val;
}

Expression Expression::to_rpm() const {
    Expression rpn_exp;
    std::stack<Expression_component> shunting_stack;

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
        if (item.is_numeric()) {
            rpn_exp.push_back(item);
        } else if(item.is_operator()){
            while (
                    !shunting_stack.empty() &&
                    shunting_stack.top().get_value().value().get_string()!="(" &&
                    (
                        shunting_stack.top().is_function() ||
                        shunting_stack.top().get_operator_precedence()<item.get_operator_precedence() ||
                        shunting_stack.top().get_operator_precedence()==item.get_operator_precedence() &&
                        !shunting_stack.top().is_right_associative()
                    )
            ){
                rpn_exp.push_back(shunting_stack.top());
                shunting_stack.pop();
            }
            shunting_stack.push(item);
        } else if(item.get_value().value().get_string() == "(" || item.is_function()){
            shunting_stack.push(item);
        } else if(item.get_value().value().get_string()  == ")"){
            while (shunting_stack.top().get_value().value().get_string() != "(") {
                rpn_exp.push_back(shunting_stack.top());
                shunting_stack.pop();
                if(shunting_stack.top().is_function()){
                    rpn_exp.push_back(shunting_stack.top());
                    shunting_stack.pop();
                }
            }
            shunting_stack.pop();
        } else {
            rpn_exp.push_back(item);
        }
    }

    while(!shunting_stack.empty()){
        rpn_exp.push_back(shunting_stack.top());
        shunting_stack.pop();
    }
    rpn_exp.rpn = true;
    return rpn_exp;
}

std::optional<resolved_parameter> Expression::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    if(components.empty()) return std::nullopt;
    if (components.size() == 1) {
        return components[0].get_value(context);
    }

    auto expr_stack = to_rpm();

    std::stack<Expression_component> evaluator_stack;
    for(auto & i : expr_stack.components){
        if(i.is_numeric()) {
            evaluator_stack.push(i);
        } else if (i.is_identifier()) {
            auto resolved = i.get_value(context);
            if (!resolved.has_value()) return std::nullopt;
            if (resolved.value().is_integer()) {
                evaluator_stack.emplace(resolved.value().get_integer(), 0);
            } else if (resolved.value().is_real()) {
                evaluator_stack.emplace(resolved.value().get_real(), 0);
            } else if (resolved.value().is_string()) {
                evaluator_stack.emplace(resolved.value().get_string(), Expression_component::string);
            } else {
                return std::nullopt;
            }
        } else {
            std::variant<hdl_integer, double> result;
            if (!i.is_operator() && !i.is_function()) return std::nullopt;
            if(i.get_operator_type() == Expression_component::unary_operator){
                auto op = evaluator_stack.top().get_value();
                result = evaluate_unary_expression(op.value(), i.get_value().value().get_string());
                evaluator_stack.pop();
            } else if(i.get_operator_type() == Expression_component::binary_operator){
                resolved_parameter op_a;
                auto op_b = evaluator_stack.top().get_value();
                evaluator_stack.pop();
                if(expr_stack.components.size()==2)
                    op_a = 0;
                else {
                    op_a = evaluator_stack.top().get_value().value();
                    evaluator_stack.pop();
                }
                result = evaluate_binary_expression(op_a, op_b.value(), i.get_value().value().get_string());
            }
            evaluator_stack.emplace(result, Expression_component::number);
        }
    }

    if (evaluator_stack.empty())throw std::runtime_error("Evaluation of an empty expression");
    return evaluator_stack.top().get_value();

}

int64_t Expression::get_size() {
    if (components.size() == 1) {
        return components[0].get_binary_size();
    }

    auto expression_value = evaluate({});
    if(expression_value.has_value()) {
        if(expression_value.value().is_integer())
            return expression_value.value().get_integer().get_size();
    }
    return 0;
}

std::variant<hdl_integer, double> Expression::evaluate_binary_expression(resolved_parameter op_a, resolved_parameter op_b, const std::string &operation) {
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
    if(operation == "+"){
        if(int_exec) return i_a + i_b;
        return d_a + d_b;
    }
    if(operation ==  "-"){
        if(int_exec) return i_a - i_b;
        return d_a - d_b;
    }
    if(operation ==  "*"){
        if(int_exec) return i_a * i_b;
        return d_a * d_b;
    }
    if(operation ==  "/"){
        if(int_exec) return i_a / i_b;
        return d_a / d_b;
    }
    if(operation ==  "%"){
        if(int_exec) return i_a % i_b;
        spdlog::warn("The modulus operator is only defined between integers");
        return 0;
    }
    if(operation ==  "<<"){
        if(int_exec) return i_a << i_b;
        spdlog::warn("The shift operator is only defined between integers");
        return 0;
    }
    if(operation ==  ">>"){
        if(int_exec) return i_a >> i_b;
        spdlog::warn("The shift operator is only defined between integers");
        return 0;
    }
    if(operation ==  ">"){
        if(int_exec) return i_a > i_b;
        return d_a > d_b;
    }
    if(operation ==  ">="){
        if(int_exec) return i_a >= i_b;
        return d_a >= d_b;

    }
    if(operation ==  "<"){
        if(int_exec) return i_a < i_b;
        return d_a < d_b;
    }
    if(operation ==  "<="){
        if(int_exec) return i_a <= i_b;
        return d_a <= d_b;
    }
    if(operation ==  "=="){
        return op_a == op_b;
    }
    if(operation ==  "!="){
        return op_a != op_b;
    }
    throw std::runtime_error("Error: Attempted evaluation of an unsupported binary expression expression " + operation);
}

std::variant<hdl_integer, double> Expression::evaluate_unary_expression(resolved_parameter operand, const std::string &operation) {
    if(operation == "$rtoi" || operation == "$itor") return evaluate_cast(operand, operation);
    if( !operand.is_integer() || operand.is_real()) {
        spdlog::warn("Attempted evaluation of operand of unsupported type");
        return  0;
    }
    const bool int_exec = operand.is_integer();

    hdl_integer int_op = 0;
    if(int_exec) int_op = operand.get_integer();
    double double_op = 0;
    if(operand.is_real()) double_op = operand.get_real();
    if(operation == "!"){
        if(int_exec) return !int_op;
        return double_op != 0 ? 1 : 0;
    }
    if(operation ==  "$ceil"){
        if(int_exec) {
            return static_cast<int64_t>(
                ceil(static_cast<double>(int_op.get_value()))
            );
        }
        return static_cast<int64_t>(
            ceil(double_op)
        );
    }
    if(operation ==  "$floor"){
        if(int_exec) {
            return static_cast<int64_t>(
                floor(static_cast<double>(operand.get_integer().get_value()))
            );
        }
        return static_cast<int64_t>(
            floor(double_op)
        );
    }
    if(!int_exec) {
        spdlog::warn("The "+operation+"() function is only defined between integers");
        return 0;
    }
    if(operation ==  "~"){
        return ~operand.get_integer();
    }
    if(operation ==  "$clog2"){
        return static_cast<int64_t>(
            ceil(log2(static_cast<double>(operand.get_integer().get_value())))
        );
    }

    throw std::runtime_error("Error: Attempted evaluation of an unsupported unary expression " + operation);
}

std::variant<hdl_integer, double> Expression::evaluate_cast(resolved_parameter operand, const std::string &operation) {
    if(operation == "$itor") {
        if(operand.is_integer()) return static_cast<double>(operand.get_integer().get_value());
        spdlog::warn("Attempted cast of an unsupported type");
        return 0;
    } else if(operation == "$rtoi") {
        if(operand.is_real()) return static_cast<int64_t>(round(operand.get_real()));
        spdlog::warn("Attempted cast of an unsupported type");
        return 0;
    }
    spdlog::warn("Attempted evaluation of an unsupported cast expression {}", operation);
    return 0;
}

void Expression::add_index(const Expression &idx) {
    if (!components.empty()) components.back().add_array_index(idx);
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
        auto deps = comp.get_dependencies();
        result.insert(deps.begin(), deps.end());
    }
    return result;
}

bool Expression::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter& param_value) {
    bool retval = true;
    for (auto & component : components) {
        if (component.is_identifier()) {
            retval &= component.propagate_constant(constant_id, param_value);
        }
    }
    return retval;
}

void Expression::propagate_expression(const qualified_identifier &constant_id,
    const std::shared_ptr<Parameter_value_base> &value) {
    std::vector<Expression_component> new_expr;

    for (auto & component : components) {
        if (component.is_identifier()) {
            if (component.get_value().value().get_string() == constant_id.name) {
                if (value->is_expression()) {
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
        component.propagate_function(def);
    }
}
