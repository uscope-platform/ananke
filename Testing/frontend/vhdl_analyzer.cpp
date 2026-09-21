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

#include <gtest/gtest.h>

#include <cmath>

#include "frontend/analysis/vhdl/vhdl_analyzer.hpp"
#include "analysis/loop_solver.hpp"
#include "analysis/parameter_solver.hpp"
#include "data_model/data_store.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/parameters/components/Cast.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/parameters/components/token/Real_token.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/common/resolved_parameter.hpp"
#include "data_model/HDL/parameters/common/qualified_identifier.hpp"
#include "data_model/documentation/module_documentation.hpp"
#include "data_model/documentation/register_documentation.hpp"

std::shared_ptr<hdl_resource_statement> parse_first_entity(const std::string &content) {
    vhdl_analyzer analyzer("test.vhd");
    auto result = analyzer.analyze_content(content, "test.vhd");
    auto &first = result.get_content()[0];
    EXPECT_TRUE(first->is<hdl_resource_statement>());
    return std::make_shared<hdl_resource_statement>(first->as<hdl_resource_statement>());
}

std::shared_ptr<hdl_resource_statement> parse_entity(const std::string &content, const std::string &name) {
    vhdl_analyzer analyzer("test.vhd");
    auto result = analyzer.analyze_content(content, "test.vhd");
    for (auto &c : result.get_content()) {
        if (c->is<hdl_resource_statement>() && c->as<hdl_resource_statement>().getName() == name)
            return std::make_shared<hdl_resource_statement>(c->as<hdl_resource_statement>());
    }
    return nullptr;
}

std::shared_ptr<hdl_instance_statement> make_instance(const std::string &name, const std::string &type) {
    auto inst = std::make_shared<hdl_instance_statement>();
    inst->set_name(name);
    inst->set_type(type);
    inst->set_dependency_class(module);
    return inst;
}

std::shared_ptr<HDL_parameter> make_integer_param(const std::string &name,
                                                  const std::shared_ptr<Expression_base> &value) {
    auto p = std::make_shared<HDL_parameter>(name);
    auto t = std::make_shared<HDL_simple_type>();
    t->set_type_name("integer");
    t->set_signed(true);
    p->set_type(t);
    p->set_raw_value(value);
    return p;
}

// Expected parameter for an instance generic-map override: carries the actual
// expression value but no formal type (the type is resolved downstream).
std::shared_ptr<HDL_parameter> make_override_param(const std::string &name,
                                                   const std::shared_ptr<Expression_base> &value) {
    auto p = std::make_shared<HDL_parameter>(name);
    p->set_raw_value(value);
    return p;
}

// Expected parameter with the given scalar type name and default value.
std::shared_ptr<HDL_parameter> make_typed_param(const std::string &name,
                                                const std::string &type_name,
                                                const std::shared_ptr<Expression_base> &value) {
    auto p = std::make_shared<HDL_parameter>(name);
    auto t = std::make_shared<HDL_simple_type>();
    t->set_type_name(type_name);
    p->set_type(t);
    p->set_raw_value(value);
    return p;
}

std::shared_ptr<Expression_v2> make_binary(Expression_v2::expression_operator op,
                                           const std::shared_ptr<Expression_base> &lhs,
                                           const std::shared_ptr<Expression_base> &rhs) {
    auto e = std::make_shared<Expression_v2>();
    e->set_lhs(lhs);
    e->set_rhs(rhs);
    e->set_operation(op);
    return e;
}

int64_t eval_generic(const std::string &decl_body, const std::string &gname) {
    std::string pattern = "entity top is\n    generic ( " + decl_body + " );\nend top;\n";
    auto res = parse_first_entity(pattern);
    Parameters_map declared_params;
    for (const auto &pp : res->get_parameter_statements()) declared_params.insert(pp);
    auto param = declared_params.get(gname);
    if (!param) return -999999;
    std::map<qualified_identifier, resolved_parameter> ctx;
    auto val = param->evaluate(ctx);
    if (!val.has_value() || !val->is_integer()) return -999999;
    return val->get_integer().get_value();
}

// Build an expected integer parameter whose default value is a concatenation
// (VHDL aggregate) of the given numeric literals.
std::shared_ptr<HDL_parameter> make_concat_param(const std::string &name,
                                                 std::initializer_list<std::string> values) {
    auto concat = std::make_shared<Concatenation>();
    for (const auto &v : values)
        concat->add_component(std::make_shared<Numeric_token>(v));
    return make_integer_param(name, concat);
}

