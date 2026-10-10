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

#include "Backend/Verilator/verilator_project_generator.hpp"
#include "frontend/macro_table.hpp"

#include <algorithm>
#include <cctype>
#include <filesystem>

namespace {

std::string shell_quote(const std::string &s) {
    std::string out = "\"";
    for (char c : s) {
        if (c == '"' || c == '\\' || c == '$' || c == '`') out += '\\';
        out += c;
    }
    out += "\"";
    return out;
}

std::string lowercase_ext(const std::string &path) {
    std::string ext = std::filesystem::path(path).extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
    return ext;
}

void emit_phase(std::ostream &output, const std::string &name) {
    output << "printf '\\n\\033[1;33m>>> " << name << " <<<\\033[0m\\n'\n";
}

// Repository-base-relative entries (Depfile convention, like include_paths)
// resolve against base_dir; absolute entries pass through verbatim.
std::string resolve_repo_path(const std::string &base_dir, const std::string &entry) {
    std::filesystem::path p(entry);
    if (p.is_absolute()) return p.string();
    return (std::filesystem::path(base_dir) / p).string();
}

// Terminates a fallible command line with a source-safe guard: reports the
// failure, then `return`s out of a sourced script or `exit`s an executed one.
std::string guard_fail(const std::string &msg) {
    return " || { sim_err \"" + msg + "\"; return 1 2>/dev/null || exit 1; }";
}

} // namespace

bool verilator_project_generator::is_glbl(const std::string &path) {
    return std::filesystem::path(path).filename() == "glbl.v";
}

bool verilator_project_generator::is_hdl_unit(const std::string &path) {
    const std::string ext = lowercase_ext(path);
    return ext == ".v" || ext == ".sv";
}

bool verilator_project_generator::is_cpp_harness(const std::string &path) {
    const std::string ext = lowercase_ext(path);
    return ext == ".cpp" || ext == ".cc" || ext == ".cxx" || ext == ".c++";
}

std::vector<std::string> verilator_project_generator::ordered_units() const {
    // Deterministic order: packages first, then synth units, then sim-only
    // units. Mirrors the ordering used by the xilinx sim script.
    // Files `included by closure files are skipped: Verilator compiles all
    // inputs as one unit, so the include already compiles them exactly once
    // and a positional copy would be a fatal MODDUP (e.g. VeeR tb_top.sv
    // including testbench/axi_lsu_dma_bridge.sv).
    std::set<std::string> included_norm;
    for (const auto &f : data.included_sources) {
        included_norm.insert(
            std::filesystem::absolute(std::filesystem::path(f)).lexically_normal().string());
    }
    auto is_included = [&](const std::string &f) {
        return included_norm.contains(
            std::filesystem::absolute(std::filesystem::path(f)).lexically_normal().string());
    };
    std::vector<std::string> units;
    auto push = [&](const std::set<std::string> &sources) {
        for (const auto &f : sources) {
            if (is_glbl(f)) continue; // Xilinx simulation helper, not for Verilator
            if (is_cpp_harness(f)) continue; // handled as --exe sources
            if (is_included(f)) continue; // compiled via the `include
            const std::string ext = lowercase_ext(f);
            if (ext == ".vhd" || ext == ".vhdl") {
                spdlog::warn("Skipping VHDL file {}: Verilator only supports (System)Verilog", f);
                continue;
            }
            if (!is_hdl_unit(f)) continue; // headers (.vh/.svh/.h) go via +incdir+
            if (std::find(units.begin(), units.end(), f) == units.end()) units.push_back(f);
        }
    };
    push(data.package_synth_sources);
    push(data.synth_sources);
    push(data.package_sim_sources);
    push(data.sim_sources);
    return units;
}

