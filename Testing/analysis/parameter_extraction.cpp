//  Copyright 2023 Filippo Savi
//  Author: Filippo Savi <filssavi@gmap->com>
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

#include "frontend/analysis/sv_analyzer.hpp"
#include "frontend/analysis/type_engine.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "analysis/parameter_solver.hpp"
#include "data_model/HDL/parameters/components/Replication.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/parameters/components/Cast.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/Ternary.hpp"

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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("low_control_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("'b001111", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("low_control_n");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("'b110000", Token::number));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {{"","", "low_control_p"}, 15},
        {{"","", "low_control_n"}, 48}
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    Cast c;
    c.set_size(std::make_shared<Token>("4", Token::number));
    c.set_content(std::make_shared<Token>("31'h100003", Token::number));
    p->set_raw_value(std::make_shared<Cast>(c));
    p->set_type(Type_engine::create_primitive_type("integer"));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(3, defaults.at({"","", "TEST_PARAM"}).get_integer());
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("SIZE");
    p->set_raw_value(std::make_shared<Token>(4, 2));
    p->set_type(Type_engine::create_primitive_type("integer"));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");

    Cast c;
    c.set_size(std::make_shared<Token>("SIZE", Token::identifier));
    c.set_content(std::make_shared<Token>(Token("31'h100003", Token::number)));
    p->set_raw_value(std::make_shared<Cast>(c));
    p->set_type(Type_engine::create_primitive_type("integer"));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(3, defaults.at({"","", "TEST_PARAM"}).get_integer());
}


TEST(parameter_extraction, type_cast) {
    auto test_pattern = R"(
        module test_mod #()();

            parameter reg [7:0] TEST_PARAM = unsigned'(-5);

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");

    Cast c;
    c.set_type_cast();
    c.set_target_type("unsigned");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("5", Token::number));
    e.set_operation(Expression_v2::subtract);
    c.set_content(std::make_shared<Expression_v2>(e));
    dimension_t d;
    d.first_bound = std::make_shared<Token>("7", Token::number);
    d.second_bound =  std::make_shared<Token>("0", Token::number);
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

    ASSERT_EQ(251, defaults.at({"","", "TEST_PARAM"}).get_integer());
}


TEST(parameter_extraction, nested_type_cast) {
    auto test_pattern = R"(
        module test_mod #()();
            localparam NumLevels = 4;
            parameter reg [7:0] TEST_PARAM = (NumLevels)'(unsigned'(-5));

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;



    Cast inner_c;
    inner_c.set_type_cast();
    inner_c.set_target_type("unsigned");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("5", Token::number));
    e.set_operation(Expression_v2::subtract);
    inner_c.set_content(std::make_shared<Expression_v2>(e));

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");

    Cast outer_c;
    outer_c.set_size(std::make_shared<Token>("NumLevels", Token::identifier));
    outer_c.set_content(std::make_shared<Cast>(inner_c));
    dimension_t d;
    d.first_bound = std::make_shared<Token>("7", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Cast>(outer_c));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("NumLevels");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>(4, 2));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(11, defaults.at({"","", "TEST_PARAM"}).get_integer());
}


TEST(parameter_extraction, multiple_type_cast) {
    auto test_pattern = R"(
        module test_mod #()();

            parameter reg [7:0] TEST_PARAM = unsigned'(-5);
            parameter TEST_PARAM_2 = int'(2.5);

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    Cast c;
    c.set_type_cast();
    c.set_target_type("unsigned");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("5", Token::number));
    e.set_operation(Expression_v2::subtract);
    c.set_content(std::make_shared<Expression_v2>(e));
    dimension_t d;
    d.first_bound = std::make_shared<Token>("7", Token::number);
    d.second_bound =  std::make_shared<Token>("0", Token::number);
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
    c.set_content(std::make_shared<Token>("2.5", Token::number));
    p->set_raw_value(std::make_shared<Cast>(c));

    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ(251, defaults.at({"","", "TEST_PARAM"}).get_integer());
    ASSERT_EQ(3, defaults.at({"","", "TEST_PARAM_2"}).get_integer());
}

TEST(parameter_extraction, cast_in_binary_expression) {
    auto test_pattern = R"(
        module test_mod #()();
            parameter integer x = -1;
            parameter integer y = 'hfffffffa;
            parameter integer A = unsigned'(x) + 1;
            parameter integer B = signed'(y) - 2;

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("x");
    p->set_type(Type_engine::create_primitive_type("integer"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
    p->set_name("y");
    p->set_type(Type_engine::create_primitive_type("integer"));
    p->set_raw_value(std::make_shared<Token>(4294967290, 32));
    check_params.insert(p);


    auto cast_a = std::make_shared<Cast>();
    cast_a->set_type_cast();
    cast_a->set_target_type("unsigned");
    cast_a->set_content(std::make_shared<Token>("x", Token::identifier));
    e.set_lhs(cast_a);
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::add);

    p = std::make_shared<HDL_parameter>();
    p->set_name("A");
    p->set_type(Type_engine::create_primitive_type("integer"));
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    auto cast_b = std::make_shared<Cast>();
    cast_b->set_type_cast();
    cast_b->set_target_type("signed");
    cast_b->set_content(std::make_shared<Token>("y", Token::identifier));
    e.set_lhs(cast_b);
    e.set_rhs(std::make_shared<Token>("2", Token::number));
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
        {{"","", "x"}, -1},
        {{"","", "y"}, static_cast<uint64_t>(4294967290)},
        {{"","", "A"}, static_cast<uint64_t>(4294967294)},
        {{"","", "B"}, -8},
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

            parameter integer TEST_PARAM = 10ns;

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    p->set_raw_value(std::make_shared<Token>("10ns", Token::string));
    p->set_type(Type_engine::create_primitive_type("integer"));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    ASSERT_EQ("10ns", defaults.at({"","", "TEST_PARAM"}).get_string());
}

TEST(parameter_extraction, cast_in_concat) {
    auto test_pattern = R"(
        module test_mod #(
            )();

            parameter integer TEST_PARAM = { 10'h0, 1'b1, 1'b0, 4'(31'h100003)};

        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");

    Concatenation concat;
    concat.add_component(std::make_shared<Token>("10'h0", Token::number));
    concat.add_component(std::make_shared<Token>("1'h1", Token::number));
    concat.add_component(std::make_shared<Token>("1'h0", Token::number));
    Cast c;
    c.set_size(std::make_shared<Token>("4", Token::number));
    c.set_content(std::make_shared<Token>("31'h100003", Token::number));
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

    ASSERT_EQ(35, defaults.at({"","", "TEST_PARAM"}).get_integer());
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("N_CORES");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("3", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TRANSLATION_TABLE_INIT");

    auto param_type = HDL_simple_type();
    param_type.add_dimension({
        std::make_shared<Token>("3", Token::number),
        std::make_shared<Token>("0", Token::number),
        false
    });
    Concatenation c;
    c.set_default_init();
    c.add_component(std::make_shared<Token>("\"FILE\"", Token::string));
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

    ASSERT_EQ(3, defaults.at({"","", "N_CORES"}).get_integer());
    ASSERT_EQ(tti_val, defaults.at({"","", "TRANSLATION_TABLE_INIT"}).get_string_array());
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("N_CORES");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("3", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TRANSLATION_TABLE_INIT");

    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("3", Token::number), std::make_shared<Token>("0", Token::number), false});
    Concatenation c;
    c.set_default_init();
    c.add_component(std::make_shared<Token>("\"FILE\"", Token::string));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));

    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("SEL");
    Token t = Token("TRANSLATION_TABLE_INIT", Token::identifier);
    std::vector<std::shared_ptr<Parameter_value_base>> ai;
    ai.push_back(std::make_shared<Token>("2", Token::number));
    t.set_array_index(ai);
    p->set_raw_value(std::make_shared<Token>(t));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<std::string> tti_val;
    tti_val.set_1d_slice({0, 0}, {"\"FILE\"", "\"FILE\"", "\"FILE\"", "\"FILE\""});

    ASSERT_EQ(3, defaults.at({"","", "N_CORES"}).get_integer());
    ASSERT_EQ(tti_val, defaults.at({"","", "TRANSLATION_TABLE_INIT"}).get_string_array());
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("N_CORES");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("3", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TRANSLATION_TABLE_INIT");

    auto param_type = HDL_simple_type();
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("N_CORES", Token::identifier));
    e.set_operation(Expression_v2::subtract);
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    param_type.add_dimension({
        std::make_shared<Expression_v2>(e),
        std::make_shared<Token>("0", Token::number),
        false
    });
    Concatenation c;
    c.add_component(std::make_shared<Token>("\"FILE\"", Token::string));
    c.add_component(std::make_shared<Token>("\"FILE\"", Token::string));
    c.add_component(std::make_shared<Token>("\"FILE\"", Token::string));
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

    ASSERT_EQ(3,defaults.at({"","", "N_CORES"}).get_integer());
    ASSERT_EQ(tti_val, defaults.at({"","", "TRANSLATION_TABLE_INIT"}).get_string_array());
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("LUT_DEPTH");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("9", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("STEP");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e1, e2, e3;
    e3.set_lhs(std::make_shared<Token>("2", Token::number));
    e3.set_rhs(std::make_shared<Token>("3.14159265358979323846", Token::number));
    e3.set_operation(Expression_v2::multiply);
    e2.set_lhs(std::make_shared<Expression_v2>(e3));
    e2.set_rhs(std::make_shared<Token>("4.0", Token::number));
    e2.set_operation(Expression_v2::divide);
    e1.set_lhs(std::make_shared<Expression_v2>(e2));
    e1.set_rhs(std::make_shared<Token>("LUT_DEPTH", Token::identifier));
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
        {{"","", "LUT_DEPTH"}, 9},
        {{"","", "STEP"}, 0.17453292519943295f}
    };

    ASSERT_EQ(9, defaults.at({"","", "LUT_DEPTH"}).get_integer());
    ASSERT_FLOAT_EQ(0.17453292519943295,defaults.at({"","", "STEP"}).get_real());
}

