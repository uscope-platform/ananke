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


#ifndef ANANKE_TERNARY_HPP
#define ANANKE_TERNARY_HPP

#include <memory>
#include "Parameter_value_base.hpp"
#include "Expression.hpp"


class Ternary : public Parameter_value_base{
public:
    Ternary() = default;
    void set_condition(const Expression &c) {condition = c;}
    void set_true_value(const std::shared_ptr<Parameter_value_base> &v) {true_value = v;}
    void set_false_value(const std::shared_ptr<Parameter_value_base> &v) {false_value =v;}

    [[nodiscard]] std::set<qualified_identifier> get_dependencies()const override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Parameter_value_base> &value) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;
    [[nodiscard]] std::string print() const override;

    [[nodiscard]] std::shared_ptr<Parameter_value_base> clone_ptr() const override;

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override {}


    template<class Archive>
    void serialize( Archive & ar ) {
        ar(condition, true_value, false_value);
    }

protected:
    [[nodiscard]] bool isEqual(const Parameter_value_base& other) const override;

private:
    Expression condition;
    std::shared_ptr<Parameter_value_base> true_value;
    std::shared_ptr<Parameter_value_base> false_value;

};


#endif //ANANKE_TERNARY_HPP