std::vector<std::string> verilator_project_generator::harness_sources(const std::vector<std::string> &units) const {
    std::vector<std::string> harness;
    auto consider = [&](const std::string &f) {
        if (is_cpp_harness(f) && std::find(harness.begin(), harness.end(), f) == harness.end())
            harness.push_back(f);
    };
    if (!data.sim_harness.empty()) {
        // Explicit Depfile option is authoritative: C++ harnesses are never
        // parsed/stored by the data store, so they bypass the AST closure.
        for (const auto &h : data.sim_harness) consider(resolve_repo_path(base_dir, h));
        return harness;
    }
    // Future-proof: any C++ file already in the dependency closure.
    for (const auto &s : {data.package_synth_sources, data.synth_sources, data.package_sim_sources, data.sim_sources})
        for (const auto &f : s) consider(f);
    (void)units;
    // VeeR convention: test_tb_top.cpp (or <tb_tl>.cpp) next to the testbench.
    // C++ harnesses are never parsed/stored by the data store, so they cannot
    // come from the AST closure; probe the tb's directory instead.
    std::set<std::string> tb_dirs;
    for (const auto &f : data.sim_sources) tb_dirs.insert(std::filesystem::path(f).parent_path().string());
    for (const auto &f : data.package_sim_sources) tb_dirs.insert(std::filesystem::path(f).parent_path().string());
    const std::vector<std::string> candidates = {"test_tb_top.cpp", data.tb_tl + ".cpp"};
    for (const auto &dir : tb_dirs) {
        for (const auto &c : candidates) {
            std::error_code ec;
            std::filesystem::path p = std::filesystem::path(dir) / c;
            if (std::filesystem::exists(p, ec)) consider(p.string());
        }
    }
    return harness;
}

std::string verilator_project_generator::verilator_bin() const {
    if (settings->has_tool("verilator")) return settings->get_tool_path_or("verilator", "verilator").string();
    return "verilator";
}

std::vector<std::string> verilator_project_generator::extra_include_dirs(    const std::string &hdl_store,
    const std::set<std::string> &closure_files,
    const std::vector<std::string> &existing) {
    auto strip = [](std::string s) {
        while (!s.empty() && s.front() == '/') s.erase(s.begin());
        return s;
    };
    std::set<std::string> covered;
    for (const auto &e : existing) covered.insert(strip(e));
    std::filesystem::path base(hdl_store);
    std::vector<std::string> extra;
    // closure_files is a std::set: iteration is sorted, output deterministic.
    for (const auto &f : closure_files) {
        std::error_code ec;
        std::filesystem::path rel = std::filesystem::relative(
            std::filesystem::path(f).parent_path(), base, ec);
        if (ec) continue;
        std::string rs = rel.string();
        if (rs.empty() || rs == ".") rs.clear(); // file at the repository root
        if (rs.starts_with("..")) continue; // outside the repository
        std::string entry = rs.empty() ? "/" : "/" + rs;
        if (covered.contains(strip(entry))) continue;
        covered.insert(strip(entry));
        extra.push_back(entry);
    }
    return extra;
}

