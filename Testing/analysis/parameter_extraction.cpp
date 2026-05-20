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
#include "data_model/HDL/parameters/HDL_parameter.hpp"


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
    p->add_component(Expression_component("'b001111", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("low_control_n");
    p->add_component(Expression_component("'b110000", Expression_component::number));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
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
    c.set_size(Expression({Expression_component("4", Expression_component::number)}));
    c.set_content(std::make_shared<Expression>(Expression({Expression_component("31'h100003", Expression_component::number)})));
    p->set_scalar(std::make_shared<Cast>(c));
    p->set_declared_type("integer");
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    ASSERT_EQ(3, std::get<int64_t>(defaults.at({"","", "TEST_PARAM"})));
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
    p->add_component(Expression_component(4, 2));
    p->set_declared_type("integer");
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    
    Cast c;
    c.set_size(Expression({
        Expression_component("(",Expression_component::parenthesis),
        Expression_component("SIZE", Expression_component::identifier),
        Expression_component(")",Expression_component::parenthesis)
    }));
    c.set_content(std::make_shared<Expression>(Expression({Expression_component("31'h100003", Expression_component::number)})));
    p->set_scalar(std::make_shared<Cast>(c));
    p->set_declared_type("integer");
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    ASSERT_EQ(3, std::get<int64_t>(defaults.at({"","", "TEST_PARAM"})));
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
    c.set_content(std::make_shared<Expression>(Expression({
        Expression_component("-", Expression_component::operation),
        Expression_component("5", Expression_component::number)
    })));
    dimension_t d;
    d.first_bound = {Expression_component("7", Expression_component::number)};
    d.second_bound =  {Expression_component("0", Expression_component::number)};
    d.packed = true;
    p->add_dimension(d);
    p->set_scalar(std::make_shared<Cast>(c));

    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();

    ASSERT_EQ(251, std::get<int64_t>(defaults.at({"","", "TEST_PARAM"})));
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
    inner_c.set_content(std::make_shared<Expression>(Expression({
        Expression_component("-", Expression_component::operation),
        Expression_component("5", Expression_component::number)
    })));

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    
    Cast outer_c;
    outer_c.set_size(Expression({
        Expression_component("(",Expression_component::parenthesis),
        Expression_component("NumLevels", Expression_component::identifier),
        Expression_component(")",Expression_component::parenthesis)
    }));
    outer_c.set_content(std::make_shared<Cast>(inner_c));
    dimension_t d;
    d.first_bound = {Expression_component("7", Expression_component::number)};
    d.second_bound =  {Expression_component("0", Expression_component::number)};
    d.packed = true;
    p->add_dimension(d);
    p->set_scalar(std::make_shared<Cast>(outer_c));
    
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("NumLevels");
    p->add_component(Expression_component(4, 2));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();

    ASSERT_EQ(11, std::get<int64_t>(defaults.at({"","", "TEST_PARAM"})));
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
    c.set_content(std::make_shared<Expression>(Expression({
        Expression_component("-", Expression_component::operation),
        Expression_component("5", Expression_component::number)
    })));
    dimension_t d;
    d.first_bound = {Expression_component("7", Expression_component::number)};
    d.second_bound =  {Expression_component("0", Expression_component::number)};
    d.packed = true;
    p->add_dimension(d);
    p->set_scalar(std::make_shared<Cast>(c));
    
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM_2");
    
    c = Cast();
    c.set_type_cast();
    c.set_target_type("int");
    c.set_content(std::make_shared<Expression>(Expression({
        Expression_component("2.5", Expression_component::number)
    })));
    p->set_scalar(std::make_shared<Cast>(c));
    
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();

    ASSERT_EQ(251, std::get<int64_t>(defaults.at({"","", "TEST_PARAM"})));
    ASSERT_EQ(3, std::get<int64_t>(defaults.at({"","", "TEST_PARAM_2"})));
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
    p->add_component(Expression_component("10ns", Expression_component::string));
    p->set_declared_type("integer");
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    ASSERT_EQ("10ns", std::get<std::string>(defaults.at({"","", "TEST_PARAM"})));
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
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("10'h0", Expression_component::number)})));
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("1'h1", Expression_component::number)})));
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("1'h0", Expression_component::number)})));
    Cast c;
    c.set_size(Expression({Expression_component("4", Expression_component::number)}));
    c.set_content(std::make_shared<Expression>(Expression({Expression_component("31'h100003", Expression_component::number)})));
    concat.add_component(std::make_shared<Cast>(c));
    p->add_item(std::make_shared<Concatenation>(concat));
    p->set_declared_type("integer");


    
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    ASSERT_EQ(35, std::get<int64_t>(defaults.at({"","", "TEST_PARAM"})));
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
    p->add_component(Expression_component("3", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TRANSLATION_TABLE_INIT");
    
    p->add_dimension({{Expression_component("3", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("\"FILE\"", Expression_component::string)})));
    p->add_item(std::make_shared<Concatenation>(c));
    p->set_default();

    
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
    mdarray<std::string> tti_val;
    tti_val.set_1d_slice({0, 0}, {"\"FILE\"", "\"FILE\"", "\"FILE\"", "\"FILE\""});

    ASSERT_EQ(3, std::get<int64_t>(defaults.at({"","", "N_CORES"})));
    ASSERT_EQ(tti_val, std::get<mdarray<std::string>>(defaults.at({"","", "TRANSLATION_TABLE_INIT"})));
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
    p->add_component(Expression_component("3", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TRANSLATION_TABLE_INIT");

    p->add_dimension({{Expression_component("3", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("\"FILE\"", Expression_component::string)})));
    p->add_item(std::make_shared<Concatenation>(c));
    p->set_default();
    
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("SEL");
    Expression_component e = Expression_component("TRANSLATION_TABLE_INIT", Expression_component::identifier);
    std::vector<Expression> ai;
    ai.push_back({Expression_component("2", Expression_component::number)});
    e.set_array_index(ai);
    p->add_component(e);
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
    mdarray<std::string> tti_val;
    tti_val.set_1d_slice({0, 0}, {"\"FILE\"", "\"FILE\"", "\"FILE\"", "\"FILE\""});

    ASSERT_EQ(3, std::get<int64_t>(defaults.at({"","", "N_CORES"})));
    ASSERT_EQ(tti_val, std::get<mdarray<std::string>>(defaults.at({"","", "TRANSLATION_TABLE_INIT"})));
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
    Concatenation c;
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("N_CORES");
    p->add_component(Expression_component("3", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("TRANSLATION_TABLE_INIT");
    
    p->add_dimension({
    {Expression({
        Expression_component("N_CORES", Expression_component::identifier),
        Expression_component("-", Expression_component::operation),
        Expression_component("1", Expression_component::number),
    })},
    {Expression({Expression_component("0", Expression_component::number)})},
        false
    });
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("\"FILE\"", Expression_component::string)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("\"FILE\"", Expression_component::string)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("\"FILE\"", Expression_component::string)})));
    p->add_item(std::make_shared<Concatenation>(c));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
    mdarray<std::string> tti_val;
    tti_val.set_1d_slice({0, 0}, {"\"FILE\"", "\"FILE\"", "\"FILE\""});

    ASSERT_EQ(3, std::get<int64_t>(defaults.at({"","", "N_CORES"})));
    ASSERT_EQ(tti_val, std::get<mdarray<std::string>>(defaults.at({"","", "TRANSLATION_TABLE_INIT"})));
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
    p->add_component(Expression_component("9", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("STEP");
    p->add_component(Expression_component("(", Expression_component::parenthesis));
    p->add_component(Expression_component("2", Expression_component::number));
    p->add_component(Expression_component("*", Expression_component::operation));
    p->add_component(Expression_component("3.14159265358979323846", Expression_component::number));
    p->add_component(Expression_component("/", Expression_component::operation));
    p->add_component(Expression_component("4.0", Expression_component::number));
    p->add_component(Expression_component(")", Expression_component::parenthesis));
    p->add_component(Expression_component("/", Expression_component::operation));
    p->add_component(Expression_component("LUT_DEPTH", Expression_component::identifier));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {{"","", "LUT_DEPTH"}, 9},
        {{"","", "STEP"}, 0.17453292519943295f}
    };

    ASSERT_EQ(9, std::get<int64_t>(defaults.at({"","", "LUT_DEPTH"})));
    ASSERT_FLOAT_EQ(0.17453292519943295, std::get<double>(defaults.at({"","", "STEP"})));
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
    HDL_function_call call("$rtoi");
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("16.8", Expression_component::number)})));
    
    p->set_scalar(std::make_shared<HDL_function_call>(call));
    
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
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
    HDL_function_call call("$rtoi");
    call.add_argument(std::make_shared<Expression>(Expression({
        Expression_component("14.8", Expression_component::number),
        Expression_component("+", Expression_component::operation),
        Expression_component("2", Expression_component::number)
    })));
    
    p->set_scalar(std::make_shared<HDL_function_call>(call));
    
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("CAST_2");
    call = HDL_function_call("$rtoi");
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("12.2", Expression_component::number)})));
    p->set_scalar(std::make_shared<HDL_function_call>(call));
    
    check_params.insert(p);
    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
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
        parameter PARAMETER_1 = 5;
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
    HDL_function_call call("$rtoi");
    call.add_argument(std::make_shared<Expression>(Expression({
        Expression_component("11.8", Expression_component::number),
        Expression_component("+", Expression_component::operation),
        Expression_component("PARAMETER_1", Expression_component::identifier)
    })));
    
    p->set_scalar(std::make_shared<HDL_function_call>(call));
    
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
    p->set_name("PARAMETER_1");
    p->add_component(Expression_component("5", Expression_component:: number));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
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
    p->add_component(Expression_component("46", Expression_component:: number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("CAST");
    auto inner_call = std::make_shared<HDL_function_call>("$ceil");
    Expression e;
    e.push_back(Expression_component("PARAMETER_1", Expression_component::identifier));
    e.push_back(Expression_component("/", Expression_component::operation));
    e.push_back(Expression_component("16.0", Expression_component::number));
    inner_call->add_argument(std::make_shared<Expression>(e));
    auto outer_call = std::make_shared<HDL_function_call>("$rtoi");
    outer_call->add_argument(inner_call);
    
    p->set_scalar(outer_call);
    
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
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
    p->add_component(Expression_component("32'h43c00000", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("timebase");
    p->add_component(Expression_component("bus_base", Expression_component::identifier));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("gpio");
    p->add_component(Expression_component("timebase" , Expression_component::identifier));
    p->add_component(Expression_component("+" , Expression_component::operation));
    p->add_component(Expression_component("32'h1000" , Expression_component::number));
    p->add_component(Expression_component("*" , Expression_component::operation));
    p->add_component(Expression_component("2" , Expression_component::number));
    p->add_component(Expression_component("/" , Expression_component::operation));
    p->add_component(Expression_component("2" , Expression_component::number));
    p->add_component(Expression_component("+" , Expression_component::operation));
    p->add_component(Expression_component("1" , Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("modulo_parameter");
    p->add_component(Expression_component("3" , Expression_component::number));
    p->add_component(Expression_component("%" , Expression_component::operation));
    p->add_component(Expression_component("2" , Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("subtraction_parameter");
    p->add_component(Expression_component("'o4" , Expression_component::number));
    p->add_component(Expression_component("-" , Expression_component::operation));
    p->add_component(Expression_component("'b10" , Expression_component::number));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }



    auto defaults = resource.get_default_parameters();
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
    p->add_component(Expression_component("32", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("local_p");
    p->add_component(Expression_component("74", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("sv_numeric_p");
    p->add_component(Expression_component("5'o10" , Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("dimensionless_sv_numeric_p");
    p->add_component(Expression_component("'h3F", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("string_p");
    p->add_component(Expression_component("\"423\"", Expression_component::string));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("nested_p");
    p->add_component(Expression_component("string_p", Expression_component::identifier));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {{"","", "simple_numeric_p"}, 32},
        {{"","", "local_p"}, 74},
        {{"","", "sv_numeric_p"}, 8},
        {{"","", "dimensionless_sv_numeric_p"}, 63},
        {{"","", "string_p"}, R"("423")"},
        {{"","", "nested_p"}, R"("423")"}
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
    p->add_component(Expression_component("32", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("sv_numeric_p");
    p->add_component(Expression_component("5'o10", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("dimensionless_sv_numeric_p");
    p->add_component(Expression_component("'h3F", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("add_expr_p");
    p->add_component(Expression_component("simple_numeric_p", Expression_component::identifier));
    p->add_component(Expression_component("+", Expression_component::operation));
    p->add_component(Expression_component("sv_numeric_p", Expression_component::identifier));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("sub_expr_p");
    p->add_component(Expression_component("simple_numeric_p", Expression_component::identifier));
    p->add_component(Expression_component("-", Expression_component::operation));
    p->add_component(Expression_component("sv_numeric_p", Expression_component::identifier));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("mul_expr_p");
    p->add_component(Expression_component("simple_numeric_p", Expression_component::identifier));
    p->add_component(Expression_component("*", Expression_component::operation));
    p->add_component(Expression_component("sv_numeric_p", Expression_component::identifier));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("div_expr_p");
    p->add_component(Expression_component("simple_numeric_p", Expression_component::identifier));
    p->add_component(Expression_component("/", Expression_component::operation));
    p->add_component(Expression_component("sv_numeric_p", Expression_component::identifier));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("modulo_expr_p");
    p->add_component(Expression_component("simple_numeric_p", Expression_component::identifier));
    p->add_component(Expression_component("%", Expression_component::operation));
    p->add_component(Expression_component("sv_numeric_p", Expression_component::identifier));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("chained_expression");
    p->add_component(Expression_component("add_expr_p", Expression_component::identifier));
    p->add_component(Expression_component("+", Expression_component::operation));
    p->add_component(Expression_component("mul_expr_p", Expression_component::identifier));
    p->add_component(Expression_component("*", Expression_component::operation));
    p->add_component(Expression_component("5", Expression_component::number));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("complex_log_expr_p");
    HDL_function_call call("$clog2");
    call.add_argument(std::make_shared<Expression>(Expression({
        Expression_component("add_expr_p", Expression_component::identifier),
        Expression_component("+", Expression_component::operation),
        Expression_component("2", Expression_component::number)
    })));
    
    p->set_scalar(std::make_shared<HDL_function_call>(call));
    
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("simple_log_expr_p");
    call = HDL_function_call("$clog2");
    call.add_argument(std::make_shared<Expression>(Expression({
        Expression_component("add_expr_p", Expression_component::identifier),
    })));
    p->set_scalar(std::make_shared<HDL_function_call>(call));
    
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("parenthesised_expr_p");
    p->add_component(Expression_component("(", Expression_component::parenthesis));
    p->add_component(Expression_component("add_expr_p", Expression_component::identifier));
    p->add_component(Expression_component("+", Expression_component::operation));
    p->add_component(Expression_component("mul_expr_p", Expression_component::identifier));
    p->add_component(Expression_component(")", Expression_component::parenthesis));
    p->add_component(Expression_component("*", Expression_component::operation));
    p->add_component(Expression_component("5", Expression_component::number));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
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


TEST(parameter_extraction, assay_assignment) {
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
    p->add_component(Expression_component("32", Expression_component::number));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
   
    p->set_name("sv_numeric_p");
    p->add_component(Expression_component("5'o10", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("concatenation");

    
    p->add_dimension({{Expression_component("31", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("simple_numeric_p", Expression_component::identifier)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("sv_numeric_p", Expression_component::identifier)})));
    p->add_item(std::make_shared<Concatenation>(c));

    



    check_params.insert(p);


   ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    mdarray<int64_t> av;
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

    
    p->add_dimension({{Expression_component("31", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("5", Expression_component::number)})));
    p->add_item(std::make_shared<Concatenation>(c));
    p->set_default();

    



    check_params.insert(p);


   ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        EXPECT_TRUE(parameters.contains(item->get_name()));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    mdarray<int64_t> av;
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
    p->add_component(Expression_component("32", Expression_component::number));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
   
    p->set_name("sv_numeric_p");
    p->add_component(Expression_component("5'o10", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("concatenation");

    
    p->add_dimension({{Expression_component("31", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});

    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("simple_numeric_p", Expression_component::identifier)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("sv_numeric_p", Expression_component::identifier)})));
    p->add_item(std::make_shared<Concatenation>(c));

    

    check_params.insert(p);


   ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    mdarray<int64_t> av;
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
    d.first_bound = {Expression_component("31", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = true;
    p->add_dimension(d);

    d.first_bound = {Expression_component("1", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = false;
    p->add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("32", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("5", Expression_component::number)})));
    p->add_item(std::make_shared<Concatenation>(c));
    

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> array_value;
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
    Expression e = {Expression_component("0", Expression_component::number)};
    p->set_scalar(std::make_shared<Expression>(e));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("serial_lsb_out_first");
    e = {Expression_component("1", Expression_component::number)};
    p->set_scalar(std::make_shared<Expression>(e));

    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
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
    d.first_bound = {Expression_component("31", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = true;
    p->add_dimension(d);

    d.first_bound = {Expression_component("1", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = false;
    p->add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("32", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("5", Expression_component::number)})));
    p->add_item(std::make_shared<Concatenation>(c));
    

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("array_parameter_expr_p");
    Expression_component e = Expression_component("array_parameter", Expression_component::identifier);
    std::vector<Expression> ai;
    ai.push_back(Expression({Expression_component("0", Expression_component::number)}));
    e.set_array_index(ai);
    p->add_component(e);
    p->add_component(Expression_component("+", Expression_component::operation));
    e = Expression_component("array_parameter", Expression_component::identifier);
    ai.clear();
    ai.push_back(Expression({Expression_component("1", Expression_component::number)}));
    e.set_array_index(ai);
    p->add_component(e);
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> array_value;
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
    d.first_bound = {Expression_component("31", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = true;
    p->add_dimension(d);

    d.first_bound = {Expression_component("1", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = false;
    p->add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("32", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("5", Expression_component::number)})));
    p->add_item(std::make_shared<Concatenation>(c));
    

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("array_parameter_expr_p");
    Expression_component e = Expression_component("array_parameter", Expression_component::identifier);
    std::vector<Expression> ai;
    ai.push_back({Expression_component("sv_numeric_p", Expression_component::identifier), Expression_component("*", Expression_component::operation), Expression_component("0", Expression_component::number)});
    e.set_array_index(ai);
    p->add_component(e);
    p->add_component(Expression_component("+", Expression_component::operation));
    e = Expression_component("array_parameter", Expression_component::identifier);
    ai.clear();
    ai.push_back(Expression({Expression_component("1", Expression_component::number)}));
    e.set_array_index(ai);
    p->add_component(e);
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
   
    p->set_name("sv_numeric_p");
    p->add_component(Expression_component("1", Expression_component::number));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> array_value;
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
/*
TEST(parameter_extraction, multidimensional_array_expression) {
    auto test_pattern = R"(
        module test_mod #(
            parameter repetition_size = 2,
            parameter [31:0] multidim_array_parameter [repetition_size-1:0][1:0] = '{{32,32}, {5,6}}
        )();
            parameter multidim_array_access = multidim_array_parameter[1][0];
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();
   
    p->set_name("repetition_size");
    p->add_component(Expression_component("2", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("multidim_array_parameter");


    
    p->add_dimension({{Expression_component("31", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    p->add_dimension({{Expression_component("repetition_size", Expression_component::identifier),Expression_component("-", Expression_component::operation), Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({{Expression_component("32", Expression_component::number)}})));
    c.add_component(std::make_shared<Expression>(Expression({{Expression_component("32", Expression_component::number)}})));
    p->add_item(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Expression>(Expression({{Expression_component("5", Expression_component::number)}})));
    c.add_component(std::make_shared<Expression>(Expression({{Expression_component("6", Expression_component::number)}})));
    p->add_item(std::make_shared<Concatenation>(c));

    


    check_params.insert(p);



    p = std::make_shared<HDL_parameter>();
   
    p->set_name("multidim_array_access");
    Expression_component ec = Expression_component("multidim_array_parameter", Expression_component::identifier);
    std::vector<Expression> ai;
    ai.push_back({Expression_component("1", Expression_component::number)});
    ai.push_back({Expression_component("0", Expression_component::number)});
    ec.set_array_index(ai);
    p->add_component(ec);
    check_params.insert(p);



    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }


    auto defaults = resource.get_default_parameters();

    mdarray<int64_t> av;
    av.set_2d_slice({0}, {{6,5}, {32,32}});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"", "repetition_size"}, 2},
        {{"", "multidim_array_parameter"}, av},
        {{"", "multidim_array_access"}, 32}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }

}
*/


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
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    p->add_item(std::make_shared<Concatenation>(c));
    p->set_declared_type("int");

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

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
    p->add_component(Expression_component("2", Expression_component::number));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("repetition_parameter_1");

    
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    Replication rep;
    rep.set_size({Expression_component("repetition_size", Expression_component::identifier)});
    rep.set_item(std::make_shared<Expression>(Expression({Expression_component("1", Expression_component::number)})));
    p->add_item(std::make_shared<Replication>(rep));
    p->set_declared_type("int");


    

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        EXPECT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    mdarray<int64_t> av;
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
    p->add_component(Expression_component("2", Expression_component::number));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("repetition_parameter_1");
    p->set_declared_type("int");
    
    Replication rep;
    rep.set_size({Expression_component("repetition_size", Expression_component::identifier)});
    rep.set_item(std::make_shared<Expression>(Expression({Expression_component("1", Expression_component::number)})));
    p->add_item(std::make_shared<Replication>(rep));


    

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

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
    p->add_component(Expression_component("2", Expression_component::number));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("repetition_parameter_1");

    
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    Replication r;
    r.set_size({Expression_component("repetition_size", Expression_component::identifier)});
    r.set_item(std::make_shared<Expression>(Expression({Expression_component("1", Expression_component::number)})));
    p->add_item(std::make_shared<Replication>(r));
    

    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("repetition_parameter_2");

    
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    r.set_size({Expression_component("repetition_size", Expression_component::identifier)});
    r.set_item(std::make_shared<Expression>(Expression({Expression_component("4", Expression_component::number)})));
    p->add_item(std::make_shared<Replication>(r));
    

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("multi_repetition_parameter");

    
    p->add_dimension({
    {Expression_component("3", Expression_component::number)},
    {Expression_component("0", Expression_component::number)},
    false
    });
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("repetition_parameter_1", Expression_component::identifier)})));
    c.add_component( std::make_shared<Expression>(Expression({Expression_component("repetition_parameter_2", Expression_component::identifier)})));
    p->add_item(std::make_shared<Concatenation>(c));
    

    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("mixed_repetition_parameter");

    
    p->add_dimension({
    {Expression_component("3", Expression_component::number)},
    {Expression_component("0", Expression_component::number)},
    false
    });
    c = Concatenation();
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1", Expression_component::number)})));
    c.add_component( std::make_shared<Expression>(Expression({Expression_component("2", Expression_component::number)})));
    c.add_component( std::make_shared<Expression>(Expression({Expression_component("repetition_parameter_2", Expression_component::identifier)})));
    p->add_item(std::make_shared<Concatenation>(c));
    

    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    mdarray<int64_t> av, av2, av3, av4;
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
    d.first_bound = {Expression_component("7", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = true;
    
    p->add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    p->add_item(std::make_shared<Concatenation>(c));

    

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& check_item:check_params){
        ASSERT_TRUE(parameters.contains(check_item->get_name()));
        ASSERT_EQ(*check_item, *parameters.get(check_item->get_name()));
    }

    auto defaults = resource.get_default_parameters();


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


    
    p->add_dimension({{Expression_component("7", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});


    Concatenation c2;
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c2.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation({});
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c2.add_component(std::make_shared<Concatenation>(c));
    p->add_item(std::make_shared<Concatenation>(c2));
    

    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& check_item:check_params){
        ASSERT_TRUE(parameters.contains(check_item->get_name()));
        ASSERT_EQ(*check_item, *parameters.get(check_item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    mdarray<int64_t> av;
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
    
    auto resource = analyzer.analyze("", test_pattern)[1];
    auto parameters = resource.get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
   
    p->set_name("package_param");
    Expression_component ec("bus_base", Expression_component::identifier);
    ec.set_package_prefix("test_package");
    p->add_component(ec);
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();


    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "package_param"}, "__RUNTIME_ONLY_PARAMETER__"}
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
    Expression_component ec("DATA_WIDTH", Expression_component::identifier);
    ec.set_package_prefix("test_interface");
    p->add_component(ec);
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
    p->add_component(Expression_component("-", Expression_component::operation));
    p->add_component(Expression_component("16'sd32767", Expression_component::number));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> av;
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
    p->add_dimension({
          {Expression_component("31", Expression_component::number)},
          {Expression_component("0", Expression_component::number)},
          true
      });
    p->set_scalar(std::make_shared<Expression>(Expression({
        Expression_component("-", Expression_component::operation),
        Expression_component("1", Expression_component::number) })));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("param_b");
    p->add_dimension({
    {Expression_component("5", Expression_component::number)},
    {Expression_component("0", Expression_component::number)},
    true
});
    Expression_component ec("param_a", Expression_component::identifier);
    ec.add_array_index({Expression_component("3", Expression_component::number)});
    p->set_scalar(std::make_shared<Expression>(Expression({ec})));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> av;
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

    
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}});


    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("-", Expression_component::operation),Expression_component("16'sd32767", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("16'sd32767", Expression_component::number)})));
    p->add_item(std::make_shared<Concatenation>(c));


    


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> av;
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


    
    p->add_dimension({ {Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});

    Concatenation c;
    c.add_component(std::make_shared<Expression>(
        Expression({
            Expression_component("5", Expression_component::number),
            Expression_component("+", Expression_component::operation),
            Expression_component("4", Expression_component::number)
        })
    ));
    c.add_component(std::make_shared<Expression>(
        Expression({
            Expression_component("7", Expression_component::number),
            Expression_component("*", Expression_component::operation),
            Expression_component("6", Expression_component::number)
        })
    ));
    p->add_item(std::make_shared<Concatenation>(c));


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> av;
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


    
    p->add_dimension({{Expression_component("7", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});


    Concatenation c2;
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c2.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c2.add_component(std::make_shared<Concatenation>(c));
    p->add_item(std::make_shared<Concatenation>(c2));
    

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("param_b");


    c2 = Concatenation();
    c = Concatenation();

    p->add_dimension({{Expression_component("7", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});

    Replication r;
    r.set_size({Expression_component("8", Expression_component::number)});
    r.set_item(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    c2.add_component(std::make_shared<Replication>(r));
    r = Replication();
    r.set_size({Expression_component("8", Expression_component::number)});
    r.set_item(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    c2.add_component(std::make_shared<Replication>(r));
    p->set_scalar(std::make_shared<Concatenation>(c2));
    



    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> av, av2;
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
    p->add_component(Expression_component("4", Expression_component::number));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), def_parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(def_parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *def_parameters.get(item->get_name()));
    }

    check_params.clear();

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("param_1");
    p->add_component(Expression_component("test_param", Expression_component::identifier));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
   
    p->set_name("param_2");
    p->add_component(Expression_component("test_param", Expression_component::identifier));
    p->add_component(Expression_component("+", Expression_component::operation));
    p->add_component(Expression_component("5", Expression_component::number));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();
   
    p->set_name("param_3");
    p->add_component(Expression_component("(", Expression_component::parenthesis));
    p->add_component(Expression_component("test_param", Expression_component::identifier));
    p->add_component(Expression_component("+", Expression_component::operation));
    p->add_component(Expression_component("7", Expression_component::number));
    p->add_component(Expression_component(")", Expression_component::parenthesis));
    p->add_component(Expression_component("*", Expression_component::operation));
    p->add_component(Expression_component("1", Expression_component::number));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), inst_parameters.size());

    for(const auto& item:check_params){
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
    p->add_component(Expression_component("0", Expression_component::number));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
   
    p->set_name("FIXED_REGISTER_VALUES");


    
    p->add_dimension({{Expression_component("31", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    p->add_dimension({{Expression_component("4", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, false});
    Concatenation outer_c;
    outer_c.add_component(std::make_shared<Expression>(Expression({Expression_component("3", Expression_component::number)})));
    outer_c.add_component(std::make_shared<Expression>(Expression({Expression_component("3", Expression_component::number)})));
    outer_c.add_component(std::make_shared<Expression>(Expression({Expression_component("3", Expression_component::number)})));
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("SS_POLARITY_DEFAULT", Expression_component::identifier)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("3'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("SS_POLARITY_DEFAULT", Expression_component::identifier)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("5'b0", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("4'hE", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("4'b0", Expression_component::number)})));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("2'h2", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("2'b1", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("2'h3", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("4'hE", Expression_component::number)})));
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("4'b0", Expression_component::number)})));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    p->add_item(std::make_shared<Concatenation>(outer_c));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> av;
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


    Expression v1 = {
        Expression_component("1'b1", Expression_component::number),
        Expression_component("1'b1", Expression_component::number),
        Expression_component("1'b1", Expression_component::number),
        Expression_component("1'b0", Expression_component::number),
        Expression_component("1'b0", Expression_component::number),
        Expression_component("1'b0", Expression_component::number),
        Expression_component("1'b1", Expression_component::number),
        Expression_component("1'b0", Expression_component::number)
    };
    Expression v2 = {
                Expression_component("1'b0", Expression_component::number),
                Expression_component("1'b0", Expression_component::number),
                Expression_component("1'b0", Expression_component::number),
                Expression_component("1'b1", Expression_component::number),
                Expression_component("1'b1", Expression_component::number),
                Expression_component("1'b1", Expression_component::number),
                Expression_component("1'b0", Expression_component::number),
                Expression_component("1'b1", Expression_component::number)
    };

    auto p = std::make_shared<HDL_parameter>();
   
    p->set_name("param_a");

    
    p->add_dimension({{Expression_component("7", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)},false});
    p->add_dimension({{Expression_component("1", Expression_component::number)}, {Expression_component("0", Expression_component::number)},false});
    Concatenation top_c, outer_c, inner_c;

    for(const auto& item:v1.components){
        inner_c.add_component(std::make_shared<Expression>(Expression({item})));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c  = Concatenation();
    for(const auto& item:v2.components){
        inner_c.add_component(std::make_shared<Expression>(Expression({item})));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    top_c.add_component(std::make_shared<Concatenation>(outer_c));
    outer_c = Concatenation();
    inner_c  = Concatenation();
    for(const auto& item:v2.components){
        inner_c.add_component(std::make_shared<Expression>(Expression({item})));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c  = Concatenation();
    for(const auto& item:v1.components){
        inner_c.add_component(std::make_shared<Expression>(Expression({item})));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    top_c.add_component(std::make_shared<Concatenation>(outer_c));
    p->add_item(std::make_shared<Concatenation>(top_c));
    

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> av;
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

    
    p->add_dimension({{Expression_component("4", Expression_component::number)}, {Expression_component("0", Expression_component::number)}, true});
    Replication r;
    r.set_size({Expression_component("5", Expression_component::number)});
    r.set_item(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    p->add_item(std::make_shared<Replication>(r));

    


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

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
    d.first_bound = {Expression_component("4", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = true;
    p->add_dimension(d);
    d.first_bound = {Expression_component("2", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = false;
    p->add_dimension(d);
    d.first_bound = {Expression_component("1", Expression_component::number)};
    d.second_bound = {Expression_component("0", Expression_component::number)};
    d.packed = false;
    p->add_dimension(d);
    c.add_component(std::make_shared<Expression>(Expression({Expression_component("3", Expression_component::number)})));
    p->set_default();
    p->add_item(std::make_shared<Concatenation>(c));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }
    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> av;
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
    p.add_dimension({
         {
             Expression_component("ADDR_WIDTH", Expression_component::identifier),
             Expression_component("-", Expression_component::operation),
             Expression_component("1", Expression_component::number)
         },
        {Expression_component("0", Expression_component::number)},
        true
    });
    p.set_scalar(std::make_shared<Expression>(Expression({Expression_component("CTRL_ADDR_CALC", Expression_component::identifier)})));
    HDL_function_call call("CTRL_ADDR_CALC");
    assignment a("CTRL_ADDR_CALC", std::nullopt, std::make_shared<Expression>(Expression({Expression_component("100", Expression_component::number)})));
    call.add_body({a},std::nullopt);
    p.set_scalar(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    auto defaults = resource.get_default_parameters();

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
    p.set_declared_type("integer");
    p.add_component(Expression_component("CTRL_ADDR_CALC", Expression_component::identifier));
    HDL_function_call call("get_axis_metadata");
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("11", Expression_component::number)})));
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    assignment a("get_axis_metadata", std::nullopt, nullptr);

    Concatenation concat;
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("10'h0", Expression_component::number)})));
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));

    a.set_value(std::make_shared<Concatenation>(concat));
    call.add_body({a},std::nullopt);
    p.set_scalar(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    auto defaults = resource.get_default_parameters();

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
    p.set_declared_type("integer");
    p.add_component(Expression_component("CTRL_ADDR_CALC", Expression_component::identifier));
    HDL_function_call call("get_axis_metadata");
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("11", Expression_component::number)})));
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    assignment a("get_axis_metadata", std::nullopt, nullptr);
    Cast c;
    c.set_size(Expression({Expression_component("4", Expression_component::number)}));
    c.set_content(std::make_shared<Expression>(Expression({
        Expression_component("11", Expression_component::number),
        Expression_component("-", Expression_component::operation),
        Expression_component("8", Expression_component::number)
    })));
    Replication repl;
    repl.set_size({Expression_component("4", Expression_component::number)});
    repl.set_item(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    a.set_value(std::make_shared<Replication>(repl));
    call.add_body({a},std::nullopt);
    p.set_scalar(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    auto defaults = resource.get_default_parameters();

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "TEST_PARAM"}, 15}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
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
    p.set_declared_type("integer");
    p.add_component(Expression_component("CTRL_ADDR_CALC", Expression_component::identifier));
    HDL_function_call call("get_axis_metadata");
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("11", Expression_component::number)})));
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    assignment a("get_axis_metadata", std::nullopt, nullptr);
    Cast c;
    c.set_size(Expression({Expression_component("4", Expression_component::number)}));
    c.set_content(std::make_shared<Expression>(Expression({
        Expression_component("11", Expression_component::number),
        Expression_component("-", Expression_component::operation),
        Expression_component("8", Expression_component::number)
    })));
    Concatenation concat;
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("10'h0", Expression_component::number)})));
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b0", Expression_component::number)})));
    concat.add_component(std::make_shared<Expression>(Expression({Expression_component("1'b1", Expression_component::number)})));
    concat.add_component(std::make_shared<Cast>(c));

    a.set_value(std::make_shared<Concatenation>(concat));
    call.add_body({a},std::nullopt);
    p.set_scalar(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    auto defaults = resource.get_default_parameters();

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

            function logic [31:0] CTRL_ADDR_CALC();
                for(int i = 0; i<3; i++)begin
                    CTRL_ADDR_CALC[i] = 100*i;
                end
            endfunction

            parameter [31:0] TEST_PARAM = CTRL_ADDR_CALC();
        endmodule
    )";


    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto param = resource.get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    p.add_dimension({
         {
             Expression_component("31", Expression_component::number)
         },
        {Expression_component("0", Expression_component::number)},
        true
    });
    p.set_scalar(std::make_shared<Expression>(Expression({Expression_component("CTRL_ADDR_CALC", Expression_component::identifier)})));
    HDL_function_call call("CTRL_ADDR_CALC");
    HDL_loop_metadata loop;
    HDL_parameter idx;
    idx.set_name("i");
    idx.add_component(Expression_component("0", Expression_component::number));
    loop.set_init(idx);
    loop.set_end_c({
        Expression_component("i", Expression_component::identifier),
        Expression_component("<", Expression_component::operation),
        Expression_component("3", Expression_component::number),
    });
    loop.set_iter({
        Expression_component("i", Expression_component::identifier),
        Expression_component("+", Expression_component::operation),
        Expression_component("1", Expression_component::number),
    });
    assignment a(
    "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("i", Expression_component::identifier)})),
        std::make_shared<Expression>(Expression({
            Expression_component("100", Expression_component::number),
            Expression_component("*", Expression_component::operation),
            Expression_component("i", Expression_component::identifier)
        }))
        );
    loop.add_assignment(a);
    call.add_body({},loop);
    p.set_scalar(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    auto defaults = resource.get_default_parameters();

    mdarray<int64_t> av;
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

            function logic [31:0] CTRL_ADDR_CALC();
                for(int i = 0; i<N_CHAINS; i++)begin
                    CTRL_ADDR_CALC[i] = OFFSET*i;
                end
            endfunction

            parameter [31:0] TEST_PARAM = CTRL_ADDR_CALC();
        endmodule
    )";


    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto param = resource.get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    
    p.add_component(Expression_component("CTRL_ADDR_CALC", Expression_component::identifier));
    HDL_function_call call("CTRL_ADDR_CALC");
    HDL_loop_metadata loop;
    HDL_parameter idx;
    idx.set_name("i");
    idx.add_component(Expression_component("0", Expression_component::number));
    loop.set_init(idx);
    loop.set_end_c({
        Expression_component("i", Expression_component::identifier),
        Expression_component("<", Expression_component::operation),
        Expression_component("N_CHAINS", Expression_component::identifier),
    });
    loop.set_iter({
        Expression_component("i", Expression_component::identifier),
        Expression_component("+", Expression_component::operation),
        Expression_component("1", Expression_component::number),
    });
    assignment a(
    "CTRL_ADDR_CALC",
        std::make_shared<Expression>(Expression({Expression_component("i", Expression_component::identifier)})),
        std::make_shared<Expression>(Expression({
            Expression_component("OFFSET", Expression_component::identifier),
            Expression_component("*", Expression_component::operation),
            Expression_component("i", Expression_component::identifier)
        }))
        );
    loop.add_assignment(a);
    call.add_body({},loop);
    p.add_dimension({
         {
             Expression_component("31", Expression_component::number)
         },
        {Expression_component("0", Expression_component::number)},
        true
    });
    p.set_scalar(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    auto defaults = resource.get_default_parameters();

    mdarray<int64_t> av;
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
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("5", Expression_component::component_type::number)})));
    call.add_argument(std::make_shared<Expression>(Expression({Expression_component("7", Expression_component::component_type::number)})));
    call.add_assignment({"add",std::nullopt, std::make_shared<Expression>(Expression({
        Expression_component(5, 3),
        Expression_component("+", Expression_component::operation),
        Expression_component(7, 3),
    })) });
    p.add_dimension({
        {
            Expression_component("31", Expression_component::number)
        },
       {Expression_component("0", Expression_component::number)},
       true
   });
    p.set_scalar(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    auto defaults = resource.get_default_parameters();


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
    check_param->add_component({Expression_component("DECIMATE", Expression_component::identifier)});

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
   
    p->add_component(Expression_component("32", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("USER_WIDTH");
   
    p->add_component(Expression_component("24", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("DEST_WIDTH");
   
    p->add_component(Expression_component("8", Expression_component::number));
    check_params.insert(p);

    ASSERT_EQ(check_params, parameters);
    auto defaults = resource.get_default_parameters();
    mdarray<int64_t> av;
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
    
    p.add_component(Expression_component("0", Expression_component::number));

    check_loop.set_init(p);
    check_loop.set_end_c({Expression_component("n", Expression_component::identifier), Expression_component("<", Expression_component::operation), Expression_component("N_REPETITIONS", Expression_component::identifier)});
    check_loop.set_iter({Expression_component("n", Expression_component::identifier), Expression_component("+", Expression_component::operation), Expression_component("1", Expression_component::number)});


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
    p->add_component(Expression_component("2", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("test_positive");
    Ternary t;
    t.set_condition(Expression({
        Expression_component("condition", Expression_component::identifier),
        Expression_component(">", Expression_component::operation),
        Expression_component("1", Expression_component::number),
    }));
    t.set_true_value(
        std::make_shared<Expression>(Expression({Expression_component("12", Expression_component::number)}))
        );

    t.set_false_value(
        std::make_shared<Expression>(Expression({Expression_component("34", Expression_component::number)}))
        );
    p->set_scalar(std::make_shared<Ternary>(t));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("test_negative");
    t = Ternary();
    t.set_condition(Expression({
        Expression_component("condition", Expression_component::identifier),
        Expression_component(">", Expression_component::operation),
        Expression_component("65", Expression_component::number),
    }));
    t.set_true_value(
        std::make_shared<Expression>(Expression({Expression_component("12", Expression_component::number)}))
        );

    t.set_false_value(
        std::make_shared<Expression>(Expression({Expression_component("34", Expression_component::number)}))
        );
    p->set_scalar(std::make_shared<Ternary>(t));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

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
    p->add_component(Expression_component("2", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("test_positive");
    Ternary t;
    t.set_condition(Expression({
        Expression_component("condition", Expression_component::identifier),
        Expression_component(">", Expression_component::operation),
        Expression_component("1", Expression_component::number),
    }));
    t.set_false_value(
        std::make_shared<Expression>(Expression({Expression_component("34", Expression_component::number)}))
        );

    Ternary inner_t;
    inner_t.set_condition(Expression({
        Expression_component("condition", Expression_component::identifier),
        Expression_component(">", Expression_component::operation),
        Expression_component("65", Expression_component::number),
    }));
    inner_t.set_true_value(
        std::make_shared<Expression>(Expression({Expression_component("12", Expression_component::number)}))
        );

    inner_t.set_false_value(
        std::make_shared<Expression>(Expression({Expression_component("96", Expression_component::number)}))
        );
    t.set_true_value(std::make_shared<Ternary>(inner_t));
    p->set_scalar(std::make_shared<Ternary>(t));
    check_params.insert(p);



    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

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
    p->add_component(Expression_component("4", Expression_component::number));
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
   
    p->set_name("LGNM");
    Ternary t;
    t.set_condition(Expression({
        Expression_component("NM", Expression_component::identifier),
        Expression_component(">", Expression_component::operation),
        Expression_component("1", Expression_component::number),
    }));
    HDL_function_call c("$clog2");
    c.add_argument(std::make_shared<Expression>(Expression({Expression_component("NM", Expression_component::identifier)})));
    t.set_true_value(std::make_shared<HDL_function_call>(c));
    t.set_false_value(
        std::make_shared<Expression>(Expression({Expression_component("1", Expression_component::number)}))
        );

    p->set_scalar(std::make_shared<Ternary>(t));
    check_params.insert(p);



    ASSERT_EQ(check_params.size(), parameters.size());

    for(const auto& item:check_params){
        ASSERT_TRUE(parameters.contains(item->get_name()));
        ASSERT_EQ(*item, *parameters.get(item->get_name()));
    }

    auto defaults = resource.get_default_parameters();

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {{"","", "NM"}, 4},
        {{"","", "LGNM"}, 2},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}