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
#include "data_model/HDL/parameters/components/Replication.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/parameters/components/Cast.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/HDL_builtin_function.hpp"
#include "data_model/HDL/parameters/components/Ternary.hpp"
#include "data_model/HDL/parameters/components/token/Real_token.hpp"
#include "data_model/HDL/parameters/components/token/Time_token.hpp"

using namespace std::string_literals;

TEST(parameter_extraction, init_list_after_reg) {
    auto test_pattern = R"(

    module test_module ();

        reg [5:0] low_data_n [2:0] = '{
            'b101011, //D.31.y
            'b011110, //D.30.y
            'b101110 //D.29.y
        };


        localparam low_control_p = 'b001111;
        localparam low_control_n = 'b110000;
    endmodule
)";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("low_control_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("'b001111"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("low_control_n");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("'b110000"));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("low_control_p"), 15},
        {qualified_identifier("low_control_n"), 48}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, size_cast) {
    auto test_pattern = R"(
        module test_mod #(
            )();

            parameter integer TEST_PARAM = 4'(31'h100003);

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    Cast c;
    c.set_size(std::make_shared<Numeric_token>("4"));
    c.set_content(std::make_shared<Numeric_token>("31'h100003"));
    p->set_raw_value(std::make_shared<Cast>(c));
    p->set_type(Type_engine::create_primitive_type("integer"));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(3, defaults.at(qualified_identifier("TEST_PARAM")).get_integer());
}


TEST(parameter_extraction, unsized_zero) {
    auto test_pattern = R"(
        module test_mod #(
            )();
            parameter integer TEST_PARAM = '0;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    p->set_raw_value(std::make_shared<Numeric_token>("'0"));
    p->set_type(Type_engine::create_primitive_type("integer"));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(0, defaults.at(qualified_identifier("TEST_PARAM")).get_integer());
}




TEST(parameter_extraction, unsized_one) {
    auto test_pattern = R"(
        module test_mod #(
            )();
            parameter reg [7:0] TEST_PARAM = '1;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    p->set_raw_value(std::make_shared<Numeric_token>("'1"));
    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("7");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(255, defaults.at(qualified_identifier("TEST_PARAM")).get_integer());
}


TEST(parameter_extraction, paretesized_cast) {
    auto test_pattern = R"(
        module test_mod #(
            )();
            parameter integer SIZE = 4;
            parameter integer TEST_PARAM = (SIZE)'(31'h100003);

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("SIZE");
    p->set_raw_value(std::make_shared<Numeric_token>(4, 32));
    p->set_type(Type_engine::create_primitive_type("integer"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");

    Cast c;
    c.set_size(std::make_shared<Identifier_token>(qualified_identifier("SIZE")));
    c.set_content(std::make_shared<Numeric_token>("31'h100003"));
    p->set_raw_value(std::make_shared<Cast>(c));
    p->set_type(Type_engine::create_primitive_type("integer"));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(3, defaults.at(qualified_identifier("TEST_PARAM")).get_integer());
}


TEST(parameter_extraction, type_cast) {
    auto test_pattern = R"(
        module test_mod #()();

            parameter reg [7:0] TEST_PARAM = unsigned'(-5);

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");

    Cast c;
    c.set_type_cast();
    c.set_target_type("unsigned");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("5"));
    e.set_operation(Expression_v2::subtract);
    c.set_content(std::make_shared<Expression_v2>(e));
    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("7");
    d.second_bound =  std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Cast>(c));

    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(251, defaults.at(qualified_identifier("TEST_PARAM")).get_integer());
}


TEST(parameter_extraction, nested_type_cast) {
    auto test_pattern = R"(
        module test_mod #()();
            localparam NumLevels = 4;
            parameter reg [7:0] TEST_PARAM = (NumLevels)'(unsigned'(-5));

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;



    Cast inner_c;
    inner_c.set_type_cast();
    inner_c.set_target_type("unsigned");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("5"));
    e.set_operation(Expression_v2::subtract);
    inner_c.set_content(std::make_shared<Expression_v2>(e));

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");

    Cast outer_c;
    outer_c.set_size(std::make_shared<Identifier_token>(qualified_identifier("NumLevels")));
    outer_c.set_content(std::make_shared<Cast>(inner_c));
    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("7");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Cast>(outer_c));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("NumLevels");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>(4, 32));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(11, defaults.at(qualified_identifier("TEST_PARAM")).get_integer());
}


TEST(parameter_extraction, multiple_type_cast) {
    auto test_pattern = R"(
        module test_mod #()();

            parameter reg [7:0] TEST_PARAM = unsigned'(-5);
            parameter TEST_PARAM_2 = int'(2.5);

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    Cast c;
    c.set_type_cast();
    c.set_target_type("unsigned");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("5"));
    e.set_operation(Expression_v2::subtract);
    c.set_content(std::make_shared<Expression_v2>(e));
    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("7");
    d.second_bound =  std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Cast>(c));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM_2");
    auto param_type2 = Type_engine::create_primitive_type("implicit");
    p->set_type(param_type2);

    c = Cast();
    c.set_type_cast();
    c.set_target_type("int");
    c.set_content(std::make_shared<Real_token>("2.5"));
    p->set_raw_value(std::make_shared<Cast>(c));

    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(251, defaults.at(qualified_identifier("TEST_PARAM")).get_integer());
    ASSERT_EQ(3, defaults.at(qualified_identifier("TEST_PARAM_2")).get_integer());
}

