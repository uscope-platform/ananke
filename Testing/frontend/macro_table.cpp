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
#include <gmock/gmock.h>

#include "frontend/macro_table.hpp"

using namespace preprocessor;
using kind = macro_table::verdict::kind;

static macro_definitions_map simple_def(const std::string &name, const std::string &value) {
    return {{name, value}};
}

static macro_definitions_map function_def(const std::string &name,
                                          const std::vector<function_macro_argument> &args,
                                          const std::string &body) {
    function_macro macro;
    macro.value = body;
    macro.arguments = args;
    return {{name, macro}};
}

static undefined_uses_map needs(std::initializer_list<std::pair<std::string, std::set<int>>> entries) {
    undefined_uses_map result;
    for (const auto &entry : entries) result[entry.first] = entry.second;
    return result;
}

TEST(macro_table, absent_name_is_unresolvable) {
    macro_table table;
    table.add_file_definitions("a.sv", simple_def("FOO", "1"));
    auto injection = table.build_injection(needs({{"BAR", {2}}}));
    EXPECT_TRUE(injection.definitions.empty());
    EXPECT_THAT(injection.unresolvable, testing::ElementsAre("BAR"));
    EXPECT_TRUE(injection.conflicts.empty());
    EXPECT_TRUE(injection.mismatches.empty());
}

TEST(macro_table, identical_bodies_are_one_definition) {
    macro_table table;
    table.add_file_definitions("a.sv", simple_def("FOO", "1"));
    table.add_file_definitions("b.sv", simple_def("FOO", "1"));
    auto judgement = table.judge("FOO", {}, "c.sv");
    EXPECT_EQ(judgement.state, kind::unique);
    auto injection = table.build_injection(needs({{"FOO", {}}}), "c.sv");
    ASSERT_EQ(injection.definitions.size(), 1u);
    EXPECT_EQ(std::get<std::string>(injection.definitions.at("FOO")), "1");
}

TEST(macro_table, distinct_bodies_conflict_with_definers) {
    macro_table table;
    table.add_file_definitions("a.sv", simple_def("FOO", "1"));
    table.add_file_definitions("b.sv", simple_def("FOO", "2"));
    auto judgement = table.judge("FOO", {}, "c.sv");
    EXPECT_EQ(judgement.state, kind::conflict);
    auto injection = table.build_injection(needs({{"FOO", {}}}), "c.sv");
    EXPECT_TRUE(injection.definitions.empty());
    ASSERT_EQ(injection.conflicts.size(), 1u);
    EXPECT_THAT(injection.conflicts.at("FOO"), testing::ElementsAre("a.sv", "b.sv"));
}

TEST(macro_table, reparse_replaces_same_file_entry) {
    macro_table table;
    table.add_file_definitions("a.sv", simple_def("FOO", "1"));
    table.add_file_definitions("a.sv", simple_def("FOO", "2"));
    // Same file redefining across runs is a replacement, not a conflict.
    auto judgement = table.judge("FOO", {}, "c.sv");
    EXPECT_EQ(judgement.state, kind::unique);
    ASSERT_EQ(judgement.candidates.size(), 1u);
    EXPECT_EQ(judgement.candidates[0].path, "a.sv");
}

TEST(macro_table, self_defined_names_are_never_injected) {
    macro_table table;
    // Include-guard shape: the file defines the very macro it queries.
    table.add_file_definitions("guard.svh", simple_def("GUARD_SV", ""));
    auto injection = table.build_injection(needs({{"GUARD_SV", {}}}), "guard.svh");
    EXPECT_TRUE(injection.definitions.empty());
    EXPECT_TRUE(injection.unresolvable.empty());
    EXPECT_TRUE(injection.conflicts.empty());
    EXPECT_TRUE(injection.mismatches.empty());
}

TEST(macro_table, self_definition_wins_over_other_definers) {
    macro_table table;
    table.add_file_definitions("self.sv", simple_def("FOO", "1"));
    table.add_file_definitions("other.sv", simple_def("FOO", "2"));
    // Injecting other's body would rewrite this file's own definition order.
    auto injection = table.build_injection(needs({{"FOO", {}}}), "self.sv");
    EXPECT_TRUE(injection.definitions.empty());
    // ...while unrelated files still see the conflict loud and clear.
    auto other = table.build_injection(needs({{"FOO", {}}}), "third.sv");
    EXPECT_TRUE(other.definitions.empty());
    ASSERT_EQ(other.conflicts.size(), 1u);
}