TEST(vhdl_analyzer, entity_name_extraction) {
    auto test_pattern = R"(
entity test_mod is
end test_mod;
)";

    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("test_mod");
    expected.set_type(module);
    expected.set_line_n(2);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, architecture_statement_binding) {
    auto test_pattern = R"(
entity top is
end top;

architecture rtl of top is
    component ANDGATE is
        port (
            I1 : in std_logic;
            O  : out std_logic
        );
    end component ANDGATE;
begin
    and1 : ANDGATE
        port map (
            I1 => a,
            O  => b
        );
end rtl;
)";

    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    auto inst = make_instance("and1", "andgate");
    std::unordered_map<std::string, std::vector<HDL_net>> inst_ports;
    inst_ports["i1"] = {HDL_net("a")};
    inst_ports["o"] = {HDL_net("b")};
    inst->set_ports(inst_ports);
    expected.add_statement(inst);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, case_insensitive_entity_and_architecture_binding) {
    auto test_pattern = R"(
ENTITY HALF_ADDER IS
END HALF_ADDER;

ARCHITECTURE RTL OF HALF_ADDER IS
    COMPONENT ANDGATE IS
        PORT (
            I1 : IN STD_LOGIC;
            O  : OUT STD_LOGIC
        );
    END COMPONENT ANDGATE;
BEGIN
    AND1 : ANDGATE
        PORT MAP (
            I1 => A,
            O  => B
        );
END RTL;
)";

    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("half_adder");
    expected.set_type(module);
    expected.set_line_n(2);
    auto inst = make_instance("and1", "andgate");
    std::unordered_map<std::string, std::vector<HDL_net>> inst_ports;
    inst_ports["i1"] = {HDL_net("a")};
    inst_ports["o"] = {HDL_net("b")};
    inst->set_ports(inst_ports);
    expected.add_statement(inst);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, fixture_regression) {
    // Mirrors the test data stored in Testing/check_files/test_vhdl_module.vhd,
    // which the repository walker and the analysis suite rely on.
    auto test_pattern = R"(
library ieee;
use IEEE.std_logic_1164.all;

entity half_adder is
  port (
    i_bit1  : in std_logic;
    i_bit2  : in std_logic;
    --
    o_sum   : out std_logic;
    o_carry : out std_logic
    );
end half_adder;

architecture rtl of half_adder is

    component ANDGATE is
      port (
        I1 : in std_logic;
        I2 : in std_logic;
        O  : out std_logic);
    end component ANDGATE;

begin
    o_sum   <= i_bit1 xor i_bit2;
    and_component : ANDGATE
    port map (
      I1  => i_bit1,
      I2  => i_bit2,
      O => o_carry
    );

end rtl;
)";

    auto res = parse_first_entity(test_pattern);

    std::unordered_map<std::string, HDL_port> ports;
    ports["i_bit1"] = {input_port};
    ports["i_bit2"] = {input_port};
    ports["o_sum"] = {output_port};
    ports["o_carry"] = {output_port};

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("half_adder");
    expected.set_type(module);
    expected.set_line_n(5);
    expected.set_ports(ports);

    auto inst = make_instance("and_component", "andgate");
    std::unordered_map<std::string, std::vector<HDL_net>> inst_ports;
    inst_ports["i1"] = {HDL_net("i_bit1")};
    inst_ports["i2"] = {HDL_net("i_bit2")};
    inst_ports["o"] = {HDL_net("o_carry")};
    inst->set_ports(inst_ports);
    expected.add_statement(inst);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, generic_numeric_single) {
    auto test_pattern = R"(
entity top is
    generic (
        N : integer := 8
    );
end top;
)";

    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(make_integer_param("n", std::make_shared<Numeric_token>("8")));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, generic_math_expression) {
    auto test_pattern = R"(
entity top is
    generic (
        WIDTH : integer := 2**N-1
    );
end top;
)";

    auto res = parse_first_entity(test_pattern);

    // 2**N-1 parses as (2**N) - 1
    auto pow = make_binary(Expression_v2::power,
                           std::make_shared<Numeric_token>("2"),
                           std::make_shared<Identifier_token>(qualified_identifier("n")));
    auto expr = make_binary(Expression_v2::subtract, pow, std::make_shared<Numeric_token>("1"));

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(make_integer_param("width", expr));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, generic_multiple_and_multiply) {
    auto test_pattern = R"(
entity top is
    generic (
        A : integer := 4;
        B : integer := A * 8
    );
end top;
)";

    auto res = parse_first_entity(test_pattern);

    auto b_expr = make_binary(Expression_v2::multiply,
                              std::make_shared<Identifier_token>(qualified_identifier("a")),
                              std::make_shared<Numeric_token>("8"));

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(make_integer_param("a", std::make_shared<Numeric_token>("4")));
    expected.add_statement(make_integer_param("b", b_expr));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, generic_identifier_reference) {
    auto test_pattern = R"(
entity top is
    generic (
        WIDTH : integer := DATA_WIDTH
    );
end top;
)";

    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(make_integer_param("width",
        std::make_shared<Identifier_token>(qualified_identifier("data_width"))));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, generic_expression_evaluates) {
    auto test_pattern = R"(
entity top is
    generic ( WIDTH : integer := 2**N-1 );
end top;
)";

    auto res = parse_first_entity(test_pattern);
    Parameters_map declared_params;
    for (const auto &pp : res->get_parameter_statements()) declared_params.insert(pp);
    auto param = declared_params.get("width");
    ASSERT_NE(param, nullptr);

    std::map<qualified_identifier, resolved_parameter> ctx;
    ctx[qualified_identifier("n")] = resolved_parameter(8);

    auto val = param->evaluate(ctx);
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val->is_integer());
    EXPECT_EQ(val->get_integer().get_value(), 255);
}

TEST(vhdl_analyzer, generic_range_subtype_not_polluted) {
    // The range constraint expressions in the subtype must not leak into the
    // default-value expression.
    auto test_pattern = R"(
entity top is
    generic (
        N : integer range 0 to 7 := 4
    );
end top;
)";

    auto res = parse_first_entity(test_pattern);
    Parameters_map declared_params;
    for (const auto &pp : res->get_parameter_statements()) declared_params.insert(pp);
    auto param = declared_params.get("n");
    ASSERT_NE(param, nullptr);

    std::map<qualified_identifier, resolved_parameter> ctx;
    auto val = param->evaluate(ctx);
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val->is_integer());
    EXPECT_EQ(val->get_integer().get_value(), 4);
}

TEST(vhdl_analyzer, generic_multiple_names_shared_default) {
    auto test_pattern = R"(
entity top is
    generic (
        A, B, C : integer := 8
    );
end top;
)";

    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(make_integer_param("a", std::make_shared<Numeric_token>("8")));
    expected.add_statement(make_integer_param("b", std::make_shared<Numeric_token>("8")));
    expected.add_statement(make_integer_param("c", std::make_shared<Numeric_token>("8")));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, literal_bit_string) {
    EXPECT_EQ(eval_generic("V : integer := x\"FF\"", "v"), 255);
    EXPECT_EQ(eval_generic("V : integer := b\"1010\"", "v"), 10);
    EXPECT_EQ(eval_generic("V : integer := o\"17\"", "v"), 15);
    EXPECT_EQ(eval_generic("V : integer := 8x\"1F\"", "v"), 31);
}

TEST(vhdl_analyzer, literal_signed_bit_string) {
    // 8 bits of 0xFF interpreted as two's-complement signed -> -1.
    EXPECT_EQ(eval_generic("V : integer := 8sx\"FF\"", "v"), -1);
}

