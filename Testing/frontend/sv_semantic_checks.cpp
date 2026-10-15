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

#include "frontend/analysis/system_verilog/sv_analyzer.hpp"
#include "frontend/analysis/system_verilog/sv_semantic_checker.hpp"
#include "data_model/parse_options.hpp"

TEST(sv_semantic_checker, builtin_class_methods_rejected) {
    sv_semantic_checker checker;
    for (const auto &name : {"randomize", "rand_mode", "constraint_mode"}) {
        EXPECT_TRUE(checker.check_class_method("test.sv", 3, name)) << name;
    }
    const auto &violations = checker.get_violations();
    ASSERT_EQ(violations.size(), 3);
    EXPECT_TRUE(checker.has_violations());
    for (const auto &v : violations) {
        EXPECT_EQ(v.code, semantic_error_code::builtin_method_override);
    }
    EXPECT_NE(violations[0].message.find("randomize"), std::string::npos);
    EXPECT_EQ(checker.last_violation().message, violations.back().message);
}

TEST(sv_semantic_checker, exit_code_owned_by_checker) {
    EXPECT_EQ(
        sv_semantic_checker::exit_code_for(semantic_error_code::builtin_method_override),
        56);
}

TEST(sv_semantic_checker, legal_method_names_accepted) {
    sv_semantic_checker checker;
    for (const auto &name :
         {"new", "pre_randomize", "post_randomize", "my_func", "randomize_if"}) {
        EXPECT_FALSE(checker.check_class_method("test.sv", 3, name)) << name;
    }
    EXPECT_FALSE(checker.has_violations());
    EXPECT_TRUE(checker.get_violations().empty());
}

TEST(sv_semantic_checker, disabled_checks_accept_everything) {
    sv_semantic_checker checker;
    parse_options opts;
    opts.disable_semantic_checks = true;
    checker.set_options(opts);
    for (const auto &name : {"randomize", "rand_mode", "constraint_mode"}) {
        EXPECT_FALSE(checker.check_class_method("test.sv", 3, name)) << name;
    }
    EXPECT_FALSE(checker.has_violations());
}

TEST(sv_semantic_checks, class_override_fails_analysis) {
    auto test_pattern = R"(
        class a1;
            rand int x;
            function int rand_mode();
                return 1;
            endfunction
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_FALSE(result.has_value());
    EXPECT_TRUE(analyzer.has_error());
    const auto &violations = analyzer.get_semantic_violations();
    ASSERT_EQ(violations.size(), 1);
    EXPECT_EQ(violations[0].code, semantic_error_code::builtin_method_override);
    EXPECT_NE(violations[0].message.find("rand_mode"), std::string::npos);
    EXPECT_EQ(analyzer.get_error(), violations[0].message);
}

TEST(sv_semantic_checks, class_override_passes_when_disabled) {
    auto test_pattern = R"(
        class a1;
            rand int x;
            function int rand_mode();
                return 1;
            endfunction
        endclass
    )";

    sv_analyzer analyzer;
    parse_options opts;
    opts.disable_semantic_checks = true;
    analyzer.set_options(opts);
    auto result = analyzer.analyze("test.sv", test_pattern);
    EXPECT_TRUE(result.has_value());
}

TEST(sv_semantic_checks, randomization_hooks_stay_legal) {
    auto test_pattern = R"(
        class c;
            rand int x;
            function void pre_randomize();
            endfunction
            function void post_randomize();
            endfunction
        endclass
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}

TEST(sv_semantic_checks, package_function_with_builtin_name_stays_legal) {
    auto test_pattern = R"(
        package p;
            function int randomize();
                return 1;
            endfunction
        endpackage
    )";

    sv_analyzer analyzer;
    EXPECT_TRUE(analyzer.analyze("test.sv", test_pattern).has_value());
}
