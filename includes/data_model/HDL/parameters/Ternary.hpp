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
#include "data_model/HDL/parameters/Parameter_value_base.hpp"
#include "data_model/HDL/parameters/Expression.hpp"


class Ternary : public Parameter_value_base{
public:
    Ternary();
    void set_condition(const Expression &c) {condition = c;}
    void set_true_value(const std::shared_ptr<Parameter_value_base> &v) {true_value = v;}
    void set_false_value(const std::shared_ptr<Parameter_value_base> &v) {false_value =v;}

    [[nodiscard]] std::set<qualified_identifier> get_dependencies()const override;
    bool propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value) override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Parameter_value_base> &value) override;
    std::optional<resolved_parameter> evaluate(bool pack_result) override;
    [[nodiscard]] std::string print() const override;

    int64_t get_depth() override;

    [[nodiscard]] std::shared_ptr<Parameter_value_base> clone_ptr() const override;

    void set_container_sizes(const resolved_type &s) override {}


    template<class Archive>
    void serialize( Archive & ar ) {
        ar(condition, true_value, false_value);
    }

protected:
    param_value_type type = ternary;
    [[nodiscard]] bool isEqual(const Parameter_value_base& other) const override;

private:
    Expression condition;
    std::shared_ptr<Parameter_value_base> true_value;
    std::shared_ptr<Parameter_value_base> false_value;

};


#endif //ANANKE_TERNARY_HPP