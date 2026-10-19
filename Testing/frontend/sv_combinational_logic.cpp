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

#include <sstream>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>

#include "frontend/analysis/system_verilog/sv_analyzer.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"

namespace {

using stmt_list = std::vector<std::shared_ptr<hdl_statement_base>>;

hdl_resource_statement analyze_module(const std::string &src) {
    sv_analyzer analyzer;
    auto file = analyzer.analyze("", src);
    EXPECT_TRUE(file.has_value());
    return file.value().get_content()[0]->as<hdl_resource_statement>();
}

// Expected resource skeleton: every test module is `m`, declared on line 2
// of its source (raw strings open with a newline).
hdl_resource_statement make_check_res() {
    hdl_resource_statement check_res;
    check_res.set_language(hdl_language::system_verilog);
    check_res.set_name("m");
    check_res.set_line_n(2);
    return check_res;
}

std::shared_ptr<HDL_parameter> make_param(const std::string &name, const std::string &value) {
    auto p = std::make_shared<HDL_parameter>();
    p->set_name(name);
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>(value));
    return p;
}

std::shared_ptr<Expression_base> num(const std::string &s) { return std::make_shared<Numeric_token>(s); }
std::shared_ptr<Expression_base> id(const std::string &s) {
    return std::make_shared<Identifier_token>(qualified_identifier(s));
}
std::shared_ptr<Expression_base> binary(const std::shared_ptr<Expression_base> &l, Expression_v2::expression_operator op,
                                        const std::shared_ptr<Expression_base> &r) {
    auto e = std::make_shared<Expression_v2>();
    e->set_lhs(l);
    e->set_rhs(r);
    e->set_operation(op);
    return e;
}

using bounds_t = std::pair<std::shared_ptr<Expression_base>, std::shared_ptr<Expression_base>>;

std::shared_ptr<hdl_type> make_type(const std::string &base, const std::vector<bounds_t> &packed = {},
                                    const std::vector<bounds_t> &unpacked = {}) {
    // An empty base is the bare type an implicit packed-dimension-only net
    // starts from (`wire [7:0] w`); "implicit" is what a declaration with no
    // data type at all (`tri0 s`) gets.
    auto t = base.empty() ? std::make_shared<HDL_simple_type>()
                          : std::make_shared<HDL_simple_type>(Type_engine::create_primitive_type(base)->as<HDL_simple_type>());
    std::vector<dimension_t> p, u;
    for (auto &b : packed) p.push_back(dimension_t{b.first, b.second, true});
    for (auto &b : unpacked) u.push_back(dimension_t{b.first, b.second, false});
    t->set_packed_dimensions(p);
    t->set_unpacked_dimensions(u);
    return t;
}

std::shared_ptr<hdl_signal_declaration> make_net(const std::string &name, net_type_t net, const std::shared_ptr<hdl_type> &type) {
    auto d = std::make_shared<hdl_signal_declaration>(name);
    d->set_storage(signal_storage_t::net);
    d->set_net_type(net);
    d->set_type(type);
    return d;
}

std::shared_ptr<hdl_signal_declaration> make_var(const std::string &name, const std::shared_ptr<hdl_type> &type) {
    auto d = std::make_shared<hdl_signal_declaration>(name);
    d->set_storage(signal_storage_t::variable);
    d->set_type(type);
    return d;
}

std::shared_ptr<hdl_continuous_assignment> make_driver(const std::string &target, const std::shared_ptr<Expression_base> &value) {
    auto a = std::make_shared<hdl_continuous_assignment>();
    hdl_lvalue lv;
    lv.set_target(target);
    a->set_lvalue(lv);
    a->set_value(value);
    return a;
}

std::shared_ptr<hdl_assignment_statement> make_assign(const std::string &target, const std::shared_ptr<Expression_base> &value) {
    auto a = std::make_shared<hdl_assignment_statement>();
    a->set_target(target);
    a->set_value(value);
    return a;
}

bounds_t range(const std::string &hi, const std::string &lo) { return {num(hi), num(lo)}; }

}