TEST(vhdl_analyzer, literal_character) {
    // Character literal 'a' maps to its ASCII value.
    EXPECT_EQ(eval_generic("C : integer := 'a'", "c"), 97);
}

TEST(vhdl_analyzer, operator_mod_rem) {
    // VHDL `mod` is floor modulo: 7 mod -3 = -2 (sign of divisor).
    EXPECT_EQ(eval_generic("M : integer := 7 mod -3", "m"), -2);
    // VHDL `rem` is truncated: 7 rem -3 = 1 (sign of dividend).
    EXPECT_EQ(eval_generic("R : integer := 7 rem -3", "r"), 1);
}

TEST(vhdl_analyzer, operator_rotate) {
    // Rotate operates on the default 32-bit container width, so small left
    // rotates are just left shifts: 6 rol 1 = 12.
    EXPECT_EQ(eval_generic("L : integer := 6 rol 1", "l"), 12);
    // Rotating right by 31 on a 32-bit container equals rotating left by 1:
    // 1 ror 31 = 2.
    EXPECT_EQ(eval_generic("R : integer := 1 ror 31", "r"), 2);
}

TEST(vhdl_analyzer, operator_condition_qq) {
    // VHDL `??` condition operator normalizes a condition to 0/1.
    // Note: `??` is a trigraph in C++, so the literal is split.
    EXPECT_EQ(eval_generic("V : integer := ?" "?5", "v"), 1);
    EXPECT_EQ(eval_generic("V : integer := ?" "?0", "v"), 0);
    EXPECT_EQ(eval_generic("V : integer := ?" "?(1<2)", "v"), 1);
}

TEST(vhdl_analyzer, aggregate_positional) {
    auto test_pattern = R"(
entity top is
    generic ( V : integer := (1, 2, 3) );
end top;
)";

    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(make_concat_param("v", {"1", "2", "3"}));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, aggregate_with_choices) {
    auto test_pattern = R"(
entity top is
    generic ( V : integer := (0 => 7, 1 => 8) );
end top;
)";

    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(make_concat_param("v", {"7", "8"}));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, aggregate_not_confused_with_parenthesized_expr) {
    // `(1<2)` is a parenthesized relational expression, not a single-element
    // aggregate; `??` of it must normalize the boolean to 1.
    EXPECT_EQ(eval_generic("V : integer := ?" "?(1<2)", "v"), 1);
    // A plain parenthesized expression is just the expression value.
    EXPECT_EQ(eval_generic("V : integer := (3)", "v"), 3);
}

double eval_generic_double(const std::string &decl_body, const std::string &gname) {
    std::string pattern = "entity top is\n    generic ( " + decl_body + " );\nend top;\n";
    auto res = parse_first_entity(pattern);
    Parameters_map declared_params;
    for (const auto &pp : res->get_parameter_statements()) declared_params.insert(pp);
    auto param = declared_params.get(gname);
    if (!param) return -999999.0;
    std::map<qualified_identifier, resolved_parameter> ctx;
    auto val = param->evaluate(ctx);
    if (!val.has_value()) return -999999.0;
    if (val->is_real()) return val->get_real();
    if (val->is_integer()) return static_cast<double>(val->get_integer().get_value());
    return -999999.0;
}

TEST(vhdl_analyzer, builtin_function_integer) {
    EXPECT_EQ(eval_generic("V : integer := ceil(3)", "v"), 3);
    EXPECT_EQ(eval_generic("V : integer := floor(3)", "v"), 3);
    EXPECT_EQ(eval_generic("V : integer := minimum(3, 7)", "v"), 3);
    EXPECT_EQ(eval_generic("V : integer := maximum(3, 7)", "v"), 7);
    EXPECT_EQ(eval_generic("V : integer := countones(7)", "v"), 3);
}

TEST(vhdl_analyzer, builtin_function_real) {
    EXPECT_EQ(eval_generic_double("V : real := log2(8)", "v"), std::log2(8));
    EXPECT_EQ(eval_generic_double("V : real := sqrt(16)", "v"), 4.0);
    EXPECT_EQ(eval_generic_double("V : real := ceil(2.5)", "v"), 3.0);
    EXPECT_EQ(eval_generic_double("V : real := floor(2.5)", "v"), 2.0);
}

TEST(vhdl_analyzer, builtin_function_nested) {
    // ceil(log2(N)) evaluates with N from the context.
    auto test_pattern = R"(
entity top is
    generic ( N : integer := 8;
              W : real := ceil(log2(N)) );
end top;
)";
    auto res = parse_first_entity(test_pattern);
    Parameters_map declared_params;
    for (const auto &pp : res->get_parameter_statements()) declared_params.insert(pp);
    auto param = declared_params.get("w");
    ASSERT_NE(param, nullptr);
    std::map<qualified_identifier, resolved_parameter> ctx;
    ctx[qualified_identifier("n")] = resolved_parameter(8);
    auto val = param->evaluate(ctx);
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val->is_real());
    EXPECT_EQ(val->get_real(), 3.0);
}

TEST(vhdl_analyzer, builtin_function_unknown_does_not_crash) {
    auto res = parse_first_entity("entity top is\n    generic ( V : integer := foo(3) );\nend top;\n");
    Parameters_map declared_params;
    for (const auto &pp : res->get_parameter_statements()) declared_params.insert(pp);
    auto param = declared_params.get("v");
    ASSERT_NE(param, nullptr);
}

TEST(vhdl_analyzer, qualified_expression_cast) {
    // `integer'(...)`/`natural'(...)` convert to an integer (real→int rounds).
    EXPECT_EQ(eval_generic("V : integer := integer'(5)", "v"), 5);
    EXPECT_EQ(eval_generic("V : integer := natural'(2)", "v"), 2);
    // `real'(...)` converts to real.
    EXPECT_EQ(eval_generic_double("V : real := real'(5)", "v"), 5.0);
    EXPECT_EQ(eval_generic_double("V : real := real'(2.5)", "v"), 2.5);
    // `boolean'(...)` normalizes to 0/1.
    EXPECT_EQ(eval_generic("V : integer := boolean'(0)", "v"), 0);
    EXPECT_EQ(eval_generic("V : integer := boolean'(3)", "v"), 1);
}

