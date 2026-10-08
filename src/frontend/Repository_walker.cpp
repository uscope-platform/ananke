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


#include "frontend/Repository_walker.hpp"
#include "data_model/mm_file.hpp"



Repository_walker::Repository_walker(const std::shared_ptr<settings_store>& s, const std::shared_ptr<data_store>& d, bool ephimeral) : pool(max_threads){
    construct_walker(s, d, {".git"});
}

Repository_walker::Repository_walker(const std::shared_ptr<settings_store>& s, const std::shared_ptr<data_store>& d, bool ephimeral,std::set<std::string> ex) : pool(max_threads){
    construct_walker(s, d, std::move(ex));
}

void Repository_walker::construct_walker(std::shared_ptr<settings_store> s, std::shared_ptr<data_store> d,
                                         std::set<std::string> ex) {

    s_store = std::move(s);
    d_store = std::move(d);
    excluded_directories = std::move(ex);
    target_repository = s_store->get_hdl_store();
    parse_opts_ = s_store->get_parse_options();
    if (s_store->get_include_auto_discovery()) {
        repository_index_p = std::make_shared<repository_index>();
    }
    scan_repository();
    analyze_dir();
}




/// This method analyses the Repository_walker target directory, mapping out useful things (like module dependencies, script location, etc.)
///
/// The repository must have been scanned first (see scan_repository); the analysis
/// phase iterates the resulting file list instead of walking the directory tree.
void Repository_walker::analyze_dir() {
    // Snapshot the cached hashes before any file is re-parsed, so include-change
    // detection compares against the state the cache was in at the start of this
    // run (the main pass updates cache entries as it goes).
    for (auto &file: scanned_files) {
        previous_hashes[file.string()] = d_store->get_hash(file.string());
    }

    // Seed the compilation-order macro table from persisted per-file entries
    // (cache-skipped files contribute without being re-parsed).
    seed_macro_table();
    table_at_seed_ = macro_table_.rendered_by_name();

    for (auto &file: scanned_files) {
        if(working_threads == 2*max_threads){
           this->collect_analysis_results();
        }
        analyze_file(file);
    }
    this->collect_analysis_results();
    run_macro_fixpoint();

    // Re-parse cache-hit files whose (transitive) includes changed.
    invalidate_stale_includes();
    run_macro_fixpoint();

    // Re-parse cache-skipped macro consumers whose macros changed meaning.
    invalidate_stale_macros();
    run_macro_fixpoint();

    // Revalidate branch decisions taken with partial knowledge.
    validation_sweep();
    run_macro_fixpoint();

    report_quarantine_errors();
    d_store->set_macro_table_fingerprint(macro_table_fingerprint());
}

void Repository_walker::seed_macro_table() {
    for (const auto &[path, defs] : d_store->get_all_macro_definitions()) {
        preprocessor::macro_definitions_map live;
        for (const auto &stored : defs) live[stored.name] = macro_to_live(stored);
        macro_table_.add_file_definitions(path, live);
    }
}

std::string Repository_walker::macro_table_fingerprint() const {
    return hash_file(macro_table_.canonical_string()).value_or("");
}

/// Scan the repository once, building the repository index (basename -> paths) and the list of
/// files to be analyzed. The same exclusion rules used by the analysis phase are applied, so
/// the scan and the parse operate on the exact same file set.
void Repository_walker::scan_repository() {
    std::error_code ec;
    const std::filesystem::recursive_directory_iterator end;
    auto p_iter = std::filesystem::recursive_directory_iterator(target_repository, ec);
    while (p_iter != end && !ec) {

        auto path = p_iter->path();
        if(std::filesystem::is_directory(path, ec)){
            if(is_excluded_directory(path) || contains_excluding_file(path)){
                p_iter.disable_recursion_pending();
            }
        } else{
            if(file_is_verilog(path)){
                if (repository_index_p) repository_index_p->add_file(path);
                scanned_files.push_back(path);
            } else if(file_is_vhdl(path) || file_is_script(path) || file_is_constraint(path) || file_is_data(path)){
                scanned_files.push_back(path);
            }
        }
        p_iter.increment(ec);
    }
    if (ec) spdlog::warn("Error walking repository {}: {}", target_repository, ec.message());
}