TEST(macro_table, arity_resolves_only_unambiguous_calls) {
    macro_table table;
    // The CVA6 FF shape: 3 params vs 3 params + 2 defaults.
    table.add_file_definitions("narrow.svh",
                               function_def("FF", {{"q", "", false}, {"d", "", false}, {"r", "", false}},
                                            "__q <= __d"));
    table.add_file_definitions("wide.svh",
                               function_def("FF",
                                            {{"q", "", false},
                                             {"d", "", false},
                                             {"r", "", false},
                                             {"c", "clk", true},
                                             {"a", "rst", true}},
                                            "__q <= __d"));
    // 3-arg call: satisfiable by both definitions, so genuinely ambiguous.
    // Choosing would silently pick behavior: stay loud instead.
    auto three = table.build_injection(needs({{"FF", {3}}}), "user.sv");
    EXPECT_TRUE(three.definitions.empty());
    ASSERT_EQ(three.conflicts.size(), 1u);
    // Full 5-arg call: only the wide definition accepts it. Unambiguous.
    auto five = table.build_injection(needs({{"FF", {5}}}), "user.sv");
    ASSERT_EQ(five.definitions.size(), 1u);
    EXPECT_EQ(std::get<preprocessor::function_macro>(five.definitions.at("FF")).arguments.size(), 5u);
    EXPECT_TRUE(five.conflicts.empty());
}

TEST(macro_table, arity_mismatch_reports_definers) {
    macro_table table;
    table.add_file_definitions("narrow.svh",
                               function_def("FF", {{"q", "", false}, {"d", "", false}, {"r", "", false}},
                                            "__q <= __d"));
    // 6-arg call: no definition accepts it.
    auto judgement = table.judge("FF", {6}, "user.sv");
    EXPECT_EQ(judgement.state, kind::mismatch);
    ASSERT_EQ(judgement.candidates.size(), 1u);
    EXPECT_EQ(judgement.candidates[0].path, "narrow.svh");
    auto injection = table.build_injection(needs({{"FF", {6}}}), "user.sv");
    EXPECT_TRUE(injection.definitions.empty());
    ASSERT_EQ(injection.mismatches.size(), 1u);
}

TEST(macro_table, bare_use_cannot_disambiguate_by_arity) {
    macro_table table;
    table.add_file_definitions("narrow.svh",
                               function_def("FF", {{"q", "", false}, {"d", "", false}, {"r", "", false}},
                                            "__q <= __d"));
    table.add_file_definitions("wide.svh",
                               function_def("FF",
                                            {{"q", "", false},
                                             {"d", "", false},
                                             {"r", "", false},
                                             {"c", "clk", true},
                                             {"a", "rst", true}},
                                            "__q <= __d"));
    // No call shape recorded: both stay contenders, honest conflict.
    auto judgement = table.judge("FF", {}, "user.sv");
    EXPECT_EQ(judgement.state, kind::conflict);
}

TEST(macro_table, simple_definition_rejects_call_shaped_use) {
    macro_table table;
    table.add_file_definitions("a.sv", simple_def("FOO", "1"));
    // A call-shaped use can never expand a simple body (fatal in the engine).
    auto judgement = table.judge("FOO", {2}, "user.sv");
    EXPECT_EQ(judgement.state, kind::mismatch);
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

TEST(macro_table, conflict_representatives_group_by_body) {
    macro_table table;
    table.add_file_definitions("uvm_message_defines.svh", simple_def("MSG", "real_body"));
    table.add_file_definitions("pkg_a.sv", simple_def("MSG", "real_body"));
    table.add_file_definitions("pkg_b.sv", simple_def("MSG", "real_body"));
    table.add_file_definitions("custom_macros.svh", simple_def("MSG", "shim_body"));
    table.add_file_definitions("pkg_c.sv", simple_def("MSG", "shim_body"));
    auto judgement = table.judge("MSG", {}, "user.sv");
    ASSERT_EQ(judgement.state, kind::conflict);
    EXPECT_THAT(table.representative_definers("MSG", judgement.candidates),
                testing::ElementsAre("pkg_a.sv (+2 same-body)", "custom_macros.svh (+1 same-body)"));
}
