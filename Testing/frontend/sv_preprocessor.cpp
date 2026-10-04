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
#include <gmock/gmock.h>

#include "frontend/analysis/system_verilog/preprocessor/sv_preprocessor.hpp"
#include "frontend/repository_index.hpp"


using namespace preprocessor;

TEST(preprocessor, file_directive) {
    auto test_pattern = R"(
        module test_module ();
            parameter string FILENAME = `__FILE__;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    std::ostringstream oss;
    auto result = preproc.preprocess(test_pattern);
    auto check_string =R"(
        module test_module ();
            parameter string FILENAME = "/tmp/file.sv";
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, line_directive) {
    auto test_pattern = R"(module test_module ();

            parameter LINE_NUMBER = `__LINE__;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    std::ostringstream oss;
    auto result = preproc.preprocess(test_pattern);
    auto check_string =
        R"(module test_module ();

            parameter LINE_NUMBER = 3;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, empty_define) {
    auto test_pattern = R"(
        `define  a
        module test_module ();
            parameter TEST_PARAM = `a;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = ;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, simple_define) {
    auto test_pattern = R"(
        `define  a  12
        module test_module ();
            parameter TEST_PARAM = `a;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 12;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, simple_define_with_tabs) {
    auto test_pattern = R"(
        `define  a	 	 12
        module test_module ();
            parameter TEST_PARAM = `a;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 12;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, multiple_defines) {
    auto test_pattern = R"(
        `define a 12
        `define b 15
        module test_module ();
            parameter TEST_PARAM = `a + `b;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 12 + 15;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}



TEST(preprocessor, undef) {
    auto test_pattern = R"(
        `define  a  12
        `undef   a
        module test_module ();
            parameter TEST_PARAM = `a;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    preproc.preprocess(test_pattern);
    EXPECT_TRUE(preproc.has_error());
}

TEST(preprocessor, uvm_function_macro_stripped_without_error) {
    auto test_pattern = R"(
        class my_test extends uvm_test;
            `uvm_component_utils(my_test)
            function void build_phase(uvm_phase phase);
                `uvm_info("TEST", "hello", UVM_LOW)
            endfunction
        endclass
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        class my_test extends uvm_test;
            ;
            function void build_phase(uvm_phase phase);
                ;
            endfunction
        endclass
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
}

TEST(preprocessor, undefined_macro_recorded_without_fatal) {
    auto test_pattern = R"(
        module test_module ();
            parameter TEST_PARAM = `MISSING_MACRO;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    preproc.preprocess(test_pattern);
    // Single-file behavior is unchanged (still an error), but the failure is
    // classified as deferrable: no fatal flag, name recorded for quarantine.
    // Bare use (no parentheses) carries no call-site arity.
    EXPECT_TRUE(preproc.has_error());
    EXPECT_FALSE(preproc.has_fatal_error());
    const auto &undefined = preproc.get_undefined_macros();
    ASSERT_EQ(undefined.size(), 1u);
    ASSERT_TRUE(undefined.contains("MISSING_MACRO"));
    EXPECT_TRUE(undefined.at("MISSING_MACRO").empty());
}

TEST(preprocessor, call_site_arities_recorded) {
    auto test_pattern = R"(
        module test_module ();
            parameter A = `TWICE(1, 2);
            parameter B = `TWICE(3);
            parameter C = `BARE;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    preproc.preprocess(test_pattern);
    EXPECT_TRUE(preproc.has_error());
    const auto &undefined = preproc.get_undefined_macros();
    ASSERT_EQ(undefined.size(), 2u);
    ASSERT_TRUE(undefined.contains("TWICE"));
    EXPECT_THAT(undefined.at("TWICE"), testing::ElementsAre(1, 2));
    ASSERT_TRUE(undefined.contains("BARE"));
    EXPECT_TRUE(undefined.at("BARE").empty());
}

TEST(preprocessor, unknown_conditional_recorded_without_error) {
    auto test_pattern = R"(
        module test_module ();
        `ifdef SOME_ABSENT_FLAG
            parameter TEST_PARAM = 3;
        `else
            parameter TEST_PARAM = 5;
        `endif
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 5;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
    ASSERT_EQ(preproc.get_unknown_conditionals().size(), 1u);
    EXPECT_EQ(*preproc.get_unknown_conditionals().begin(), "SOME_ABSENT_FLAG");
}

