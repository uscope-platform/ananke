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
#include "data_model/settings_store.hpp"

#include <utility>

settings_store::settings_store(bool e, std::string cache_dir_path) {
    store_path = std::move(cache_dir_path);
    std::filesystem::create_directory(store_path);
    ephemeral = e;
    settings_file = store_path + "/settings";

    if(!ephemeral){
        if(!std::filesystem::exists(settings_file) ) {
            std::ofstream ofs(settings_file);
            ofs << "{}";
            ofs.flush();
            ofs.close();
        }
        std::ifstream ifs(settings_file);
        settings_backend =  nlohmann::json::parse( ifs );
    }
}

std::filesystem::path settings_store::get_hdl_store() {
    if (!settings_backend.contains("hdl_store")) {
        std::string target_repository;
        spdlog::info("Please enter the absolute path of the HDL repository");
        std::cin >> target_repository;
        settings_backend["hdl_store"]  = target_repository;
    }

    return settings_backend["hdl_store"];
}

std::filesystem::path settings_store::get_tool_path(const std::string &tool) {
    if (!settings_backend.contains(tool + "_path")) {
        std::string target_repository;
        spdlog::info("Please enter the absolute path of the {} installation", tool);
        std::cin >> target_repository;
        settings_backend[tool + "_path"]  = target_repository;
    }

    return settings_backend[tool + "_path"];
}

/*
std::optional<std::string> get_path(const std::string &setting) {
    std::string ret_val = settings_backend[setting];
    if (ret_val.ends_with("/")) ret_val.replace(ret_val.size()-1, 1, "");
    return ret_val;
}

std::set<std::string> get_setting_list(const std::string &setting) const{
    if (!settings_backend.contains(setting)) return {};
    return settings_backend[setting];
}
*/

settings_store::~settings_store() {
    if(!ephemeral){
        flush();
    }

}

void settings_store::remove_setting(const std::string &setting) {
    settings_backend.erase(setting);
}

void settings_store::flush() {
    std::ofstream ofs(settings_file);
    ofs << settings_backend.dump();
}



