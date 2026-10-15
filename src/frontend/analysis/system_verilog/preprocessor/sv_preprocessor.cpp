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


#include "frontend/analysis/system_verilog/preprocessor/sv_preprocessor.hpp"
#include "data_model/include_config.hpp"




namespace preprocessor {

// Nesting depth shared across instances on a thread (separate objects nest).
// Defined here so both include sites and the conditional path observe it.

namespace {
thread_local int pp_depth = 0;

// IEEE 1800 20.14 predefined coverage constants. Lowest-priority seed: any
// repository or file-local `define of the same name overrides them.
const std::pair<const char *, const char *> predefined_macros[] = {
    {"SV_COV_START", "0"},      {"SV_COV_STOP", "1"},       {"SV_COV_RESET", "2"},
    {"SV_COV_CHECK", "3"},      {"SV_COV_MODULE", "10"},    {"SV_COV_HIER", "11"},
    {"SV_COV_ASSERTION", "20"}, {"SV_COV_FSM_STATE", "21"}, {"SV_COV_STATEMENT", "22"},
    {"SV_COV_TOGGLE", "23"},    {"SV_COV_OVERFLOW", "-2"},  {"SV_COV_ERROR", "-1"},
    {"SV_COV_NOCOV", "0"},      {"SV_COV_OK", "1"},         {"SV_COV_PARTIAL", "2"},
};
}

// Position of the first `//` outside a string literal, or npos. A naive
// find("//") truncates lines like
//   $sformatf("csrr x%0d, 0x%0x // MSTATUS", tp, status)
// mid-string, leaving an unterminated literal and a lexer cascade downstream.
static std::string::size_type find_line_comment(const std::string &line) {
    bool in_string = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (in_string) {
            if (c == '\\' && i + 1 < line.size()) { ++i; continue; }
            if (c == '"') in_string = false;
        } else {
            if (c == '"') in_string = true;
            else if (c == '/' && i + 1 < line.size() && line[i + 1] == '/') return i;
        }
    }
    return std::string::npos;
}

// End of a `define body: first `//` outside a "..." or `"..."` literal.
// Per IEEE 1800 comments are stripped before directive processing, so a
// trailing `//` is never part of the replacement text (all commercial tools
// agree). A naive find("//") would corrupt bodies like "http://x", hence the
// string tracking (both quoting styles).
static std::string_view::size_type define_body_end(std::string_view line) {
    bool in_string = false;
    bool in_macro_string = false;
    for (size_t i = 0; i < line.size(); ++i) {
        char c = line[i];
        if (c == '\\' && i + 1 < line.size() && (in_string || in_macro_string)) { ++i; continue; }
        if (in_string) {
            if (c == '"') in_string = false;
        } else if (in_macro_string) {
            if (c == '`' && i + 1 < line.size() && line[i + 1] == '"') { in_macro_string = false; ++i; }
        } else {
            if (c == '`' && i + 1 < line.size() && line[i + 1] == '"') { in_macro_string = true; ++i; }
            else if (c == '"') in_string = true;
            else if (c == '/' && i + 1 < line.size() && line[i + 1] == '/') return i;
        }
    }
    return std::string_view::npos;
}

// True when `t` starts with directive `name` as a whole word, so a user
// macro such as `timescale_foo is not mistaken for the directive.
static bool is_directive(std::string_view t, std::string_view name) {
    if (!t.starts_with(name)) return false;
    if (t.size() == name.size()) return true;
    const unsigned char c = static_cast<unsigned char>(t[name.size()]);
    return !(std::isalnum(c) || c == '_' || c == '$');
}

// IEEE 1800 22.7: `timescale time_unit / time_precision, each a magnitude of
// 1, 10 or 100 followed by one of s, ms, us, ns, ps, fs. Consumes one
// "<magnitude> <unit>" pair from the front of sv and returns its value in
// femtoseconds (at most 100 s = 1e17, so it always fits).
static std::optional<uint64_t> consume_time_value(std::string_view &sv) {
    auto skip_ws = [&sv] {
        while (!sv.empty() && (sv.front() == ' ' || sv.front() == '\t')) sv.remove_prefix(1);
    };
    skip_ws();
    size_t n = 0;
    while (n < sv.size() && sv[n] >= '0' && sv[n] <= '9') ++n;
    const auto magnitude = sv.substr(0, n);
    if (magnitude != "1" && magnitude != "10" && magnitude != "100") return std::nullopt;
    uint64_t femtoseconds = magnitude == "1" ? 1 : magnitude == "10" ? 10 : 100;
    sv.remove_prefix(n);
    skip_ws();
    n = 0;
    while (n < sv.size() && sv[n] >= 'a' && sv[n] <= 'z') ++n;
    const auto unit = sv.substr(0, n);
    uint64_t scale = 0;
    if (unit == "fs") scale = 1ULL;
    else if (unit == "ps") scale = 1'000ULL;
    else if (unit == "ns") scale = 1'000'000ULL;
    else if (unit == "us") scale = 1'000'000'000ULL;
    else if (unit == "ms") scale = 1'000'000'000'000ULL;
    else if (unit == "s") scale = 1'000'000'000'000'000ULL;
    else return std::nullopt;
    sv.remove_prefix(n);
    skip_ws();
    return femtoseconds * scale;
}

struct timescale_args {
    uint64_t unit_fs;
    uint64_t precision_fs;
};

// `args` is the text after the directive keyword, comments already stripped.
static std::optional<timescale_args> parse_timescale_args(std::string_view args) {
    auto unit = consume_time_value(args);
    if (!unit || args.empty() || args.front() != '/') return std::nullopt;
    args.remove_prefix(1);
    auto precision = consume_time_value(args);
    if (!precision || !args.empty()) return std::nullopt;
    return timescale_args{*unit, *precision};
}

// IEEE 1800 22.12: `line number "filename" level, with number a positive
// integer, filename a string literal and level 0, 1 or 2. `args` is the text
// after the directive keyword, comments already stripped.
struct line_args {
    uint64_t number;
    std::string file;
    int level;
};

static std::optional<line_args> parse_line_args(std::string_view args) {
    auto skip_ws = [&args] {
        while (!args.empty() && (args.front() == ' ' || args.front() == '\t')) args.remove_prefix(1);
    };
    skip_ws();
    size_t n = 0;
    while (n < args.size() && args[n] >= '0' && args[n] <= '9') ++n;
    // Positive, and short enough that the conversion cannot overflow.
    if (n == 0 || n > 18 || args.substr(0, n).find_first_not_of('0') == std::string_view::npos) return std::nullopt;
    line_args result{};
    result.number = std::stoull(std::string(args.substr(0, n)));
    args.remove_prefix(n);
    skip_ws();
    if (args.empty() || args.front() != '"') return std::nullopt;
    size_t i = 1;
    while (i < args.size() && args[i] != '"') i += (args[i] == '\\' ? 2 : 1);
    if (i >= args.size()) return std::nullopt;
    result.file = std::string(args.substr(1, i - 1));
    args.remove_prefix(i + 1);
    skip_ws();
    if (args.size() != 1 || args[0] < '0' || args[0] > '2') return std::nullopt;
    result.level = args[0] - '0';
    return result;
}

