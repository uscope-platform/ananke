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

#include "data_model/HDL/parameters/components/Expression_v2.hpp"

bool Expression_v2::isEqual(const Parameter_value_base &other) const {
    const auto& other_exp = static_cast<const Expression_v2&>(other);
    bool ret = true;
    ret &= *lhs == *other_exp.lhs;
    ret &= *rhs == *other_exp.rhs;
    ret &=  operation == other_exp.operation;
    return ret;
}

void Expression_v2::set_container_sizes(const resolved_type &s,
    const std::map<qualified_identifier, resolved_parameter> &context) {
}

bool operator==(const Expression_v2 &lhs, const Expression_v2 &rhs) {
    bool ret = true;
    ret &= *lhs.lhs == *rhs.lhs;
    ret &= *lhs.rhs == *rhs.rhs;
    ret &= lhs.operation == rhs.operation;
    return ret;
}

std::optional<resolved_parameter> Expression_v2::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    return Parameter_value_base::evaluate(context);
}

int64_t Expression_v2::get_size() {
    return Parameter_value_base::get_size();
}

std::set<qualified_identifier> Expression_v2::get_dependencies() const {
    return Parameter_value_base::get_dependencies();
}

std::variant<hdl_integer, double> Expression_v2::evaluate_binary_expression(resolved_parameter op_a,
    resolved_parameter op_b) {
    return 0;
}

std::variant<hdl_integer, double> Expression_v2::evaluate_unary_expression(resolved_parameter operand) {
    return 0;
}