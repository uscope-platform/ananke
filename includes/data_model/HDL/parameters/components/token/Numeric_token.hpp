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


#ifndef ANANKE_NUMERIC_TOKEN_HPP
#define ANANKE_NUMERIC_TOKEN_HPP


#include <charconv>

#include "../Parameter_value_base.hpp"

class Numeric_token: public Parameter_value_base {
public:
    Numeric_token() = default;
    Numeric_token(const Numeric_token &c);


    explicit Numeric_token(const std::string &s);
    explicit Numeric_token(std::variant<hdl_integer, double> n, int64_t b_s);

    parameter_deps_t get_dependencies() const override {return {};}
    void propagate_function(const HDL_function_def &def) override {}
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;

    std::string print() const override;
    int64_t get_size() override;


    friend bool operator==(const Numeric_token &lhs, const Numeric_token &rhs);

    void set_value(const resolved_parameter &v) {value = v;}
    std::optional<resolved_parameter> get_value() const {return value;}


    void set_binary_size(int64_t s) {binary_size = s;}

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(value, binary_size);
    }

private:

    static std::pair<resolved_parameter, int64_t> process_number(const std::string_view &s);
    static std::pair<resolved_parameter, int64_t> process_wide_integer(const std::string_view &s, uint8_t base, bool signed_number);


    bool isEqual(const Parameter_value_base& other) const override;


    resolved_parameter value = 0;

    int64_t binary_size = 0;
    int64_t container_size = 0;


};


#endif //ANANKE_NUMERIC_TOKEN_HPP
