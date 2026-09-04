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
#include "data_model/HDL/parameters/components/token/Real_token.hpp"
#include "data_model/HDL/parameters/components/token/String_token.hpp"
#include "data_model/HDL/parameters/common/hdl_integer.hpp"
#include "data_model/HDL/parameters/common/resolved_parameter.hpp"
#include <sstream>
#include <cereal/archives/binary.hpp>
#include <cereal/types/polymorphic.hpp>
#include <cereal/types/memory.hpp>




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
        hdl_integer i(-31);
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
    check_val.set_value(wide_integer("0x10DE0B5E3A7640000"));
    check_val.set_signed(false);
    check.set_binary_size(65);
    check.set_value(check_val);
    EXPECT_EQ(check, test_token);
}

TEST(hdl_integer, cross_representation_equality) {
    hdl_integer narrow = 5;
    hdl_integer wide;
    wide.set_value(wide_integer(5));
    EXPECT_EQ(narrow, wide);
    EXPECT_EQ(wide, narrow);
}

TEST(hdl_integer, wide_ordering) {
    hdl_integer a, b, c;
    a.set_value(wide_integer(1) << 100);
    b.set_value(wide_integer(1) << 99);
    c.set_value(wide_integer(1) << 101);
    EXPECT_TRUE(a > b);
    EXPECT_TRUE(a < c);
    EXPECT_TRUE(b < a);
}

TEST(hdl_integer, get_value_low64_of_wide) {
    hdl_integer w;
    w.set_value((wide_integer(1) << 100) + 5);
    EXPECT_EQ(w.get_value(), std::numeric_limits<int64_t>::max());
    EXPECT_EQ(w.to_wide(), (wide_integer(1) << 100) + 5);
    EXPECT_TRUE(w.is_wide());
}

TEST(hdl_integer, to_wide_of_narrow) {
    hdl_integer n = 42;
    EXPECT_EQ(n.to_wide(), wide_integer(42));
    EXPECT_FALSE(n.is_wide());
    EXPECT_EQ(n.get_value(), 42);
}

TEST(hdl_integer, wide_arithmetic_demotes) {
    hdl_integer w;
    w.set_value(wide_integer(1) << 100);
    hdl_integer w2;
    w2.set_value(wide_integer(1) << 100);
    auto sum = w + w2;          // 2^101, still wide
    EXPECT_TRUE(sum.is_wide());
    EXPECT_EQ(sum.to_wide(), wide_integer(1) << 101);

    hdl_integer one = 1;
    auto back = sum - w2;       // 2^100, still wide
    EXPECT_TRUE(back.is_wide());

    hdl_integer big;
    big.set_value((wide_integer(1) << 100) + 1);
    auto small = big - big;     // 0, should demote to narrow
    EXPECT_FALSE(small.is_wide());
    EXPECT_EQ(small.get_value(), 0);
}

TEST(hdl_integer, wide_to_string) {
    hdl_integer w;
    w.set_value(wide_integer(0xDEADBEEFCAFEBABE));
    std::string s = w.to_string();
    EXPECT_FALSE(s.empty());
    EXPECT_NE(s, "0");
}

TEST(hdl_integer, serialization_round_trip) {
    hdl_integer orig;
    orig.set_value((wide_integer(1) << 100) + 0xCAFE);
    orig.set_signed(true);
    orig.set_size(128);

    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oa(ss);
        oa(orig);
    }
    hdl_integer loaded;
    {
        cereal::BinaryInputArchive ia(ss);
        ia(loaded);
    }
    EXPECT_EQ(orig, loaded);
    EXPECT_TRUE(loaded.is_wide());
    EXPECT_EQ(loaded.to_wide(), (wide_integer(1) << 100) + 0xCAFE);
    EXPECT_EQ(loaded.get_signed(), true);
    EXPECT_EQ(loaded.get_size(), 128u);
}

TEST(resolved_parameter, serialization_preserves_undefined_flag) {
    resolved_parameter defined_val = resolved_parameter("SOME_STRING");
    EXPECT_FALSE(defined_val.is_undefined());

    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oa(ss);
        oa(defined_val);
    }
    resolved_parameter loaded;
    {
        cereal::BinaryInputArchive ia(ss);
        ia(loaded);
    }
    EXPECT_EQ(loaded, defined_val);
    EXPECT_FALSE(loaded.is_undefined());
    EXPECT_TRUE(loaded.is_string());
    EXPECT_EQ(loaded.get_string(), "SOME_STRING");

    resolved_parameter undefined_val;
    undefined_val.set_undefined();
    std::stringstream ss2;
    {
        cereal::BinaryOutputArchive oa(ss2);
        oa(undefined_val);
    }
    resolved_parameter loaded_undef;
    {
        cereal::BinaryInputArchive ia(ss2);
        ia(loaded_undef);
    }
    EXPECT_TRUE(loaded_undef.is_undefined());
}

TEST(resolved_parameter, serialization_preserves_undefined_flag_for_tokens) {
    String_token orig("\"FALSE\"");
    auto raw = std::static_pointer_cast<Expression_base>(std::make_shared<String_token>(orig));

    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oa(ss);
        oa(raw);
    }
    std::shared_ptr<Expression_base> loaded;
    {
        cereal::BinaryInputArchive ia(ss);
        ia(loaded);
    }
    ASSERT_TRUE(loaded->is<String_token>());
    auto val = loaded->evaluate({});
    ASSERT_TRUE(val.has_value());
    EXPECT_FALSE(val.value().is_undefined());
    EXPECT_TRUE(val.value().is_string());
}