TEST(vhdl_analyzer, qualified_expression_cast_expected) {
    auto test_pattern = R"(
entity top is
    generic ( V : integer := integer'(7) );
end top;
)";
    auto res = parse_first_entity(test_pattern);

    auto cast = std::make_shared<Cast>();
    cast->set_type_cast();
    cast->set_target_type("integer");
    cast->set_content(std::make_shared<Numeric_token>("7"));

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(make_integer_param("v", cast));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, cast_transforms_value) {
    // real -> integer rounds.
    EXPECT_EQ(eval_generic("V : integer := integer'(2.9)", "v"), 3);
    EXPECT_EQ(eval_generic("V : integer := integer'(2.2)", "v"), 2);
    // integer -> real widens the representation.
    EXPECT_EQ(eval_generic_double("V : real := real'(7)", "v"), 7.0);
    // boolean'(non-zero) -> 1 collapses to a 1-bit truth value.
    EXPECT_EQ(eval_generic("V : integer := boolean'(5)", "v"), 1);
}

namespace {
    // Parse a generic default and return the resolved type of the parameter.
    std::shared_ptr<hdl_type> generic_type(const std::string &decl_body, const std::string &gname) {
        std::string pattern = "entity top is\n    generic ( " + decl_body + " );\nend top;\n";
        auto res = parse_first_entity(pattern);
        Parameters_map declared_params;
        for (const auto &pp : res->get_parameter_statements()) declared_params.insert(pp);
        auto param = declared_params.get(gname);
        if (!param) return nullptr;
        return param->get_type();
    }
}

TEST(vhdl_analyzer, type_vector_range) {
    HDL_simple_type expected;
    expected.set_type_name("std_logic_vector");
    expected.set_packed_dimensions({{std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true}});
    ASSERT_EQ(generic_type("V : std_logic_vector(7 downto 0)", "v")->as<HDL_simple_type>(), expected);

    expected = HDL_simple_type();
    expected.set_type_name("std_logic_vector");
    expected.set_packed_dimensions({{std::make_shared<Numeric_token>("31"), std::make_shared<Numeric_token>("0"), true}});
    ASSERT_EQ(generic_type("W : std_logic_vector(31 downto 0)", "w")->as<HDL_simple_type>(), expected);

    expected = HDL_simple_type();
    expected.set_type_name("unsigned");
    expected.set_packed_dimensions({{std::make_shared<Numeric_token>("3"), std::make_shared<Numeric_token>("0"), true}});
    ASSERT_EQ(generic_type("U : unsigned(3 downto 0)", "u")->as<HDL_simple_type>(), expected);

    expected = HDL_simple_type();
    expected.set_type_name("signed");
    expected.set_signed(true);
    expected.set_packed_dimensions({{std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true}});
    ASSERT_EQ(generic_type("S : signed(7 downto 0)", "s")->as<HDL_simple_type>(), expected);
}

TEST(vhdl_analyzer, type_ascending_range) {
    // `0 to 7` ascending produces the same width as `7 downto 0`.
    HDL_simple_type expected;
    expected.set_type_name("std_logic_vector");
    expected.set_packed_dimensions({{std::make_shared<Numeric_token>("0"), std::make_shared<Numeric_token>("7"), true}});
    ASSERT_EQ(generic_type("V : std_logic_vector(0 to 7)", "v")->as<HDL_simple_type>(), expected);
}

TEST(vhdl_analyzer, type_scalar_builtins) {
    HDL_simple_type expected;
    expected.set_type_name("integer");
    expected.set_signed(true);
    ASSERT_EQ(generic_type("N : integer", "n")->as<HDL_simple_type>(), expected);

    expected = HDL_simple_type();
    expected.set_type_name("real");
    expected.set_real(true);
    ASSERT_EQ(generic_type("X : real", "x")->as<HDL_simple_type>(), expected);

    expected = HDL_simple_type();
    expected.set_type_name("std_logic");
    ASSERT_EQ(generic_type("B : std_logic", "b")->as<HDL_simple_type>(), expected);
}

TEST(vhdl_analyzer, type_compound_range_bound) {
    // A symbolic/compound bound like (N-1) downto 0 builds an expression bound.
    auto test_pattern = R"(
entity top is
    generic ( N : integer := 8;
              V : std_logic_vector(N-1 downto 0) );
end top;
)";
    auto res = parse_first_entity(test_pattern);
    Parameters_map declared_params;
    for (const auto &pp : res->get_parameter_statements()) declared_params.insert(pp);
    auto param = declared_params.get("v");
    ASSERT_NE(param, nullptr);

    auto bound = make_binary(Expression_v2::subtract,
                             std::make_shared<Identifier_token>(qualified_identifier("n")),
                             std::make_shared<Numeric_token>("1"));
    HDL_simple_type expected;
    expected.set_type_name("std_logic_vector");
    expected.set_packed_dimensions({{bound, std::make_shared<Numeric_token>("0"), true}});
    ASSERT_EQ(param->get_type()->as<HDL_simple_type>(), expected);
}

namespace {
    // Parse a whole design and look up a typedef declared on the entity resource.
    std::shared_ptr<hdl_type> declared_typedef(const std::string &content, const std::string &tname) {
        auto res = parse_first_entity(content);
        auto typedefs = res->get_typedefs();
        auto it = typedefs.find(tname);
        if (it == typedefs.end()) return nullptr;
        return it->second;
    }
}

TEST(vhdl_analyzer, local_subtype_declaration) {
    auto content = R"(
entity top is
end top;

architecture rtl of top is
    subtype small_int is integer range 0 to 7;
begin
end rtl;
)";
    auto t = declared_typedef(content, "small_int");
    ASSERT_NE(t, nullptr);
    HDL_simple_type expected;
    expected.set_type_name("integer");
    expected.set_signed(true);
    expected.set_packed_dimensions({{std::make_shared<Numeric_token>("0"), std::make_shared<Numeric_token>("7"), true}});
    ASSERT_EQ(t->as<HDL_simple_type>(), expected);
}

