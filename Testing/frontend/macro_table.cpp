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

#include "frontend/macro_table.hpp"

using namespace preprocessor;

static macro_definitions_map simple_def(const std::string &name, const std::string &value) {
    return {{name, value}};
}

TEST(macro_table, absent_name_is_unresolvable) {
    macro_table table;
    table.add_file_definitions("a.sv", simple_def("FOO", "1"));
    auto injection = table.build_injection({"BAR"});
    EXPECT_TRUE(injection.definitions.empty());
    ASSERT_EQ(injection.unresolvable.size(), 1u);
    EXPECT_EQ(injection.unresolvable[0], "BAR");
    EXPECT_TRUE(injection.conflicts.empty());
}

TEST(macro_table, identical_bodies_are_one_definition) {
    macro_table table;
    table.add_file_definitions("a.sv", simple_def("FOO", "1"));
    table.add_file_definitions("b.sv", simple_def("FOO", "1"));
    auto resolution = table.resolve("FOO");
    EXPECT_EQ(resolution.state, macro_table::resolution::status::unique);
    ASSERT_EQ(resolution.definers.size(), 2u);
    auto injection = table.build_injection({"FOO"}, "c.sv");
    ASSERT_EQ(injection.definitions.size(), 1u);
    EXPECT_EQ(std::get<std::string>(injection.definitions.at("FOO")), "1");
}

TEST(macro_table, distinct_bodies_conflict_with_definers) {
    macro_table table;
    table.add_file_definitions("a.sv", simple_def("FOO", "1"));
    table.add_file_definitions("b.sv", simple_def("FOO", "2"));
    auto resolution = table.resolve("FOO");
    EXPECT_EQ(resolution.state, macro_table::resolution::status::conflict);
    auto injection = table.build_injection({"FOO"}, "c.sv");
    EXPECT_TRUE(injection.definitions.empty());
    ASSERT_EQ(injection.conflicts.size(), 1u);
    ASSERT_EQ(injection.conflicts.at("FOO").size(), 2u);
}

TEST(macro_table, reparse_replaces_same_file_entry) {
    macro_table table;
    table.add_file_definitions("a.sv", simple_def("FOO", "1"));
    table.add_file_definitions("a.sv", simple_def("FOO", "2"));
    // Same file redefining across runs is a replacement, not a conflict.
    auto resolution = table.resolve("FOO");
    EXPECT_EQ(resolution.state, macro_table::resolution::status::unique);
    ASSERT_EQ(resolution.definers.size(), 1u);
    EXPECT_EQ(resolution.definers[0], "a.sv");
}

TEST(macro_table, self_defined_names_are_never_injected) {
    macro_table table;
    // Include-guard shape: the file defines the very macro it queries.
    table.add_file_definitions("guard.svh", simple_def("GUARD_SV", ""));
    auto injection = table.build_injection({"GUARD_SV"}, "guard.svh");
    EXPECT_TRUE(injection.definitions.empty());
    EXPECT_TRUE(injection.unresolvable.empty());
    EXPECT_TRUE(injection.conflicts.empty());
}

TEST(macro_table, self_definition_wins_over_other_definers) {
    macro_table table;
    table.add_file_definitions("self.sv", simple_def("FOO", "1"));
    table.add_file_definitions("other.sv", simple_def("FOO", "2"));
    // Injecting other's body would rewrite this file's own definition order.
    auto injection = table.build_injection({"FOO"}, "self.sv");
    EXPECT_TRUE(injection.definitions.empty());
    // ...while unrelated files still see the conflict loud and clear.
    auto other = table.build_injection({"FOO"}, "third.sv");
    EXPECT_TRUE(other.definitions.empty());
    ASSERT_EQ(other.conflicts.size(), 1u);
}

TEST(macro_table, stored_round_trip_simple) {
    macro_definitions_map::mapped_type def = std::string("8");
    auto stored = macro_to_stored("W", def);
    EXPECT_EQ(stored.name, "W");
    EXPECT_FALSE(stored.is_function);
    EXPECT_EQ(stored.value, "8");
    auto live = macro_to_live(stored);
    ASSERT_TRUE(std::holds_alternative<std::string>(live));
    EXPECT_EQ(std::get<std::string>(live), "8");
}

TEST(macro_table, stored_round_trip_function) {
    preprocessor::function_macro macro;
    macro.value = "a+b";
    macro.arguments.push_back({"a", "5", true});
    macro.arguments.push_back({"b", "", false});
    macro_definitions_map::mapped_type def = macro;
    auto stored = macro_to_stored("ADD", def);
    EXPECT_EQ(stored.name, "ADD");
    EXPECT_TRUE(stored.is_function);
    auto live = macro_to_live(stored);
    ASSERT_TRUE(std::holds_alternative<preprocessor::function_macro>(live));
    EXPECT_EQ(std::get<preprocessor::function_macro>(live), macro);
    EXPECT_EQ(macro_canonical_body(def), macro_canonical_body(live));
}
