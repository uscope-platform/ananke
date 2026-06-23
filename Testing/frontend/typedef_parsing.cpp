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

#include "frontend/analysis/sv_analyzer.hpp"

TEST(typedef_parsing, mixed_packing_array) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef logic [31:0] ctrl_addr_init_t [1:0];
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto typedefs = resource.get_typedefs();
    EXPECT_TRUE(typedefs.contains("ctrl_addr_init_t"));

    EXPECT_EQ(typedefs["ctrl_addr_init_t"]->as<HDL_simple_type>().get_packed_dimensions().size(), 1);
    EXPECT_EQ(typedefs["ctrl_addr_init_t"]->as<HDL_simple_type>().get_unpacked_dimensions().size(), 1);

    dimension_t check_d;
    check_d.first_bound = std::make_shared<Token>("31", Token::number);
    check_d.second_bound = std::make_shared<Token>("0", Token::number);
    check_d.packed = true;
    auto packed_dim = typedefs["ctrl_addr_init_t"]->as<HDL_simple_type>().get_packed_dimensions()[0];
    EXPECT_EQ(packed_dim, check_d);

    check_d.first_bound = std::make_shared<Token>("1", Token::number);
    check_d.second_bound = std::make_shared<Token>("0", Token::number);
    check_d.packed = false;
    auto unpacked_dim = typedefs["ctrl_addr_init_t"]->as<HDL_simple_type>().get_unpacked_dimensions()[0];
    EXPECT_EQ(unpacked_dim, check_d);

}


TEST(typedef_parsing, basic_packed_struct_definition) {
    auto test_pattern = R"(
        package test_package;

            typedef struct packed {
                int unsigned field_1;
                int field_2;
            } test_struct;
        endpackage
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto structs = resource.get_typedefs();
    EXPECT_TRUE(structs.contains("test_struct"));
    HDL_struct_type check_struct;
    check_struct.packed = true;
    struct_member m;
    m.name = "field_1";
    auto t1 = Type_engine::create_primitive_type("int");
    t1->as<HDL_simple_type>().set_signed(false);
    m.type = t1;
    check_struct.member.emplace_back(m);
    m.name = "field_2";
    auto t2 = Type_engine::create_primitive_type("int");
    m.type = t2;
    check_struct.member.emplace_back(m);
    auto result_struct = structs.at("test_struct");
    EXPECT_EQ(check_struct,result_struct->as<HDL_struct_type>());

}


TEST(typedef_parsing, basic_unpacked_struct_definition) {
    auto test_pattern = R"(
        package test_package;

            typedef struct {
                int unsigned field_1;
                int field_2;
            } test_struct;
        endpackage
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto structs = resource.get_typedefs();
    EXPECT_TRUE(structs.contains("test_struct"));
    HDL_struct_type check_struct;
    check_struct.packed = false;
    struct_member m;
    m.name = "field_1";
    auto t1 = Type_engine::create_primitive_type("int");
    t1->as<HDL_simple_type>().set_signed(false);
    m.type = t1;
    check_struct.member.emplace_back(m);
    m.name = "field_2";
    auto t2 = Type_engine::create_primitive_type("int");
    m.type = t2;
    check_struct.member.emplace_back(m);
    auto result_struct = structs.at("test_struct");
    EXPECT_EQ(check_struct,result_struct->as<HDL_struct_type>());

}



TEST(typedef_parsing, bits_in_struct_definition) {
    auto test_pattern = R"(
        package test_package;

            typedef struct packed {
                int unsigned field_1;
                bit field_2;
            } test_struct;
        endpackage
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto structs = resource.get_typedefs();
    EXPECT_TRUE(structs.contains("test_struct"));
    HDL_struct_type check_struct;
    check_struct.packed = true;
    struct_member m;
    m.name = "field_1";
    auto t1 = Type_engine::create_primitive_type("int");
    t1->as<HDL_simple_type>().set_signed(false);
    m.type = t1;
    check_struct.member.emplace_back(m);
    m = {};
    m.name = "field_2";
    m.type = std::make_shared<HDL_simple_type>();
    check_struct.member.emplace_back(m);
    auto result_struct = structs.at("test_struct");
    EXPECT_EQ(check_struct,result_struct->as<HDL_struct_type>());

}




TEST(typedef_parsing, struct_with_unpacked_array_of_packed) {
    auto test_pattern = R"(
        package test_package;

            typedef struct packed {
                bit [3:0] field_1 [1:0];
            } test_struct;
        endpackage
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto structs = resource.get_typedefs();
    EXPECT_TRUE(structs.contains("test_struct"));
    HDL_struct_type check_struct;
    check_struct.packed = true;
    struct_member m;
    m.name = "field_1";
    HDL_simple_type t;
    t.set_packed_dimensions({
        {
            std::make_shared<Token>(3, 2),
            std::make_shared<Token>(0, 1),
            true
        }});
    t.set_unpacked_dimensions({
        {std::make_shared<Token>(1, 1), std::make_shared<Token>(0, 1), false}
    });
    m.type = std::make_shared<HDL_simple_type>(t);
    check_struct.member.emplace_back(m);
    auto result_struct = structs.at("test_struct");
    EXPECT_EQ(check_struct,result_struct->as<HDL_struct_type>());

}



TEST(typedef_parsing, anonymous_simple_struct) {
    auto test_pattern = R"(
        package test_package;

            struct {
                int unsigned field_1;
                int field_2;
            } test_struct;
        endpackage
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

}