TEST(vhdl_analyzer, local_enum_type) {
    auto content = R"(
entity top is
end top;

architecture rtl of top is
    type state is (idle, run, done);
begin
end rtl;
)";
    auto t = declared_typedef(content, "state");
    ASSERT_NE(t, nullptr);
    ASSERT_TRUE(t->is<HDL_enum_type>());
    HDL_enum_type expected;
    expected.members = {{"idle", 0}, {"run", 1}, {"done", 2}};
    ASSERT_EQ(t->as<HDL_enum_type>(), expected);
}

TEST(vhdl_analyzer, local_array_type) {
    auto content = R"(
entity top is
end top;

architecture rtl of top is
    type byte_arr is array(0 to 7) of std_logic;
begin
end rtl;
)";
    auto t = declared_typedef(content, "byte_arr");
    ASSERT_NE(t, nullptr);
    HDL_simple_type expected;
    expected.set_type_name("byte_arr");
    expected.set_packed_dimensions({{std::make_shared<Numeric_token>("0"), std::make_shared<Numeric_token>("7"), true}});
    ASSERT_EQ(t->as<HDL_simple_type>(), expected);
}

TEST(vhdl_analyzer, local_record_type) {
    auto content = R"(
entity top is
end top;

architecture rtl of top is
    type pixel is record
        r, g, b : std_logic_vector(7 downto 0);
    end record;
begin
end rtl;
)";
    auto t = declared_typedef(content, "pixel");
    ASSERT_NE(t, nullptr);
    ASSERT_TRUE(t->is<HDL_struct_type>());
    auto field_t = std::make_shared<HDL_simple_type>();
    field_t->set_type_name("std_logic_vector");
    field_t->set_packed_dimensions({{std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true}});
    HDL_struct_type expected;
    expected.member = {{"r", field_t}, {"g", field_t}, {"b", field_t}};
    ASSERT_EQ(t->as<HDL_struct_type>(), expected);
}

TEST(vhdl_analyzer, local_array_of_subtype) {
    auto content = R"(
entity top is
end top;

architecture rtl of top is
    subtype bit_t is std_logic;
    subtype byte_t is integer range 0 to 255;
    type arr is array(0 to 3) of bit_t;
    type arr2 is array(0 to 3) of byte_t;
begin
end rtl;
)";
    auto t = declared_typedef(content, "arr");
    ASSERT_NE(t, nullptr);
    ASSERT_TRUE(t->is<HDL_simple_type>());
    HDL_simple_type expected;
    expected.set_type_name("arr");
    expected.set_packed_dimensions({{std::make_shared<Numeric_token>("0"), std::make_shared<Numeric_token>("3"), true}});
    ASSERT_EQ(t->as<HDL_simple_type>(), expected);

    // An array of a non-bit-like element type is not representable yet: it
    // degrades to an external (unknown) type reference.
    auto t2 = declared_typedef(content, "arr2");
    ASSERT_NE(t2, nullptr);
    ASSERT_EQ(t2->as<HDL_external_type>(), HDL_external_type(qualified_identifier("arr2")));
}

TEST(vhdl_analyzer, port_extraction_basic) {
    auto test_pattern = R"(
entity top is
    port (
        clk   : in std_logic;
        rst   : in std_logic;
        dout  : out std_logic_vector(7 downto 0);
        din   : inout unsigned(3 downto 0)
    );
end top;
)";
    auto res = parse_first_entity(test_pattern);

    std::unordered_map<std::string, HDL_port> ports;
    ports["clk"] = {input_port};
    ports["rst"] = {input_port};
    ports["dout"] = {output_port};
    ports["din"] = {inout_port};

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.set_ports(ports);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, port_typed_name_list) {
    // `a, b, c : in std_logic` shares one direction across all names.
    auto test_pattern = R"(
entity top is
    port (
        a, b, c : in std_logic
    );
end top;
)";
    auto res = parse_first_entity(test_pattern);

    std::unordered_map<std::string, HDL_port> ports;
    ports["a"] = {input_port};
    ports["b"] = {input_port};
    ports["c"] = {input_port};

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.set_ports(ports);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, port_modes) {
    auto test_pattern = R"(
entity top is
    port (
        i  : in std_logic;
        o  : out std_logic;
        io : inout std_logic;
        b  : buffer std_logic
    );
end top;
)";
    auto res = parse_first_entity(test_pattern);

    std::unordered_map<std::string, HDL_port> ports;
    ports["i"] = {input_port};
    ports["o"] = {output_port};
    ports["io"] = {inout_port};
    ports["b"] = {output_port};   // buffer is treated as output

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.set_ports(ports);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, generic_default_boolean_string) {
    auto test_pattern = R"(
entity top is
    generic (
        WIDTH : integer := 8;
        FLAG  : boolean := true;
        NAME  : string := "blinky"
    );
    port (
        dout : out std_logic_vector(WIDTH - 1 downto 0)
    );
end top;
)";
    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement check_res;
    check_res.set_language(hdl_language::vhdl);
    check_res.set_name("top");
    check_res.set_type(module);
    check_res.set_line_n(2);
    check_res.add_statement(make_integer_param("width", std::make_shared<Numeric_token>("8")));
    check_res.add_statement(make_typed_param("flag", "boolean",
                                          std::make_shared<Identifier_token>(qualified_identifier("true"))));
    // `string` is not a modeled builtin in the type engine: the value is a
    // String_token but the declared type degrades to an external reference.
    auto name = std::make_shared<HDL_parameter>("name");
    name->set_type(std::make_shared<HDL_external_type>(qualified_identifier("string")));
    name->set_raw_value(std::make_shared<String_token>("\"blinky\""));
    check_res.add_statement(name);
    std::unordered_map<std::string, HDL_port> ports;
    ports["dout"] = {output_port};
    check_res.set_ports(ports);

    ASSERT_EQ(*res, check_res);
}