TEST(parameter_extraction, cast_in_binary_expression) {
    auto test_pattern = R"(
        module test_mod #()();
            parameter integer x = -2;
            parameter integer y = 'hfffffffa;
            parameter integer A = unsigned'(x) + 1;
            parameter integer B = signed'(y) - 2;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("x");
    p->set_type(Type_engine::create_primitive_type("integer"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("2"));
    e.set_operation(Expression_v2::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
    p->set_name("y");
    p->set_type(Type_engine::create_primitive_type("integer"));
    p->set_raw_value(std::make_shared<Numeric_token>(4294967290, 32));
    check_params.insert(p);


    auto cast_a = std::make_shared<Cast>();
    cast_a->set_type_cast();
    cast_a->set_target_type("unsigned");
    cast_a->set_content(std::make_shared<Identifier_token>(qualified_identifier("x")));
    e.set_lhs(cast_a);
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::add);

    p = std::make_shared<HDL_parameter>();
    p->set_name("A");
    p->set_type(Type_engine::create_primitive_type("integer"));
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    auto cast_b = std::make_shared<Cast>();
    cast_b->set_type_cast();
    cast_b->set_target_type("signed");
    cast_b->set_content(std::make_shared<Identifier_token>(qualified_identifier("y")));
    e.set_lhs(cast_b);
    e.set_rhs(std::make_shared<Numeric_token>("2"));
    e.set_operation(Expression_v2::subtract);

    p = std::make_shared<HDL_parameter>();
    p->set_name("B");
    p->set_type(Type_engine::create_primitive_type("integer"));
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());
    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("x"), -2},
        {qualified_identifier("y"), -6},
        {qualified_identifier("A"), -1},
        {qualified_identifier("B"), -8},
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction,time_literal) {
    auto test_pattern = R"(
        module test_mod #(
            )();

            parameter TEST_PARAM = 10ns;
            parameter TEST_PARAM_2 = 1.5us;

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    p->set_raw_value(std::make_shared<Time_token>("10ns"));
    p->set_type(Type_engine::create_primitive_type("implicit"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM_2");
    p->set_raw_value(std::make_shared<Time_token>("1.5us"));
    p->set_type(Type_engine::create_primitive_type("implicit"));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("TEST_PARAM")).get_real(), 1e-8);
    EXPECT_DOUBLE_EQ(defaults.at(qualified_identifier("TEST_PARAM_2")).get_real(), 1.5e-6);
}



TEST(parameter_extraction, cast_in_concat) {
    auto test_pattern = R"(
        module test_mod #(
            )();

            parameter integer TEST_PARAM = { 10'h0, 1'b1, 1'b0, 4'(31'h100003)};

        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");

    Concatenation concat;
    concat.add_component(std::make_shared<Numeric_token>("10'h0"));
    concat.add_component(std::make_shared<Numeric_token>("1'h1"));
    concat.add_component(std::make_shared<Numeric_token>("1'h0"));
    Cast c;
    c.set_size(std::make_shared<Numeric_token>("4"));
    c.set_content(std::make_shared<Numeric_token>("31'h100003"));
    concat.add_component(std::make_shared<Cast>(c));
    auto param_type = Type_engine::create_primitive_type("integer");
    p->set_type(param_type);
    p->set_raw_value(std::make_shared<Concatenation>(concat));



    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(35, defaults.at(qualified_identifier("TEST_PARAM")).get_integer());
}


TEST(parameter_extraction, strings_dafault_init) {
    auto test_pattern = R"(
        module test_mod #(
            )();

            parameter N_CORES = 3;
            parameter string TRANSLATION_TABLE_INIT[3:0] = '{default:"FILE"};

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("N_CORES");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("3"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TRANSLATION_TABLE_INIT");

    auto param_type = HDL_simple_type();
    param_type.add_dimension({
        std::make_shared<Numeric_token>("3"),
        std::make_shared<Numeric_token>("0"),
        false
    });
    Concatenation c;
    c.set_default_init();
    c.add_component(std::make_shared<String_token>("\"FILE\""));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<std::string> tti_val;
    tti_val.set_1d_slice({0, 0}, {"\"FILE\"", "\"FILE\"", "\"FILE\"", "\"FILE\""});

    ASSERT_EQ(3, defaults.at(qualified_identifier("N_CORES")).get_integer());
    ASSERT_EQ(tti_val, defaults.at(qualified_identifier("TRANSLATION_TABLE_INIT")).get_string_array());
}


TEST(parameter_extraction, string_array_selection) {
    auto test_pattern = R"(
        module test_mod #(
            )();

            parameter N_CORES = 3;
            parameter string TRANSLATION_TABLE_INIT[3:0] = '{default:"FILE"};

            parameter string SEL = TRANSLATION_TABLE_INIT[2];
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("N_CORES");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("3"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TRANSLATION_TABLE_INIT");

    auto param_type = HDL_simple_type();
    param_type.add_dimension({
        std::make_shared<Numeric_token>("3"),
        std::make_shared<Numeric_token>("0"),
        false
    });
    Concatenation c;
    c.set_default_init();
    c.add_component(std::make_shared<String_token>("\"FILE\""));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));

    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("SEL");
    auto t = Identifier_token(qualified_identifier("TRANSLATION_TABLE_INIT"));
    std::vector<std::shared_ptr<Expression_base>> ai;
    ai.push_back(std::make_shared<Numeric_token>("2"));
    t.set_array_index(ai);
    p->set_raw_value(std::make_shared<Identifier_token>(t));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<std::string> tti_val;
    tti_val.set_1d_slice({0, 0}, {"\"FILE\"", "\"FILE\"", "\"FILE\"", "\"FILE\""});

    ASSERT_EQ(3, defaults.at(qualified_identifier("N_CORES")).get_integer());
    ASSERT_EQ(tti_val, defaults.at(qualified_identifier("TRANSLATION_TABLE_INIT")).get_string_array());
}

