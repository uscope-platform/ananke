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

// Compilation-order macros (the riscv-dv dv_defines.svh shape: a header
// defines macros, a fragment uses them with no `include). Exercises the
// quarantine protocol directly: harvest one unit's definitions, inject them
// as another unit's base, resolve uniquely-defined macros, and drop
// consumers of missing or conflicting ones.

#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "frontend/analysis/system_verilog/sv_analyzer.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"
#include "frontend/macro_table.hpp"

TEST(macro_quarantine, resolves_order_dependent_macros) {
    auto defs_pattern = R"(
        `define SHARED_W 8
        `define ADD1(x) ((x)+1)
    )";
    auto fragment_pattern = R"(
        module user_mod #(
            parameter W = `SHARED_W,
            parameter V = `ADD1(41)
        )(
            input logic clock
        );
        endmodule
    )";

    // Pass 1, no base knowledge: the fragment fails, recording its needs.
    sv_analyzer frag_cold;
    EXPECT_FALSE(frag_cold.analyze("", fragment_pattern).has_value());
    EXPECT_THAT(frag_cold.get_undefined_macros(), testing::ElementsAre("ADD1", "SHARED_W"));

    // The header parses standalone and harvests both macros.
    sv_analyzer defs;
    ASSERT_TRUE(defs.analyze("", defs_pattern).has_value());
    const auto &harvested = defs.get_harvested_definitions();
    ASSERT_TRUE(harvested.contains("SHARED_W"));
    ASSERT_TRUE(harvested.contains("ADD1"));

    // Pass 2, harvested definitions injected as base: full resolution.
    sv_analyzer frag_warm;
    frag_warm.set_injected_definitions(harvested);
    auto processed = frag_warm.preprocess("", fragment_pattern);
    auto check_string = R"(
        module user_mod #(
            parameter W = 8,
            parameter V = ((41)+1)
        )(
            input logic clock
        );
        endmodule
    )";
    EXPECT_EQ(processed.first, check_string);
    EXPECT_FALSE(frag_warm.has_error());
    auto resource = frag_warm.analyze("", fragment_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    EXPECT_EQ(resource.getName(), "user_mod");
}

TEST(macro_quarantine, truly_undefined_macros_still_dropped) {
    auto test_pattern = R"(
        module bad_mod #(
            parameter W = `NOPE_NOWHERE
        )(
            input logic clock
        );
        endmodule
    )";

    sv_analyzer analyzer;
    EXPECT_FALSE(analyzer.analyze("", test_pattern).has_value());
    EXPECT_FALSE(analyzer.has_fatal_error());
    EXPECT_THAT(analyzer.get_undefined_macros(), testing::ElementsAre("NOPE_NOWHERE"));
}

TEST(macro_quarantine, conflicting_definitions_error_out_consumer) {
    auto header_a = R"(
        `define CLASHING 1
    )";
    auto header_b = R"(
        `define CLASHING 2
    )";
    auto consumer = R"(
        module clash_user #(
            parameter W = `CLASHING
        )(
            input logic clock
        );
        endmodule
    )";

    sv_analyzer analyzer_a;
    ASSERT_TRUE(analyzer_a.analyze("", header_a).has_value());
    sv_analyzer analyzer_b;
    ASSERT_TRUE(analyzer_b.analyze("", header_b).has_value());

    macro_table table;
    table.add_file_definitions("h1.svh", analyzer_a.get_harvested_definitions());
    table.add_file_definitions("h2.svh", analyzer_b.get_harvested_definitions());

    // Same bodies would merge; distinct bodies conflict with both definers.
    auto injection = table.build_injection({"CLASHING"}, "user.sv");
    EXPECT_TRUE(injection.definitions.empty());
    ASSERT_EQ(injection.conflicts.size(), 1u);
    EXPECT_THAT(injection.conflicts.at("CLASHING"), testing::ElementsAre("h1.svh", "h2.svh"));

    // Without a pickable definition the consumer cannot parse.
    sv_analyzer analyzer_user;
    EXPECT_FALSE(analyzer_user.analyze("", consumer).has_value());
}

TEST(macro_quarantine, include_guard_not_inverted) {
    auto guard_pattern = R"(
        `ifndef GUARDED_SV
        `define GUARDED_SV
        module guarded_mod (
            input logic clock
        );
        endmodule
        `endif
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", guard_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    EXPECT_EQ(resource.getName(), "guarded_mod");

    // The guard is harvested under its own file: injecting it back into the
    // same file would invert the guard and parse an empty file, so the table
    // must exclude self-definers ...
    macro_table table;
    table.add_file_definitions("guarded.sv", analyzer.get_harvested_definitions());
    EXPECT_TRUE(table.build_injection({"GUARDED_SV"}, "guarded.sv").definitions.empty());
    // ... while other files still resolve it normally.
    EXPECT_EQ(table.build_injection({"GUARDED_SV"}, "other.sv").definitions.size(), 1u);
}