TEST(parameter_extraction, simple_system_task) {
    auto test_pattern = R"(

    module test_mod #(
    )();

        localparam CAST = $rtoi(16.8);

    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("CAST");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    HDL_function_call call("$rtoi");
    call.add_argument(std::make_shared<Token>("16.8", Token::number));

    p->set_raw_value(std::make_shared<HDL_function_call>(call));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {{"","", "CAST"}, 17},
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, multiple_system_task) {
    auto test_pattern = R"(

    module test_mod #(
    )();

        localparam CAST = $rtoi(14.8+2);
        localparam CAST_2 = $rtoi(12.2);

    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("CAST");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    HDL_function_call call("$rtoi");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("14.8", Token::number));
    e.set_rhs(std::make_shared<Token>("2", Token::number));
    e.set_operation(Expression_v2::add);
    call.add_argument(std::make_shared<Expression_v2>(e));

    p->set_raw_value(std::make_shared<HDL_function_call>(call));

    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("CAST_2");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    call = HDL_function_call("$rtoi");
    call.add_argument(std::make_shared<Token>("12.2", Token::number));
    p->set_raw_value(std::make_shared<HDL_function_call>(call));

    check_params.insert(p);
    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {{"","", "CAST"}, 17},
        {{"","", "CAST_2"}, 12},
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, system_task_propagation) {
    auto test_pattern = R"(

    module test_mod #(
        parameter PARAMETER_1 = 5
    )();

        localparam CAST = $rtoi(11.8 + PARAMETER_1);

    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("CAST");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    HDL_function_call call("$rtoi");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("11.8", Token::number));
    e.set_rhs(std::make_shared<Token>("PARAMETER_1", Token::identifier));
    e.set_operation(Expression_v2::add);
    call.add_argument(std::make_shared<Expression_v2>(e));

    p->set_raw_value(std::make_shared<HDL_function_call>(call));

    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
    p->set_name("PARAMETER_1");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("5", Token:: number));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {{"","", "CAST"}, 17},
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, nested_system_task) {
    auto test_pattern = R"(

    module test_mod #(
        parameter PARAMETER_1 = 46
    )();

        localparam CAST = $rtoi($ceil(PARAMETER_1/16.0));

    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("PARAMETER_1");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("46", Token:: number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("CAST");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    auto inner_call = std::make_shared<HDL_function_call>("$ceil");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("PARAMETER_1", Token::identifier));
    e.set_operation(Expression_v2::divide);
    e.set_rhs(std::make_shared<Token>("16.0", Token::number));
    inner_call->add_argument(std::make_shared<Expression_v2>(e));
    auto outer_call = std::make_shared<HDL_function_call>("$rtoi");
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
        {{"","", "PARAMETER_1"}, 46},
        {{"","", "CAST"}, 3}
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("bus_base");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("32'h43c00000", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("timebase");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("bus_base", Token::identifier));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("gpio");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e, e1, e2, e3;
    e3.set_lhs(std::make_shared<Token>("32'h1000", Token::number));
    e3.set_rhs(std::make_shared<Token>("2", Token::number));
    e3.set_operation(Expression_v2::multiply);
    e2.set_lhs(std::make_shared<Expression_v2>(e3));
    e2.set_rhs(std::make_shared<Token>("2", Token::number));
    e2.set_operation(Expression_v2::divide);
    e1.set_lhs(std::make_shared<Token>("timebase", Token::identifier));
    e1.set_rhs(std::make_shared<Expression_v2>(e2));
    e1.set_operation(Expression_v2::add);
    e.set_lhs(std::make_shared<Expression_v2>(e1));
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("modulo_parameter");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Token>("3", Token::number));
    e.set_rhs(std::make_shared<Token>("2", Token::number));
    e.set_operation(Expression_v2::expression_operator::modulo);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("subtraction_parameter");
    p->set_type(Type_engine::create_primitive_type("implicit"));

    e.set_lhs(std::make_shared<Token>("'o4", Token::number));
    e.set_rhs(std::make_shared<Token>("'b10", Token::number));
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
        {{"","", "bus_base"}, 0x43c00000},
        {{"","", "timebase"}, 0x43c00000},
        {{"","", "gpio"}, 0x43c01001},
        {{"","", "modulo_parameter"}, 1},
        {{"","", "subtraction_parameter"}, 2}
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("simple_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("32", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("local_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("74", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("5'o10", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("dimensionless_sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("'h3F", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("string_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("\"423\"", Token::string));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("nested_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("string_p", Token::identifier));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {{"","", "simple_numeric_p"}, 32},
        {{"","", "local_p"}, 74},
        {{"","", "sv_numeric_p"}, 8},
        {{"","", "dimensionless_sv_numeric_p"}, 63},
        {{"","", "string_p"}, R"("423")"s},
        {{"","", "nested_p"}, R"("423")"s}
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("simple_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("32", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("5'o10", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("dimensionless_sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("'h3F", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("add_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("simple_numeric_p", Token::identifier));
    e.set_rhs(std::make_shared<Token>("sv_numeric_p", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("sub_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Token>("simple_numeric_p", Token::identifier));
    e.set_rhs(std::make_shared<Token>("sv_numeric_p", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("mul_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Token>("simple_numeric_p", Token::identifier));
    e.set_rhs(std::make_shared<Token>("sv_numeric_p", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::multiply);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("div_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Token>("simple_numeric_p", Token::identifier));
    e.set_rhs(std::make_shared<Token>("sv_numeric_p", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::divide);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("modulo_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Token>("simple_numeric_p", Token::identifier));
    e.set_rhs(std::make_shared<Token>("sv_numeric_p", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::modulo);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("chained_expression");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Token>("add_expr_p", Token::identifier));
    Expression_v2 e2;
    e2.set_lhs(std::make_shared<Token>("mul_expr_p", Token::identifier));
    e2.set_rhs(std::make_shared<Token>("5", Token::number));
    e2.set_operation(Expression_v2::expression_operator::multiply);
    e.set_rhs(std::make_shared<Expression_v2>(e2));
    e.set_operation(Expression_v2::expression_operator::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("complex_log_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    HDL_function_call call("$clog2");
    e = Expression_v2();
    e.set_lhs(std::make_shared<Token>("add_expr_p", Token::identifier));
    e.set_rhs(std::make_shared<Token>("2", Token::number));
    e.set_operation(Expression_v2::expression_operator::add);
    call.add_argument(std::make_shared<Expression_v2>(e));

    p->set_raw_value(std::make_shared<HDL_function_call>(call));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("simple_log_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    call = HDL_function_call("$clog2");
    call.add_argument(std::make_shared<Token>("add_expr_p", Token::identifier));
    p->set_raw_value(std::make_shared<HDL_function_call>(call));

    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("parenthesised_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));

    e2 = Expression_v2();
    e2.set_lhs(std::make_shared<Token>("add_expr_p", Token::identifier));
    e2.set_operation(Expression_v2::expression_operator::add);
    e2.set_rhs(std::make_shared<Token>("mul_expr_p", Token::identifier));
    e = Expression_v2();
    e.set_rhs(std::make_shared<Token>("5", Token::number));
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
        {{"", "", "simple_numeric_p"}, 32},
        {{"", "", "sv_numeric_p"}, 8},
        {{"", "", "dimensionless_sv_numeric_p"}, 63},
        {{"", "", "simple_log_expr_p"},6},
        {{"", "", "add_expr_p"}, 40},
        {{"", "", "sub_expr_p"}, 24},
        {{"", "", "mul_expr_p"}, 256},
        {{"", "", "div_expr_p"}, 4},
        {{"", "", "chained_expression"}, 1320},
        {{"", "", "modulo_expr_p"}, 0},
        {{"", "", "complex_log_expr_p"}, 6},
        {{"", "", "parenthesised_expr_p"}, 1480}
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("9", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("12", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_and_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::bitwise_and);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_or_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::bitwise_or);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_xor_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::bitwise_xor);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_xnor_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::bitwise_xnor);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("b_xnor2_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
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
        {{"", "", "op_a"}, 9},
        {{"", "", "op_b"}, 12},
        {{"", "", "b_and_p"}, 8},
        {{"", "", "b_or_p"},13},
        {{"", "", "b_xor_p"}, 5},
        {{"", "", "b_xnor_p"}, -6},
        {{"", "", "b_xnor2_p"}, -6}
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("2", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("5", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("pow_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
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
        {{"", "", "op_a"}, 2},
        {{"", "", "op_b"}, 5},
        {{"", "", "pow_expr"},32},
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("3", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("2", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("shl_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
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
        {{"", "", "op_a"}, 3},
        {{"", "", "op_b"}, 2},
        {{"", "", "shl_expr"},12},
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("8", Token::number));
    e.set_operation(Expression_v2::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("2", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("shr_a_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = Expression_v2();
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::arithmetic_shift_right);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("shr_l_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
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
        {{"", "", "op_a"}, -8},
        {{"", "", "op_b"}, 2},
        {{"", "", "shr_a_expr"}, -2},
        {{"", "", "shr_l_expr"}, 1073741822},
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("op_a");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("3", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_b");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("0", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("op_c");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("5", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("log_and_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::logical_and);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("log_or_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::logical_or);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("mixed_expr");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e2;
    e.set_lhs(std::make_shared<Token>("op_a", Token::identifier));
    e.set_rhs(std::make_shared<Token>("op_b", Token::identifier));
    e.set_operation(Expression_v2::expression_operator::logical_and);
    e2.set_lhs(std::make_shared<Expression_v2>(e));
    e2.set_rhs(std::make_shared<Token>("op_c", Token::identifier));
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
        {{"", "", "op_a"}, 3},
        {{"", "", "op_b"}, 0},
        {{"", "", "op_c"}, 5},
        {{"", "", "log_and_expr"}, 0},
        {{"", "", "log_or_expr"}, 1},
        {{"", "", "mixed_expr"}, 1},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, array_assignment) {
    auto test_pattern = R"(
        module test_mod #(
            parameter simple_numeric_p = 32,
            sv_numeric_p = 5'o10
        )();
            parameter logic [31:0] concatenation [1:0] = '{simple_numeric_p, sv_numeric_p};

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("simple_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("32", Token::number));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();

    p->set_name("sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("5'o10", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("concatenation");

    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("31", Token::number), std::make_shared<Token>("0", Token::number), true});
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    Concatenation c;
    c.add_component(std::make_shared<Token>("simple_numeric_p", Token::identifier));
    c.add_component(std::make_shared<Token>("sv_numeric_p", Token::identifier));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));





    check_params.insert(p);


   ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {8, 32});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"", "", "simple_numeric_p"}, 32},
        {{"", "", "sv_numeric_p"}, 8},
        {{"", "", "concatenation"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, default_assign) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            parameter logic [31:0] test_array [1:0] = '{default:5};

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_array");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("31", Token::number), std::make_shared<Token>("0", Token::number), true});
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    Concatenation c;
    c.set_default_init();
    c.add_component(std::make_shared<Token>("5", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));





    check_params.insert(p);


   ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {5, 5});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"", "", "test_array"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, array_concatenation) {
    auto test_pattern = R"(
        module test_mod #(
            parameter simple_numeric_p = 32,
            sv_numeric_p = 5'o10
        )();
            parameter logic [31:0] concatenation [1:0] = {simple_numeric_p, sv_numeric_p};

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("simple_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("32", Token::number));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();

    p->set_name("sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("5'o10", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("concatenation");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("31", Token::number), std::make_shared<Token>("0", Token::number), true});
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    Concatenation c;
    c.add_component(std::make_shared<Token>("simple_numeric_p", Token::identifier));
    c.add_component(std::make_shared<Token>("sv_numeric_p", Token::identifier));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));



    check_params.insert(p);


   ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {8,32});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"", "", "simple_numeric_p"}, 32},
        {{"", "", "sv_numeric_p"}, 8},
        {{"", "", "concatenation"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, array_parameter) {
    auto test_pattern = R"(
        module test_mod #(
            parameter [31:0] array_parameter [1:0] = '{32, 5}
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter");



    dimension_t d;
    d.first_bound = std::make_shared<Token>("31", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Token>("1", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = false;
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Token>("32", Token::number));
    c.add_component(std::make_shared<Token>("5", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {5, 32});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "array_parameter"}, array_value}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, integer_localparams) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            localparam serial_msb_out_first = 0, serial_lsb_out_first = 1;
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("serial_msb_out_first");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("0", Token::number));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("serial_lsb_out_first");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("1", Token::number));

    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "serial_msb_out_first"}, 0},
        {{"","", "serial_lsb_out_first"}, 1},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, simple_array_propagation) {
    auto test_pattern = R"(
        module test_mod #(
            parameter [31:0] array_parameter [1:0] = '{32, 5}
        )();
            parameter array_parameter_expr_p = array_parameter[0] + array_parameter[1];
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter");



    dimension_t d;
    d.first_bound = std::make_shared<Token>("31", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Token>("1", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = false;
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Token>("32", Token::number));
    c.add_component(std::make_shared<Token>("5", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    Token t = Token("array_parameter", Token::identifier);
    std::vector<std::shared_ptr<Parameter_value_base>> ai;
    ai.push_back(std::make_shared<Token>("0", Token::number));
    t.set_array_index(ai);
    e.set_lhs(std::make_shared<Token>(t));
    t = Token("array_parameter", Token::identifier);
    ai.clear();
    ai.push_back(std::make_shared<Token>("1", Token::number));
    t.set_array_index(ai);
    e.set_rhs(std::make_shared<Token>(t));
    e.set_operation(Expression_v2::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {5, 32});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "array_parameter"}, array_value},
        {{"","", "array_parameter_expr_p"}, 37},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, array_expression) {
    auto test_pattern = R"(
        module test_mod #(
            parameter sv_numeric_p = 1,
            parameter [31:0] array_parameter [1:0] = '{32, 5}
        )();
            parameter array_parameter_expr_p = array_parameter[sv_numeric_p*0] + array_parameter[1];
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter");



    dimension_t d;
    d.first_bound = std::make_shared<Token>("31", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Token>("1", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = false;
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Token>("32", Token::number));
    c.add_component(std::make_shared<Token>("5", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Token t = Token("array_parameter", Token::identifier);
    std::vector<std::shared_ptr<Parameter_value_base>> ai;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("sv_numeric_p", Token::identifier));
    e.set_rhs(std::make_shared<Token>("0", Token::number));
    e.set_operation(Expression_v2::multiply);
    ai.push_back(std::make_shared<Expression_v2>(e));
    t.set_array_index(ai);
    Expression_v2 e2;
    e2.set_lhs(std::make_shared<Token>(t));
    t = Token("array_parameter", Token::identifier);
    ai.clear();
    ai.push_back(std::make_shared<Token>("1", Token::number));
    t.set_array_index(ai);
    e2.set_rhs(std::make_shared<Token>(t));
    e2.set_operation(Expression_v2::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e2));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();

    p->set_name("sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("1", Token::number));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {5, 32});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "array_parameter"}, array_value},
        {{"","", "array_parameter_expr_p"}, 37},
        {{"","", "sv_numeric_p"}, 1},

    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, int_concat_initialization) {
    auto test_pattern = R"(
        module test_mod #(
            parameter int test_parameter  = '{1'b1, 1'b1, 1'b1}
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_parameter");

    Concatenation c;
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    auto param_type = Type_engine::create_primitive_type("int");
    p->set_type(param_type);
    p->set_raw_value(std::make_shared<Concatenation>(c));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "test_parameter"}, 7}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }

}


TEST(parameter_extraction, implicit_type_concatenation) {
    auto test_pattern = R"(
        module test_mod #(
            parameter test_parameter  = {1'b1, 1'b1, 1'b1}
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_parameter");

    Concatenation c;
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    auto param_type = Type_engine::create_primitive_type("implicit");
    p->set_type(param_type);
    p->set_raw_value(std::make_shared<Concatenation>(c));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "test_parameter"}, 7}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }

}


TEST(parameter_extraction, simple_repetition_initialization) {
    auto test_pattern = R"(
        module test_mod #(
            parameter repetition_size = 2,
            parameter int repetition_parameter_1 [1:0]  = '{repetition_size{1}}
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_size");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("2", Token::number));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_parameter_1");


    auto param_type = Type_engine::create_primitive_type("int")->as<HDL_simple_type>();
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    Replication rep;
    rep.set_size(std::make_shared<Token>("repetition_size", Token::identifier));
    rep.set_item(std::make_shared<Token>("1", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Replication>(rep));




    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_2d_slice({0}, {{1,1}});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "repetition_size"}, 2},
        {{"","", "repetition_parameter_1"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }

}

TEST(parameter_extraction, packed_repetition_initialization) {
    auto test_pattern = R"(
        module test_mod #(
            parameter repetition_size = 2,
            parameter int repetition_parameter_1  = {repetition_size{1}}
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_size");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("2", Token::number));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_parameter_1");

    Replication rep;
    auto size = std::make_shared<Token>("repetition_size", Token::identifier);
    rep.set_size(size);
    rep.set_item(std::make_shared<Token>("1", Token::number));
    auto param_type = Type_engine::create_primitive_type("int");
    p->set_type(param_type);
    p->set_raw_value(std::make_shared<Replication>(rep));




    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "repetition_size"}, 2},
        {{"", "","repetition_parameter_1"}, 3}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }

}

TEST(parameter_extraction, repetition_initialization) {
    auto test_pattern = R"(
        module test_mod #(
            parameter repetition_size = 2,
            parameter bit repetition_parameter_1 [1:0]  = '{repetition_size{1}},
            parameter bit repetition_parameter_2 [1:0]  = '{repetition_size{4}},
            parameter bit multi_repetition_parameter [3:0]  = {repetition_parameter_1,repetition_parameter_2},
            parameter bit mixed_repetition_parameter [3:0]  = {1,2,repetition_parameter_2}
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_size");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("2", Token::number));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_parameter_1");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    Replication r;
    auto size = std::make_shared<Token>("repetition_size", Token::identifier);

    r.set_size(size);
    r.set_item(std::make_shared<Token>("1", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Replication>(r));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_parameter_2");


    auto param_type_2 = HDL_simple_type();
    param_type_2.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    size = std::make_shared<Token>("repetition_size", Token::identifier);
    r.set_size(size);
    r.set_item(std::make_shared<Token>("4", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type_2));
    p->set_raw_value(std::make_shared<Replication>(r));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("multi_repetition_parameter");


    auto param_type_3 = HDL_simple_type();
    param_type_3.add_dimension({
    std::make_shared<Token>("3", Token::number),
    std::make_shared<Token>("0", Token::number),
    false
    });
    Concatenation c;
    c.add_component(std::make_shared<Token>("repetition_parameter_1", Token::identifier));
    c.add_component( std::make_shared<Token>("repetition_parameter_2", Token::identifier));
    p->set_type(std::make_shared<HDL_simple_type>(param_type_3));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("mixed_repetition_parameter");


    auto param_type_4 = HDL_simple_type();
    param_type_4.add_dimension({
    std::make_shared<Token>("3", Token::number),
    std::make_shared<Token>("0", Token::number),
    false
    });
    c = Concatenation();
    c.add_component(std::make_shared<Token>("1", Token::number));
    c.add_component( std::make_shared<Token>("2", Token::number));
    c.add_component( std::make_shared<Token>("repetition_parameter_2", Token::identifier));
    p->set_type(std::make_shared<HDL_simple_type>(param_type_4));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    mdarray<hdl_integer> av, av2, av3, av4;
    av.set_2d_slice({0}, {{1,1}});
    av2.set_2d_slice({0}, {{4,4}});
    av3.set_2d_slice({0}, {{4, 4, 1, 1}});
    av4.set_2d_slice({0}, {{4, 4, 2, 1}});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "repetition_size"}, 2},
        {{"","", "repetition_parameter_1"}, av},
        {{"","", "repetition_parameter_2"}, av2},
        {{"","", "multi_repetition_parameter"}, av3},
        {{"","", "mixed_repetition_parameter"}, av4}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }

}