TEST(parameter_extraction, strings_array) {
    auto test_pattern = R"(
        module test_mod #(
            )();

            parameter N_CORES = 3;
            parameter string TRANSLATION_TABLE_INIT[N_CORES-1:0] = '{
                "FILE",
                "FILE",
                "FILE"
            };

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("N_CORES");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("3"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TRANSLATION_TABLE_INIT");

    auto param_type = HDL_simple_type();
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N_CORES")));
    e.set_operation(Expression_v2::subtract);
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    param_type.add_dimension({
        std::make_shared<Expression_v2>(e),
        std::make_shared<Numeric_token>("0"),
        false
    });
    Concatenation c;
    c.add_component(std::make_shared<String_token>("\"FILE\""));
    c.add_component(std::make_shared<String_token>("\"FILE\""));
    c.add_component(std::make_shared<String_token>("\"FILE\""));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<std::string> tti_val;
    tti_val.set_1d_slice({0, 0}, {"\"FILE\"", "\"FILE\"", "\"FILE\""});

    ASSERT_EQ(3,defaults.at(qualified_identifier("N_CORES")).get_integer());
    ASSERT_EQ(tti_val, defaults.at(qualified_identifier("TRANSLATION_TABLE_INIT")).get_string_array());
}


TEST(parameter_extraction, float_parameter) {
    auto test_pattern = R"(
        module test_mod #(
            parameter LUT_DEPTH   = 9
            )();

            localparam STEP   = (2*3.14159265358979323846/4.0) / LUT_DEPTH;

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("LUT_DEPTH");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("9"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("STEP");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e1, e2, e3;
    e3.set_lhs(std::make_shared<Numeric_token>("2"));
    e3.set_rhs(std::make_shared<Real_token>("3.14159265358979323846"));
    e3.set_operation(Expression_v2::multiply);
    e2.set_lhs(std::make_shared<Expression_v2>(e3));
    e2.set_rhs(std::make_shared<Real_token>("4.0"));
    e2.set_operation(Expression_v2::divide);
    e1.set_lhs(std::make_shared<Expression_v2>(e2));
    e1.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("LUT_DEPTH")));
    e1.set_operation(Expression_v2::divide);
    p->set_raw_value(std::make_shared<Expression_v2>(e1));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("LUT_DEPTH"), 9},
        {qualified_identifier("STEP"), 0.17453292519943295f}
    };

    ASSERT_EQ(9, defaults.at(qualified_identifier("LUT_DEPTH")).get_integer());
    ASSERT_FLOAT_EQ(0.17453292519943295,defaults.at(qualified_identifier("STEP")).get_real());
}

