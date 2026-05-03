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

#include "ananke.hpp"


TEST( end_to_end , clear_cache) {


    ananke::CLI_opt opts;
    opts.clear_cache = true;
    opts.cache_dir = "/tmp/ananke_test_cache";
    std::filesystem::create_directory(opts.cache_dir);
    std::ofstream ofs(opts.cache_dir + "/settings");
    ofs << "cache_dump,/home/vivado/hdl/public/Applications/uscope_testing/tb/uscope_testing_tb.sv>32:c5:3b:35:ee:4b:fb:aa:d9:b1:5c:a2:94:a4:2d:e4:ee:26:aa:ff:e0:45:9e:f3:63:86:42:d4:f2:4c:6e:03;" << std::endl;
    ofs << "hdl_store,/home/vivado/hdl" << std::endl;
    ofs.flush();
    ofs = std::ofstream(opts.cache_dir + "/unified_cache");
    ofs << "test2";
    ofs.flush();

    ananke uut(opts);
    auto rc = uut.clear_cache();
    EXPECT_EQ(rc, 0);
    std::ifstream ifs(opts.cache_dir + "/settings");
    std::string settings;
    ifs >> settings;
    EXPECT_EQ(settings, "hdl_store,/home/vivado/hdl");
    std::filesystem::remove_all(opts.cache_dir);
}



TEST( end_to_end , set_settings) {


    ananke::CLI_opt opts;
    opts.set_setting = "test_setting=52";
    opts.cache_dir = "/tmp/ananke_test_cache";
    std::filesystem::create_directory(opts.cache_dir);
    std::ofstream ofs(opts.cache_dir + "/settings");

    ofs.flush();
    ofs = std::ofstream(opts.cache_dir + "/unified_cache");
    ofs << "test2";
    ofs.flush();

    ananke uut(opts);
    uut.set_settings();

    std::ifstream ifs(opts.cache_dir + "/settings");
    std::string settings;
    ifs >> settings;
    EXPECT_EQ(settings, "test_setting,52");
    std::filesystem::remove_all(opts.cache_dir);
}




TEST( end_to_end , get_setting) {

    ananke::CLI_opt opts;
    opts.get_setting = "test_setting";
    opts.cache_dir = "/tmp/ananke_test_cache";
    std::filesystem::create_directory(opts.cache_dir);
    std::ofstream ofs(opts.cache_dir + "/settings");
    ofs << "test_setting,76" << std::endl;
    ofs.flush();
    ofs = std::ofstream(opts.cache_dir + "/unified_cache");
    ofs << "test2";
    ofs.flush();
    testing::internal::CaptureStdout();
    ananke uut(opts);
    uut.get_settings();
    std::string result = testing::internal::GetCapturedStdout();

    EXPECT_EQ(result, "76\n");
    std::filesystem::remove_all(opts.cache_dir);
}



TEST( end_to_end , new_sv_application) {


    ananke::CLI_opt opts;
    opts.no_cache = true;

    opts.new_app_name = "test_app";
    opts.new_app_lang = "sv";

    std::string test_dir = "/tmp/test_generate_app";
    auto wd = std::filesystem::current_path();
    std::filesystem::create_directory(test_dir);
    std::filesystem::current_path(test_dir);

    ananke uut(opts);
    auto rc = uut.generate_new_app();
    std::filesystem::current_path(wd);

    auto app_dir = test_dir + "/test_app";
    EXPECT_EQ(rc, 0);


    EXPECT_TRUE(std::filesystem::exists(app_dir));
    EXPECT_TRUE(std::filesystem::is_directory(app_dir));
    EXPECT_TRUE(std::filesystem::exists(app_dir + "/Depfile"));

    std::ifstream ifs(app_dir + "/Depfile");
    std::string result;
    ifs >> result;
    EXPECT_EQ(result, "{\"constraints\":[\"test_app.xdc\"],\"excluded_modules\":[],\"general\":{\"include_paths\":[\"/public/Components/Common\"],\"project_name\":\"test_app\",\"sim_modules\":[],\"sim_tl\":\"test_app_tb\",\"synth_modules\":[],\"synth_tl\":\"test_app\",\"target_part\":\"xc7z020clg400\"},\"scripts\":[]}");

    EXPECT_TRUE(std::filesystem::exists(app_dir + "/rtl"));
    EXPECT_TRUE(std::filesystem::is_directory(app_dir + "/rtl"));
    EXPECT_TRUE(std::filesystem::exists(app_dir + "/rtl/test_app.sv"));

    ifs = std::ifstream(app_dir + "/rtl/test_app.sv");
    std::stringstream ss;
    ss << ifs.rdbuf();
    EXPECT_EQ(ss.str(), "`timescale 10ns / 1ns\n\nmodule test_app (\n);\n\nendmodule");


    EXPECT_TRUE(std::filesystem::exists(app_dir + "/tb"));
    EXPECT_TRUE(std::filesystem::exists(app_dir + "/tb/test_app_tb.sv"));
    ss = std::stringstream();
    ifs = std::ifstream(app_dir + "/tb/test_app_tb.sv");
    ss << ifs.rdbuf();
    EXPECT_EQ(ss.str(), "`timescale 10ns / 1ns\n\nmodule test_app_tb ();\n\n\nendmodule");

    EXPECT_TRUE(std::filesystem::is_directory(app_dir + "/tb"));
    EXPECT_TRUE(std::filesystem::exists(app_dir + "/test_app.xdc"));


    std::filesystem::remove_all(app_dir);
}

TEST( end_to_end , directed_parsing) {


    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.parse_targets = {"check_files/test_data/Components/controls/PID/rtl/PID.sv"};


    ananke uut(opts);
    auto rc = uut.directed_parsing();

    EXPECT_EQ(rc, 0);

}


TEST( end_to_end , directed_parsing_file_not_found) {


    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.parse_targets = {"check_files/test_data/Components/controls/PID/rtl/PID.sv.error"};


    ananke uut(opts);
    auto rc = uut.directed_parsing();

    EXPECT_EQ(rc, 50);

}


TEST( end_to_end , directed_parsing_preprocessor_error) {


    auto test_file = "/tmp/test_preproc.sv";
    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.parse_targets = {test_file};

    std::ofstream ofs(test_file);

    ofs << "`define ADD(a, b) a+b\nmodule test_module();\n integer a = `ADD();\nendmodule\n";
    ofs.close();

    ananke uut(opts);
    auto rc = uut.directed_parsing();

    EXPECT_EQ(rc, 51);
    std::filesystem::remove(test_file);
}