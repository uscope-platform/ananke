// Copyright 2021 Filippo Savi
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

#include "Backend/Toolchain_manager.hpp"
#include "Backend/Xilinx/Vivado_manager.hpp"


std::string tcm_settings_path = "/tmp/test_settings_store";
auto settings_file = tcm_settings_path + "/settings";

std::shared_ptr<settings_store> tcm_setup_settings() {
    std::filesystem::create_directories(tcm_settings_path);
    std::ofstream ofs(settings_file);
    ofs << "{\"amd_vivado_path\":\"/dev/null\"}";
    ofs.flush();
    ofs.close();
    std::error_code ec;
    std::filesystem::resize_file(settings_file, std::filesystem::file_size(settings_file, ec), ec);
    return std::make_shared<settings_store>(false, tcm_settings_path);
}

void tcm_clean_settings() {
    std::filesystem::remove_all(settings_file);
    std::filesystem::remove_all(tcm_settings_path);
}


TEST(Toolchain_manager, str_vect_to_char_p){

    std::vector<std::string> test_vect = {"s1", "s2", "s3"};
    std::vector<const char*>  result = Toolchain_manager::str_vect_to_char_p(test_vect);

    ASSERT_TRUE(result.size()==test_vect.size()+1);
    ASSERT_TRUE(result[test_vect.size()]== nullptr);
    for (int i = 0; i < test_vect.size(); ++i) {
        ASSERT_TRUE(strcmp(result[i],test_vect[i].c_str()) == 0 );
    }
}



TEST(Toolchain_manager, vivado_manager){
    std::shared_ptr<settings_store> s_store = tcm_setup_settings();
    Vivado_manager v(s_store, true, "test");
    std::vector<std::string> result = v.prepare_call("test_project");
    std::vector<std::string> check = {"/dev/null/bin/vivado", "-mode", "batch", "-nolog", "-nojournal", "-source", "test_project" };
    EXPECT_EQ(result, check);
    tcm_clean_settings();
}