TEST(parameter_extraction, package_parameters) {
    auto test_pattern = R"(

        package test_package;


            parameter bus_base = 32'h43c00000;
            parameter timebase = bus_base;
            parameter gpio = timebase + 32'h1000 * 2 / 2 + 1;
            parameter modulo_parameter = 3 % 2;
            parameter subtraction_parameter = 'o4 - 'b10;

        endpackage

    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("bus_base");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("32'h43c00000"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("timebase");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("bus_base")));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("gpio");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e, e1, e2, e3;
    e3.set_lhs(std::make_shared<Numeric_token>("32'h1000"));
    e3.set_rhs(std::make_shared<Numeric_token>("2"));
    e3.set_operation(Expression_v2::multiply);
    e2.set_lhs(std::make_shared<Expression_v2>(e3));
    e2.set_rhs(std::make_shared<Numeric_token>("2"));
    e2.set_operation(Expression_v2::divide);
    e1.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("timebase")));
    e1.set_rhs(std::make_shared<Expression_v2>(e2));
    e1.set_operation(Expression_v2::add);
    e.set_lhs(std::make_shared<Expression_v2>(e1));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("modulo_parameter");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Numeric_token>("3"));
    e.set_rhs(std::make_shared<Numeric_token>("2"));
    e.set_operation(Expression_v2::expression_operator::modulo);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("subtraction_parameter");
    p->set_type(Type_engine::create_primitive_type("implicit"));

    e.set_lhs(std::make_shared<Numeric_token>("'o4"));
    e.set_rhs(std::make_shared<Numeric_token>("'b10"));
    e.set_operation(Expression_v2::expression_operator::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }



    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("bus_base"), 0x43c00000},
        {qualified_identifier("timebase"), 0x43c00000},
        {qualified_identifier("gpio"), 0x43c01001},
        {qualified_identifier("modulo_parameter"), 1},
        {qualified_identifier("subtraction_parameter"), 2}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, simple_parameters) {
    auto test_pattern = R"(
        module test_mod #(
            parameter simple_numeric_p = 32,
            sv_numeric_p = 5'o10,
            dimensionless_sv_numeric_p  = 'h3F,
            string_p = "423",
            nested_p = string_p
        )();
        localparam local_p = 74;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("simple_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("32"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("local_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("74"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("5'o10"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("dimensionless_sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("'h3F"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("string_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<String_token>("\"423\""));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("nested_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("string_p")));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("simple_numeric_p"), 32},
        {qualified_identifier("local_p"), 74},
        {qualified_identifier("sv_numeric_p"), 8},
        {qualified_identifier("dimensionless_sv_numeric_p"), 63},
        {qualified_identifier("string_p"), R"("423")"s},
        {qualified_identifier("nested_p"), R"("423")"s}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, simple_expressions) {
    auto test_pattern = R"(
        module test_mod #(
            parameter simple_numeric_p = 32,
            sv_numeric_p = 5'o10,
            dimensionless_sv_numeric_p  = 'h3F
        )();

            parameter add_expr_p = simple_numeric_p + sv_numeric_p;
            parameter sub_expr_p = simple_numeric_p - sv_numeric_p;
            parameter mul_expr_p = simple_numeric_p * sv_numeric_p;
            parameter div_expr_p = simple_numeric_p / sv_numeric_p;
            parameter modulo_expr_p = simple_numeric_p % sv_numeric_p;
            parameter chained_expression = add_expr_p + mul_expr_p  * 5;
            parameter complex_log_expr_p = $clog2(add_expr_p+2);
            parameter simple_log_expr_p = $clog2(add_expr_p);
            parameter parenthesised_expr_p = ( add_expr_p + mul_expr_p ) * 5;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("simple_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("32"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("5'o10"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("dimensionless_sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("'h3F"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("add_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("simple_numeric_p")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("sv_numeric_p")));
    e.set_operation(Expression_v2::expression_operator::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("sub_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("simple_numeric_p")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("sv_numeric_p")));
    e.set_operation(Expression_v2::expression_operator::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("mul_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("simple_numeric_p")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("sv_numeric_p")));
    e.set_operation(Expression_v2::expression_operator::multiply);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("div_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("simple_numeric_p")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("sv_numeric_p")));
    e.set_operation(Expression_v2::expression_operator::divide);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("modulo_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("simple_numeric_p")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("sv_numeric_p")));
    e.set_operation(Expression_v2::expression_operator::modulo);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("chained_expression");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("add_expr_p")));
    Expression_v2 e2;
    e2.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("mul_expr_p")));
    e2.set_rhs(std::make_shared<Numeric_token>("5"));
    e2.set_operation(Expression_v2::expression_operator::multiply);
    e.set_rhs(std::make_shared<Expression_v2>(e2));
    e.set_operation(Expression_v2::expression_operator::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("complex_log_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    auto call = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::clog2);
    e = Expression_v2();
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("add_expr_p")));
    e.set_rhs(std::make_shared<Numeric_token>("2"));
    e.set_operation(Expression_v2::expression_operator::add);
    call->add_argument(std::make_shared<Expression_v2>(e));

    p->set_raw_value(call);

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("simple_log_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    call = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::clog2);
    call->add_argument(std::make_shared<Identifier_token>(qualified_identifier("add_expr_p")));
    p->set_raw_value(call);

    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("parenthesised_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));

    e2 = Expression_v2();
    e2.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("add_expr_p")));
    e2.set_operation(Expression_v2::expression_operator::add);
    e2.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("mul_expr_p")));
    e = Expression_v2();
    e.set_rhs(std::make_shared<Numeric_token>("5"));
    e.set_lhs(std::make_shared<Expression_v2>(e2));
    e.set_operation(Expression_v2::expression_operator::multiply);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier( "simple_numeric_p"), 32},
        {qualified_identifier("sv_numeric_p"), 8},
        {qualified_identifier("dimensionless_sv_numeric_p"), 63},
        {qualified_identifier("simple_log_expr_p"),6},
        {qualified_identifier("add_expr_p"), 40},
        {qualified_identifier("sub_expr_p"), 24},
        {qualified_identifier("mul_expr_p"), 256},
        {qualified_identifier("div_expr_p"), 4},
        {qualified_identifier("chained_expression"), 1320},
        {qualified_identifier("modulo_expr_p"), 0},
        {qualified_identifier("complex_log_expr_p"), 6},
        {qualified_identifier("parenthesised_expr_p"), 1480}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}



TEST(parameter_extraction, bitwise_expressions) {
    auto test_pattern = R"(
        module test_mod #(
            parameter op_a = 9,
            op_b = 12
        )();

            parameter b_and_p = op_a & op_b;
            parameter b_or_p = op_a | op_b;
            parameter b_xor_p =  op_a ^ op_b;
            parameter b_xnor_p = op_a ~^ op_b;
            parameter b_xnor2_p = op_a ^~ op_b;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("9"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("12"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_and_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::bitwise_and);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_or_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::bitwise_or);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_xor_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::bitwise_xor);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_xnor_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::bitwise_xnor);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_xnor2_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::bitwise_xnor);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("op_a"), 9},
        {qualified_identifier("op_b"), 12},
        {qualified_identifier("b_and_p"), 8},
        {qualified_identifier("b_or_p"),13},
        {qualified_identifier("b_xor_p"), 5},
        {qualified_identifier("b_xnor_p"), -6},
        {qualified_identifier("b_xnor2_p"), -6}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        EXPECT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, power_expression) {
    auto test_pattern = R"(
        module test_mod #(
            parameter op_a = 2,
            op_b = 5
        )();

            parameter pow_expr = op_a ** op_b;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("2"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("5"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("pow_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::power);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("op_a"), 2},
        {qualified_identifier("op_b"), 5},
        {qualified_identifier("pow_expr"),32},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, arithmetic_shift_left) {
    auto test_pattern = R"(
        module test_mod #(
            parameter op_a = 3,
            op_b = 2
        )();
            parameter shl_expr = op_a <<< op_b;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("3"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("2"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("shl_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::arithmetic_shift_left);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    // 3 <<< 2 = 12
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("op_a"), 3},
        {qualified_identifier("op_b"), 2},
        {qualified_identifier("shl_expr"),12},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, arithmetic_shift_right) {
    auto test_pattern = R"(
        module test_mod #(
            parameter op_a = -8,
            op_b = 2
        )();
            parameter shr_a_expr = op_a >>> op_b;
            parameter shr_l_expr = op_a >> op_b;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("8"));
    e.set_operation(Expression_v2::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("2"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("shr_a_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::arithmetic_shift_right);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("shr_l_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::logic_shift_right);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("op_a"), -8},
        {qualified_identifier("op_b"), 2},
        {qualified_identifier("shr_a_expr"), -2},
        {qualified_identifier("shr_l_expr"), 1073741822},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, logical_and_or) {
    auto test_pattern = R"(
        module test_mod #(
            parameter op_a = 3,
            op_b = 0,
            op_c = 5
        )();
            parameter log_and_expr = op_a && op_b;
            parameter log_or_expr = op_a || op_b;
            parameter mixed_expr = op_a && op_b || op_c;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("3"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("0"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_c");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("5"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("log_and_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::logical_and);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("log_or_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::logical_or);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("mixed_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e2;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("op_a")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_b")));
    e.set_operation(Expression_v2::expression_operator::logical_and);
    e2.set_lhs(std::make_shared<Expression_v2>(e));
    e2.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("op_c")));
    e2.set_operation(Expression_v2::expression_operator::logical_or);
    p->set_raw_value(std::make_shared<Expression_v2>(e2));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("op_a"), 3},
        {qualified_identifier("op_b"), 0},
        {qualified_identifier("op_c"), 5},
        {qualified_identifier("log_and_expr"), 0},
        {qualified_identifier("log_or_expr"), 1},
        {qualified_identifier("mixed_expr"), 1},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, package_parameters_use) {
    auto test_pattern = R"(

        package test_package;
            parameter bus_base = 67;
        endpackage

        module test_mod #(
             parameter package_param = test_package::bus_base
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto resource = resources[1]->as<hdl_resource_statement>();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("package_param");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("test_package", "bus_base")));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> pkg_ctx;
    for (auto &[id, val] : pkg_defaults) {
        pkg_ctx[qualified_identifier("test_package", id.get_name())] = val;
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), pkg_ctx);
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("package_param"), 67}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, interface_parameter_use) {
    auto test_pattern = R"(


    interface test_if #(DATA_WIDTH = 32);
    endinterface

    module test_mod #(
         parameter package_param = test_package::bus_base
    )();
        test_if test_interface();
        dependency_dep #(.TEST_PARAM(test_interface.DATA_WIDTH)) dep();
    endmodule
    )";



    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(resources[1]->as<hdl_resource_statement>().get_statements()[2]);
    auto parameters = stmt->get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("TEST_PARAM");
    p->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("test_interface", "DATA_WIDTH")));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

}