TEST(preprocessor, injected_base_resolves_order_dependent_macro) {
    auto test_pattern = R"(
        module test_module ();
            parameter TEST_PARAM = `SHARED_WIDTH;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    preproc.set_base_definitions({{"SHARED_WIDTH", std::string("8")}});

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 8;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
    EXPECT_TRUE(preproc.get_undefined_macros().empty());
}

TEST(preprocessor, local_define_shadows_injected_base) {
    auto test_pattern = R"(
        `define SHARED_WIDTH 16
        module test_module ();
            parameter TEST_PARAM = `SHARED_WIDTH;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    preproc.set_base_definitions({{"SHARED_WIDTH", std::string("8")}});

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 16;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
}

TEST(preprocessor, harvested_definitions_exposed) {
    auto test_pattern = R"(
        `define LOCAL_CONST 42
        module test_module ();
            parameter TEST_PARAM = `LOCAL_CONST;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    preproc.preprocess(test_pattern);
    EXPECT_FALSE(preproc.has_error());
    const auto harvested = preproc.get_harvested_definitions();
    ASSERT_TRUE(harvested.contains("LOCAL_CONST"));
    ASSERT_TRUE(std::holds_alternative<std::string>(harvested.at("LOCAL_CONST")));
    EXPECT_EQ(std::get<std::string>(harvested.at("LOCAL_CONST")), "42");
}

TEST(preprocessor, injected_base_not_harvested) {
    auto test_pattern = R"(
        `define LOCAL_CONST 42
        module test_module ();
            parameter A = `LOCAL_CONST;
            parameter B = `SHARED_WIDTH;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    preproc.set_base_definitions({{"SHARED_WIDTH", std::string("8")}});

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter A = 42;
            parameter B = 8;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
    // Only what the file itself defined may contribute to the table:
    // re-attributing SHARED_WIDTH here would defeat self-exclusion.
    const auto harvested = preproc.get_harvested_definitions();
    EXPECT_TRUE(harvested.contains("LOCAL_CONST"));
    EXPECT_FALSE(harvested.contains("SHARED_WIDTH"));
}

TEST(preprocessor, include_via_macro_body_inlined) {
    std::ofstream ofs("/tmp/embed_inc.svh");
    ofs << "`define FROM_INC 7\n";
    ofs.close();

    auto test_pattern = R"(
        `define INC_FILE `include "/tmp/embed_inc.svh"
        `INC_FILE
        module test_module ();
            parameter TEST_PARAM = `FROM_INC;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    std::filesystem::remove("/tmp/embed_inc.svh");
    auto check_string = R"(

        module test_module ();
            parameter TEST_PARAM = 7;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
    EXPECT_EQ(preproc.get_includes().size(), 1u);
}

TEST(preprocessor, define_in_macro_body_evaluated) {
    auto test_pattern = R"(
        `define OUTER `define INNER 99
        `OUTER
        module test_module ();
            parameter TEST_PARAM = `INNER;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(

        module test_module ();
            parameter TEST_PARAM = 99;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
}

TEST(preprocessor, structural_names_never_recorded_undefined) {
    auto test_pattern = R"(
        `define VEC_INC(f) `ifdef GUARD_ENABLED `include f `endif
        `VEC_INC("whatever.svh")
        module test_module ();
            parameter TEST_PARAM = 5;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = "\n        \n\n        module test_module ();\n"
                        "            parameter TEST_PARAM = 5;\n"
                        "        endmodule\n    ";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
    EXPECT_TRUE(preproc.get_undefined_macros().empty());
}

TEST(preprocessor, uvm_simple_macro_stripped_without_error) {
    auto test_pattern = R"(
        class my_cfg extends uvm_object;
            `uvm_object_utils_begin(my_cfg)
                `uvm_field_int(my_field, UVM_DEFAULT)
            `uvm_object_utils_end
        endclass
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        class my_cfg extends uvm_object;
            ;
                ;
            
        endclass
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
}

TEST(preprocessor, global_defines) {
    auto test_pattern = R"(
        module test_module ();
            `ifdef KINTEX7
            parameter BOARD = `BOARD_ID;
            `else
            parameter BOARD = 0;
            `endif
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    preproc.set_defines({"KINTEX7", "BOARD_ID=42"});

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter BOARD = 42;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
}


TEST(preprocessor, line_comment_elimination) {
    auto test_pattern = R"(
        wire a;// This is a comment needs to go
    )";

    sv_preprocessor proc;
    auto res = proc.preprocess(test_pattern);
    auto check_string = R"(
        wire a;
    )";
    EXPECT_EQ(check_string, res);
}

