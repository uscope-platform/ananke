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

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto param =  resource->get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    auto param_type = HDL_simple_type();
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("ADDR_WIDTH")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::subtract);
    param_type.add_dimension({
         std::make_shared<Expression_v2>(e),
        std::make_shared<Numeric_token>("0"),
        true
    });
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("CTRL_ADDR_CALC")));
    HDL_function_call call("CTRL_ADDR_CALC");
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("TEST_PARAM"), 100}
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

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto param =  resource->get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    p.set_type(Type_engine::create_primitive_type("integer"));
    p.set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("CTRL_ADDR_CALC")));
    HDL_function_call call("get_axis_metadata");
    call.add_argument(std::make_shared<Numeric_token>("11"));
    call.add_argument(std::make_shared<Numeric_token>("1'b1"));
    call.add_argument(std::make_shared<Numeric_token>("1'b0"));
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("TEST_PARAM"), 3}
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

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto param =  resource->get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    p.set_type(Type_engine::create_primitive_type("integer"));
    p.set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("CTRL_ADDR_CALC")));
    HDL_function_call call("get_axis_metadata");
    call.add_argument(std::make_shared<Numeric_token>("11"));
    call.add_argument(std::make_shared<Numeric_token>("1'b1"));
    call.add_argument(std::make_shared<Numeric_token>("1'b0"));
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("TEST_PARAM"), 15}
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

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto param =  resource->get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    p.set_type(Type_engine::create_primitive_type("integer"));
    p.set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("CTRL_ADDR_CALC")));
    HDL_function_call call("get_axis_metadata");
    call.add_argument(std::make_shared<Numeric_token>("11"));
    call.add_argument(std::make_shared<Numeric_token>("1'b1"));
    call.add_argument(std::make_shared<Numeric_token>("1'b0"));
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    EXPECT_EQ(p, *param);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("TEST_PARAM"), 19}
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

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto func = resource.get_functions()["CTRL_ADDR_CALC"];



    hdl_function_statement f;
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

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto param = resource->get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    auto param_type = HDL_simple_type();

    param_type.add_dimension({
         std::make_shared<Numeric_token>("31"),
        std::make_shared<Numeric_token>("0"),
        true
    });
    param_type.add_dimension({
         std::make_shared<Numeric_token>("2"),
        std::make_shared<Numeric_token>("0"),
        false
    });
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("CTRL_ADDR_CALC")));
    HDL_function_call call("CTRL_ADDR_CALC");
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {0, 100, 200});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("TEST_PARAM"), av}
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

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto param = resource->get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");
    p.set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("CTRL_ADDR_CALC")));
    HDL_function_call call("CTRL_ADDR_CALC");

    auto param_type = HDL_simple_type();
    param_type.add_dimension({
         std::make_shared<Numeric_token>("31"),
        std::make_shared<Numeric_token>("0"),
        true
    });
    param_type.add_dimension({
        std::make_shared<Numeric_token>("2"),
       std::make_shared<Numeric_token>("0"),
       false
   });
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {0, 100, 200});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("TEST_PARAM"), av}
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

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto param = resource->get_parameters().get("TEST_PARAM");

    HDL_parameter p;
    p.set_name("TEST_PARAM");

    HDL_function_call call("add");
    call.add_argument(std::make_shared<Numeric_token>("5"));
    call.add_argument(std::make_shared<Numeric_token>("7"));
    Expression_v2 e;
    e.set_lhs(std::make_shared<Numeric_token>(5, 3));
    e.set_rhs(std::make_shared<Numeric_token>(7, 3));
    e.set_operation(Expression_v2::add);
    auto param_type = HDL_simple_type();
    param_type.add_dimension({
        std::make_shared<Numeric_token>("31"),
       std::make_shared<Numeric_token>("0"),
       true
   });
    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<HDL_function_call>(call));

    ASSERT_EQ(p, *param);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});


    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("TEST_PARAM"), 12}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, function_with_variables) {
    auto test_pattern = R"(
        module test_mod #(
        )();

            function integer compute();
                int tmp;
                tmp = 42;
                compute = tmp;
            endfunction

            parameter integer TEST_PARAM = compute();
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto functions = resource->get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_name("compute");

    auto lv = std::make_shared<HDL_parameter>("tmp");
    lv->set_type(Type_engine::create_primitive_type("int"));
    check_f.add_local_variable(lv);

    auto s1 = std::make_shared<hdl_assignment_statement>();
    s1->set_target("tmp");
    s1->set_value(std::make_shared<Numeric_token>("42"));
    check_f.add_statement(s1);

    auto s2 = std::make_shared<hdl_assignment_statement>();
    s2->set_target("compute");
    s2->set_value(std::make_shared<Identifier_token>(qualified_identifier("tmp")));
    check_f.add_statement(s2);

    EXPECT_EQ(check_f, result);


    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    EXPECT_EQ(defaults[sid], 42);

}