TEST(parameter_extraction, negative_number_parameters) {
    auto test_pattern = R"(
        module test_mod #(
             parameter negative_param = -16'sd32767
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("negative_param");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("16'sd32767"));
    e.set_operation(Expression_v2::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {8, 32});

    hdl_integer v;
    v.set_value( -32767);
    v.set_signed(true);
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("negative_param"), v}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, instance_parameter) {
    auto test_pattern = R"(
    module test_mod #(
        parameter test_param = 4
    )();

    module_type #(
        .param_1(test_param),
        .param_2(test_param+5),
        .param_3((test_param + 7)*1)
    ) test_instance ();

    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto def_parameters = resource.get_parameters();

    auto inst_parameters = std::dynamic_pointer_cast<hdl_instance_statement>(resource.get_statements()[0])->get_parameters();

    std::vector<std::pair<std::string, std::vector<std::string>>> vect_params = {
            {"test_param", {"4"}}
    };

    Parameters_map check_params;
    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_param");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("4"));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), def_parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(def_parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *def_parameters.get(item->get_name()));
    }

    check_params.clear();

    p = std::make_shared<HDL_parameter>();

    p->set_name("param_1");
    p->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("test_param")));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();

    p->set_name("param_2");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("test_param")));
    e.set_rhs(std::make_shared<Numeric_token>("5"));
    e.set_operation(Expression_v2::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();

    p->set_name("param_3");
    Expression_v2 e1, e2;
    e1.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("test_param")));
    e1.set_rhs(std::make_shared<Numeric_token>("7"));
    e1.set_operation(Expression_v2::add);
    e2.set_lhs(std::make_shared<Expression_v2>(e1));
    e2.set_rhs(std::make_shared<Numeric_token>("1"));
    e2.set_operation(Expression_v2::multiply);
    p->set_raw_value(std::make_shared<Expression_v2>(e2));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), inst_parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(inst_parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *inst_parameters.get(item->get_name()));
    }

}


TEST(parameter_extraction, unrelated_wire_dependency_conflict) {
    auto test_pattern = R"(
    module test_mod #(
        DECIMATE = 1
    )();
    assign unrelated_wire = {5{1}};

        dependency #(
            .DECIMATED(DECIMATE)
        ) dep();
    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(resource.get_statements()[0]);
    auto parameter = inst->get_parameters().const_get("DECIMATED");

    auto check_param = std::make_shared<HDL_parameter>();
    check_param->set_name("DECIMATED");
    check_param->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("DECIMATE")));

    ASSERT_EQ(*parameter, *check_param);
}


