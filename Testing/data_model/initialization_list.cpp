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

#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "frontend/analysis/sv_analyzer.hpp"
#include "analysis/parameter_solver.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"


TEST(Initialization_list, get_values_1d_unpacked)  {


    HDL_parameter p("param");
    HDL_simple_type param_type;

    param_type.add_dimension({
        Expression(Expression_component("4", Expression_component::number)),
        Expression(Expression_component("0", Expression_component::number)),
    false});
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("5", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("3", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("4", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("6", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("69", Expression_component::number))));
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(c));

    mdarray<hdl_integer> check_array;
    check_array.set_1d_slice({0, 0}, {69, 6, 4 , 3, 5});

    auto res = p.evaluate({});
    ASSERT_TRUE(res.has_value());
    auto values = res.value().get_int_array();
    ASSERT_EQ(values, check_array);
}


TEST(Initialization_list, get_values_2d_unpacked) {


    HDL_parameter p("param");
    HDL_simple_type param_type;

    param_type.add_dimension({
        Expression(Expression_component("2", Expression_component::number)),
        Expression(Expression_component("0", Expression_component::number)),
        false
    });
    param_type.add_dimension({
        Expression(Expression_component("1", Expression_component::number)),
        Expression(Expression_component("0", Expression_component::number)),
        false
    });
    Concatenation outer_c;
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("5", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("3", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("4", Expression_component::number))));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("6", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("69", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("54", Expression_component::number))));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(outer_c));


    mdarray<hdl_integer> check_array;
    check_array.set_2d_slice({0, 0}, {{54,69,6},{4,3,5}});

    auto res = p.evaluate({});
    ASSERT_TRUE(res.has_value());
    auto values = res.value().get_int_array();
    ASSERT_EQ(values, check_array);

}


TEST(Initialization_list, get_values_3d_unpacked) {

    HDL_parameter p("param");
    HDL_simple_type param_type;

    param_type.add_dimension({
        Expression(Expression_component("2", Expression_component::number)),
        Expression(Expression_component("0", Expression_component::number)),
            false
    });
    param_type.add_dimension({
        Expression(Expression_component("1", Expression_component::number)),
        Expression(Expression_component("0", Expression_component::number)),
        false
    });
    param_type.add_dimension({
        Expression(Expression_component("1", Expression_component::number)),
        Expression(Expression_component("0", Expression_component::number)),
        false
    });

    Concatenation c_inner, c_outer, c_top;
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("5", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("3", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("4", Expression_component::number))));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_inner = Concatenation();
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("6", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("69", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("54", Expression_component::number))));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_top.add_component(std::make_shared<Concatenation>(c_outer));
    c_inner = Concatenation();
    c_outer = Concatenation();
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("57", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("13", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("24", Expression_component::number))));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_inner = Concatenation();
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("43", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("82", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("11", Expression_component::number))));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_top.add_component(std::make_shared<Concatenation>(c_outer));

    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(c_top));



    mdarray<hdl_integer> check_array;
    check_array.set_data({{{11,82,43},{24,13,57}},{{54,69,6},{4,3,5}}});

    auto res = p.evaluate({});
    ASSERT_TRUE(res.has_value());
    auto values = res.value().get_int_array();
    EXPECT_EQ(values, check_array);

}

TEST(Initialization_list, packed_concatenation) {

    HDL_parameter p("param");
    HDL_simple_type param_type;

    param_type.add_dimension({
        Expression_component("7", Expression_component::number),
        Expression_component("0", Expression_component::number),
        true
    });
    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));

    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(c));
    auto res = p.evaluate({});


    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(169, res.value().get_integer());

}



TEST(Initialization_list, get_values_1d_packed) {


    HDL_parameter p("param");
    HDL_simple_type param_type;
    param_type.add_dimension({
        Expression_component("2", Expression_component::number),
        Expression_component("0", Expression_component::number),
        true
    });

    param_type.add_dimension({
        Expression_component("4", Expression_component::number),
        Expression_component("0", Expression_component::number),
        false
    });

    Concatenation inner_c, outer_c;
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c = Concatenation();
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c = Concatenation();
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c = Concatenation();
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    inner_c = Concatenation();
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    inner_c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(outer_c));


    mdarray<hdl_integer> check_array;
    check_array.set_1d_slice({0, 0}, {1, 6, 5, 2, 5});

    auto res = p.evaluate({});
    ASSERT_TRUE(res.has_value());
    auto values = res.value().get_int_array();
    ASSERT_EQ(check_array, values);


}