TEST(preprocessor, comment_continuation_elimination) {
    auto test_pattern = R"(
        // This is a comment \
           that should also go away!
        wire a;)";

    sv_preprocessor proc;
    auto res = proc.flatten_source(test_pattern);
    auto check_string ="\n        // This is a comment            that should also go away!\n        wire a;";
    EXPECT_EQ(check_string, res);
}



TEST(preprocessor, triangle_textmojy_comment) {
    auto test_pattern = R"(
       // /!\ Only
        assign 77
            && 145;
    )";

    sv_preprocessor proc;
    auto res = proc.flatten_source(test_pattern);
    auto check_string ="\n       // /!\\ Only\n        assign 77\n            && 145;\n    ";
    EXPECT_EQ(check_string, res);
}





TEST(preprocessor, block_comment_elimination) {
    auto test_pattern = R"(
        /*
           This is a block comment \
           that should also go away!
        */
        wire a;)";
    sv_preprocessor proc;
    auto res = proc.flatten_source(test_pattern);
    auto check_string ="\n        \n        wire a;";
    EXPECT_EQ(check_string, res);
}


TEST(preprocessor, string_continuation) {
    auto test_pattern = R"(
module test;
parameter string ML_STRING = "TEST \
  parameter";
endmodule
    )";
    sv_preprocessor proc;
    auto res = proc.flatten_source(test_pattern);
    auto check_string = R"(
module test;
parameter string ML_STRING = "TEST   parameter";
endmodule
    )";
    EXPECT_EQ(check_string, res);
}


TEST(preprocessor, macro_continuation) {
    auto test_pattern = R"(
module test;
`define SUM(a,b) a + \
b
endmodule
    )";
    sv_preprocessor proc;
    auto res = proc.flatten_source(test_pattern);
    auto check_string = R"(
module test;
`define SUM(a,b) a + b
endmodule
    )";
    EXPECT_EQ(check_string, res);
}


TEST(preprocessor, multiline_macro_continuation) {
    auto test_pattern = R"(
module test;
`define SUM(a,b) a \
+ \
b
endmodule
    )";
    sv_preprocessor proc;
    auto res = proc.flatten_source(test_pattern);
    auto check_string = R"(
module test;
`define SUM(a,b) a + b
endmodule
    )";
    EXPECT_EQ(check_string, res);
}

TEST(preprocessor, ifdef_false) {
    auto test_pattern = R"(
module test;
`ifdef A
    parameter TEST_PARAM = 3;
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, ifdef_true) {
    auto test_pattern = R"(
`define A
module test;
`ifdef A
    parameter TEST_PARAM = 3;
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
    parameter TEST_PARAM = 3;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}



TEST(preprocessor, ifndef_true) {
    auto test_pattern = R"(
module test;
`ifndef A
    parameter TEST_PARAM = 3;
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
    parameter TEST_PARAM = 3;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, ifndef_false) {
    auto test_pattern = R"(
`define A
module test;
`ifndef A
    parameter TEST_PARAM = 3;
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, ifdef_else) {
    auto test_pattern = R"(
module test;
`ifdef A
    parameter TEST_PARAM = 3;
`else
    parameter TEST_PARAM = 5;
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
    parameter TEST_PARAM = 5;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}



