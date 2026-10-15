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
    s_store->set_strict_override(opts.strict);
    load_early_depfile();

    t1 = std::chrono::high_resolution_clock::now();
}

std::optional<std::string> ananke::resolve_depfile_candidate(
    const std::string &explicit_target,
    const std::filesystem::path &cwd,
    const std::optional<std::filesystem::path> &hdl_store) {
    std::error_code ec;
    auto absolute_normalized = [](const std::filesystem::path &p) {
        return std::filesystem::absolute(p).lexically_normal().string();
    };
    if (!explicit_target.empty()) {
        std::filesystem::path target(explicit_target);
        if (!target.is_absolute()) target = cwd / target;
        if (std::filesystem::exists(target, ec)) return absolute_normalized(target);
        ec.clear();
    }
    auto cwd_dep = cwd / "Depfile";
    if (std::filesystem::exists(cwd_dep, ec)) return absolute_normalized(cwd_dep);
    ec.clear();
    if (hdl_store.has_value()) {
        auto root_dep = hdl_store.value() / "Depfile";
        if (std::filesystem::exists(root_dep, ec)) return absolute_normalized(root_dep);
    }
    return std::nullopt;
}

void ananke::load_early_depfile() {
    // Parse the whole Depfile once, up front: its `includes`/`defines`
    // sections steer the repository walk below, and build_flow reuses the
    // held object instead of re-reading the file. Only explicit/CWD roots
    // are considered here (hdl_store may still be unconfigured, and looking
    // it up would prompt); load_data_cache adds that fallback.
    std::error_code ec;
    auto cwd = std::filesystem::current_path(ec);
    if (ec) return;
    auto candidate = resolve_depfile_candidate(opts.target, cwd, std::nullopt);
    if (candidate.has_value()) load_early_depfile_from(candidate.value());
}

void ananke::load_early_depfile_from(const std::string &candidate) {
    std::ifstream ifs(candidate);
    if (!ifs.good()) return;
    Depfile dep(ifs);
    early_depfile_path_ = candidate;
    if (dep.has_error()) {
        // Deferred as a hard error to build_flow, but say so NOW: otherwise
        // a typo'd Depfile silently contributes empty includes/defines to
        // the whole walk (e.g. all ignores stop working) with no indication
        // until project generation — or never, on refresh-only runs.
        early_depfile_error_ = dep.get_error();
        spdlog::warn("Depfile {} is invalid ({}); analysis continues without its "
                     "includes/defines sections", candidate, early_depfile_error_);
    }
    populate_analysis_overlays(dep);
    early_depfile_ = std::move(dep);
    spdlog::debug("Loaded Depfile {}", candidate);
}

