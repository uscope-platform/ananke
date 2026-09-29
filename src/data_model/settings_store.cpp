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

settings_store::settings_store(bool e, std::string cache_dir_path, std::string profile) {
    store_path = std::move(cache_dir_path);
    std::error_code ec;
    std::filesystem::create_directories(store_path, ec);
    if (ec) {
        spdlog::warn("Could not create settings directory {}: {}", store_path, ec.message());
    }
    ephemeral = e;
    settings_file = store_path + "/settings";
    selected_profile = std::move(profile);
    if(!ephemeral){
        if(!std::filesystem::exists(settings_file) ) {
            std::ofstream ofs(settings_file);
            ofs << "{}";
            ofs.flush();
            ofs.close();
        }
        load_settings(settings_file);
    }
}

std::filesystem::path settings_store::get_hdl_store() {
    if (profiles[selected_profile].hdl_store.empty()) {
        std::string target_repository;
        spdlog::info("Please enter the absolute path of the HDL repository");
        std::cin >> target_repository;
        profiles[selected_profile].hdl_store = target_repository;
    }

    return profiles[selected_profile].hdl_store;
}

std::filesystem::path settings_store::get_tool_path(const std::string &tool) {

    if (!tool_paths.contains(tool)) {
        std::string target_repository;
        spdlog::info("Please enter the absolute path of the {} installation", tool);
        std::cin >> target_repository;
        tool_paths[tool]  = target_repository;
    }

    return tool_paths[tool];
}

std::string settings_store::get_selected_profile() const {
    return selected_profile;
}

std::set<std::string> settings_store::get_default_includes() {

    std::set<std::string> raw_includes;

    for (const auto& item : profiles[selected_profile].includes) {
        raw_includes.insert(profiles[selected_profile].hdl_store / item);
    }
    return  raw_includes;
}

std::set<std::string> settings_store::get_excluded_paths() {

    std::set<std::string> raw_includes;

    for (const auto& item : profiles[selected_profile].excludes) {
        raw_includes.insert(profiles[selected_profile].hdl_store / std::filesystem::path(item));
    }
    return  raw_includes;
}

std::set<std::string> settings_store::get_defines() {
    return profiles[selected_profile].defines;
}

bool settings_store::get_include_auto_discovery() {
    return profiles[selected_profile].include_auto_discovery;
}

settings_store::~settings_store() {
    if(!ephemeral){
        flush();
    }
}

void settings_store::load_settings(const std::string  &settings_file) {
    std::ifstream ifs(settings_file);
    nlohmann::json settings;
    try {
        settings = nlohmann::json::parse(ifs);
    } catch (const std::exception &e) {
        spdlog::warn("Could not parse settings file {} ({}), using defaults", settings_file, e.what());
        return;
    }

    if (settings.contains("profiles")) {
        for (auto &[key, value]: settings["profiles"].items()) {
            if (value.contains("hdl_store"))
                profiles[key].hdl_store = std::filesystem::path(value["hdl_store"]);
            if (value.contains("default_includes"))
                profiles[key].includes = value["default_includes"];
            if (value.contains("excluded_paths"))
                profiles[key].excludes = value["excluded_paths"];
            if (value.contains("defines"))
                profiles[key].defines = value["defines"];
            if (value.contains("include_auto_discovery"))
                profiles[key].include_auto_discovery = value["include_auto_discovery"];
        }
    }
    if (settings.contains("amd_vivado_path"))
        tool_paths["amd_vivado"] = std::filesystem::path(settings["amd_vivado_path"]);
    if (settings.contains("lattice_radiant_path"))
        tool_paths["lattice_radiant"] = std::filesystem::path(settings["lattice_radiant_path"]);

    if (settings.contains("default_profile") && selected_profile.empty()) selected_profile = settings["default_profile"];
}

void settings_store::flush() {
    try {
        std::ofstream ofs(settings_file);
        if (!ofs.good()) {
            spdlog::error("Could not open settings file {} for writing", settings_file);
            return;
        }
        nlohmann::json settings;

        for (const auto& [key, profile] : profiles) {
            settings["profiles"][key]["hdl_store"] = profile.hdl_store.string();
            settings["profiles"][key]["default_includes"] = profile.includes;
            settings["profiles"][key]["defines"] = profile.defines;
            settings["profiles"][key]["include_auto_discovery"] = profile.include_auto_discovery;
        }
        for (auto &[tool_name, path]: tool_paths) {
            settings[tool_name + "_path"] = path;
        }
        settings["default_profile"] = selected_profile;
        ofs << settings.dump(4);
    } catch (const std::exception &e) {
        spdlog::error("Could not save settings file {}: {}", settings_file, e.what());
    }
}