TEST(parameter_extraction, interface_parameters) {
    auto test_pattern = R"(
        interface axi_stream #(DATA_WIDTH = 32, USER_WIDTH = 24,  DEST_WIDTH = 8);
        endinterface
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("DATA_WIDTH");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("32"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("USER_WIDTH");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("24"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("DEST_WIDTH");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("8"));
    check_params.insert(p);

    ASSERT_EQ(check_params, parameters);
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {8, 32});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("DATA_WIDTH"), 32},
        {qualified_identifier("USER_WIDTH"), 24},
        {qualified_identifier("DEST_WIDTH"), 8}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, generate_for) {
    auto test_pattern = R"(
    module test_mod #(
        N_REPETITION = 2
    )();

        parameter [31:0] ARRAY_PARAM [1:0] = '{10, 440};

        genvar n;

        generate
            for(n = 0; n<N_REPETITIONS; n=n+1)begin
                dependency #(
                    .DEP_PARAM(ARRAY_PARAM[n])
                ) dep();
            end
        endgenerate

    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto loop_stmt = std::dynamic_pointer_cast<hdl_loop_statement>(resource.get_statements()[0]);
    ASSERT_NE(loop_stmt, nullptr);

    hdl_loop_statement check_loop;

    HDL_parameter p;
    p.set_name("n");
    p.set_raw_value(std::make_shared<Numeric_token>("0"));

    check_loop.set_init(std::make_shared<HDL_parameter>(p));

    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("n")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("N_REPETITIONS")));
    e.set_operation(Expression_v2::less);
    check_loop.set_end_condition(std::make_shared<Expression_v2>(e));

    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("n")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::add);
    check_loop.set_iteration(std::make_shared<Expression_v2>(e));

    auto dep_inst = std::make_shared<hdl_instance_statement>();
    dep_inst->set_name("dep");
    dep_inst->set_type("dependency");
    dep_inst->set_dependency_class(module);
    auto dep_param = std::make_shared<HDL_parameter>();
    dep_param->set_name("DEP_PARAM");
    Identifier_token arr_idx(qualified_identifier("ARRAY_PARAM"));
    arr_idx.add_array_index(std::make_shared<Identifier_token>(qualified_identifier("n")));
    dep_param->set_raw_value(std::make_shared<Identifier_token>(arr_idx));
    dep_inst->add_parameter(dep_param);
    check_loop.add_body_stmt(dep_inst);

    ASSERT_EQ(*loop_stmt, check_loop);
}





TEST(parameter_extraction, param_ternary_conditional) {
    auto test_pattern = R"(
        module test_mod #(
            parameter condition = 2,
            parameter test_positive = condition > 1 ? 12 : 34,
            parameter test_negative = condition > 65 ? 12 : 34
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("condition");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("2"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("test_positive");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Ternary t;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("condition")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::greater);
    t.set_condition(std::make_shared<Expression_v2>(e));
    t.set_true_value(
        std::make_shared<Numeric_token>("12"));

    t.set_false_value(
        std::make_shared<Numeric_token>("34"));
    p->set_raw_value(std::make_shared<Ternary>(t));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("test_negative");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    t = Ternary();
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("condition")));
    e.set_rhs(std::make_shared<Numeric_token>("65"));
    e.set_operation(Expression_v2::greater);
    t.set_condition(std::make_shared<Expression_v2>(e));
    t.set_true_value(std::make_shared<Numeric_token>("12"));

    t.set_false_value(std::make_shared<Numeric_token>("34"));
    p->set_raw_value(std::make_shared<Ternary>(t));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("condition"), 2},
        {qualified_identifier("test_positive"), 12},
        {qualified_identifier("test_negative"), 34},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, nested_ternary_conditional) {
    auto test_pattern = R"(
        module test_mod #(
            parameter condition = 2,
            parameter test_positive = condition > 1 ?  condition > 65 ? 12 : 96 : 34
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("condition");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("2"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("test_positive");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Ternary t;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("condition")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::greater);
    t.set_condition(std::make_shared<Expression_v2>(e));
    t.set_false_value(std::make_shared<Numeric_token>("34"));

    Ternary inner_t;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("condition")));
    e.set_rhs(std::make_shared<Numeric_token>("65"));
    e.set_operation(Expression_v2::greater);
    inner_t.set_condition(std::make_shared<Expression_v2>(e));

    inner_t.set_true_value(std::make_shared<Numeric_token>("12"));

    inner_t.set_false_value(std::make_shared<Numeric_token>("96"));
    t.set_true_value(std::make_shared<Ternary>(inner_t));
    p->set_raw_value(std::make_shared<Ternary>(t));
    check_params.insert(p);



    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("condition"), 2},
        {qualified_identifier("test_positive"), 96},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, complex_ternary_conditional) {
    auto test_pattern = R"(
        module test_mod #(
            parameter NM = 4
        )();
            localparam	LGNM = NM>1 ? $clog2(NM) : 1;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("NM");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("4"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("LGNM");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Ternary t;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("NM")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::greater);
    t.set_condition(std::make_shared<Expression_v2>(e));
    auto c = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::clog2);
    c->add_argument(std::make_shared<Identifier_token>(qualified_identifier("NM")));
    t.set_true_value(c);
    t.set_false_value(std::make_shared<Numeric_token>("1"));

    p->set_raw_value(std::make_shared<Ternary>(t));
    check_params.insert(p);



    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("NM"), 4},
        {qualified_identifier("LGNM"), 2},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}



TEST(parameter_extraction, typedef_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef logic [31:0] ctrl_addr_init_t [1:0];
            parameter ctrl_addr_init_t array_parameter = '{32, 5};
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();


    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {5, 32});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("array_parameter"), array_value}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}




TEST(parameter_processing, typedef_in_package_parameter) {

    auto test_pattern = R"(
        package test_pkg;
            typedef logic [31:0] test_type [15:0];
        endpackage

        module test_mod #()();
            parameter test_pkg::test_type array_parameter = '{32, 5};
        endmodule
    )";

    sv_analyzer analyzer;
    auto file =  analyzer.analyze("", test_pattern).value();
    auto resources= file.get_content();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto module = std::static_pointer_cast<hdl_resource_statement>(resources[1]);
    auto param = module->get_parameters().get("array_parameter");
    HDL_parameter p;
    p.set_name("array_parameter");
    p.set_type(std::make_shared<HDL_external_type>(qualified_identifier("test_pkg", "test_type")));
    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("32"));
    c.add_component(std::make_shared<Numeric_token>("5"));
    p.set_raw_value(std::make_shared<Concatenation>(c));
    ASSERT_EQ(p, *param);

    parameter_solver::propagate_types(module, d_store);
    auto defaults = parameter_solver::process_parameters(module->get_parameters(), {});

    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {5, 32});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("array_parameter"), array_value}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }

}