void ananke::populate_analysis_overlays(const Depfile &dep) {
    if (!dep.includes.ignored.empty() || !dep.includes.deconfliction.empty()) {
        std::set<std::string> ignored(dep.includes.ignored.begin(), dep.includes.ignored.end());
        s_store->set_ephemeral_include_config(std::move(ignored), dep.includes.deconfliction);
    }
    if (!dep.defines.empty()) {
        s_store->set_ephemeral_defines(dep.defines);
    }
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
        // Explicit --profile clears just that profile, keeping the others;
        // bare --clear-cache keeps the legacy clear-everything behavior.
        if (!opts.profile.empty()) {
            data_store::clear_cache(opts.cache_dir, opts.profile);
        } else {
            data_store::clear_cache(opts.cache_dir);
        }
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
    if (opts.parse_targets.empty()) {
        return {};
    }
    if (opts.parse_mode != "preprocessing" && opts.parse_mode != "parsing" &&
        opts.parse_mode != "elaboration") {
        std::cout << "Invalid --parse-mode: " << opts.parse_mode
                  << " (expected preprocessing, parsing or elaboration)" << std::endl;
        return std::unexpected(54);
    }
    // Stateless per-invocation options: profile/depfile overlays provide the
    // base (strict flag, profile defines), CLI args merge on top so a single
    // sv-tests invocation needs no Depfile, cache or settings edit.
    parse_options base_opts = s_store->get_parse_options();
    for (auto &d : opts.parse_incdirs) base_opts.include_directories.insert(d);
    for (auto &d : opts.parse_defines) base_opts.defines.insert(d);
    base_opts.disable_semantic_checks = opts.no_semantic_checks;
    base_opts.explicit_file_mode = true;

    std::unordered_map<std::string, std::string> res_map;
    for (auto &target : opts.parse_targets) {
        if (!std::filesystem::exists(target)) {
            std::cout << "Target: " << target << " not found" << std::endl;
            return std::unexpected(50);
        }
        try {
            sv_analyzer analyzer;
            auto f_opt = mm_file::try_open(target);
            if (!f_opt.has_value()) {
                std::cout << "Target: " << target << " not readable" << std::endl;
                return std::unexpected(51);
            }
            analyzer.set_options(base_opts);
            if (opts.parse_mode == "preprocessing") {
                analyzer.preprocess(target, f_opt->view());
                if (analyzer.has_error()) {
                    std::cout << "Error preprocessing target: " << target << ": " << analyzer.get_error()
                              << std::endl;
                    return std::unexpected(52);
                }
                res_map.insert({target, target});
                continue;
            }
            auto analysis_result = analyzer.analyze(target, f_opt->view());
            if (!analysis_result.has_value()) {
                std::cout << "Error parsing target: " << target << ": " << analyzer.get_error() << std::endl;
                // The exit code is owned by the checker: rule violations
                // report their own code, anything else keeps 52.
                const auto &violations = analyzer.get_semantic_violations();
                if (!violations.empty()) {
                    return std::unexpected(
                        sv_semantic_checker::exit_code_for(violations.front().code));
                }
                return std::unexpected(52);
            }
            for (auto &res : analysis_result.value().get_content()) {
                std::string statement_id;
                if (res->is<hdl_resource_statement>()) statement_id = res->as<hdl_resource_statement>().getName();
                else if (res->is<hdl_package_statement>()) statement_id = res->as<hdl_package_statement>().getName();
                else if (res->is<hdl_interface_statement>()) statement_id = res->as<hdl_interface_statement>().getName();
                else if (res->is<hdl_function_statement>()) statement_id = res->as<hdl_function_statement>().get_name();
                if (!statement_id.empty()) res_map.insert({statement_id, target});
            }
        } catch (std::runtime_error &err) {
            std::cout << err.what();
            return std::unexpected(53);
        }
    }
    if (opts.parse_mode == "elaboration" && !opts.parse_top.empty() &&
        !res_map.contains(opts.parse_top)) {
        std::cout << "Top module: " << opts.parse_top << " not found in parsed targets" << std::endl;
        return std::unexpected(55);
    }
    return res_map;
}

std::optional<int> ananke::load_data_cache() {

    {
        auto join = [](const auto &items) {
            std::string out;
            bool first = true;
            for (const auto &item : items) {
                if (!first) out += ", ";
                first = false;
                out += item;
            }
            if (out.empty()) out = "<none>";
            return out;
        };
        spdlog::trace("Cache refresh starting:\n"
                      "  profile='{}'\n"
                      "  hdl_store='{}'\n"
                      "  cache_dir='{}'\n"
                      "  no_cache={}\n"
                      "  refresh_cache={}\n"
                      "  include_auto_discovery={}\n"
                      "  default_includes=[{}]\n"
                      "  excluded_paths=[{}]\n"
                      "  defines=[{}]\n"
                      "  ignored_includes=[{}]",
                      s_store->get_selected_profile(),
                      s_store->get_hdl_store().string(),
                      opts.cache_dir,
                      opts.no_cache,
                      opts.refresh_cache,
                      s_store->get_include_auto_discovery(),
                      join(s_store->get_default_includes()),
                      join(s_store->get_excluded_paths()),
                      join(s_store->get_defines()),
                      join(s_store->get_ignored_includes()));
    }

    // Refresh is per-profile: other profiles' data stays intact. The walker
    // below then re-parses this profile's repository from scratch.
    if (opts.refresh_cache) data_store::clear_cache(opts.cache_dir, s_store->get_selected_profile());
    d_store = std::make_shared<data_store>(opts.no_cache, opts.cache_dir, s_store->get_selected_profile());

    // hdl_store fallback for the early Depfile: if no explicit/CWD Depfile
    // was loaded in the constructor, try one at the repository root now that
    // the path is known (get_hdl_store() was already called for the trace
    // above, so this never prompts). A found file anywhere blocks the other
    // roots: there is exactly one effective Depfile.
    if (!early_depfile_.has_value()) {
        auto candidate = resolve_depfile_candidate(
            opts.target, std::filesystem::current_path(), s_store->get_hdl_store());
        if (candidate.has_value()) load_early_depfile_from(candidate.value());
    }

    // analyze repository content and update cache
    Repository_walker walker(s_store, d_store, opts.no_cache, s_store->get_excluded_paths());

    if(opts.refresh_cache) {
       return 0;
    }
    return std::nullopt;
}