// Include nesting depth is orthogonal to the cycle guards below: guards catch
// repeats, but a deep *acyclic* chain (machine-generated file lists) would
// still smash the stack with no diagnostic. Same backstop pattern as the
// conditional-nesting cap.
namespace {
thread_local int include_nesting_depth = 0;
struct include_depth_guard {
    include_depth_guard() { ++include_nesting_depth; }
    ~include_depth_guard() { --include_nesting_depth; }
};
}  // namespace
static constexpr int MAX_INCLUDE_DEPTH = 256;

    std::string sv_preprocessor::normalize_include_path(const std::string &p) {
        return std::filesystem::path(p).lexically_normal().string();
    }

    static std::optional<std::pair<uint64_t, uint64_t>> file_identity(const std::string &p) {
        struct stat st{};
        if (::stat(p.c_str(), &st) != 0) return std::nullopt;
        return std::make_pair(static_cast<uint64_t>(st.st_dev), static_cast<uint64_t>(st.st_ino));
    }

    bool sv_preprocessor::claim_include_file(const std::string &p) {
        auto id = file_identity(p);
        if (!id.has_value()) return false;
        if (active_inodes.contains(id.value())) return true;
        active_inodes.insert(id.value());
        return false;
    }

    void sv_preprocessor::release_include_file(const std::string &p) {
        auto id = file_identity(p);
        if (id.has_value()) active_inodes.erase(id.value());
    }

    void sv_preprocessor::report_error(const std::string &msg) {
        spdlog::error(msg);
        if (!error) error = msg;
        fatal_error = true;
    }

    void sv_preprocessor::check_timescale(std::string_view directive_line, macro_processor *macro_engine) {
        auto args = directive_line.substr(sizeof("`timescale") - 1);
        std::string expanded;
        // Arguments may be macro-built (`timescale `TU / `TP). Text that came
        // out of an expansion is already fully expanded, so no engine there.
        if (macro_engine != nullptr && args.find('`') != std::string_view::npos) {
            expanded = macro_engine->process_macro(args);
            if (fatal_error) return;
            args = expanded;
        }
        auto parsed = parse_timescale_args(macro_processor::trim(args));
        if (!parsed.has_value()) {
            report_error(fmt::format("Invalid `timescale [{}] at line {} in file: {}",
                                     macro_processor::trim(directive_line), line_number, path));
            return;
        }
        if (semantic_checker.check_timescale(
                path, static_cast<unsigned int>(line_number), parsed->unit_fs,
                parsed->precision_fs, macro_processor::trim(directive_line))) {
            // Already logged by the checker; only flag the failure.
            if (!error) error = semantic_checker.last_violation().message;
            fatal_error = true;
        }
    }

    void sv_preprocessor::check_line(std::string_view directive_line, macro_processor *macro_engine) {
        auto args = directive_line.substr(sizeof("`line") - 1);
        std::string expanded;
        if (macro_engine != nullptr && args.find('`') != std::string_view::npos) {
            expanded = macro_engine->process_macro(args);
            if (fatal_error) return;
            args = expanded;
        }
        auto parsed = parse_line_args(macro_processor::trim(args));
        if (!parsed.has_value()) {
            report_error(fmt::format("Invalid `line [{}] at line {} in file: {}",
                                     macro_processor::trim(directive_line), line_number, path));
            return;
        }
        // The line after the directive becomes line `number` of `file`.
        remap.offset = static_cast<int64_t>(parsed->number) - static_cast<int64_t>(line_number + 1);
        remap.file = parsed->file;
    }

    void sv_preprocessor::record_unknown_conditional(const std::string_view &name) {
        auto trimmed = macro_processor::trim(name);
        if (!trimmed.empty()) unknown_conditionals.emplace(trimmed);
    }

    const std::string& sv_preprocessor::get_error() const {
        if (error.has_value()) return error.value();
        if (deferred_error.empty() && !undefined_macros.empty()) {
            std::string names;
            for (const auto &[id, ignored] : undefined_macros) {
                (void)ignored;
                if (!names.empty()) names += ", ";
                names += id;
            }
            deferred_error =
                fmt::format("Undefined macro(s) {} in file {}", names, path);
        }
        return deferred_error;
    }

    std::string sv_preprocessor::preprocess(const std::string_view &file_content, unsigned int initial_output_line) {
        // Seed order: repository base < global defines < file-local `defines.
        // Seeding happens only at the top-level entry: recursive include
        // processing shares this instance and must keep accumulated defines.
        definitions.clear();
        for (const auto &[name, value] : predefined_macros) definitions[name] = value;
        for (const auto &[name, def] : base_definitions) definitions[name] = def;
        locally_defined.clear();
        top_path = path;
        undefined_macros.clear();
        unknown_conditionals.clear();
        deferred_error.clear();
        active_inodes.clear();
        claim_include_file(path);
        return preprocess_body(file_content, initial_output_line);
    }

    std::string sv_preprocessor::preprocess_body(const std::string_view &file_content, unsigned int initial_output_line) {
        remap = {};
        macro_processor macro_engine(definitions , line_number, path, error, undefined_macros, fatal_error,
                                       opts_, remap);

        install_global_defines();
        auto flat_source= flatten_source(file_content);
        std::istringstream iss(flat_source);
        std::ostringstream out;
        output_line_n = initial_output_line;
        source_map.open_range(output_line_n, path);

        std::string line;
        line_number = 1;

        while (std::getline(iss, line)) {
            // Only fatal errors abort the scan: order-dependent unknowns only
            // record ids (quarantine), so scanning continues and one pass
            // collects every unknown and harvests every definition. Output
            // is discarded with the file.
            if (fatal_error) break;
            std::string_view trimmed_line = macro_processor::ltrim(line);

            bool skipped_directive = trimmed_line.starts_with("`resetall")
            || trimmed_line.starts_with("`timescale")
            || trimmed_line.starts_with("`default_nettype")
            || trimmed_line.starts_with("`nounconnected_drive")
            || trimmed_line.starts_with("`unconnected_drive")
            || trimmed_line.starts_with("`endcelldefine")
            || trimmed_line.starts_with("`celldefine")
            || trimmed_line.starts_with("`pragma")
            || trimmed_line.starts_with("`end_keywords")
            || trimmed_line.starts_with("`begin_keywords")
            || trimmed_line.starts_with("`line");
            std::string uncommented_line = line;
            // Gate the string-aware comment scan on a raw '/' hit first:
            // most code lines have no slash at all, and find('/') is memchr.
            if (line.find('/') != std::string::npos) {
                if (auto comment_pos = find_line_comment(line); comment_pos != std::string::npos) {
                    uncommented_line = line.substr(0, comment_pos);
                    std::string_view comment(line.data() + comment_pos, line.size() - comment_pos);
                    if (comment.contains("pragma translate_off")) disable_preprocessor = true;
                    if (comment.contains("pragma translate_on")) disable_preprocessor = false;
                }
            }
            if (disable_preprocessor) uncommented_line = "";

            // Judged on the comment-stripped line so translate_off regions and
            // trailing comments behave like every other directive.
            const auto active_line = macro_processor::ltrim(uncommented_line);
            if (is_directive(active_line, "`timescale") && c_solver.is_active()) {
                check_timescale(active_line, &macro_engine);
            } else if (is_directive(active_line, "`line") && c_solver.is_active()) {
                check_line(active_line, &macro_engine);
            } else if (trimmed_line.starts_with("`define") && c_solver.is_active()) {
                parse_definition(trimmed_line, 7);
            } else if (trimmed_line.starts_with("`include") && c_solver.is_active()) {

                auto included_file = parse_include_path(trimmed_line);
                if (included_file.has_value()) {
                    includes.insert(included_file.value());
                    {
                        // Ancestor re-entry (including guarded mutual includes
                        // such as A includes B includes A behind an ifndef
                        // guard) is reprocessed so the guards evaluate
                        // naturally through the shared definitions map; the
                        // guarded body then yields empty as designed.
                        // Guardless true cycles are bounded by the depth cap
                        // below instead of crashing the stack.
                        auto saved_path = path;
                        auto saved_line = line_number;
                        auto saved_remap = remap;
                        path = included_file.value().path;
                        active_includes.insert(path);
                        source_map.close_range(output_line_n);

                        auto f_opt = mm_file::try_open(path);
                        if (!f_opt.has_value()) {
                            report_error(fmt::format("Could not open include file: {}", path));
                        } else if (include_nesting_depth >= MAX_INCLUDE_DEPTH) {
                            report_error(fmt::format("Maximum include depth ({}) exceeded including file {} in file {}",
                                                     MAX_INCLUDE_DEPTH, path, saved_path));
                        } else {
                            claim_include_file(path);
                            include_depth_guard depth_guard;
                            auto content = preprocess_body(f_opt->view(), output_line_n);
                            if (!error) { out << content; out.put('\n'); }
                            release_include_file(path);
                        }

                        active_includes.erase(path);
                        line_number = saved_line;
                        remap = saved_remap;
                        path = saved_path;
                        source_map.open_range(output_line_n, path);
                    }
                }
            } else if (trimmed_line.starts_with("`ifdef")) {
                auto condition = std::string(macro_processor::trim(parse_one_arg_directive(trimmed_line, 6)));
                if (!definitions.contains(condition)) record_unknown_conditional(condition);
                c_solver.start_loop(definitions.contains(condition));
            } else if (trimmed_line.starts_with("`ifndef")) {
                auto condition = std::string(macro_processor::trim(parse_one_arg_directive(trimmed_line, 7)));
                if (!definitions.contains(condition)) record_unknown_conditional(condition);
                c_solver.start_loop(!definitions.contains(condition));
            } else if (trimmed_line.starts_with("`elsif")) {
                auto condition = std::string(macro_processor::trim(parse_one_arg_directive(trimmed_line, 6)));
                if (!definitions.contains(condition)) record_unknown_conditional(condition);
                c_solver.advance_elseif(definitions.contains(condition));
            } else if (trimmed_line.starts_with("`else")) {
                c_solver.advance_else();
            } else if (trimmed_line.starts_with("`endif")) {
                c_solver.close_loop();
            } else if (trimmed_line.starts_with("`undef") && c_solver.is_active()) {
                auto identifier = parse_one_arg_directive(trimmed_line, 6);
                definitions.erase(std::string(identifier));
            } else if (trimmed_line.starts_with("`undefineall") && c_solver.is_active()) {
                definitions.clear();
            } else if (!skipped_directive && c_solver.is_active()) {
                std::string output_line;
                if (uncommented_line.empty()) {
                    output_line = '\n';
                } else {
                    if (uncommented_line.contains('`')) {
                        uncommented_line = gather_multi_line_macro(uncommented_line, iss);
                    }

                    // 1. Expand the macro normally
                    output_line =  post_process_macro_expansion(macro_engine.process_macro(uncommented_line) + '\n');
                }
                output_line_n += std::count(output_line.begin(), output_line.end(), '\n');
                out << output_line;
            }
            line_number++;
        }
        auto retval = out.str();
        if (!retval.empty()) retval.pop_back();
        source_map.close_range(output_line_n);
        return retval;
    }


    void sv_preprocessor::install_global_defines() {
        for (const auto &d: opts_.defines) {
            auto eq = d.find('=');
            if (eq == std::string::npos) {
                definitions[d] = "1";
            } else {
                auto name = d.substr(0, eq);
                auto value = d.substr(eq + 1);
                value.erase(0, value.find_first_not_of("\t "));
                definitions[name] = value;
            }
        }
    }

    std::optional<include_dependency> sv_preprocessor::parse_include_path(const std::string_view &line) {
        auto start_identifier = line.find_first_of("\"<");
        if (start_identifier == std::string_view::npos) {
            report_error(fmt::format("Malformed include [{}] at line {} in file: {}", line,line_number, path));
            return std::nullopt;
        }else if (line[start_identifier] == '\"') {
            auto end_identifier = line.substr(start_identifier+1).find_first_of('"');
            auto name = line.substr(start_identifier+1, end_identifier);
            if (name.starts_with('/')) return include_dependency{normalize_include_path(std::string(name)), include_resolution::regular};
            const std::string parent_dir = std::filesystem::path(path).parent_path().string();
            std::string cache_key;
            cache_key.reserve(2 + parent_dir.size() + name.size());
            cache_key.append("q:");
            cache_key.append(parent_dir);
            cache_key.push_back('\0');
            cache_key.append(name);
            if (auto hit = include_resolution_cache.find(cache_key); hit != include_resolution_cache.end()) {
                return hit->second;
            }
            // Intentionally-absent headers (`includes.ignored`, exact
            // basename): skip silently — no warning, no error, no edge.
            // Quoted-only: the angle branch below never consults the list.
            if (is_ignored_include(std::string(name))) {
                spdlog::trace("Ignoring include file {} at line {} in file: {} (ignored_includes)",
                              std::string(name), line_number, path);
                return std::optional<include_dependency>{};
            }
            std::string full_path;
            auto rel_path = std::string(std::filesystem::path(parent_dir)/name);
            if (std::filesystem::exists(rel_path)) {
                full_path = rel_path;
            } else {
                for (std::filesystem::path dir: opts_.include_directories) {
                    auto tmp_path = std::string(dir/name);
                    if (std::filesystem::exists(tmp_path)) { full_path = tmp_path; break; }
                }
            }
            std::optional<include_dependency> resolved;
            if (full_path.empty()) {
                auto discovered = resolve_include(std::string(name));
                if (discovered.resolved.has_value()) {
                    discovered.resolved->path = normalize_include_path(discovered.resolved->path);
                    resolved = discovered.resolved.value();
                    include_resolution_cache.emplace(std::move(cache_key), resolved);
                } else if (!discovered.candidates.empty()) {
                    // Ambiguous: an explicit `includes.deconfliction` pick
                    // (scoped longest-prefix, else global) wins silently;
                    // otherwise report the candidates, not "not found".
                    // Skipped without failing the parse (quoted include).
                    // Not cached: keep per-occurrence warn/line info exact.
                    if (auto pick = resolve_deconflicted(std::string(name), discovered.candidates);
                        pick.has_value()) {
                        pick->path = normalize_include_path(pick->path);
                        resolved = pick.value();
                        include_resolution_cache.emplace(std::move(cache_key), resolved);
                    } else {
                        std::string candidate_dirs;
                        for (auto &c : discovered.candidates) {
                            candidate_dirs += "\n    " + c.parent_path().string();
                        }
                        spdlog::warn("include file {} is ambiguous ({} candidates), skipping include "
                                     "at line {} in file: {}{}", std::string(name),
                                     discovered.candidates.size(), line_number, path, candidate_dirs);
                        resolved = std::optional<include_dependency>{};
                    }
                } else {
                    // No candidates: a deconflicted target may still exist
                    // on disk without being indexed — try it before warning.
                    if (auto pick = resolve_deconflicted(std::string(name), discovered.candidates);
                        pick.has_value()) {
                        pick->path = normalize_include_path(pick->path);
                        resolved = pick.value();
                        include_resolution_cache.emplace(std::move(cache_key), resolved);
                    } else {
                        spdlog::warn("include file not found: {} at line {} in file: {}", std::string(name), line_number, path);
                        resolved = std::optional<include_dependency>{};
                    }
                }
            } else {
                resolved = include_dependency{normalize_include_path(full_path), include_resolution::regular};
                include_resolution_cache.emplace(std::move(cache_key), resolved);
            }
            return resolved;
        } else {
            auto filename = std::string(line.substr(start_identifier+1, line.find_first_of('>')- start_identifier-1));
            std::string cache_key;
            cache_key.reserve(2 + filename.size());
            cache_key.append("a:");
            cache_key.append(filename);
            if (auto hit = include_resolution_cache.find(cache_key); hit != include_resolution_cache.end()) {
                return hit->second;
            }
            std::optional<include_dependency> resolved;
            bool found = false;
            std::string full_path;
            for (std::filesystem::path dir: opts_.include_directories) {
                auto candidate = dir / filename;
                if (std::filesystem::exists(candidate)) {
                    full_path = candidate.string();
                    found = true;
                    break;
                }
            }
            if (!found) {
                auto discovered = resolve_include(filename);
                if (discovered.resolved.has_value()) {
                    discovered.resolved->path = normalize_include_path(discovered.resolved->path);
                    resolved = discovered.resolved.value();
                    include_resolution_cache.emplace(std::move(cache_key), resolved);
                } else if (!discovered.candidates.empty()) {
                    std::string candidate_dirs;
                    for (auto &c : discovered.candidates) {
                        candidate_dirs += "\n    " + c.parent_path().string();
                    }
                    report_error(fmt::format("included file {} is ambiguous, candidates found in:{}",
                                             filename, candidate_dirs));
                    resolved = std::nullopt;
                } else {
                    // Not cached: report_error is fatal (aborts the scan),
                    // so caching it would be pointless anyway.
                    report_error(fmt::format("included file not found: {}", filename));
                    resolved = std::nullopt;
                }
            } else {
                resolved = include_dependency{normalize_include_path(full_path), include_resolution::regular};
                include_resolution_cache.emplace(std::move(cache_key), resolved);
            }
            return resolved;
        }
    }

    sv_preprocessor::include_lookup sv_preprocessor::resolve_include(const std::string &name) {
        include_lookup result;
        if (!repo_idx) return result;
        result.candidates = repo_idx->lookup(name);
        if (result.candidates.size() == 1) {
            result.resolved = include_dependency{result.candidates[0].string(),
                                                 include_resolution::auto_discovered};
        }
        return result;
    }

    bool sv_preprocessor::is_ignored_include(const std::string &name) const {
        if (opts_.ignored_includes.empty()) return false;
        const std::string base = std::filesystem::path(name).filename().generic_string();
        for (const auto &entry : opts_.ignored_includes) {
            const std::string ebase = std::filesystem::path(entry).filename().generic_string();
            if (!ebase.empty() && ebase == base) return true;
        }
        return false;
    }

    std::optional<include_dependency> sv_preprocessor::resolve_deconflicted(
        const std::string &name,
        const std::vector<std::filesystem::path> &candidates) {
        if (opts_.include_deconfliction.empty()) return std::nullopt;
        const include_header_resolution_opt *rule = nullptr;
        const std::string full_key = include_config::include_key_of(name);
        if (auto it = opts_.include_deconfliction.find(full_key); it != opts_.include_deconfliction.end()) {
            rule = &it->second;
        } else {
            const std::string base = include_config::basename_of(name);
            if (auto it = opts_.include_deconfliction.find(base); it != opts_.include_deconfliction.end()) {
                rule = &it->second;
            } else if (auto it2 = opts_.include_deconfliction.find(name); it2 != opts_.include_deconfliction.end()) {
                rule = &it2->second;
            }
        }
        if (rule == nullptr) return std::nullopt;

        auto match_candidate = [&](const std::string &target) -> std::optional<include_dependency> {
            for (const auto &c : candidates) {
                if (include_config::target_matches(c.generic_string(), target)) {
                    return include_dependency{normalize_include_path(c.string()),
                                              include_resolution::auto_discovered};
                }
            }
            // Target may exist on disk without being indexed (e.g. `.h`
            // headers are index-only, or the index predates the file).
            std::error_code ec;
            std::filesystem::path tp(target);
            if (!tp.is_absolute() && !opts_.repo_root.empty()) tp = std::filesystem::path(opts_.repo_root) / tp;
            if (std::filesystem::exists(tp, ec)) {
                return include_dependency{normalize_include_path(tp.string()),
                                          include_resolution::auto_discovered};
            }
            return std::nullopt;
        };

        // Scoped entries first: longest includer-dir prefix wins, so nested
        // scopes (rtl/foo/sub) inherit their parent unless overridden.
        if (!rule->scoped_targets.empty()) {
            const std::string includer_dir = include_config::includer_scope_of(path, opts_.repo_root);
            std::string best_key;
            std::string best_norm;
            for (const auto &[scope, ignored] : rule->scoped_targets) {
                (void)ignored;
                const std::string norm_scope = include_config::normalize_repo_path(scope);
                if (!includer_dir.empty() && include_config::scope_covers(norm_scope, includer_dir) &&
                    norm_scope.size() > best_norm.size()) {
                    best_norm = norm_scope;
                    best_key = scope;
                }
            }
            if (!best_key.empty()) {
                const std::string target = rule->scoped_targets.at(best_key);
                if (auto hit = match_candidate(target); hit.has_value()) {
                    spdlog::trace("Deconflicted include '{}' in '{}' (scope '{}') -> {}",
                                  name, path, best_norm, hit->path);
                    return hit;
                }
                spdlog::warn("include deconfliction for '{}' (scope '{}') target '{}' matches no candidate; "
                             "falling back to default resolution", name, best_norm, target);
                return std::nullopt;
            }
            // No scope covers this includer: fall through to global/default.
            if (rule->global_target.has_value()) {
                if (auto hit = match_candidate(*rule->global_target); hit.has_value()) {
                    spdlog::trace("Deconflicted include '{}' in '{}' (global fallback) -> {}",
                                  name, path, hit->path);
                    return hit;
                }
                spdlog::warn("include deconfliction for '{}' target '{}' matches no candidate; "
                             "falling back to default resolution", name, *rule->global_target);
            }
            return std::nullopt;
        }
        if (rule->global_target.has_value()) {
            if (auto hit = match_candidate(*rule->global_target); hit.has_value()) {
                spdlog::trace("Deconflicted include '{}' in '{}' -> {}", name, path, hit->path);
                return hit;
            }
            spdlog::warn("include deconfliction for '{}' target '{}' matches no candidate; "
                         "falling back to default resolution", name, *rule->global_target);
        }
        return std::nullopt;
    }

    std::string sv_preprocessor::get_define_replacement(const std::string_view &identifier) {
        std::string_view purged_identifier = {identifier.begin()+1, identifier.end()};
        std::string replacement;
        if (purged_identifier == "__FILE__") {
            replacement = "\"" + remap.file.value_or(path) + "\"";

        } else if (purged_identifier == "__LINE__"){
            replacement = std::to_string(static_cast<int64_t>(line_number) + remap.offset);
        } else {
            auto id = std::string(purged_identifier);
            if (!definitions.contains(id)) {
                report_error(fmt::format("{}:{} MACRO {} is not defined", path, line_number, id));
                return "";
            }
            auto def = definitions.at(id);
            if (std::holds_alternative<std::string>(def)) {
                replacement = std::get<std::string>(def);
            }
        }
        return replacement;
    }

    macro_definitions_map sv_preprocessor::get_harvested_definitions() const {
        macro_definitions_map harvested;
        for (const auto &[name, def] : definitions) {
            if (locally_defined.contains(name)) harvested[name] = def;
        }
        return harvested;
    }

    void sv_preprocessor::parse_definition(const std::string_view &sv, int prefix_length) {
        auto trimmed_view = sv.substr(prefix_length);
        // A trailing `//` comment is not part of the replacement text:
        // without this, `` `define MHPME_CLK_ACTIVE 6'd1 // OOP `` would
        // expand to `6'd1 // OOP` mid-expression, commenting out the rest of
        // the line at every use site. String-aware so bodies containing
        // `"http://..."` survive intact.
        if (auto body_end = define_body_end(trimmed_view); body_end != std::string_view::npos) {
            trimmed_view = macro_processor::trim(trimmed_view.substr(0, body_end));
        }
        auto first = trimmed_view.find_first_not_of("\t ");
        if (first == std::string_view::npos) {
            report_error(fmt::format("Malformed `define without identifier at line {} in file: {}", line_number, path));
            return;
        }
        trimmed_view = trimmed_view.substr(first);
        auto id_last = trimmed_view.find_first_of("\t (");
        auto identifier = trimmed_view.substr(0, id_last);
        // Textual-only harvest: defines reached through `include bodies
        // still enter `definitions` (this file's parse needs them) but are
        // not attributed to this file; the included file harvests them when
        // it is scanned itself as a top-level file.
        const bool lineal = (path == top_path);
        if (id_last == std::string_view::npos) {
            definitions[std::string(identifier)] = "";
            if (lineal) locally_defined.insert(std::string(identifier));
            return;
        }
        auto remaining_view = trimmed_view.substr(id_last+1);
        if (trimmed_view[id_last] == '(') {
            auto macro = macro_processor::parse_function_macro(trimmed_view.substr(id_last+1));
            if (!macro.has_value()) {
                report_error(fmt::format("The arguments list in macro {} is never closed [{}]", identifier, path));
                return;
            }
            definitions[std::string(identifier)] = macro.value();
            if (lineal) locally_defined.insert(std::string(identifier));
        } else {
            // Right-trim like the commented-define path above: trailing
            // whitespace is never significant after lexing, and the
            // replacement can become an identifier (e.g. a module name from
            // a cell-name define), which must not carry stray blanks.
            auto value_first = remaining_view.find_first_not_of("\t ");
            std::string value = (value_first == std::string_view::npos)
                ? ""
                : std::string{macro_processor::trim(remaining_view.substr(value_first))};
            definitions[std::string(identifier)] = value;
            if (lineal) locally_defined.insert(std::string(identifier));
        }

    }

    std::string_view sv_preprocessor::parse_one_arg_directive(const std::string_view &sv, int prefix_length) {
        auto trimmed_line = sv.substr(prefix_length);
        return trimmed_line.substr( trimmed_line.find_first_not_of(' '));
    }


    std::string sv_preprocessor::flatten_source(const std::string_view &content) {

        std::string result;
        result.reserve(content.size());
        size_t cursor = 0;
        bool in_string = false;
        bool in_macro = false;

        // Source flattening uses a search and append approach to be cache and SIMD friendly
        // The main loop keeps searching for potentially problematic characters and appends as needed
        while (cursor < content.size()) {

            size_t next_identifier = content.find_first_of("/\"\\`\n\r", cursor);

            if (next_identifier == std::string_view::npos) {
                result.append(content.substr(cursor));
                break;
            }

            if (next_identifier > cursor) {
                result.append(content.substr(cursor, next_identifier - cursor));
            }
            cursor = next_identifier;
            char trigger = content[cursor];

            if (trigger == '\n' || trigger == '\r') {
                in_macro = false;
                result.push_back(trigger);
                cursor++;
            } else if (trigger == '`') {
                auto directive = content.substr(cursor);
                if (directive.starts_with("`define")) {
                    in_macro = true;
                }
                result.push_back('`');
                cursor++;
            } else if (trigger == '/') {
                if (cursor != content.size()-1) {
                    if (content[cursor + 1] == '/') {
                        if (in_string) {
                            // A `//` inside a string literal is content, not a comment.
                            result.push_back('/');
                            cursor++;
                        } else if (in_macro) {
                            // Inside a multi-line `define a `//` comment ends at the
                            // physical newline: interior comment lines (e.g. the
                            // `// ... \` documentation inside OpenTitan assertion
                            // macros) are discarded without truncating the rest of
                            // the definition, while a trailing `\` still continues
                            // the directive. Outside defines the historical C-style
                            // continuation swallowing below is kept.
                            size_t com = cursor + 2;
                            while (com < content.size() && content[com] != '\n' && content[com] != '\r') com++;
                            size_t back = com;
                            while (back > cursor + 2 && (content[back - 1] == ' ' || content[back - 1] == '\t')) --back;
                            if (back > cursor + 2 && content[back - 1] == '\\') {
                                size_t next = com + 1;
                                if (next < content.size() && content[com] == '\r' && content[next] == '\n') ++next;
                                cursor = next;
                            } else {
                                cursor = com;
                            }
                            // Drop whitespace preceding the comment so the body keeps
                            // no trailing blanks (matches the old trim-via-truncation
                            // behavior for single-line `define trailing comments).
                            while (!result.empty() && (result.back() == ' ' || result.back() == '\t')) result.pop_back();
                        } else {
                            result.append("//");
                            cursor += 2;
                            while (cursor < content.size() && content[cursor] != '\n' && content[cursor] != '\r') {
                                if (cursor + 1 < content.size() && content[cursor] == '\\' && (content[cursor+1] == '\n' || content[cursor+1] == '\r')) {
                                    size_t next = content.find_first_of("\n\r", cursor + 1);
                                    if (next != std::string_view::npos) {
                                        cursor = next + (content[next] == '\r' && next + 1 < content.size() && content[next+1] == '\n' ? 2 : 1);
                                        continue; // Skip backslash and newline, stay in comment
                                    }
                                }
                                result.push_back(content[cursor++]);
                            }
                        }
                    } else if (content[cursor + 1] == '*') {

                        size_t end = content.find("*/", cursor + 2);
                        if (content[cursor + 2] == '*') {
                            documentation_comments.emplace_back(content.substr(cursor+3, end - cursor-4));
                        }
                        if (end == std::string_view::npos) end = content.size() - 2;
                        cursor = end + 2;
                    }
                    else {
                        result.push_back('/');
                        cursor++;
                    }
                } else {
                    result.push_back('/');
                    cursor++;
                }
            } else if (trigger == '"') {
                if (in_string) {
                    in_string = false;
                } else {
                    in_string = true;
                }
                result.push_back('\"');
                cursor++;
            } else if (trigger == '`'){
                if (cursor + 1 < content.size() && content[cursor + 1] == '"') {
                    result.append("`\"");
                    cursor += 2;
                    continue;
                }

                // This is your original backtick logic that handles `define
                auto directive = content.substr(cursor);
                if (directive.starts_with("`define")) {
                    in_macro = true;
                }
                result.push_back('`');
                cursor++;
            } else if (trigger == '\\') {
                if (cursor + 1 < content.size()) {
                    char next = content[cursor + 1];

                    // skip backslash and newline when necessary
                    if (next == '\n' || next == '\r') {
                        if (in_macro || in_string) {
                            size_t skip_len = (next == '\r' && content[cursor+2] == '\n') ? 3 : 2;

                            if (cursor + skip_len < content.size()) {
                                char next_char = content[cursor + skip_len];

                                // Check if BOTH flanking sides are valid identifier characters
                                bool left_is_word = !result.empty() && (std::isalnum(static_cast<unsigned char>(result.back())) || result.back() == '_');
                                bool right_is_word = std::isalnum(static_cast<unsigned char>(next_char)) || next_char == '_';

                                // Check that the character before the backslash isn't already whitespace
                                bool already_has_space = !result.empty() && (result.back() == ' ' || result.back() == '\t');

                                if (left_is_word && right_is_word && !already_has_space) {
                                    result.push_back(' ');
                                }
                            }

                            cursor += skip_len;
                            continue;
                        }
                    }

                    //keep escaped quotes as is
                    if (in_string && next == '"') {
                        result.push_back('\\');
                        result.push_back('\"');
                        cursor += 2;
                        continue;
                    }
                }
                // pass through normal backslashes
                result.push_back('\\');
                cursor++;
            }
        }

        return result;
    }

    // Single-pass line-leading directive scan over a string_view, no streams.
    // Returns true if any line (after ltrim) starts with `define/`undef/`include.
    static bool has_line_leading_directive(std::string_view text) {
        size_t pos = 0;
        const size_t n = text.size();
        while (pos < n) {
            size_t eol = text.find('\n', pos);
            std::string_view line = (eol == std::string_view::npos)
                ? text.substr(pos) : text.substr(pos, eol - pos);
            // ltrim spaces/tabs only (matches macro_processor::ltrim).
            size_t s = line.find_first_not_of(" \t");
            if (s != std::string_view::npos) {
                std::string_view t = line.substr(s);
                if (t.starts_with("`define") || t.starts_with("`undef") ||
                    t.starts_with("`include") || is_directive(t, "`timescale") ||
                    is_directive(t, "`line")) {
                    return true;
                }
            }
            if (eol == std::string_view::npos) break;
            pos = eol + 1;
        }
        return false;
    }

    std::string sv_preprocessor::evaluate_embedded_directives(const std::string &text) {
        if (text.find('`') == std::string::npos) return text;
        // Fast path: nothing line-leading to evaluate (single manual scan,
        // no istringstream allocation per expanded line).
        if (!has_line_leading_directive(text)) return text;

        std::string result;
        result.reserve(text.size());
        bool first_out = true;
        auto emit_line = [&](std::string_view l) {
            if (!first_out) result.push_back('\n');
            first_out = false;
            result.append(l);
        };
        size_t pos = 0;
        const size_t n = text.size();
        auto next_line_view = [&](std::string_view &out) -> bool {
            if (pos > n) return false;
            if (pos == n) return false;
            size_t eol = text.find('\n', pos);
            if (eol == std::string::npos) {
                out = std::string_view(text.data() + pos, n - pos);
                pos = n + 1;
            } else {
                out = std::string_view(text.data() + pos, eol - pos);
                pos = eol + 1;
            }
            return true;
        };
        std::string_view line_view;
        // NOTE: lines needing parse_definition/parse_include_path are
        // materialized to std::string only on that path; pass-through lines
        // are appended as views (no per-line std::string + vector entry).
        while (next_line_view(line_view)) {
            // Deliberately no error break: map operations below are pure, so
            // every harvestable definition is collected even when the same
            // line also carries an unknown (the file quarantines regardless).
            auto trimmed = macro_processor::ltrim(line_view);
            if (is_directive(trimmed, "`timescale") && c_solver.is_active()) {
                check_timescale(trimmed, nullptr);
                emit_line("");
            } else if (is_directive(trimmed, "`line") && c_solver.is_active()) {
                check_line(trimmed, nullptr);
                emit_line("");
            } else if (trimmed.starts_with("`define") && c_solver.is_active()) {
                parse_definition(trimmed, 7);
                emit_line("");
            } else if (trimmed.starts_with("`undef") && c_solver.is_active()) {
                definitions.erase(std::string(parse_one_arg_directive(trimmed, 6)));
                emit_line("");
            } else if (trimmed.starts_with("`include") && c_solver.is_active()) {
                auto included_file = parse_include_path(trimmed);
                if (!included_file.has_value()) {
                    // Quoted-missing already warned, angle-missing errored.
                    continue;
                }
                includes.insert(included_file.value());
                auto saved_path = path;
                auto saved_line = line_number;
                auto saved_remap = remap;
                path = included_file.value().path;
                active_includes.insert(path);
                source_map.close_range(output_line_n);
                auto f_opt = mm_file::try_open(path);
                if (!f_opt.has_value()) {
                    report_error(fmt::format("Could not open include file: {}", path));
                } else if (include_nesting_depth >= MAX_INCLUDE_DEPTH) {
                    report_error(fmt::format("Maximum include depth ({}) exceeded including file {} in file {}",
                                             MAX_INCLUDE_DEPTH, path, saved_path));
                } else {
                    include_depth_guard depth_guard;
                    // Shared instance: included defines join this file's scope.
                    auto content = preprocess_body(f_opt->view(), output_line_n);
                    if (!error) {
                        // Same getline-drop-trailing-newline semantics as
                        // before, without an istringstream per include.
                        size_t cpos = 0;
                        const size_t cn = content.size();
                        while (cpos < cn) {
                            size_t ceol = content.find('\n', cpos);
                            if (ceol == std::string::npos) {
                                emit_line(std::string_view(content.data() + cpos, cn - cpos));
                                break;
                            }
                            emit_line(std::string_view(content.data() + cpos, ceol - cpos));
                            cpos = ceol + 1;
                        }
                    }
                    release_include_file(path);
                }
                active_includes.erase(path);
                line_number = saved_line;
                remap = saved_remap;
                path = saved_path;
                source_map.open_range(output_line_n, path);
            } else {
                emit_line(line_view);
            }
        }
        if (!text.empty() && text.back() == '\n') result.push_back('\n');
        return result;
    }

    // Single-scan check for embedded conditionals. The old code ran five
    // separate contains() scans per expanded line; most expanded lines
    // contain a backtick but no conditional, so one pass over backticks wins.
    static bool has_embedded_conditional(std::string_view text) {
        size_t pos = text.find('`');
        while (pos != std::string_view::npos) {
            std::string_view sub = text.substr(pos);
            if (sub.starts_with("`ifdef") || sub.starts_with("`ifndef") ||
                sub.starts_with("`elsif") || sub.starts_with("`else") ||
                sub.starts_with("`endif")) {
                return true;
            }
            pos = text.find('`', pos + 1);
        }
        return false;
    }

    std::string sv_preprocessor::post_process_macro_expansion(const std::string &text) {
        // Fast path: plain code needs no conditional re-evaluation. The
        // evaluate_embedded_directives() call below early-returns on
        // backtick-less text anyway, so skip it and only pay for the
        // block-comment strip when "/*" is actually present.
        if (!has_embedded_conditional(text)) {
            // NOTE: embedded `define/`undef/`include still need evaluation
            // here (they arrive without any conditional); only skip the call
            // when there is no backtick at all.
            std::string output_line =
                (text.find('`') == std::string::npos)
                ? text : evaluate_embedded_directives(text);
            if (output_line.find("/*") == std::string::npos) return output_line;
            // Fall through to shared block-comment stripping.
            std::string stripped;
            stripped.reserve(output_line.size());
            size_t pos = 0;
            while (pos < output_line.size()) {
                if (pos + 1 < output_line.size() && output_line[pos] == '/' && output_line[pos + 1] == '*') {
                    size_t end_pos = output_line.find("*/", pos + 2);
                    if (end_pos != std::string::npos) {
                        pos = end_pos + 2;
                    } else {
                        pos = output_line.size(); // Malformed/unclosed comment case
                    }
                } else {
                    stripped.push_back(output_line[pos]);
                    pos++;
                }
            }
            return stripped;
        }
        std::string output_line = text;
        {
            // 3. Format the line to isolate embedded directives onto their own separate lines
            std::string formatted;
            formatted.reserve(output_line.size() + 16);
            size_t i = 0;
            while (i < output_line.size()) {
                if (output_line[i] == '`') {
                    std::string_view sub = std::string_view(output_line).substr(i);
                    std::string_view directive;
                    size_t dir_len = 0;
                    bool has_arg = false;

                    if (sub.starts_with("`ifdef"))       { directive = "`ifdef";  dir_len = 6; has_arg = true; }
                    else if (sub.starts_with("`ifndef")) { directive = "`ifndef"; dir_len = 7; has_arg = true; }
                    else if (sub.starts_with("`elsif"))  { directive = "`elsif";  dir_len = 6; has_arg = true; }
                    else if (sub.starts_with("`else"))   { directive = "`else";   dir_len = 5; has_arg = false; }
                    else if (sub.starts_with("`endif"))  { directive = "`endif";  dir_len = 6; has_arg = false; }

                    if (!directive.empty()) {
                        if (!formatted.empty() && formatted.back() != '\n') {
                            formatted.push_back('\n');
                        }
                        formatted.append(directive);
                        i += dir_len;

                        if (has_arg) {
                            while (i < output_line.size() && (output_line[i] == ' ' || output_line[i] == '\t')) {
                                formatted.push_back(output_line[i]);
                                i++;
                            }
                            while (i < output_line.size() && (std::isalnum(static_cast<unsigned char>(output_line[i])) || output_line[i] == '_' || output_line[i] == '$')) {
                                formatted.push_back(output_line[i]);
                                i++;
                            }
                        }
                        formatted.push_back('\n');
                        continue;
                    }
                }
                formatted.push_back(output_line[i]);
                i++;
            }

            // 4. Instantiate a nested preprocessor step with pre-populated definitions
            // Depth backstop: every guard so far assumes well-formed input, but
            // a missed cycle route must degrade to a clean diagnostic, never a
            // stack-smashing unbounded recursion. 32 nested conditional layers
            // via macro expansion exceeds anything real code produces.
            struct pp_depth_guard { ~pp_depth_guard() { --pp_depth; } } pp_depth_guard_instance;
            (void)pp_depth_guard_instance;
            static constexpr int MAX_NESTED_PREPROCESS_DEPTH = 32;
            ++pp_depth;
            if (pp_depth > MAX_NESTED_PREPROCESS_DEPTH) {
                report_error(fmt::format("Nested conditional expansion exceeded maximum depth ({}) in file {}",
                                         MAX_NESTED_PREPROCESS_DEPTH, path));
                spdlog::error("Nesting trigger in file {} (depth {}): {:.300}",
                              path, pp_depth, formatted);
                return "";
            }
            sv_preprocessor nested_preproc;
            nested_preproc.definitions = definitions;
            nested_preproc.opts_ = opts_;
            nested_preproc.repo_idx = repo_idx;
            nested_preproc.path = path;
            nested_preproc.top_path = top_path;
            // Share the ancestor include chain: a cycle routed through
            // macro-generated includes would otherwise see a fresh,
            // empty guard set at every nesting level and recurse forever.
            nested_preproc.active_includes = active_includes;
            nested_preproc.active_inodes = active_inodes;

            // Evaluate the conditional branches cleanly using the existing rules.
            // Body directly: definitions were pre-populated above and must
            // not be re-seeded (the seeding entry would wipe them).
            output_line = nested_preproc.preprocess_body(formatted, output_line_n) + '\n';
            // Propagate order-dependent unknowns: a macro hidden inside an
            // embedded conditional is just as quarantine-worthy.
            undefined_macros.insert(nested_preproc.undefined_macros.begin(),
                                    nested_preproc.undefined_macros.end());
            unknown_conditionals.insert(nested_preproc.unknown_conditionals.begin(),
                                        nested_preproc.unknown_conditionals.end());
            if (nested_preproc.has_error()) {
                // No per-level logging here: the originating level already
                // logged via report_error; echoing at every unwind level
                // turns one trip into dozens of identical lines.
                if (!error) error = nested_preproc.get_error();
                if (nested_preproc.has_fatal_error()) fatal_error = true;
                return "";
            }

            definitions = nested_preproc.definitions;
            locally_defined.insert(nested_preproc.locally_defined.begin(),
                                   nested_preproc.locally_defined.end());
            includes.insert(nested_preproc.includes.begin(), nested_preproc.includes.end());
        }
        output_line = evaluate_embedded_directives(output_line);
        if (output_line.contains("/*")) {
            std::string stripped;
            stripped.reserve(output_line.size());
            size_t pos = 0;
            while (pos < output_line.size()) {
                if (pos + 1 < output_line.size() && output_line[pos] == '/' && output_line[pos + 1] == '*') {
                    size_t end_pos = output_line.find("*/", pos + 2);
                    if (end_pos != std::string::npos) {
                        pos = end_pos + 2;
                    } else {
                        pos = output_line.size(); // Malformed/unclosed comment case
                    }
                } else {
                    stripped.push_back(output_line[pos]);
                    pos++;
                }
            }
            output_line = stripped;
        }
        return output_line;
    }
    // Paren delta of one chunk, tracking string state across chunks. The old
    // code rescanned the whole accumulated string per extra line (O(n^2) on
    // long UVM macro calls); this keeps a running balance instead.
    // NOTE: in_string intentionally persists across lines: an unterminated
    // quote keeps parens quoted until the closing quote, matching the old
    // per-rescan behavior where a quote opened on an earlier line toggles the
    // same single in_string flag for the rest of the accumulated text.
    static int paren_balance_delta(std::string_view chunk, bool &in_string) {
        int balance = 0;
        for (size_t i = 0; i < chunk.size(); ++i) {
            if (chunk[i] == '"' && (i == 0 || chunk[i-1] != '\\')) {
                in_string = !in_string;
            }
            if (!in_string) {
                if (chunk[i] == '(') balance++;
                if (chunk[i] == ')') balance--;
            }
        }
        return balance;
    }

    std::string sv_preprocessor::gather_multi_line_macro(const std::string &first_line, std::istringstream &iss) {
        std::string accumulated = first_line;

        bool in_string = false;
        int balance = paren_balance_delta(first_line, in_string);

        std::string next_line;
        // A lookahead line starting with a structural directive terminates
        // gathering (left for the main loop): swallowing it would re-assemble
        // text that post_process splits apart again, recreating the nesting
        // input at every level (unbounded self-similar recursion on fragments
        // like `if (` + kept macro with `ifndef on following lines).
        auto is_directive_line = [](std::string_view ln) {
            auto t = macro_processor::ltrim(ln);
            if (t.size() < 2 || t[0] != '`') return false;
            size_t i = 1;
            while (i < t.size() && (std::isalnum(static_cast<unsigned char>(t[i])) ||
                                    t[i] == '_' || t[i] == '$')) ++i;
            return is_structural_directive(t.substr(1, i - 1));
        };
        // Continue pulling raw lines from the stream as long as parentheses are unbalanced.
        // balance is tracked incrementally (not rescanned from scratch per line).
        while (balance > 0) {
            auto pos = iss.tellg();
            if (!std::getline(iss, next_line)) break;

            // Strip single-line comments from the lookahead segment before combining
            std::string_view next_view = next_line;
            if (auto comment_pos = find_line_comment(next_line); comment_pos != std::string::npos) {
                next_view = std::string_view(next_line.data(), comment_pos);
            }
            if (is_directive_line(next_view)) {
                iss.seekg(pos);
                break;
            }

            line_number++; // Crucial: Keeps the preprocessor line tracking 1:1 with the file

            balance += paren_balance_delta(next_view, in_string);
            accumulated.push_back('\n');
            accumulated.append(next_view);
        }

        return accumulated;
    }

}