TEST(parameter_extraction, packed_array) {
    auto test_pattern = R"(
        module test_mod #(
             parameter logic [7:0] packed_param = {1'b1,1'b0,1'b1,1'b0,1'b1,1'b0,1'b0,1'b1}
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("packed_param");

    dimension_t d;
    d.first_bound = std::make_shared<Token>("7", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = true;

    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));



    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});


    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "packed_param"}, 169}
    };

    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, multpidim_packed_array) {
    auto test_pattern = R"(
        module test_mod #(
             parameter logic [7:0] packed_param [1:0] = '{{1'b1,1'b0,1'b1,1'b0,1'b1,1'b0,1'b0,1'b1},{1'b1,1'b1,1'b0,1'b0,1'b1,1'b0,1'b0,1'b1}}
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("packed_param");



    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("7", Token::number), std::make_shared<Token>("0", Token::number), true});
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    Concatenation c2;
    Concatenation c;
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c2.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation({});
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c2.add_component(std::make_shared<Concatenation>(c));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c2));


    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {201, 169});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "packed_param"}, av }
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

    auto resources = analyzer.analyze("", test_pattern);
    auto resource = resources[1];
    auto pkg = resources[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("package_param");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Token t("bus_base", Token::identifier);
    t.set_package_prefix("test_package");
    p->set_raw_value(std::make_shared<Token>(t));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> pkg_ctx;
    for (auto &[id, val] : pkg_defaults) {
        pkg_ctx[{"test_package", "", id.name}] = val;
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), pkg_ctx);
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "package_param"}, 67}
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

    auto resources = analyzer.analyze("", test_pattern);
    auto res = resources[1].get_dependencies()[2];
    auto parameters = res.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("TEST_PARAM");
    Token ec("DATA_WIDTH", Token::identifier);
    ec.set_package_prefix("test_interface");
    p->set_raw_value(std::make_shared<Token>(ec));
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("negative_param");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("16'sd32767", Token::number));
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

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "negative_param"}, -32767}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, packed_bit_access) {
    auto test_pattern = R"(
        module test_mod #(
            parameter [31:0] param_a = -1,
            parameter [5:0] param_b = param_a[3]
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("param_a");
    auto param_type = HDL_simple_type();
    param_type.add_dimension({
          std::make_shared<Token>("31", Token::number),
          std::make_shared<Token>("0", Token::number),
          true
      });
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("param_b");
    auto param_type_2 = HDL_simple_type();
    param_type_2.add_dimension({
    std::make_shared<Token>("5", Token::number),
    std::make_shared<Token>("0", Token::number),
    true
});
    Token t("param_a", Token::identifier);
    t.add_array_index(std::make_shared<Token>("3", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type_2));
    p->set_raw_value(std::make_shared<Token>(t));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {8, 32});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "param_a"}, -1},
        {{"","", "param_b"}, 1}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, negative_number_array_init) {
    auto test_pattern = R"(
        module test_mod #(
             parameter negative_array_param [1:0] = '{-16'sd32767, 16'sd32767}
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("negative_array_param");
    auto param_type = Type_engine::create_primitive_type("implicit")->as<HDL_simple_type>();
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number)});
    Concatenation c;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("16'sd32767", Token::number));
    e.set_operation(Expression_v2::subtract);
    c.add_component(std::make_shared<Expression_v2>(e));
    c.add_component(std::make_shared<Token>("16'sd32767", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {32767, -32767});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "negative_array_param"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, expression_array_init) {
    auto test_pattern = R"(
        module test_mod #(
             parameter expression_array_param [1:0] = '{5+4, 7*6}
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("expression_array_param");
    auto param_type = Type_engine::create_primitive_type("implicit")->as<HDL_simple_type>();
    param_type.add_dimension({ std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    Concatenation c;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("5", Token::number));
    e.set_rhs(std::make_shared<Token>("4", Token::number));
    e.set_operation(Expression_v2::add);
    c.add_component(std::make_shared<Expression_v2>(e));

    e.set_lhs(std::make_shared<Token>("7", Token::number));
    e.set_rhs(std::make_shared<Token>("6", Token::number));
    e.set_operation(Expression_v2::multiply);
    c.add_component(std::make_shared<Expression_v2>(e));

    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {42, 9});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "expression_array_param"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, combined_packed_unpacked_init) {
    auto test_pattern = R"(
    module test_mod #(
      parameter reg [7:0] param_a [1:0] = '{{1'b1,1'b1,1'b1,1'b0,1'b0,1'b0,1'b1,1'b0}, {1'b0,1'b0,1'b0,1'b1,1'b1,1'b1,1'b0,1'b1}},
      parameter reg [7:0] param_b [1:0] = '{{8{1'b1}},{8{1'b0}}}
    )();
    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("param_a");



    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("7", Token::number), std::make_shared<Token>("0", Token::number), true});
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    Concatenation c2;
    Concatenation c;
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c2.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c.add_component(std::make_shared<Token>("1'b0", Token::number));
    c.add_component(std::make_shared<Token>("1'b1", Token::number));
    c2.add_component(std::make_shared<Concatenation>(c));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c2));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("param_b");


    c2 = Concatenation();
    c = Concatenation();

    auto param_type_2 = HDL_simple_type();
    param_type_2.add_dimension({std::make_shared<Token>("7", Token::number), std::make_shared<Token>("0", Token::number), true});
    param_type_2.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number), false});
    Replication r;
    auto size = std::make_shared<Token>("8", Token::number);
    r.set_size(size);
    r.set_item(std::make_shared<Token>("1'b1", Token::number));
    c2.add_component(std::make_shared<Replication>(r));
    r = Replication();
    size = std::make_shared<Token>("8", Token::number);
    r.set_size(size);
    r.set_item(std::make_shared<Token>("1'b0", Token::number));
    c2.add_component(std::make_shared<Replication>(r));
    p->set_type(std::make_shared<HDL_simple_type>(param_type_2));
    p->set_raw_value(std::make_shared<Concatenation>(c2));




    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av, av2;
    av.set_1d_slice({0, 0}, {29, 226});
    av2.set_1d_slice({0, 0}, {0, 255});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"", "", "param_a"}, av},
        {{"", "", "param_a"}, av2}
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto def_parameters = resource.get_parameters();

    auto inst_parameters = resource.get_dependencies()[0].get_parameters();

    std::vector<std::pair<std::string, std::vector<std::string>>> vect_params = {
            {"test_param", {"4"}}
    };

    Parameters_map check_params;
    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_param");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("4", Token::number));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), def_parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(def_parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *def_parameters.get(item->get_name()));
    }

    check_params.clear();

    p = std::make_shared<HDL_parameter>();

    p->set_name("param_1");
    p->set_raw_value(std::make_shared<Token>("test_param", Token::identifier));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();

    p->set_name("param_2");
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("test_param", Token::identifier));
    e.set_rhs(std::make_shared<Token>("5", Token::number));
    e.set_operation(Expression_v2::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();

    p->set_name("param_3");
    Expression_v2 e1, e2;
    e1.set_lhs(std::make_shared<Token>("test_param", Token::identifier));
    e1.set_rhs(std::make_shared<Token>("7", Token::number));
    e1.set_operation(Expression_v2::add);
    e2.set_lhs(std::make_shared<Expression_v2>(e1));
    e2.set_rhs(std::make_shared<Token>("1", Token::number));
    e2.set_operation(Expression_v2::multiply);
    p->set_raw_value(std::make_shared<Expression_v2>(e2));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), inst_parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(inst_parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *inst_parameters.get(item->get_name()));
    }

}