TEST(parameter_extraction, concat_size_mixup_in_function) {
    auto test_pattern = R"(

        module test_mod #(
        )();
           function [15:0] get_axis_metadata ();
            reg [3:0] biased_size;
            begin
                biased_size = 10;
                get_axis_metadata = { 10'h1, biased_size};
            end
            endfunction
            localparam TEST_PARAM = get_axis_metadata();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resource = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[0]);


    parameter_solver::propagate_functions(resource, d_store);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    EXPECT_EQ(defaults[sid], 26);

}

TEST(parameter_extraction, top_level_function_simple) {
    auto test_pattern = R"(
        function integer calc();
            calc = 77;
        endfunction

        module test_mod #(
        )();
            localparam TEST_PARAM = calc();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resource = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[1]);
    auto functions = resource->get_functions();

    parameter_solver::propagate_functions(resource, d_store);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    EXPECT_EQ(defaults[sid], 77);
}


TEST(parameter_extraction, top_level_function_with_args) {
    auto test_pattern = R"(
        function integer add(input integer a, input integer b);
            add = a + b;
        endfunction

        module test_mod #(
        )();
            localparam TEST_PARAM = add(5, 7);
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resource = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[1]);

    parameter_solver::propagate_functions(resource, d_store);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    EXPECT_EQ(defaults[sid], 12);
}


TEST(parameter_extraction, conditional_in_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            parameter CONDITION = 1;

            function integer compute();
                if(CONDITION ==2)begin
                    compute = 32;
                end else begin
                    compute = 47;
                end
            endfunction


            localparam TEST_PARAM = compute();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resource = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[0]);

    parameter_solver::propagate_functions(resource, d_store);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    EXPECT_EQ(defaults[sid], 47);
}

TEST(parameter_extraction, struct_returning_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            typedef struct {
                logic [31:0] base;
                logic [31:0] size;
            } addr_range_t;

            function addr_range_t compute_addr();
                compute_addr.base = 32'h1000;
                compute_addr.size = 32'h400;
            endfunction

            parameter logic [31:0] TEST_PARAM [1:0] = compute_addr();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resource = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[0]);

    parameter_solver::propagate_functions(resource, d_store);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    mdarray<hdl_integer> av;
    av.set_1d_slice({0, 0}, {0x1000, 0x400});

    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("TEST_PARAM"), av}
    };
    for (const auto& [name, value] : check_defaults) {
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, packed_struct_returning_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            typedef struct packed {
                logic [15:0] base;
                logic [15:0] size;
            } addr_range_t;

            function addr_range_t compute_addr();
                compute_addr.base = 16'hCAFE;
                compute_addr.size = 16'hBEBE;
            endfunction

            parameter logic [31:0] TEST_PARAM = compute_addr();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resource = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[0]);

    parameter_solver::propagate_functions(resource, d_store);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});


    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("TEST_PARAM"), hdl_integer(0xBEBECAFE)}
    };
    for (const auto& [name, value] : check_defaults) {
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}


