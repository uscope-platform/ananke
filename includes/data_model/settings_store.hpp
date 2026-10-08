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
#include <optional>
#include <string>
#include <sstream>
#include <fstream>
#include <set>
#include <map>
#include <spdlog/spdlog.h>
#include <iostream>

#include "data_model/parse_options.hpp"

struct settings_profile {
    std::filesystem::path hdl_store;
    std::set<std::string> includes;
    std::set<std::string> excludes;
    std::set<std::string> defines;
    bool include_auto_discovery = true;
    // Strip undefined UVM/OVM methodology macros instead of erroring on them.
    // Off by default: strict mode treats them like any other undefined macro
    // (quarantine, then terminal error if nothing in the repo defines them).
    bool strip_uvm_macros = false;
    // Strict mode (see parse_options): reject non-compliant syntax instead
    // of applying the lenient workarounds. Off by default.
    bool strict = false;
};

class settings_store {
public:
    settings_store(bool e, std::string cache_dir_path, std::string profile);

    std::filesystem::path get_hdl_store();
    std::filesystem::path get_tool_path(const std::string &tool);
    std::string get_selected_profile() const;
    std::set<std::string> get_default_includes();
    std::set<std::string> get_excluded_paths();
    std::set<std::string> get_defines();
    bool get_include_auto_discovery();
    bool get_strip_uvm_macros();
    bool get_strict();
    // CLI runtime override for strict mode (e.g. --strict): applies to this
    // invocation only and is never persisted by flush().
    void set_strict_override(bool strict);
    parse_options get_parse_options();
    void flush();
    ~settings_store();
private:

    void load_settings(const std::string  &settings);

    std::map<std::string, settings_profile> profiles;
    std::map<std::string, std::filesystem::path> tool_paths;
    std::string selected_profile;
    std::optional<bool> strict_override;

    bool ephemeral;
    std::string store_path;
    std::string settings_file;
};


#endif //ANANKE_SETTINGS_STORE_HPP
