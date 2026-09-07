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

#include "../Expression_base.hpp"

class Numeric_token: public Expression_base {
public:
    Numeric_token() = default;
    Numeric_token(const Numeric_token &c);

    explicit Numeric_token(const std::string &s);
    explicit Numeric_token(std::variant<hdl_integer, double> n, int64_t b_s);

    parameter_deps_t get_dependencies() const override {return {};}
    void propagate_function(const hdl_function_def_ptr &def) override {}
    std::expected<resolved_parameter, solver_errors> evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) override;

    std::string print() const override;
    int64_t get_size();

    friend bool operator==(const Numeric_token &lhs, const Numeric_token &rhs);

    void set_value(const resolved_parameter &v) {value = v;}
    std::optional<resolved_parameter> get_value() const {return value;}

    void set_binary_size(int64_t s) {binary_size = s;}
    void set_sized_explicit(bool e) {sized_explicit = e;}
    [[nodiscard]] bool is_sized_explicit() const {return sized_explicit;}

    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(value, binary_size, sized_explicit);
    }

private:

    struct numeric_parse_result {
        resolved_parameter value;
        int64_t binary_size;
        bool sized_explicit;
    };

    struct literal_qualifiers {
        std::string_view digits;
        int base;
        int64_t explicit_size;
        bool sized_explicit;
        bool based;
    };

    static numeric_parse_result process_number(const std::string_view &s);

    static std::string_view strip_sign(const std::string_view &s, bool &is_signed, bool &is_negative);
    static literal_qualifiers parse_qualifiers(const std::string_view &body, bool &is_signed);
    static int detect_base(std::string_view &digits);
    static std::string purge_underscores(const std::string_view &digits);
    static std::pair<resolved_parameter, int64_t> parse_integer_digits(const std::string &digits, int base,
        bool is_signed, int64_t explicit_size);
    static std::pair<resolved_parameter, int64_t> finalize_integer(hdl_integer value, int64_t explicit_size, bool is_signed);
    static std::pair<resolved_parameter, int64_t> process_wide_integer(const std::string_view &s, uint8_t base,
        bool signed_number, int64_t explicit_size);

    bool isEqual(const Expression_base& other) const override;

    resolved_parameter value = 0;

    int64_t binary_size = 0;
    bool sized_explicit = false;

};

#endif //ANANKE_NUMERIC_TOKEN_HPP
