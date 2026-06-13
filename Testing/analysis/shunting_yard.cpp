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
                    Token("add_expr_p", Token::identifier),
                    Token(Token::add),
                    Token("mul_expr_p", Token::identifier),
                    Token(Token::multiply),
                    Token("5", Token::number)
    };
    auto rpn_expr = expr.to_rpm();

    Expression expected_result = {
            Token("add_expr_p", Token::identifier),
            Token("mul_expr_p", Token::identifier),
            Token("5", Token::number),
            Token(Token::multiply),
            Token(Token::add)
    };
    expected_result.set_rpn(true);
    ASSERT_EQ(rpn_expr, expected_result);

}

TEST(shunting_yard, shunting_yard_parenthesis){

    Expression expr_1 = {
                    Token("(", Token::parenthesis),
                    Token("add_expr_p", Token::identifier),
                    Token(Token::add),
                    Token("mul_expr_p", Token::identifier),
                    Token(")", Token::parenthesis),
                    Token(Token::multiply),
                    Token("5", Token::number)
    };
    auto rpn_expr_1 = expr_1.to_rpm();

    Expression expected_result_1 = {
                    Token("add_expr_p", Token::identifier),
                    Token("mul_expr_p", Token::identifier),
                    Token(Token::add),
                    Token("5", Token::number),
                    Token(Token::multiply)
    };

    expected_result_1.set_rpn(true);
    ASSERT_EQ(rpn_expr_1, expected_result_1);

    Expression expr_2 = {
                    Token("5", Token::number),
                    Token(Token::multiply),
                    Token("(", Token::parenthesis),
                    Token("add_expr_p", Token::identifier),
                    Token(Token::add),
                    Token("mul_expr_p", Token::identifier),
                    Token(")", Token::parenthesis)
    };

    auto rpn_expr_2 = expr_2.to_rpm();

    Expression expected_result_2 = {
                    Token("5", Token::number),
                    Token("add_expr_p", Token::identifier),
                    Token("mul_expr_p", Token::identifier),
                    Token(Token::add),
                    Token(Token::multiply)
    };
    expected_result_2.set_rpn(true);
    ASSERT_EQ(rpn_expr_2, expected_result_2);
}

TEST(shunting_yard, shunting_yard_parenthesis_complex){

    Expression expr_1 = {
                    Token("(", Token::parenthesis),
                    Token("4", Token::number),
                    Token(Token::multiply),
                    Token("3", Token::number),
                    Token(Token::add),
                    Token("5", Token::number),
                    Token(")", Token::parenthesis),
                    Token(Token::add),
                    Token("1", Token::number)
    };
    auto rpn_expr_1 = expr_1.to_rpm();


    Expression expected_result_1 = {
                    Token("4", Token::number),
                    Token("3", Token::number),
                    Token(Token::multiply),
                    Token("5", Token::number),
                    Token(Token::add),
                    Token("1", Token::number),
                    Token(Token::add)
    };

    expected_result_1.set_rpn(true);
    ASSERT_EQ(rpn_expr_1, expected_result_1);
}

TEST(shunting_yard, shunting_yard_test_5){

    Expression expr_1 = {
                    Token("N_CHANNELS", Token::identifier),
                    Token("8", Token::number),
                    Token(Token::divide),
                    Token("1", Token::number),
                    Token(Token::add)
    };
    expr_1.set_rpn(true);
    auto rpn_expr_1 = expr_1.to_rpm();
    ASSERT_EQ(rpn_expr_1, expr_1);
}
