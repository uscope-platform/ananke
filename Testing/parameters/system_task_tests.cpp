//  Copyright 2023 Filippo Savi
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
#include "data_model/HDL/statement/hdl_statements.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "analysis/parameter_solver.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/HDL_builtin_function.hpp"
#include "data_model/HDL/parameters/components/token/Real_token.hpp"

using namespace std::string_literals;

TEST(system_task, simple) {
    auto test_pattern = R"(

    module test_mod #(
    )();

        localparam CAST = $rtoi(16.8);

    endmodule
    )";    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("CAST");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    auto call = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::rtoi);
    call->add_argument(std::make_shared<Real_token>("16.8"));

    p->set_raw_value(call);

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("CAST"), 16},
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(system_task, multiple) {
    auto test_pattern = R"(

    module test_mod #(
    )();

        localparam CAST = $rtoi(14.8+2);
        localparam CAST_2 = $rtoi(12.2);

    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("CAST");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    auto call = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::rtoi);
    Expression_v2 e;
    e.set_lhs(std::make_shared<Real_token>("14.8"));
    e.set_rhs(std::make_shared<Numeric_token>("2"));
    e.set_operation(Expression_v2::add);
    call->add_argument(std::make_shared<Expression_v2>(e));

    p->set_raw_value(call);

    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("CAST_2");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    call = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::rtoi);
    call->add_argument(std::make_shared<Real_token>("12.2"));
    p->set_raw_value(call);

    check_params.insert(p);
    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("CAST"), 16},
        {qualified_identifier("CAST_2"), 12},
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(system_task, propagation) {
    auto test_pattern = R"(

    module test_mod #(
        parameter PARAMETER_1 = 5
    )();

        localparam CAST = $rtoi(11.8 + PARAMETER_1);

    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("CAST");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    auto call = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::rtoi);
    Expression_v2 e;
    e.set_lhs(std::make_shared<Real_token>("11.8"));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("PARAMETER_1")));
    e.set_operation(Expression_v2::add);
    call->add_argument(std::make_shared<Expression_v2>(e));

    p->set_raw_value(call);

    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
    p->set_name("PARAMETER_1");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("5"));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("CAST"), 16},
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(system_task, nested) {
    auto test_pattern = R"(

    module test_mod #(
        parameter PARAMETER_1 = 46
    )();

        localparam CAST = $rtoi($ceil(PARAMETER_1/16.0));

    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("PARAMETER_1");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("46"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("CAST");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    auto inner_call = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::ceil);
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("PARAMETER_1")));
    e.set_operation(Expression_v2::divide);
    e.set_rhs(std::make_shared<Real_token>("16.0"));
    inner_call->add_argument(std::make_shared<Expression_v2>(e));
    auto outer_call = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::rtoi);
    outer_call->add_argument(inner_call);

    p->set_raw_value(outer_call);

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("PARAMETER_1"), 46},
        {qualified_identifier("CAST"), 3}
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(system_task, ln) {
    auto test_pattern = R"(
        module test_mod ();
            localparam V = $ln(1.0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("V")).get_real(), 0.0);
}

TEST(system_task, log10) {
    auto test_pattern = R"(
        module test_mod ();
            localparam V = $log10(100);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("V")).get_real(), 2.0);
}

TEST(system_task, sqrt) {
    auto test_pattern = R"(
        module test_mod ();
            localparam V = $sqrt(16);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("V")).get_real(), 4.0);
}

TEST(system_task, pow) {
    auto test_pattern = R"(
        module test_mod ();
            localparam V = $pow(2, 3);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("V")).get_real(), 8.0);
}

TEST(system_task, min_int) {
    auto test_pattern = R"(
        module test_mod ();
            localparam V = $min(5, 3, 7, 1);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("V")).get_integer(), 1);
}

TEST(system_task, max_int) {
    auto test_pattern = R"(
        module test_mod ();
            localparam V = $max(5, 3, 7, 1);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("V")).get_integer(), 7);
}