TEST(vhdl_analyzer, instance_generic_map) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    u_sub : entity work.sub
        generic map (
            WIDTH => 16,
            MODE  => 2
        );
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    auto inst = make_instance("u_sub", "sub");
    inst->add_parameter(make_override_param("width", std::make_shared<Numeric_token>("16")));
    inst->add_parameter(make_override_param("mode", std::make_shared<Numeric_token>("2")));

    hdl_resource_statement check;
    check.set_language(hdl_language::vhdl);
    check.set_name("top");
    check.set_type(module);
    check.set_line_n(2);
    check.add_statement(inst);

    ASSERT_EQ(*res, check);
}

TEST(vhdl_analyzer, instance_generic_map_expr) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    u_sub : entity work.sub
        generic map (
            WIDTH => 2**4-1
        );
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    auto pow = make_binary(Expression_v2::power,
                           std::make_shared<Numeric_token>("2"),
                           std::make_shared<Numeric_token>("4"));
    auto expr = make_binary(Expression_v2::subtract, pow, std::make_shared<Numeric_token>("1"));

    auto inst = make_instance("u_sub", "sub");
    inst->add_parameter(make_override_param("width", expr));

    hdl_resource_statement check;
    check.set_language(hdl_language::vhdl);
    check.set_name("top");
    check.set_type(module);
    check.set_line_n(2);
    check.add_statement(inst);

    ASSERT_EQ(*res, check);
}

