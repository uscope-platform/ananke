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

#include "main.hpp"

struct MainProfiler {
    MainProfiler() { LOG_TIMEPOINT("Application Start"); }
    ~MainProfiler() { LOG_TIMEPOINT("Application end"); }
};

int main(int argc, char *argv[]){

    // Command line interface

    ananke::CLI_opt opts;

    CLI::App app("Ananke FPGA build system version 2.0");

    app.add_option("-D,--depfile", opts.target, "Target Depfile")->check(CLI::ExistingFile);
    app.add_flag("--X",opts.generate_xilinx,"Generate Xilinx Makefile");
    app.add_flag("--A",opts.generate_app_definition,"Generate application definition file");
    app.add_flag("--P",opts.generate_periph_definition,"Generate Peripherals definition file");
    app.add_flag("--L",opts.generate_lattice,"Generate Lattice Makefile");
    app.add_flag("--S",opts.synth_design,"synthetize design");
    app.add_flag("--k",opts.keep_makefile, "if set to true does not remove the makefile after use");
    app.add_option("--get_setting",opts.get_setting, "Print the value of a cached user setting");
    app.add_option("--set_setting",opts.set_setting, "Modify the value of a cached user setting");
    app.add_option("--new_app",opts.new_app_name, "Setup a new blank application");
    app.add_option("--lang",opts.new_app_lang, "Specify the language for the new app");
    app.add_flag("--measure-runtime",opts.measure_runtime, "Measure the runtime of the current program invocation");
    app.add_flag("--no-cache",opts.no_cache, "Run the program without touching the repository cache");
    app.add_flag("--refresh-cache",opts.refresh_cache, "Refresh the repository cache and exit");
    app.add_flag("--clear-cache",opts.clear_cache, "Clear HDL cache");
    app.add_flag("--wait_profiler", opts.wait_profiler, "Wait for the profiler to be ready before executing");
    app.add_flag("--trace", opts.trace, "Enable extended internal state tracing");
    app.add_option("--cache_dir", opts.cache_dir, "Specify a non-default repository cache file");
    app.add_flag("--no_open", opts.no_open, "Do not open the generated project");
    app.add_flag("--makefile_only", opts.makefile_only, "Only generate the setup script and not the project");
    app.add_flag("--sim_script", opts.generate_sim_script, "Generate simulation script for the chosen platform");
    app.add_flag("--synth_script", opts.generate_synth_script, "Generate Synthesis script for the chosen platform");
    app.add_flag("--dump_ast", opts.dump_ast, "Dump AST to /tmp/ast.json");
    app.add_option("--parse",opts.parse_targets, "Specify a list of files to parse");

    CLI11_PARSE(app, argc, argv);

    MainProfiler profiler;

    ananke engine(opts);
    std::optional<int> return_code = engine.clear_cache();
    if (return_code.has_value()) return return_code.value();
    engine.set_settings();
    engine.get_settings();
    return_code = engine.generate_new_app();
    if (return_code.has_value()) return return_code.value();
    auto parsing_result = engine.directed_parsing();
    if (!parsing_result) return parsing_result.error();

    engine.load_data_cache();
    LOG_TIMEPOINT("Cache loaded");
    return_code = engine.build_flow();
    if (return_code.has_value()) return return_code.value();

    return 0;
}