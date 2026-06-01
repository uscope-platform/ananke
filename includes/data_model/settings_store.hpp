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

#ifndef ANANKE_SETTINGS_STORE_HPP
#define ANANKE_SETTINGS_STORE_HPP


#include <filesystem>
#include <nlohmann/json.hpp>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <spdlog/spdlog.h>
#include <iostream>

class settings_store {
public:
    settings_store(bool e, std::string cache_dir_path);

    std::filesystem::path get_hdl_store();
    std::filesystem::path get_tool_path(const std::string &tool);
    std::set<std::string> get_default_includes();
    std::set<std::string> get_excluded_paths();
    void remove_setting(const std::string& setting);
    void flush();
    ~settings_store();
private:

    nlohmann::json settings_backend;

    bool ephemeral;
    std::string store_path;
    std::string settings_file;
};


#endif //ANANKE_SETTINGS_STORE_HPP
