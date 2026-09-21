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
#include "analysis/parameter_solver.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"

TEST(type_parameter_extraction, with_default) {
    auto test_pattern = R"(
        module test_mod #(
            parameter type T = int
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    std::vector<std::shared_ptr<HDL_parameter>> check_params;
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("T");
    p->is_type_param = true;
    p->set_type(Type_engine::create_primitive_type("int"));
    check_params.push_back(p);

    ASSERT_EQ(check_params.size(), parameters.size());
    for (size_t i = 0; i < check_params.size(); ++i) {
        ASSERT_EQ(*check_params[i], *parameters[i]);
    }
}

TEST(type_parameter_extraction, no_default) {
    auto test_pattern = R"(
        module test_mod #(
            parameter type T
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    std::vector<std::shared_ptr<HDL_parameter>> check_params;
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("T");
    p->is_type_param = true;
    check_params.push_back(p);

    ASSERT_EQ(check_params.size(), parameters.size());
    for (size_t i = 0; i < check_params.size(); ++i) {
        ASSERT_EQ(*check_params[i], *parameters[i]);
    }
}

TEST(type_parameter_extraction, unsigned_type) {
    auto test_pattern = R"(
        module test_mod #(
            parameter type B = bit
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    std::vector<std::shared_ptr<HDL_parameter>> check_params;
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("B");
    p->is_type_param = true;
    p->set_type(Type_engine::create_primitive_type("bit"));
    check_params.push_back(p);

    ASSERT_EQ(check_params.size(), parameters.size());
    for (size_t i = 0; i < check_params.size(); ++i) {
        ASSERT_EQ(*check_params[i], *parameters[i]);
    }
}

TEST(type_parameter_extraction, localparam_type) {
    auto test_pattern = R"(
        module test_mod ();
            localparam type U = int;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    std::vector<std::shared_ptr<HDL_parameter>> check_params;
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("U");
    p->is_type_param = true;
    p->set_type(Type_engine::create_primitive_type("int"));
    check_params.push_back(p);

    ASSERT_EQ(check_params.size(), parameters.size());
    for (size_t i = 0; i < check_params.size(); ++i) {
        ASSERT_EQ(*check_params[i], *parameters[i]);
    }
}

TEST(type_parameter_extraction, processing_with_default) {
    auto test_pattern = R"(
        module test_mod #(
            parameter type T = int,
            parameter X = 42
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto result = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    ASSERT_TRUE(result.contains(qualified_identifier("T")));
    ASSERT_TRUE(result.contains(qualified_identifier("X")));
    EXPECT_EQ(result.at(qualified_identifier("X")).get_integer(), 42);
}

TEST(type_parameter_extraction, processing_no_default) {
    auto test_pattern = R"(
        module test_mod #(
            parameter type T,
            parameter X = 7
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto result = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    ASSERT_TRUE(result.contains(qualified_identifier("T")));
    ASSERT_TRUE(result.contains(qualified_identifier("X")));
    EXPECT_EQ(result.at(qualified_identifier("X")).get_integer(), 7);
}

TEST(type_parameter_extraction, processing_type_param_chain) {
    auto test_pattern = R"(
        module test_mod #(
            parameter type T = int,
            parameter type U = T,
            parameter B = $bits(T)
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto result = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    EXPECT_EQ(result.at(qualified_identifier("B")).get_integer(), 32);
}

TEST(type_parameter_extraction, processing_bits_of_type_param) {
    auto test_pattern = R"(
        module test_mod #(
            parameter type T = shortint,
            parameter B = $bits(T)
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto result = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    EXPECT_EQ(result.at(qualified_identifier("B")).get_integer(), 16);
}

TEST(type_parameter_extraction, processing_multiple_type_params) {
    auto test_pattern = R"(
        module test_mod #(
            parameter type A = int,
            parameter type B = bit,
            parameter C = 99
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto result = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    ASSERT_TRUE(result.contains(qualified_identifier("A")));
    ASSERT_TRUE(result.contains(qualified_identifier("B")));
    ASSERT_TRUE(result.contains(qualified_identifier("C")));
    EXPECT_EQ(result.at(qualified_identifier("C")).get_integer(), 99);
}

TEST(type_parameter_extraction, override_type_param) {
    auto test_pattern = R"(
        module child #(
            parameter type T = bit
        )();
        endmodule

        module parent #(
            parameter type U = int
        )();
            child #(.T(U)) inst();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});
    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"parent"}))[0];

    auto child = ast_v2->get_dependencies()[0];
    ASSERT_TRUE(child->has_parameter("T"));
    auto t = child->find_parameter("T");
    ASSERT_NE(t, nullptr);
    ASSERT_TRUE(t->is_type_param);
    ASSERT_TRUE(t->get_type() != nullptr);
    EXPECT_TRUE(t->get_type()->is<HDL_simple_type>());
    EXPECT_TRUE(t->get_type()->as<HDL_simple_type>().get_signed());
}

TEST(type_parameter_extraction, override_recomputes_dependent_param) {
    auto test_pattern = R"(
        module child #(
            parameter type T = int,
            parameter W = $bits(T)
        )();
        endmodule

        module parent #(
            parameter type U = shortint
        )();
            child #(.T(U)) inst();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});
    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"parent"}))[0];

    auto child = ast_v2->get_dependencies()[0];
    ASSERT_TRUE(child->has_parameter("T"));
    auto t_param = child->find_parameter("T");
    ASSERT_NE(t_param, nullptr);
    ASSERT_TRUE(t_param->is_type_param);
    ASSERT_TRUE(child->has_parameter("W"));
    auto w_param = child->find_parameter("W");
    ASSERT_NE(w_param, nullptr);
    EXPECT_EQ(w_param->get_numeric_value(), 16);
}

TEST(type_parameter_extraction, used_in_port_declaration) {
    auto test_pattern = R"(
        module test_mod #(
            parameter type T = int
        )(
            input T data_in,
            output T data_out
        );
        endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    auto resource = resources.get_content()[0]->as<hdl_resource_statement>();
    // THis test mainly verifies that using a parametrized type on a port does not crash the program
    std::shared_ptr<HDL_parameter> t;
    for (const auto &d : resource.get_parameter_statements())
        if (d->get_name() == "T") t = d;
    ASSERT_NE(t, nullptr);
    ASSERT_TRUE(t->is_type_param);
    EXPECT_TRUE(t->get_type()->is<HDL_simple_type>());
    EXPECT_TRUE(t->get_type()->as<HDL_simple_type>().get_signed());
}
