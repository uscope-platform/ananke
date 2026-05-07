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

#include "data_model/Constraints.hpp"
#include "data_model/Script.hpp"
#include "data_model/Depfile.hpp"


class DepfileTest : public ::testing::Test {
protected:
    void SetUp(const std::string &filename) {
        file = new Depfile(filename);
    }

    virtual void TearDown() {
        delete file;
    }

    Depfile *file;
};

TEST_F( DepfileTest , Depfile_project_name) {
    SetUp("Depfile");

    ASSERT_EQ(file->get_project_name(), "test_project_name");

}

TEST_F( DepfileTest , Depfile_additional_synth_modules) {
    SetUp("Depfile");

    ASSERT_THAT(file->get_additional_synth_modules(), testing::ElementsAre("test_synth_module_add_1", "test_synth_module_add_2"));

}

TEST_F( DepfileTest , Depfile_additional_sim_modules) {
    SetUp("Depfile");

    ASSERT_THAT(file->get_additional_sim_modules(), testing::ElementsAre("test_sim_module_add_1", "test_sim_module_add_2"));

}


TEST_F( DepfileTest , Depfile_synth_top) {
    SetUp("Depfile");

    ASSERT_EQ(file->get_synth_tl(), "test_synth_tl");

}

TEST_F( DepfileTest , Depfile_sim_top) {
    SetUp("Depfile");
    ASSERT_EQ(file->get_sim_tl(), "test_sim_tl");

}


TEST_F( DepfileTest , Depfile_excluded_modules) {
    SetUp("Depfile");

    ASSERT_THAT(file->get_excluded_modules(),
                testing::ElementsAre( "test_excluded_module_1", "test_excluded_module_2"));

}

TEST_F( DepfileTest , Depfile_include_paths) {
    SetUp("Depfile");

    ASSERT_THAT(file->get_include_directories(),
                testing::ElementsAre("test_include_1","test_include_2"));

}

TEST_F( DepfileTest , Depfile_constraints) {
    SetUp("Depfile");
    std::vector<Constraints> correct_answer;
    correct_answer.emplace_back("test_constraints_1");
    correct_answer.emplace_back("test_constraints_2");
    ASSERT_THAT(file->get_constraints(), testing::ContainerEq(correct_answer));
}


TEST_F( DepfileTest , Depfile_scripts) {
    SetUp("Depfile");
    std::vector<Script> correct_answer;
    correct_answer.emplace_back("test_script.tcl", "tcl");
    correct_answer.emplace_back("test_script.py", "py");
    correct_answer.emplace_back("test_script.py", "py");
    correct_answer.emplace_back("test_script_args.tcl", "tcl");

    auto arg_s = std::vector<std::string>({"B"});
    correct_answer[1].set_arguments(arg_s);

    arg_s =std::vector<std::string>({"A"});
    correct_answer[2].set_arguments(arg_s);

    nlohmann::json p1;
    p1["name"] = "A";
    p1["value"] ="1";
    nlohmann::json p2;
    p2["name"] = "B";
    p2["value"] ="2";

    auto arguments = std::vector({p1, p2});
    correct_answer[3].set_arguments(arguments);

    auto res = file->get_scripts();
    ASSERT_THAT(res, testing::ContainerEq(correct_answer));
}