std::vector<std::string> verilator_project_generator::resolve_leading_units(
    const std::unordered_map<std::string, std::vector<stored_macro_def>> &all_defs,
    const std::unordered_map<std::string, std::vector<std::string>> &macro_needs,
    const std::unordered_map<std::string, std::vector<std::string>> &includes,
    const std::set<std::string> &closure_files,
    const std::set<std::string> &profile_define_names) {
    auto norm = [](const std::string &p) {
        return std::filesystem::absolute(std::filesystem::path(p)).lexically_normal().string();
    };
    auto is_compilable = [](const std::string &p) {
        std::string ext = std::filesystem::path(p).extension().string();
        for (auto &c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        return ext == ".v" || ext == ".sv" || ext == ".vh" || ext == ".svh";
    };

    std::set<std::string> closure_norm;
    for (const auto &f : closure_files) closure_norm.insert(norm(f));
    std::set<std::string> included_norm;
    for (const auto &[file, targets] : includes) {
        if (!closure_norm.contains(norm(file))) continue;
        for (const auto &t : targets) included_norm.insert(norm(t));
    }

    // Macro-attributed definers first: files the closure consumed macros from.
    macro_table table;
    for (const auto &[path, defs] : all_defs) {
        preprocessor::macro_definitions_map live;
        for (const auto &d : defs) live[d.name] = macro_to_live(d);
        table.add_file_definitions(path, live);
    }
    std::vector<std::string> leading;
    auto consider = [&](const std::string &p) {
        std::string n = norm(p);
        for (const auto &e : leading) if (norm(e) == n) return;
        leading.push_back(p);
    };
    // Deterministic: sorted requesting files, sorted macro names.
    std::vector<std::string> requesters;
    for (const auto &[file, names] : macro_needs) {
        if (closure_norm.contains(norm(file))) requesters.push_back(file);
    }
    std::sort(requesters.begin(), requesters.end());
    for (const auto &file : requesters) {
        std::vector<std::string> names = macro_needs.at(file);
        std::sort(names.begin(), names.end());
        for (const auto &name : names) {
            if (profile_define_names.contains(name)) continue; // +define+ covers it
            auto v = table.judge(name, {}, file);
            if (v.state != macro_table::verdict::kind::unique || v.candidates.empty()) continue;
            const std::string &definer = v.candidates.front().path;
            if (!is_compilable(definer)) continue;
            if (std::filesystem::path(definer).filename() == "glbl.v") continue;
            std::string dn = norm(definer);
            if (included_norm.contains(dn)) continue;
            // Already positional as a regular unit: .v/.sv files are emitted
            // by ordered_units(); anything else must lead explicitly.
            if (closure_norm.contains(dn) && is_hdl_unit(definer)) continue;
            consider(definer);
        }
    }

    // Closure headers nothing includes (e.g. a .vh holding an instantiated
    // module): true headers stay on +incdir+ only.
    std::vector<std::string> ordered_closure(closure_norm.begin(), closure_norm.end());
    for (const auto &n : ordered_closure) {
        std::string ext = std::filesystem::path(n).extension().string();
        for (auto &c : ext) c = static_cast<char>(std::tolower(static_cast<unsigned char>(c)));
        if ((ext == ".vh" || ext == ".svh") && !included_norm.contains(n)) consider(n);
    }
    return leading;
}

void verilator_project_generator::write_makefile(std::ostream &output) {
    // Sim-only backend: the shell script is the only artifact.
    generate_sim_script(output);
}

void verilator_project_generator::generate_synth_script(std::ostream &output) {
    (void)output;
    spdlog::error("Verilator is a simulation-only backend: synthesis script generation is not supported");
}

void verilator_project_generator::generate_sim_script(std::ostream &output) {
    const auto units = ordered_units();
    const auto harness = harness_sources(units);
    const bool use_harness = !harness.empty();

    // Leading units first positionally: Verilator processes positional files
    // in order with `define directives persisting across them. An explicit
    // Depfile sim_defines list is an override: it fully determines the
    // leading slots (user order, verbatim apart from path resolution) and
    // skips automatic resolution, so either side of a macro conflict the
    // flow refuses to guess can be forced. Otherwise the flow-resolved
    // header_units apply (see resolve_leading_units).
    std::vector<std::string> leading_units;
    if (!data.sim_defines.empty()) {
        for (const auto &h : data.sim_defines) {
            std::string resolved = resolve_repo_path(base_dir, h);
            if (std::find(leading_units.begin(), leading_units.end(), resolved) == leading_units.end())
                leading_units.push_back(resolved);
        }
    } else {
        leading_units = data.header_units;
    }
    std::set<std::string> leading_norm;
    for (const auto &l : leading_units) {
        leading_norm.insert(
            std::filesystem::absolute(std::filesystem::path(l)).lexically_normal().string());
    }

    if (data.tb_tl.empty()) spdlog::warn("Verilator sim script: no simulation top specified (sim_tl empty)");
    if (units.empty() && leading_units.empty()) spdlog::warn("Verilator sim script: simulation closure is empty");

    // +define+ flags from the profile. VERILATOR itself is deliberately NOT
    // passed: the tool predefines it, and redefining it trips REDEFMACRO.
    // (It stays undefined during ananke analysis; `ifdef VERILATOR branches
    // in the sources therefore select correctly in both worlds.)
    std::vector<std::string> defines;
    for (const auto &d : settings->get_defines()) {
        if (d == "VERILATOR" || d.starts_with("VERILATOR=")) {
            spdlog::warn("Ignoring profile define '{}': Verilator predefines VERILATOR itself", d);
            continue;
        }
        defines.push_back(d);
    }

    output << "#!/usr/bin/env bash\n";
    output << "# Generated by ananke --V for project " << data.name << "\n";
    output << "# Top: " << data.tb_tl << (use_harness ? " (C++ harness flow, cf. VeeR tools/Makefile verilator-build)\n"
                                                      : " (no C++ harness found: --binary flow)\n");
    output << "# Standalone define-headers (Depfile sim_defines override, else flow-resolved)\n";
    output << "# are compiled first positionally so their `defines precede all units.\n";
    output << "# Stimulus (program.hex etc.) is NOT built here; stage it in $RUN_DIR before running\n";
    output << "# (or set HEX=/path/to/image to stage it as program.hex), as the VeeR flow does\n";
    output << "# (`verilator: program.hex verilator-build`).\n";
    output << "# NOTE: VeeR appends `undef ASSERT_ON to common_defines.vh before building;\n";
    output << "# apply the same to your generated defines header if assertion macros fire.\n";
    output << "# Run from the directory containing this script, either executed\n";
    output << "# (./sim_verilator.sh, bash, zsh, sh) or sourced. It only uses POSIX\n";
    output << "# constructs, never alters the caller's shell options, and reports\n";
    output << "# failures without killing the calling shell.\n";
    // Source-safe failure handling: each fallible command is guarded by an
    // inline `{ ...; return ... || exit ...; }` block. `return` unwinds a
    // sourced script while `exit` stops an executed one (a global `set -e`
    // is deliberately NOT used: it would leak into the caller's shell when
    // sourced, killing e.g. interactive zsh on the next failing command).
    // NOTE: the guard must stay inline — a `return` inside a helper function
    // would only leave the function and the `|| exit` would then kill a
    // sourcing shell.
    output << "sim_err() { echo \"sim_verilator.sh: error: $*\" >&2; }\n\n";

    output << "VERILATOR=${VERILATOR:-" << shell_quote(verilator_bin()) << "}\n";
    output << "TOP=" << shell_quote(data.tb_tl) << "\n";
    output << "OBJ_DIR=\"./obj_dir\"\n";
    output << "RUN_DIR=\"./verilator_sim\"\n";
    output << "TRACE=\"${TRACE:-0}\"\n\n";

    output << "mkdir -p \"$OBJ_DIR\" \"$RUN_DIR\"" << guard_fail("cannot create work directories") << "\n";
    for (const auto &f : data.data_synth_sources) {
        output << "cp " << shell_quote(f) << " \"$RUN_DIR/\"" << guard_fail("cannot stage data file") << "\n";
    }
    for (const auto &f : data.data_sim_sources) {
        output << "cp " << shell_quote(f) << " \"$RUN_DIR/\"" << guard_fail("cannot stage data file") << "\n";
    }
    output << "\n";

    output << "TRACE_FLAGS=\"\"\n";
    output << "if [ \"$TRACE\" = \"1\" ]; then TRACE_FLAGS=\"--trace-fst\"; fi\n\n";

    // Test stimulus: $readmem images absent from the repository (flow-filled
    // missing_data_files) must exist under RUN_DIR at run time. Point HEX at
    // a program image to stage it as program.hex. Missing images fail here —
    // before a build — instead of hanging the sim on $readmem warnings.
    output << "# Test program image (e.g. VeeR program.hex, built outside this flow).\n";
    output << "if [ -n \"${HEX:-}\" ]; then\n";
    output << "    cp \"$HEX\" \"$RUN_DIR/program.hex\""
           << guard_fail("cannot stage HEX image") << "\n";
    output << "fi\n";
    {
        for (const auto &img : data.missing_data_files) {
            output << "if [ ! -f \"$RUN_DIR/" << img << "\" ]; then\n";
            output << "    sim_err \"required stimulus '" << img << "' not found in $RUN_DIR"
                   << " (read by \\$readmem in the sources)\"\n";
            if (img == "program.hex" || img.ends_with("/program.hex")) {
                output << "    sim_err \"copy the image into $RUN_DIR, or point HEX at it"
                       << " (HEX=/path/to/image stages it as program.hex)\"\n";
            } else {
                output << "    sim_err \"stage the file into $RUN_DIR before running\"\n";
            }
            output << "    return 1 2>/dev/null || exit 1\n";
            output << "fi\n";
        }
    }
    output << "\n";

    emit_phase(output, "PHASE 1: VERILATE");
    output << "\"$VERILATOR\" ";
    if (use_harness) {
        output << "--cc ";
    } else {
        output << "--binary --timing ";
    }
    if (!data.verilator.cflags.empty()) {
        output << "-CFLAGS \"";
        for (size_t i = 0; i < data.verilator.cflags.size(); ++i) {
            if (i > 0) output << " ";
            output << data.verilator.cflags[i];
        }
        output << "\" \\\n";
    }
    for (const auto &w : data.verilator.waivers) {
        output << "    " << shell_quote("-Wno-" + w) << " \\\n";
    }
    output << "    ";
    if (data.verilator.autoflush) output << "--autoflush ";
    output << "$TRACE_FLAGS \\\n";
    output << "    --top-module \"$TOP\" \\\n";
    for (const auto &d : defines) output << "    " << shell_quote("+define+" + d) << " \\\n";
    for (const auto &d : data.commons_dir) output << "    " << shell_quote("+incdir+" + base_dir + d) << " \\\n";
    for (const auto &a : data.verilator.extra_args) output << "    " << shell_quote(a) << " \\\n";
    for (const auto &h : leading_units) output << "    " << shell_quote(h) << " \\\n";
    // Explicitly led files that are also closure units must not be emitted
    // twice (double compilation breaks on redefinitions).
    auto is_led = [&](const std::string &u) {
        return leading_norm.contains(
            std::filesystem::absolute(std::filesystem::path(u)).lexically_normal().string());
    };
    for (const auto &u : units) {
        if (is_led(u)) continue;
        output << "    " << shell_quote(u) << " \\\n";
    }
    if (use_harness) {
        output << "    --exe \\\n";
        for (const auto &h : harness) output << "    " << shell_quote(h) << " \\\n";
    }
    output << "    --Mdir \"$OBJ_DIR\"" << guard_fail("verilation failed") << "\n\n";

    if (use_harness) {
        emit_phase(output, "PHASE 2: MAKE");
        output << "NPROC=\"${NPROC:-$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)}\"\n";
        output << "make -j\"$NPROC\" -C \"$OBJ_DIR\" -f \"V${TOP}.mk\"";
        for (const auto &m : data.verilator.make_args) output << " " << shell_quote(m);
        output << guard_fail("model build failed") << "\n\n";
        emit_phase(output, "PHASE 3: RUN");
        output << "cp \"$OBJ_DIR/V${TOP}\" \"$RUN_DIR/\"" << guard_fail("cannot stage model binary") << "\n";
        output << "( cd \"$RUN_DIR\" && \"./V${TOP}\" \"$@\" )" << guard_fail("simulation run failed") << "\n";
    } else {
        emit_phase(output, "PHASE 2: RUN");
        output << "\"$OBJ_DIR/V${TOP}\" \"$@\"" << guard_fail("simulation run failed") << "\n";
    }
}
