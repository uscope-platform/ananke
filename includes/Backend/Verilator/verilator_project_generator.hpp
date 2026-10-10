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

#ifndef ANANKE_VERILATOR_PROJECT_GENERATOR_HPP
#define ANANKE_VERILATOR_PROJECT_GENERATOR_HPP

#include "Backend/project_generator_base.hpp"
#include "data_model/macro_definition.hpp"
#include <spdlog/spdlog.h>
#include <set>
#include <unordered_map>
#include <vector>

class verilator_project_generator : public project_generator_base {
public:
    verilator_project_generator(const std::shared_ptr<settings_store> &s_store) : project_generator_base(s_store) {};
    void write_makefile(std::ostream &output) override;
    void generate_sim_script(std::ostream &output) override;
    void generate_synth_script(std::ostream &output) override;
    // Leading compile units for the Verilator command line: files whose macro
    // definitions the closure consumed (via injection) but which reach the
    // command line no other way. Rebuilds the repository macro table from the
    // persisted per-file harvests and attributes each consumed macro with the
    // same verdicts analysis used. A definer is pulled only when the verdict
    // is unique (conflicts/mismatches refuse to guess, like analysis);
    // self-satisfied (shadowed) and profile-global macros need no file;
    // definers already positional as units or reachable via `include stay out.
    // Deterministic output. Pure function of its inputs (no store access).
    static std::vector<std::string> resolve_leading_units(
        const std::unordered_map<std::string, std::vector<stored_macro_def>> &all_defs,
        const std::unordered_map<std::string, std::vector<std::string>> &macro_needs,
        const std::unordered_map<std::string, std::vector<std::string>> &includes,
        const std::set<std::string> &closure_files,
        const std::set<std::string> &profile_define_names);
    // Extra +incdir entries for the Verilator command line: the parent    // directory of every closure file, repo-relative with a leading '/'
    // (same convention as the auto-discovered entries). Relative `includes
    // (e.g. VeeR tb_top.sv -> testbench/dasm.svi, covered upstream by an
    // explicit -I testbench) are not guaranteed to resolve against the
    // including file alone, and quarantined files record no edges at all.
    // Entries already covered and files outside the repository are skipped.
    // Deterministic output. Purely lexical (no filesystem access).
    static std::vector<std::string> extra_include_dirs(
        const std::string &hdl_store,
        const std::set<std::string> &closure_files,
        const std::vector<std::string> &existing);
private:
    // Verilator is a simulation-only backend: there is no synthesis flow and
    // no project/makefile concept. The only artifact is sim_verilator.sh.
    static bool is_hdl_unit(const std::string &path);
    static bool is_cpp_harness(const std::string &path);
    static bool is_glbl(const std::string &path);
    // Full simulation closure in deterministic order: packages first, then
    // synth units, then sim-only units (mirrors the xilinx sim script).
    std::vector<std::string> ordered_units() const;
    // C++ harness sources: the explicit Depfile `sim_harness` option when set
    // (authoritative); otherwise any .cpp/.cc in the closure plus the VeeR
    // convention sibling (test_tb_top.cpp / <tb_tl>.cpp next to the tb).
    std::vector<std::string> harness_sources(const std::vector<std::string> &units) const;
    std::string verilator_bin() const;
};

#endif //ANANKE_VERILATOR_PROJECT_GENERATOR_HPP
