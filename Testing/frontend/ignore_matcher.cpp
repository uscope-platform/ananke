// Copyright 2026 Filippo Savi
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

#include <gtest/gtest.h>

#include <fstream>

#include "frontend/ignore_matcher.hpp"

namespace {
const std::filesystem::path kBase = "/r";

ignore_matcher with_rules(std::initializer_list<std::string> patterns) {
    ignore_matcher m;
    for (const auto &p : patterns) m.add_pattern(p, kBase);
    return m;
}
} // namespace

TEST(ignore_matcher, floating_star_matches_basename_at_any_depth) {
    auto m = with_rules({"*.sv"});
    EXPECT_TRUE(m.matches("/r/a/b/c.sv", false));
    EXPECT_TRUE(m.matches("/r/c.sv", false));
    EXPECT_FALSE(m.matches("/r/a/b/c.v", false));
    EXPECT_FALSE(m.matches("/r/a/b", true));
    // A bare `*` matches directory basenames too.
    auto any = with_rules({"*"});
    EXPECT_TRUE(any.matches("/r/a/b", true));
}

TEST(ignore_matcher, anchored_by_slash_matches_single_level) {
    auto m = with_rules({"snap/*.sv"});
    EXPECT_TRUE(m.matches("/r/snap/f.sv", false));
    EXPECT_FALSE(m.matches("/r/other/f.sv", false));
    EXPECT_FALSE(m.matches("/r/snap/sub/f.sv", false));
}

TEST(ignore_matcher, double_star_spans_segments) {
    auto m = with_rules({"snap/**/*.sv"});
    EXPECT_TRUE(m.matches("/r/snap/f.sv", false));
    EXPECT_TRUE(m.matches("/r/snap/a/b/f.sv", false));
    EXPECT_FALSE(m.matches("/r/snap/a/b/f.v", false));
}

TEST(ignore_matcher, leading_slash_anchors_to_base) {
    auto m = with_rules({"/top.sv"});
    EXPECT_TRUE(m.matches("/r/top.sv", false));
    EXPECT_FALSE(m.matches("/r/sub/top.sv", false));
}

TEST(ignore_matcher, trailing_slash_is_dir_only) {
    auto m = with_rules({"build/"});
    EXPECT_TRUE(m.matches("/r/build", true));
    EXPECT_FALSE(m.matches("/r/build", false));
    EXPECT_FALSE(m.matches("/r/other", true));
}

TEST(ignore_matcher, negation_and_last_match_wins) {
    auto excluded = with_rules({"*.sv", "!keep.sv"});
    EXPECT_TRUE(excluded.matches("/r/a.sv", false));
    EXPECT_FALSE(excluded.matches("/r/keep.sv", false));

    auto re_excluded = with_rules({"!keep.sv", "*.sv"});
    EXPECT_TRUE(re_excluded.matches("/r/keep.sv", false));
}

TEST(ignore_matcher, question_and_class_wildcards) {
    auto m = with_rules({"file?.sv", "bus[12].sv"});
    EXPECT_TRUE(m.matches("/r/file1.sv", false));
    EXPECT_FALSE(m.matches("/r/file12.sv", false));
    EXPECT_TRUE(m.matches("/r/bus1.sv", false));
    EXPECT_FALSE(m.matches("/r/bus3.sv", false));
}

TEST(ignore_matcher, star_matches_dotfiles) {
    auto m = with_rules({"*"});
    EXPECT_TRUE(m.matches("/r/.hidden", false));
}

TEST(ignore_matcher, pattern_cannot_escape_base) {
    ignore_matcher m;
    m.add_pattern("../x.sv", "/r/sub");
    EXPECT_FALSE(m.matches("/r/x.sv", false));
}

TEST(ignore_matcher, add_file_parses_comments_and_blanks) {
    const std::string dir = "/tmp/ananke_ignore_marker";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);
    {
        std::ofstream ofs(dir + "/.mkignore");
        ofs << "# a comment\n\n   \n*.sv\n!keep.sv\n";
    }
    ignore_matcher m;
    EXPECT_EQ(m.add_file(dir + "/.mkignore"), 2u);
    EXPECT_TRUE(m.matches(dir + "/a.sv", false));
    EXPECT_FALSE(m.matches(dir + "/keep.sv", false));
    std::filesystem::remove_all(dir);
}

TEST(ignore_matcher, empty_marker_adds_no_rules) {
    const std::string dir = "/tmp/ananke_ignore_empty";
    std::filesystem::remove_all(dir);
    std::filesystem::create_directories(dir);
    {
        std::ofstream ofs(dir + "/.mkignore");
        ofs << "# only a comment\n";
    }
    ignore_matcher m;
    EXPECT_EQ(m.add_file(dir + "/.mkignore"), 0u);
    EXPECT_TRUE(m.empty());
    std::filesystem::remove_all(dir);
}