TEST(parameter_extraction, mixed_packed_unpacked_init) {
    auto test_pattern = R"(
    module test_mod #(
        SS_POLARITY_DEFAULT = 0
    )();


        localparam logic [31:0] FIXED_REGISTER_VALUES [4:0]= '{
            3,
            3,
            3,
            {SS_POLARITY_DEFAULT,3'b0,SS_POLARITY_DEFAULT,5'b0,4'hE,4'b0},
            {2'h2,2'b1,2'h3,4'hE,4'b0}
        };
    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    std::vector<std::pair<std::string, std::vector<std::string>>> vect_params = {
            {"", {}}
    };

    check_params.clear();

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("SS_POLARITY_DEFAULT");
    p->set_raw_value(std::make_shared<Token>("0", Token::number));
    p->set_type(Type_engine::create_primitive_type("implicit"));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("FIXED_REGISTER_VALUES");



    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("31", Token::number), std::make_shared<Token>("0", Token::number), true});
    param_type.add_dimension({std::make_shared<Token>("4", Token::number), std::make_shared<Token>("0", Token::number), false});
    Concatenation outer_c;
    outer_c.add_component(std::make_shared<Token>("3", Token::number));
    outer_c.add_component(std::make_shared<Token>("3", Token::number));
    outer_c.add_component(std::make_shared<Token>("3", Token::number));
    Concatenation c;
    c.add_component(std::make_shared<Token>("SS_POLARITY_DEFAULT", Token::identifier));
    c.add_component(std::make_shared<Token>("3'b0", Token::number));
    c.add_component(std::make_shared<Token>("SS_POLARITY_DEFAULT", Token::identifier));
    c.add_component(std::make_shared<Token>("5'b0", Token::number));
    c.add_component(std::make_shared<Token>("4'hE", Token::number));
    c.add_component(std::make_shared<Token>("4'b0", Token::number));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Token>("2'h2", Token::number));
    c.add_component(std::make_shared<Token>("2'b1", Token::number));
    c.add_component(std::make_shared<Token>("2'h3", Token::number));
    c.add_component(std::make_shared<Token>("4'hE", Token::number));
    c.add_component(std::make_shared<Token>("4'b0", Token::number));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(outer_c));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {0x27e0, 0xe0, 3 , 3, 3});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "SS_POLARITY_DEFAULT"}, 0},
        {{"", "", "FIXED_REGISTER_VALUES"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, multidimensional_packed_array) {
    auto test_pattern = R"(
    module test_mod #(
        parameter reg [7:0] param_a [1:0][1:0] = '{
            {
                {1'b1,1'b1,1'b1,1'b0,1'b0,1'b0,1'b1,1'b0},
                {1'b0,1'b0,1'b0,1'b1,1'b1,1'b1,1'b0,1'b1}
            },
            {
                {1'b0,1'b0,1'b0,1'b1,1'b1,1'b1,1'b0,1'b1},
                {1'b1,1'b1,1'b1,1'b0,1'b0,1'b0,1'b1,1'b0}
            }
        }
    )();
    endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    check_params.clear();


    auto v1 = {
        Token("1'b1", Token::number),
        Token("1'b1", Token::number),
        Token("1'b1", Token::number),
        Token("1'b0", Token::number),
        Token("1'b0", Token::number),
        Token("1'b0", Token::number),
        Token("1'b1", Token::number),
        Token("1'b0", Token::number)
    };
    auto v2 = {
                Token("1'b0", Token::number),
                Token("1'b0", Token::number),
                Token("1'b0", Token::number),
                Token("1'b1", Token::number),
                Token("1'b1", Token::number),
                Token("1'b1", Token::number),
                Token("1'b0", Token::number),
                Token("1'b1", Token::number)
    };

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("param_a");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("7", Token::number), std::make_shared<Token>("0", Token::number), true});
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number),false});
    param_type.add_dimension({std::make_shared<Token>("1", Token::number), std::make_shared<Token>("0", Token::number),false});
    Concatenation top_c, outer_c, inner_c;

    for(const auto& item:v1){
        inner_c.add_component(std::make_shared<Token>(item));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c  = Concatenation();
    for(const auto& item:v2){
        inner_c.add_component(std::make_shared<Token>(item));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    top_c.add_component(std::make_shared<Concatenation>(outer_c));
    outer_c = Concatenation();
    inner_c  = Concatenation();
    for(const auto& item:v2){
        inner_c.add_component(std::make_shared<Token>(item));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c  = Concatenation();
    for(const auto& item:v1){
        inner_c.add_component(std::make_shared<Token>(item));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    top_c.add_component(std::make_shared<Concatenation>(outer_c));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(top_c));


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {226, 29});
    av.set_1d_slice({0, 1}, {29, 226});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"", "", "param_a"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, packed_replication_init) {
    auto test_pattern = R"(
        module test_mod #(
             parameter [4:0] test_parameter = {5{1'b1}}
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_parameter");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Token>("4", Token::number), std::make_shared<Token>("0", Token::number), true});
    Replication r;
    auto size = std::make_shared<Token>("5", Token::number);
    r.set_size(size);
    r.set_item(std::make_shared<Token>("1'b1", Token::number));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Replication>(r));




    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "test_parameter"}, 31}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }

}