TEST(vhdl_analyzer, instance_port_map) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    u_sub : entity work.sub
        port map (
            CLK  => clk,
            RST  => open,
            DATA => data_in,
            MODE => 3
        );
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    auto inst = make_instance("u_sub", "sub");
    std::unordered_map<std::string, std::vector<HDL_net>> inst_ports;
    inst_ports["clk"] = {HDL_net("clk")};
    inst_ports["data"] = {HDL_net("data_in")};
    inst_ports["mode"] = {HDL_net("3")};
    inst->set_ports(inst_ports);
    expected.add_statement(inst);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, instance_port_map_complex_actuals) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    u_sub : entity work.sub
        port map (
            DATA => data_bus(7 downto 0),
            BIT  => flag(0),
            MIX  => (a & b)
        );
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    auto inst = make_instance("u_sub", "sub");
    std::unordered_map<std::string, std::vector<HDL_net>> inst_ports;

    HDL_net data_net("data_bus");
    {
        HDL_range range;
        Expression_v2 accessor;
        accessor.set_lhs(std::make_shared<Numeric_token>("7"));
        range.accessor = accessor;
        Expression_v2 bound;
        bound.set_lhs(std::make_shared<Numeric_token>("0"));
        range.range = bound;
        range.type = HDL_range::decreasing_range;
        data_net.set_range(range);
    }
    inst_ports["data"] = {data_net};

    HDL_net flag_net("flag");
    Expression_v2 index;
    index.set_lhs(std::make_shared<Numeric_token>("0"));
    flag_net.add_index_expression(index);
    inst_ports["bit"] = {flag_net};

    inst_ports["mix"] = {HDL_net("a"), HDL_net("b")};
    inst->set_ports(inst_ports);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(inst);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, for_generate_loop) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    gen : for i in 0 to 3 generate
        u_sub : entity work.sub
            port map ( CLK => clk );
    end generate;
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    auto init = std::make_shared<HDL_parameter>("i");
    init->set_raw_value(std::make_shared<Numeric_token>("0"));

    auto end_cond = std::make_shared<Expression_v2>();
    end_cond->set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    end_cond->set_rhs(std::make_shared<Numeric_token>("3"));
    end_cond->set_operation(Expression_v2::less_equal);

    auto iter = std::make_shared<Expression_v2>();
    iter->set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    iter->set_rhs(std::make_shared<Numeric_token>("1"));
    iter->set_operation(Expression_v2::add);

    auto inst = make_instance("u_sub", "sub");
    std::unordered_map<std::string, std::vector<HDL_net>> inst_ports;
    inst_ports["clk"] = {HDL_net("clk")};
    inst->set_ports(inst_ports);

    auto loop = std::make_shared<hdl_loop_statement>();
    loop->set_init(init);
    loop->set_end_condition(end_cond);
    loop->set_iteration(iter);
    loop->add_body_stmt(inst);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(loop);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, if_generate_elsif_else) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    gen : if CONDITION generate
        u_a : entity work.a port map ( CLK => clk );
    elsif OTHER generate
        u_b : entity work.b port map ( CLK => clk );
    else generate
        u_c : entity work.c port map ( CLK => clk );
    end generate;
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    auto u_a = make_instance("u_a", "a");
    std::unordered_map<std::string, std::vector<HDL_net>> clk_ports;
    clk_ports["clk"] = {HDL_net("clk")};
    u_a->set_ports(clk_ports);
    auto u_b = make_instance("u_b", "b");
    u_b->set_ports(clk_ports);
    auto u_c = make_instance("u_c", "c");
    u_c->set_ports(clk_ports);

    auto conditional = std::make_shared<hdl_conditional_statement>();
    conditional->add_branch(std::make_shared<Identifier_token>(qualified_identifier("condition")));
    conditional->add_to_branch(u_a);
    conditional->add_branch(std::make_shared<Identifier_token>(qualified_identifier("other")));
    conditional->add_to_branch(u_b);
    conditional->add_to_else(u_c);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(conditional);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, if_generate_plain) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    gen : if ENABLE generate
        u_a : entity work.a port map ( CLK => clk );
    end generate;
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    auto u_a = make_instance("u_a", "a");
    std::unordered_map<std::string, std::vector<HDL_net>> clk_ports;
    clk_ports["clk"] = {HDL_net("clk")};
    u_a->set_ports(clk_ports);

    auto conditional = std::make_shared<hdl_conditional_statement>();
    conditional->add_branch(std::make_shared<Identifier_token>(qualified_identifier("enable")));
    conditional->add_to_branch(u_a);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(conditional);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, if_inside_for_generate) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    gen : for i in 0 to 1 generate
        gen2 : if ENABLE generate
            u_a : entity work.a port map ( CLK => clk );
        end generate;
    end generate;
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    auto u_a = make_instance("u_a", "a");
    std::unordered_map<std::string, std::vector<HDL_net>> clk_ports;
    clk_ports["clk"] = {HDL_net("clk")};
    u_a->set_ports(clk_ports);

    auto conditional = std::make_shared<hdl_conditional_statement>();
    conditional->add_branch(std::make_shared<Identifier_token>(qualified_identifier("enable")));
    conditional->add_to_branch(u_a);

    auto init = std::make_shared<HDL_parameter>("i");
    init->set_raw_value(std::make_shared<Numeric_token>("0"));
    auto end_cond = std::make_shared<Expression_v2>();
    end_cond->set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    end_cond->set_rhs(std::make_shared<Numeric_token>("1"));
    end_cond->set_operation(Expression_v2::less_equal);
    auto iter = std::make_shared<Expression_v2>();
    iter->set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    iter->set_rhs(std::make_shared<Numeric_token>("1"));
    iter->set_operation(Expression_v2::add);

    auto loop = std::make_shared<hdl_loop_statement>();
    loop->set_init(init);
    loop->set_end_condition(end_cond);
    loop->set_iteration(iter);
    loop->add_body_stmt(conditional);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(loop);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, case_generate) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    gen : case MODE generate
        when 0 =>
            u_a : entity work.a port map ( CLK => clk );
        when 1 =>
            u_b : entity work.b port map ( CLK => clk );
        when others =>
            u_c : entity work.c port map ( CLK => clk );
    end generate;
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    std::unordered_map<std::string, std::vector<HDL_net>> clk_ports;
    clk_ports["clk"] = {HDL_net("clk")};
    auto u_a = make_instance("u_a", "a");
    u_a->set_ports(clk_ports);
    auto u_b = make_instance("u_b", "b");
    u_b->set_ports(clk_ports);
    auto u_c = make_instance("u_c", "c");
    u_c->set_ports(clk_ports);

    auto mode = std::make_shared<Identifier_token>(qualified_identifier("mode"));
    auto cond0 = std::make_shared<Expression_v2>();
    cond0->set_lhs(mode);
    cond0->set_rhs(std::make_shared<Numeric_token>("0"));
    cond0->set_operation(Expression_v2::equal);
    auto cond1 = std::make_shared<Expression_v2>();
    cond1->set_lhs(mode);
    cond1->set_rhs(std::make_shared<Numeric_token>("1"));
    cond1->set_operation(Expression_v2::equal);

    auto conditional = std::make_shared<hdl_conditional_statement>();
    conditional->add_branch(cond0);
    conditional->add_to_branch(u_a);
    conditional->add_branch(cond1);
    conditional->add_to_branch(u_b);
    conditional->add_to_else(u_c);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(conditional);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, case_generate_multi_choice) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    gen : case MODE generate
        when 1 | 3 =>
            u_a : entity work.a port map ( CLK => clk );
        when others =>
            u_b : entity work.b port map ( CLK => clk );
    end generate;
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    std::unordered_map<std::string, std::vector<HDL_net>> clk_ports;
    clk_ports["clk"] = {HDL_net("clk")};
    auto u_a = make_instance("u_a", "a");
    u_a->set_ports(clk_ports);
    auto u_b = make_instance("u_b", "b");
    u_b->set_ports(clk_ports);

    auto mode = std::make_shared<Identifier_token>(qualified_identifier("mode"));
    auto eq1 = std::make_shared<Expression_v2>();
    eq1->set_lhs(mode);
    eq1->set_rhs(std::make_shared<Numeric_token>("1"));
    eq1->set_operation(Expression_v2::equal);
    auto eq3 = std::make_shared<Expression_v2>();
    eq3->set_lhs(mode);
    eq3->set_rhs(std::make_shared<Numeric_token>("3"));
    eq3->set_operation(Expression_v2::equal);
    auto cond = std::make_shared<Expression_v2>();
    cond->set_lhs(eq1);
    cond->set_rhs(eq3);
    cond->set_operation(Expression_v2::logical_or);

    auto conditional = std::make_shared<hdl_conditional_statement>();
    conditional->add_branch(cond);
    conditional->add_to_branch(u_a);
    conditional->add_to_else(u_b);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(conditional);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, case_generate_range_choice) {
    auto test_pattern = R"(
entity top is
end top;
architecture rtl of top is
begin
    gen : case MODE generate
        when 1 to 3 =>
            u_a : entity work.a port map ( CLK => clk );
        when others =>
            u_b : entity work.b port map ( CLK => clk );
    end generate;
end rtl;
)";
    auto res = parse_first_entity(test_pattern);

    std::unordered_map<std::string, std::vector<HDL_net>> clk_ports;
    clk_ports["clk"] = {HDL_net("clk")};
    auto u_a = make_instance("u_a", "a");
    u_a->set_ports(clk_ports);
    auto u_b = make_instance("u_b", "b");
    u_b->set_ports(clk_ports);

    auto mode = std::make_shared<Identifier_token>(qualified_identifier("mode"));
    auto lo = std::make_shared<Expression_v2>();
    lo->set_lhs(mode);
    lo->set_rhs(std::make_shared<Numeric_token>("1"));
    lo->set_operation(Expression_v2::greater_equal);
    auto hi = std::make_shared<Expression_v2>();
    hi->set_lhs(mode);
    hi->set_rhs(std::make_shared<Numeric_token>("3"));
    hi->set_operation(Expression_v2::less_equal);
    auto cond = std::make_shared<Expression_v2>();
    cond->set_lhs(lo);
    cond->set_rhs(hi);
    cond->set_operation(Expression_v2::logical_and);

    auto conditional = std::make_shared<hdl_conditional_statement>();
    conditional->add_branch(cond);
    conditional->add_to_branch(u_a);
    conditional->add_to_else(u_b);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(conditional);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, positional_port_map) {
    auto test_pattern = R"(
entity sub is
    port (
        clk  : in std_logic;
        rst  : in std_logic;
        dout : out std_logic_vector(7 downto 0)
    );
end sub;

entity top is
end top;
architecture rtl of top is
begin
    u_sub : entity work.sub
        port map (clk2, rst2, data_out2);
end rtl;
)";
    auto res = parse_entity(test_pattern, "top");

    auto u_sub = make_instance("u_sub", "sub");
    std::unordered_map<std::string, std::vector<HDL_net>> inst_ports;
    inst_ports["clk"] = {HDL_net("clk2")};
    inst_ports["rst"] = {HDL_net("rst2")};
    inst_ports["dout"] = {HDL_net("data_out2")};
    u_sub->set_ports(inst_ports);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(10);
    expected.add_statement(u_sub);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, positional_generic_and_port_map) {
    auto test_pattern = R"(
entity sub is
    generic (
        WIDTH : integer := 8;
        DEPTH : integer := 4
    );
    port (
        clk : in std_logic;
        rst : in std_logic
    );
end sub;

entity top is
end top;
architecture rtl of top is
begin
    u_sub : entity work.sub
        generic map (16, 2)
        port map (clk2, rst2);
end rtl;
)";
    auto res = parse_entity(test_pattern, "top");

    auto u_sub = make_instance("u_sub", "sub");
    u_sub->add_parameter(make_override_param("width", std::make_shared<Numeric_token>("16")));
    u_sub->add_parameter(make_override_param("depth", std::make_shared<Numeric_token>("2")));
    std::unordered_map<std::string, std::vector<HDL_net>> inst_ports;
    inst_ports["clk"] = {HDL_net("clk2")};
    inst_ports["rst"] = {HDL_net("rst2")};
    u_sub->set_ports(inst_ports);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(13);
    expected.add_statement(u_sub);

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, documentation_comments) {
    auto test_pattern = R"(
-- {
--     "name": "Decoder",
--     "type": "peripheral",
--     "registers":[
--         {
--             "name": "simple_register_r",
--             "offset": "0x0",
--             "description": "Single word read only register",
--             "direction": "R"
--         }
--     ]
-- }
entity Decoder is
end Decoder;
)";
    auto res = parse_first_entity(test_pattern);

    module_documentation expected_doc;
    expected_doc.set_name("Decoder");
    register_documentation reg("simple_register_r", 0, "Single word read only register", true, false);
    expected_doc.add_register(reg);

    ASSERT_EQ(res->get_documentation(), expected_doc);
}