TEST(preprocessor, ifdef_elseif_taken) {
    auto test_pattern = R"(
`define B
module test;
`ifdef A
    parameter TEST_PARAM = 3;
`elsif B
    parameter TEST_PARAM = 5;
`else
    parameter TEST_PARAM = 7;
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
    parameter TEST_PARAM = 5;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, ifdef_elseif_shadowed) {
    auto test_pattern = R"(
`define A
`define B
module test;
`ifdef A
    parameter TEST_PARAM = 3;
`elsif B
    parameter TEST_PARAM = 5;
`else
    parameter TEST_PARAM = 7;
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
    parameter TEST_PARAM = 3;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, nested_ifdefs_skipped) {
    auto test_pattern = R"(
module test;
`define B
`ifdef A
    `ifdef B
        parameter TEST_PARAM = 3;
    `endif
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, nested_ifdef_else_skipped) {
    auto test_pattern = R"(
module test;
`define B
`define C
`ifdef A
    `ifdef B
        parameter TEST_PARAM = 3;
    `elsif C
        parameter TEST_PARAM = 5;
    `else
        parameter TEST_PARAM = 7;
    `endif
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, nested_ifdefs_outer_taken) {
    auto test_pattern = R"(
module test;
`define A
`ifdef A
    `ifdef B
        parameter TEST_PARAM = 3;
    `endif
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, nested_ifdefs_fully_taken) {
    auto test_pattern = R"(
module test;
`define A
`define B
`ifdef A
    `ifdef B
        parameter TEST_PARAM = 3;
    `endif
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
        parameter TEST_PARAM = 3;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}



TEST(preprocessor, nested_ifdefs_fully_taken_repeat) {
    auto test_pattern = R"(
module test;
`define A
`define B
`ifdef A
    `ifdef B
        parameter TEST_PARAM = 3;
    `endif
    `ifdef C
        parameter TEST_PARAM2 = 7;
    `endif
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
        parameter TEST_PARAM = 3;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, nested_elsif_taken) {
    auto test_pattern = R"(
module test;
`define C
`define A
`ifdef A
    `ifdef B
        parameter TEST_PARAM = 3;
    `elsif C
        parameter TEST_PARAM = 5;
    `else
        parameter TEST_PARAM = 7;
    `endif
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
        parameter TEST_PARAM = 5;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, nested_else_taken) {
    auto test_pattern = R"(
module test;
`define A
`ifdef A
    `ifdef B
        parameter TEST_PARAM = 3;
    `elsif C
        parameter TEST_PARAM = 5;
    `else
        parameter TEST_PARAM = 7;
    `endif
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
        parameter TEST_PARAM = 7;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, nested_else_taken_with_outer_elseif) {
    auto test_pattern = R"(
module test;
`define A
`define D
`define E
`ifdef A
    `ifdef B
        parameter TEST_PARAM = 3;
    `elsif C
        parameter TEST_PARAM = 5;
    `else
        parameter TEST_PARAM = 7;
    `endif
`else
    `ifdef D
        parameter TEST_PARAM = 9;
    `elsif E
        parameter TEST_PARAM = 11;
    `else
        parameter TEST_PARAM = 15;
    `endif
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
        parameter TEST_PARAM = 7;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, nested_else_taken_with_outer_else) {
    auto test_pattern = R"(
module test;
`define A
`define E
`ifdef A
    `ifdef B
        parameter TEST_PARAM = 3;
    `elsif C
        parameter TEST_PARAM = 5;
    `else
        parameter TEST_PARAM = 7;
    `endif
`else
    `ifdef D
        parameter TEST_PARAM = 9;
    `elsif E
        parameter TEST_PARAM = 11;
    `else
        parameter TEST_PARAM = 15;
    `endif
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test;
        parameter TEST_PARAM = 7;
endmodule
    )";
    EXPECT_EQ(check_string, result);
}


TEST(preprocessor, triple_nested_ifdef) {
    auto test_pattern = R"(
`define LEVEL_1
`define LEVEL_3
module test;
`ifdef LEVEL_1
    parameter L1 = 1;
    `ifdef LEVEL_2
        parameter L2 = 1;
        `ifdef LEVEL_3
            parameter L3_FAIL = 1;
        `endif
    `else
        parameter L2_ELSE = 1;
        `ifdef LEVEL_3
            parameter L3_SUCCESS = 1;
        `endif
    `endif
`else
    parameter L1_FAIL = 1;
`endif
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto result = preproc.preprocess(test_pattern);

    auto check_string = R"(
module test;
    parameter L1 = 1;
        parameter L2_ELSE = 1;
            parameter L3_SUCCESS = 1;
endmodule
    )";

    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, simple_macro_with_args) {
    auto test_pattern = R"(
        `define  ADD(a, b)  a+b
        module test_module ();
            parameter TEST_PARAM = `ADD(5,7);
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 5+7;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, multiline_macro_preprocessing) {
    auto test_pattern = R"(
`define SUM(a,b) a \
+ \
b
module test_module ();
    parameter TEST_PARAM = `SUM(5,7);
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto res = preproc.preprocess(test_pattern);
    auto check_string = R"(
module test_module ();
    parameter TEST_PARAM = 5 + 7;
endmodule
    )";
    EXPECT_EQ(check_string, res);
}



TEST(preprocessor, macro_pattern_in_line_comment) {
    auto test_pattern = R"(
// `UNDEFINED macro in a commend
module test_module ();
    parameter TEST_PARAM = 5;
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto res = preproc.preprocess(test_pattern);
    auto check_string = R"(

module test_module ();
    parameter TEST_PARAM = 5;
endmodule
    )";
    EXPECT_EQ(check_string, res);
}


