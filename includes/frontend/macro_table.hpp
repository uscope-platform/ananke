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

#ifndef ANANKE_MACRO_TABLE_HPP
#define ANANKE_MACRO_TABLE_HPP

#include <map>
#include <set>
#include <string>
#include <vector>

#include "data_model/macro_definition.hpp"
#include "frontend/analysis/system_verilog/preprocessor/macro_processor.hpp"

// Repository-wide macro table for compilation-order macros.
//
// SystemVerilog `defines are compilation-unit scoped: a macro defined in one
// file is visible in files compiled after it, with no per-file `include
// required (this is how riscv-dv's dv_defines.svh reaches corev-dv fragments
// through manifest.f ordering). Since this tool analyzes files independently,
// the table re-learns those macros from the repository itself: every file
// contributes its textual definitions (even quarantined ones, partially),
// and quarantined files are re-parsed with the resolved entries injected.
//
// Conflict policy: byte-identical bodies from several files are one logical
// definition (same macro textually defined in mirrored headers). Distinct
// bodies for one name are a genuine conflict and only surface in files
// that actually need the macro — unrelated files never break on it.
class macro_table {
public:
    // One repository definition of a macro, with its call-shape signature.
    // Simple (non-function) macros accept no arguments.
    struct candidate_info {
        std::string path;
        bool is_function = false;
        int total_params = 0;
        int required_params = 0;
    };

    struct verdict {
        enum class kind { absent, unique, conflict, mismatch, shadowed };
        kind state = kind::absent;
        preprocessor::macro_definitions_map::mapped_type definition;
        std::vector<candidate_info> candidates;
    };

    struct injection {
        preprocessor::macro_definitions_map definitions;
        std::vector<std::string> resolved;
        std::vector<std::string> unresolvable;
        std::map<std::string, std::vector<std::string>> conflicts;
        std::map<std::string, std::vector<std::string>> mismatches;
    };

    // (Re)places all definitions harvested from one file. Pass 1 seeds from
    // persisted per-file entries; fresh parses overwrite their own entry.
    void add_file_definitions(const std::string &path,
                              const preprocessor::macro_definitions_map &defs);
    void remove_file(const std::string &path);
    // Single-name verdict used by injection building and terminal reporting.
    // arities holds the observed call-site argument counts (empty = used
    // without parentheses, no shape information). requester enables
    // self-exclusion (empty = no filter).
    [[nodiscard]] verdict judge(const std::string &name,
                                const std::set<int> &arities,
                                const std::string &requester = "") const;
    // Injection subset for one file's recorded needs (name -> observed call
    // arities). Names the requesting file defines itself are never injected:
    // seeding them would invert include guards (`` `ifndef FOO_SV `` must see
    // FOO_SV undefined) and would also rewrite intra-file use-before-define
    // order, which fails in a simulator too.
    [[nodiscard]] injection build_injection(const preprocessor::undefined_uses_map &needs,
                                            const std::string &requester = "") const;
    [[nodiscard]] bool empty() const;
    [[nodiscard]] std::set<std::string> names() const;
    // Human-readable candidate signature for diagnostics, e.g.
    // "path/to/file (5 params, 3 required)" or "path/to/file (simple)".
    static std::string describe_candidate(const candidate_info &candidate);
    // Deterministic rendering for fingerprinting (caller hashes it).
    [[nodiscard]] std::string canonical_string() const;
    // Canonical bodies per defining file for one macro (empty when absent).
    // Used to group conflict definers by distinct body.
    [[nodiscard]] std::map<std::string, std::string> bodies_for(const std::string &name) const;
    // Per-name deterministic renderings for cross-run change detection
    // (compare strings across runs; hash the whole canonical_string for
    // the table fingerprint).
    [[nodiscard]] std::map<std::string, std::string> rendered_by_name() const;

private:
    // name -> (defining file -> canonical body). Canonical bodies drive
    // conflict detection; live copies below drive injection.
    std::map<std::string, std::map<std::string, std::string>> bodies_;
    std::map<std::string,
             std::map<std::string, preprocessor::macro_definitions_map::mapped_type>> live_;
};

// Boundary conversions between live preprocessor definitions and the
// persisted data_model form.
stored_macro_def macro_to_stored(
    const std::string &name,
    const preprocessor::macro_definitions_map::mapped_type &def);
preprocessor::macro_definitions_map::mapped_type macro_to_live(const stored_macro_def &stored);
std::string macro_canonical_body(
    const preprocessor::macro_definitions_map::mapped_type &def);

#endif //ANANKE_MACRO_TABLE_HPP