TEST(parameter_extraction, wide_int_parameter) {
    auto test_pattern = R"(
        module test_mod #(
            )();
            parameter [72:0] TEST_PARAM = 72'hCAFEBEBEDEADBEEFCAFE + 5;

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");

    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("72'hCAFEBEBEDEADBEEFCAFE"));
    e.set_rhs(std::make_shared<Numeric_token>("5"));
    e.set_operation(Expression_v2::add);
    auto param_type = HDL_simple_type();
    param_type.add_dimension({
          std::make_shared<Numeric_token>("72"),
          std::make_shared<Numeric_token>("0"),
          true
      });
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    auto param = defaults.at(qualified_identifier("TEST_PARAM"));
    // Self-determined result width is max(72, 32) = 72 bits, so the 80-bit sum
    // is truncated to 72 bits. Expected value written as a Verilog literal.
    Numeric_token expected("72'hFEBEBEDEADBEEFCB03");
    ASSERT_EQ(expected.get_value().value().get_integer(), param.get_integer());
}



TEST(parameter_extraction, wide_integer_subtraction) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [127:0] BIG = 128'hFFFFFFFFFFFFFFFF0000000000000003;
            parameter SUB = BIG - 1;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    auto big_val = defaults.at(qualified_identifier("BIG")).get_integer().to_wide();
    auto sub_val = defaults.at(qualified_identifier("SUB")).get_integer().to_wide();
    EXPECT_EQ(sub_val, big_val - 1);
}

TEST(parameter_extraction, wide_integer_bitwise_and) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [127:0] A = 128'hFFFFFFFFFFFFFFFF000000000000000F;
            parameter [127:0] B = 128'hAAAAAAAAAAAAAAAA5555555555555555;
            parameter AND_RES = A & B;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    auto and_val = defaults.at(qualified_identifier("AND_RES")).get_integer().to_wide();
    auto expected = wide_integer("0xAAAAAAAAAAAAAAAA0000000000000005");
    EXPECT_EQ(and_val, expected);
}

TEST(parameter_extraction, wide_integer_shift_left) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [127:0] SRC = 128'h00000000000000000000000000000001;
            parameter SHIFTED = SRC << 64;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    auto shifted_val = defaults.at(qualified_identifier("SHIFTED")).get_integer().to_wide();
    EXPECT_EQ(shifted_val, wide_integer("0x00000000000000010000000000000000"));
}

TEST(parameter_extraction, wide_integer_mixed_arithmetic) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [127:0] BIG = 128'hFFFFFFFFFFFFFFFF0000000000000000;
            parameter MULT = BIG * 2;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    auto mult_val = defaults.at(qualified_identifier("MULT")).get_integer().to_wide();
    // BIG is 128-bit, 2 is unsized (32-bit); the self-determined result width is
    // 128 bits, so the 129-bit product is truncated to the low 128 bits. Expected
    // value written as a Verilog literal.
    Numeric_token expected_tok("128'hFFFFFFFFFFFFFFFE0000000000000000");
    auto expected = expected_tok.get_value().value().get_integer().to_wide();
    EXPECT_EQ(mult_val, expected);
}

TEST(parameter_extraction, streaming_operators) {
    auto test_pattern = R"(
        module test_mod ();
            parameter logic [31:0] BASE_ADDR_LE = 32'h1234_5678;
            localparam logic [31:0] BASE_ADDR_BE = {<< 8 {BASE_ADDR_LE}};
            localparam logic [31:0] REVERSED_MASK = {<< {32'h0000_00FF}};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});


    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("BASE_ADDR_LE"), 0x12345678},
        {qualified_identifier("BASE_ADDR_BE"), 0x78563412},
        {qualified_identifier("REVERSED_MASK"), hdl_integer(0xff000000)}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }

    int i = 0;
}

TEST(parameter_extraction, streaming_bit_reversal) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] SRC = 8'hAB;
            parameter X = {<<{SRC}};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer(), 0xD5);
}

TEST(parameter_extraction, unary_logical_not) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [3:0] V = 4'b0011;
            parameter X = !V;
            parameter Y = ~V;
            parameter Z = !0;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("Y")).get_integer(), 12);
    EXPECT_EQ(defaults.at(qualified_identifier("Z")).get_integer(), 1);
}

TEST(parameter_extraction, reduction_operators) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [3:0] V = 4'b1011;
            parameter A = &V;
            parameter O = |V;
            parameter X = ^V;
            parameter NA = ~&V;
            parameter NO = ~|V;
            parameter NX = ~^V;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("A")).get_integer(), 0);   // 1011: not all ones
    EXPECT_EQ(defaults.at(qualified_identifier("O")).get_integer(), 1);   // 1011: any bit set
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer(), 1);   // 1011: odd parity (3 ones)
    EXPECT_EQ(defaults.at(qualified_identifier("NA")).get_integer(), 1);  // NAND
    EXPECT_EQ(defaults.at(qualified_identifier("NO")).get_integer(), 0);  // NOR
    EXPECT_EQ(defaults.at(qualified_identifier("NX")).get_integer(), 0);  // XNOR
}

TEST(parameter_extraction, int_real_equality) {
    auto test_pattern = R"(
        module test_mod ();
            parameter int I = 1;
            parameter E1 = (I == 1.0);
            parameter E2 = (I != 2.0);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("E1")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("E2")).get_integer(), 1);
}


