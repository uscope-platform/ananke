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
#include "data_model/Depfile/Depfile.hpp"


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
    SetUp("Depfile");

    ASSERT_EQ(file->general.project_name, "test_project_name");
    ASSERT_EQ(file->general.synth_tl, "test_synth_tl");
    ASSERT_EQ(file->general.sim_tl, "test_sim_tl");
    ASSERT_THAT(file->general.include_paths,
                testing::ElementsAre("test_include_1","test_include_2"));
    ASSERT_THAT(file->general.sim_modules, testing::ElementsAre("test_sim_module_add_1", "test_sim_module_add_2"));
    ASSERT_THAT(file->general.synth_modules, testing::ElementsAre("test_synth_module_add_1", "test_synth_module_add_2"));


}


TEST_F( DepfileTest , Depfile_excluded_modules) {
    SetUp("Depfile");

    ASSERT_THAT(file->excluded_modules,
                testing::ElementsAre( "test_excluded_module_1", "test_excluded_module_2"));

}


TEST_F( DepfileTest , Depfile_constraints) {
    SetUp("Depfile");
    std::vector<Constraints> correct_answer;
    correct_answer.emplace_back("test_constraints_1");
    correct_answer.emplace_back("test_constraints_2");
    ASSERT_THAT(file->constraints, testing::ContainerEq(correct_answer));
}


TEST_F( DepfileTest , Depfile_scripts) {
    SetUp("Depfile");
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