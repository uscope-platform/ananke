// Copyright 2021 University of Nottingham Ningbo China
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <sstream>

#include "data_model/Constraints.hpp"
#include "data_model/Script.hpp"
#include "data_model/Depfile/Depfile.hpp"

#include "test_paths.hpp"


class DepfileTest : public ::testing::Test {
protected:
    void SetUp(const std::string &filename) {
        auto stream = std::ifstream(filename);
        file = new Depfile(stream);
    }

    virtual void TearDown() {
        delete file;
    }

    Depfile *file;
};


TEST_F( DepfileTest , general_section_parsing) {
    SetUp(td_file("Depfile"));

    ASSERT_EQ(file->general.project_name, "test_project_name");
    ASSERT_EQ(file->general.synth_tl, "test_synth_tl");
    ASSERT_EQ(file->general.sim_tl, "test_sim_tl");
    ASSERT_THAT(file->general.include_paths,
                testing::ElementsAre("test_include_1","test_include_2"));
    ASSERT_THAT(file->general.sim_modules, testing::ElementsAre("test_sim_module_add_1", "test_sim_module_add_2"));
    ASSERT_THAT(file->general.synth_modules, testing::ElementsAre("test_synth_module_add_1", "test_synth_module_add_2"));
    ASSERT_TRUE(file->general.sim_harness.empty());
    ASSERT_TRUE(file->general.sim_defines.empty());


}


TEST_F( DepfileTest , Depfile_excluded_modules) {
    SetUp(td_file("Depfile"));

    ASSERT_THAT(file->excluded_modules,
                testing::ElementsAre( "test_excluded_module_1", "test_excluded_module_2"));

}


TEST_F( DepfileTest , Depfile_constraints) {
    SetUp(td_file("Depfile"));
    std::vector<Constraints> correct_answer;
    correct_answer.emplace_back("test_constraints_1");
    correct_answer.emplace_back("test_constraints_2");
    ASSERT_THAT(file->constraints, testing::ContainerEq(correct_answer));
}


TEST_F( DepfileTest , Depfile_scripts) {
    SetUp(td_file("Depfile"));
    std::vector<Script> correct_answer;
    script_specs s;

    s.name = "test_script.tcl";
    s.type = "tcl";
    correct_answer.emplace_back(s);

    s.name = "test_script.py";
    s.type = "py";
    s.positional_arguments = {"B"};
    correct_answer.emplace_back(s);

    s.name = "test_script.py";
    s.type = "py";
    s.positional_arguments = {"A"};
    correct_answer.emplace_back(s);

    s.name = "test_script_args.tcl";
    s.type = "tcl";
    s.named_arguments.emplace_back("A", "1");
    s.named_arguments.emplace_back("B", "2");
    correct_answer.emplace_back(s);


    auto res = file->get_scripts();
    ASSERT_EQ(res.size(), correct_answer.size());
    for (int i = 0; i<res.size(); i++)
        EXPECT_EQ(res[i], correct_answer[i]);
}

TEST( DepfileHarnessTest , sim_harness_parsing) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "harness_proj",
            "synth_tl": "dut",
            "sim_tl": "tb_top",
            "sim_harness": ["testbench/test_tb_top.cpp", "/abs/harness.cpp"],
            "sim_defines": ["snapshots/default/common_defines.vh"]
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    ASSERT_THAT(f.general.sim_harness,
                testing::ElementsAre("testbench/test_tb_top.cpp", "/abs/harness.cpp"));
    ASSERT_THAT(f.general.sim_defines,
                testing::ElementsAre("snapshots/default/common_defines.vh"));
}

TEST(DepfileVerilatorTest, tool_section_parsing) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        },
        "verilator": {
            "cflags": ["-std=c++17", "-O2"],
            "waivers": [],
            "make_args": ["OPT_FAST=-O2", "VERBOSE=1"],
            "autoflush": false,
            "extra_args": ["--x-initial", "fast"]
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    ASSERT_THAT(f.verilator.cflags, testing::ElementsAre("-std=c++17", "-O2"));
    ASSERT_TRUE(f.verilator.waivers.empty());
    ASSERT_THAT(f.verilator.make_args, testing::ElementsAre("OPT_FAST=-O2", "VERBOSE=1"));
    ASSERT_FALSE(f.verilator.autoflush);
    ASSERT_THAT(f.verilator.extra_args, testing::ElementsAre("--x-initial", "fast"));
}

TEST(DepfileVerilatorTest, tool_section_defaults) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    ASSERT_THAT(f.verilator.cflags, testing::ElementsAre("-std=c++14"));
    ASSERT_THAT(f.verilator.waivers, testing::ElementsAre("UNOPTFLAT"));
    ASSERT_THAT(f.verilator.make_args, testing::ElementsAre("OPT_FAST=-Os"));
    ASSERT_TRUE(f.verilator.autoflush);
    ASSERT_TRUE(f.verilator.extra_args.empty());
}