TEST(parameter_extraction, wide_size_cast) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [127:0] V = 128'hFEDCBA9876543210FEDCBA9876543210;
            parameter X = 63'(V);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    auto x = defaults.at(qualified_identifier("X")).get_integer();
    EXPECT_EQ(x.get_value(), static_cast<int64_t>(0x7EDCBA9876543210));
}


TEST(parameter_extraction, borderline_wide_size_cast) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [127:0] V = 128'hFEDCBA9876543210FEDCBA9876543210;
            parameter X = 64'(V);
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    auto x = defaults.at(qualified_identifier("X")).get_integer();
    EXPECT_TRUE(x.is_wide());
    EXPECT_EQ(x.to_wide(), wide_integer("0xFEDCBA9876543210"));
}

TEST(parameter_extraction, wide_packed_concatenation) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [63:0] HI = 64'hCAFEBABECAFEBABE;
            parameter [63:0] LO = 64'hDEADBEEFDEADBEEF;
            parameter [127:0] C = {HI, LO};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    auto c = defaults.at(qualified_identifier("C")).get_integer();
    EXPECT_EQ(c.to_wide(), wide_integer("0xCAFEBABECAFEBABEDEADBEEFDEADBEEF"));
}

TEST(parameter_extraction, wide_streaming_byte_reversal) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [127:0] V = 128'h0123456789ABCDEF0123456789ABCDEF;
            parameter X = {<<8{V}};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    auto x = defaults.at(qualified_identifier("X")).get_integer();
    EXPECT_EQ(x.to_wide(), wide_integer("0xEFCDAB8967452301EFCDAB8967452301"));
}

TEST(parameter_extraction, wide_replication) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [63:0] V = 64'hDEADBEEFCAFEBABE;
            parameter [127:0] R = {2{V}};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    auto r = defaults.at(qualified_identifier("R")).get_integer();
    EXPECT_EQ(r.to_wide(), wide_integer("0xDEADBEEFCAFEBABEDEADBEEFCAFEBABE"));
}

TEST(parameter_extraction, wide_bit_select) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [127:0] V = 128'h00000000000000020000000000000000;
            parameter W = V;
            parameter B65 = V[65];
            parameter B64 = V[64];
            parameter B63 = V[63];
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    auto w = defaults.at(qualified_identifier("W")).get_integer();
    ASSERT_TRUE(w.is_wide());
    EXPECT_EQ(w.to_wide(), wide_integer(1) << 65);
    EXPECT_EQ(defaults.at(qualified_identifier("B65")).get_integer().get_value(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("B64")).get_integer().get_value(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("B63")).get_integer().get_value(), 0);
}

TEST(parameter_extraction, wide_logic_shift_right) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [127:0] V = 128'h10000000000000000000000000000000;
            parameter X = V >> 64;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    auto x = defaults.at(qualified_identifier("X")).get_integer();
    EXPECT_EQ(x.to_wide(), wide_integer(0x1000000000000000));
}



TEST(parameter_extraction, streaming_byte_reversal) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [31:0] SRC = 32'h44332211;
            parameter X = {<<8{SRC}};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer(), 0x11223344);
}

TEST(parameter_extraction, streaming_right_shift_no_reorder) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [31:0] SRC = 32'h44332211;
            parameter X = {>>8{SRC}};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer().get_value(), 0x44332211);
}

TEST(parameter_extraction, streaming_ieee_examples) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [5:0] SRC = 6'b110101;
            parameter X = {<<4{SRC}};
            parameter Y = {>>4{SRC}};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer().get_value(), 0b010111);
    EXPECT_EQ(defaults.at(qualified_identifier("Y")).get_integer().get_value(), 0b110101);
}

TEST(parameter_extraction, streaming_multi_component) {
    auto test_pattern = R"(
        module test_mod ();
            parameter [7:0] A = 8'h12;
            parameter [7:0] B = 8'h34;
            parameter X = {<<8{A, B}};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer(), 0x3412);
}

TEST(parameter_extraction, streaming_slice_size_expression) {
    auto test_pattern = R"(
        module test_mod ();
            parameter N = 8;
            parameter [31:0] SRC = 32'h44332211;
            parameter X = {<<N{SRC}};
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer(), 0x11223344);
}


TEST(parameter_extraction, unary_minus_in_comparison) {
    // Counter-example: -8'sd1 == 8'hFF is true (both 8-bit: -1 is 0xFF), so the
    // ternary must yield 1. The frontend's expression factory drops the unary
    // minus when a binary operator follows, building 1 == 255 -> false -> 0.
    auto test_pattern = R"(
        module test_mod ();
            parameter f = -8'sd1 == 8'hFF ? 1 : 0;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("f")).get_integer(), 1);
}

TEST(parameter_extraction, case_wildcard_equality) {
    auto test_pattern = R"(
        module test_mod ();
            parameter A = 4'hA === 4'hA ? 1 : 0;
            parameter B = 4'hA !== 4'hB ? 1 : 0;
            parameter C = 4'hA ==? 4'hA ? 1 : 0;
            parameter D = 4'hA !=? 4'hF ? 1 : 0;
            parameter E = 4'hA === 4'hB ? 1 : 0;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("A")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("C")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("D")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("E")).get_integer(), 0);
}

TEST(parameter_extraction, result_width_truncation) {
    auto test_pattern = R"(
        module test_mod ();
            parameter A = 4'hF + 4'h1;   // 16, truncated to 4 bits -> 0
            parameter B = 8'hFF + 8'h01; // 256, truncated to 8 bits -> 0
            parameter C = 4'hA * 4'h4;   // 40, truncated to 4 bits -> 8
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("A")).get_integer(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("B")).get_integer(), 0);
    EXPECT_EQ(defaults.at(qualified_identifier("C")).get_integer(), 8);
}