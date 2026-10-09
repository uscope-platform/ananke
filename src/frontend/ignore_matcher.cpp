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

#include "frontend/ignore_matcher.hpp"

#include <fnmatch.h>
#include <fstream>

size_t ignore_matcher::add_file(const std::filesystem::path &marker_file) {
    std::ifstream content(marker_file);
    if (!content.good()) return 0;
    const size_t before = rules_.size();
    const auto base = marker_file.parent_path();
    std::string line;
    while (std::getline(content, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        line = trim_right(line);
        if (line.empty() || line.front() == '#') continue;
        add_pattern(line, base);
    }
    return rules_.size() - before;
}

void ignore_matcher::add_pattern(const std::string &raw_pattern, const std::filesystem::path &base) {
    std::string pattern = trim_right(raw_pattern);
    if (pattern.empty() || pattern.front() == '#') return;
    rule r;
    r.base = base.lexically_normal();
    if (pattern.front() == '!') {
        r.negated = true;
        pattern = trim_right(pattern.substr(1));
        if (pattern.empty()) return;
    }
    if (pattern.back() == '/') {
        r.dir_only = true;
        pattern.pop_back();
        pattern = trim_right(pattern);
        if (pattern.empty()) return;
    }
    if (pattern.front() == '/') {
        r.anchored = true;
        pattern = pattern.substr(1);
        if (pattern.empty()) return;
    } else if (pattern.find('/') != std::string::npos) {
        r.anchored = true;
    }
    r.pattern = pattern;
    rules_.push_back(std::move(r));
}

bool ignore_matcher::matches(const std::filesystem::path &path, bool is_dir) const {
    bool ignored = false;
    for (const auto &r : rules_) {
        if (match_rule(r, path, is_dir)) ignored = !r.negated;
    }
    return ignored;
}

bool ignore_matcher::match_rule(const rule &r, const std::filesystem::path &path, bool is_dir) {
    if (r.dir_only && !is_dir) return false;
    const auto norm = path.lexically_normal();
    if (!r.anchored) {
        return fnmatch(r.pattern.c_str(), norm.filename().string().c_str(), 0) == 0;
    }
    std::error_code ec;
    const auto rel = std::filesystem::relative(norm, r.base, ec);
    if (ec) return false;
    const auto rel_str = rel.generic_string();
    if (rel_str.empty() || rel_str == "." || rel_str.starts_with("..")) return false;
    return match_segments(split_segments(r.pattern), 0, split_segments(rel_str), 0);
}

bool ignore_matcher::match_segments(const std::vector<std::string> &pat, size_t pi,
                                    const std::vector<std::string> &segs, size_t si) {
    if (pi == pat.size()) return si == segs.size();
    if (pat[pi] == "**") {
        for (size_t k = si; k <= segs.size(); ++k) {
            if (match_segments(pat, pi + 1, segs, k)) return true;
        }
        return false;
    }
    if (si == segs.size()) return false;
    if (fnmatch(pat[pi].c_str(), segs[si].c_str(), 0) != 0) return false;
    return match_segments(pat, pi + 1, segs, si + 1);
}

std::vector<std::string> ignore_matcher::split_segments(const std::string &s) {
    std::vector<std::string> segs;
    std::string cur;
    for (char c : s) {
        if (c == '/') {
            segs.push_back(cur);
            cur.clear();
        } else {
            cur.push_back(c);
        }
    }
    segs.push_back(cur);
    return segs;
}

std::string ignore_matcher::trim_right(const std::string &s) {
    size_t end = s.size();
    while (end > 0 && (s[end - 1] == ' ' || s[end - 1] == '\t')) --end;
    return s.substr(0, end);
}
