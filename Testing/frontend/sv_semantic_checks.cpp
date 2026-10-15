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

#include "frontend/analysis/system_verilog/sv_analyzer.hpp"
#include "frontend/analysis/system_verilog/sv_semantic_checker.hpp"
#include "frontend/analysis/system_verilog/sv_parsing_helpers.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"
#include "data_model/parse_options.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/parameters/common/dimension.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"

namespace {

std::shared_ptr<HDL_simple_type> make_logic_base(
    const std::string &hi, const std::string &lo, const std::string &type_name = "logic") {
    auto base = std::make_shared<HDL_simple_type>();
    base->set_type_name(type_name);
    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>(hi);
    d.second_bound = std::make_shared<Numeric_token>(lo);
    d.packed = true;
    base->set_packed_dimensions({d});
    return base;
}

} // namespace

TEST(sv_semantic_checker, builtin_class_methods_rejected) {
    sv_semantic_checker checker;
    for (const auto &name : {"randomize", "rand_mode", "constraint_mode"}) {
        EXPECT_TRUE(checker.check_class_method("test.sv", 3, name)) << name;
    }
    const auto &violations = checker.get_violations();
    ASSERT_EQ(violations.size(), 3);
    EXPECT_TRUE(checker.has_violations());
    for (const auto &v : violations) {
        EXPECT_EQ(v.code, semantic_error_code::builtin_method_override);
    }
    EXPECT_NE(violations[0].message.find("randomize"), std::string::npos);
    EXPECT_EQ(checker.last_violation().message, violations.back().message);
}

TEST(sv_semantic_checker, exit_code_owned_by_checker) {
    EXPECT_EQ(
        sv_semantic_checker::exit_code_for(semantic_error_code::builtin_method_override),
        56);
}

TEST(sv_semantic_checker, legal_method_names_accepted) {
    sv_semantic_checker checker;
    for (const auto &name :
         {"new", "pre_randomize", "post_randomize", "my_func", "randomize_if"}) {
        EXPECT_FALSE(checker.check_class_method("test.sv", 3, name)) << name;
    }
    EXPECT_FALSE(checker.has_violations());
    EXPECT_TRUE(checker.get_violations().empty());
}

TEST(sv_semantic_checker, disabled_checks_accept_everything) {
    sv_semantic_checker checker;
    parse_options opts;
    opts.disable_semantic_checks = true;
    checker.set_options(opts);
    for (const auto &name : {"randomize", "rand_mode", "constraint_mode"}) {
        EXPECT_FALSE(checker.check_class_method("test.sv", 3, name)) << name;
    }
    EXPECT_FALSE(checker.has_violations());
}

TEST(sv_semantic_checks, class_override_fails_analysis) {
    auto test_pattern = R"(
        class a1;
            rand int x;
            function int rand_mode();
                return 1;
            endfunction
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_FALSE(result.has_value());
    EXPECT_TRUE(analyzer.has_error());
    const auto &violations = analyzer.get_semantic_violations();
    ASSERT_EQ(violations.size(), 1);
    EXPECT_EQ(violations[0].code, semantic_error_code::builtin_method_override);
    EXPECT_NE(violations[0].message.find("rand_mode"), std::string::npos);
    EXPECT_EQ(analyzer.get_error(), violations[0].message);
}

TEST(sv_semantic_checks, class_override_passes_when_disabled) {
    auto test_pattern = R"(
        class a1;
            rand int x;
            function int rand_mode();
                return 1;
            endfunction
        endclass
    )";

    sv_analyzer analyzer;
    parse_options opts;
    opts.disable_semantic_checks = true;
    analyzer.set_options(opts);
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_TRUE(result.has_value());
}

TEST(sv_semantic_checks, randomization_hooks_stay_legal) {
    auto test_pattern = R"(
        class c;
            rand int x;
            function void pre_randomize();
            endfunction
            function void post_randomize();
            endfunction
        endclass
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, package_function_with_builtin_name_stays_legal) {
    auto test_pattern = R"(
        package p;
            function int randomize();
                return 1;
            endfunction
        endpackage
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checker, sized_literal_discriminator) {
    // Only the N'... form counts as a sized literal constant: unsized
    // decimal and unbased literals must never trip the enum size rule.
    auto sized = std::dynamic_pointer_cast<Numeric_token>(sv_parsing_helpers::make_value("4'h2"));
    ASSERT_TRUE(sized);
    EXPECT_TRUE(sized->is_sized_explicit());
    EXPECT_EQ(sized->get_size(), 4);

    auto unsized = std::dynamic_pointer_cast<Numeric_token>(sv_parsing_helpers::make_value("10"));
    ASSERT_TRUE(unsized);
    EXPECT_FALSE(unsized->is_sized_explicit());

    auto unbased = std::dynamic_pointer_cast<Numeric_token>(sv_parsing_helpers::make_value("'h2"));
    ASSERT_TRUE(unbased);
    EXPECT_FALSE(unbased->is_sized_explicit());
}

