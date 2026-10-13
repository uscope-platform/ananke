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
#include "data_model/include_config.hpp"

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

std::filesystem::path settings_store::get_tool_path_or(const std::string &tool, const std::string &fallback) const {
    auto it = tool_paths.find(tool);
    if (it != tool_paths.end()) return it->second;
    return std::filesystem::path(fallback);
}

bool settings_store::has_tool(const std::string &tool) const {
    return tool_paths.contains(tool);
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

std::set<std::string> settings_store::get_ignored_includes() {
    return profiles[selected_profile].ignored_includes;
}

std::unordered_map<std::string, include_header_resolution_opt> settings_store::get_include_deconfliction() {
    return profiles[selected_profile].include_deconfliction;
}

bool settings_store::get_include_auto_discovery() {
    return profiles[selected_profile].include_auto_discovery;
}

bool settings_store::get_strip_uvm_macros() {
    return profiles[selected_profile].strip_uvm_macros;
}

bool settings_store::get_strict() {
    if (strict_override.has_value()) return strict_override.value();
    return profiles[selected_profile].strict;
}

void settings_store::set_strict_override(bool strict) {
    strict_override = strict;
}

parse_options settings_store::get_parse_options() {
    parse_options opts;
    opts.include_directories = get_default_includes();
    opts.defines = get_defines();
    opts.strip_uvm_macros = get_strip_uvm_macros();
    opts.strict = get_strict();
    opts.ignored_includes = get_ignored_includes();
    opts.include_deconfliction = get_include_deconfliction();
    if (ephemeral_includes_set) {
        for (const auto &s : ephemeral_ignored) opts.ignored_includes.insert(s);
        for (const auto &kv : ephemeral_decon) {
            auto &dst = opts.include_deconfliction[kv.first];
            if (kv.second.global_target.has_value()) dst.global_target = kv.second.global_target;
            for (const auto &sv : kv.second.scoped_targets) {
                dst.scoped_targets[sv.first] = sv.second;
            }
        }
    }
    if (ephemeral_defines_set) {
        // Depfile dictionary wins per macro name over the profile list:
        // drop any profile entry for the same name ("NAME" or "NAME=..."),
        // then insert the Depfile "NAME=value" form.
        for (const auto &[name, value] : ephemeral_defines) {
            for (auto it = opts.defines.begin(); it != opts.defines.end();) {
                const std::string &e = *it;
                if (e == name || (e.size() > name.size() && e.compare(0, name.size(), name) == 0 &&
                                  e[name.size()] == '=')) {
                    it = opts.defines.erase(it);
                } else {
                    ++it;
                }
            }
            opts.defines.insert(name + "=" + value);
        }
    }
    std::error_code ec;
    auto abs_root = std::filesystem::absolute(profiles[selected_profile].hdl_store, ec);
    if (!ec) opts.repo_root = abs_root.lexically_normal().string();
    else opts.repo_root = profiles[selected_profile].hdl_store.string();
    return opts;
}

void settings_store::set_ephemeral_include_config(std::set<std::string> ignored,
    std::unordered_map<std::string, include_header_resolution_opt> decon) {
    ephemeral_ignored = std::move(ignored);
    ephemeral_decon = std::move(decon);
    ephemeral_includes_set = true;
}

bool settings_store::has_ephemeral_include_config() const {
    return ephemeral_includes_set;
}

void settings_store::set_ephemeral_defines(std::unordered_map<std::string, std::string> defines) {
    ephemeral_defines = std::move(defines);
    ephemeral_defines_set = true;
}

bool settings_store::has_ephemeral_defines() const {
    return ephemeral_defines_set;
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
            if (value.contains("strip_uvm_macros"))
                profiles[key].strip_uvm_macros = value["strip_uvm_macros"];
            if (value.contains("strict"))
                profiles[key].strict = value["strict"];
            if (value.contains("ignored_includes"))
                profiles[key].ignored_includes = value["ignored_includes"].get<std::set<std::string>>();
            if (value.contains("include_deconfliction") && value["include_deconfliction"].is_object()) {
                for (auto &[hkey, hval] : value["include_deconfliction"].items()) {
                    include_header_resolution_opt res;
                    if (hval.is_string()) {
                        res.global_target = hval.get<std::string>();
                    } else if (hval.is_object()) {
                        for (auto &[scope, target] : hval.items()) {
                            res.scoped_targets.emplace(
                                include_config::normalize_repo_path(scope),
                                target.get<std::string>());
                        }
                    }
                    profiles[key].include_deconfliction.emplace(
                        include_config::normalize_repo_path(hkey), std::move(res));
                }
            }
        }
    }
    if (settings.is_object()) for (auto &[key, value] : settings.items()) {
        // Any "<tool>_path" entry configures a tool installation
        // (amd_vivado_path, lattice_radiant_path, verilator_path, ...).
        if (key.ends_with("_path") && value.is_string()) {
            tool_paths[key.substr(0, key.size() - 5)] = std::filesystem::path(value.get<std::string>());
        }
    }

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
            settings["profiles"][key]["strip_uvm_macros"] = profile.strip_uvm_macros;
            settings["profiles"][key]["strict"] = profile.strict;
            if (!profile.ignored_includes.empty())
                settings["profiles"][key]["ignored_includes"] = profile.ignored_includes;
            if (!profile.include_deconfliction.empty()) {
                nlohmann::json decon = nlohmann::json::object();
                for (const auto &[hkey, res] : profile.include_deconfliction) {
                    if (!res.scoped_targets.empty() && !res.global_target.has_value()) {
                        nlohmann::json scoped = nlohmann::json::object();
                        for (const auto &[scope, target] : res.scoped_targets) scoped[scope] = target;
                        decon[hkey] = std::move(scoped);
                    } else if (res.global_target.has_value() && res.scoped_targets.empty()) {
                        decon[hkey] = *res.global_target;
                    } else if (res.global_target.has_value()) {
                        // Mixed form is not representable in the flat profile
                        // schema: keep the scoped map (the common case) and
                        // drop the global rather than serializing garbage.
                        nlohmann::json scoped = nlohmann::json::object();
                        for (const auto &[scope, target] : res.scoped_targets) scoped[scope] = target;
                        decon[hkey] = std::move(scoped);
                    }
                }
                settings["profiles"][key]["include_deconfliction"] = std::move(decon);
            }
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



