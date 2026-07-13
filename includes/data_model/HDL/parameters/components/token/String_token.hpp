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


#ifndef ANANKE_STRING_TOKEN_HPP
#define ANANKE_STRING_TOKEN_HPP

#include <string>

#include "../Expression_base.hpp"

class String_token: public Expression_base {
public:
    String_token() = default;
    String_token(const String_token &c);

    explicit String_token(const std::string &s);

    parameter_deps_t get_dependencies() const override {return {};}
    void propagate_function(const HDL_function_def &def) override {}
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;
    std::string print() const override;
    int64_t get_size() override {return 0;}

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override {}

    void set_value(const resolved_parameter &v) {value = v;}
    std::optional<resolved_parameter> get_value() const {return value;}

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(value);
    }

    friend bool operator==(const String_token &lhs, const String_token &rhs);

private:
    bool isEqual(const Expression_base& other) const override;

    resolved_parameter value;
};

#endif