TEST(system_task, countones) {
    auto test_pattern = R"(
        module test_mod ();
            localparam V = $countones(8'b10110011);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("V")).get_integer(), 5);
}

TEST(system_task, ln_int_arg) {
    auto test_pattern = R"(
        module test_mod ();
            localparam BASE = 7;
            localparam V = $ln(BASE);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("V")).get_real(), std::log(7.0));
}

TEST(system_task, sqrt_int_arg) {
    auto test_pattern = R"(
        module test_mod ();
            localparam N = 25;
            localparam V = $sqrt(N);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("V")).get_real(), 5.0);
}

TEST(system_task, bits_sized) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [31:0] W = 32;
            parameter B = $bits(W);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 32);
}

TEST(system_task, bits_packed_dim) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0][3:0] V = 64;
            parameter B = $bits(V);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 32);
}

TEST(system_task, bits_type_argument_no_dependency) {
    auto test_pattern = R"(
        module test_mod ();
            parameter B = $bits(logic [7:0]);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto param = resource.get_parameters().get("B");
    EXPECT_TRUE(param->get_dependencies().data.empty());
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 8);
}

TEST(system_task, bits_dependency) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [15:0] X = 42;
            parameter B = $bits(X);
            parameter C = B + 1;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 16);
    EXPECT_EQ(defaults.at(qualified_identifier("C")).get_integer(), 17);
}

TEST(system_task, size) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] arr [3:0] = '{4{8'hFF}};
            parameter S = $size(arr);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("S")).get_integer(), 4);
}

TEST(system_task, high_low) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] arr [3:0] = '{4{8'hFF}};
            parameter H = $high(arr);
            parameter L = $low(arr);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("H")).get_integer(), 3);
    EXPECT_EQ(defaults.at(qualified_identifier("L")).get_integer(), 0);
}

TEST(system_task, left_right) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] arr [5:2] = '{4{8'hFF}};
            parameter LFT = $left(arr);
            parameter RGT = $right(arr);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("LFT")).get_integer(), 5);
    EXPECT_EQ(defaults.at(qualified_identifier("RGT")).get_integer(), 2);
}

TEST(system_task, signed) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] RAW = 8'hFF;
            parameter S = $signed(RAW);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("S")).get_integer(), -1);
}

TEST(system_task, unsigned) {
    auto test_pattern = R"(
        module test_mod ();
            parameter signed [3:0] NEG = -1;
            parameter U = $unsigned(NEG);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("U")).get_integer(), 15);
}

TEST(system_task, trig) {
    auto test_pattern = R"(
        module test_mod ();
            localparam S = $sin(0.0);
            localparam C = $cos(0.0);
            localparam T = $tan(0.0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("S")).get_real(), 0.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("C")).get_real(), 1.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("T")).get_real(), 0.0);
}

TEST(system_task, exp) {
    auto test_pattern = R"(
        module test_mod ();
            localparam E = $exp(0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("E")).get_real(), 1.0);
}

TEST(system_task, hypot) {
    auto test_pattern = R"(
        module test_mod ();
            localparam H = $hypot(3, 4);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("H")).get_real(), 5.0);
}

TEST(system_task, onehot) {
    auto test_pattern = R"(
        module test_mod ();
            localparam O1 = $onehot(8'b00010000);
            localparam O0 = $onehot(8'b00000000);
            localparam N1 = $onehot0(8'b00000000);
            localparam N2 = $onehot0(8'b00010000);
            localparam N3 = $onehot0(8'b00110000);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("O1")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("O0")).get_integer(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("N1")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("N2")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("N3")).get_integer(), 0);
}

TEST(system_task, isunknown) {
    auto test_pattern = R"(
        module test_mod ();
            localparam U = $isunknown(42);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("U")).get_integer(), 0);
}

TEST(system_task, isunbounded) {
    auto test_pattern = R"(
        module test_mod ();
            localparam U = $isunbounded(42);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("U")).get_integer(), 0);
}

TEST(system_task, hyperbolic) {
    auto test_pattern = R"(
        module test_mod ();
            localparam SH = $sinh(0.0);
            localparam CH = $cosh(0.0);
            localparam TH = $tanh(0.0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("SH")).get_real(), 0.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("CH")).get_real(), 1.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("TH")).get_real(), 0.0);
}

