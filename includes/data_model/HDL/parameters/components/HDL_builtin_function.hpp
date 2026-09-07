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

#ifndef ANANKE_HDL_BUILTIN_FUNCTION_HPP
#define ANANKE_HDL_BUILTIN_FUNCTION_HPP

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include "data_model/HDL/parameters/components/Expression_base.hpp"

class HDL_builtin_function : public Expression_base {
public:
    enum class function {
        // Type / size queries
        bits, size, left, right, high, low, dimensions, unpacked_dimensions,
        typename_fn, signed_fn, unsigned_fn,
        // Numeric conversions and math
        rtoi, itor, ceil, floor, round, truncate,
        ln, log10, log2, clog2, sqrt, pow, exp, hypot,
        min, max, countones, countbits,
        sin, cos, tan, asin, acos, atan, atan2,
        sinh, cosh, tanh, asinh, acosh, atanh,
        // Vector / string helpers
        onehot, onehot0, isunknown, isunbounded,
        realtobits, bitstoreal, shortrealtobits, bitstoshortreal,
        len, substr, to_lower, to_upper, atoi, compare, sformatf,
        // Random / distribution (recognized, not constant-evaluable)
        random, urandom, urandom_range, dist_uniform, dist_normal, dist_exponential,
        dist_poisson, dist_chi_square, dist_t, dist_erlang
    };


    static std::optional<function> from_sv(std::string_view name);
    static std::optional<function> from_vhdl(std::string_view name);
    static std::optional<function> parse(std::string_view name);
    static std::string_view sv_name(function f);

    static bool is_known(std::string_view name);


    HDL_builtin_function() = default;
    explicit HDL_builtin_function(function f) : function_kind(f) {}

    void set_function(function f) { function_kind = f; }
    function get_function() const { return function_kind; }

    void add_argument(const std::shared_ptr<Expression_base> &a) { arguments.push_back(a); }
    const std::vector<std::shared_ptr<Expression_base>>& get_arguments() const { return arguments; }

    std::expected<resolved_parameter, solver_errors> evaluate(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) override;
    std::string print() const override;
    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_def_ptr &def) override {}
    void set_container_sizes(const resolved_type &s,
                             const std::map<qualified_identifier, resolved_parameter> &context = {}) override;
    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const override;

    template<class Archive>
    void serialize(Archive & ar) {
        ar(function_kind, arguments);
    }

protected:
    bool isEqual(const Expression_base& other) const override;

private:
    std::expected<resolved_parameter, solver_errors> evaluate_type_query(
        const std::map<qualified_identifier, resolved_parameter> &context);
    std::expected<resolved_parameter, solver_errors> evaluate_typename(
        const std::map<qualified_identifier, resolved_parameter> &context);
    std::expected<resolved_parameter, solver_errors> evaluate_signedness(
        const std::map<qualified_identifier, resolved_parameter> &context);

    function function_kind = function::bits;
    std::vector<std::shared_ptr<Expression_base>> arguments;
};

#endif //ANANKE_HDL_BUILTIN_FUNCTION_HPP