TEST(hdl_integer, serialization_round_trip_negative_wide) {
    hdl_integer orig;
    orig.set_value(-((wide_integer(1) << 100) + 0xCAFE));
    orig.set_signed(true);

    std::stringstream ss;
    {
        cereal::BinaryOutputArchive oa(ss);
        oa(orig);
    }
    hdl_integer loaded;
    {
        cereal::BinaryInputArchive ia(ss);
        ia(loaded);
    }
    EXPECT_EQ(orig, loaded);
    EXPECT_TRUE(loaded.is_wide());
    EXPECT_EQ(loaded.to_wide(), -((wide_integer(1) << 100) + 0xCAFE));
}

TEST(hdl_integer, wide_division_and_modulo) {
    hdl_integer a, b;
    a.set_value(wide_integer(1) << 100);
    b.set_value(wide_integer(1) << 50);
    auto q = a / b;
    EXPECT_EQ(q.to_wide(), wide_integer(1) << 50);
    auto r = a % b;
    EXPECT_EQ(r.to_wide(), wide_integer(0));
}

TEST(hdl_integer, wide_shift) {
    hdl_integer w;
    w.set_value(wide_integer(1) << 100);
    auto shl = w << 10;
    EXPECT_EQ(shl.to_wide(), wide_integer(1) << 110);
    auto shr = w >> 10;
    EXPECT_EQ(shr.to_wide(), wide_integer(1) << 90);
}

TEST(hdl_integer, json_dump_shape) {
    hdl_integer n = 42;
    nlohmann::json jn = n;
    EXPECT_TRUE(jn.contains("value"));
    EXPECT_TRUE(jn.contains("size"));
    EXPECT_TRUE(jn.contains("signedness"));
    EXPECT_TRUE(jn.contains("wide_value"));
    EXPECT_TRUE(jn.contains("wide"));
    EXPECT_EQ(jn["value"], 42);
    EXPECT_EQ(jn["wide"], false);
    hdl_integer n2 = jn.get<hdl_integer>();
    EXPECT_EQ(n2, n);

    hdl_integer w;
    w.set_value(wide_integer(1) << 100);
    nlohmann::json jw = w;
    EXPECT_TRUE(jw.contains("value"));
    EXPECT_TRUE(jw.contains("wide_value"));
    EXPECT_TRUE(jw.contains("wide"));
    EXPECT_EQ(jw["wide"], true);
    hdl_integer w2 = jw.get<hdl_integer>();
    EXPECT_EQ(w2, w);
    EXPECT_TRUE(w2.is_wide());
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
    check_val.set_value(wide_integer("0xCAFEBEBEDEADBEEFCAFE"));
    check_val.set_signed(false);
    check.set_binary_size(72);
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
    check_val.set_value(wide_integer("0xCAFEBEBEDEADBEEFCAFE"));
    check_val.set_signed(false);
    check.set_binary_size(80);
    check.set_value(check_val);
    EXPECT_EQ(check, test_token);
}

TEST(Token, scientific_notation_real) {
    Real_token ec("1e3");
    auto val = ec.get_value();
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val.value().is_real());
    EXPECT_DOUBLE_EQ(val.value().get_real(), 1000.0);

    Real_token ec2("1.5e-2");
    auto val2 = ec2.get_value();
    ASSERT_TRUE(val2.value().is_real());
    EXPECT_DOUBLE_EQ(val2.value().get_real(), 1.5e-2);
}

TEST(Token, hex_e_not_scientific) {
    Numeric_token ec("'hE");
    auto val = ec.get_value();
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val.value().is_integer());
    EXPECT_EQ(val.value().get_integer(), 14);
}

TEST(Token, leading_minus_consistency) {
    Numeric_token a("-8'sd5");
    auto va = a.get_value();
    ASSERT_TRUE(va.has_value());
    EXPECT_EQ(va.value().get_integer().get_value(), -5);
    EXPECT_EQ(a.get_size(), 8);

    Numeric_token neg("-42");
    auto vneg = neg.get_value();
    ASSERT_TRUE(vneg.has_value());
    EXPECT_EQ(vneg.value().get_integer().get_value(), -42);
}

TEST(Token, partial_parse_rejected) {
    Numeric_token ec("8'b1011x");
    auto val = ec.get_value();
    ASSERT_TRUE(val.has_value());
    ASSERT_TRUE(val.value().is_integer());
    EXPECT_EQ(val.value().get_integer(), 0);
}

TEST(Token, power_of_two_sized_correctly) {
    Numeric_token ec("'d4");
    EXPECT_EQ(ec.get_size(), 3);
    Numeric_token ec2("'d8");
    EXPECT_EQ(ec2.get_size(), 4);
}

TEST(Token, width_aware_signed_unsigned_equality) {
    // Counter-example: -8'sd1 is -1 as an 8-bit value, i.e. 0xFF, so per IEEE
    // 1800 it must equal 8'hFF. The current to_wide()-only comparison (signed
    // -1 vs unsigned 255) makes this false, which is wrong.
    Numeric_token neg("-8'sd1");
    Numeric_token all_ones("8'hFF");
    EXPECT_TRUE(neg.get_value().value().get_integer() == all_ones.get_value().value().get_integer());
    EXPECT_FALSE(neg.get_value().value().get_integer() != all_ones.get_value().value().get_integer());
}
