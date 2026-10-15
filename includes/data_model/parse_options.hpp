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

#ifndef ANANKE_PARSE_OPTIONS_HPP
#define ANANKE_PARSE_OPTIONS_HPP

#include <optional>
#include <set>
#include <string>
#include <unordered_map>
#include <vector>

// Parse-relevant settings bundled for dependency injection. This is the only
// options parcel threaded walker -> task -> analyzer -> preprocessor ->
// macro engine, so a future toggle is one field here and zero plumbing.
struct include_header_resolution_opt {
    std::optional<std::string> global_target;
    // Scope dir (repo-relative, normalized, no leading slash) -> target
    // (repo-relative). Longest matching scope prefix wins.
    std::unordered_map<std::string, std::string> scoped_targets;
};

struct parse_options {
    std::set<std::string> include_directories;
    std::set<std::string> defines;
    // Exact basenames of quoted `includes to skip silently (no warning).
    // Quoted-only: `<...>` angle includes still error when missing.
    std::set<std::string> ignored_includes;
    // Ambiguous-header resolution keyed on the include-file key (basename or
    // subpath). Value is a global target or scoped includer-dir -> target.
    // All paths repo-relative (see repo_root).
    std::unordered_map<std::string, include_header_resolution_opt> include_deconfliction;
    // Absolute repository root used to relativize includer paths for scoped
    // matching and to resolve repo-relative targets. Empty = scoping by
    // absolute suffix fallback.
    std::string repo_root;
    bool strip_uvm_macros = false;
    // LRM semantic rule checks (like built-in class methods
    // that shall not be overridden). On by default; set to
    // skip them on hot paths that only need dependency information.
    bool disable_semantic_checks = false;
    // Strict mode: reject out-of-spec code that lenient mode tolerates to
    // get real-world trees (UVM-heavy ones in particular) through the
    // analyzer. Every LRM deviation must consult this flag so strict stays
    // a single switch: no new tolerated construct without a strict gate.
    // Currently gated: empty-parens macro calls (`f()` for a 1-formal
    // macro), streaming-concatenation lvalue approximation, and unit-less
    // `.svh`/`.vh` include fragments (bare `logic`/`assign` with no
    // module/interface/package/program/primitive/config declaration, meant
    // to be `included inside a module, stored as empty resources).
    bool strict = false;
    // Repository mode, lenient: file-level names that do not resolve inside
    // the file are not errors on their own (the tool has no $unit context
    // spanning files). They are exported with the file and judged against
    // the names declared anywhere in the repository. Off for single-file
    // parsing, where an unresolved name is an error.
    bool defer_unit_name_errors = false;
};

#endif //ANANKE_PARSE_OPTIONS_HPP
