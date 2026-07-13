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
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"




TEST(Token, sized_binary_sv_constant_parsing){
    Numeric_token ec("8'b10110");

    Numeric_token check;
    check.set_binary_size(8);
    check.set_value(22);
    EXPECT_EQ(check, ec);
}


TEST(Token, sized_octal_sv_constant_parsing){
    Numeric_token ec("12'o547");

    Numeric_token check;
    check.set_binary_size(12);
    check.set_value(359);
    EXPECT_EQ(check, ec);
}


TEST(Token, sized_decimal_sv_constant_parsing){
    Numeric_token ec("14'd1542");

    Numeric_token check;
    check.set_binary_size(14);
    check.set_value(1542);
    EXPECT_EQ(check, ec);
}


TEST(Token, sized_hexadecimal_sv_constant_parsing){
    Numeric_token ec("20'hCA54");

    Numeric_token check;
    check.set_binary_size(20);
    check.set_value(51796);
    EXPECT_EQ(check, ec);

}



TEST(Token, unsized_binary_sv_constant_parsing){
    Numeric_token ec("'b10110");

    Numeric_token check;
    check.set_binary_size(5);
    check.set_value(22);
    EXPECT_EQ(check, ec);

}


TEST(Token, unsized_octal_sv_constant_parsing){
    Numeric_token ec("'o547");


    Numeric_token check;
    check.set_binary_size(9);
    check.set_value(359);
    EXPECT_EQ(check, ec);

}


TEST(Token, unsized_decimal_sv_constant_parsing){
    Numeric_token ec("'d1542");

    Numeric_token check;
    check.set_binary_size(11);
    check.set_value(1542);
    EXPECT_EQ(check, ec);

}


TEST(Token, unsized_hexadecimal_sv_constant_parsing){
    Numeric_token ec("'hCA54");


    Numeric_token check;
    check.set_binary_size(16);
    check.set_value(51796);
    EXPECT_EQ(check, ec);

}

TEST(Token, signed_sized_hex_with_separator){
    Numeric_token ec("-8'sh1_F");

    Numeric_token check;
    check.set_binary_size(8);
    hdl_integer i;
    i.set_value(31);
    i.set_signed(true);
    check.set_value(i);
    EXPECT_EQ(check, ec);
}


;

TEST(Token, wide_input_decimal_processing) {
    auto test_str = "19446743180356354048";
    Numeric_token test_token(test_str);

    auto val = test_token.get_value();

    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val.value().is_integer());

    Numeric_token check;
    hdl_integer check_val;
    check_val.set_value(int1024_t(test_str));
    check_val.set_signed(false);
    check.set_value(check_val);
    EXPECT_EQ(check, test_token);
}


TEST(Token, narrow_input_zero_padded) {

    Numeric_token test_token("00099974318035635404");

    auto val = test_token.get_value();

    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val.value().is_integer());
    EXPECT_EQ(val.value().get_integer().get_value(), 99974318035635404);
}


TEST(Token, wide_input_sized_processing) {

    Numeric_token test_token("72'hCAFEBEBEDEADBEEFCAFE");

    auto val = test_token.get_value();

    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val.value().is_integer());

    Numeric_token check;
    hdl_integer check_val;
    check_val.set_value(int1024_t("0xCAFEBEBEDEADBEEFCAFE"));
    check_val.set_signed(false);
    check.set_value(check_val);
    EXPECT_EQ(check, test_token);
}


TEST(Token, wide_input_auto_sized_processing) {

    Numeric_token test_token("'hCAFEBEBEDEADBEEFCAFE");

    auto val = test_token.get_value();

    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val.value().is_integer());

    Numeric_token check;
    hdl_integer check_val;
    check_val.set_value(int1024_t("0xCAFEBEBEDEADBEEFCAFE"));
    check_val.set_signed(false);
    check.set_value(check_val);
    EXPECT_EQ(check, test_token);
}
