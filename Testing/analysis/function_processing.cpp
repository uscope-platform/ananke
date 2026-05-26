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
    assignment a("CTRL_ADDR_CALC", std::nullopt, std::make_shared<Expression>(Expression({Expression_component("67", Expression_component::number)})));
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
        std::make_shared<Expression>(Expression({Expression_component("0", Expression_component::number)})),
        std::make_shared<Expression>(Expression({Expression_component("100", Expression_component::number)}))
        );
    check_f.add_assignment(a);
    a = assignment(
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("1", Expression_component::number)})),
        std::make_shared<Expression>(Expression({Expression_component("200", Expression_component::number)}))
        );
    check_f.add_assignment(a);
    a = assignment(
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("2", Expression_component::number)})),
        std::make_shared<Expression>(Expression({Expression_component("300", Expression_component::number)}))
        );
    check_f.add_assignment(a);
    EXPECT_EQ(check_f,result);

    auto call = HDL_function_call("CTRL_ADDR_CALC");
    call.add_body(functions["CTRL_ADDR_CALC"].get_assignments(), functions["CTRL_ADDR_CALC"].get_loop());

    mdarray<hdl_integer> check_val;
    check_val.set_1d_slice({0,0}, {100,200,300});
    auto values = call.evaluate({});
    ASSERT_TRUE(values.has_value());
    EXPECT_TRUE(values.value().is_int_array());
    auto result_value = values.value().get_int_array();
    EXPECT_EQ(result_value, check_val);
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
    p.add_component(Expression_component("0", Expression_component::number));
    metadata.set_init(p);
    metadata.set_end_c({
        Expression_component("i", Expression_component::identifier),
        Expression_component("<", Expression_component::operation),
        Expression_component("3", Expression_component::number)
        });

    metadata.set_iter({
        Expression_component("i", Expression_component::identifier),
        Expression_component("+", Expression_component::operation),
        Expression_component("1", Expression_component::number)});

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("i", Expression_component::identifier)})),
        std::make_shared<Expression>(Expression({Expression_component("100", Expression_component::number), Expression_component("*", Expression_component::operation), Expression_component("i", Expression_component::identifier)}))
    };

    metadata.add_assignment(a);
    check_f.add_loop_metadata(metadata);
     EXPECT_EQ(check_f,result);

    auto call = HDL_function_call("CTRL_ADDR_CALC");
    call.add_body(functions["CTRL_ADDR_CALC"].get_assignments(), functions["CTRL_ADDR_CALC"].get_loop());

    mdarray<hdl_integer> check_val;
    check_val.set_1d_slice({0,0}, {0, 100,200});
    auto values = call.evaluate({});
    ASSERT_TRUE(values.has_value());
    EXPECT_TRUE(values.value().is_int_array());
    auto result_value = values.value().get_int_array();
    EXPECT_EQ(result_value, check_val);
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
    p.add_component(Expression_component("0", Expression_component::number));
    metadata.set_init(p);
    metadata.set_end_c({
        Expression_component("i", Expression_component::identifier),
        Expression_component("<", Expression_component::operation),
        Expression_component("N_CORES", Expression_component::identifier)
        });

    metadata.set_iter({
        Expression_component("i", Expression_component::identifier),
        Expression_component("+", Expression_component::operation),
        Expression_component("1", Expression_component::number)});

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("i", Expression_component::identifier)})),
        std::make_shared<Expression>(Expression({Expression_component("100", Expression_component::number), Expression_component("*", Expression_component::operation), Expression_component("i", Expression_component::identifier)}))
    };

    metadata.add_assignment(a);
    check_f.add_loop_metadata(metadata);
    EXPECT_EQ(check_f,result);

    mdarray<hdl_integer> check_val;
    check_val.set_1d_slice({0,0}, {0,100,200});

    auto call = HDL_function_call("CTRL_ADDR_CALC");
    call.add_body(functions["CTRL_ADDR_CALC"].get_assignments(), functions["CTRL_ADDR_CALC"].get_loop());


    auto res= call.propagate_constant({"","", "N_CORES"}, 3);
    auto values = call.evaluate({});
    ASSERT_TRUE(values.has_value());
    EXPECT_TRUE(values.value().is_int_array());
    auto result_value = values.value().get_int_array();
    EXPECT_EQ(result_value, check_val);
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
    p.add_component(Expression_component("1", Expression_component::number));
    metadata.set_init(p);
    metadata.set_end_c({
                Expression_component("i", Expression_component::identifier),
               Expression_component("<", Expression_component::operation),
               Expression_component("N_CORES", Expression_component::identifier),
               Expression_component("+", Expression_component::operation),
               Expression_component("1", Expression_component::number)
    });

    metadata.set_iter({
            Expression_component("i", Expression_component::identifier),
            Expression_component("+", Expression_component::operation),
            Expression_component("1", Expression_component::number)
        });

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("i", Expression_component::identifier)})),
        std::make_shared<Expression>(Expression({Expression_component("100", Expression_component::number), Expression_component("*", Expression_component::operation), Expression_component("i", Expression_component::identifier)}))
    };

    metadata.add_assignment(a);
    check_f.add_loop_metadata(metadata);
    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("0", Expression_component::number)})),
        std::make_shared<Expression>(Expression({Expression_component("44", Expression_component::number)}))
    };
    check_f.add_assignment(a);
    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("4", Expression_component::number)})),
        std::make_shared<Expression>(Expression({Expression_component("667", Expression_component::number)}))
    };
    check_f.add_assignment(a);
    EXPECT_EQ(check_f,result);

    auto call = HDL_function_call("CTRL_ADDR_CALC");
    call.add_body(functions["CTRL_ADDR_CALC"].get_assignments(), functions["CTRL_ADDR_CALC"].get_loop());


    mdarray<hdl_integer> check_val;
    check_val.set_1d_slice({0,0}, {44, 100,200,300, 667});
    call.propagate_constant({"","", "N_CORES"}, 3);
    auto values = call.evaluate({});
    ASSERT_TRUE(values.has_value());
    EXPECT_TRUE(values.value().is_int_array());
    auto result_value = values.value().get_int_array();
    EXPECT_EQ(result_value, check_val);
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
        std::make_shared<Expression>(Expression({Expression_component("0", Expression_component::number)})),
        std::make_shared<Expression>(Expression({Expression_component("44", Expression_component::number)}))
    };
    check_f.add_assignment(a);
    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("N_CORES", Expression_component::identifier)})),
        std::make_shared<Expression>(Expression({Expression_component("33", Expression_component::number)}))
    };
    check_f.add_assignment(a);
    EXPECT_EQ(check_f,result);

    mdarray<hdl_integer> check_val;
    check_val.set_1d_slice({0,0}, {44, 33});

    auto call = HDL_function_call("CTRL_ADDR_CALC");
    call.add_body(functions["CTRL_ADDR_CALC"].get_assignments(), functions["CTRL_ADDR_CALC"].get_loop());


    call.propagate_constant({"","", "N_CORES"}, 1);
    auto values = call.evaluate({});
    ASSERT_TRUE(values.has_value());
    EXPECT_TRUE(values.value().is_int_array());
    auto result_value =values.value().get_int_array();
    EXPECT_EQ(result_value, check_val);
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
        std::make_shared<Expression>(Expression({Expression_component("0", Expression_component::number)})),
        std::make_shared<Expression>(Expression({Expression_component("bus_base", Expression_component::identifier)}))
    };
    a.get_value()->as<Expression>().components[0].set_package_prefix("hil_address_space");
    check_f.add_assignment(a);

    EXPECT_EQ(check_f,result);


    auto call = HDL_function_call("CTRL_ADDR_CALC");
    call.add_body(functions["CTRL_ADDR_CALC"].get_assignments(), functions["CTRL_ADDR_CALC"].get_loop());
    call.propagate_constant({"hil_address_space","", "bus_base"}, 21);
    auto values = call.evaluate({});
    ASSERT_TRUE(values.has_value());
    ASSERT_TRUE(values.value().is_integer());
    auto result_value = values.value().get_integer();
    EXPECT_EQ(result_value, 21);
}