void Repository_walker::collect_analysis_results() {
    pool.wait_for_tasks();

    for (auto &f : hdl_futures) {
        try {
            auto ctx = f.get();
            if (ctx.cache_skipped) {
                if (!ctx.path.empty()) file_hashes[ctx.path] = ctx.hash;
                continue;
            }
            if (ctx.quarantined) {
                // Accumulate needs across passes (merging call-site arities
                // per macro): each attempt only observes the unknowns visible
                // past previously injected ones.
                auto &entry = quarantine_[ctx.path];
                for (const auto &[name, arities] : ctx.undefined_macros) {
                    entry.undefined_macros[name].insert(arities.begin(), arities.end());
                }
                entry.unknown_conditionals.insert(ctx.unknown_conditionals.begin(),
                                                  ctx.unknown_conditionals.end());
                entry.hash = ctx.hash;
                entry.last_undefined = ctx.undefined_macros;
                entry.last_unknowns = ctx.unknown_conditionals;
                entry.last_error = ctx.error_detail;
                macro_table_.add_file_definitions(ctx.path, ctx.harvested);
                const size_t pending =
                    entry.undefined_macros.size() + entry.unknown_conditionals.size();
                spdlog::trace("Deferring {}: {} order-dependent macro(s) unresolved",
                              ctx.path, pending);
                continue;
            }
            if (!ctx.path.empty()) {
                // Uniform rule: every analyzed file replaces its own table
                // entry (possibly empty), so stale bodies can never linger.
                macro_table_.add_file_definitions(ctx.path, ctx.harvested);
                quarantine_.erase(ctx.path);
                last_injections_.erase(ctx.path);
            }
            if (ctx.resource) {
                file_hashes[ctx.path] = ctx.hash;
                // Macro deps = injected names consumed here plus names that
                // were queried-unknown at parse time (branch-flip detection).
                std::set<std::string> needs(ctx.unknown_conditionals.begin(),
                                            ctx.unknown_conditionals.end());
                std::map<std::string, std::string> consumed;
                if (auto pit = pending_injections_.find(ctx.path); pit != pending_injections_.end()) {
                    for (const auto &[name, def] : pit->second) {
                        needs.insert(name);
                        consumed[name] = macro_canonical_body(def);
                    }
                    pending_injections_.erase(pit);
                }
                parsed_unknowns_[ctx.path] = ctx.unknown_conditionals;
                parsed_consumed_[ctx.path] = std::move(consumed);
                freshly_parsed_.insert(ctx.path);
                std::vector<stored_macro_def> stored_defs;
                for (const auto &[name, def] : ctx.harvested) {
                    stored_defs.push_back(macro_to_stored(name, def));
                }
                std::vector<std::string> deps(needs.begin(), needs.end());
                d_store->store_file({ctx.path, ctx.hash, ctx.resource.value(),
                                     ctx.includes, stored_defs, deps});
            }
        } catch (const std::exception &e) {
            spdlog::error("Error analyzing a file: {}", e.what());
        } catch (...) {
            spdlog::error("Unknown error while analyzing a file");
        }
    }
    hdl_futures.clear();
    auto store = [this](auto &futures) {
        for(auto &f : futures) {
            try {
                auto ctx = f.get();
                if (!ctx.path.empty()) file_hashes[ctx.path] = ctx.hash;
                if (ctx.resource) d_store->store_file({ctx.path, ctx.hash, ctx.resource.value(), ctx.includes});
            } catch (const std::exception &e) {
                spdlog::error("Error analyzing a file: {}", e.what());
            } catch (...) {
                spdlog::error("Unknown error while analyzing a file");
            }
        }
        futures.clear();
    };
    store(scripts_futures);
    store(constraints_futures);
    store(data_futures);

    working_threads =0;
}

