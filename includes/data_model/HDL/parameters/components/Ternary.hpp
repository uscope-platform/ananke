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
#include "Expression_base.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"


class Ternary : public Expression_base{
public:
    Ternary() = default;
    void set_condition(const std::shared_ptr<Expression_base> &c) {condition = c;}
    void set_true_value(const std::shared_ptr<Expression_base> &v) {true_value = v;}
    void set_false_value(const std::shared_ptr<Expression_base> &v) {false_value =v;}

    [[nodiscard]] parameter_deps_t get_dependencies()const override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Expression_base> &value) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;
    [[nodiscard]] std::string print() const override;

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override {}


    template<class Archive>
    void serialize( Archive & ar ) {
        ar(condition, true_value, false_value);
    }

protected:
    [[nodiscard]] bool isEqual(const Expression_base& other) const override;

private:
    std::shared_ptr<Expression_base> condition;
    std::shared_ptr<Expression_base> true_value;
    std::shared_ptr<Expression_base> false_value;

};


#endif //ANANKE_TERNARY_HPP