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


#include <charconv>
#include "data_model/HDL/parameters/components/token/Real_token.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Real_token)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, Real_token)

Real_token::Real_token(const Real_token &c) {
    value = c.value;
}

Real_token::Real_token(double v) {
    value = v;
}

Real_token::Real_token(const std::string &s) {
    double v;
    std::from_chars(s.data(), s.data() + s.size(), v);
    value = v;
}

std::expected<resolved_parameter, solver_errors> Real_token::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) {
    return value;
}

std::optional<resolved_type> Real_token::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) const {
    resolved_type result;
    result.is_real = true;
    return result;
}

std::string Real_token::print() const {
    return std::to_string(value.get_real());
}

bool operator==(const Real_token &lhs, const Real_token &rhs) {
    return lhs.value == rhs.value;
}

bool Real_token::isEqual(const Expression_base &other) const {
    const auto& rhs = static_cast<const Real_token&>(other);
    return value == rhs.value;
}
