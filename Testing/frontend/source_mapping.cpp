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

#include "frontend/analysis/system_verilog/preprocessor/sv_preprocessor.hpp"


using namespace preprocessor;


TEST(source_mapping, simple_include) {
    auto test_pattern = R"(
        `include "/tmp/include_test.svh"
        module test_module ();
            parameter TEST_PARAM = 6;
        endmodule
    )";

    std::ofstream ofs("/tmp/include_test.svh");
    if (!ofs.is_open()) {
        std::cerr << "CRITICAL ERROR: Could not create the file in /tmp!" << std::endl;
    }
    ofs<< "module test_module_2 ();\n wire test_wire;\nassign test_wire = 5;\nendmodule;"<< std::endl;

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto result_map = preproc.get_source_map();
    std::filesystem::remove_all("/tmp/include_test.svh");
    source_map_t reference_map = {
        {1, 1,"/tmp/file.sv"},
        {2, 5, "/tmp/include_test.svh"},
        {6, 9, "/tmp/file.sv"}
    };
    EXPECT_EQ(result_map, reference_map);

    EXPECT_EQ(source_mapper::get_path(1, result_map),"/tmp/file.sv");
    EXPECT_EQ(source_mapper::get_path(2, result_map),"/tmp/include_test.svh");
    EXPECT_EQ(source_mapper::get_path(9, result_map),"/tmp/file.sv");
    EXPECT_EQ(source_mapper::get_path(15, result_map),std::nullopt);
}



TEST(source_mapping, nested_includes) {
    auto test_pattern = R"(
        `include "/tmp/include_test.svh"
        module test_module ();
            parameter TEST_PARAM = 6;
        endmodule
    )";

    std::ofstream ofs("/tmp/include_test.svh");

    ofs<< "\n\n`include \"/tmp/include_nested.svh\"\n\n\nmodule test_module_2 ();\n wire test_wire;\nassign test_wire = 5;\nendmodule;";
    ofs.close();

    ofs = std::ofstream("/tmp/include_nested.svh");

    ofs<< "module test_module_3 ();\n wire test_wire;\nassign test_wire = 4;\nendmodule;";
    ofs.close();

    sv_preprocessor preproc;
    preproc.set_path("/tmp/file.sv");

    auto result = preproc.preprocess(test_pattern);
    auto result_map = preproc.get_source_map();
    std::filesystem::remove_all("/tmp/include_test.svh");
    source_map_t reference_map = {
        {1, 1,"/tmp/file.sv"},
        {2, 3, "/tmp/include_test.svh"},
        {4, 7, "/tmp/include_nested.svh"},
        {8, 13, "/tmp/include_test.svh"},
        {14, 17, "/tmp/file.sv"}
    };
    EXPECT_EQ(result_map, reference_map);

    EXPECT_EQ(source_mapper::get_path(1, result_map),"/tmp/file.sv");
    EXPECT_EQ(source_mapper::get_path(2, result_map),"/tmp/include_test.svh");
    EXPECT_EQ(source_mapper::get_path(5, result_map),"/tmp/include_nested.svh");
    EXPECT_EQ(source_mapper::get_path(9, result_map),"/tmp/include_test.svh");
    EXPECT_EQ(source_mapper::get_path(15, result_map),"/tmp/file.sv");
}