/// After the main analysis pass, evict and re-parse any file that was served
/// from the cache (own hash unchanged) but whose transitive include files have
/// changed since it was cached. The include sets are transitive, so a single
/// hash comparison per include is sufficient.
void Repository_walker::invalidate_stale_includes() {
    std::vector<std::filesystem::path> stale;
    for (auto &file: scanned_files) {
        if (!file_is_verilog(file)) continue;
        auto path = file.string();
        // Own hash changed: the file was already re-parsed in the main pass.
        if (file_hashes[path] != previous_hashes[path]) continue;
        auto includes = d_store->get_includes(path);
        if (!includes.has_value()) continue;
        for (auto &inc: includes.value()) {
            auto current = file_hashes.find(inc.path);
            if (current == file_hashes.end()) continue;  // external/unscanned include: cannot track
            if (current->second != previous_hashes[inc.path]) {
                stale.push_back(file);
                break;
            }
        }
    }

    for (auto &file: stale) {
        spdlog::trace("Invalidating {}: an included file changed", file.string());
        d_store->evict_file(file.string());
        analyze_file(file);
    }
    if (!stale.empty()) this->collect_analysis_results();
}

/// Bounded fixpoint over quarantined files: each round re-submits every file
/// whose injection changed since its last attempt (the table only gains
/// entries from other files' harvests, so identical injections would repeat
/// the identical outcome). Termination is structural: attempts are
/// deduplicated by injection content and rounds are capped.
void Repository_walker::run_macro_fixpoint() {
    // Operates on the whole repo at once: pass 1 (the caller) always spans
    // every scanned file, so by the time this runs the table holds complete
    // knowledge and one round resolves everything resolvable. Further rounds
    // only serve revelation chains (an injected macro's expansion revealing
    // the next unknown).
    const size_t initial = quarantine_.size();
    if (initial == 0) return;
    int pass = 0;
    while (!quarantine_.empty() && pass < max_macro_passes) {
        std::vector<std::pair<std::filesystem::path, preprocessor::macro_definitions_map>> work;
        for (const auto &[path, entry] : quarantine_) {
            preprocessor::undefined_uses_map needs;
            for (const auto &[name, arities] : entry.undefined_macros) {
                needs[name].insert(arities.begin(), arities.end());
            }
            for (const auto &name : entry.unknown_conditionals) {
                needs.try_emplace(name);
            }
            auto injection = macro_table_.build_injection(needs, path);
            auto last = last_injections_.find(path);
            if (last != last_injections_.end() && last->second == injection.definitions) continue;
            work.emplace_back(std::filesystem::path(path), injection.definitions);
        }
        // No file's injection changed: further passes would repeat identical
        // attempts. This is the only early exit: table/quarantine-size
        // stability must NOT stop the loop, since revelation chains keep
        // quarantine size and table contents static while newly revealed
        // macros change injections every round.
        if (work.empty()) break;
        for (auto &[file, injected] : work) {
            if (working_threads == 2 * max_threads) collect_analysis_results();
            last_injections_[file.string()] = injected;
            analyze_file_with_injection(file, injected);
        }
        collect_analysis_results();
        ++pass;
    }
    spdlog::trace("Macro fixpoint: {} quarantined repo-wide, {} remaining after {} pass(es)",
                  initial, quarantine_.size(), pass);
}