TEST(system_task, ahyperbolic) {
    auto test_pattern = R"(
        module test_mod ();
            localparam ASH = $asinh(0.0);
            localparam ACH = $acosh(1.0);
            localparam ATH = $atanh(0.0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("ASH")).get_real(), 0.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("ACH")).get_real(), 0.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("ATH")).get_real(), 0.0);
}

TEST(system_task, dimensions) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] arr [3:0] = '{default:0};
            localparam D = $dimensions(arr);
            localparam UD = $unpacked_dimensions(arr);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("D")).get_integer(), 2);
    EXPECT_EQ(defaults.at(qualified_identifier("UD")).get_integer(), 1);
}

TEST(system_task, dimensions_scalar) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [31:0] V = 0;
            localparam D = $dimensions(V);
            localparam UD = $unpacked_dimensions(V);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("D")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("UD")).get_integer(), 0);
}

TEST(system_task, typename) {
    auto test_pattern = R"(
        module test_mod ();
            parameter int V = 0;
            parameter TN = $typename(V);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("TN")).get_string(), "int");
}

TEST(system_task, rtoi_truncates) {
    auto test_pattern = R"(
        module test_mod ();
            localparam A = $rtoi(16.8);
            localparam B = $rtoi(-16.8);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("A")).get_integer(), 16);
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), -16);
}

TEST(system_task, min_max_single_arg) {
    auto test_pattern = R"(
        module test_mod ();
            localparam A = $min(42);
            localparam B = $max(7);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("A")).get_integer(), 42);
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 7);
}

TEST(system_task, ln_domain_guard) {
    auto test_pattern = R"(
        module test_mod ();
            localparam A = $ln(-1);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("A")).get_integer(), 0);
}

TEST(system_task, bits_literal) {
    auto test_pattern = R"(
        module test_mod ();
            localparam A = $bits(8'hFF);
            localparam B = $bits(1'b1);
            localparam C = $bits(42);
            localparam D = $bits('h3F);
            localparam E = $bits(1.0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("A")).get_integer(), 8);
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("C")).get_integer(), 32);
    EXPECT_EQ(defaults.at(qualified_identifier("D")).get_integer(), 32);
    EXPECT_EQ(defaults.at(qualified_identifier("E")).get_integer(), 64);
}

TEST(system_task, bits_expression) {
    auto test_pattern = R"(
        module test_mod ();
            localparam A = $bits(8'hFF + 1'b1);
            localparam B = $bits(16'(42));
            localparam C = $bits({8'hFF, 8'h00});
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("A")).get_integer(), 8);
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 16);
    EXPECT_EQ(defaults.at(qualified_identifier("C")).get_integer(), 16);
}

TEST(system_task, bits_type) {
    auto test_pattern = R"(
        module test_mod ();
            localparam A = $bits(bit [7:0]);
            localparam B = $bits(int);
            localparam C = $bits(bit [7:0][3:0]);
            localparam D = $bits(shortint);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("A")).get_integer(), 8);
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 32);
    EXPECT_EQ(defaults.at(qualified_identifier("C")).get_integer(), 32);
    EXPECT_EQ(defaults.at(qualified_identifier("D")).get_integer(), 16);
}

TEST(system_task, bits_unpacked_array) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] arr [3:0] = '{4{8'hFF}};
            parameter B = $bits(arr);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 32);
}

TEST(system_task, size_packed_only) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [31:0] V = 0;
            parameter S = $size(V);
            parameter L = $left(V);
            parameter R = $right(V);
            parameter H = $high(V);
            parameter L2 = $low(V);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("S")).get_integer(), 32);
    EXPECT_EQ(defaults.at(qualified_identifier("L")).get_integer(), 31);
    EXPECT_EQ(defaults.at(qualified_identifier("R")).get_integer(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("H")).get_integer(), 31);
    EXPECT_EQ(defaults.at(qualified_identifier("L2")).get_integer(), 0);
}