TEST(parameter_extraction, array_initialization_default) {
    auto test_pattern = R"(
        module test_mod #(
             parameter [4:0] test_parameter [2:0][1:0] = '{default:3}
        )();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("test_parameter");

    Concatenation c;

    dimension_t d;
    d.first_bound = std::make_shared<Token>("4", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Token>("2", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = false;
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Token>("1", Token::number);
    d.second_bound = std::make_shared<Token>("0", Token::number);
    d.packed = false;
    param_type.add_dimension(d);
    c.set_default_init();
    c.add_component(std::make_shared<Token>("3", Token::number));

    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {3, 3});
    av.set_1d_slice({0, 1}, {3, 3});
    av.set_1d_slice({0, 2}, {3, 3});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "test_parameter"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}



TEST(parameter_extraction, simple_function_parameter) {
    auto test_pattern = R"(


        module test_mod #(
        )();
            localparam ADDR_WIDTH = 31;
            function logic [ADDR_WIDTH-1:0] CTRL_ADDR_CALC();
                CTRL_ADDR_CALC = 100;
            endfunction

            parameter [ADDR_WIDTH-1:0] TEST_PARAM = CTRL_ADDR_CALC();
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto param =  resource.get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    auto param_type = HDL_simple_type();
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("ADDR_WIDTH", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::subtract);
    param_type.add_dimension({
         std::make_shared<Expression_v2>(e),
        std::make_shared<Token>("0", Token::number),
        true
    });
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Token>("CTRL_ADDR_CALC", Token::identifier));
    HDL_function_call call("CTRL_ADDR_CALC");
    assignment a(
        "CTRL_ADDR_CALC",
        std::nullopt,
        std::make_shared<Token>("100", Token::number)
    );
    call.add_body({a},std::nullopt);
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "TEST_PARAM"}, 100}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, concat_in_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function [15:0] get_axis_metadata (input [4:0] size,input is_signed, input is_float);
              begin
                get_axis_metadata = { 10'h0, is_float, is_signed, 1'b1};
              end
            endfunction

            parameter integer TEST_PARAM = get_axis_metadata(11, 1'b1, 1'b0);
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto param =  resource.get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    p.set_type(Type_engine::create_primitive_type("integer"));
    p.set_raw_value(std::make_shared<Token>("CTRL_ADDR_CALC", Token::identifier));
    HDL_function_call call("get_axis_metadata");
    call.add_argument(std::make_shared<Token>("11", Token::number));
    call.add_argument(std::make_shared<Token>("1'b1", Token::number));
    call.add_argument(std::make_shared<Token>("1'b0", Token::number));
    assignment a("get_axis_metadata", std::nullopt, nullptr);

    Concatenation concat;
    concat.add_component(std::make_shared<Token>("10'h0", Token::number));
    concat.add_component(std::make_shared<Token>("1'b0", Token::number));
    concat.add_component(std::make_shared<Token>("1'b1", Token::number));
    concat.add_component(std::make_shared<Token>("1'b1", Token::number));

    a.set_value(std::make_shared<Concatenation>(concat));
    call.add_body({a},std::nullopt);
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "TEST_PARAM"}, 3}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, replication_in_function) {
    auto test_pattern = R"(


        module test_mod #(
        )();
            function [15:0] get_axis_metadata (input [4:0] size,input is_signed, input is_float);
              begin
                get_axis_metadata = {4{1'b1}};
              end
            endfunction

            parameter integer TEST_PARAM = get_axis_metadata(11, 1'b1, 1'b0);
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto param =  resource.get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    p.set_type(Type_engine::create_primitive_type("integer"));
    p.set_raw_value(std::make_shared<Token>("CTRL_ADDR_CALC", Token::identifier));
    HDL_function_call call("get_axis_metadata");
    call.add_argument(std::make_shared<Token>("11", Token::number));
    call.add_argument(std::make_shared<Token>("1'b1", Token::number));
    call.add_argument(std::make_shared<Token>("1'b0", Token::number));
    assignment a("get_axis_metadata", std::nullopt, nullptr);
    Cast c;
    c.set_size(std::make_shared<Token>("4", Token::number));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("11", Token::number));
    e.set_rhs(std::make_shared<Token>("8", Token::number));
    e.set_operation(Expression_v2::subtract);
    c.set_content(std::make_shared<Expression_v2>(e));
    Replication repl;
    auto size = std::make_shared<Token>("4", Token::number);
    repl.set_size(size);
    repl.set_item(std::make_shared<Token>("1'b1", Token::number));
    a.set_value(std::make_shared<Replication>(repl));
    call.add_body({a},std::nullopt);
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "TEST_PARAM"}, 15}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        EXPECT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, cast_in_concat_in_function) {
    auto test_pattern = R"(


        module test_mod #(
        )();
            function [15:0] get_axis_metadata (input [4:0] size,input is_signed, input is_float);
              begin
                get_axis_metadata = { 10'h0, is_float, is_signed, 4'(size - 8)};
              end
            endfunction

            parameter integer TEST_PARAM = get_axis_metadata(11, 1'b1, 1'b0);
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto param =  resource.get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    p.set_type(Type_engine::create_primitive_type("integer"));
    p.set_raw_value(std::make_shared<Token>("CTRL_ADDR_CALC", Token::identifier));
    HDL_function_call call("get_axis_metadata");
    call.add_argument(std::make_shared<Token>("11", Token::number));
    call.add_argument(std::make_shared<Token>("1'b1", Token::number));
    call.add_argument(std::make_shared<Token>("1'b0", Token::number));
    assignment a("get_axis_metadata", std::nullopt, nullptr);
    Cast c;
    c.set_size(std::make_shared<Token>("4", Token::number));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("11", Token::number));
    e.set_rhs(std::make_shared<Token>("8", Token::number));
    e.set_operation(Expression_v2::subtract);
    c.set_content(std::make_shared<Expression_v2>(e));
    Concatenation concat;
    concat.add_component(std::make_shared<Token>("10'h0", Token::number));
    concat.add_component(std::make_shared<Token>("1'b0", Token::number));
    concat.add_component(std::make_shared<Token>("1'b1", Token::number));
    concat.add_component(std::make_shared<Cast>(c));

    a.set_value(std::make_shared<Concatenation>(concat));
    call.add_body({a},std::nullopt);
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "TEST_PARAM"}, 19}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, function_with_parameters) {
    auto test_pattern = R"(


        module test_mod #(
        )();

            function logic [ADDR_WIDTH-1:0] CTRL_ADDR_CALC(int i, reg [5:0] b [1:0]);
            endfunction

        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto func = resource.get_functions()["CTRL_ADDR_CALC"];



    HDL_function_def f;
    f.set_name("CTRL_ADDR_CALC");
    f.add_argument("i");
    f.add_argument("b");

    ASSERT_EQ(f,  func);

}



