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

#ifndef ANANKE_EXPRESSION_HPP
#define ANANKE_EXPRESSION_HPP


#include <vector>
#include <tuple>
#include <stack>
#include "Expression_component.hpp"
#include "Parameter_value_base.hpp"

class Expression : public Parameter_value_base{
public:
    std::vector<Expression_component> components;
    bool rpn = false;

    Expression clone() const;
    Expression(const Expression &other) = default;
    Expression(Expression &&other) noexcept = default;

    Expression & operator=(const Expression &other) = default;
    Expression & operator=(Expression &&other) noexcept = default;

    Expression() = default;

    Expression(const Expression_component &ec)
            : components({ec}) {}
    Expression(const std::initializer_list<Expression_component> &list)
            : components(list) {}
    void clear() {components.clear(); rpn = false;}
    bool empty() const {return components.empty();}
    void push_back(const Expression_component &ec) {components.emplace_back(ec);}
    void push_front(const Expression_component &ec) {components.insert(components.begin(), ec);}
    void emplace_back(const std::string &ec, Expression_component::component_type t) {components.emplace_back(ec, t);}
    void emplace_back(const hdl_integer &ec) {components.emplace_back(ec, Expression_component::number);}
    std::set<qualified_identifier> get_dependencies()const override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Parameter_value_base> &value) override;
    void propagate_function(const HDL_function_def &def) override;
    std::string print() const override;
    Expression to_rpm() const;
    void set_rpn(bool s) {rpn = s;}
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;
    int64_t get_size() override;
    std::variant<hdl_integer, double> evaluate_binary_expression(resolved_parameter op_a, resolved_parameter op_b, Expression_component::sv_operators operation);
    std::variant<hdl_integer, double> evaluate_unary_expression(resolved_parameter operand, Expression_component::sv_operators operation);

    std::variant<hdl_integer, double> evaluate_cast(resolved_parameter operand, const std::string &operation);

    void add_index(const Expression &idx);

    friend bool operator==(const Expression &lhs, const Expression &rhs) {
        return std::tie(lhs.components, lhs.rpn) == std::tie(rhs.components, rhs.rpn);
    }
    friend bool operator!=(const Expression &lhs, const Expression &rhs) {
        return !(lhs == rhs);
    }

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    std::shared_ptr<Parameter_value_base> clone_ptr() const override {
        return std::make_shared<Expression>(*this);  // Copy constructor
    }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(components, rpn);
    }

    bool isEqual(const Parameter_value_base& other) const override {
        // 1. Safe cast: The base class operator== already verified
        // that this->type == other.type (which is 'expression')
        const auto& rhs = static_cast<const Expression&>(other);

        // 2. Compare the internal state
        // This will use the operator== of Expression_component for each element in the vector
        return std::tie(components, rpn) == std::tie(rhs.components, rhs.rpn);
    }
    uint64_t current_size=0;
};


#endif //ANANKE_EXPRESSION_HPP