TEST(preprocessor, macro_pattern_in_block_comment) {
    auto test_pattern = R"(
/*
 `UNDEFINED macro in a commend
*/
module test_module ();
    parameter TEST_PARAM = 5;
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");
    auto res = preproc.preprocess(test_pattern);
    auto check_string = R"(

module test_module ();
    parameter TEST_PARAM = 5;
endmodule
    )";
    EXPECT_EQ(check_string, res);
}


TEST(preprocessor, simple_macro_with_default_args) {
    auto test_pattern = R"(
        `define  ADD(a=5, b=7)  a+b
        module test_module ();
            parameter TEST_PARAM = `ADD(,);
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 5+7;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, simple_macro_with_default_args_overriden) {
    auto test_pattern = R"(
        `define  ADD(a=5, b=7)  a+b
        module test_module ();
            parameter TEST_PARAM = `ADD(,3);
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 5+3;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, multiple_macros) {
    auto test_pattern = R"(
        `define  ADD(a=5, b=7)  a+b
        module test_module ();
            parameter TEST_PARAM = `ADD(,3) * `ADD(7,);
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 5+3 * 7+7;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, simple_macro_full_defaults) {
    auto test_pattern = R"(
        `define  ADD(a=5, b=7)  a+b
        module test_module ();
            parameter TEST_PARAM = `ADD(,);
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 5+7;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, simple_macro_full_defaults_no_comma) {
    auto test_pattern = R"(
        `define  ADD(a=5, b=7)  a+b
        module test_module ();
            parameter TEST_PARAM = `ADD();
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 5+7;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}



TEST(preprocessor, simple_macro_empty_without_defaults) {
    auto test_pattern = R"(
        `define  ADD(a, b)  a 2+b 1
        module test_module ();
            parameter TEST_PARAM = `ADD(,);
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM =  2+ 1;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, comma_in_string_argument) {
    std::string test_pattern = R"(
        `define DISPLAY_MSG( msg, suffix) initial $display("%s %s", msg, suffix);
        `DISPLAY_MSG("Hello, World", "!!!")
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        initial $display("%s %s", "Hello, World", "!!!");
    )";
    EXPECT_EQ(result, check_string);
}



TEST(preprocessor, parenthesis_in_string_argument) {
    auto test_pattern = R"(
        `define DISPLAY_MSG( msg, suffix) initial $display("%s %s", msg, suffix);
        `DISPLAY_MSG("Hello( World", "!!!")
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        initial $display("%s %s", "Hello( World", "!!!");
    )";
    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, nested_macros) {
    auto test_pattern = R"(
        `define A 12
        `define B 57
        `define  ADD(a=5, b=7)  a+b
        module test_module ();
            parameter TEST_PARAM = `ADD(`A,`B);
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 12+57;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}



TEST(preprocessor, nested_macros_with_arguments) {
    auto test_pattern = R"(
        `define  MUL(c, d)  c*d
        `define  ADD(a=5, b=7)  `MUL(a, b)+`MUL(a, b)
        module test_module ();
            parameter TEST_PARAM = `ADD(12,32);
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = 12*32+12*32;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, self_nested_macro) {
    auto test_pattern = R"(
        `define TOP(a,b) a + b
        module test_module ();
            parameter TEST_PARAM = `TOP( `TOP(b,1), `TOP(42,a));
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = b + 1 + 42 + a;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, escaped_quotes_in_macro_args) {
    auto test_pattern = R"(
        `define DISPLAY_VAL(prefix, msg) initial $display("%s: %s", prefix, msg);
        module test_module ();
            `DISPLAY_VAL("DEBUG", "Value is \"hidden in quotes")
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);

    auto check_string = R"(
        module test_module ();
            initial $display("%s: %s", "DEBUG", "Value is \"hidden in quotes");
        endmodule
    )";

    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, include_absolute_path) {
    auto test_pattern = R"(
        `include "/tmp/include_test.svh"
        module test_module ();
            parameter TEST_PARAM = `A + `B;
        endmodule
    )";

    std::ofstream ofs("/tmp/include_test.svh");

    ofs<< "`define A 5\n`define B 6\n";
    ofs.close();

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    std::filesystem::remove("/tmp/include_test.svh");
    auto check_string = R"(

        module test_module ();
            parameter TEST_PARAM = 5 + 6;
        endmodule
    )";

    EXPECT_EQ(result, check_string);
}



TEST(preprocessor, include_relative_path) {
    auto test_pattern = R"(
        `include "include_test.svh"
        module test_module ();
            parameter TEST_PARAM = `A + `B;
        endmodule
    )";

    std::ofstream ofs("/tmp/include_test.svh");

    ofs<< "`define A 5\n`define B 6\n";
    ofs.close();

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    std::filesystem::remove("/tmp/include_test.svh");
    auto check_string = R"(

        module test_module ();
            parameter TEST_PARAM = 5 + 6;
        endmodule
    )";

    EXPECT_EQ(result, check_string);
}



