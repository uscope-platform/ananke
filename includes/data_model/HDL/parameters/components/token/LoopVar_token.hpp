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

#ifndef ANANKE_LOOP_VAR_TOKEN_HPP
#define ANANKE_LOOP_VAR_TOKEN_HPP

#include <string>

#include <cereal/types/string.hpp>

#include "data_model/HDL/parameters/components/Expression_base.hpp"


class LoopVar_token : public Expression_base {
public:
    LoopVar_token() = default;

    explicit LoopVar_token(std::string n) : name(std::move(n)) {}

    parameter_deps_t get_dependencies() const override {
        parameter_deps_t result;

        result.loop_vars.insert(qualified_identifier(name));
        return result;
    }

    std::expected<resolved_parameter, solver_errors> evaluate(
        const std::map<qualified_identifier, resolved_parameter> &context,
        const std::optional<resolved_type> &expected_type = std::nullopt) override;

    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &,
        const std::optional<resolved_type> & = std::nullopt) const override {
        return std::nullopt;
    }

    std::string print() const override {
        return name;
    }

    std::string get_name() const { return name; }

    template<class Archive>
    void serialize(Archive & ar) {
        ar(name);
    }

private:
    bool isEqual(const Expression_base& other) const override {
        return name == static_cast<const LoopVar_token&>(other).name;
    }

    std::string name;
};

#endif //ANANKE_LOOP_VAR_TOKEN_HPP
