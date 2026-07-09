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

#include "frontend/analysis/preprocessor/sv_preprocessor.hpp"


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

    EXPECT_THROW(preproc.preprocess(test_pattern), std::runtime_error);
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
  __name: assert property (@(posedge __clk) disable iff ((__rst) !== '0) (__prop))       \
`endif
`ASSERT(en2addrHit, (reg_we || reg_re) |-> $onehot0(addr_hit))
){";


    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto check_string = "\nen2addrHit: assert property (@(posedge test_clk) disable iff ((test_rst) !== '0) ((reg_we || reg_re) |-> $onehot0(addr_hit)))       `endif";

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