/// Revalidate parses whose branch decisions were taken with partial
/// knowledge: a consumed macro body that no longer matches the final table,
/// or a recorded-unknown conditional that is now a known macro, means the
/// file must be re-parsed with current knowledge.
void Repository_walker::validation_sweep() {
    if (macro_table_.empty()) return;
    std::vector<std::filesystem::path> drifted;
    for (const auto &path : freshly_parsed_) {
        bool drift = false;
        if (auto cit = parsed_consumed_.find(path); cit != parsed_consumed_.end()) {
            for (const auto &[name, body] : cit->second) {
                auto judgement = macro_table_.judge(name, {}, path);
                if (judgement.state != macro_table::verdict::kind::unique ||
                    macro_canonical_body(judgement.definition) != body) {
                    drift = true;
                    break;
                }
            }
        }
        if (!drift) {
            // Unknown conditionals only matter when the re-parse would see
            // something new: same self-exclusion as injection (a file's own
            // include guards must never flip its branches).
            if (auto uit = parsed_unknowns_.find(path); uit != parsed_unknowns_.end()) {
                preprocessor::undefined_uses_map probe_needs;
                for (const auto &name : uit->second) probe_needs.try_emplace(name);
                auto probe = macro_table_.build_injection(probe_needs, path);
                if (!probe.definitions.empty()) drift = true;
            }
        }
        if (drift) drifted.emplace_back(path);
    }
    for (auto &file : drifted) {
        spdlog::trace("Revalidating {}: repository macros changed under it", file.string());
        d_store->evict_file(file.string());
        last_injections_.erase(file.string());
        // Seeds carry no arities; the re-parse records fresh call shapes and
        // the following fixpoint round applies arity preference again.
        preprocessor::undefined_uses_map needs;
        if (auto uit = parsed_unknowns_.find(file.string()); uit != parsed_unknowns_.end()) {
            for (const auto &name : uit->second) needs.try_emplace(name);
        }
        if (auto cit = parsed_consumed_.find(file.string()); cit != parsed_consumed_.end()) {
            for (const auto &[name, ignored] : cit->second) {
                (void)ignored;
                needs.try_emplace(name);
            }
        }
        auto injection = macro_table_.build_injection(needs, file.string());
        if (working_threads == 2 * max_threads) collect_analysis_results();
        last_injections_[file.string()] = injection.definitions;
        analyze_file_with_injection(file, injection.definitions);
    }
    if (!drifted.empty()) collect_analysis_results();
}

/// Re-parse cache-skipped macro consumers whose macros changed meaning since
/// their parse: a consumed or queried-unknown name that was added, removed,
/// or redefined invalidates the stored parse.
void Repository_walker::invalidate_stale_macros() {
    const auto current = macro_table_.rendered_by_name();
    std::set<std::string> changed;
    for (const auto &[name, rendered] : current) {
        auto it = table_at_seed_.find(name);
        if (it == table_at_seed_.end() || it->second != rendered) changed.insert(name);
    }
    for (const auto &[name, ignored] : table_at_seed_) {
        (void)ignored;
        if (!current.contains(name)) changed.insert(name);
    }
    if (changed.empty()) return;
    bool submitted = false;
    for (auto &file : scanned_files) {
        if (!file_is_verilog(file)) continue;
        const std::string path = file.string();
        if (freshly_parsed_.contains(path)) continue;
        const auto deps = d_store->get_macro_dependencies(path);
        bool stale = false;
        for (const auto &dep : deps) {
            if (changed.contains(dep)) {
                stale = true;
                break;
            }
        }
        if (!stale) continue;
        spdlog::trace("Invalidating {}: a consumed macro changed", path);
        preprocessor::undefined_uses_map seed_needs;
        for (const auto &dep : deps) seed_needs.try_emplace(dep);
        d_store->evict_file(path);
        last_injections_.erase(path);
        auto injection = macro_table_.build_injection(seed_needs, path);
        if (working_threads == 2 * max_threads) collect_analysis_results();
        last_injections_[path] = injection.definitions;
        analyze_file_with_injection(file, injection.definitions);
        submitted = true;
    }
    if (submitted) collect_analysis_results();
}

