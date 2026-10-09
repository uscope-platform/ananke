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

#ifndef ANANKE_REPOSITORY_WALKER_HPP
#define ANANKE_REPOSITORY_WALKER_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <set>
#include <vector>
#include <unordered_map>
#include <future>
#include <utility>
#include <gtest/gtest.h>
#include <openssl/evp.h>
#include <utility>
#include <spdlog/spdlog.h>

#include "data_model/settings_store.hpp"
#include "data_model/Script.hpp"
#include "data_model/Constraints.hpp"
#include "data_model/data_store.hpp"
#include "data_model/DataFile.hpp"
#include "data_model/include_dependency.hpp"
#include "analysis/system_verilog/sv_analyzer.hpp"
#include "analysis/vhdl/vhdl_analyzer.hpp"
#include "frontend/repository_index.hpp"
#include "frontend/macro_table.hpp"
#include "third_party/thread_pool.hpp"

template<typename  T>
struct file_analysis_context {
    std::string path;
    std::string hash;
    std::optional<T> resource;
    std::vector<include_dependency> includes;
    // Compilation-order macro support (Verilog only): files whose analysis
    // failed solely on repository-learnable unknowns are quarantined instead
    // of erroring, then re-parsed with table entries injected.
    bool quarantined = false;
    // Cache hit: content unchanged, cache entry stands as-is. Harvested is
    // empty (nothing re-parsed); must not disturb the macro table.
    bool cache_skipped = false;
    preprocessor::undefined_uses_map undefined_macros;
    std::set<std::string> unknown_conditionals;
    preprocessor::macro_definitions_map harvested;
    // Analyzer's failure text for the attempt (macro summary or parse
    // failure). Surfaced by the quarantine report when macros converged
    // but the file still fails.
    std::string error_detail;
};

static file_analysis_context<hdl_file> analyze_verilog(const std::filesystem::path &file, const parse_options &opts, const std::string &old_hash, const std::shared_ptr<repository_index> &idx, const preprocessor::macro_definitions_map &injected);
static file_analysis_context<hdl_file> analyze_vhdl(const std::filesystem::path &file, std::set<std::string> i_d, const std::string &old_hash);
static file_analysis_context<Script>  analyze_script(const std::filesystem::path &file, std::set<std::string> i_d, const std::string &old_hash);
static file_analysis_context<DataFile>  analyze_data(const std::filesystem::path &file, std::set<std::string> i_d, const std::string &old_hash);
static file_analysis_context<Constraints>  analyze_constraint(const std::filesystem::path &file, std::set<std::string> i_d, const std::string &old_hash);
static std::optional<std::string> hash_file(const std::string_view &file_content);

const unsigned int max_threads = [] {
    auto h = std::thread::hardware_concurrency();
    return h > 1 ? (h - 1) : 1;
}();

class Repository_walker {

public:
    Repository_walker(const std::shared_ptr<settings_store>& s, const std::shared_ptr<data_store>& d, bool ephimeral);
    Repository_walker(const std::shared_ptr<settings_store>& s, const std::shared_ptr<data_store>& d, bool ephimeral, std::set<std::string> ex);
    void scan_repository();
    void analyze_dir();
    std::shared_ptr<repository_index> get_repository_index() { return repository_index_p; }
private:
    void construct_walker(std::shared_ptr<settings_store> s, std::shared_ptr<data_store> d, std::set<std::string> ex);
    bool is_excluded_directory(const std::filesystem::path& dir);
    bool contains_excluding_file(const std::filesystem::path& dir);
    void read_ignore_file(const std::filesystem::path& file);
    void analyze_file(std::filesystem::path& dir);
    void collect_analysis_results();
    void invalidate_stale_includes();
    // Content-hash tracking for consumed index-only `.h` headers (see
    // file_is_sv_include_header): hashes bytes without parsing, seeds
    // previous_hashes/file_hashes so invalidate_stale_includes() notices
    // header edits, and persists the result in the data store.
    void refresh_include_header_hashes();
    // Compilation-order macro fixpoint (see macro_table.hpp): seeds the table
    // from persisted entries, drains the quarantine with injected re-parses,
    // revalidates branch decisions taken with partial knowledge, revalidates
    // cache-skipped macro consumers across runs, and errors out leftovers.
    void seed_macro_table();
    void run_macro_fixpoint();    void validation_sweep();
    void invalidate_stale_macros();
    void report_quarantine_errors();
    std::string macro_table_fingerprint() const;

