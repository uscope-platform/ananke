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

// Tool-specific backend options: the `verilator` Depfile section. Only the
// Verilator backend consumes it. Member initializers are the defaults used
// when the section (or a key) is absent; an explicit empty list disables
// that default (e.g. `"waivers": []` passes no -Wno- flags).
struct verilator_tool_options {
    // Extra compiler flags for the generated-model build, joined after -CFLAGS.
    std::vector<std::string> cflags = {"-std=c++14"};
    // Verilator warning codes waived as -Wno-<code>.
    std::vector<std::string> waivers = {"UNOPTFLAT"};
    // Extra arguments appended to the model `make` invocation.
    std::vector<std::string> make_args = {"OPT_FAST=-Os"};
    // Pass --autoflush to Verilator.
    bool autoflush = true;
    // Free-form extra Verilator arguments, one element per argv entry,
    // inserted before the file list.
    std::vector<std::string> extra_args = {};
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
    // Files `included by closure files (absolute paths). The Verilator
    // backend does not emit these positionally: Verilator compiles all
    // inputs as one unit, so an included file would be defined twice
    // (MODDUP, fatal). Its content stays visible via the include.
    std::set<std::string> included_sources;
    // Referenced data images with no repository file ($readmem literals as
    // written, e.g. toolchain-generated "program.hex"). The backend checks
    // these exist in the run directory instead of hanging the sim.
    std::vector<std::string> missing_data_files;
    // Verilator backend options (Depfile `verilator` section, else defaults).
    verilator_tool_options verilator;
    std::string board_part;
    std::string target_part;
    std::string tb_tl;
    std::unordered_set<std::string> constraints_sources;
    std::vector<script_source> scripts;
};

#endif //ANANKE_BACKEND_TYPES_HPP