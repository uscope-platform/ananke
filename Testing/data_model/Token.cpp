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



TEST(Token, sized_binary_sv_constant_parsing){
    Token ec("8'b10110", Token::number);

    ASSERT_TRUE(ec.is_numeric());
    ASSERT_EQ(ec.get_value().value().get_integer(), 22);
    ASSERT_EQ(ec.get_binary_size(), 8);
}


TEST(Token, sized_octal_sv_constant_parsing){
    Token ec("12'o547", Token::number);

    ASSERT_TRUE(ec.is_numeric());
    ASSERT_EQ(ec.get_value().value().get_integer(), 359);
    ASSERT_EQ(ec.get_binary_size(), 12);
}


TEST(Token, sized_decimal_sv_constant_parsing){
    Token ec("14'd1542", Token::number);

    ASSERT_TRUE(ec.is_numeric());
    ASSERT_EQ(ec.get_value().value().get_integer(), 1542);
    ASSERT_EQ(ec.get_binary_size(), 14);
}


TEST(Token, sized_hexadecimal_sv_constant_parsing){
    Token ec("20'hCA54", Token::number);

    ASSERT_TRUE(ec.is_numeric());
    ASSERT_EQ(ec.get_value().value().get_integer(), 51796);
    ASSERT_EQ(ec.get_binary_size(), 20);
}



TEST(Token, unsized_binary_sv_constant_parsing){
    Token ec("'b10110", Token::number);

    ASSERT_TRUE(ec.is_numeric());
    ASSERT_EQ(ec.get_value().value().get_integer(), 22);
    ASSERT_EQ(ec.get_binary_size(), 5);
}


TEST(Token, unsized_octal_sv_constant_parsing){
    Token ec("'o547", Token::number);

    ASSERT_TRUE(ec.is_numeric());
    ASSERT_EQ(ec.get_value().value().get_integer(), 359);
    ASSERT_EQ(ec.get_binary_size(), 9);
}


TEST(Token, unsized_decimal_sv_constant_parsing){
    Token ec("'d1542", Token::number);

    ASSERT_TRUE(ec.is_numeric());
    ASSERT_EQ(ec.get_value().value().get_integer(), 1542);
    ASSERT_EQ(ec.get_binary_size(), 11);
}


TEST(Token, unsized_hexadecimal_sv_constant_parsing){
    Token ec("'hCA54", Token::number);

    ASSERT_TRUE(ec.is_numeric());
    ASSERT_EQ(ec.get_value().value().get_integer(), 51796);
    ASSERT_EQ(ec.get_binary_size(), 16);
}


TEST(Token, string){
    Token ec("\"FALSE\"", Token::string);

    ASSERT_TRUE(ec.is_string());
    ASSERT_FALSE(ec.is_identifier());
    ASSERT_EQ(ec.get_value().value().get_string(), "\"FALSE\"");
}


TEST(Token, identifier){
    Token ec("FALSE", Token::identifier);

    ASSERT_TRUE(ec.is_identifier());
    ASSERT_FALSE(ec.is_string());
    ASSERT_EQ(ec.get_value().value().get_string(), "FALSE");
}

