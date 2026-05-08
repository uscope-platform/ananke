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

#include "data_model/Script.hpp"
#include <cereal/archives/binary.hpp>


TEST( Script_test , get_name) {
    script_specs s;
    s.name = "test";
    s.type = "tcl";
    Script cnstr(s);
    ASSERT_EQ(cnstr.get_name(), "test");
}

TEST( Script_test , path) {
    script_specs s;
    s.name = "test";
    s.type = "python";
    Script cnstr(s);
    cnstr.set_path("test_path");
    ASSERT_EQ(cnstr.get_path(), "test_path");
}


TEST( Script_test , ser_des_script) {

    script_specs s;
    s.name = "test";
    s.type = "tcl";
    Script scr_out(s);

    scr_out.set_path("/test/path");

    std::stringstream os;
    {
        cereal::BinaryOutputArchive archive_out(os);
        archive_out(scr_out);
    }

    std::string json_str = os.str();
    std::stringstream is(json_str);
    Script scr_in;
    cereal::BinaryInputArchive archive_in(is);
    archive_in(scr_in);
    ASSERT_EQ(scr_out, scr_in);

}

TEST( Script_test , string_arguments) {
    script_specs s;
    s.name = "test";
    s.type = "tcl";
    s.positional_arguments =  {"test_arg_1", "test_arg_2"};
    Script test_script(s);
    auto args = test_script.get_arguments();
    std::vector<std::pair<std::string, std::string>> results = {{"test_arg_1", ""}, {"test_arg_2",""}};

    ASSERT_THAT(args, testing::ContainerEq(results));
}

TEST( Script_test , named_arguments) {

    script_specs s;
    s.name = "test";
    s.type = "tcl";
    s.named_arguments.emplace_back("A", "1");
    s.named_arguments.emplace_back("B", "2");
    Script test_script(s);
    auto args = test_script.get_arguments();
    std::vector<std::pair<std::string, std::string>> results = {{"A", "1"}, {"B","2"}};

    ASSERT_THAT(args, testing::ContainerEq(results));
}



TEST( Script_test , get_type) {
    script_specs s;
    s.name = "test";
    s.type = "tcl";
    Script test_script(s);
    script_type_t type = test_script.get_type();

    ASSERT_EQ(type, tcl_script);
}


TEST( Script_test , uninit_script) {
    script_specs s;
    s.name = "test";
    s.type = "unknown";
    Script test_script(s);
    script_type_t type = test_script.get_type();

    ASSERT_EQ(type, uninit_script);
}