    // File type discrimination methods
    // TODO: use these to make file associations dynamic a la vscode
    static bool file_is_verilog(const std::filesystem::path &file);
    static bool file_is_sv_include_header(const std::filesystem::path &file);
    static bool file_is_vhdl(const std::filesystem::path &file);
    static bool file_is_script(const std::filesystem::path &file);
    static bool file_is_constraint(const std::filesystem::path &file);
    static bool file_is_data(const std::filesystem::path &file);
    // TODO: Make excluded directories dynamic with a mechanism similar to .gitignore

    FRIEND_TEST(repository_walker , file_type_handling);
    FRIEND_TEST(repository_walker, quarantine_report_grouping);

    std::set<std::string> excluded_directories;
    std::set<std::string> excluding_extensions = {".xpr"};
    std::string ignore_file_name = ".mkignore";

    std::string target_repository;
    std::shared_ptr<settings_store> s_store;
    std::shared_ptr<data_store> d_store;
    std::shared_ptr<repository_index> repository_index_p;
    std::vector<std::filesystem::path> scanned_files;
    std::unordered_map<std::string, std::string> file_hashes;
    std::unordered_map<std::string, std::string> previous_hashes;

    std::vector<std::future<file_analysis_context<hdl_file>>> hdl_futures;
    std::vector<std::future<file_analysis_context<Script>>> scripts_futures;
    std::vector<std::future<file_analysis_context<Constraints>>> constraints_futures;
    std::vector<std::future<file_analysis_context<DataFile>>> data_futures;

    thread_pool pool;
    int working_threads = 0;
    parse_options parse_opts_;

    // Bounded fixpoint: chains and mutual macro dependencies converge in a
    // few rounds (riscv-dv shape needs 2, deep UVM helper chains ~10); the
    // injection-dedup rule above guarantees termination regardless of the cap.
    static constexpr int max_macro_passes = 16;

    struct quarantine_entry {
        preprocessor::undefined_uses_map undefined_macros;
        std::set<std::string> unknown_conditionals;
        std::string hash;
        // Latest attempt's state (overwrite, not merge): the accumulated
        // maps above mix stale resolved names with current ones (each
        // pass only sees unknowns past previously injected ones), so
        // reporting must use the final state while injection keeps the
        // union. last_error carries the analyzer's failure reason for
        // attempts where macros had already converged.
        preprocessor::undefined_uses_map last_undefined;
        std::set<std::string> last_unknowns;
        std::string last_error;
    };

    macro_table macro_table_;
    std::map<std::string, quarantine_entry> quarantine_;
    // Files parsed fresh this run (stored successes), for the validation sweep.
    std::set<std::string> freshly_parsed_;
    // In-memory record of unknowns at each fresh file's parse time, plus the
    // bodies it consumed via injection: the sweep re-parses on any drift.
    std::map<std::string, std::set<std::string>> parsed_unknowns_;
    std::map<std::string, std::map<std::string, std::string>> parsed_consumed_;
    // Pending per-file injections (submit time) for dep recording at collect.
    std::map<std::string, preprocessor::macro_definitions_map> pending_injections_;
    // Last attempted injection per quarantined file: a file is only
    // resubmitted when its injection changed (termination argument).
    std::map<std::string, preprocessor::macro_definitions_map> last_injections_;
    // Table rendering snapshot taken right after seeding (pre-pass-1), for
    // cross-run change detection against the final table.
    std::map<std::string, std::string> table_at_seed_;
    void analyze_file_with_injection(std::filesystem::path &file,
                                     const preprocessor::macro_definitions_map &injected);
    // One log line per distinct quarantine problem (shared by all files
    // hitting it), for the report above. Static for unit testing.
    static std::vector<std::string> build_quarantine_report(
        const macro_table &table,
        const std::map<std::string, quarantine_entry> &quarantine);
};


#endif //ANANKE_REPOSITORY_WALKER_HPP