TEST(sv_semantic_checker, enum_value_size_mismatch_rejected) {
    sv_semantic_checker checker;
    auto base = make_logic_base("2", "0");
    EXPECT_TRUE(checker.check_enum_value("test.sv", 17, "Global", 4, true, false, base));
    const auto &violations = checker.get_violations();
    ASSERT_EQ(violations.size(), 1);
    EXPECT_EQ(violations[0].code, semantic_error_code::enum_value_size_mismatch);
    EXPECT_NE(violations[0].message.find("Global"), std::string::npos);
    EXPECT_NE(violations[0].message.find("4"), std::string::npos);
    EXPECT_NE(violations[0].message.find("3"), std::string::npos);
    EXPECT_EQ(
        sv_semantic_checker::exit_code_for(violations[0].code), 56);
}

TEST(sv_semantic_checker, enum_value_matching_and_unsized_accepted) {
    sv_semantic_checker checker;
    auto base = make_logic_base("2", "0");
    EXPECT_FALSE(checker.check_enum_value("test.sv", 3, "A", 3, true, false, base));
    EXPECT_FALSE(checker.check_enum_value("test.sv", 3, "B", 32, false, false, base));
    EXPECT_FALSE(checker.has_violations());
}

TEST(sv_semantic_checker, enum_default_base_is_int) {
    sv_semantic_checker checker;
    EXPECT_FALSE(checker.check_enum_value("test.sv", 3, "A", 32, true, false, nullptr));
    EXPECT_TRUE(checker.check_enum_value("test.sv", 3, "B", 8, true, false, nullptr));
}

TEST(sv_semantic_checker, enum_value_check_disabled) {
    sv_semantic_checker checker;
    parse_options opts;
    opts.disable_semantic_checks = true;
    checker.set_options(opts);
    EXPECT_FALSE(
        checker.check_enum_value("test.sv", 3, "Global", 4, true, false, make_logic_base("2", "0")));
    EXPECT_FALSE(checker.has_violations());
}

TEST(sv_semantic_checks, enum_sized_literal_mismatch_fails_analysis) {
    auto test_pattern = R"(
        module top();
            enum logic [2:0] {
              Global = 4'h2,
              Local = 4'h3
            } myenum;
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_FALSE(result.has_value());
    const auto &violations = analyzer.get_semantic_violations();
    ASSERT_FALSE(violations.empty());
    EXPECT_EQ(violations[0].code, semantic_error_code::enum_value_size_mismatch);
    EXPECT_EQ(analyzer.get_error(), violations[0].message);
}

