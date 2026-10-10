//  Copyright 2025 Filippo Savi
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

#ifndef ANANKE_BACKEND_TYPES_HPP
#define ANANKE_BACKEND_TYPES_HPP

#include <map>
#include <set>
#include <string>
#include <vector>
#include <unordered_set>

struct script_source {
    std::string name;
    std::string path;
    bool function_mode = false;
    std::map<std::string, std::string> variables;

    bool operator==(const script_source& other) const {
        return path == other.path && variables == other.variables && name == other.name && function_mode == other.function_mode;
    }
};

struct project_data {
    std::string name;
    std::string repo_dir;
    std::vector<std::string> commons_dir;
    std::set<std::string> synth_sources;
    std::set<std::string> package_synth_sources;
    std::set<std::string> package_sim_sources;
    std::set<std::string> data_synth_sources;
    std::set<std::string> data_sim_sources;
    std::set<std::string> sim_sources;
    std::string synth_tl;
    // C++ simulation harness sources (Verilator --exe files). Entries may be
    // absolute or relative to the repository base; the generator resolves them.
    std::vector<std::string> sim_harness;
    // Explicit leading compile units (Depfile sim_defines override). Same path
    // convention as sim_harness. When non-empty the generator emits exactly
    // these first and skips automatic define-header resolution.
    std::vector<std::string> sim_defines;
    // Flow-resolved leading compile units (see resolve_leading_units):
    // standalone macro define-headers the closure needs but nothing includes.
    // Resolved absolute paths, set by the flow (never by hand).
    std::vector<std::string> header_units;
    std::string board_part;
    std::string target_part;
    std::string tb_tl;
    std::unordered_set<std::string> constraints_sources;
    std::vector<script_source> scripts;
};

#endif //ANANKE_BACKEND_TYPES_HPP