TEST(preprocessor, include_with_comment) {
    auto test_pattern = R"(
        `include "include_test.svh" // COMMENT
        module test_module ();
            parameter TEST_PARAM = `A + `B;
        endmodule
    )";

    std::ofstream ofs("/tmp/include_test.svh");

    ofs<< "`define A 5\n`define B 6\n";
    ofs.close();

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    std::filesystem::remove("/tmp/include_test.svh");
    auto check_string = R"(

        module test_module ();
            parameter TEST_PARAM = 5 + 6;
        endmodule
    )";

    EXPECT_EQ(result, check_string);
}



TEST(preprocessor, absolute_include) {
    auto test_pattern = R"(
        `include <include_test.svh>
        module test_module ();
            parameter TEST_PARAM = `A + `B;
        endmodule
    )";

    std::ofstream ofs("/tmp/include_test.svh");

    ofs<< "`define A 5\n`define B 6\n";
    ofs.close();

    sv_preprocessor preproc;
    preproc.set_path("/tmp/test/file.sv");
    preproc.set_include_directories({"/tmp"});
    auto result = preproc.preprocess(test_pattern);
    std::filesystem::remove("/tmp/include_test.svh");
    auto check_string = R"(

        module test_module ();
            parameter TEST_PARAM = 5 + 6;
        endmodule
    )";

    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, nested_includes) {
    auto test_pattern = R"(
        `include "/tmp/include_test.svh"
        module test_module ();
            parameter TEST_PARAM = `A + `B;
        endmodule
    )";

    std::ofstream ofs("/tmp/include_test.svh");

    ofs<< "\n\n`include \"/tmp/include_nested.svh\"\n\n";
    ofs.close();

    ofs = std::ofstream("/tmp/include_nested.svh");

    ofs<< "`define A 5\n`define B 6\n";
    ofs.close();


    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    std::filesystem::remove("/tmp/include_test.svh");
    std::filesystem::remove("/tmp/include_nested.svh");
    auto check_string = R"(




        module test_module ();
            parameter TEST_PARAM = 5 + 6;
        endmodule
    )";

    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, translate_on_off) {
    auto test_pattern = R"(
        module test_module ();
            parameter TEST_PARAM = 5 + 6;
            //pragma translate_off
            parameter TEST_PARAM2 = 7;
            //pragma translate_on
        endmodule
    )";


    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = "\n        module test_module ();\n            parameter TEST_PARAM = 5 + 6;\n\n\n            \n        endmodule\n    ";

    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, complex_macro_substitution) {
    auto test_pattern = R"{(
`define ASSERT(__name, __prop, __clk = test_clk, __rst = test_rst ) \
  __name: assert property (@(posedge __clk) disable iff ((__rst) !== '0) (__prop))
`ASSERT(en2addrHit, (reg_we || reg_re) |-> $onehot0(addr_hit))
){";


    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = "\nen2addrHit: assert property (@(posedge test_clk) disable iff ((test_rst) !== '0) ((reg_we || reg_re) |-> $onehot0(addr_hit)))";

    EXPECT_EQ(result, check_string);
}