TEST(parameter_extraction, packed_struct_returning_function_reverse_order) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            typedef struct packed {
                logic [15:0] base;
                logic [15:0] size;
            } addr_range_t;

            function addr_range_t compute_addr();
                compute_addr.size = 16'hBEBE;
                compute_addr.base = 16'hCAFE;
            endfunction

            parameter logic [31:0] TEST_PARAM = compute_addr();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resource = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[0]);

    parameter_solver::propagate_functions(resource, d_store);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});


    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("TEST_PARAM"), hdl_integer(0xBEBECAFE)}
    };
    for (const auto& [name, value] : check_defaults) {
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, packed_struct_returning_computed_fields) {
    // Struct fields copied from another struct carry minimal (unset) widths,
    // so packing must use the declared member widths: a=3, b=5 in 16-bit
    // fields packs as 3 + 5*65536. (Member layout follows the existing
    // function-packing convention, member[0] at LSB.)
    auto test_pattern = R"(
        module test_mod #(
        )();
            typedef struct packed {
                logic [15:0] a;
                logic [15:0] b;
            } pair_t;

            localparam pair_t U = '{16'd3, 16'd5};

            function pair_t build();
                build.a = U.a;
                build.b = U.b;
            endfunction

            parameter pair_t P = build();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resource = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[0]);

    parameter_solver::propagate_functions(resource, d_store);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("P");
    ASSERT_TRUE(defaults.contains(sid));
    EXPECT_EQ(defaults[sid], 327683);
}

TEST(parameter_extraction, concat_and_assignment_in_function) {
    auto test_pattern = R"(

        module test_mod #(
        )();
           function [15:0] get_axis_metadata (input [4:0] size,input is_signed, input is_float);
            reg [3:0] biased_size;
            begin
                biased_size = size -8;
                get_axis_metadata = { 10'h0, is_float, is_signed, biased_size};
            end
            endfunction
            localparam TEST_PARAM = get_axis_metadata(18, 1, 0);
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto resource = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[0]);


    parameter_solver::propagate_functions(resource, d_store);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    EXPECT_EQ(defaults[sid], 26);

}

TEST(parameter_extraction, return_statement_function_parameter) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                return a + 1;
            endfunction

            parameter integer TEST_PARAM = compute(41);
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier tcid = qualified_identifier("TEST_PARAM");
    ASSERT_TRUE(defaults.contains(tcid));
    EXPECT_EQ(defaults[tcid], 42);
}

TEST(parameter_extraction, case_statement_function_parameter) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            parameter integer SEL_M = 1;
            parameter integer SEL_D = 5;
            function integer compute_m();
                case (SEL_M)
                    0, 1: compute_m = 10;
                    2: compute_m = 20;
                    default: compute_m = 30;
                endcase
            endfunction
            function integer compute_d();
                case (SEL_D)
                    0, 1: compute_d = 10;
                    2: compute_d = 20;
                    default: compute_d = 30;
                endcase
            endfunction

            parameter integer P_M = compute_m();
            parameter integer P_D = compute_d();
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier mid = qualified_identifier("P_M");
    ASSERT_TRUE(defaults.contains(mid));
    EXPECT_EQ(defaults[mid], 10);
    qualified_identifier did = qualified_identifier("P_D");
    ASSERT_TRUE(defaults.contains(did));
    EXPECT_EQ(defaults[did], 30);
}

TEST(parameter_extraction, initialized_local_function_parameter) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                int t = a + 10;
                compute = t;
            endfunction

            parameter integer TEST_PARAM = compute(5);
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    ASSERT_TRUE(defaults.contains(sid));
    EXPECT_EQ(defaults[sid], 15);
}

TEST(parameter_extraction, nested_call_function_parameter) {
    // Repro for KNOWN_ISSUES.md #11: a user call nested inside another
    // function's body parses but never receives its definition (single-pass
    // propagation), so it evaluates to 0. No name shadowing involved.
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer helper(input integer x);
                helper = x + 1;
            endfunction
            function integer compute(input integer a);
                compute = helper(a) * 10;
            endfunction

            parameter integer TEST_PARAM = compute(1);
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    ASSERT_TRUE(defaults.contains(sid));
    EXPECT_EQ(defaults[sid], 20);
}
