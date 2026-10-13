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

#ifndef ANANKE_INCLUDE_CONFIG_HPP
#define ANANKE_INCLUDE_CONFIG_HPP

#include <algorithm>
#include <filesystem>
#include <string>
#include <vector>

#include "data_model/parse_options.hpp"

// Shared normalization for the `includes` config (Depfile `includes`
// section + settings-profile fallback). All persisted paths are
// repo-relative with generic separators, no leading slash, lexically
// normal; matching is exact/prefix-based (no globs).
namespace include_config {

inline std::string normalize_repo_path(const std::string &p) {
    std::string s = std::filesystem::path(p).generic_string();
    // Lexically normalize without touching the filesystem.
    s = std::filesystem::path(s).lexically_normal().generic_string();
    while (!s.empty() && (s.front() == '/' || s.front() == '\\')) s.erase(s.begin());
    while (s.size() >= 2 && s.substr(0, 2) == "./") s.erase(0, 2);
    return s;
}

inline std::string include_key_of(const std::string &name) {
    // Keep subpaths (repository_index suffix-matches them); normalize only.
    return normalize_repo_path(name);
}

inline std::string basename_of(const std::string &name) {
    return std::filesystem::path(name).filename().generic_string();
}

// Repo-relative dir of an absolute includer path. Returns "" when the file
// is outside repo_root (never matches a scope).
inline std::string includer_scope_of(const std::string &abs_path, const std::string &repo_root) {
    namespace fs = std::filesystem;
    std::error_code ec;
    fs::path abs_norm = fs::path(abs_path).lexically_normal();
    std::string abs_s = abs_norm.generic_string();
    if (repo_root.empty()) return "";
    fs::path root_norm = fs::path(repo_root).lexically_normal();
    std::string root_s = root_norm.generic_string();
    if (abs_s.size() < root_s.size()) return "";
    if (abs_s.compare(0, root_s.size(), root_s) != 0) return "";
    std::string rel;
    if (abs_s.size() == root_s.size()) rel = "";
    else {
        rel = abs_s.substr(root_s.size());
        while (!rel.empty() && (rel.front() == '/' || rel.front() == '\\')) rel.erase(rel.begin());
    }
    if (rel.empty()) return "";
    fs::path rel_p(rel);
    if (rel_p.has_filename() && rel.find('/') != std::string::npos) rel = rel_p.parent_path().generic_string();
    else if (rel.find('/') == std::string::npos && rel.find('\\') == std::string::npos) rel = "";
    else rel = rel_p.parent_path().generic_string();
    return normalize_repo_path(rel);
}

inline bool scope_covers(const std::string &scope, const std::string &includer_dir) {
    if (scope.empty()) return false;
    if (includer_dir == scope) return true;
    return includer_dir.size() > scope.size() &&
           includer_dir.compare(0, scope.size(), scope) == 0 &&
           includer_dir[scope.size()] == '/';
}

// Suffix tolerance shared with data_store::paths_match: stored absolute vs
// recorded repo-relative.
inline bool target_matches(const std::string &candidate, const std::string &target) {
    if (candidate.empty() || target.empty()) return false;
    if (candidate == target) return true;
    auto norm = [](const std::string &s) {
        return std::filesystem::path(s).lexically_normal().generic_string();
    };
    std::string c = norm(candidate), t = norm(target);
    if (c == t) return true;
    if (c.size() >= t.size() && c.compare(c.size() - t.size(), t.size(), t) == 0) {
        if (c.size() == t.size() || c[c.size() - t.size() - 1] == '/') return true;
    }
    if (t.size() >= c.size() && t.compare(t.size() - c.size(), c.size(), c) == 0) {
        if (t.size() == c.size() || t[t.size() - c.size() - 1] == '/') return true;
    }
    return false;
}

// Deterministic fingerprint for cache invalidation (config change forces a
// re-parse; include outcomes and global macro meanings are only
// re-evaluated on parse). Covers the `includes` config and the effective
// global defines (profile list + Depfile overlay alike).
inline std::string fingerprint(const parse_options &opts) {
    std::vector<std::string> parts;
    for (const auto &s : opts.ignored_includes) parts.push_back("i:" + normalize_repo_path(s));
    std::vector<std::string> keys;
    keys.reserve(opts.include_deconfliction.size());
    for (const auto &kv : opts.include_deconfliction) keys.push_back(kv.first);
    std::sort(keys.begin(), keys.end());
    for (const auto &k : keys) {
        const auto &r = opts.include_deconfliction.at(k);
        std::string nk = normalize_repo_path(k);
        if (r.global_target.has_value()) parts.push_back("g:" + nk + "=" + normalize_repo_path(*r.global_target));
        std::vector<std::string> scopes;
        scopes.reserve(r.scoped_targets.size());
        for (const auto &sv : r.scoped_targets) scopes.push_back(sv.first);
        std::sort(scopes.begin(), scopes.end());
        for (const auto &s : scopes) {
            parts.push_back("s:" + nk + "@" + normalize_repo_path(s) + "=" +
                            normalize_repo_path(r.scoped_targets.at(s)));
        }
    }
    std::sort(parts.begin(), parts.end());
    std::string out;
    for (const auto &p : parts) { out += p; out.push_back('\n'); }
    for (const auto &d : opts.defines) { out += "d:"; out += d; out.push_back('\n'); }
    return out;
}

} // namespace include_config

#endif //ANANKE_INCLUDE_CONFIG_HPP