TEST(preprocessor, macro_string_concat) {
    auto test_pattern = R"(
        `define  CONCAT(a)  a``_req_t
        module test_module ();
            parameter TEST_PARAM = `CONCAT(test);
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
        module test_module ();
            parameter TEST_PARAM = test_req_t;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, macro_call_with_spaces_before_paren) {
    // SV Standard allows spaces between the macro name and the parenthesis.
    auto test_pattern = R"(
`define AXI_LITE_TYPEDEF_R_CHAN_T(r_chan_t, data_t) \
    typedef struct packed { data_t data; } r_chan_t;
module test_module ();
    `AXI_LITE_TYPEDEF_R_CHAN_T (my_chan_t, logic [31:0])
endmodule
)";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);

    auto check_string = "\nmodule test_module ();\n    typedef struct packed { logic [31:0] data; } my_chan_t;\nendmodule";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, multi_line_macro_call_no_backslashes) {
    // Arbitrary SV allows a macro invocation's argument list to be split over multiple lines
    // without requiring trailing line continuation backslashes (\).
    auto test_pattern = R"(
`define AXI_TYPEDEF_ALL(__name, __addr_t, __id_t) \
  typedef struct packed { __id_t id; __addr_t addr; } __name``_t;

`AXI_TYPEDEF_ALL(axi_slave,
                 logic [31:0],
                 logic [3:0])
)";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/multi_line_macro_test.sv");

    auto result = preproc.preprocess(test_pattern);

    auto check_string = "\n\ntypedef struct packed { \n                 logic [3:0] id; \n                 logic [31:0] addr; } axi_slave_t;";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, macro_flatten_basic_word_boundary) {
    // Verifies that a line continuation backslash immediately followed by a newline
    // inserts a space to prevent distinct tokens from merging together.
    auto test_pattern = R"(
`define BASIC_MACRO \
end\
end
module test;
  `BASIC_MACRO
endmodule
)";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/basic_boundary_test.sv");

    auto result = preproc.preprocess(test_pattern);

    // The output should cleanly evaluate "end end" separated by a space
    // instead of merging them into the invalid token "endend".
    auto check_string = "\nmodule test;\n  end end\nendmodule";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, macro_stringification_token_handling) {
    // Verifies that `" does not toggle the regular string tracking state,
    // allowing subsequent lines and macro calls to be parsed correctly.
    auto test_pattern = R"(
`define SV_RAND_CHECK(r) \
  $display("\"%s\"", `"r`");
module test;
  initial begin
    `SV_RAND_CHECK(1)
  end
endmodule
)";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/stringification_test.sv");

    auto result = preproc.preprocess(test_pattern);

    // If the fix works, the module tokens, begin, and end tags expand cleanly.
    // If it fails, the parser gets stuck "inside a string" and corrupts the output.
    auto check_string = "\nmodule test;\n  initial begin\n    $display(\"\\\"%s\\\"\", \"1\");\n  end\nendmodule";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, macro_stringification_multiline_minimal) {
    // Verifies that a multiline argument flattened inside a `" context
    // converts destructive newlines to spaces, keeping the string literal legal.
    auto test_pattern = R"(
`define STR(x) `"x`"
module test;
  initial begin
    $display(`STR(line1
line2));
  end
endmodule
)";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/stringification_minimal_test.sv");

    auto result = preproc.preprocess(test_pattern);

    auto check_string = "\nmodule test;\n  initial begin\n    $display(\"line1 line2\");\n  end\nendmodule";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, parenthesis_starting_macro_argument) {
    auto test_pattern = R"(
`define DEF_VAL 42
`define FOO(a, b = `DEF_VAL) a == b
module test;
    `FOO((x || y) |-> z);
endmodule
)";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/minimal_precedence_test.sv");

    auto result = preproc.preprocess(test_pattern);

    auto check_string = "\nmodule test;\n    (x || y) |-> z == 42;\nendmodule";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, macro_with_embedded_ifdef_taken) {
    auto test_pattern = R"(
`define INC_ASSERT
`define ASSERT(name, prop) `ifdef INC_ASSERT assert (prop)`endif
module test;
`ASSERT(check, (a || b) |-> c);
endmodule
)";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/macro_ifdef_test.sv");

    auto result = preproc.preprocess(test_pattern);

    auto check_string = "\nmodule test;\n assert ((a || b) |-> c)\n;\nendmodule";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, multi_line_macro_with_synopsys_pragma_minimal) {
    auto test_pattern = R"(`define COMPACT_ASSERT(prop) assert(prop) /``* synopsys sync_set_reset `"clk`" *``/
module test;
`COMPACT_ASSERT((a || b));
endmodule
)";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/minimal_test.sv");

    auto result = preproc.preprocess(test_pattern);

    // The block comment is entirely stripped away by the preprocessor
    auto check_string = "module test;\nassert((a || b)) ;\nendmodule";
    EXPECT_EQ(result, check_string);
}
TEST(preprocessor, recursive_macro_capped) {
    auto test_pattern = R"(
        `define A `A `A
        module test_module ();
            parameter TEST_PARAM = `A;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    // Must not hang or exhaust memory: the expansion blows past the cap and
    // the preprocessor reports an error instead.
    auto res = preproc.preprocess(test_pattern);
    EXPECT_TRUE(preproc.has_error());
}

