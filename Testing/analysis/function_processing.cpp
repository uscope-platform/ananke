//  Copyright 2023 Filippo Savi
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

#include "frontend/analysis/sv_analyzer.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"


TEST(function_processing, simple_function_scalar) {
    auto test_pattern = R"(
        module test_mod #(
        )();

            function integer CTRL_ADDR_CALC();
                CTRL_ADDR_CALC = 67;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern)[0];
    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));

    auto call = HDL_function_call("CTRL_ADDR_CALC");
    call.add_body(functions["CTRL_ADDR_CALC"].get_assignments(), functions["CTRL_ADDR_CALC"].get_loop());

    HDL_function_def check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    assignment a("CTRL_ADDR_CALC", std::nullopt, std::make_shared<Token>("67", Token::number));
    check_f.add_assignment(a);
    EXPECT_EQ(check_f,functions["CTRL_ADDR_CALC"]);

    auto values = call.evaluate({});
    ASSERT_TRUE(values.has_value());
    EXPECT_TRUE(values.value().is_integer());
    auto result_value = values.value().get_integer();
    EXPECT_EQ(result_value, 67);
}


TEST(function_processing, simple_function_array) {
    auto test_pattern = R"(
        module test_mod #(
        )();

            function integer CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 100;
                CTRL_ADDR_CALC[1] = 200;
                CTRL_ADDR_CALC[2] = 300;
                IGNORED_ASSIGNMENT[2] = 1;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("", test_pattern)[0];
    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];
    HDL_function_def check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    assignment a(
        "CTRL_ADDR_CALC",
        std::make_shared<Token>("0", Token::number),
        std::make_shared<Token>("100", Token::number)
        );
    check_f.add_assignment(a);
    a = assignment(
        "CTRL_ADDR_CALC",
        std::make_shared<Token>("1", Token::number),
        std::make_shared<Token>("200", Token::number)
        );
    check_f.add_assignment(a);
    a = assignment(
        "CTRL_ADDR_CALC",
        std::make_shared<Token>("2", Token::number),
        std::make_shared<Token>("300", Token::number)
        );
    check_f.add_assignment(a);
    EXPECT_EQ(check_f,result);
}



TEST(function_processing, simple_loop_function) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 3
        )();

            function logic [31:0] CTRL_ADDR_CALC();
                for(int i = 0; i<3; i++)begin
                    CTRL_ADDR_CALC[i] = 100*i;
                end
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    HDL_function_def check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    auto metadata = HDL_loop_metadata();

    HDL_parameter p;
    p.set_name("i");
    p.add_component(Token("0", Token::number));
    metadata.set_init(p);

    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Token>("3", Token::number));
    e.set_operation(Expression_v2::less);
    metadata.set_end_c(e);

    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::add);
    metadata.set_iter(e);

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression(Token("i", Token::identifier))),
        std::make_shared<Expression>(Expression({Token("100", Token::number), Token(Token::multiply), Token("i", Token::identifier)}))
    };

    metadata.add_assignment(a);
    check_f.add_loop_metadata(metadata);
     EXPECT_EQ(check_f,result);
}


TEST(function_processing, parametric_loop_function) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 3
        )();

            function logic [31:0] CTRL_ADDR_CALC();
                for(int i = 0; i<N_CORES; i++)begin
                    CTRL_ADDR_CALC[i] = 100*i;
                end
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    HDL_function_def check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    auto metadata = HDL_loop_metadata();

    HDL_parameter p;
    p.set_name("i");
    p.add_component(Token("0", Token::number));
    metadata.set_init(p);

    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Token>("N_CORES", Token::identifier));
    e.set_operation(Expression_v2::less);
    metadata.set_end_c(e);

    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::identifier));
    e.set_operation(Expression_v2::add);
    metadata.set_iter(e);

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression(Token("i", Token::identifier))),
        std::make_shared<Expression>(Expression({Token("100", Token::number), Token(Token::multiply), Token("i", Token::identifier)}))
    };

    metadata.add_assignment(a);
    check_f.add_loop_metadata(metadata);
    EXPECT_EQ(check_f,result);
}

TEST(function_processing, complex_loop_function) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 3
        )();

            function logic [31:0] CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 44;
                for(int i = 1; i<N_CORES+1; i++)begin
                    CTRL_ADDR_CALC[i] = 100*i;
                end
                CTRL_ADDR_CALC[4] = 667;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    HDL_function_def check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    auto metadata = HDL_loop_metadata();

    HDL_parameter p;
    p.set_name("i");
    p.add_component(Token("1", Token::number));
    metadata.set_init(p);
    Expression_v2 e, e2;
    e2.set_lhs(std::make_shared<Token>("N_CORES", Token::identifier));
    e2.set_rhs(std::make_shared<Token>("1", Token::number));
    e2.set_operation(Expression_v2::add);
    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Expression_v2>(e2));
    e.set_operation(Expression_v2::less);
    metadata.set_end_c(e);

    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::identifier));
    e.set_operation(Expression_v2::add);
    metadata.set_iter(e);

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression(Token("i", Token::identifier))),
        std::make_shared<Expression>(Expression({Token("100", Token::number), Token(Token::multiply), Token("i", Token::identifier)}))
    };

    metadata.add_assignment(a);
    check_f.add_loop_metadata(metadata);
    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression(Token("0", Token::number))),
        std::make_shared<Expression>(Expression(Token("44", Token::number)))
    };
    check_f.add_assignment(a);
    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression(Token("4", Token::number))),
        std::make_shared<Expression>(Expression(Token("667", Token::number)))
    };
    check_f.add_assignment(a);
    EXPECT_EQ(check_f,result);
}


TEST(function_processing, parametrized_function) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 1
        )();

            function logic [31:0] CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 44;
                CTRL_ADDR_CALC[N_CORES] = 33;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    HDL_function_def check_f;
    check_f.set_name("CTRL_ADDR_CALC");

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression(Token("0", Token::number))),
        std::make_shared<Expression>(Expression(Token("44", Token::number)))
    };
    check_f.add_assignment(a);
    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression(Token("N_CORES", Token::identifier))),
        std::make_shared<Expression>(Expression(Token("33", Token::number)))
    };
    check_f.add_assignment(a);
    EXPECT_EQ(check_f,result);
}


TEST(function_processing, package_assignment) {
    auto test_pattern = R"(
        module test_mod ();


        function logic[31:0] CTRL_ADDR_CALC();
            CTRL_ADDR_CALC[0] = hil_address_space::bus_base;
        endfunction

        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];


    HDL_function_def check_f;
    check_f.set_name("CTRL_ADDR_CALC");

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression(Token("0", Token::number))),
        std::make_shared<Expression>(Expression(Token("bus_base", Token::identifier)))
    };
    a.get_value()->as<Expression>().components[0]->as<Token>().set_package_prefix("hil_address_space");
    check_f.add_assignment(a);

    EXPECT_EQ(check_f,result);

}