TEST(parameter_extraction, loop_function_parameter) {
    auto test_pattern = R"(


        module test_mod #(
        )();
            typedef logic [31:0] ctrl_addr_init_t [2:0];
                function ctrl_addr_init_t CTRL_ADDR_CALC();
                    for(int i = 0; i<3; i++)begin
                        CTRL_ADDR_CALC[i] = 100*i;
                    end
                endfunction

            parameter logic [31:0] TEST_PARAM [2:0] = CTRL_ADDR_CALC();
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto param = resource.get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    auto param_type = HDL_simple_type();

    param_type.add_dimension({
         std::make_shared<Token>("31", Token::number),
        std::make_shared<Token>("0", Token::number),
        true
    });
    param_type.add_dimension({
         std::make_shared<Token>("2", Token::number),
        std::make_shared<Token>("0", Token::number),
        false
    });
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Token>("CTRL_ADDR_CALC", Token::identifier));
    HDL_function_call call("CTRL_ADDR_CALC");
    HDL_loop_metadata loop;
    HDL_parameter idx;
    idx.set_name("i");
    idx.set_raw_value(std::make_shared<Token>("0", Token::number));
    loop.set_init(idx);

    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Token>("3", Token::number));
    e.set_operation(Expression_v2::less);
    loop.set_end_c(e);

    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::add);
    loop.set_iter(e);

    e.set_lhs(std::make_shared<Token>("100", Token::number));
    e.set_rhs(std::make_shared<Token>("i", Token::identifier));
    e.set_operation(Expression_v2::multiply);
    assignment a(
    "CTRL_ADDR_CALC",
        std::make_shared<Token>("i", Token::identifier),
        std::make_shared<Expression_v2>(e));
    loop.add_assignment(a);
    call.add_body({},loop);
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {0, 100, 200});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "TEST_PARAM"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, parametric_loop_function_parameter) {
    auto test_pattern = R"(

        module test_mod #(
            parameter N_CHAINS = 3,
            parameter OFFSET = 100
        )();

            typedef logic [31:0] ctrl_addr_init_t [2:0];
            function ctrl_addr_init_t CTRL_ADDR_CALC();
                for(int i = 0; i<N_CHAINS; i++)begin
                    CTRL_ADDR_CALC[i] = OFFSET*i;
                end
            endfunction

        parameter logic [31:0] TEST_PARAM [2:0] = CTRL_ADDR_CALC();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto param = resource.get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    p.set_raw_value(std::make_shared<Token>("CTRL_ADDR_CALC", Token::identifier));
    HDL_function_call call("CTRL_ADDR_CALC");
    HDL_loop_metadata loop;
    HDL_parameter idx;
    idx.set_name("i");
    idx.set_raw_value(std::make_shared<Token>("0", Token::number));
    loop.set_init(idx);

    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Token>("N_CHAINS", Token::identifier));
    e.set_operation(Expression_v2::less);
    loop.set_end_c(e);

    e.set_lhs(std::make_shared<Token>("i", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::add);
    loop.set_iter(e);

    e.set_lhs(std::make_shared<Token>("OFFSET", Token::identifier));
    e.set_rhs(std::make_shared<Token>("i", Token::identifier));
    e.set_operation(Expression_v2::multiply);
    assignment a(
        "CTRL_ADDR_CALC",
        std::make_shared<Token>("i", Token::identifier),
        std::make_shared<Expression_v2>(e)
    );
    loop.add_assignment(a);

    call.add_body({},loop);
    auto param_type = HDL_simple_type();
    param_type.add_dimension({
         std::make_shared<Token>("31", Token::number),
        std::make_shared<Token>("0", Token::number),
        true
    });
    param_type.add_dimension({
        std::make_shared<Token>("2", Token::number),
       std::make_shared<Token>("0", Token::number),
       false
   });
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {0, 100, 200});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "TEST_PARAM"}, av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, function_with_arguments) {
    auto test_pattern = R"(
        module test_mod #(
        )();

            function logic [31:0] add(integer a, integer b);
                add = a + b;
            endfunction

          parameter [31:0] TEST_PARAM = add(5, 7);

        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto param = resource.get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");

    HDL_function_call call("add");
    call.add_argument(std::make_shared<Token>("5", Token::number));
    call.add_argument(std::make_shared<Token>("7", Token::number));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>(5, 3));
    e.set_rhs(std::make_shared<Token>(7, 3));
    e.set_operation(Expression_v2::add);
    call.add_assignment({"add",std::nullopt, std::make_shared<Expression_v2>(e) });
    auto param_type = HDL_simple_type();
    param_type.add_dimension({
        std::make_shared<Token>("31", Token::number),
       std::make_shared<Token>("0", Token::number),
       true
   });
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});


    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"", "", "TEST_PARAM"}, 12}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
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

    auto resource = analyzer.analyze("", test_pattern)[0];

    HDL_instance i = resource.get_dependencies()[0];

    auto parameter = i.get_parameters().get("DECIMATED");

    auto check_param = std::make_shared<HDL_parameter>();
    check_param->set_name("DECIMATED");
    check_param->set_raw_value(std::make_shared<Token>("DECIMATE", Token::identifier));

    ASSERT_EQ(*parameter, *check_param);
}