std::vector<std::string> Repository_walker::build_quarantine_report(
    const macro_table &table,
    const std::map<std::string, quarantine_entry> &quarantine) {
    // One line per distinct problem (not per file): the same conflict
    // repeats for every consumer (270 files x 3 UVM-vs-shim macros), which
    // buries real information in a wall of identical lines.
    std::map<std::string, std::pair<std::string, std::vector<std::string>>> groups;
    auto add = [&](std::string key, std::string message, const std::string &path) {
        auto &slot = groups[std::move(key)];
        if (slot.second.empty()) slot.first = std::move(message);
        slot.second.push_back(path);
    };
    for (const auto &[path, entry] : quarantine) {
        // Report the final attempt's misses, not the accumulated union:
        // every pass only sees unknowns past previously injected ones, so
        // the union mixes long-resolved names with current ones and would
        // blame macros that already expanded fine. Injection still uses the
        // union (per-macro arity merging); reporting uses final state.
        // Blocker-first: absent, mismatched, conflicting and shadowed names
        // doom the file regardless of the rest; resolvable names held back
        // by those blockers are collateral, reported only when nothing else
        // blocks.
        bool blocked = false;
        for (const auto &[name, arities] : entry.last_undefined) {
            auto judgement = table.judge(name, arities, path);
            using kind = macro_table::verdict::kind;
            if (judgement.state == kind::conflict) {
                blocked = true;
                std::string definers;
                for (const auto &rep : table.representative_definers(name, judgement.candidates)) {
                    if (!definers.empty()) definers += ", ";
                    definers += rep;
                }
                if (definers.empty()) {
                    for (const auto &candidate : judgement.candidates) {
                        if (!definers.empty()) definers += ", ";
                        definers += candidate.path;
                    }
                }
                add("conflict|" + name + "|" + definers,
                    "macro " + name + " has conflicting definitions in: " + definers, path);
            } else if (judgement.state == kind::mismatch) {
                blocked = true;
                std::string observed;
                for (int arity : arities) {
                    if (!observed.empty()) observed += ", ";
                    observed += std::to_string(arity);
                }
                std::string definers;
                for (const auto &candidate : judgement.candidates) {
                    if (!definers.empty()) definers += ", ";
                    definers += macro_table::describe_candidate(candidate);
                }
                add("mismatch|" + name + "|" + observed + "|" + definers,
                    "macro " + name + " called with " + observed + " argument(s), "
                    "but no repository definition accepts that (defined in: " + definers + ")", path);
            } else if (judgement.state == kind::absent) {
                blocked = true;
                add("absent|" + name,
                    "macro " + name + " is not defined in the repository "
                    "(defined in no analyzed file)", path);
            } else if (judgement.state == kind::shadowed) {
                blocked = true;
                add("shadowed|" + name,
                    "macro " + name + " is used before its definition in the same file", path);
            }
        }
        if (!blocked) {
            if (!entry.last_undefined.empty()) {
                for (const auto &[name, arities] : entry.last_undefined) {
                    (void)arities;
                    auto judgement = table.judge(name, arities, path);
                    if (judgement.state == macro_table::verdict::kind::unique &&
                        !judgement.candidates.empty()) {
                        add("unique|" + name + "|" + judgement.candidates.front().path,
                            "macro " + name + " defined in " + judgement.candidates.front().path +
                            " could not be resolved within " + std::to_string(max_macro_passes) +
                            " fixpoint passes", path);
                    }
                }
            } else {
                // Macros converged but the file still fails: the fixpoint
                // cannot help (e.g. an unsupported construct in the expanded
                // text). Report the analyzer's own failure reason instead of
                // macro blame, plus any conditionals that never resolved.
                std::string unknowns;
                for (const auto &n : entry.last_unknowns) {
                    auto j = table.judge(n, {}, path);
                    if (j.state != macro_table::verdict::kind::absent) continue;
                    if (!unknowns.empty()) unknowns += ", ";
                    unknowns += n;
                }
                if (entry.last_error.empty() && unknowns.empty()) continue;
                std::string message;
                if (unknowns.empty()) {
                    message = entry.last_error;
                } else if (entry.last_error.empty()) {
                    message = "preprocessor conditionals never resolved: " + unknowns;
                } else {
                    message = entry.last_error + " (unresolved preprocessor conditionals: " + unknowns + ")";
                }
                add("converged|" + message, message, path);
            }
        }
    }
    std::vector<std::string> lines;
    for (auto &[key, group] : groups) {
        (void)key;
        const auto &paths = group.second;
        std::string sample;
        for (size_t i = 0; i < paths.size() && i < 3; ++i) {
            if (!sample.empty()) sample += ", ";
            sample += paths[i];
        }
        if (paths.size() > 3) sample += ", and " + std::to_string(paths.size() - 3) + " more";
        const std::string head = paths.size() == 1 ? "Error analyzing 1 file ["
                                                   : "Error analyzing " + std::to_string(paths.size()) +
                                                     " files [";
        lines.push_back(head + sample + "]: " + group.first);
    }
    return lines;
}

void Repository_walker::report_quarantine_errors() {
    for (const auto &line : build_quarantine_report(macro_table_, quarantine_)) {
        spdlog::error("{}", line);
    }
}