TEST(sv_combinational_logic, nets_and_variables_are_told_apart) {
    auto resource = analyze_module(R"(
        module m();
            wire [7:0] bus;
            tri0 strobe;
            reg [3:0] state;
            logic flag;
            supply1 vdd;
        endmodule
    )");

    auto check_res = make_check_res();
    check_res.add_statement(make_net("bus", net_type_t::wire, make_type("", {range("7", "0")})));
    check_res.add_statement(make_net("strobe", net_type_t::tri0, make_type("implicit")));
    check_res.add_statement(make_var("state", make_type("reg", {range("3", "0")})));
    check_res.add_statement(make_var("flag", make_type("logic")));
    check_res.add_statement(make_net("vdd", net_type_t::supply1, make_type("implicit")));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, parametric_width_keeps_the_expression) {
    auto resource = analyze_module(R"(
        module m #(parameter W = 8)();
            wire [W-1:0] data;
        endmodule
    )");

    auto check_res = make_check_res();
    check_res.add_statement(make_param("W", "8"));
    check_res.add_statement(make_net("data", net_type_t::wire, make_type("", {{binary(id("W"), Expression_v2::subtract, num("1")), num("0")}})));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, multiple_declarators_become_separate_signals) {
    auto resource = analyze_module(R"(
        module m();
            wire [1:0] a, b, c;
            reg [7:0] mem [0:15];
            reg x [3:0], y;
        endmodule
    )");

    auto pair = make_type("", {range("1", "0")});
    auto check_res = make_check_res();
    check_res.add_statement(make_net("a", net_type_t::wire, pair));
    check_res.add_statement(make_net("b", net_type_t::wire, pair));
    check_res.add_statement(make_net("c", net_type_t::wire, pair));
    check_res.add_statement(make_var("mem", make_type("reg", {range("7", "0")}, {range("0", "15")})));
    // per-name unpacked shapes cannot be told apart once declarators share a
    // type, so they are dropped rather than misattributed
    check_res.add_statement(make_var("x", make_type("reg")));
    check_res.add_statement(make_var("y", make_type("reg")));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, initializer_does_not_corrupt_the_type) {
    auto resource = analyze_module(R"(
        module m();
            wire [3:0] w = 4'd5;
            logic [3:0] v = 4'd1 + 4'd2;
        endmodule
    )");

    auto check_res = make_check_res();
    check_res.add_statement(make_net("w", net_type_t::wire, make_type("", {range("3", "0")})));
    check_res.add_statement(make_var("v", make_type("logic", {range("3", "0")})));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, continuous_assign_one_statement_per_target) {
    auto resource = analyze_module(R"(
        module m();
            wire [3:0] a, b, c;
            assign a = b & c, c = b;
        endmodule
    )");

    auto t = make_type("", {range("3", "0")});
    auto check_res = make_check_res();
    check_res.add_statement(make_net("a", net_type_t::wire, t));
    check_res.add_statement(make_net("b", net_type_t::wire, t));
    check_res.add_statement(make_net("c", net_type_t::wire, t));
    check_res.add_statement(make_driver("a", binary(id("b"), Expression_v2::bitwise_and, id("c"))));
    check_res.add_statement(make_driver("c", id("b")));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, continuous_assign_lvalue_forms) {
    auto resource = analyze_module(R"(
        module m();
            assign bus[2] = hi;
            assign {hi, lo} = other;
        endmodule
    )");

    auto bit_driver = std::make_shared<hdl_continuous_assignment>();
    hdl_lvalue bit_lv;
    bit_lv.set_target("bus");
    bit_lv.set_index(num("2"));
    bit_driver->set_lvalue(bit_lv);
    bit_driver->set_value(id("hi"));

    auto concat_driver = std::make_shared<hdl_continuous_assignment>();
    hdl_lvalue concat_lv;
    concat_lv.set_targets({qualified_identifier("hi"), qualified_identifier("lo")});
    concat_driver->set_lvalue(concat_lv);
    concat_driver->set_value(id("other"));

    auto check_res = make_check_res();
    check_res.add_statement(bit_driver);
    check_res.add_statement(concat_driver);

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, always_comb_body_keeps_control_flow) {
    auto resource = analyze_module(R"(
        module m();
            always_comb begin
                y = 4'd0;
                if (sel) y = 4'd1;
                else y = 4'd2;
            end
        endmodule
    )");

    auto check_if = std::make_shared<hdl_conditional_statement>();
    check_if->add_branch(id("sel"));
    check_if->add_to_branch(make_assign("y", num("4'd1")));
    check_if->add_to_else(make_assign("y", num("4'd2")));

    auto block = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always_comb);
    block->add_statement(make_assign("y", num("4'd0")));
    block->add_statement(check_if);

    auto check_res = make_check_res();
    check_res.add_statement(block);

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, always_star_and_case) {
    auto resource = analyze_module(R"(
        module m();
            always @* begin
                case (s)
                    2'd0: y = 2'd3;
                    2'd1: y = 2'd2;
                    default: y = 2'd0;
                endcase
            end
        endmodule
    )");

    auto check_case = std::make_shared<hdl_conditional_statement>();
    check_case->add_branch(binary(id("s"), Expression_v2::case_equal, num("2'd0")));
    check_case->add_to_branch(make_assign("y", num("2'd3")));
    check_case->add_branch(binary(id("s"), Expression_v2::case_equal, num("2'd1")));
    check_case->add_to_branch(make_assign("y", num("2'd2")));
    check_case->add_branch(num("1"));
    check_case->add_to_branch(make_assign("y", num("2'd0")));

    auto block = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always_star);
    block->add_statement(check_case);

    auto check_res = make_check_res();
    check_res.add_statement(block);

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, only_combinational_blocks_are_recorded) {
    auto resource = analyze_module(R"(
        module m(input clk);
            reg a, b, c;
            always @(posedge clk) a <= b;
            always_ff @(posedge clk) b <= c;
            always_latch if (clk) c = a;
            initial a = 1'b0;
            always @(a or b) c = a & b;
        endmodule
    )");

    std::unordered_map<std::string, HDL_port> check_ports;
    check_ports["clk"] = {input_port};

    auto check_res = make_check_res();
    check_res.set_ports(check_ports);
    check_res.add_statement(make_var("a", make_type("reg")));
    check_res.add_statement(make_var("b", make_type("reg")));
    check_res.add_statement(make_var("c", make_type("reg")));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, generate_scopes_are_skipped) {
    auto resource = analyze_module(R"(
        module m #(parameter EN = 1)();
            wire top_level;
            if (EN) begin : g
                wire inner;
                assign inner = top_level;
                always_comb inner = top_level;
            end
        endmodule
    )");

    auto check_res = make_check_res();
    check_res.add_statement(make_param("EN", "1"));
    check_res.add_statement(make_net("top_level", net_type_t::wire, make_type("implicit")));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, unmodeled_lvalue_drops_the_construct_not_the_file) {
    auto resource = analyze_module(R"(
        module m();
            always_comb begin
                '{b} = a;
            end
            assign b = a;
        endmodule
    )");

    auto check_res = make_check_res();
    check_res.add_statement(make_driver("b", id("a")));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, logic_does_not_disturb_neighbouring_constructs) {
    auto resource = analyze_module(R"(
        module m #(parameter W = 4)(input [W-1:0] i, output [W-1:0] o);
            wire [W-1:0] mid;
            function integer twice(input integer x);
                twice = x * 2;
            endfunction
            assign mid = i;
            child u0 (.d(mid), .q(o));
        endmodule
    )");

    std::unordered_map<std::string, HDL_port> check_ports;
    check_ports["i"] = {input_port};
    check_ports["o"] = {output_port};

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("twice");
    check_f.add_argument("x");
    check_f.add_statement(make_assign("twice", binary(id("x"), Expression_v2::multiply, num("2"))));

    auto check_inst = std::make_shared<hdl_instance_statement>();
    check_inst->set_name("u0");
    check_inst->set_type("child");
    check_inst->set_dependency_class(module);
    check_inst->add_port_connection("d", {HDL_net("mid")});
    check_inst->add_port_connection("q", {HDL_net("o")});

    auto width = binary(id("W"), Expression_v2::subtract, num("1"));
    auto check_res = make_check_res();
    check_res.set_ports(check_ports);
    check_res.add_statement(make_param("W", "4"));
    check_res.add_statement(make_net("mid", net_type_t::wire, make_type("", {{width, num("0")}})));
    check_res.add_statement(std::make_shared<hdl_function_statement>(check_f));
    check_res.add_statement(make_driver("mid", id("i")));
    check_res.add_statement(check_inst);

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, tracked_logic_survives_the_cache) {
    auto resource = std::make_shared<hdl_resource_statement>(analyze_module(R"(
        module m();
            wire [3:0] a, b;
            reg [3:0] y;
            assign a = b;
            always_comb y = a;
        endmodule
    )"));
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive out(ss);
        out(resource);
    }
    std::shared_ptr<hdl_resource_statement> copy;
    cereal::BinaryInputArchive in(ss);
    in(copy);
    ASSERT_NE(copy, nullptr);

    auto check_res = make_check_res();
    auto t = make_type("", {range("3", "0")});
    auto block = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always_comb);
    block->add_statement(make_assign("y", id("a")));
    check_res.add_statement(make_net("a", net_type_t::wire, t));
    check_res.add_statement(make_net("b", net_type_t::wire, t));
    check_res.add_statement(make_var("y", make_type("reg", {range("3", "0")})));
    check_res.add_statement(make_driver("a", id("b")));
    check_res.add_statement(block);

    ASSERT_EQ(*copy, check_res);
}