TEST(Initialization_list, get_values_2d_packed) {


    HDL_parameter p("param");
    HDL_simple_type param_type;
    param_type.add_dimension({
        Expression_component("2", Expression_component::number),
        Expression_component("0", Expression_component::number),
        true
    });

    param_type.add_dimension({
        Expression_component("1", Expression_component::number),
        Expression_component("0", Expression_component::number),
        false
    });


    param_type.add_dimension({
        Expression_component("1", Expression_component::number),
        Expression_component("0", Expression_component::number),
        false
    });



    Concatenation c_inner, c_outer, c_top;
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_inner = Concatenation();
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_top.add_component(std::make_shared<Concatenation>(c_outer));
    c_inner = Concatenation();
    c_outer = Concatenation();
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_inner = Concatenation();
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_inner.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_top.add_component(std::make_shared<Concatenation>(c_outer));
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(c_top));


    mdarray<hdl_integer> check_array;
    check_array.set_2d_slice({0}, {{6, 5}, {2, 5}});

    auto res = p.evaluate({});
    ASSERT_TRUE(res.has_value());
    auto values = res.value().get_int_array();

    ASSERT_EQ(check_array, values);

}

TEST(Initialization_list, get_values_3d_packed) {


    HDL_parameter p("param");
    HDL_simple_type param_type;
    param_type.add_dimension({
        Expression_component("1", Expression_component::number),
        Expression_component("0", Expression_component::number), true
        });

    param_type.add_dimension({
        Expression_component("1", Expression_component::number),
        Expression_component("0", Expression_component::number),
        false
    });


    param_type.add_dimension({
        Expression_component("1", Expression_component::number),
        Expression_component("0", Expression_component::number),
        false
    });

    param_type.add_dimension({
        Expression_component("1", Expression_component::number),
        Expression_component("0", Expression_component::number),
        false
    });


    Concatenation c_pack, c_inner, c_outer,c_top;
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_inner.add_component(std::make_shared<Concatenation>(c_pack));
    c_pack = Concatenation();
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_inner.add_component(std::make_shared<Concatenation>(c_pack));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_pack = Concatenation();
    c_inner = Concatenation();
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_inner.add_component(std::make_shared<Concatenation>(c_pack));
    c_pack = Concatenation();
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_inner.add_component(std::make_shared<Concatenation>(c_pack));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_top.add_component(std::make_shared<Concatenation>(c_outer));
    c_pack = Concatenation();
    c_inner = Concatenation();
    c_outer = Concatenation();
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_inner.add_component(std::make_shared<Concatenation>(c_pack));
    c_pack = Concatenation();
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_inner.add_component(std::make_shared<Concatenation>(c_pack));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_pack = Concatenation();
    c_inner = Concatenation();
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_inner.add_component(std::make_shared<Concatenation>(c_pack));
    c_pack = Concatenation();
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b0", Expression_component::number))));
    c_pack.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c_inner.add_component(std::make_shared<Concatenation>(c_pack));
    c_outer.add_component(std::make_shared<Concatenation>(c_inner));
    c_top.add_component(std::make_shared<Concatenation>(c_outer));
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(c_top));




    mdarray<hdl_integer> check_array;
    check_array.set_data(
    {
            {
                {1, 2},
                {0, 3}
            },
            {
                {3, 0},
                {2, 1}
            }
        }
    );

    auto res = p.evaluate({});
    ASSERT_TRUE(res.has_value());
    auto values = res.value().get_int_array();
    ASSERT_EQ(check_array, values);



}

TEST(Initialization_list, get_values_concatenation_initialization) {


    HDL_parameter p("param");
    HDL_simple_type param_type;
    param_type.add_dimension({
        Expression_component("31", Expression_component::number),
        Expression_component("0", Expression_component::number),
        true
    });
    param_type.add_dimension({
        Expression_component("1", Expression_component::number),
        Expression_component("0", Expression_component::number),
        false
    });

    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("31", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("43", Expression_component::number))));
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(c));

    mdarray<hdl_integer> check_array;
    check_array.set_1d_slice({0, 0}, {43, 31});

    auto res = p.evaluate({});
    ASSERT_TRUE(res.has_value());
    auto values = res.value().get_int_array();

    ASSERT_EQ(check_array, values);

}

