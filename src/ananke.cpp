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

#include "ananke.hpp"

ananke::ananke(const CLI_opt &options) {
    opts = options;

    if(opts.trace) {
        spdlog::set_level(spdlog::level::trace);
    } else {
        spdlog::set_level(spdlog::level::info);
    }

    if(opts.wait_profiler) {
        spdlog::info("Press ANY key to continue");
        getchar();
    }

    s_store = std::make_shared<settings_store>(false, opts.cache_dir, opts.profile);

    t1 = std::chrono::high_resolution_clock::now();
}

ananke::~ananke() {
    if(opts.measure_runtime){
        auto t2 = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> ms_double = t2 - t1;
        spdlog::info("The Program runtime was : {} ms", std::to_string(ms_double.count()));
    }
}

std::optional<int> ananke::clear_cache() const {
    if(opts.clear_cache) {
        data_store::clear_cache(opts.cache_dir);
        s_store->flush();
        return 0;
    }
    return std::nullopt;
}

std::optional<int> ananke::generate_new_app() const {
    if(!opts.new_app_name.empty()){
        std::string lang = "sv";
        if(!opts.new_app_lang.empty()){
            if(opts.new_app_lang == "sv"|| opts.new_app_lang == "vhdl") {
                lang = opts.new_app_lang;
            } else {
                spdlog::error("Unknown language option: ", opts.new_app_lang);
                return 1;
            }
        }
        new_app_generator gen(opts.new_app_name, lang);
        return 0;
    }
    return std::nullopt;
}

std::expected<std::unordered_map<std::string, std::string>, int> ananke::directed_parsing() const {
    if (!opts.parse_targets.empty()) {
        for (auto &target :opts.parse_targets) {
            if (!std::filesystem::exists(target)) {
                std::cout << "Target: " << target << " not found" << std::endl;
                return std::unexpected(50);
            }
            try {
                sv_analyzer analyzer;
                mm_file file(target);
                auto resources = analyzer.analyze(target, file.view());
                std::unordered_map<std::string, std::string> res_map;
                for (auto &res:resources) {
                    res_map.insert({res.getName(), res.get_path()});
                }
                return res_map;
            } catch (std::runtime_error &err) {
                std::cout << err.what();
                return std::unexpected(51);
            }

        }
    }
    return {};
}

std::optional<int> ananke::load_data_cache() {

    if (opts.refresh_cache) data_store::clear_cache(opts.cache_dir);
    d_store = std::make_shared<data_store>(opts.no_cache, opts.cache_dir);

    // analyze repository content and update cache
    Repository_walker walker(s_store, d_store, opts.no_cache, s_store->get_excluded_paths());

    if(opts.refresh_cache) {
       return 0;
    }
    return std::nullopt;
}

