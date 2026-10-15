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
            enum logic [2:0] {A = 3'h2, B = 3'h3, C = 1, D} myenum;
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