TEST(sv_semantic_checks, enum_sized_literal_match_and_unsized_stay_legal) {
    auto test_pattern = R"(
        module top();
            enum logic [2:0] {A = 3'h2, B = 3'h3, C = 4, D} myenum;
        endmodule
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, enum_replication_value_stays_legal) {
    auto test_pattern = R"(
        module top();
            enum integer {a=0, b={32{1'bx}}, c=1} val;
        endmodule
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, enum_parameterized_base_skipped_safely) {
    auto test_pattern = R"(
        module top #(parameter int N = 4)();
            enum logic [N-1:0] {A = 4'h2} myenum;
        endmodule
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, enum_typedef_base_resolves_width) {
    // uvm_resource.svh shape: an enum over a typedef base with matching
    // sized literals must resolve the base (bit[1:0]/2) instead of
    // defaulting to int/32 and misfiring.
    auto test_pattern = R"(
        module top();
            typedef bit[1:0] override_t;
            typedef enum override_t { TYPE_OVERRIDE = 2'b01,
                                      NAME_OVERRIDE = 2'b10 } override_e;
        endmodule
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, enum_unresolvable_typedef_base_skipped_safely) {
    // A base the single-file parse cannot resolve (another package, or a
    // qualified pkg::t) gets the benefit of the doubt rather than being
    // checked against the default int base.
    auto test_pattern = R"(
        package other_pkg;
            typedef bit[1:0] some_t;
        endpackage
        module top();
            typedef enum some_pkg_t { A = 2'b01, B = 2'b10 } my_e;
            typedef enum other_pkg::some_t { C = 2'b01, D = 2'b10 } my_e2;
        endmodule
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, enum_in_interface_body_uses_declared_width) {
    // soc_ifc_cov_if.sv shape: a direction-less enum declaration in an
    // interface body parses as ansi_port_declaration, not
    // data_declaration. Its base must still resolve (bit[3:0]/4) instead
    // of defaulting to int/32.
    auto test_pattern = R"(
        interface cov_if;
            enum bit [3:0] {IDLE = '0, AHB_RD = 4'h8, AHB_WR = 4'h4,
                            AXI_RD = 4'h2, AXI_WR = 4'h1} bus_event_e;
        endinterface
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, enum_in_interface_body_mismatch_rejected) {
    // The interface path must still catch genuine mismatches, reported
    // against the real base width.
    auto test_pattern = R"(
        interface bad_if;
            enum bit [3:0] {A = 2'b01, B = 4'h2} my_e;
        endinterface
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_FALSE(result.has_value());
    const auto &violations = analyzer.get_semantic_violations();
    ASSERT_FALSE(violations.empty());
    EXPECT_EQ(violations[0].code, semantic_error_code::enum_value_size_mismatch);
    EXPECT_NE(violations[0].message.find("4"), std::string::npos);
}

TEST(sv_semantic_checks, enum_header_port_uses_declared_width) {
    // Enum-typed ANSI header ports take the same path: matching literals
    // stay legal.
    auto test_pattern = R"(
        module m(input enum bit [1:0] {A = 2'b00, B = 2'b01} p, input logic clk);
        endmodule
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, enum_mismatch_passes_when_disabled) {
    auto test_pattern = R"(
        module top();
            enum logic [2:0] {Global = 4'h2} myenum;
        endmodule
    )";

    sv_analyzer analyzer;
    parse_options opts;
    opts.disable_semantic_checks = true;
    analyzer.set_options(opts);
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

namespace {

std::shared_ptr<HDL_simple_type> make_bit_base() {
    return std::dynamic_pointer_cast<HDL_simple_type>(
        Type_engine::create_primitive_type("bit"));
}

} // namespace

TEST(sv_semantic_checker, enum_unknown_in_two_state_rejected) {
    sv_semantic_checker checker;
    EXPECT_TRUE(
        checker.check_enum_value("test.sv", 12, "b", 2, true, true, make_logic_base("1", "0", "bit")));
    const auto &violations = checker.get_violations();
    ASSERT_EQ(violations.size(), 1);
    EXPECT_EQ(violations[0].code, semantic_error_code::enum_unknown_in_two_state);
    EXPECT_NE(violations[0].message.find("b"), std::string::npos);
}

TEST(sv_semantic_checker, enum_unknown_in_four_state_or_default_accepted) {
    sv_semantic_checker checker;
    // 4-state base: x/z member values are legal.
    EXPECT_FALSE(
        checker.check_enum_value("test.sv", 12, "b", 2, true, true, make_logic_base("1", "0")));
    // Unsized literals are never examined, even with x digits.
    EXPECT_FALSE(
        checker.check_enum_value("test.sv", 12, "b", 0, false, true, make_bit_base()));
    EXPECT_FALSE(checker.has_violations());
}

TEST(sv_semantic_checker, enum_unknown_with_default_base_rejected) {
    // No explicit base means the default int base, which is 2-state.
    sv_semantic_checker checker;
    EXPECT_TRUE(checker.check_enum_value("test.sv", 12, "b", 1, true, true, nullptr));
}

TEST(sv_semantic_checks, enum_unknown_in_two_state_fails_analysis) {
    auto test_pattern = R"(
        module top();
            enum bit [1:0] {a=0, b=2'bxx, c=1} val;
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_FALSE(result.has_value());
    const auto &violations = analyzer.get_semantic_violations();
    ASSERT_FALSE(violations.empty());
    EXPECT_EQ(violations[0].code, semantic_error_code::enum_unknown_in_two_state);
    EXPECT_EQ(analyzer.get_error(), violations[0].message);
}

TEST(sv_semantic_checks, enum_unknown_in_four_state_stays_legal) {
    auto test_pattern = R"(
        module top();
            enum logic [1:0] {a=0, b=2'bxx, c=1} val;
        endmodule
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, pure_virtual_in_non_virtual_class_fails_analysis) {
    auto test_pattern = R"(
        class myclass;
            pure virtual task pure_task1;
            pure virtual task pure_task2(int arg);
        endclass
        module test;
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_FALSE(result.has_value());
    EXPECT_TRUE(analyzer.has_error());
    const auto &violations = analyzer.get_semantic_violations();
    ASSERT_GE(violations.size(), 1);
    EXPECT_EQ(violations[0].code, semantic_error_code::pure_virtual_in_non_virtual_class);
    EXPECT_NE(violations[0].message.find("myclass"), std::string::npos);
}

TEST(sv_semantic_checks, pure_virtual_in_virtual_class_stays_legal) {
    auto test_pattern = R"(
        virtual class myclass;
            pure virtual task pure_task1;
            pure virtual function int f(int arg);
        endclass
        module test;
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_TRUE(result.has_value());
    EXPECT_FALSE(analyzer.has_error());
}

TEST(sv_semantic_checker, pure_virtual_check_unit) {
    sv_semantic_checker checker;
    EXPECT_FALSE(checker.check_pure_virtual_method("test.sv", 3, "c", true));
    EXPECT_TRUE(checker.check_pure_virtual_method("test.sv", 3, "c", false));
    EXPECT_EQ(checker.last_violation().code, semantic_error_code::pure_virtual_in_non_virtual_class);
}

TEST(sv_semantic_checks, enum_literal_assignment_fails_analysis) {
    auto test_pattern = R"(
        module top();
            typedef enum {a, b, c, d} e;

            initial begin
                e val;
                val = 1;
            end
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_FALSE(result.has_value());
    const auto &violations = analyzer.get_semantic_violations();
    ASSERT_FALSE(violations.empty());
    EXPECT_EQ(violations[0].code, semantic_error_code::enum_assignment_type_mismatch);
}

TEST(sv_semantic_checks, enum_member_and_cast_assignment_stay_legal) {
    auto test_pattern = R"(
        module top();
            typedef enum {a, b, c, d} e;

            initial begin
                e val;
                val = b;
                val = e'(2);
            end
        endmodule
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, timescale_precision_larger_than_unit_fails_analysis) {
    auto test_pattern = R"(
`timescale 1ps / 1ns
        module top();
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_FALSE(result.has_value());
    const auto &violations = analyzer.get_semantic_violations();
    ASSERT_FALSE(violations.empty());
    EXPECT_EQ(violations[0].code, semantic_error_code::timescale_precision_exceeds_unit);
    EXPECT_EQ(analyzer.get_error(), violations[0].message);
}

TEST(sv_semantic_checks, timescale_precision_not_larger_than_unit_stays_legal) {
    for (const char *ts : {"`timescale 1ns / 1ps", "`timescale 10ns / 1ns", "`timescale 100 fs / 100 fs",
                           "`timescale 1 s / 1 fs"}) {
        sv_analyzer analyzer;
        auto src = std::string(ts) + "\nmodule top(); endmodule\n";
        EXPECT_TRUE(analyzer.analyze("test.sv", src).has_value()) << ts;
    }
}

TEST(sv_semantic_checks, timescale_precision_mismatch_passes_when_disabled) {
    parse_options opts;
    opts.disable_semantic_checks = true;
    sv_analyzer analyzer;
    analyzer.set_options(opts);
    EXPECT_TRUE(analyzer.analyze("test.sv", "`timescale 1ps / 1ns\nmodule top(); endmodule\n").has_value());
}

TEST(sv_semantic_checker, timescale_check_unit) {
    sv_semantic_checker checker;
    EXPECT_FALSE(checker.check_timescale("t.sv", 1, 1000, 1000, "`timescale 1ns / 1ns"));
    EXPECT_FALSE(checker.check_timescale("t.sv", 1, 1000000, 1000, "`timescale 1ns / 1ps"));
    EXPECT_TRUE(checker.check_timescale("t.sv", 1, 1000, 1000000, "`timescale 1ps / 1ns"));
    EXPECT_EQ(checker.last_violation().code, semantic_error_code::timescale_precision_exceeds_unit);
}

namespace {

semantic_error_code first_code(const std::string &src) {
    sv_analyzer analyzer;
    auto res = analyzer.analyze("test.sv", src);
    EXPECT_FALSE(res.has_value());
    EXPECT_FALSE(analyzer.get_semantic_violations().empty());
    return analyzer.get_semantic_violations().front().code;
}

} // namespace

TEST(sv_semantic_checks, typedef_of_undefined_type_rejected) {
    EXPECT_EQ(
        first_code("typedef some_other_type myalias;\nmodule test; endmodule\n"),
        semantic_error_code::undefined_identifier);
}

TEST(sv_semantic_checks, packed_struct_undefined_bound_rejected) {
    EXPECT_EQ(
        first_code("typedef struct packed { reg [A-1:0] addr; reg parity; } req_t;\n"
                   "module test; endmodule\n"),
        semantic_error_code::undefined_identifier);
}

TEST(sv_semantic_checks, unresolved_forward_typedef_rejected) {
    EXPECT_EQ(
        first_code("typedef missing_forward_typedef;\nmodule test; endmodule\n"),
        semantic_error_code::unresolved_forward_typedef);
}

TEST(sv_semantic_checks, unit_scope_typedefs_with_declared_names_stay_legal) {
    for (const char *src : {
             "typedef int base_t;\ntypedef base_t myalias;\nmodule test; endmodule\n",
             "parameter A = 4;\ntypedef struct packed { reg [A-1:0] addr; } req_t;\n"
             "module test; endmodule\n",
             "typedef fwd_t;\ntypedef struct packed { logic a; } fwd_t;\nmodule test; endmodule\n",
             "import some_pkg::*;\ntypedef from_pkg_t alias_t;\nmodule test; endmodule\n",
             "typedef some_pkg::item_t alias_t;\nmodule test; endmodule\n",
         }) {
        sv_analyzer analyzer;
        EXPECT_TRUE(analyzer.analyze("test.sv", src).has_value()) << src;
    }
}

TEST(sv_semantic_checks, unit_scope_undefined_name_passes_when_disabled) {
    parse_options opts;
    opts.disable_semantic_checks = true;
    sv_analyzer analyzer;
    analyzer.set_options(opts);
    EXPECT_TRUE(analyzer.analyze("test.sv", "typedef nope_t a;\nmodule test; endmodule\n").has_value());
}

TEST(sv_semantic_checks, typedef_inside_class_sees_class_parameters) {
    // uvm_push_sequencer shape: REQ/RSP are class parameters, not unit names.
    for (const char *src : {
             "class seq #(type REQ=int, RSP=REQ) extends base #(REQ, RSP);\n"
             "  typedef seq #(REQ, RSP) this_type;\nendclass\n",
             "function void f(); typedef local_t alias_t; endfunction\n",
         }) {
        sv_analyzer analyzer;
        EXPECT_TRUE(analyzer.analyze("test.sv", src).has_value()) << src;
    }
}

TEST(sv_semantic_checks, deferred_unit_names_are_exported_not_errors) {
    // Repository mode: a name the file cannot resolve is judged repo-wide.
    parse_options opts;
    opts.defer_unit_name_errors = true;
    sv_analyzer analyzer;
    analyzer.set_options(opts);
    auto res = analyzer.analyze(
        "test.sv", "typedef uvm_action act_t;\ntypedef fwd_t;\nmodule test; endmodule\n");
    ASSERT_TRUE(res.has_value());
    const auto &pending = res->get_unresolved_names();
    ASSERT_EQ(pending.size(), 2);
    EXPECT_EQ(pending[0].name, "uvm_action");
    EXPECT_FALSE(pending[0].forward);
    EXPECT_EQ(pending[1].name, "fwd_t");
    EXPECT_TRUE(pending[1].forward);
    EXPECT_TRUE(res->get_declared_names().contains("act_t"));
}

TEST(sv_semantic_checks, deferred_unit_names_still_error_in_single_file_mode) {
    sv_analyzer analyzer;
    EXPECT_FALSE(analyzer.analyze("test.sv", "typedef uvm_action act_t;\nmodule test; endmodule\n").has_value());
}

namespace {
bool declarations_rejected(const std::string &source) {
    sv_analyzer analyzer;
    return !analyzer.analyze("test.sv", source).has_value();
}
} // namespace

TEST(sv_semantic_checks, nonansi_port_legal_completions_accepted) {
    // ivtest module_nonansi_*1/2 and vec1/2: a bare port plus one matching
    // net/variable declaration, in either order.
    EXPECT_FALSE(declarations_rejected("module t(x); input x; wire x; endmodule"));
    EXPECT_FALSE(declarations_rejected("module t(x); output x; integer x; endmodule"));
    EXPECT_FALSE(declarations_rejected("module t(x); real x; output x; endmodule"));
    EXPECT_FALSE(declarations_rejected("module t(x); output [7:0] x; reg [7:0] x; endmodule"));
    EXPECT_FALSE(declarations_rejected("module t(x); reg [7:0] x; output [7:0] x; endmodule"));
    EXPECT_FALSE(declarations_rejected("module t(x); output [W-1:0] x; reg [W-1:0] x; endmodule"));
    EXPECT_FALSE(declarations_rejected("module t; task t1; input x; string x; endtask endmodule"));
}

TEST(sv_semantic_checks, nonansi_port_declared_twice_rejected) {
    EXPECT_TRUE(declarations_rejected("module t(x); input x; input x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(x); output integer x; output integer x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; task t1; input x; input x; endtask endmodule"));
}

TEST(sv_semantic_checks, net_or_variable_declared_twice_rejected) {
    EXPECT_TRUE(declarations_rejected("module t(x); input x; wire x; wire x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(x); input x; wire x; reg x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; wire [7:0] v, v; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg [7:0] v, v; endmodule"));
}

TEST(sv_semantic_checks, typed_port_redeclared_rejected) {
    EXPECT_TRUE(declarations_rejected("module t(x); output real x; real x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(x); output real x; integer x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(x); wire integer x; input integer x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; task t1; real x; output integer x; endtask endmodule"));
}

TEST(sv_semantic_checks, port_range_conflicts_with_type_rejected) {
    EXPECT_TRUE(declarations_rejected("module t(x); output [31:0] x; integer x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(x); output [15:0] x; shortint x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(x); output [3:0] x; reg [7:0] x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(x); output [7:0] x; reg x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(x); output x; reg [7:0] x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; input [1:0] x; wire [3:0] x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; task t1; input [7:0] x; reg x; endtask endmodule"));
}

TEST(sv_semantic_checks, same_name_in_separate_scopes_accepted) {
    EXPECT_FALSE(declarations_rejected(
        "module t; reg x; initial begin integer i; end initial begin integer i; end "
        "task a; input v; reg w; endtask task b; input v; reg w; endtask endmodule"));
}

TEST(sv_semantic_checks, tf_port_takes_range_from_later_declaration) {
    // ivtest br_gh788 / pr1841300: legal in tasks and functions, unlike modules.
    EXPECT_FALSE(declarations_rejected("module t; task p; input n; reg [7:0] n; endtask endmodule"));
    EXPECT_FALSE(declarations_rejected(
        "module t; function [4:0] f; input l; reg [4:0] l; f = ~l; endfunction endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; function [4:0] f; input [7:0] l; reg l; f = 0; endfunction endmodule"));
}

TEST(sv_semantic_checks, matching_end_labels_accepted) {
    EXPECT_FALSE(declarations_rejected(
        "module top; initial begin : a end : a initial fork : b join : b "
        "task t; endtask : t function f; input x; endfunction : f endmodule : top "
        "package pkg; endpackage : pkg program p; endprogram : p interface i; endinterface : i "
        "primitive buf1 (out, in); output out; input in; table 0 : 0; endtable endprimitive : buf1"));
    EXPECT_FALSE(declarations_rejected("module t; initial begin end endmodule"));
}

TEST(sv_semantic_checks, mismatched_end_labels_rejected) {
    EXPECT_TRUE(declarations_rejected("module top; initial begin : a end : b endmodule"));
    EXPECT_TRUE(declarations_rejected("module top; initial fork : a join : b endmodule"));
    EXPECT_TRUE(declarations_rejected("module top; initial fork : a join_any : b endmodule"));
    EXPECT_TRUE(declarations_rejected("module top; endmodule : other"));
    EXPECT_TRUE(declarations_rejected("module top; task t; endtask : u endmodule"));
    EXPECT_TRUE(declarations_rejected("module top; function f; input x; endfunction : g endmodule"));
    EXPECT_TRUE(declarations_rejected("package p; endpackage : q"));
    EXPECT_TRUE(declarations_rejected("program p; class c; endclass : d endprogram"));
}

TEST(sv_semantic_checks, end_label_on_unnamed_block_rejected) {
    EXPECT_TRUE(declarations_rejected("module top; initial begin end : label endmodule"));
    EXPECT_TRUE(declarations_rejected("module top; initial fork join : label endmodule"));
    EXPECT_TRUE(declarations_rejected("module top; if (1) begin end : label endmodule"));
}

TEST(sv_semantic_checks, legal_type_shapes_accepted) {
    EXPECT_FALSE(declarations_rejected(
        "module t; typedef logic [3:0] nib; typedef int word; typedef enum nib {A, B} e1; "
        "enum bit [1:0] {C = 0, D, E, F} e2; enum {G = 5, H = 2} e3; "
        "typedef struct packed { logic [3:0] a; nib b; } s1; union packed { logic [7:0] x; nib [1:0] y; } u1; "
        "typedef struct { int q[$]; } node; wire logic [3:0] w; wire [7:0] v; endmodule"));
}

TEST(sv_semantic_checks, enum_base_type_rules) {
    EXPECT_TRUE(declarations_rejected("module t; typedef logic T[1:0]; enum T {A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef logic T[]; enum T {A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef logic T[$]; enum T {A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef enum {X} T; enum T {A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef int T; enum T [1:0] {A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef bit [1:0][1:0] T; enum T {A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef real T; enum T {A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef string T; enum T {A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef struct packed { int x; } T; enum T {A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef T; typedef enum T {A, B} T; endmodule"));
}

TEST(sv_semantic_checks, enum_member_value_rules) {
    EXPECT_TRUE(declarations_rejected("module t; enum bit [4:0] {some[4] = 100} v; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; enum bit [1:0] {nega = -1, b, c} v; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; enum bit [1:0] {a = 3, b, c} v; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; enum {red = 1, green, blue = 2} light; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; enum logic {A, B = A} e; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; enum {VAL4, XX4 = $time} en; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; enum {VAL5, XX5 = 'bx} en; endmodule"));
    EXPECT_FALSE(declarations_rejected("module t; enum logic [1:0] {A = 'bx, B = 2'b11} en; endmodule"));
}

TEST(sv_semantic_checks, packed_aggregate_member_rules) {
    EXPECT_TRUE(declarations_rejected("module t; struct packed { int x[]; } s; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; struct packed { int x[$]; } s; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; struct packed { int x[2]; } s; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; struct packed { integer x = 10; } s; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; struct packed { real r; } s; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; union packed { int x[2]; } u; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef real R; struct packed { R r; } s; endmodule"));
}

TEST(sv_semantic_checks, circular_typedefs_rejected) {
    EXPECT_TRUE(declarations_rejected("module t; typedef T1; typedef T1 T2; typedef T2 T1; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef T; typedef struct packed { T x; } T; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef T; typedef union packed { T x; } T; endmodule"));
    EXPECT_FALSE(declarations_rejected("module t; typedef T; typedef struct { T q[$]; } T; endmodule"));
}

TEST(sv_semantic_checks, nets_of_non_net_types_rejected) {
    EXPECT_TRUE(declarations_rejected("module t; wire string x; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; wire x[$]; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; class C; endclass wire C x; endmodule"));
    EXPECT_FALSE(declarations_rejected("module t; wire x[3:0]; endmodule"));
}

TEST(sv_semantic_checks, enum_signed_sized_literal_wraps_at_its_width) {
    // ivtest enum_test4: -4'sd1 is the 4-bit pattern 4'b1111.
    EXPECT_FALSE(declarations_rejected("module t; enum bit [3:0] {first, second, last = -4'sd1} v; endmodule"));
}

TEST(sv_semantic_checks, assignment_pattern_arity_rules) {
    EXPECT_FALSE(declarations_rejected(
        "module t; bit [2:0][3:0] x = '{1, 2, 3}; struct packed { int a; shortint b; } s = '{1, 2}; "
        "int arr[3] = '{1, 2, 3}; bit [3:0][1:0] d = '{default: 0}; bit [3:0][1:0] r = '{4{2'b01}}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; bit [2:0][3:0] x = '{1, 2}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; bit [2:0][3:0] x = '{1, 2, 3, 4}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; bit x = '{1'b1}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; struct packed { int x; shortint y; byte z; } v = '{1, 2}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; struct packed { int x; shortint y; byte z; } v = '{1, 2, 3, 4}; endmodule"));
}

TEST(sv_semantic_checks, foreach_loop_variable_count_rules) {
    EXPECT_FALSE(declarations_rejected("module t; logic a[10]; logic [3:0] b [2]; initial begin foreach (a[i]) begin end foreach (b[i, j]) begin end end endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; logic a[10]; initial begin foreach (a[i, j]) begin end end endmodule"));
}

TEST(sv_semantic_checks, automatic_variable_misuse_rules) {
    EXPECT_FALSE(declarations_rejected(
        "module t; reg g; reg q; task automatic ok; reg l; begin l = 1; g <= l; end endtask "
        "task st; reg l; begin l <= 1; end endtask "
        "task automatic with_static; static reg s; begin s <= 1; end endtask endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; task automatic a; struct packed { logic x; } s; s.x <= 10; endtask endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; class C; endclass task automatic a; C c1, c2; c1 <= c2; endtask endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg g; task automatic a; reg l; begin: b assign g = 1'(l); end endtask endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg g; task automatic a; reg l; begin: b assign g = signed'(l); end endtask endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg g; task automatic a; reg l; begin: b assign g = reg'(l); end endtask endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg g; task automatic a; reg l; g <= @(l) 0; endtask endmodule"));
}

TEST(sv_semantic_checks, foreach_ignores_same_name_declared_in_other_scopes) {
    // uvm_mem.svh shape: `addr` is a scalar in one function and an array `ref`
    // port in another; the foreach in the second must not see the first.
    EXPECT_FALSE(declarations_rejected(
        "package p; function void f(); bit addr; endfunction "
        "function void g(ref bit [7:0] addr[]); foreach (addr[i]) begin end endfunction endpackage"));
    EXPECT_FALSE(declarations_rejected(
        "module t; bit addr; function void g(ref bit [7:0] addr[]); foreach (addr[i]) begin end endfunction endmodule"));
}

TEST(sv_semantic_checks, legal_constant_expressions_accepted) {
    EXPECT_FALSE(declarations_rejected(
        "module t #(parameter W = 8, parameter D = W * 2)(input [W-1:0] a, output reg [D-1:0] y); "
        "localparam N = $clog2(W); reg [N:0] cnt; wire [W-1:0] w [D-1:0]; integer i; reg [7:0] v; reg q [$]; "
        "genvar g; generate for (g = 0; g < 2; g = g + 1) begin : b wire [g:0] z; end endgenerate "
        "initial begin v[i +: 2] = 0; v[3:0] = a[3:0]; end "
        "assign y = {2{a}}; assign y[7:0] = {D{1'b0}}; "
        "parameter P = $bits(v); endmodule"));
    EXPECT_FALSE(declarations_rejected("module t; wire [3:0] a; wire [7:0] b; assign b = {4'b0, {0{a}}, a}; endmodule"));
    EXPECT_FALSE(declarations_rejected(
        "module t; int q [$]; int n; initial begin q = {1, 2, 3}; n = q[0:n].size(); end endmodule"));
}

TEST(sv_semantic_checks, variables_in_constant_contexts_rejected) {
    EXPECT_TRUE(declarations_rejected("module t; integer v; parameter p = v; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; integer n; (* attr = n *) initial $display(n); endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(input [2:0] n, input [7:n] a); endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(input [2:0] n); wire [n:0] w [7:0]; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(input [2:0] n); wire [7:0] w [7:n]; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(input [2:0] n, input [7:0] a, output reg [7:0] o); initial o[7:1] = a[7:n]; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(input [2:0] n, input [7:0] a, output reg [7:0] o); initial o[n:0] = a[7:1]; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(input [2:0] n, input [7:0] a); wire [7:0] arr [7:0]; assign arr[n][0] = a[0]; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg [15:0] a; reg [15:0] m; always @(*) a[15*m:0] = 1; endmodule"));
}

TEST(sv_semantic_checks, invalid_widths_and_counts_rejected) {
    EXPECT_TRUE(declarations_rejected("module t; localparam integer X = 32'b01x; reg [X:0] r; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg [0] r; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg [] r; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg [$] r; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; typedef logic [-1] T; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t(input [] a); endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; localparam integer N = 0; int x, y; initial y = N'(x); endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; localparam integer N = -1; int x, y; initial y = N'(x); endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; localparam integer N = 32'hx; int x, y; initial y = N'(x); endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; localparam wid = 9; wire [31:0] a = {(wid-16){8'b0}}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; localparam rep = 4'bx; wire [31:0] b = {rep{8'hab}}; endmodule"));
    EXPECT_TRUE(declarations_rejected("package p; parameter wid = 9; wire [31:0] a = {(wid-16){8'b0}}; endpackage"));
    EXPECT_TRUE(declarations_rejected("module t; reg [7:0] r; initial r = {0{1'b1}}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg [7:0] r; initial r = {{0{1'b1}}}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg [31:0] r; wire s; initial r = {{2{{0{s}}}}, 16'h1}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; localparam o = 1; localparam w = 0; integer v; reg [3:0] a; initial v = a[o +: w]; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; parameter real a = 1.0; parameter real b = 2.0; parameter real c = {a, b}; endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; parameter real c = {2.0, 1.0}; endmodule"));
}

TEST(sv_semantic_checks, module_parameter_defaults_are_not_final_values) {
    // bsg_* style: a negative or zero default means "must be overridden", so a module parameter's
    // default is never judged. Only localparams and package parameters are.
    EXPECT_FALSE(declarations_rejected(
        "module t #(parameter n_p = -1, parameter w_p = 0, parameter x_p = 4'bx)(input [7:0] a, output [7:0] y); "
        "wire [7:0] b = {(n_p){a}}; wire [3:0] c = a[3 +: w_p]; wire [3:0] d [x_p:0]; int q; initial q = n_p'(a); endmodule"));
}

TEST(sv_semantic_checks, dpi_import_formals_do_not_shadow_module_names) {
    EXPECT_FALSE(declarations_rejected(
        "module t #(parameter longint cycle_p = 4); "
        "import \"DPI-C\" function int reg_clock(input longint cycle_p, input string h); "
        "localparam longint lp = {32'b0, cycle_p[31:0]}; endmodule"));
}

TEST(sv_semantic_checks, zero_width_in_conditional_generate_is_not_judged) {
    // scr1: the zero-width select sits in a branch that is not elaborated when RO_BITS == 0.
    EXPECT_FALSE(declarations_rejected(
        "module t; localparam RO = 0; logic [31:0] v; wire [3:0] o; "
        "if (RO > 0) begin : g assign o = v[2 +: RO]; end endmodule"));
}

TEST(sv_semantic_checks, real_queue_literal_is_not_a_real_concatenation) {
    // ivtest sv_queue_real: `{1.0, 2.0}` initialises a queue of real.
    EXPECT_FALSE(declarations_rejected("module t; real q [$]; initial q = {1.0, 2.0}; endmodule"));
}

TEST(sv_semantic_checks, statement_label_names_the_block_for_its_end_label) {
    // sv-tests 9.3.5: `name: begin ... end : name` and `name: fork ... join : name`.
    EXPECT_FALSE(declarations_rejected("module t; reg a; initial name: begin a = 1; end : name endmodule"));
    EXPECT_FALSE(declarations_rejected("module t; reg a; initial name: fork a = 1; join : name endmodule"));
    EXPECT_TRUE(declarations_rejected("module t; reg a; initial name: begin a = 1; end : other endmodule"));
}