TEST(parameter_extraction, interface_parameters) {
    auto test_pattern = R"(
        interface axi_stream #(DATA_WIDTH = 32, USER_WIDTH = 24,  DEST_WIDTH = 8);
        endinterface
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("DATA_WIDTH");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("32", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("USER_WIDTH");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("24", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("DEST_WIDTH");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("8", Token::number));
    check_params.insert(p);

    ASSERT_EQ(check_params, parameters);
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {8, 32});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "DATA_WIDTH"}, 32},
        {{"","", "USER_WIDTH"}, 24},
        {{"", "", "DEST_WIDTH"}, 8}
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

    auto resource = analyzer.analyze("", test_pattern)[0];

    auto dep = resource.get_dependencies()[0];

    ASSERT_EQ(dep.get_n_loops(), 1);

    auto loop = dep.get_inner_loop();

    HDL_loop_metadata check_loop;

    HDL_parameter p;
    p.set_name("n");
    p.set_raw_value(std::make_shared<Token>("0", Token::number));

    check_loop.set_init(p);

    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("n", Token::identifier));
    e.set_rhs(std::make_shared<Token>("N_REPETITIONS", Token::identifier));
    e.set_operation(Expression_v2::less);
    check_loop.set_end_c(e);

    e.set_lhs(std::make_shared<Token>("n", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::add);
    check_loop.set_iter(e);


    ASSERT_EQ(loop, check_loop);
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("condition");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("2", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("test_positive");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Ternary t;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("condition", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::greater);
    t.set_condition(std::make_shared<Expression_v2>(e));
    t.set_true_value(
        std::make_shared<Token>("12", Token::number));

    t.set_false_value(
        std::make_shared<Token>("34", Token::number));
    p->set_raw_value(std::make_shared<Ternary>(t));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("test_negative");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    t = Ternary();
    e.set_lhs(std::make_shared<Token>("condition", Token::identifier));
    e.set_rhs(std::make_shared<Token>("65", Token::number));
    e.set_operation(Expression_v2::greater);
    t.set_condition(std::make_shared<Expression_v2>(e));
    t.set_true_value(std::make_shared<Token>("12", Token::number));

    t.set_false_value(std::make_shared<Token>("34", Token::number));
    p->set_raw_value(std::make_shared<Ternary>(t));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "condition"}, 2},
        {{"","", "test_positive"}, 12},
        {{"","", "test_negative"}, 34},
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("condition");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("2", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("test_positive");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Ternary t;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("condition", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::greater);
    t.set_condition(std::make_shared<Expression_v2>(e));
    t.set_false_value(std::make_shared<Token>("34", Token::number));

    Ternary inner_t;
    e.set_lhs(std::make_shared<Token>("condition", Token::identifier));
    e.set_rhs(std::make_shared<Token>("65", Token::number));
    e.set_operation(Expression_v2::greater);
    inner_t.set_condition(std::make_shared<Expression_v2>(e));

    inner_t.set_true_value(std::make_shared<Token>("12", Token::number));

    inner_t.set_false_value(std::make_shared<Token>("96", Token::number));
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
        {{"","", "condition"}, 2},
        {{"","", "test_positive"}, 96},
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

    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("NM");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Token>("4", Token::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("LGNM");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Ternary t;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Token>("NM", Token::identifier));
    e.set_rhs(std::make_shared<Token>("1", Token::number));
    e.set_operation(Expression_v2::greater);
    t.set_condition(std::make_shared<Expression_v2>(e));
    HDL_function_call c("$clog2");
    c.add_argument(std::make_shared<Token>("NM", Token::identifier));
    t.set_true_value(std::make_shared<HDL_function_call>(c));
    t.set_false_value(std::make_shared<Token>("1", Token::number));

    p->set_raw_value(std::make_shared<Ternary>(t));
    check_params.insert(p);



    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(parameters.contains(name));
        ASSERT_EQ(*item, *parameters.get(name));
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "NM"}, 4},
        {{"","", "LGNM"}, 2},
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

    auto resource = analyzer.analyze("", test_pattern)[0];


    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {5, 32});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "array_parameter"}, array_value}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, struct_typed_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef struct packed {
                int field_a;
                int field_b;
            } my_struct_t;
            parameter my_struct_t struct_param = '{42, 17};
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("struct_param"));

    auto p = parameters.get("struct_param");
    HDL_struct_type check_struct;
    check_struct.packed = true;
    struct_member m;
    m.name = "field_a";
    auto t1 = Type_engine::create_primitive_type("int");
    m.type = t1;
    check_struct.member.emplace_back(m);
    m.name = "field_b";
    auto t2 = Type_engine::create_primitive_type("int");
    m.type = t2;
    check_struct.member.emplace_back(m);

    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    EXPECT_EQ(check_struct, p->get_type()->as<HDL_struct_type>());

    Concatenation c;
    c.add_component(std::make_shared<Token>("42", Token::number));
    c.add_component(std::make_shared<Token>("17", Token::number));
    EXPECT_TRUE(p->get_expression()->is<Concatenation>());
    EXPECT_EQ(p->get_expression()->as<Concatenation>(), c);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = {"","", "struct_param"};
    EXPECT_EQ(defaults[check_id], static_cast<uint64_t>(180388626449));
}


