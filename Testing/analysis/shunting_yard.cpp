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

#include "data_model/HDL/parameters/HDL_parameter.hpp"


TEST(shunting_yard, shunting_yard_priority){


    Expression expr = {
                    Expression_component("add_expr_p", Expression_component::identifier),
                    Expression_component("+", Expression_component::operation),
                    Expression_component("mul_expr_p", Expression_component::identifier),
                    Expression_component("*", Expression_component::operation),
                    Expression_component("5", Expression_component::number)
    };
    auto rpn_expr = expr.to_rpm();

    Expression expected_result = {
            Expression_component("add_expr_p", Expression_component::identifier),
            Expression_component("mul_expr_p", Expression_component::identifier),
            Expression_component("5", Expression_component::number),
            Expression_component("*", Expression_component::operation),
            Expression_component("+", Expression_component::operation)
    };
    expected_result.set_rpn(true);
    ASSERT_EQ(rpn_expr, expected_result);

}

TEST(shunting_yard, shunting_yard_parenthesis){

    Expression expr_1 = {
                    Expression_component("(", Expression_component::parenthesis),
                    Expression_component("add_expr_p", Expression_component::identifier),
                    Expression_component("+", Expression_component::operation),
                    Expression_component("mul_expr_p", Expression_component::identifier),
                    Expression_component(")", Expression_component::parenthesis),
                    Expression_component("*", Expression_component::operation),
                    Expression_component("5", Expression_component::number)
    };
    auto rpn_expr_1 = expr_1.to_rpm();

    Expression expected_result_1 = {
                    Expression_component("add_expr_p", Expression_component::identifier),
                    Expression_component("mul_expr_p", Expression_component::identifier),
                    Expression_component("+", Expression_component::operation),
                    Expression_component("5", Expression_component::number),
                    Expression_component("*", Expression_component::operation)
    };

    expected_result_1.set_rpn(true);
    ASSERT_EQ(rpn_expr_1, expected_result_1);

    Expression expr_2 = {
                    Expression_component("5", Expression_component::number),
                    Expression_component("*", Expression_component::operation),
                    Expression_component("(", Expression_component::parenthesis),
                    Expression_component("add_expr_p", Expression_component::identifier),
                    Expression_component("+", Expression_component::operation),
                    Expression_component("mul_expr_p", Expression_component::identifier),
                    Expression_component(")", Expression_component::parenthesis)
    };

    auto rpn_expr_2 = expr_2.to_rpm();

    Expression expected_result_2 = {
                    Expression_component("5", Expression_component::number),
                    Expression_component("add_expr_p", Expression_component::identifier),
                    Expression_component("mul_expr_p", Expression_component::identifier),
                    Expression_component("+", Expression_component::operation),
                    Expression_component("*", Expression_component::operation)
    };
    expected_result_2.set_rpn(true);
    ASSERT_EQ(rpn_expr_2, expected_result_2);
}

TEST(shunting_yard, shunting_yard_parenthesis_complex){

    Expression expr_1 = {
                    Expression_component("(", Expression_component::parenthesis),
                    Expression_component("4", Expression_component::number),
                    Expression_component("*", Expression_component::operation),
                    Expression_component("3", Expression_component::number),
                    Expression_component("+", Expression_component::operation),
                    Expression_component("5", Expression_component::number),
                    Expression_component(")", Expression_component::parenthesis),
                    Expression_component("+", Expression_component::operation),
                    Expression_component("1", Expression_component::number)
    };
    auto rpn_expr_1 = expr_1.to_rpm();


    Expression expected_result_1 = {
                    Expression_component("4", Expression_component::number),
                    Expression_component("3", Expression_component::number),
                    Expression_component("*", Expression_component::operation),
                    Expression_component("5", Expression_component::number),
                    Expression_component("+", Expression_component::operation),
                    Expression_component("1", Expression_component::number),
                    Expression_component("+", Expression_component::operation)
    };

    expected_result_1.set_rpn(true);
    ASSERT_EQ(rpn_expr_1, expected_result_1);
}

TEST(shunting_yard, shunting_yard_test_5){

    Expression expr_1 = {
                    Expression_component("N_CHANNELS", Expression_component::identifier),
                    Expression_component("8", Expression_component::number),
                    Expression_component("/", Expression_component::operation),
                    Expression_component("1", Expression_component::number),
                    Expression_component("+", Expression_component::operation)
    };
    expr_1.set_rpn(true);
    auto rpn_expr_1 = expr_1.to_rpm();
    ASSERT_EQ(rpn_expr_1, expr_1);
}
