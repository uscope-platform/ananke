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

    EXPECT_EQ(typedefs["ctrl_addr_init_t"].get_packed_dimensions().size(), 1);
    EXPECT_EQ(typedefs["ctrl_addr_init_t"].get_unpacked_dimensions().size(), 1);

    dimension_t check_d;
    check_d.first_bound = {Expression_component("31", Expression_component::number)};
    check_d.second_bound = {Expression_component("0", Expression_component::number)};
    check_d.packed = true;
    auto packed_dim = typedefs["ctrl_addr_init_t"].get_packed_dimensions()[0];
    EXPECT_EQ(packed_dim, check_d);

    check_d.first_bound = {Expression_component("1", Expression_component::number)};
    check_d.second_bound = {Expression_component("0", Expression_component::number)};
    check_d.packed = false;
    auto unpacked_dim = typedefs["ctrl_addr_init_t"].get_unpacked_dimensions()[0];
    EXPECT_EQ(unpacked_dim, check_d);

}


TEST(typedef_parsing, struct_definition) {
    auto test_pattern = R"(
        package test_package;

            typedef struct packed {
                int unsigned field_1;
                int unsigned field_2;

            } test_struct;
        endpackage
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    EXPECT_FALSE(true);
}