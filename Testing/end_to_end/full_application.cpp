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
}