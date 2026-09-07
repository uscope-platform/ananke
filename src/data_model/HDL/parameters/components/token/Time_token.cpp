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
#include "data_model/HDL/parameters/components/token/Time_token.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Time_token)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, Time_token)

Time_token::Time_token(const Time_token &c) {
    value = c.value;
}

Time_token::Time_token(double v) {
    value = v;
}

Time_token::Time_token(const std::string &s) {
    double scale = 1.0;
    size_t unit_size = 1;
    if (s.ends_with("fs")) { scale = 1e-15; unit_size = 2; }
    else if (s.ends_with("ps")) { scale = 1e-12; unit_size = 2; }
    else if (s.ends_with("ns")) { scale = 1e-9; unit_size = 2; }
    else if (s.ends_with("us")) { scale = 1e-6; unit_size = 2; }
    else if (s.ends_with("ms")) { scale = 1e-3; unit_size = 2; }

    auto number = s.substr(0, s.size() - unit_size);
    double v;
    std::from_chars(number.data(), number.data() + number.size(), v);
    value = v * scale;
}

std::expected<resolved_parameter, solver_errors> Time_token::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) {
    return value;
}

std::optional<resolved_type> Time_token::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) const {
    resolved_type result;
    result.is_real = true;
    return result;
}

std::string Time_token::print() const {
    return std::to_string(value.get_real());
}

bool operator==(const Time_token &lhs, const Time_token &rhs) {
    return lhs.value == rhs.value;
}

bool Time_token::isEqual(const Expression_base &other) const {
    const auto& rhs = static_cast<const Time_token&>(other);
    return value == rhs.value;
}
