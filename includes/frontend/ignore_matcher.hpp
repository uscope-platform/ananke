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

#ifndef ANANKE_IGNORE_MATCHER_HPP
#define ANANKE_IGNORE_MATCHER_HPP

#include <filesystem>
#include <string>
#include <vector>

// Pragmatic subset of gitignore matching for `.mkignore` files.
//
// Supported syntax:
//   - `*`, `?`, `[...]` wildcards (dotfiles match like any other file).
//   - `**` spanning any number of path segments (including zero).
//   - Trailing `/` restricts the rule to directories.
//   - Leading `/` anchors the pattern to the marker's directory; a pattern
//     containing `/` elsewhere is likewise anchored. A slash-free pattern
//     floats: it matches the basename at any depth below the marker dir.
//   - `#` comments, blank lines skipped, `!` negation, last match wins.
//
// Documented deviations from git: no backslash escaping, and a file below an
// excluded directory cannot be re-included (the walk prunes such directories
// before their own markers are ever loaded).
class ignore_matcher {
public:
    // Parse a `.mkignore` file; patterns apply to marker_file.parent_path().
    // Returns the number of rules added (0 for empty/missing/unreadable).
    size_t add_file(const std::filesystem::path &marker_file);
    // Add a single raw pattern with an explicit base directory.
    void add_pattern(const std::string &raw_pattern, const std::filesystem::path &base);
    // Last-match-wins verdict for path (negated rules re-include).
    bool matches(const std::filesystem::path &path, bool is_dir) const;
    bool empty() const { return rules_.empty(); }

private:
    struct rule {
        std::string pattern;
        std::filesystem::path base;
        bool negated = false;
        bool dir_only = false;
        bool anchored = false;
    };
    std::vector<rule> rules_;

    static bool match_rule(const rule &r, const std::filesystem::path &path, bool is_dir);
    static bool match_segments(const std::vector<std::string> &pat, size_t pi,
                               const std::vector<std::string> &segs, size_t si);
    static std::vector<std::string> split_segments(const std::string &s);
    static std::string trim_right(const std::string &s);
};

#endif //ANANKE_IGNORE_MATCHER_HPP
