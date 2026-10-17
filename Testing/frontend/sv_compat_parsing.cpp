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

TEST(sv_compat_parsing, extern_function_without_return_type) {
    // Caliptra soc_ifc_tb_pkg.sv shape: the return type of an extern function
    // declaration may be omitted (defaults to logic), same as in a plain
    // function_declaration.
    auto test_pattern = R"(
        class SocRegisters;
            extern function new();
            extern function lock_fuses();
            extern function void build_inverse_addr_map();
            extern function word_addr_t get_addr(string name);
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(sv_compat_parsing, bare_brace_assoc_literal_string_keys) {
    // Caliptra soc_ifc_tb_pkg.sv shape: associative-array literal with string
    // keys but no `'` assignment-pattern prefix (simulator-tolerated).
    auto test_pattern = R"(
        package p;
            logic [2:0] dict [string] = { "AAA" : 3'b000, "BBB" : 3'b100 };
        endpackage
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(sv_compat_parsing, bare_brace_assoc_literal_identifier_keys) {
    // Struct literal shape: `{addr: addr, data: d}` without apostrophe.
    auto test_pattern = R"(
        package p;
            typedef struct { logic [31:0] addr; logic [31:0] data; int tid; } transaction_t;
            function automatic void f(logic [31:0] addr, logic [31:0] d, int t);
                transaction_t new_trans;
                new_trans = {addr: addr, data: d, tid: t};
            endfunction
        endpackage
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(sv_compat_parsing, plain_concatenations_unaffected) {
    // Guards against the new assoc-literal alternative hijacking ordinary
    // concatenations: string members, ternaries (which contain a colon) and
    // replications must still parse as concatenations.
    auto test_pattern = R"(
        module m (
            input logic clk, input logic sel, input logic [7:0] a, input logic [7:0] b,
            output logic [7:0] y, output logic [15:0] z, output string s
        );
            assign y = sel ? a : b;
            assign z = {a, b};
            assign s = {"x", "y"};
            assign y = {2{a[3:0]}};
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(sv_compat_parsing, begin_keywords_verilog_region_frees_sv_keywords) {
    // ivtest pr1978358 / pr2470181b shape: SV-only keywords are plain
    // identifiers inside a `begin_keywords "1364-2005" region.
    auto test_pattern = R"(
`begin_keywords "1364-2005"
module m;
  reg bit;
  reg expect;
  wire var;
  assign var = bit & expect;
endmodule
`end_keywords
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(sv_compat_parsing, begin_keywords_region_ends_with_end_keywords) {
    // After `end_keywords the SV keyword set is back: `bit` is a type again.
    auto test_pattern = R"(
`begin_keywords "1364-2001"
module a;
  reg bit;
endmodule
`end_keywords
module b;
  bit x;
endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(sv_compat_parsing, begin_keywords_1995_drops_2001_keywords) {
    auto test_pattern = R"(
`begin_keywords "1364-1995"
module m;
  integer generate;
  integer signed;
endmodule
`end_keywords
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(sv_compat_parsing, width_timing_check_without_threshold) {
    // basejump_stl mobile_ddr.sv shape: Verilog-2001 allows $width with only
    // the limit argument (threshold and notifier are optional).
    auto test_pattern = R"(
        module m(input clk, input a, input b);
            specify
                $width (posedge a &&& b, 5);
                $width (negedge a, 5, 1);
                $setuphold(posedge clk, a, 1, 2);
            endspecify
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(sv_compat_parsing, width_timing_check_without_threshold_rejected_in_strict) {
    auto test_pattern = R"(
        module m(input a);
            specify
                $width (negedge a, 5);
            endspecify
        endmodule
    )";

    parse_options strict;
    strict.strict = true;
    sv_analyzer analyzer;
    analyzer.set_options(strict);
    EXPECT_FALSE(analyzer.analyze("", test_pattern).has_value());
}

TEST(sv_compat_parsing, bare_begin_end_block_at_module_scope) {
    // BlackParrot declare_csr macro shape: a named begin/end block holding an
    // instance at module scope, with no enclosing generate construct.
    auto test_pattern = R"(
        module m(input clk, input [3:0] d);
            logic [3:0] q;
            begin : creg_x
                dff #(.width_p(4)) x_reg (.clk_i(clk), .data_i(d), .data_o(q));
            end
            assign d2 = q;
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value()) << analyzer.get_error();
    EXPECT_FALSE(analyzer.has_syntax_errors());

    parse_options strict;
    strict.strict = true;
    sv_analyzer strict_analyzer;
    strict_analyzer.set_options(strict);
    EXPECT_FALSE(strict_analyzer.analyze("", test_pattern).has_value());
}