TEST(vhdl_analyzer, package_declaration) {
    auto test_pattern = R"(
package params_pkg is
    constant WIDTH : integer := 8;
    constant DEPTH : integer := 4;
end params_pkg;
)";
    auto res = parse_entity(test_pattern, "params_pkg");

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("params_pkg");
    expected.set_type(package);
    expected.set_line_n(2);
    expected.add_statement(make_integer_param("width", std::make_shared<Numeric_token>("8")));
    expected.add_statement(make_integer_param("depth", std::make_shared<Numeric_token>("4")));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, package_constant_in_generic) {
    // A selected name `pkg.CONST` in a generic default becomes a
    // package-qualified reference (the suffix is no longer dropped).
    auto test_pattern = R"(
entity top is
    generic (
        N : integer := params_pkg.WIDTH
    );
end top;
)";
    auto res = parse_first_entity(test_pattern);

    hdl_resource_statement expected;
    expected.set_language(hdl_language::vhdl);
    expected.set_name("top");
    expected.set_type(module);
    expected.set_line_n(2);
    expected.add_statement(make_integer_param(
        "n", std::make_shared<Identifier_token>(qualified_identifier("params_pkg", "width"))));

    ASSERT_EQ(*res, expected);
}

TEST(vhdl_analyzer, package_constant_resolution) {
    // End-to-end: `params_pkg.WIDTH` in a generic default resolves through
    // `parameter_solver::retrieve_package_parameters` once the package resource
    // is in the store.
    auto test_pattern = R"(
package params_pkg is
    constant WIDTH : integer := 8;
end params_pkg;

entity top is
    generic (
        N : integer := params_pkg.WIDTH
    );
end top;
)";
    vhdl_analyzer analyzer("test.vhd");
    auto file = analyzer.analyze_content(test_pattern, "test.vhd");
    auto d_store = std::make_shared<data_store>(true, "/tmp/vhdl_pkg_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resources = file.get_content();
    auto &top = resources[1]->as<hdl_resource_statement>();
    auto pkg_params = parameter_solver::retrieve_package_parameters(top.get_parameter_statements(), d_store);
    auto it = pkg_params.find(qualified_identifier("params_pkg", "width"));
    ASSERT_NE(it, pkg_params.end());
    ASSERT_EQ(it->second.get_integer().get_value(), 8);
}

TEST(vhdl_analyzer, use_clause_import_stmt) {
    auto test_pattern = R"(
library work;
use work.params_pkg.all;
use work.params_pkg.CONST;

entity top is
end top;
architecture rtl of top is
begin
end rtl;
)";
    vhdl_analyzer analyzer("test.vhd");
    auto file = analyzer.analyze_content(test_pattern, "test.vhd");

    std::vector<std::shared_ptr<hdl_import_stmt>> imports;
    for (auto &c : file.get_content())
        if (c->is<hdl_import_stmt>()) imports.push_back(std::dynamic_pointer_cast<hdl_import_stmt>(c));
    ASSERT_EQ(imports.size(), 2u);

    auto wild = std::make_shared<hdl_import_stmt>();
    wild->set_library("work");
    wild->set_package("params_pkg");
    wild->set_wildcard(true);
    ASSERT_EQ(*imports[0], *wild);

    auto item = std::make_shared<hdl_import_stmt>();
    item->set_library("work");
    item->set_package("params_pkg");
    item->set_item("const");
    ASSERT_EQ(*imports[1], *item);
}