TEST(parameter_extraction, packed_struct_access_initialization) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef struct packed {
                int field_a;
                int field_b;
            } my_struct_t;
            parameter my_struct_t struct_param = '{42, 17};
            parameter integer struct_access_param = struct_param.field_b;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = {"","", "struct_access_param"};
    EXPECT_EQ(defaults[check_id], 17);
}



TEST(parameter_extraction, struct_unpacked_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef struct {
                int field_a;
                int field_b;
            } my_struct_t;
            parameter my_struct_t struct_param = '{42, 17};
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("struct_param"));

    auto p = parameters.get("struct_param");
    HDL_struct_type check_struct;
    struct_member m;
    m.name = "field_a";
    auto t1 = Type_engine::create_primitive_type("int");
    m.type = t1;
    check_struct.member.emplace_back(m);
    m.name = "field_b";
    auto t2 = Type_engine::create_primitive_type("int");
    m.type = t2;
    check_struct.member.emplace_back(m);

    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    EXPECT_EQ(check_struct, p->get_type()->as<HDL_struct_type>());

    Concatenation c;
    c.add_component(std::make_shared<Token>("42", Token::number));
    c.add_component(std::make_shared<Token>("17", Token::number));
    EXPECT_TRUE(p->get_expression()->is<Concatenation>());
    EXPECT_EQ(p->get_expression()->as<Concatenation>(), c);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {17, 42});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "struct_param"}, array_value},
        {{"","struct_param", "field_a"}, hdl_integer(42)},
        {{"","struct_param", "field_b"}, hdl_integer(17)}
    };
    EXPECT_EQ(defaults, check_defaults);
}




TEST(parameter_extraction, unpacked_struct_access_initialization) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef struct {
                int field_a;
                int field_b;
            } my_struct_t;
            parameter my_struct_t struct_param = '{12, 32};
            parameter integer struct_access_param = struct_param.field_b;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = {"","", "struct_access_param"};
    EXPECT_EQ(defaults[check_id], 32);

}

TEST(parameter_extraction, packed_struct_parametrized_member_width) {
    auto test_pattern = R"(
        module test_mod #(
            parameter WIDTH = 8
        )();
            typedef struct packed {
                int field_a;
                logic [WIDTH-1:0] field_b;
            } my_struct_t;
            parameter my_struct_t s = '{42, 17};
            parameter integer extracted = s.field_b;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    qualified_identifier s_id = {"","", "s"};
    ASSERT_TRUE(defaults.contains(s_id));
    EXPECT_EQ(defaults[s_id], static_cast<uint64_t>(10769));

    qualified_identifier ext_id = {"","", "extracted"};
    ASSERT_TRUE(defaults.contains(ext_id));
    EXPECT_EQ(defaults[ext_id], 17);
}

TEST(parameter_extraction, packed_struct_parametrized_member_width_wide) {
    auto test_pattern = R"(
        module test_mod #(
            parameter WIDTH = 16
        )();
            typedef struct packed {
                logic [WIDTH-1:0] field_a;
                logic [WIDTH-1:0] field_b;
            } my_struct_t;
            parameter my_struct_t s = '{1000, 50000};
            parameter integer extracted = s.field_a;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    qualified_identifier s_id = {"","", "s"};
    ASSERT_TRUE(defaults.contains(s_id));
    uint64_t expected = 65586000;
    EXPECT_EQ(defaults[s_id], expected);

    qualified_identifier ext_id = {"","", "extracted"};
    ASSERT_TRUE(defaults.contains(ext_id));
    EXPECT_EQ(defaults[ext_id], 1000);

    // Verify struct field entries in solved parameters
    qualified_identifier fa_id = {"","s", "field_a"};
    ASSERT_TRUE(defaults.contains(fa_id));
    EXPECT_EQ(defaults[fa_id], 1000);

    qualified_identifier fb_id = {"","s", "field_b"};
    ASSERT_TRUE(defaults.contains(fb_id));
    EXPECT_EQ(defaults[fb_id], 50000);
}


TEST(parameter_extraction, anonymous_packed_struct_typed_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            struct packed {
                int field_a;
                int field_b;
            } anon_struct = '{42, 17};
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("anon_struct"));

    auto p = parameters.get("anon_struct");
    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    auto &st = p->get_type()->as<HDL_struct_type>();
    EXPECT_TRUE(st.packed);
    ASSERT_EQ(st.member.size(), 2);
    EXPECT_EQ(st.member[0].name, "field_a");
    ASSERT_TRUE(st.member[0].type != nullptr);
    dimension_t check_dim;
    check_dim.packed = true;
    check_dim.first_bound = std::make_shared<Token>(31, 5);
    check_dim.second_bound = std::make_shared<Token>(0, 1);
    ASSERT_EQ(st.member[0].type->as<HDL_simple_type>().get_packed_dimensions()[0],check_dim);
    EXPECT_EQ(st.member[1].name, "field_b");
    ASSERT_TRUE(st.member[1].type != nullptr);
    ASSERT_EQ(st.member[1].type->as<HDL_simple_type>().get_packed_dimensions()[0],check_dim);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = {"","", "anon_struct"};
    EXPECT_EQ(defaults[check_id], static_cast<uint64_t>(180388626449));
}

TEST(parameter_extraction, anonymous_unpacked_struct_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            struct {
                int field_a;
                int field_b;
            } anon_struct =  '{42, 17};
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("anon_struct"));

    auto p = parameters.get("anon_struct");
    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    auto &st = p->get_type()->as<HDL_struct_type>();
    EXPECT_FALSE(st.packed);
    ASSERT_EQ(st.member.size(), 2);
    EXPECT_EQ(st.member[0].name, "field_a");
    ASSERT_TRUE(st.member[0].type != nullptr);
    EXPECT_EQ(st.member[1].name, "field_b");
    ASSERT_TRUE(st.member[1].type != nullptr);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {17, 42});
    qualified_identifier sid = {"","", "anon_struct"};
    EXPECT_EQ(defaults[sid], array_value);
}

