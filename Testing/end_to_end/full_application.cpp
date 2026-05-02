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
    ofs << "test";
    ofs.flush();
    ofs = std::ofstream(opts.cache_dir + "/unified_cache");
    ananke uut(opts);
    int i = 0;
}