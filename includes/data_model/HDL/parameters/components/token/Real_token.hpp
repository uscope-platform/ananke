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

#ifndef ANANKE_REAL_TOKEN_HPP
#define ANANKE_REAL_TOKEN_HPP

#include "../Expression_base.hpp"

class Real_token: public Expression_base {
public:
    Real_token() = default;
    Real_token(const Real_token &c);

    explicit Real_token(double v);
    explicit Real_token(const std::string &s);

    parameter_deps_t get_dependencies() const override {return {};}
    void propagate_function(const hdl_function_def_ptr &def) override {}
    std::expected<resolved_parameter, solver_errors> evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) override;

    std::string print() const override;

    friend bool operator==(const Real_token &lhs, const Real_token &rhs);

    void set_value(const resolved_parameter &v) {value = v;}
    std::optional<resolved_parameter> get_value() const {return value;}

    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(value);
    }

private:
    bool isEqual(const Expression_base& other) const override;

    resolved_parameter value;
};

#endif //ANANKE_REAL_TOKEN_HPP
