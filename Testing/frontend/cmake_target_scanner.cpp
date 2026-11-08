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

#include <gtest/gtest.h>

#include "frontend/cmake_target_scanner.hpp"

namespace {

std::vector<std::string> names(const std::vector<cmake_target> &targets) {
    std::vector<std::string> out;
    for (const auto &t : targets) out.push_back(t.name);
    return out;
}

} // namespace

TEST(cmake_target_scanner, finds_plain_library_declarations) {
    const auto t = cmake_target_scanner::parse(
        "add_library(sctb STATIC src/a.cpp src/b.cpp)\n"
        "add_library(other SHARED x.cpp)\n"
        "add_library(helper INTERFACE)\n", "/r/lib");
    EXPECT_EQ(names(t), (std::vector<std::string>{"sctb", "other", "helper"}));
    EXPECT_EQ(t[0].dir, std::filesystem::path("/r/lib"));
}

TEST(cmake_target_scanner, tolerates_case_whitespace_and_multiple_lines) {
    const auto t = cmake_target_scanner::parse(
        "ADD_LIBRARY (\n    sctb\n    STATIC\n    src/a.cpp\n)\n"
        "Add_Library(second OBJECT b.cpp)", "/r");
    EXPECT_EQ(names(t), (std::vector<std::string>{"sctb", "second"}));
}

TEST(cmake_target_scanner, ignores_imported_and_alias_targets) {
    const auto t = cmake_target_scanner::parse(
        "add_library(ext SHARED IMPORTED)\n"
        "add_library(ext2 IMPORTED GLOBAL)\n"
        "add_library(ns::real ALIAS real)\n"
        "add_library(real STATIC r.cpp)\n", "/r");
    EXPECT_EQ(names(t), (std::vector<std::string>{"real"}));
}

TEST(cmake_target_scanner, ignores_names_that_need_evaluation) {
    const auto t = cmake_target_scanner::parse(
        "add_library(${PROJECT_NAME} STATIC a.cpp)\n"
        "add_library(lib_${SUFFIX} STATIC a.cpp)\n"
        "add_library($<TARGET_NAME:x> STATIC a.cpp)\n"
        "add_library(plain STATIC a.cpp)\n", "/r");
    EXPECT_EQ(names(t), (std::vector<std::string>{"plain"}));
}

TEST(cmake_target_scanner, ignores_comments_and_unrelated_commands) {
    const auto t = cmake_target_scanner::parse(
        "# add_library(commented STATIC a.cpp)\n"
        "#[[ add_library(blockcomment STATIC a.cpp)\n"
        "    add_library(blockcomment2 STATIC a.cpp) ]]\n"
        "message(\"add_library(instring STATIC a.cpp) # not a comment\")\n"
        "my_add_library(notacall STATIC a.cpp)\n"
        "add_library_extra(notacall2 STATIC a.cpp)\n"
        "add_library(real STATIC a.cpp) # trailing add_library(trailing STATIC a.cpp)\n", "/r");
    EXPECT_EQ(names(t), (std::vector<std::string>{"real"}));
}

TEST(cmake_target_scanner, quoted_names_and_nested_parentheses) {
    const auto t = cmake_target_scanner::parse(
        "add_library(\"quoted\" STATIC $<$<BOOL:ON>:a.cpp> \"b (1).cpp\")\n"
        "add_library(after STATIC c.cpp)\n", "/r");
    EXPECT_EQ(names(t), (std::vector<std::string>{"quoted", "after"}));
}

TEST(cmake_target_scanner, repeated_declaration_in_one_file_is_reported_once) {
    const auto t = cmake_target_scanner::parse(
        "if(A)\n add_library(dup STATIC a.cpp)\nelse()\n add_library(dup STATIC b.cpp)\nendif()\n", "/r");
    EXPECT_EQ(names(t), (std::vector<std::string>{"dup"}));
}

TEST(cmake_target_scanner, unterminated_call_does_not_crash) {
    EXPECT_NO_THROW(cmake_target_scanner::parse("add_library(broken STATIC a.cpp", "/r"));
    EXPECT_TRUE(cmake_target_scanner::parse("add_library(broken STATIC a.cpp", "/r").empty());
    EXPECT_TRUE(cmake_target_scanner::parse("add_library", "/r").empty());
    EXPECT_TRUE(cmake_target_scanner::parse("", "/r").empty());
}

TEST(cmake_target_scanner, resolve_finds_a_unique_target) {
    const std::vector<cmake_target> targets = {{"a", "/r/a"}, {"b", "/r/b"}};
    auto r = cmake_target_scanner::resolve(targets, "b");
    ASSERT_TRUE(r.has_value());
    EXPECT_EQ(r->dir, std::filesystem::path("/r/b"));
}

TEST(cmake_target_scanner, resolve_reports_unknown_targets_with_the_known_ones) {
    const std::vector<cmake_target> targets = {{"a", "/r/a"}, {"b", "/r/b"}};
    auto r = cmake_target_scanner::resolve(targets, "missing");
    ASSERT_FALSE(r.has_value());
    EXPECT_NE(r.error().find("'missing' not found"), std::string::npos);
    EXPECT_NE(r.error().find("a b"), std::string::npos);

    auto none = cmake_target_scanner::resolve({}, "x");
    ASSERT_FALSE(none.has_value());
    EXPECT_NE(none.error().find("no add_library()"), std::string::npos);
}

TEST(cmake_target_scanner, resolve_reports_ambiguous_targets_with_every_location) {
    const std::vector<cmake_target> targets = {{"dup", "/r/one"}, {"dup", "/r/two"}, {"other", "/r/o"}};
    auto r = cmake_target_scanner::resolve(targets, "dup");
    ASSERT_FALSE(r.has_value());
    EXPECT_NE(r.error().find("more than one place"), std::string::npos);
    EXPECT_NE(r.error().find("/r/one"), std::string::npos);
    EXPECT_NE(r.error().find("/r/two"), std::string::npos);
}
