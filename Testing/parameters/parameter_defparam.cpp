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
#include "analysis/HDL_ast_builder_v2.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"

TEST(parameter_defparam, simple_override) {
    auto test_pattern = R"(
        module child #(
            parameter P = 0
        )();
        endmodule

        module test_mod ();
            child u1();
            defparam u1.P = 5;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});
    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto child = ast_v2->get_dependencies()[0];
    ASSERT_TRUE(child->has_parameter("P"));
    EXPECT_EQ(child->find_parameter("P")->get_numeric_value(), 5);
}

TEST(parameter_defparam, override_with_parent_param) {
    auto test_pattern = R"(
        module child #(
            parameter P = 0
        )();
        endmodule

        module test_mod #(
            parameter N = 7
        )();
            child u1();
            defparam u1.P = N + 1;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});
    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto child = ast_v2->get_dependencies()[0];
    ASSERT_TRUE(child->has_parameter("P"));
    EXPECT_EQ(child->find_parameter("P")->get_numeric_value(), 8);
}

TEST(parameter_defparam, nested_override) {
    auto test_pattern = R"(
        module leaf #(
            parameter Q = 0
        )();
        endmodule

        module mid ();
            leaf u3();
        endmodule

        module test_mod ();
            mid u2();
            defparam u2.u3.Q = 9;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});
    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto mid_node = ast_v2->get_dependencies()[0];
    auto leaf = mid_node->get_dependencies()[0];
    ASSERT_TRUE(leaf->has_parameter("Q"));
    EXPECT_EQ(leaf->find_parameter("Q")->get_numeric_value(), 9);
}

TEST(parameter_defparam, root_qualified_warns_and_does_not_crash) {
    auto test_pattern = R"(
        module child #(
            parameter P = 0
        )();
        endmodule

        module test_mod ();
            child u1();
            defparam $root.u1.P = 5;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});
    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto child = ast_v2->get_dependencies()[0];
    ASSERT_TRUE(child->has_parameter("P"));
    EXPECT_EQ(child->find_parameter("P")->get_numeric_value(), 0);
}

TEST(parameter_defparam, unit_scope_reference_accepted) {
    // $unit::W is legal SystemVerilog (compilation-unit scope resolution):
    // the file must be accepted and elaborate without crashing. ($unit-scope
    // lookup itself is not resolved.)
    auto test_pattern = R"(
        parameter int W = 8;
        module m ();
            localparam X = $unit::W;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    EXPECT_FALSE(analyzer.has_error());
    bool found_m = false;
    for (auto &res : resources.get_content()) {
        if (res->is<hdl_resource_statement>() &&
            res->as<hdl_resource_statement>().getName() == "m") found_m = true;
    }
    ASSERT_TRUE(found_m);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});
    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"m"}))[0];
    EXPECT_EQ(ast_v2->get_type(), "m");
}

TEST(parameter_defparam, unit_hierarchical_defparam_rejected) {
    // defparam $unit.u1.P is NOT legal SystemVerilog ($unit is a scope
    // qualifier, used with ::, not a hierarchical path prefix): it must fail
    // loudly at parse time - never silently, never with a crash.
    auto test_pattern = R"(
        module child #(
            parameter P = 0
        )();
        endmodule

        module test_mod ();
            child u1();
            defparam $unit.u1.P = 5;
        endmodule
    )";
    sv_analyzer analyzer;
    EXPECT_FALSE(analyzer.analyze("", test_pattern).has_value());
    EXPECT_TRUE(analyzer.has_error());
    EXPECT_TRUE(analyzer.has_syntax_errors());
}