TEST(system_task, packed_dim_numbering) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0][3:0] V = 0;
            localparam L1 = $left(V, 1);
            localparam R1 = $right(V, 1);
            localparam S1 = $size(V, 1);
            localparam L2 = $left(V, 2);
            localparam S2 = $size(V, 2);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("L1")).get_integer(), 7);
    EXPECT_EQ(defaults.at(qualified_identifier("R1")).get_integer(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("S1")).get_integer(), 8);
    EXPECT_EQ(defaults.at(qualified_identifier("L2")).get_integer(), 3);
    EXPECT_EQ(defaults.at(qualified_identifier("S2")).get_integer(), 4);
}

TEST(system_task, size_packed_dim_after_unpacked) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] arr [3:0] = '{4{8'hFF}};
            parameter S1 = $size(arr, 1);
            parameter S2 = $size(arr, 2);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("S1")).get_integer(), 4);
    EXPECT_EQ(defaults.at(qualified_identifier("S2")).get_integer(), 8);
}

TEST(system_task, size_literal) {
    auto test_pattern = R"(
        module test_mod ();
            localparam S = $size(8'hFF);
            localparam D = $dimensions(8'hFF);
            localparam UD = $unpacked_dimensions(8'hFF);
            localparam DR = $dimensions(1.0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("S")).get_integer(), 8);
    EXPECT_EQ(defaults.at(qualified_identifier("D")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("UD")).get_integer(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("DR")).get_integer(), 0);
}

TEST(system_task, signed_unsigned_literals) {
    auto test_pattern = R"(
        module test_mod ();
            localparam S = $signed(8'hFF);
            localparam U = $unsigned(8'hFF);
            localparam S2 = $signed(42);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("S")).get_integer(), -1);
    EXPECT_EQ(defaults.at(qualified_identifier("U")).get_integer(), 255);
    EXPECT_EQ(defaults.at(qualified_identifier("S2")).get_integer(), 42);
}

TEST(system_task, type_queries_on_type) {
    auto test_pattern = R"(
        module test_mod ();
            localparam S = $size(bit [7:0]);
            localparam L = $left(bit [7:0]);
            localparam R = $right(bit [7:0]);
            localparam D = $dimensions(bit [7:0]);
            localparam UD = $unpacked_dimensions(bit [7:0]);
            localparam S2 = $size(bit [7:0][3:0], 2);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("S")).get_integer(), 8);
    EXPECT_EQ(defaults.at(qualified_identifier("L")).get_integer(), 7);
    EXPECT_EQ(defaults.at(qualified_identifier("R")).get_integer(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("D")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("UD")).get_integer(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("S2")).get_integer(), 4);
}

TEST(system_task, type_queries_expression) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] A = 8'h1;
            localparam B = $bits(A + 1'b1);
            localparam S = $size(A + 1'b1);
            localparam L = $left(A + 1'b1);
            localparam R = $right(A + 1'b1);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 8);
    EXPECT_EQ(defaults.at(qualified_identifier("S")).get_integer(), 8);
    EXPECT_EQ(defaults.at(qualified_identifier("L")).get_integer(), 7);
    EXPECT_EQ(defaults.at(qualified_identifier("R")).get_integer(), 0);
}

TEST(system_task, log2) {
    auto test_pattern = R"(
        module test_mod ();
            localparam V = $log2(8);
            localparam F = $log2(0.5);
            localparam N = $log2(0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("V")).get_real(), 3.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("F")).get_real(), -1.0);
    EXPECT_EQ(defaults.at(qualified_identifier("N")).get_integer(), 0);
}

TEST(system_task, inverse_trig) {
    auto test_pattern = R"(
        module test_mod ();
            localparam A = $asin(1.0);
            localparam AC = $acos(1.0);
            localparam AT = $atan(1.0);
            localparam AT2 = $atan2(1.0, 1.0);
            localparam OOD = $asin(2.0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("A")).get_real(), std::asin(1.0));
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("AC")).get_real(), 0.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("AT")).get_real(), std::atan(1.0));
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("AT2")).get_real(), std::atan2(1.0, 1.0));
    EXPECT_EQ(defaults.at(qualified_identifier("OOD")).get_integer(), 0);
}