TEST(preprocessor, self_include_rejected) {
    // A file that includes itself must be detected and error out instead of
    // recursing until the stack overflows.
    std::filesystem::create_directories("/tmp/ananke_inc_test");
    std::string inc = "/tmp/ananke_inc_test/self.svh";
    {
        std::ofstream ofs(inc);
        ofs << "`include \"/tmp/ananke_inc_test/self.svh\"\nwire self_wire;\n";
    }
    std::string pattern = "`include \"/tmp/ananke_inc_test/self.svh\"\nmodule m; endmodule\n";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/ananke_inc_test/main.sv");
    preproc.preprocess(pattern);
    EXPECT_TRUE(preproc.has_error());
    std::filesystem::remove_all("/tmp/ananke_inc_test");
}

TEST(preprocessor, include_auto_discovery) {
    std::filesystem::create_directories("/tmp/ananke_disc_test/rtl");
    std::filesystem::create_directories("/tmp/ananke_disc_test/Common");
    std::ofstream ofs("/tmp/ananke_disc_test/Common/interfaces.svh");
    ofs << "`define A 5\n`define B 6\n";
    ofs.close();

    auto idx = std::make_shared<repository_index>();
    idx->add_file("/tmp/ananke_disc_test/Common/interfaces.svh");

    auto test_pattern = R"(
        `include "interfaces.svh"
        module test_module ();
            parameter TEST_PARAM = `A + `B;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/ananke_disc_test/rtl/file.sv");
    preproc.set_repository_index(idx);

    auto result = preproc.preprocess(test_pattern);
    std::filesystem::remove_all("/tmp/ananke_disc_test");

    ASSERT_FALSE(preproc.has_error());
    auto check_string = R"(

        module test_module ();
            parameter TEST_PARAM = 5 + 6;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, include_auto_discovery_ambiguous) {
    std::filesystem::create_directories("/tmp/ananke_disc_test/a");
    std::filesystem::create_directories("/tmp/ananke_disc_test/b");
    std::ofstream ofs("/tmp/ananke_disc_test/a/interfaces.svh");
    ofs << "`define A 5\n";
    ofs.close();
    ofs = std::ofstream("/tmp/ananke_disc_test/b/interfaces.svh");
    ofs << "`define A 6\n";
    ofs.close();

    auto idx = std::make_shared<repository_index>();
    idx->add_file("/tmp/ananke_disc_test/a/interfaces.svh");
    idx->add_file("/tmp/ananke_disc_test/b/interfaces.svh");

    auto test_pattern = R"(
        `include "interfaces.svh"
        module test_module ();
            parameter TEST_PARAM = `A;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/ananke_disc_test/file.sv");
    preproc.set_repository_index(idx);

    auto result = preproc.preprocess(test_pattern);
    std::filesystem::remove_all("/tmp/ananke_disc_test");

    EXPECT_TRUE(preproc.has_error());
}

TEST(preprocessor, include_auto_discovery_angle) {
    std::filesystem::create_directories("/tmp/ananke_disc_test/Common");
    std::ofstream ofs("/tmp/ananke_disc_test/Common/interfaces.svh");
    ofs << "`define A 5\n";
    ofs.close();

    auto idx = std::make_shared<repository_index>();
    idx->add_file("/tmp/ananke_disc_test/Common/interfaces.svh");

    auto test_pattern = R"(
        `include <interfaces.svh>
        module test_module ();
            parameter TEST_PARAM = `A;
        endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/ananke_disc_test/file.sv");
    preproc.set_repository_index(idx);

    auto result = preproc.preprocess(test_pattern);
    std::filesystem::remove_all("/tmp/ananke_disc_test");

    ASSERT_FALSE(preproc.has_error());
    auto check_string = R"(

        module test_module ();
            parameter TEST_PARAM = 5;
        endmodule
    )";
    EXPECT_EQ(result, check_string);
}

TEST(preprocessor, stripped_multiline_macro_keeps_else_valid) {
    auto test_pattern = R"(
module m;
  initial begin
    if (a)
      x = 1;
    else
      `uvm_error(a,
                 b)
  end
endmodule
    )";

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = R"(
module m;
  initial begin
    if (a)
      x = 1;
    else
      ;

  end
endmodule
    )";
    EXPECT_EQ(result, check_string);
    EXPECT_FALSE(preproc.has_error());
}
