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
#include "data_model/HDL/parameters/components/Ternary.hpp"

using namespace std::string_literals;

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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

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

    p->set_name("concatenation");

    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Numeric_token>("31"), std::make_shared<Numeric_token>("0"), true});
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    Concatenation c;
    c.add_component(std::make_shared<Identifier_token>(qualified_identifier("simple_numeric_p")));
    c.add_component(std::make_shared<Identifier_token>(qualified_identifier("sv_numeric_p")));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));





    check_params.insert(p);


   ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        EXPECT_TRUE(check_params.contains(p->get_name()));
        EXPECT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {8, 32});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("simple_numeric_p"), 32},
        {qualified_identifier("sv_numeric_p"), 8},
        {qualified_identifier("concatenation"), av}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_array");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Numeric_token>("31"), std::make_shared<Numeric_token>("0"), true});
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    Concatenation c;
    c.set_default_init();
    c.add_component(std::make_shared<Numeric_token>("5"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));





    check_params.insert(p);


   ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        EXPECT_TRUE(check_params.contains(p->get_name()));
        EXPECT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {5, 5});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("test_array"), av}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

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

    p->set_name("concatenation");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Numeric_token>("31"), std::make_shared<Numeric_token>("0"), true});
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    Concatenation c;
    c.add_component(std::make_shared<Identifier_token>(qualified_identifier("simple_numeric_p")));
    c.add_component(std::make_shared<Identifier_token>(qualified_identifier("sv_numeric_p")));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));



    check_params.insert(p);


   ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {8,32});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("simple_numeric_p"), 32},
        {qualified_identifier("sv_numeric_p"), 8},
        {qualified_identifier("concatenation"), av}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter");



    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("31");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Numeric_token>("1");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = false;
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("32"));
    c.add_component(std::make_shared<Numeric_token>("5"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
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


TEST(parameter_extraction, array_parameter_ascending) {
    auto test_pattern = R"(
        module test_mod #(
            parameter [31:0] array_parameter [0:1] = '{32, 5}
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter");



    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("31");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Numeric_token>("0");
    d.second_bound = std::make_shared<Numeric_token>("1");
    d.packed = false;
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("32"));
    c.add_component(std::make_shared<Numeric_token>("5"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {32, 5});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("array_parameter"), array_value}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("serial_msb_out_first");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("0"));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("serial_lsb_out_first");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("1"));

    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("serial_msb_out_first"), 0},
        {qualified_identifier("serial_lsb_out_first"), 1},
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter");



    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("31");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Numeric_token>("1");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = false;
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("32"));
    c.add_component(std::make_shared<Numeric_token>("5"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    auto t = Identifier_token(qualified_identifier("array_parameter"));
    std::vector<std::shared_ptr<Expression_base>> ai;
    ai.push_back(std::make_shared<Numeric_token>("0"));
    t.set_array_index(ai);
    e.set_lhs(std::make_shared<Identifier_token>(t));
    t = Identifier_token(qualified_identifier("array_parameter"));
    ai.clear();
    ai.push_back(std::make_shared<Numeric_token>("1"));
    t.set_array_index(ai);
    e.set_rhs(std::make_shared<Identifier_token>(t));
    e.set_operation(Expression_v2::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {5, 32});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("array_parameter"), array_value},
        {qualified_identifier("array_parameter_expr_p"), 37},
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, simple_ascending_array_propagation) {
    auto test_pattern = R"(
        module test_mod #(
            parameter [31:0] array_parameter [0:1] = '{32, 5}
        )();
            parameter array_parameter_expr_p = array_parameter[0] - array_parameter[1];
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter");



    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("31");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Numeric_token>("0");
    d.second_bound = std::make_shared<Numeric_token>("1");
    d.packed = false;
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("32"));
    c.add_component(std::make_shared<Numeric_token>("5"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    auto t = Identifier_token(qualified_identifier("array_parameter"));
    std::vector<std::shared_ptr<Expression_base>> ai;
    ai.push_back(std::make_shared<Numeric_token>("0"));
    t.set_array_index(ai);
    e.set_lhs(std::make_shared<Identifier_token>(t));
    t = Identifier_token(qualified_identifier("array_parameter"));
    ai.clear();
    ai.push_back(std::make_shared<Numeric_token>("1"));
    t.set_array_index(ai);
    e.set_rhs(std::make_shared<Identifier_token>(t));
    e.set_operation(Expression_v2::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {32, 5});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("array_parameter"), array_value},
        {qualified_identifier("array_parameter_expr_p"), 27},
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter");



    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("31");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Numeric_token>("1");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = false;
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("32"));
    c.add_component(std::make_shared<Numeric_token>("5"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("array_parameter_expr_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    auto t = Identifier_token(qualified_identifier("array_parameter"));
    std::vector<std::shared_ptr<Expression_base>> ai;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("sv_numeric_p")));
    e.set_rhs(std::make_shared<Numeric_token>("0"));
    e.set_operation(Expression_v2::multiply);
    ai.push_back(std::make_shared<Expression_v2>(e));
    t.set_array_index(ai);
    Expression_v2 e2;
    e2.set_lhs(std::make_shared<Identifier_token>(t));
    t = Identifier_token(qualified_identifier("array_parameter"));
    ai.clear();
    ai.push_back(std::make_shared<Numeric_token>("1"));
    t.set_array_index(ai);
    e2.set_rhs(std::make_shared<Identifier_token>(t));
    e2.set_operation(Expression_v2::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e2));
    check_params.insert(p);
    p = std::make_shared<HDL_parameter>();

    p->set_name("sv_numeric_p");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("1"));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        EXPECT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {5, 32});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("array_parameter"), array_value},
        {qualified_identifier("array_parameter_expr_p"), 37},
        {qualified_identifier("sv_numeric_p"), 1},

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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_parameter");

    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    auto param_type = Type_engine::create_primitive_type("int");
    p->set_type(param_type);
    p->set_raw_value(std::make_shared<Concatenation>(c));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        EXPECT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("test_parameter"), 7}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_parameter");

    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    auto param_type = Type_engine::create_primitive_type("implicit");
    p->set_type(param_type);
    p->set_raw_value(std::make_shared<Concatenation>(c));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        EXPECT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("test_parameter"), 7}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_size");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("2"));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_parameter_1");


    auto param_type = Type_engine::create_primitive_type("int")->as<HDL_simple_type>();
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    Replication rep;
    rep.set_size(std::make_shared<Identifier_token>(qualified_identifier("repetition_size")));
    rep.set_item(std::make_shared<Numeric_token>("1"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Replication>(rep));




    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        EXPECT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    mdarray<hdl_integer> av;
    av.set_2d_slice({0}, {{1,1}});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("repetition_size"), 2},
        {qualified_identifier("repetition_parameter_1"), av}
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
            parameter int repetition_parameter_1  = {repetition_size{1}},
            parameter int repetition_parameter_2  = {repetition_size{1'b1}}
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_size");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("2"));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_parameter_1");

    Replication rep;
    auto size = std::make_shared<Identifier_token>(qualified_identifier("repetition_size"));
    rep.set_size(size);
    rep.set_item(std::make_shared<Numeric_token>("1"));
    auto param_type = Type_engine::create_primitive_type("int");
    p->set_type(param_type);
    p->set_raw_value(std::make_shared<Replication>(rep));




    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_parameter_2");

    Replication rep2;
    auto size_2 = std::make_shared<Identifier_token>(qualified_identifier("repetition_size"));
    rep2.set_size(size_2);
    rep2.set_item(std::make_shared<Numeric_token>("1'b1"));
    auto param_type_2 = Type_engine::create_primitive_type("int");
    p->set_type(param_type_2);
    p->set_raw_value(std::make_shared<Replication>(rep2));




    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("repetition_size"), 2},
        {qualified_identifier("repetition_parameter_1"), 1},
        {qualified_identifier("repetition_parameter_2"), 3}
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
            parameter int repetition_parameter_2 [1:0]  = '{repetition_size{4}},
            parameter int multi_repetition_parameter [3:0]  = {repetition_parameter_1,repetition_parameter_2},
            parameter int mixed_repetition_parameter [3:0]  = {1,2,repetition_parameter_2}
        )();

        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_size");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("2"));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_parameter_1");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    Replication r;
    auto size = std::make_shared<Identifier_token>(qualified_identifier("repetition_size"));

    r.set_size(size);
    r.set_item(std::make_shared<Numeric_token>("1"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Replication>(r));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("repetition_parameter_2");


    auto param_type_2 = Type_engine::create_primitive_type("int");
    param_type_2->as<HDL_simple_type>().add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    size = std::make_shared<Identifier_token>(qualified_identifier("repetition_size"));
    r.set_size(size);
    r.set_item(std::make_shared<Numeric_token>("4"));
    p->set_type(param_type_2);
    p->set_raw_value(std::make_shared<Replication>(r));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("multi_repetition_parameter");


    auto param_type_3 = Type_engine::create_primitive_type("int");
    param_type_3->as<HDL_simple_type>().add_dimension({
    std::make_shared<Numeric_token>("3"),
    std::make_shared<Numeric_token>("0"),
    false
    });
    Concatenation c;
    c.add_component(std::make_shared<Identifier_token>(qualified_identifier("repetition_parameter_1")));
    c.add_component( std::make_shared<Identifier_token>(qualified_identifier("repetition_parameter_2")));
    p->set_type(param_type_3);
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();

    p->set_name("mixed_repetition_parameter");


    auto param_type_4 = Type_engine::create_primitive_type("int");
    param_type_4->as<HDL_simple_type>().add_dimension({
    std::make_shared<Numeric_token>("3"),
    std::make_shared<Numeric_token>("0"),
    false
    });
    c = Concatenation();
    c.add_component(std::make_shared<Numeric_token>("1"));
    c.add_component( std::make_shared<Numeric_token>("2"));
    c.add_component( std::make_shared<Identifier_token>(qualified_identifier("repetition_parameter_2")));
    p->set_type(param_type_4);
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    mdarray<hdl_integer> av, av2, av3, av4;
    av.set_2d_slice({0}, {{1,1}});
    av2.set_2d_slice({0}, {{4,4}});
    av3.set_2d_slice({0}, {{4, 4, 1, 1}});
    av4.set_2d_slice({0}, {{4, 4, 2, 1}});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("repetition_size"), 2},
        {qualified_identifier("repetition_parameter_1"), av},
        {qualified_identifier("repetition_parameter_2"), av2},
        {qualified_identifier("multi_repetition_parameter"), av3},
        {qualified_identifier("mixed_repetition_parameter"), av4}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("packed_param");

    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("7");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = true;

    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));



    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});


    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("packed_param"), 169}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("packed_param");



    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true});
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    Concatenation c2;
    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c2.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c2.add_component(std::make_shared<Concatenation>(c));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c2));


    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {201, 169});
    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("packed_param"), av }
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("param_a");
    auto param_type = HDL_simple_type();
    param_type.add_dimension({
          std::make_shared<Numeric_token>("31"),
          std::make_shared<Numeric_token>("0"),
          true
      });
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));

    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("param_b");
    auto param_type_2 = HDL_simple_type();
    param_type_2.add_dimension({
    std::make_shared<Numeric_token>("5"),
    std::make_shared<Numeric_token>("0"),
    true
});
    Identifier_token t(qualified_identifier("param_a"));
    t.add_array_index(std::make_shared<Numeric_token>("3"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type_2));
    p->set_raw_value(std::make_shared<Identifier_token>(t));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {8, 32});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("param_a"), -1},
        {qualified_identifier("param_b"), 1}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("negative_array_param");
    auto param_type = Type_engine::create_primitive_type("implicit")->as<HDL_simple_type>();
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0")});
    Concatenation c;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("16'sd32767"));
    e.set_operation(Expression_v2::subtract);
    c.add_component(std::make_shared<Expression_v2>(e));
    c.add_component(std::make_shared<Numeric_token>("16'sd32767"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));

    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> av;
    hdl_integer a;
    a.set_value(32767);
    a.set_signed(true);
    hdl_integer b;
    b.set_value(-32767);
    b.set_signed(true);
    av.set_1d_slice({0, 0}, {a, b});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("negative_array_param"), av}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("expression_array_param");
    auto param_type = Type_engine::create_primitive_type("implicit")->as<HDL_simple_type>();
    param_type.add_dimension({ std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    Concatenation c;
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>("5"));
    e.set_rhs(std::make_shared<Numeric_token>("4"));
    e.set_operation(Expression_v2::add);
    c.add_component(std::make_shared<Expression_v2>(e));

    e.set_lhs(std::make_shared<Numeric_token>("7"));
    e.set_rhs(std::make_shared<Numeric_token>("6"));
    e.set_operation(Expression_v2::multiply);
    c.add_component(std::make_shared<Expression_v2>(e));

    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {42, 9});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("expression_array_param"), av}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("param_a");



    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true});
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    Concatenation c2;
    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c2.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c.add_component(std::make_shared<Numeric_token>("1'b0"));
    c.add_component(std::make_shared<Numeric_token>("1'b1"));
    c2.add_component(std::make_shared<Concatenation>(c));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c2));


    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("param_b");


    c2 = Concatenation();
    c = Concatenation();

    auto param_type_2 = HDL_simple_type();
    param_type_2.add_dimension({std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true});
    param_type_2.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"), false});
    Replication r;
    auto size = std::make_shared<Numeric_token>("8");
    r.set_size(size);
    r.set_item(std::make_shared<Numeric_token>("1'b1"));
    c2.add_component(std::make_shared<Replication>(r));
    r = Replication();
    size = std::make_shared<Numeric_token>("8");
    r.set_size(size);
    r.set_item(std::make_shared<Numeric_token>("1'b0"));
    c2.add_component(std::make_shared<Replication>(r));
    p->set_type(std::make_shared<HDL_simple_type>(param_type_2));
    p->set_raw_value(std::make_shared<Concatenation>(c2));




    check_params.insert(p);


    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> av, av2;
    av.set_1d_slice({0, 0}, {29, 226});
    av2.set_1d_slice({0, 0}, {0, 255});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("param_a"), av},
        {qualified_identifier("param_a"), av2}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    std::vector<std::pair<std::string, std::vector<std::string>>> vect_params = {
            {"", {}}
    };

    check_params.clear();

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("SS_POLARITY_DEFAULT");
    p->set_raw_value(std::make_shared<Numeric_token>("0"));
    p->set_type(Type_engine::create_primitive_type("implicit"));
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();

    p->set_name("FIXED_REGISTER_VALUES");



    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Numeric_token>("31"), std::make_shared<Numeric_token>("0"), true});
    param_type.add_dimension({std::make_shared<Numeric_token>("4"), std::make_shared<Numeric_token>("0"), false});
    Concatenation outer_c;
    outer_c.add_component(std::make_shared<Numeric_token>("3"));
    outer_c.add_component(std::make_shared<Numeric_token>("3"));
    outer_c.add_component(std::make_shared<Numeric_token>("3"));
    Concatenation c;
    c.add_component(std::make_shared<Identifier_token>(qualified_identifier("SS_POLARITY_DEFAULT")));
    c.add_component(std::make_shared<Numeric_token>("3'b0"));
    c.add_component(std::make_shared<Identifier_token>(qualified_identifier("SS_POLARITY_DEFAULT")));
    c.add_component(std::make_shared<Numeric_token>("5'b0"));
    c.add_component(std::make_shared<Numeric_token>("4'hE"));
    c.add_component(std::make_shared<Numeric_token>("4'b0"));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Numeric_token>("2'h2"));
    c.add_component(std::make_shared<Numeric_token>("2'b1"));
    c.add_component(std::make_shared<Numeric_token>("2'h3"));
    c.add_component(std::make_shared<Numeric_token>("4'hE"));
    c.add_component(std::make_shared<Numeric_token>("4'b0"));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(outer_c));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {0x27e0, 0xe0, 3 , 3, 3});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("SS_POLARITY_DEFAULT"), 0},
        {qualified_identifier("FIXED_REGISTER_VALUES"), av}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    check_params.clear();


    auto v1 = {
        Numeric_token("1'b1"),
        Numeric_token("1'b1"),
        Numeric_token("1'b1"),
        Numeric_token("1'b0"),
        Numeric_token("1'b0"),
        Numeric_token("1'b0"),
        Numeric_token("1'b1"),
        Numeric_token("1'b0")
    };
    auto v2 = {
                Numeric_token("1'b0"),
                Numeric_token("1'b0"),
                Numeric_token("1'b0"),
                Numeric_token("1'b1"),
                Numeric_token("1'b1"),
                Numeric_token("1'b1"),
                Numeric_token("1'b0"),
                Numeric_token("1'b1")
    };

    auto p = std::make_shared<HDL_parameter>();

    p->set_name("param_a");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true});
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"),false});
    param_type.add_dimension({std::make_shared<Numeric_token>("1"), std::make_shared<Numeric_token>("0"),false});
    Concatenation top_c, outer_c, inner_c;

    for(const auto& item:v1){
        inner_c.add_component(std::make_shared<Numeric_token>(item));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c  = Concatenation();
    for(const auto& item:v2){
        inner_c.add_component(std::make_shared<Numeric_token>(item));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    top_c.add_component(std::make_shared<Concatenation>(outer_c));
    outer_c = Concatenation();
    inner_c  = Concatenation();
    for(const auto& item:v2){
        inner_c.add_component(std::make_shared<Numeric_token>(item));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c  = Concatenation();
    for(const auto& item:v1){
        inner_c.add_component(std::make_shared<Numeric_token>(item));
    }
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    top_c.add_component(std::make_shared<Concatenation>(outer_c));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(top_c));


    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {226, 29});
    av.set_1d_slice({0, 1}, {29, 226});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("param_a"), av}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;


    auto p = std::make_shared<HDL_parameter>();

    p->set_name("test_parameter");


    auto param_type = HDL_simple_type();
    param_type.add_dimension({std::make_shared<Numeric_token>("4"), std::make_shared<Numeric_token>("0"), true});
    Replication r;
    auto size = std::make_shared<Numeric_token>("5");
    r.set_size(size);
    r.set_item(std::make_shared<Numeric_token>("1'b1"));
    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Replication>(r));




    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }

    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("test_parameter"), 31}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameter_statements();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("test_parameter");

    Concatenation c;

    dimension_t d;
    d.first_bound = std::make_shared<Numeric_token>("4");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = true;
    auto param_type = HDL_simple_type();
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Numeric_token>("2");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = false;
    param_type.add_dimension(d);
    d.first_bound = std::make_shared<Numeric_token>("1");
    d.second_bound = std::make_shared<Numeric_token>("0");
    d.packed = false;
    param_type.add_dimension(d);
    c.set_default_init();
    c.add_component(std::make_shared<Numeric_token>("3"));

    p->set_type(std::make_shared<HDL_simple_type>(param_type));
    p->set_raw_value(std::make_shared<Concatenation>(c));
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), parameters.size());

    for (const auto &p : parameters) {
        ASSERT_TRUE(check_params.contains(p->get_name()));
        ASSERT_EQ(*check_params.get(p->get_name()), *p);
    }
    auto defaults = parameter_solver::process_parameters(resource.get_parameter_statements(), {});
    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {3, 3});
    av.set_1d_slice({0, 1}, {3, 3});
    av.set_1d_slice({0, 2}, {3, 3});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("test_parameter"), av}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