TEST(system_task, round_truncate) {
    auto test_pattern = R"(
        module test_mod ();
            localparam R = $round(2.5);
            localparam RN = $round(-2.5);
            localparam T = $truncate(2.7);
            localparam TN = $truncate(-2.7);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("R")).get_real(), 3.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("RN")).get_real(), -3.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("T")).get_real(), 2.0);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("TN")).get_real(), -2.0);
}

TEST(system_task, countbits) {
    auto test_pattern = R"(
        module test_mod ();
            localparam C1 = $countbits(8'b10110011, 1'b1);
            localparam C0 = $countbits(8'b10110011, 1'b0);
            localparam CZ = $countbits(8'b00000001, 1'b0);
            localparam CA = $countbits(8'b00000001, 2'b01);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("C1")).get_integer(), 5);
    EXPECT_EQ(defaults.at(qualified_identifier("C0")).get_integer(), 3);
    EXPECT_EQ(defaults.at(qualified_identifier("CZ")).get_integer(), 7);
    EXPECT_EQ(defaults.at(qualified_identifier("CA")).get_integer(), 8);
}

TEST(system_task, real_bits_conversion) {
    auto test_pattern = R"(
        module test_mod ();
            localparam RB = $realtobits(1.0);
            localparam BR = $bitstoreal(64'h3FF0000000000000);
            localparam SRB = $shortrealtobits(1.0);
            localparam SBR = $bitstoshortreal(32'h3F800000);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("RB")).get_integer(), static_cast<hdl_integer>(0x3FF0000000000000ULL));
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("BR")).get_real(), 1.0);
    EXPECT_EQ(defaults.at(qualified_identifier("SRB")).get_integer(), static_cast<hdl_integer>(0x3F800000));
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("SBR")).get_real(), 1.0);
}

TEST(system_task, string_functions) {
    auto test_pattern = R"(
        module test_mod ();
            localparam L = $len("hello");
            localparam U = $to_upper("hello");
            localparam LO = $to_lower("HeLLo");
            localparam I = $atoi("-42");
            localparam C = $compare("abc", "abd");
            localparam S = $substr("hello", 1, 3);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("L")).get_integer(), 5);
    EXPECT_EQ(defaults.at(qualified_identifier("U")).get_string(), "HELLO");
    EXPECT_EQ(defaults.at(qualified_identifier("LO")).get_string(), "hello");
    EXPECT_EQ(defaults.at(qualified_identifier("I")).get_integer(), -42);
    EXPECT_EQ(defaults.at(qualified_identifier("C")).get_integer(), -1);
    EXPECT_EQ(defaults.at(qualified_identifier("S")).get_string(), "ell");
}

TEST(system_task, sformatf) {
    auto test_pattern = R"(
        module test_mod ();
            localparam F1 = $sformatf("val=%d", 42);
            localparam F2 = $sformatf("hex=%0x", 255);
            localparam F3 = $sformatf("%s %d", "x", 7);
            localparam F4 = $sformatf("bin=%0b", 5);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("F1")).get_string(), "val=42");
    EXPECT_EQ(defaults.at(qualified_identifier("F2")).get_string(), "hex=ff");
    EXPECT_EQ(defaults.at(qualified_identifier("F3")).get_string(), "x 7");
    EXPECT_EQ(defaults.at(qualified_identifier("F4")).get_string(), "bin=101");
}

TEST(system_task, unknown_function_defaults_to_zero) {
    auto test_pattern = R"(
        module test_mod ();
            localparam X = $bogus_fn(3);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer(), 0);
}



