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

#include "data_model/HDL/parameters/components/token/LoopVar_token.hpp"

#include <spdlog/spdlog.h>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(LoopVar_token)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, LoopVar_token)
CEREAL_REGISTER_DYNAMIC_INIT(LoopVar_token)

std::expected<resolved_parameter, solver_errors> LoopVar_token::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context,
    const std::optional<resolved_type> &expected_type) {
    auto it = context.find(qualified_identifier(name));
    if (it != context.end()) {
        const auto &resolved = it->second;
        if (resolved.is_undefined()) {
            spdlog::warn("Parameter {} is undefined, using 0 as a default", name);
        }
        return resolved;
    }
    return std::unexpected{missing_value};
}