TEST(Initialization_list, get_values_1d_mixed_packed_unpacked) {
    HDL_parameter p("param");
    HDL_simple_type param_type;
    Concatenation outer_c;
    param_type.add_dimension({
        Expression_component("31", Expression_component::number),
        Expression_component("0", Expression_component::number),
        true
    });
    param_type.add_dimension({
        Expression_component("4", Expression_component::number),
        Expression_component("0", Expression_component::number),
        false
    });
    outer_c.add_component(std::make_shared<Expression>(Expression(Expression_component("3", Expression_component::number))));
    outer_c.add_component(std::make_shared<Expression>(Expression(Expression_component("3", Expression_component::number))));
    outer_c.add_component(std::make_shared<Expression>(Expression(Expression_component("3", Expression_component::number))));

    Concatenation c;
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("3'b0", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("1'b1", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("5'b0", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("4'hE", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("4'b0", Expression_component::number))));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    c = Concatenation();
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("2'h2", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("2'b1", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("2'h3", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("4'hE", Expression_component::number))));
    c.add_component(std::make_shared<Expression>(Expression(Expression_component("4'b0", Expression_component::number))));
    outer_c.add_component(std::make_shared<Concatenation>(c));
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(outer_c));

    mdarray<hdl_integer> check_array;
    check_array.set_1d_slice({0, 0}, {0x27e0, 0x220e0, 3 , 3, 3});

    auto res = p.evaluate({});
    ASSERT_TRUE(res.has_value());
    auto values = res.value().get_int_array();

    ASSERT_EQ(check_array, values);

}



TEST(Initialization_list, get_array_dependencies) {
    auto test_pattern = R"(
        module dependency #(
            parameter SS_POLARITY_DEFAULT = 0
        )();

            localparam [31:0] FIXED_REGISTER_VALUES [3:0]= '{SS_POLARITY_DEFAULT,SS_POLARITY_DEFAULT,1};

            localparam [31:0] VARIABLE_INITIAL_VALUES [2:0] = '{3{2'h2}};
            parameter [31:0] INITIAL_REGISTER_VALUES [N_REGISTERS-1:0] = {VARIABLE_INITIAL_VALUES, FIXED_REGISTER_VALUES};

        endmodule

    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern)[0];

    auto params = resource.get_parameters();
    auto deps_a = params.get("INITIAL_REGISTER_VALUES")->get_dependencies();
    auto deps_b = params.get("FIXED_REGISTER_VALUES")->get_dependencies();
    auto deps_c = params.get("VARIABLE_INITIAL_VALUES")->get_dependencies();

    std::set<qualified_identifier> check_a = {{"","","VARIABLE_INITIAL_VALUES"}, {"","", "N_REGISTERS"}, {"","", "FIXED_REGISTER_VALUES"}};
    std::set<qualified_identifier> check_b = {{"","", "SS_POLARITY_DEFAULT"}};
    EXPECT_EQ(deps_a, check_a);
    EXPECT_EQ(deps_b, check_b);
    EXPECT_TRUE(deps_c.empty());

}

TEST(Initialization_list, concatenation_of_packed_arrays) {
    auto test_pattern = R"(
        module dependency #(
            parameter SS_POLARITY_DEFAULT = 0,
            N_CHANNELS=3
        )();
            localparam  N_REGISTERS = 4 + N_CHANNELS;

            localparam [31:0] FIXED_REGISTER_VALUES [3:0]= '{
            0,
            0,
            1,
            {SS_POLARITY_DEFAULT,3'b0,SS_POLARITY_DEFAULT,5'b0,4'hE,4'b0}
            };

            localparam [31:0] VARIABLE_INITIAL_VALUES [2:0] = '{3{2'h2}};
            parameter [31:0] INITIAL_REGISTER_VALUES [N_REGISTERS-1:0] = {VARIABLE_INITIAL_VALUES, FIXED_REGISTER_VALUES};

        endmodule

    )";


    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern)[0];

    auto p = parameter_solver::process_parameters(resource.get_parameters(), {});
    auto param = p[{"","", "INITIAL_REGISTER_VALUES"}];
    mdarray<hdl_integer>::md_1d_array check_array = {224,1,0,0,2,2,2};
    auto result = param.get_int_array().get_1d_slice({0,0});

    ASSERT_EQ(check_array, result);

}
