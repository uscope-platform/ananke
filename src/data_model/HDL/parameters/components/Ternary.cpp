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


#include "data_model/HDL/parameters/components/Ternary.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Ternary)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Parameter_value_base, Ternary)


parameter_deps_t Ternary::get_dependencies() const {
    parameter_deps_t ret_val;
    ret_val.merge(condition->get_dependencies());
    ret_val.merge(true_value->get_dependencies());
    ret_val.merge(false_value->get_dependencies());

    return ret_val;
}


void Ternary::propagate_expression(const qualified_identifier &constant_id,
    const std::shared_ptr<Parameter_value_base> &value) {
    condition->propagate_expression(constant_id, value);
    true_value->propagate_expression(constant_id, value);
    false_value->propagate_expression(constant_id, value);
}

std::optional<resolved_parameter> Ternary::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    auto condition_value = condition->evaluate(context);
    if (!condition_value.has_value()) return std::nullopt;
    auto int_val = condition_value.value().get_integer();
    if (int_val == 0) {
        return false_value->evaluate(context);
    } else {
        return true_value->evaluate(context);
    }
}

std::string Ternary::print() const {
    std::ostringstream oss;
    oss << condition->print();
    oss << " ? ";
    if (true_value) oss << true_value->print();
    oss << " : ";
    if (false_value) oss << false_value->print();
    return oss.str();
}


bool Ternary::isEqual(const Parameter_value_base &other) const {
        const auto& rhs = static_cast<const Ternary&>(other);

    bool ret_val = true;
    ret_val &= *condition == *rhs.condition;
    ret_val &= (true_value && rhs.true_value) && *true_value == *rhs.true_value;
    ret_val &= (false_value && rhs.false_value) && *false_value == *rhs.false_value;
    return ret_val;
}