/// Check if the target directory needs to be skipped on the base of its name
/// \param dir Target directory
/// \return true if the directory needs to be skipped
bool Repository_walker::is_excluded_directory(const std::filesystem::path& dir) {
    auto norm_dir = dir.lexically_normal();

    for (const auto& excl_dir : excluded_directories) {
        if(excl_dir.starts_with('*')) {
            auto excl_name = excl_dir.substr(1, excl_dir.size() -1);
            if(norm_dir.filename() == excl_name) return true;
        }  else {
            if(norm_dir == std::filesystem::path(excl_dir).lexically_normal()) return true;
        }

    }
    return false;

}


/// Check if the target directory needs to be skipped on the base of its content
/// \param dir Target directory
/// \return true if the directory needs to be skipped
bool Repository_walker::contains_excluding_file(const std::filesystem::path &dir) {
    try {
        for(auto& p: std::filesystem::directory_iterator(dir)){
            if(!std::filesystem::is_directory(p.path())){
                bool is_excluded = excluding_extensions.find(p.path().extension()) != excluding_extensions.end();
                if(p.path().filename() == ignore_file_name){
                    this->read_ignore_file(p.path());
                    return true;
                }
                if(is_excluded) return true;
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        spdlog::warn("Error iterating directory {}: {}", dir.string(), e.what());
        return false;
    }
    return false;
}

void Repository_walker::read_ignore_file(const std::filesystem::path &file) {
    std::ifstream content(file);
    std::string ignore_line;
    std::string ignore_path;
    while (std::getline(content, ignore_line)){
        ignore_path = file.parent_path().string() + "/" + ignore_line;
        if(std::filesystem::is_directory(ignore_path)) excluded_directories.insert(ignore_path);
    }

}



/// File analysis Dispatcher
/// \param file File to analyze
///
/// This method is a simple dispatcher that based on the file type calls the appropriate analysis method.
void Repository_walker::analyze_file(std::filesystem::path &file) {
    if(!(file_is_verilog(file) || file_is_vhdl(file) || file_is_constraint(file)|| file_is_script(file) || file_is_data(file))) return;

    spdlog::trace("Analizing file: {}", file.string());
    if(file_is_verilog(file)){
        auto old_hash = d_store->get_hash(file);
        hdl_futures.push_back(pool.submit(analyze_verilog, file, parse_opts_, old_hash, repository_index_p, preprocessor::macro_definitions_map{}));
        working_threads++;
    } else if(file_is_script(file)){
        std::set<std::string> includes;
        auto old_hash = d_store->get_hash(file);
        scripts_futures.push_back(pool.submit(analyze_script, file, includes, old_hash));
        working_threads++;
    } else if(file_is_vhdl(file)){
        auto old_hash = d_store->get_hash(file);
        hdl_futures.push_back(pool.submit(analyze_vhdl, file, parse_opts_.include_directories, old_hash));
        working_threads++;
    } else if(file_is_constraint(file)){
        std::set<std::string> includes;
        auto old_hash = d_store->get_hash(file);
        constraints_futures.push_back(pool.submit(analyze_constraint, file, includes, old_hash));
        working_threads++;
    } else if(file_is_data(file)){
        std::set<std::string> includes;
        auto old_hash = d_store->get_hash(file);
        data_futures.push_back(pool.submit(analyze_data, file, includes, old_hash));
        working_threads++;
    }

}

void Repository_walker::analyze_file_with_injection(std::filesystem::path &file,
                                                    const preprocessor::macro_definitions_map &injected) {
    if (!file_is_verilog(file)) return;
    spdlog::trace("Analizing file: {}", file.string());
    auto old_hash = d_store->get_hash(file);
    pending_injections_[file.string()] = injected;
    hdl_futures.push_back(pool.submit(analyze_verilog, file, parse_opts_,
                                      old_hash, repository_index_p, injected));
    working_threads++;
}

/// Check if the target file appertains to the verilog language family
/// \param file Target file
/// \return True if the file is verilog
bool Repository_walker::file_is_verilog(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".svh" || extension == ".sv" || extension == ".vh" || extension == ".v";
}

/// Check if the target file appertains to the vhdl language family
/// \param file Target file
/// \return
bool Repository_walker::file_is_vhdl(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".vhd" || extension == ".vhdl";
}

/// Check if the target file is a recognized script (TCL and python)
/// \param file Target file
/// \return true if the file is a recognized script
bool Repository_walker::file_is_script(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".tcl" || extension == ".py";
}

/// Check if the target is a constrain file
/// \param file Target file
/// \return true if the file is a constrain file
bool Repository_walker::file_is_constraint(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".xdc";
}

bool Repository_walker::file_is_data(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".dat" || extension == ".mem";
}


/// Analyze the target verilog-type file to extract declared and used instantiated design elements
/// \param file Target file
/// \param injected Repository-learned macro definitions seeding this parse
///        (compilation-order macros; file-local and global defines win over it)
/// \param strip_uvm Strip undefined UVM/OVM macros (opt-in, off by default)
file_analysis_context<hdl_file> analyze_verilog(
    const std::filesystem::path &file,
    const parse_options &opts,
    const std::string &old_hash,
    const std::shared_ptr<repository_index> &idx,
    const preprocessor::macro_definitions_map &injected
) {
    spdlog::trace("PARSING: {}", file.c_str());
    try {

        auto f_opt = mm_file::try_open(file.string());
        if (!f_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not open file", file.string());
            return {};
        }
        auto hash_opt = hash_file(f_opt->view());
        if (!hash_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not calculate file hash", file.string());
            return {};
        }
        std::string hash = hash_opt.value();
        if (old_hash == hash) {
            file_analysis_context<hdl_file> skipped;
            skipped.path = file.string();
            skipped.hash = hash;
            skipped.cache_skipped = true;
            return skipped;
        }
        sv_analyzer file_processor;
        file_processor.set_options(opts);
        file_processor.set_repository_index(idx);
        file_processor.set_injected_definitions(injected);
        auto analysis = file_processor.analyze(file, f_opt->view());
        // Uniform harvest carrier: every analyzed file reports the
        // definitions its preprocessing collected (partial on abort), so the
        // collector can keep the macro table in sync unconditionally.
        file_analysis_context<hdl_file> ctx;
        ctx.path = file.string();
        ctx.hash = hash;
        ctx.undefined_macros = file_processor.get_undefined_macros();
        ctx.unknown_conditionals = file_processor.get_unknown_conditionals();
        ctx.harvested = file_processor.get_harvested_definitions();
        if (!analysis.has_value()) {
            // Quarantine instead of erroring when the ONLY thing missing is
            // repository-learnable: undefined macros and/or conditionals
            // queried while unknown. Anything fatal (bad includes, syntax,
            // macro misuse) errors out immediately, as before.
            if (!file_processor.has_fatal_error() &&
                (!ctx.undefined_macros.empty() || !ctx.unknown_conditionals.empty())) {
                ctx.quarantined = true;
                if (file_processor.has_error()) ctx.error_detail = file_processor.get_error();
                return ctx;
            }
            spdlog::error("Error analyzing {}: {}", file.string(), file_processor.get_error());
            return ctx;
        }
        auto includes = file_processor.get_includes();
        ctx.resource = analysis.value();
        ctx.includes = {includes.begin(), includes.end()};
        return ctx;
    } catch (const std::exception &err) {
        spdlog::error("Error analyzing {}: {}", file.string(), err.what());
        return {};
    } catch (...) {
        spdlog::error("Unknown error analyzing {}", file.string());
        return {};
    }
}
std::optional<std::string> hash_file(const std::string_view &file_content) {

    EVP_MD_CTX* context = EVP_MD_CTX_new();

    if(context != nullptr){
        if(EVP_DigestInit_ex(context, EVP_sha256(), nullptr)) {
            if(EVP_DigestUpdate(context, file_content.begin(), file_content.length())) {
                unsigned char hash[EVP_MAX_MD_SIZE];
                unsigned int lengthOfHash = 0;

                if(EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
                    std::stringstream ss;
                    for(unsigned int i = 0; i < lengthOfHash; ++i) {
                        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
                        if(i<lengthOfHash-1){
                            ss<<":";
                        }
                    }
                    EVP_MD_CTX_free(context);
                    return ss.str();
                }
            }
        }
    }
    EVP_MD_CTX_free(context);
    return std::nullopt;
};

/// Analyze the target vhdl-type file to extract declared and used instantiated design elements
/// \param file Target file
file_analysis_context<hdl_file> analyze_vhdl(
    const std::filesystem::path &file,
    std::set<std::string> i_d,
    const std::string &old_hash
) {
    try {
        auto f_opt = mm_file::try_open(file.string());
        if (!f_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not open file", file.string());
            return {};
        }
        auto hash_opt = hash_file(f_opt->view());
        if (!hash_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not calculate file hash", file.string());
            return {};
        }
        std::string hash = hash_opt.value();
        if (old_hash == hash) {
            return {file.string(), hash, std::nullopt};
        }
        vhdl_analyzer file_processor(file);
        file_processor.cleanup_content("");
        return {file.string(), hash, file_processor.analyze()};
    } catch (const std::exception &err) {
        spdlog::error("Error analyzing {}: {}", file.string(), err.what());
        return {};
    } catch (...) {
        spdlog::error("Unknown error analyzing {}", file.string());
        return {};
    }
}


/// Analyze the target Script extracting the necessary metadata
/// \param file Target file
file_analysis_context<DataFile> analyze_data(
    const std::filesystem::path &file,
    std::set<std::string> i_d,
    const std::string &old_hash
) {
    try {
        auto f_opt = mm_file::try_open(file.string());
        if (!f_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not open file", file.string());
            return {};
        }
        auto hash_opt = hash_file(f_opt->view());
        if (!hash_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not calculate file hash", file.string());
            return {};
        }
        std::string hash = hash_opt.value();
        if (old_hash == hash) {
            return {file.string(), hash, std::nullopt};
        }

        DataFile data(file.stem(), file.string());
        return {file.string(), hash, data};
    } catch (const std::exception &err) {
        spdlog::error("Error analyzing {}: {}", file.string(), err.what());
        return {};
    } catch (...) {
        spdlog::error("Unknown error analyzing {}", file.string());
        return {};
    }
}

/// Analyze the target Script extracting the necessary metadata
/// \param file Target file
file_analysis_context<Script> analyze_script(
    const std::filesystem::path &file,
    std::set<std::string> i_d,
    const std::string &old_hash
) {
    try {
        auto f_opt = mm_file::try_open(file.string());
        if (!f_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not open file", file.string());
            return {};
        }
        auto hash_opt = hash_file(f_opt->view());
        if (!hash_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not calculate file hash", file.string());
            return {};
        }
        std::string hash = hash_opt.value();
        if (old_hash == hash) {
            return {file.string(), hash, std::nullopt};
        }

        std::string ext = file.extension();
        if (!ext.empty() && ext[0] == '.') ext = ext.substr(1);
        script_specs s;
        s.name = file.stem();
        s.type = ext;
        Script scr(s);
        scr.set_path(file);
        return {file.string(), hash, scr};
    } catch (const std::exception &err) {
        spdlog::error("Error analyzing {}: {}", file.string(), err.what());
        return {};
    } catch (...) {
        spdlog::error("Unknown error analyzing {}", file.string());
        return {};
    }
}

/// Analyze the target constraint file extracting the necessary metadata
/// \param file Target file
file_analysis_context<Constraints> analyze_constraint(
    const std::filesystem::path &file,
    std::set<std::string> i_d,
    const std::string &old_hash
) {
    try {
        auto f_opt = mm_file::try_open(file.string());
        if (!f_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not open file", file.string());
            return {};
        }
        auto hash_opt = hash_file(f_opt->view());
        if (!hash_opt.has_value()) {
            spdlog::error("Error analyzing {}: could not calculate file hash", file.string());
            return {};
        }
        std::string hash = hash_opt.value();
        if (old_hash == hash) {
            return {file.string(), hash, std::nullopt};
        }
        Constraints constr(file.stem());
        constr.set_path(file);
        return {file.string(), hash, constr};
    } catch (const std::exception &err) {
        spdlog::error("Error analyzing {}: {}", file.string(), err.what());
        return {};
    } catch (...) {
        spdlog::error("Unknown error analyzing {}", file.string());
        return {};
    }
}




