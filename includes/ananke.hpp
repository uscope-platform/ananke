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



#ifndef ANANKE_ANANKE_HPP
#define ANANKE_ANANKE_HPP

#include <string>
#include <memory>
#include <vector>
#include <spdlog/spdlog.h>

#include "data_model/settings_store.hpp"
#include "data_model/data_store.hpp"
#include "Backend/new_app_generator.hpp"
#include "frontend/Repository_walker.hpp"
#include "frontend/analysis/sv_analyzer.hpp"
#include "Backend/python_script_runner.hpp"
#include "Backend/Auxiliary_resolver.hpp"
#include "data_model/Depfile.hpp"
#include "analysis/HDL_ast_builder_v2.hpp"
#include "Backend/Dependency_resolver.hpp"
#include "analysis/control_bus_analysis.hpp"
#include "analysis/proxy_bus_analysis.hpp"
#include "Backend/Xilinx/xilinx_project_generator.hpp"
#include "Backend/Lattice/lattice_project_generator.hpp"
#include "Backend/Lattice/Radiant_manager.hpp"
#include "Backend/Xilinx/Vivado_manager.hpp"
#include "Backend/uplatform/application_definition_generator.hpp"
#include "Backend/uplatform/peripheral_definition_generator.hpp"
#include "analysis/data_acquisition_analysis.hpp"

class ananke {
    public:

    struct CLI_opt{
        std::string target;
        bool generate_xilinx = false;
        bool generate_sim_script = false;
        bool generate_synth_script = false;
        bool generate_app_definition = false;
        bool generate_periph_definition = false;
        bool generate_lattice = false;
        bool synth_design = false;
        bool dump_ast = false;
        bool keep_makefile = false;
        std::string get_setting;
        std::string set_setting;
        std::string new_app_name;
        std::string new_app_lang;
        std::vector<std::string> parse_targets;
        bool measure_runtime = true;
        bool no_cache = false;
        bool trace = false;
        bool no_open = false;
        bool clear_cache = false;
        bool refresh_cache = false;
        std::string cache_dir = std::string(std::getenv("HOME")) + "/.ananke_store";
        bool wait_profiler = false;
    };

    explicit ananke(const CLI_opt &options);
    ~ananke();
    [[nodiscard]] std::optional<int> clear_cache() const;
    void set_settings() const;
    [[nodiscard]] std::optional<int> generate_new_app() const;
    [[nodiscard]] std::optional<int> directed_parsing() const;
    std::optional<int> load_data_cache();
    void build_flow();
private:
    std::shared_ptr<settings_store>  s_store;
    std::shared_ptr<data_store> d_store;
    CLI_opt opts;
    std::chrono::time_point<std::chrono::high_resolution_clock> t1;

};



#endif //ANANKE_ANANKE_HPP
