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

#include "data_model/HDL/parameters/components/token/String_token.hpp"
#include <cereal/types/polymorphic.hpp>

CEREAL_REGISTER_TYPE(String_token)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, String_token)

String_token::String_token(const String_token &c) {
    value = c.value;
}

String_token::String_token(const std::string &s) {
    value = resolved_parameter(s);
}

std::optional<resolved_parameter> String_token::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    return value;
}

std::string String_token::print() const {
    if (value.is_string())
        return "\"" + value.get_string() + "\"";
    return value.get_string();
}

bool operator==(const String_token &lhs, const String_token &rhs) {
    return lhs.value == rhs.value;
}

bool String_token::isEqual(const Expression_base &other) const {
    const auto& rhs = static_cast<const String_token&>(other);
    return value == rhs.value;
}