std::optional<int> ananke::build_flow() {

    // Parse depfile
    if(opts.target.empty()) opts.target = std::filesystem::current_path().string() + "/Depfile";
    if(std::filesystem::exists(opts.target)) {
        std::ifstream ifs(opts.target);
        Depfile dep(ifs);

        // Resolve auxiliary files (scripts and constraints)
        Auxiliary_resolver aux_resolver(d_store);


        auto scripts = dep.get_scripts();

        python_script_runner py_runner;
        py_runner.run_python_scripts(aux_resolver.get_python_objects(scripts));
        auto script_deps = aux_resolver.get_tcl_script_paths(scripts);

        auto additional_script_deps = py_runner.get_script_dependencies();
        script_deps.insert(script_deps.end(), additional_script_deps.begin(), additional_script_deps.end());

        std::set<std::string> additional_constr_deps = py_runner.get_constraints_dependencies();
        std::unordered_set<std::string> constr_deps = aux_resolver.get_constraints(dep.constraints);
        constr_deps.insert(additional_constr_deps.begin(), additional_constr_deps.end());

        LOG_TIMEPOINT("Scripts & constraints resolved");

        // BUILD ASTs FOR TOP LEVEL AND ADDITIONAL MODULES
        HDL_ast_builder_v2 b(s_store, d_store, dep);
        auto synth_ast = b.build_ast(std::vector({dep.general.synth_tl}))[0];
        auto additional_synth_modules = b.build_ast(dep.general.synth_modules);
        additional_synth_modules.insert(additional_synth_modules.end(), synth_ast);

        LOG_TIMEPOINT("Build synth AST");

        auto sim_ast = b.build_ast(std::vector({dep.general.sim_tl}))[0];
        auto additional_sim_modules = b.build_ast(dep.general.sim_modules);
        additional_sim_modules.insert(additional_sim_modules.end(), sim_ast);

        LOG_TIMEPOINT("Build sim AST");

        // RESOLVE DEPENDENCIES
        Dependency_resolver_v2 synth_r(additional_synth_modules, d_store);
        auto synth_sources = synth_r.get_dependencies();
        auto synth_packages = synth_r.get_packages();
        auto synth_data = synth_r.get_data();

        LOG_TIMEPOINT("Solved synth dependencies");

        Dependency_resolver_v2 sim_r(additional_sim_modules, d_store);
        auto sim_sources = sim_r.get_dependencies();
        auto sim_packages = sim_r.get_packages();
        auto sim_data = sim_r.get_data();

        LOG_TIMEPOINT("Solved sim dependencies");

        // BUS MAPPING

        control_bus_analysis bus_analyzer(dep);
        bus_analyzer.analyze_bus(synth_ast);
        LOG_TIMEPOINT("control bus analysis");

        proxy_bus_analysis proxy_analyzer(s_store, d_store, dep);
        proxy_analyzer.analyze(synth_ast);

        LOG_TIMEPOINT("proxy bus analysis");
        //Generate makefile
        if(opts.generate_xilinx){

            xilinx_project_generator generator(s_store);

            project_data data;

            data.name = dep.general.project_name;
            data.synth_sources = synth_sources;
            data.package_synth_sources = synth_packages;
            data.data_synth_sources = synth_data;
            data.sim_sources = sim_sources;
            data.package_sim_sources = sim_packages;
            data.data_sim_sources = sim_data;
            data.scripts = script_deps;
            data.constraints_sources = constr_deps;
            data.tb_tl = dep.general.sim_tl;
            data.synth_tl = dep.general.synth_tl;
            data.commons_dir = dep.general.include_paths;
            data.repo_dir = std::filesystem::current_path();
            if (dep.general.target_part) {
                data.target_part = dep.general.target_part.value();
            } else if(dep.general.board){
                data.board_part = dep.general.board.value();
            } else {
                return 76;
            }

            generator.set_data(data);


            if (opts.generate_sim_script) {
                std::ofstream simfile("sim.sh");
                generator.generate_sim_script(simfile);
                if (!std::filesystem::exists("sim.tcl")) {
                    std::ofstream ofs("sim.tcl");

                    generator.write_sim_control_script(ofs);
                }
            } else if (opts.generate_synth_script) {
                std::ofstream simfile("synth.tcl");
                generator.generate_synth_script(simfile);
            } else {
                std::ofstream makefile("makefile.tcl");
                generator.write_makefile(makefile);


                Vivado_manager manager(s_store, !opts.keep_makefile, dep.general.project_name);

                LOG_TIMEPOINT("Build script generated");
                if (!opts.makefile_only) manager.create_project("makefile.tcl",  !opts.no_open);
            }
        }

        if(opts.generate_lattice){
            lattice_project_generator generator(s_store);
            project_data data;

            data.name = dep.general.project_name;
            data.synth_sources = synth_sources;
            data.package_synth_sources = synth_packages;
            data.data_synth_sources = synth_data;
            data.sim_sources = sim_sources;
            data.package_sim_sources = sim_packages;
            data.data_sim_sources = sim_data;
            data.scripts = script_deps;
            data.constraints_sources = constr_deps;
            data.tb_tl = dep.general.sim_tl;
            data.synth_tl = dep.general.synth_tl;

            std::ofstream makefile("makefile.tcl");
            generator.write_makefile(makefile);

            Radiant_manager manager(s_store, !opts.keep_makefile, dep.general.project_name);
            LOG_TIMEPOINT("Build script generated");
            if (!opts.makefile_only) manager.create_project("makefile.tcl",  !opts.no_open);
        }
        if (opts.generate_periph_definition || opts.generate_app_definition) {
            peripheral_definition_generator periph_def_gen(d_store, synth_ast);
            if(opts.generate_periph_definition){
                periph_def_gen.write_definition_file(dep.general.project_name + "_periph_def.json");
            }
            LOG_TIMEPOINT("peripheral definition generation");

            if(opts.generate_app_definition){
                application_definition_generator app_def_gen(
                synth_ast,
                periph_def_gen.get_peripheral_definitions(),
                periph_def_gen.get_alias_map(),
                periph_def_gen.get_variant_peripherals()
                );

                LOG_TIMEPOINT("Application definition generation");
                data_acquisition_analysis daq_analyzer(true);
                daq_analyzer.analyze(synth_ast);
                app_def_gen.add_datapoints(daq_analyzer.get_datapoints());
                app_def_gen.add_channel_groups(daq_analyzer.get_channel_groups());
                app_def_gen.add_scope(daq_analyzer.get_scope_data());

                LOG_TIMEPOINT("Data acquisition analysis");
                app_def_gen.construct_application(dep.general.project_name);

                app_def_gen.write_definition_file(dep.general.project_name + "_app_def.json");
            }
        }

        if (opts.dump_ast) {
            auto ast_dump = synth_ast->dump().dump(4);
            std::ofstream ast_file("/tmp/ast.json");
            ast_file << ast_dump;
            ast_file.close();
        }
    }
    return 0;
}