std::optional<int> ananke::build_flow() {

    // Reuse the startup parse when it addresses the same file; otherwise
    // (nothing found then, or an exotic flow) fall back to reading it here.
    // A stored validation error surfaces exactly as before (error 60).
    if(opts.target.empty()) opts.target = std::filesystem::current_path().string() + "/Depfile";
    std::optional<Depfile> fresh_dep;
    const Depfile *dep_ptr = nullptr;
    {
        std::error_code ec;
        const std::string effective =
            std::filesystem::absolute(opts.target, ec).lexically_normal().string();
        if (!ec && early_depfile_.has_value() && effective == early_depfile_path_) {
            if (!early_depfile_error_.empty()) {
                spdlog::error("Invalid Depfile {}: {}", opts.target, early_depfile_error_);
                return 60;
            }
            dep_ptr = &early_depfile_.value();
        } else if (std::filesystem::exists(opts.target, ec)) {
            std::ifstream ifs(opts.target);
            fresh_dep.emplace(ifs);
            if (fresh_dep->has_error()) {
                spdlog::error("Invalid Depfile {}: {}", opts.target, fresh_dep->get_error());
                return 60;
            }
            dep_ptr = &fresh_dep.value();
        }
    }
    if(dep_ptr != nullptr) {
        const Depfile &dep = *dep_ptr;

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

        //TODO: DIFFERENTIATE THE SYNTH AND SIM TB BUILDING STRATEGY, NOT ALL PASSES ARE RELEVANT ON SIMS
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

        // Collect the include directories that were auto-discovered (only) by
        // files in the project's dependency closure. The discovered header
        // paths are cached per file; their parent directories are derived here
        // so the generated project carries exactly the include paths it needs.
        std::set<std::string> discovered_dirs;
        auto collect_discovered = [&](const std::set<std::string> &sources) {
            for (auto &src: sources) {
                if (auto inc = d_store->get_includes(src); inc.has_value()) {
                    for (auto &dep: inc.value()) {
                        if (dep.resolution == include_resolution::auto_discovered) {
                            discovered_dirs.insert(std::filesystem::path(dep.path).parent_path().string());
                        }
                    }
                }
            }
        };
        collect_discovered(synth_sources);
        collect_discovered(synth_packages);
        collect_discovered(sim_sources);
        collect_discovered(sim_packages);

        std::vector<std::string> commons_dir = dep.general.include_paths;
        for (auto &dir: discovered_dirs) {
            commons_dir.push_back("/" + std::filesystem::relative(dir, s_store->get_hdl_store()).string());
        }

        LOG_TIMEPOINT("control bus analysis");

        control_bus_analysis bus_analyzer(dep);
        bus_analyzer.analyze_bus(synth_ast);

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
            data.commons_dir = commons_dir;
            data.repo_dir = std::filesystem::current_path();
            if (dep.general.board) {
                data.board_part = dep.general.board.value();
            } else if(dep.general.target_part){
                data.target_part = dep.general.target_part.value();
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


                LOG_TIMEPOINT("Build script generated");
                if (!opts.makefile_only) {
                    Vivado_manager manager(s_store, !opts.keep_makefile, dep.general.project_name);
                    manager.create_project("makefile.tcl",  !opts.no_open);
                }
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
            data.commons_dir = commons_dir;
            data.repo_dir = std::filesystem::current_path();
            if (dep.general.target_part) {
                data.target_part = dep.general.target_part.value();
            } else if(dep.general.board){
                data.board_part = dep.general.board.value();
            } else {
                return 76;
            }

            generator.set_data(data);

            if (opts.generate_synth_script) {
                std::ofstream synthfile("synth.tcl");
                generator.generate_synth_script(synthfile);
            } else {
                std::ofstream makefile("makefile.tcl");
                generator.write_makefile(makefile);

                LOG_TIMEPOINT("Build script generated");
                if (!opts.makefile_only) {
                    Radiant_manager manager(s_store, !opts.keep_makefile, dep.general.project_name);
                    manager.create_project("makefile.tcl",  !opts.no_open);
                }
            }
        }
        if(opts.generate_verilator){
            verilator_project_generator generator(s_store);
            project_data data;

            data.name = dep.general.project_name;
            data.synth_sources = synth_sources;
            data.package_synth_sources = synth_packages;
            data.data_synth_sources = synth_data;
            data.sim_sources = sim_sources;
            data.package_sim_sources = sim_packages;
            data.data_sim_sources = sim_data;
            data.tb_tl = dep.general.sim_tl;
            data.synth_tl = dep.general.synth_tl;
            data.sim_harness = dep.general.sim_harness;
            data.sim_defines = dep.general.sim_defines;
            data.verilator = dep.verilator;
            data.commons_dir = commons_dir;
            data.repo_dir = std::filesystem::current_path();
            // No target_part/board required: Verilator is device-independent.

            // Verilator evaluates `define directives in command-line order, so
            // files whose macro definitions the closure consumed (via
            // injection) but which reach the command line no other way are
            // resolved from the persisted macro harvests and passed
            // positionally first. See resolve_leading_units for the policy:
            // unique verdicts only, profile globals need no file, true
            // headers stay on +incdir+. Only edges from closure files count —
            // out-of-closure files (e.g. VeeR's pd_defines.vh, in no flist
            // and hence in no closure) must not reclassify a unit-header.
            {
                std::unordered_map<std::string, std::vector<stored_macro_def>> all_defs =
                    d_store->get_all_macro_definitions();
                std::unordered_map<std::string, std::vector<std::string>> macro_needs, includes;
                std::set<std::string> closure;
                for (auto *set : {&synth_sources, &synth_packages, &sim_sources, &sim_packages}) {
                    for (auto &src : *set) {
                        closure.insert(src);
                        macro_needs[src] = d_store->get_macro_dependencies(src);
                        if (auto inc = d_store->get_includes(src); inc.has_value())
                            for (auto &dep : inc.value()) includes[src].push_back(dep.path);
                    }
                }
                std::set<std::string> profile_names;
                for (const auto &d : s_store->get_defines())
                    profile_names.insert(d.substr(0, d.find('=')));
                data.header_units = verilator_project_generator::resolve_leading_units(
                    all_defs, macro_needs, includes, closure, profile_names);
                // Files included by closure files are compiled via the
                // include; the backend must not also pass them positionally
                // (Verilator's single unit would define them twice: MODDUP).
                for (const auto &[file, targets] : includes)
                    for (const auto &t : targets) data.included_sources.insert(t);
            }

            // Every includer directory joins the +incdir set: relative
            // `includes (e.g. tb_top.sv -> testbench/dasm.svi) are not
            // guaranteed to resolve against the including file alone.
            {
                std::set<std::string> closure;
                for (auto *set : {&synth_sources, &synth_packages, &sim_sources, &sim_packages})
                    for (auto &src : *set) closure.insert(src);
                for (auto &d : verilator_project_generator::extra_include_dirs(
                         s_store->get_hdl_store().string(), closure, data.commons_dir))
                    data.commons_dir.push_back(d);
            }

            // Referenced stimulus images absent from the repository (e.g. a
            // toolchain-generated program.hex): the script checks for them
            // in the run directory instead of hanging the sim.
            {
                std::set<std::string> missing;
                for (const auto &m : synth_r.get_missing_data()) missing.insert(m);
                for (const auto &m : sim_r.get_missing_data()) missing.insert(m);
                data.missing_data_files = {missing.begin(), missing.end()};
            }

            generator.set_data(data);

            if (opts.generate_synth_script) {
                spdlog::error("Verilator is a simulation-only backend: --synth_script is not supported with --V");
                return 77;
            }
            std::ofstream simfile("sim_verilator.sh");
            generator.generate_sim_script(simfile);
            simfile.close();
            std::error_code ec;
            std::filesystem::permissions("sim_verilator.sh",
                std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec,
                std::filesystem::perm_options::add, ec);
            if (ec) spdlog::warn("Could not set executable bit on sim_verilator.sh: {}", ec.message());
            LOG_TIMEPOINT("Verilator sim script generated");
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
    return std::nullopt;
}
