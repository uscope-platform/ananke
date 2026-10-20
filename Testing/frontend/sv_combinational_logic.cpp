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
#include "data_model/HDL/parameters/components/token/LoopVar_token.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"

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

TEST(sv_combinational_logic, typedef_typed_variables_are_signals) {
    auto resource = analyze_module(R"(
        module m();
            typedef logic [3:0] nib_t;
            typedef enum logic [1:0] {A, B} e_t;
            nib_t n, k;
            e_t e;
            logic z;
        endmodule
    )");

    // A name followed by a declarator parses as a user-defined-nettype declaration; it is a variable of that type.
    ASSERT_TRUE(resource.get_typedefs().contains("nib_t"));
    ASSERT_TRUE(resource.get_typedefs().contains("e_t"));
    auto nib = make_type("logic", {range("3", "0")});
    EXPECT_TRUE(resource.get_typedefs().at("nib_t")->is_equal(*nib));

    auto check_res = make_check_res();
    check_res.add_typedef("nib_t", resource.get_typedefs().at("nib_t"));
    check_res.add_typedef("e_t", resource.get_typedefs().at("e_t"));
    check_res.add_statement(make_var("n", nib));
    check_res.add_statement(make_var("k", nib));
    check_res.add_statement(make_var("e", resource.get_typedefs().at("e_t")));
    check_res.add_statement(make_var("z", make_type("logic")));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, unknown_type_names_stay_external_references) {
    auto resource = analyze_module(R"(
        module m();
            imported_t x [1:0];
            imported_t y, w;
        endmodule
    )");

    auto external = std::make_shared<HDL_external_type>(qualified_identifier("imported_t"));
    auto external_array = std::make_shared<HDL_external_type>(qualified_identifier("imported_t"));
    external_array->set_unpacked_dimensions({dimension_t{num("1"), num("0"), false}});

    auto check_res = make_check_res();
    check_res.add_statement(make_var("x", external_array));
    check_res.add_statement(make_var("y", external));
    check_res.add_statement(make_var("w", external));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, declared_nettypes_are_not_variables) {
    auto resource = analyze_module(R"(
        module m();
            nettype real real_net;
            real_net rn;
            logic z;
        endmodule
    )");

    auto check_res = make_check_res();
    check_res.add_statement(make_var("z", make_type("logic")));

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

TEST(sv_combinational_logic, range_select_lvalues_are_partial_writes) {
    auto resource = analyze_module(R"(
        module m #(parameter W = 4)();
            assign f[W-1:0] = a;
            assign g[W] = a;
        endmodule
    )");

    // a range select with parametric bounds keeps no index, but must not read as a write of the whole variable
    auto range_driver = make_driver("f", id("a"));
    hdl_lvalue range_lv;
    range_lv.set_target("f");
    range_lv.set_member_selects({concat_member_select{-1, -1, true}});
    range_driver->set_lvalue(range_lv);

    auto bit_driver = make_driver("g", id("a"));
    hdl_lvalue bit_lv;
    bit_lv.set_target("g");
    bit_lv.set_index(id("W"));
    bit_driver->set_lvalue(bit_lv);

    auto check_res = make_check_res();
    check_res.add_statement(make_param("W", "4"));
    check_res.add_statement(range_driver);
    check_res.add_statement(bit_driver);

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

TEST(sv_combinational_logic, other_blocks_are_recorded_by_their_writes) {
    auto resource = analyze_module(R"(
        module m(input clk);
            reg a, b, c, d;
            always @(posedge clk) a <= b;
            always_ff @(posedge clk) b <= c;
            always_latch if (clk) c = a;
            initial a = 1'b0;
            always @(a or b) d = a & b;
        endmodule
    )");

    auto write = [](const std::string &name) {
        hdl_lvalue lv;
        lv.set_target(name);
        return lv;
    };

    auto flop_a = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always);
    flop_a->add_sensitivity(sensitivity_edge_t::posedge, id("clk"));
    flop_a->add_written(write("a"));

    auto flop_b = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always_ff);
    flop_b->add_sensitivity(sensitivity_edge_t::posedge, id("clk"));
    flop_b->add_written(write("b"));

    auto latch = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always_latch);
    latch->add_written(write("c"));

    auto init = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::initial);
    init->add_written(write("a"));

    auto level = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always);
    level->add_sensitivity(sensitivity_edge_t::level, id("a"));
    level->add_sensitivity(sensitivity_edge_t::level, id("b"));
    level->add_written(write("d"));

    std::unordered_map<std::string, HDL_port> check_ports;
    check_ports["clk"] = {input_port};

    auto check_res = make_check_res();
    check_res.set_ports(check_ports);
    check_res.add_statement(make_var("a", make_type("reg")));
    check_res.add_statement(make_var("b", make_type("reg")));
    check_res.add_statement(make_var("c", make_type("reg")));
    check_res.add_statement(make_var("d", make_type("reg")));
    check_res.add_statement(flop_a);
    check_res.add_statement(flop_b);
    check_res.add_statement(latch);
    check_res.add_statement(init);
    check_res.add_statement(level);

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, write_sets_keep_selects_partial_and_skip_locals) {
    auto resource = analyze_module(R"(
        module m();
            reg [7:0] v, w, x;
            always @(posedge clk) begin
                reg [7:0] w;
                v[3:0] <= 4'd1;
                w = 8'd2;
                {x, y} <= z;
            end
        endmodule
    )");

    hdl_lvalue part;
    part.set_target("v");
    part.set_member_selects({concat_member_select{-1, -1, true}});
    hdl_lvalue x_write;
    x_write.set_target("x");
    hdl_lvalue y_write;
    y_write.set_target("y");

    auto block = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always);
    block->add_sensitivity(sensitivity_edge_t::posedge, id("clk"));
    block->add_written(part);
    block->add_written(x_write);
    block->add_written(y_write);
    block->add_local_name("w");

    auto t = make_type("reg", {range("7", "0")});
    auto check_res = make_check_res();
    check_res.add_statement(make_var("v", t));
    check_res.add_statement(make_var("w", t));
    check_res.add_statement(make_var("x", t));
    check_res.add_statement(block);

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, generate_if_keeps_logic_in_its_branches) {
    auto resource = analyze_module(R"(
        module m #(parameter EN = 1)();
            wire top_level;
            if (EN) begin : g
                wire inner;
                assign inner = top_level;
            end else begin : h
                wire other;
                assign other = top_level;
            end
        endmodule
    )");

    auto check_cond = std::make_shared<hdl_conditional_statement>();
    check_cond->add_branch(id("EN"));
    check_cond->add_to_branch(make_net("inner", net_type_t::wire, make_type("implicit")));
    check_cond->add_to_branch(make_driver("inner", id("top_level")));
    check_cond->add_to_else(make_net("other", net_type_t::wire, make_type("implicit")));
    check_cond->add_to_else(make_driver("other", id("top_level")));

    auto check_res = make_check_res();
    check_res.add_statement(make_param("EN", "1"));
    check_res.add_statement(make_net("top_level", net_type_t::wire, make_type("implicit")));
    check_res.add_statement(check_cond);

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, generate_branches_without_a_block_keep_their_single_item) {
    auto resource = analyze_module(R"(
        module m #(parameter A = 1)();
            wire v;
            if (A) assign v = 1'b0; else assign v = 1'b1;
        endmodule
    )");

    auto check_cond = std::make_shared<hdl_conditional_statement>();
    check_cond->add_branch(id("A"));
    check_cond->add_to_branch(make_driver("v", num("1'b0")));
    check_cond->add_to_else(make_driver("v", num("1'b1")));

    auto check_res = make_check_res();
    check_res.add_statement(make_param("A", "1"));
    check_res.add_statement(make_net("v", net_type_t::wire, make_type("implicit")));
    check_res.add_statement(check_cond);

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, generate_for_keeps_logic_in_the_loop_body) {
    auto resource = analyze_module(R"(
        module m();
            wire t;
            for (genvar n = 0; n < 4; n = n + 1) begin : g
                wire x;
                assign x = t;
            end
        endmodule
    )");

    hdl_loop_statement check_loop;
    auto init = std::make_shared<HDL_parameter>();
    init->set_name("n");
    init->set_raw_value(num("0"));
    check_loop.set_init(init);
    check_loop.set_end_condition(binary(std::make_shared<LoopVar_token>("n"), Expression_v2::less, num("4")));
    check_loop.set_iteration(binary(std::make_shared<LoopVar_token>("n"), Expression_v2::add, num("1")));
    check_loop.add_body_stmt(make_net("x", net_type_t::wire, make_type("implicit")));
    check_loop.add_body_stmt(make_driver("x", id("t")));

    auto check_res = make_check_res();
    check_res.add_statement(make_net("t", net_type_t::wire, make_type("implicit")));
    check_res.add_statement(std::make_shared<hdl_loop_statement>(check_loop));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, procedural_if_stays_inside_a_block_in_a_generate_branch) {
    auto resource = analyze_module(R"(
        module m #(parameter EN = 1)();
            reg y, s;
            if (EN) begin : g
                always_comb begin
                    y = 1'b0;
                    if (s) y = 1'b1;
                end
            end
        endmodule
    )");

    auto procedural_if = std::make_shared<hdl_conditional_statement>();
    procedural_if->add_branch(id("s"));
    procedural_if->add_to_branch(make_assign("y", num("1'b1")));

    auto block = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always_comb);
    block->add_statement(make_assign("y", num("1'b0")));
    block->add_statement(procedural_if);

    auto generate_if = std::make_shared<hdl_conditional_statement>();
    generate_if->add_branch(id("EN"));
    generate_if->add_to_branch(block);

    auto check_res = make_check_res();
    check_res.add_statement(make_param("EN", "1"));
    check_res.add_statement(make_var("y", make_type("reg")));
    check_res.add_statement(make_var("s", make_type("reg")));
    check_res.add_statement(generate_if);

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, nested_generate_constructs_nest_their_logic) {
    auto resource = analyze_module(R"(
        module m #(parameter A = 1)();
            reg y;
            for (genvar n = 0; n < 2; n = n + 1) begin : g
                if (A) begin : h
                    reg q;
                    always_comb q = y;
                end
            end
        endmodule
    )");

    auto block = std::make_shared<hdl_procedural_block_statement>(procedural_block_kind_t::always_comb);
    block->add_statement(make_assign("q", id("y")));

    auto inner_if = std::make_shared<hdl_conditional_statement>();
    inner_if->add_branch(id("A"));
    inner_if->add_to_branch(make_var("q", make_type("reg")));
    inner_if->add_to_branch(block);

    hdl_loop_statement check_loop;
    auto init = std::make_shared<HDL_parameter>();
    init->set_name("n");
    init->set_raw_value(num("0"));
    check_loop.set_init(init);
    check_loop.set_end_condition(binary(std::make_shared<LoopVar_token>("n"), Expression_v2::less, num("2")));
    check_loop.set_iteration(binary(std::make_shared<LoopVar_token>("n"), Expression_v2::add, num("1")));
    check_loop.add_body_stmt(inner_if);

    auto check_res = make_check_res();
    check_res.add_statement(make_param("A", "1"));
    check_res.add_statement(make_var("y", make_type("reg")));
    check_res.add_statement(std::make_shared<hdl_loop_statement>(check_loop));

    ASSERT_EQ(resource, check_res);
}

TEST(sv_combinational_logic, case_generate_and_stray_blocks_stay_untracked) {
    auto resource = analyze_module(R"(
        module m #(parameter A = 1)();
            wire v;
            case (A)
                0: begin : c0 wire in_case; assign v = 1'b0; end
                default: assign v = 1'b1;
            endcase
            begin : bare
                wire in_bare;
            end
            if (A) begin : outer
                begin : nested
                    wire in_nested;
                end
            end
        endmodule
    )");

    auto check_res = make_check_res();
    check_res.add_statement(make_param("A", "1"));
    check_res.add_statement(make_net("v", net_type_t::wire, make_type("implicit")));

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
