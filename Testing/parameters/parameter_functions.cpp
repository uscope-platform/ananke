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
        {qualified_identifier("TEST_PARAM"), hdl_integer(0xCAFEBEBE)}
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
        {qualified_identifier("TEST_PARAM"), hdl_integer(0xCAFEBEBE)}
    };
    for (const auto& [name, value] : check_defaults) {
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, packed_struct_returning_computed_fields) {
    // Struct fields copied from another struct carry minimal (unset) widths,
    // so packing must use the declared member widths: a=3, b=5 in 16-bit
    // fields packs as 3*65536 + 5. (Member layout follows the SV packed
    // convention, member[0] most significant, matching struct literals and
    // extract_struct_fields.)
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
    EXPECT_EQ(defaults[sid], 196613);
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

TEST(parameter_extraction, streaming_function_parameter) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            parameter [7:0] SRC = 8'hAB;
            function logic [7:0] compute();
                compute = {<<{SRC}};
            endfunction
            function logic [7:0] compute_arg(input logic [7:0] a);
                compute_arg = {<<{a}};
            endfunction

            parameter logic [7:0] P_SRC = compute();
            parameter logic [7:0] P_ARG = compute_arg(8'hAB);
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("P_SRC");
    ASSERT_TRUE(defaults.contains(sid));
    EXPECT_EQ(defaults[sid], 0xD5);
    qualified_identifier aid = qualified_identifier("P_ARG");
    ASSERT_TRUE(defaults.contains(aid));
    EXPECT_EQ(defaults[aid], 0xD5);
}

TEST(parameter_extraction, anonymous_struct_local_function_parameter) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            typedef struct packed {
                logic [7:0] hi;
                logic [7:0] lo;
            } pair_t;
            localparam pair_t U = '{8'hCA, 8'hFE};
            function logic [15:0] compute();
                struct packed { logic [7:0] hi; logic [7:0] lo; } tmp;
                tmp = U;
                compute = tmp;
            endfunction

            parameter logic [15:0] TEST_PARAM = compute();
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    ASSERT_TRUE(defaults.contains(sid));
    EXPECT_EQ(defaults[sid], 0xCAFE);
}

TEST(parameter_extraction, anonymous_enum_member_function_parameter) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                enum {IDLE, RUN, DONE} state;
                state = RUN;
                compute = state;
            endfunction

            parameter integer TEST_PARAM = compute(0);
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    ASSERT_TRUE(defaults.contains(sid));
    EXPECT_EQ(defaults[sid], 1);
}

TEST(parameter_extraction, anonymous_enum_local_function_parameter) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                enum {IDLE, RUN, DONE} state;
                state = 2;
                compute = state + a;
            endfunction

            parameter integer TEST_PARAM = compute(40);
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    ASSERT_TRUE(defaults.contains(sid));
    EXPECT_EQ(defaults[sid], 42);
}

namespace {
// Print-snapshot of a function's body statements. Used to prove the shared
// definition is never mutated by solving (any pointer reseat in an
// expression changes its print).
std::string snapshot_function_body(const std::shared_ptr<hdl_resource_statement> &resource, const std::string &fname) {
    auto def = resource->get_function_shared(fname);
    if (!def) return "<missing>";
    std::string s;
    for (const auto &stmt : def->get_body()) {
        if (stmt) {
            s += stmt->print();
            s += "\n";
        }
    }
    return s;
}
}

TEST(parameter_extraction, function_multi_site_no_cross_contamination) {
    // KNOWN_ISSUES.md item 1 repro: two call sites of the same function with
    // different actuals must not corrupt each other, in either solve order,
    // and the shared definition must come out identical to how it went in.
    // The old clone+substitute design solved Z as 1 (stuck always-true).
    const char *pattern_yz = R"(
        module test_mod #(
        )();
            function integer compute2(input integer a);
                bit f = (a == 32) ? 1'b1 : 1'b0;
                compute2 = f;
            endfunction
            parameter integer Y = compute2(32);
            parameter integer Z = compute2(64);
        endmodule
    )";
    const char *pattern_zy = R"(
        module test_mod #(
        )();
            function integer compute2(input integer a);
                bit f = (a == 32) ? 1'b1 : 1'b0;
                compute2 = f;
            endfunction
            parameter integer Z = compute2(64);
            parameter integer Y = compute2(32);
        endmodule
    )";
    for (const char *pattern : {pattern_yz, pattern_zy}) {
        sv_analyzer analyzer;
        auto resource = std::static_pointer_cast<hdl_resource_statement>(
            analyzer.analyze("", pattern).value().get_content()[0]);

        const std::string before = snapshot_function_body(resource, "compute2");
        ASSERT_NE(before, "<missing>");

        parameter_solver::propagate_functions(resource, nullptr);
        auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

        EXPECT_EQ(defaults[qualified_identifier("Y")], 1);
        EXPECT_EQ(defaults[qualified_identifier("Z")], 0);
        EXPECT_EQ(snapshot_function_body(resource, "compute2"), before);
    }
}

TEST(parameter_extraction, function_nested_call_solves) {
    // Depth-2 nesting through the shared link: the nested call links during
    // the solver fixpoint cascade, binds in the chained caller context, and
    // neither definition is mutated.
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer helper(input integer x);
                helper = x * 2;
            endfunction
            function integer compute(input integer a);
                compute = helper(a) + 1;
            endfunction
            parameter integer TEST_PARAM = compute(5);
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = std::static_pointer_cast<hdl_resource_statement>(
        analyzer.analyze("", test_pattern).value().get_content()[0]);

    const std::string helper_before = snapshot_function_body(resource, "helper");
    const std::string compute_before = snapshot_function_body(resource, "compute");
    ASSERT_NE(helper_before, "<missing>");
    ASSERT_NE(compute_before, "<missing>");

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("TEST_PARAM");
    ASSERT_TRUE(defaults.contains(sid));
    EXPECT_EQ(defaults[sid], 11);
    EXPECT_EQ(snapshot_function_body(resource, "helper"), helper_before);
    EXPECT_EQ(snapshot_function_body(resource, "compute"), compute_before);
}

TEST(parameter_extraction, function_call_sites_stable_across_solves) {
    // Reverse direction of KNOWN_ISSUES.md item 1: solving must never mutate
    // the caller's own trees (the old grafting aliased caller nodes into
    // bodies, letting later passes mutate call sites). Solving the same
    // resource twice must print identical call sites and solve identical
    // values.
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute2(input integer a);
                bit f = (a == 32) ? 1'b1 : 1'b0;
                compute2 = f;
            endfunction
            parameter integer Y = compute2(32);
            parameter integer Z = compute2(64);
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = std::static_pointer_cast<hdl_resource_statement>(
        analyzer.analyze("", test_pattern).value().get_content()[0]);

    auto call_print = [&](const std::string &name) {
        return resource->get_parameters().get(name)->get_expression()->print();
    };
    const std::string y_before = call_print("Y");
    const std::string z_before = call_print("Z");
    ASSERT_EQ(y_before, "compute2(32)");
    ASSERT_EQ(z_before, "compute2(64)");

    parameter_solver::propagate_functions(resource, nullptr);
    auto first = parameter_solver::process_parameters(resource->get_parameters(), {});
    ASSERT_EQ(first[qualified_identifier("Y")], 1);
    ASSERT_EQ(first[qualified_identifier("Z")], 0);

    EXPECT_EQ(call_print("Y"), y_before);
    EXPECT_EQ(call_print("Z"), z_before);

    auto second = parameter_solver::process_parameters(resource->get_parameters(), {});
    EXPECT_EQ(second[qualified_identifier("Y")], 1);
    EXPECT_EQ(second[qualified_identifier("Z")], 0);
    EXPECT_EQ(call_print("Y"), y_before);
    EXPECT_EQ(call_print("Z"), z_before);
}

TEST(parameter_extraction, function_formal_shadows_caller_name) {
    // A formal must hide a same-named caller parameter inside the body:
    // Y must be 1 (the bound actual), not 100 (the leaked caller value).
    // Actuals themselves still evaluate in the caller context (see the
    // mixed test below: a comes from the caller, b is caller-visible).
    auto test_pattern = R"(
        module test_mod #(
        )();
            parameter integer a = 100;
            function integer f(input integer a);
                f = a;
            endfunction
            parameter integer Y = f(1);
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = std::static_pointer_cast<hdl_resource_statement>(
        analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    ASSERT_TRUE(defaults.contains(qualified_identifier("Y")));
    EXPECT_EQ(defaults[qualified_identifier("Y")], 1);
}

TEST(parameter_extraction, function_actuals_evaluate_in_caller_context) {
    // Companion to the shadowing test: non-formal names stay visible from
    // the caller context, and actuals are evaluated there before binding.
    auto test_pattern = R"(
        module test_mod #(
        )();
            parameter integer b = 5;
            function integer f(input integer a);
                f = a + b;
            endfunction
            parameter integer Y = f(b);
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = std::static_pointer_cast<hdl_resource_statement>(
        analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    ASSERT_TRUE(defaults.contains(qualified_identifier("Y")));
    EXPECT_EQ(defaults[qualified_identifier("Y")], 10);
}

TEST(parameter_extraction, function_return_struct_read_leaks_into_dependencies) {
    // Repro for the CVA6 build_config issue: intra-function reads of the
    // return struct under construction (cfg.FETCH_WIDTH) leak into the
    // call's dependencies. They are bound inside the body, so no FETCH_WIDTH
    // entry should survive dependency filtering (the sorter leaf-matches it
    // against any same-named parameter).
    auto test_pattern = R"(
        package config_pkg;
            typedef struct packed {
                logic [31:0] XLEN;
                logic [31:0] FETCH_WIDTH;
            } my_cfg_t;
        endpackage

        package build_pkg;
            function integer build_config(config_pkg::my_cfg_t CVA6Cfg);
                config_pkg::my_cfg_t cfg;
                cfg.XLEN = CVA6Cfg.XLEN;
                cfg.FETCH_WIDTH = CVA6Cfg.XLEN;
                build_config = cfg.FETCH_WIDTH;
            endfunction
        endpackage

        module test_mod #(
            parameter integer RESULT = build_pkg::build_config(0)
        )();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto mod = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[2]);
    parameter_solver::propagate_functions(mod, d_store);

    auto param = mod->get_parameters().get("RESULT");
    auto call = std::dynamic_pointer_cast<HDL_function_call>(param->get_expression());
    ASSERT_TRUE(call);
    ASSERT_TRUE(call->get_linked_definition());

    const auto deps = param->get_dependencies();
    EXPECT_TRUE(deps.functions.contains(qualified_identifier("build_pkg", "build_config")));
    for (const auto &d : deps.data) {
        EXPECT_NE(d.get_name(), "FETCH_WIDTH") << "leaked dep: " << d.print();
        if (!d.get_instance().empty()) {
            EXPECT_NE(d.get_instance().front(), "CVA6Cfg") << "leaked dep: " << d.print();
            EXPECT_NE(d.get_instance().front(), "cfg") << "leaked dep: " << d.print();
        }
    }
}

TEST(parameter_extraction, function_struct_local_return_solves) {
    // End-to-end CVA6 shape: struct formal, struct-typed return-var local
    // built via cfg.FIELD assigns, field reads back, package-qualified
    // actual, `return cfg`. Asserts solved field values, definition
    // immutability, and dependency cleanliness together.
    auto test_pattern = R"(
        package config_pkg;
            typedef struct packed {
                logic [31:0] XLEN;
                logic [31:0] FETCH_WIDTH;
            } my_cfg_t;
            parameter my_cfg_t user_cfg = '{32'd32, 32'd2};
        endpackage

        package build_pkg;
            function config_pkg::my_cfg_t build_config(config_pkg::my_cfg_t CVA6Cfg);
                config_pkg::my_cfg_t cfg;
                cfg.XLEN = CVA6Cfg.XLEN;
                cfg.FETCH_WIDTH = CVA6Cfg.XLEN + 32'd2;
                return cfg;
            endfunction
        endpackage

        module test_mod #(
            parameter config_pkg::my_cfg_t RESULT = build_pkg::build_config(config_pkg::user_cfg)
        )();
        endmodule
    )";

    sv_analyzer analyzer;
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    auto file = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto resources = file.get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[2]);

    const std::string def_before = snapshot_function_body(
        std::static_pointer_cast<hdl_resource_statement>(resources[1]), "build_config");
    ASSERT_NE(def_before, "<missing>");

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    // Seed exactly as retrieve_package_parameters exports: instance-preserving
    // canonical form plus the flat legacy alias for bare pkg::FIELD reads.
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto &[id, val] : pkg_defaults) {
        qualified_identifier qid(id.get_name());
        qid.set_package_prefix({"config_pkg"});
        const auto inst = id.get_instance();
        if (!inst.empty()) qid.set_instance_prefix(inst);
        ctx[qid] = val;
        if (!inst.empty()) {
            qualified_identifier flat("config_pkg", id.get_name());
            if (!ctx.contains(flat)) ctx[flat] = val;
        }
    }

    parameter_solver::propagate_types(mod, d_store);
    parameter_solver::propagate_functions(mod, d_store);

    auto param = mod->get_parameters().get("RESULT");
    for (const auto &d : param->get_dependencies().data) {
        EXPECT_NE(d.get_name(), "FETCH_WIDTH") << "leaked dep: " << d.print();
        if (!d.get_instance().empty()) {
            EXPECT_NE(d.get_instance().front(), "CVA6Cfg") << "leaked dep: " << d.print();
            EXPECT_NE(d.get_instance().front(), "cfg") << "leaked dep: " << d.print();
        }
    }

    auto solved = parameter_solver::process_parameters(mod->get_parameters(), ctx);
    qualified_identifier sid = qualified_identifier("RESULT");
    ASSERT_TRUE(solved.contains(sid));
    ASSERT_TRUE(solved.at(sid).is_integer());
    // SV packed layout: member[0] (XLEN=32) most significant, member[1]
    // (32+2=34) least significant — matching struct literals and
    // extract_struct_fields, so downstream XLEN reads come back correct.
    EXPECT_EQ(solved.at(sid).get_integer().get_value(), (32LL << 32) | 34);
    EXPECT_EQ(snapshot_function_body(
        std::static_pointer_cast<hdl_resource_statement>(resources[1]), "build_config"), def_before);
}

TEST(parameter_extraction, struct_field_downstream_uses) {
    // Downstream uses of a function-produced struct: plain field read,
    // struct-field-driven size cast (CVA6Cfg.XLEN'(…) shape), and keyed
    // struct literal. Locks the ::-field, package-export, and re-key fixes.
    auto test_pattern = R"(
        package config_pkg;
            typedef struct packed {
                logic [31:0] XLEN;
                logic [31:0] FETCH_WIDTH;
            } my_cfg_t;
            parameter my_cfg_t user_cfg = '{32'd32, 32'd2};
        endpackage

        package build_pkg;
            function config_pkg::my_cfg_t build_config(config_pkg::my_cfg_t CVA6Cfg);
                config_pkg::my_cfg_t cfg;
                cfg.XLEN = CVA6Cfg.XLEN;
                cfg.FETCH_WIDTH = CVA6Cfg.XLEN + 32'd2;
                return cfg;
            endfunction
        endpackage

        module test_mod #(
            parameter config_pkg::my_cfg_t RESULT = build_pkg::build_config(config_pkg::user_cfg)
        )();
            parameter integer XL = RESULT.XLEN;
            parameter integer W = RESULT.XLEN'(1);
            parameter config_pkg::my_cfg_t K = '{XLEN: 32'd7, FETCH_WIDTH: 32'd9};
        endmodule
    )";

    sv_analyzer analyzer;
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    auto file = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto resources = file.get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[2]);

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    // Seed exactly as retrieve_package_parameters exports: instance-preserving
    // canonical form plus the flat legacy alias for bare pkg::FIELD reads.
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto &[id, val] : pkg_defaults) {
        qualified_identifier qid(id.get_name());
        qid.set_package_prefix({"config_pkg"});
        const auto inst = id.get_instance();
        if (!inst.empty()) qid.set_instance_prefix(inst);
        ctx[qid] = val;
        if (!inst.empty()) {
            qualified_identifier flat("config_pkg", id.get_name());
            if (!ctx.contains(flat)) ctx[flat] = val;
        }
    }

    parameter_solver::propagate_types(mod, d_store);
    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), ctx);

    EXPECT_EQ(solved.at(qualified_identifier("XL")).get_integer(), 32);
    EXPECT_EQ(solved.at(qualified_identifier("W")).get_integer(), 1);
}

TEST(parameter_extraction, interrupt_shaped_uses) {
    // Full CVA6 interrupt shape: package enum constant as cast content,
    // replication inside a keyed literal, and a local type whose dimensions
    // depend on a function-produced struct field. Package enum members are
    // seeded into ctx exactly as retrieve_package_parameters exports them.
    auto test_pattern = R"(
        package config_pkg;
            typedef struct packed {
                logic [31:0] XLEN;
                logic [31:0] FETCH_WIDTH;
            } my_cfg_t;
            typedef enum logic [1:0] {IRQ_S = 3, IRQ_M = 5} irq_t;
            parameter my_cfg_t user_cfg = '{32'd32, 32'd2};
        endpackage

        package build_pkg;
            function config_pkg::my_cfg_t build_config(config_pkg::my_cfg_t CVA6Cfg);
                config_pkg::my_cfg_t cfg;
                cfg.XLEN = CVA6Cfg.XLEN;
                cfg.FETCH_WIDTH = CVA6Cfg.XLEN + 32'd2;
                return cfg;
            endfunction
        endpackage

        module test_mod #(
            parameter config_pkg::my_cfg_t RESULT = build_pkg::build_config(config_pkg::user_cfg)
        )();
            parameter integer XL2 = RESULT.XLEN;
            parameter integer Q = (5 << 1) | 3;
            localparam type itype_t = struct packed {
                logic [RESULT.XLEN-1:0] f0;
                logic [RESULT.XLEN-1:0] f1;
            };
            localparam itype_t I = '{
                f0: (RESULT.XLEN'(1) << (RESULT.XLEN - 1)) | RESULT.XLEN'(config_pkg::IRQ_S),
                f1: {2{2'b01}}
            };
        endmodule
    )";

    sv_analyzer analyzer;
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    auto file = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto resources = file.get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[2]);

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto &[id, val] : pkg_defaults) {
        qualified_identifier qid(id.get_name());
        qid.set_package_prefix({"config_pkg"});
        const auto inst = id.get_instance();
        if (!inst.empty()) qid.set_instance_prefix(inst);
        ctx[qid] = val;
        if (!inst.empty()) {
            qualified_identifier flat("config_pkg", id.get_name());
            if (!ctx.contains(flat)) ctx[flat] = val;
        }
    }
    ctx[qualified_identifier("config_pkg", "", "IRQ_S")] = 3;
    ctx[qualified_identifier("config_pkg", "", "IRQ_M")] = 5;

    parameter_solver::propagate_types(mod, d_store);
    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), ctx);

    ASSERT_TRUE(solved.contains(qualified_identifier("XL2")));
    EXPECT_EQ(solved.at(qualified_identifier("XL2")).get_integer(), 32);
    ASSERT_TRUE(solved.contains(qualified_identifier("Q")));
    EXPECT_EQ(solved.at(qualified_identifier("Q")).get_integer(), 11);
}


TEST(parameter_extraction, wide_struct_member_roundtrip) {
    // A >1024-bit packed struct must pack bit-exactly (shift/mask caps) and
    // its members must read back through package-qualified dotted selects
    // (::-branch + instance-preserving export). U == 7*2^1056 + 9.
    auto test_pattern = R"(
        package config_pkg;
            typedef struct packed {
                logic [31:0] A;
                logic [1023:0] W;
                logic [31:0] B;
            } wide_t;
            parameter wide_t U = '{32'd7, 1024'({64'd0}), 32'd9};
        endpackage
        module test_mod #(
        )();
            parameter integer A = config_pkg::U.A;
            parameter integer B = config_pkg::U.B;
        endmodule
    )";
    sv_analyzer analyzer;
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    auto file = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto resources = file.get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);
    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto &[id, val] : pkg_defaults) {
        qualified_identifier qid(id.get_name());
        qid.set_package_prefix({"config_pkg"});
        const auto inst = id.get_instance();
        if (!inst.empty()) qid.set_instance_prefix(inst);
        ctx[qid] = val;
        if (!inst.empty()) {
            qualified_identifier flat("config_pkg", id.get_name());
            if (!ctx.contains(flat)) ctx[flat] = val;
        }
    }
    parameter_solver::propagate_types(mod, d_store);
    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), ctx);
    EXPECT_EQ(solved.at(qualified_identifier("A")).get_integer(), 7);
    EXPECT_EQ(solved.at(qualified_identifier("B")).get_integer(), 9);
    EXPECT_EQ(pkg_defaults.at(qualified_identifier("U")).get_integer().to_wide().str(),
              "5404723255734155000562543590669331166637026017566661180489767098205613620666379654860392326011886106362569980812903150099672932287981240608478903959599188115885246162170274847226212898996934945635522360799061643523918351684466739236170667608429400994818383772226377055611930474497274466420380754701698470471015755415561");
}

TEST(parameter_extraction, huge_struct_member_roundtrip) {
    // Tens-of-kb packed struct (CVA6 shapes): members round-trip through the
    // package export and single-bit selects past bit 1023 resolve.
    // H == 0x80000007 * 2^40032 + 9, 40064 bits total.
    auto test_pattern = R"(
        package config_pkg;
            typedef struct packed {
                logic [31:0] A;
                logic [39999:0] W;
                logic [31:0] B;
            } huge_t;
            parameter huge_t H = '{32'h80000007, 40000'({64'd0}), 32'd9};
        endpackage
        module test_mod #(
        )();
            parameter integer A = config_pkg::H.A;
            parameter integer B = config_pkg::H.B;
            parameter integer TOPBIT = config_pkg::H[40063];
            parameter integer BOTBIT = config_pkg::H[0];
            parameter integer WBIT = config_pkg::H[32];
        endmodule
    )";
    sv_analyzer analyzer;
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    auto file = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto resources = file.get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);
    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto &[id, val] : pkg_defaults) {
        qualified_identifier qid(id.get_name());
        qid.set_package_prefix({"config_pkg"});
        const auto inst = id.get_instance();
        if (!inst.empty()) qid.set_instance_prefix(inst);
        ctx[qid] = val;
        if (!inst.empty()) {
            qualified_identifier flat("config_pkg", id.get_name());
            if (!ctx.contains(flat)) ctx[flat] = val;
        }
    }
    parameter_solver::propagate_types(mod, d_store);
    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), ctx);
    EXPECT_EQ(solved.at(qualified_identifier("A")).get_integer(), 0x80000007LL);
    EXPECT_EQ(solved.at(qualified_identifier("B")).get_integer(), 9);
    EXPECT_EQ(solved.at(qualified_identifier("TOPBIT")).get_integer(), 1);
    EXPECT_EQ(solved.at(qualified_identifier("BOTBIT")).get_integer(), 1);
    EXPECT_EQ(solved.at(qualified_identifier("WBIT")).get_integer(), 0);
}

TEST(parameter_extraction, package_localparam_struct_shapes) {
    // localparams in one package typed by another package's struct, with an
    // unsized cast, a cross-package enum member, replication, and a sized
    // cast of an expression. Keys are in member order (out-of-order keyed
    // literals assemble positionally — separate deferred work).
    auto test_pattern = R"(
        package config_pkg;
            typedef struct packed {
                logic [31:0] X;
                logic [31:0] Y;
            } pair_t;
            typedef enum logic [1:0] {E_ZERO = 0, E_ONE = 1} e_t;
        endpackage
        package user_pkg;
            localparam integer A = 5;
            localparam config_pkg::pair_t P = '{X: unsigned'(A), Y: config_pkg::E_ONE};
            localparam config_pkg::pair_t Q = '{X: 32'd7, Y: 32'd9};
            localparam integer R = {2{2'b01}};
            localparam integer S = 32'(A + 1);
        endpackage
        module test_mod #(
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    auto pkg = std::static_pointer_cast<hdl_resource_statement>(resources[1]);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});
    parameter_solver::propagate_types(pkg, d_store);
    std::map<qualified_identifier, resolved_parameter> pctx;
    pctx[qualified_identifier("config_pkg", "", "E_ZERO")] = 0;
    pctx[qualified_identifier("config_pkg", "", "E_ONE")] = 1;
    auto solved = parameter_solver::process_parameters(pkg->get_parameters(), pctx);
    EXPECT_EQ(solved.at(qualified_identifier("A")).get_integer(), 5);
    EXPECT_EQ(solved.at(qualified_identifier("P")).get_integer(), (5LL << 32) | 1);
    EXPECT_EQ(solved.at(qualified_identifier("Q")).get_integer(), (7LL << 32) | 9);
    EXPECT_EQ(solved.at(qualified_identifier("R")).get_integer(), 5);
    EXPECT_EQ(solved.at(qualified_identifier("S")).get_integer(), 6);
}

TEST(parameter_extraction, cast_in_binary_inside_literal) {
    // A cast inside an enclosing binary expression inside a stacked consumer
    // (struct literal) must not fragment: stop_cast routes the completed
    // cast operand-first. Consumer-first routing solved f0 as 2 here.
    auto test_pattern = R"(
        package p;
            typedef struct packed { logic [31:0] f0; logic [31:0] f1; } s_t;
            parameter s_t V = '{f0: (8'(1) << 2), f1: 32'd5};
        endpackage
        module test_mod #(
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto solved = parameter_solver::process_parameters(pkg.get_parameters(), {});
    EXPECT_EQ(solved.at(qualified_identifier("V")).get_integer(), (4LL << 32) | 5);
}

TEST(parameter_extraction, keyed_struct_literal_member_order) {
    // Keyed struct literals assemble by member name, not source position:
    // scrambled key order packs into member order, and unmentioned members
    // default to zero (partial literals).
    auto test_pattern = R"(
        package config_pkg;
            typedef struct packed {
                logic [31:0] A;
                logic [31:0] B;
                logic [31:0] C;
            } trio_t;
        endpackage
        package user_pkg;
            localparam config_pkg::trio_t T = '{
                C: 32'd3,
                A: 32'd1,
                B: 32'd2
            };
            localparam config_pkg::trio_t P = '{
                B: 32'd20
            };
        endpackage
        module test_mod #(
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    auto pkg = std::static_pointer_cast<hdl_resource_statement>(resources[1]);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});
    parameter_solver::propagate_types(pkg, d_store);
    auto solved = parameter_solver::process_parameters(pkg->get_parameters(), {});
    auto field = [&](const std::string &root, const std::string &name) {
        qualified_identifier qid(name);
        qid.set_instance_prefix({root});
        return solved.at(qid).get_integer().get_value();
    };
    EXPECT_EQ(field("T", "A"), 1);
    EXPECT_EQ(field("T", "B"), 2);
    EXPECT_EQ(field("T", "C"), 3);
    EXPECT_EQ(solved.at(qualified_identifier("T")).get_integer().to_wide().str(), "18446744082299486211");
    EXPECT_EQ(field("P", "A"), 0);
    EXPECT_EQ(field("P", "B"), 20);
    EXPECT_EQ(field("P", "C"), 0);
}

TEST(parameter_extraction, ternary_unsized_cast_branches) {
    // Ternary branches inherit the container type so nested unsized casts
    // (unsigned'/bit') size as at top level instead of going missing.
    auto test_pattern = R"(
        package p;
            function integer pick(bit sel);
                integer r;
                r = sel ? unsigned'(2) : unsigned'(1);
                return r;
            endfunction
            function bit pickb(bit sel);
                bit r;
                r = sel ? bit'(1) : bit'(0);
                return r;
            endfunction
        endpackage
        module test_mod #(
            parameter integer V0 = p::pick(1'b0),
            parameter integer V1 = p::pick(1'b1),
            parameter bit W0 = p::pickb(1'b0),
            parameter bit W1 = p::pickb(1'b1)
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});
    parameter_solver::propagate_types(mod, d_store);
    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), {});
    EXPECT_EQ(solved.at(qualified_identifier("V0")).get_integer(), 1);
    EXPECT_EQ(solved.at(qualified_identifier("V1")).get_integer(), 2);
    EXPECT_EQ(solved.at(qualified_identifier("W0")).get_integer(), 0);
    EXPECT_EQ(solved.at(qualified_identifier("W1")).get_integer(), 1);
}

TEST(parameter_extraction, cast_scalar_target_widths) {

    auto test_pattern = R"(
        package p;
            localparam integer A = shortint'(-5);
            localparam integer B = shortint'(70000);
            localparam longint C = longint'(1099511627776);
            localparam integer D = longint'(-1);
            localparam integer E = byte'(200);
            localparam integer F = byte'(-1);
            localparam integer G = int'(-1);
            localparam integer H = integer'(70000);
            localparam integer I = integer'(1099511627776);
            localparam integer J = bit'(0);
            localparam integer K = bit'(1);
            localparam integer L = bit'(5);
        endpackage
    )";
    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto solved = parameter_solver::process_parameters(pkg.get_parameters(), {});
    // NOTE: compared in the int64 domain (get_value): hdl_integer operator==
    // is width-relative and would alias e.g. 200 and -56 at 8 bits.
    EXPECT_EQ(solved.at(qualified_identifier("A")).get_integer().get_value(), -5);
    EXPECT_EQ(solved.at(qualified_identifier("B")).get_integer().get_value(), 4464);
    EXPECT_EQ(solved.at(qualified_identifier("C")).get_integer().get_value(), 1099511627776LL);
    EXPECT_EQ(solved.at(qualified_identifier("D")).get_integer().get_value(), -1);
    EXPECT_EQ(solved.at(qualified_identifier("E")).get_integer().get_value(), -56);
    EXPECT_EQ(solved.at(qualified_identifier("F")).get_integer().get_value(), -1);
    EXPECT_EQ(solved.at(qualified_identifier("G")).get_integer().get_value(), -1);
    EXPECT_EQ(solved.at(qualified_identifier("H")).get_integer().get_value(), 70000);
    EXPECT_EQ(solved.at(qualified_identifier("I")).get_integer().get_value(), 0);
    EXPECT_EQ(solved.at(qualified_identifier("J")).get_integer().get_value(), 0);
    EXPECT_EQ(solved.at(qualified_identifier("K")).get_integer().get_value(), 1);
    EXPECT_EQ(solved.at(qualified_identifier("L")).get_integer().get_value(), 1);
}

TEST(parameter_extraction, function_struct_bit_cast_width) {
    // A bit' cast in a function body sizes to 1 bit, not to the call's
    // container: previously it inherited the whole struct width and blew up
    // the return pack (R.XLEN-style top members read back as 0).
    auto test_pattern = R"(
        package p;
            typedef struct packed {
                logic [31:0] X;
                bit F;
            } s_t;
            function s_t mk(bit b);
                s_t r;
                r.X = 32'd64;
                r.F = bit'(b);
                return r;
            endfunction
        endpackage
        module test_mod #(
            parameter p::s_t R = p::mk(1'b1)
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});
    parameter_solver::propagate_types(mod, d_store);
    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), {});
    auto field = [&](const std::string &name) {
        qualified_identifier qid(name);
        qid.set_instance_prefix({"R"});
        return solved.at(qid).get_integer().get_value();
    };
    EXPECT_EQ(field("X"), 64);
    EXPECT_EQ(field("F"), 1);
    EXPECT_EQ(solved.at(qualified_identifier("R")).get_integer().to_wide().str(), "129");
}

TEST(parameter_extraction, cast_size_single_operand) {
    // A cast size following a binary operator takes only the immediately
    // preceding operand (`A op X'(C)` sizes by X, not by `A op X`).
    auto test_pattern = R"(
        module test_mod #(
            parameter integer V = (8'(1) << 7) | 8'(3),
            parameter integer W = 1 + 8'(3)
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[0]);

    auto c1 = std::make_shared<Cast>();
    c1->set_size(std::make_shared<Numeric_token>("8"));
    c1->set_content(std::make_shared<Numeric_token>("1"));
    auto sh = std::make_shared<Expression_v2>();
    sh->set_lhs(c1);
    sh->set_operation(Expression_v2::expression_operator::logic_shift_left);
    sh->set_rhs(std::make_shared<Numeric_token>("7"));
    auto c2 = std::make_shared<Cast>();
    c2->set_size(std::make_shared<Numeric_token>("8"));
    c2->set_content(std::make_shared<Numeric_token>("3"));
    auto top_v = std::make_shared<Expression_v2>();
    top_v->set_lhs(sh);
    top_v->set_operation(Expression_v2::expression_operator::bitwise_or);
    top_v->set_rhs(c2);

    HDL_parameter check_v("V");
    check_v.set_type(Type_engine::create_primitive_type("integer"));
    check_v.set_raw_value(top_v);

    auto c3 = std::make_shared<Cast>();
    c3->set_size(std::make_shared<Numeric_token>("8"));
    c3->set_content(std::make_shared<Numeric_token>("3"));
    auto top_w = std::make_shared<Expression_v2>();
    top_w->set_lhs(std::make_shared<Numeric_token>("1"));
    top_w->set_operation(Expression_v2::expression_operator::add);
    top_w->set_rhs(c3);

    HDL_parameter check_w("W");
    check_w.set_type(Type_engine::create_primitive_type("integer"));
    check_w.set_raw_value(top_w);

    EXPECT_EQ(check_v, *mod->get_parameters().get("V"));
    EXPECT_EQ(check_w, *mod->get_parameters().get("W"));

    auto solved = parameter_solver::process_parameters(mod->get_parameters(), {});
    EXPECT_EQ(solved.at(qualified_identifier("V")).get_integer().get_value(), 131);
    EXPECT_EQ(solved.at(qualified_identifier("W")).get_integer().get_value(), 4);
}

TEST(parameter_extraction, package_function_owner_disambiguates) {
    // Two same-named packages where only one defines the called function:
    // the call must link to the defining package regardless of store order.
    auto test_pattern = R"(
        module test_mod #(
            parameter integer RESULT = test_pkg::buildit()
        )();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto decoy = std::make_shared<hdl_resource_statement>();
    decoy->set_name("test_pkg");
    auto other = std::make_shared<HDL_parameter>("OTHER");
    other->set_type(Type_engine::create_primitive_type("integer"));
    decoy->add_parameter(other);

    auto real = std::make_shared<hdl_resource_statement>();
    real->set_name("test_pkg");
    hdl_function_statement func;
    func.set_name("buildit");
    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("buildit");
    stmt->set_value(std::make_shared<Numeric_token>("42"));
    func.add_statement(stmt);
    real->add_function(func);

    hdl_file fdecoy;
    fdecoy.set_content({decoy});
    hdl_file freal;
    freal.set_content({real});
    d_store->store_file({"/dev/decoy", "hash_decoy", fdecoy});
    d_store->store_file({"/dev/real", "hash_real", freal});

    auto mod = std::static_pointer_cast<hdl_resource_statement>(file.get_content()[0]);
    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), {});

    qualified_identifier sid = qualified_identifier("RESULT");
    ASSERT_TRUE(solved.contains(sid));
    EXPECT_EQ(solved.at(sid).get_integer(), 42);
}

TEST(parameter_extraction, while_loop_solvable) {
    // While loops in constant functions are first-class: condition-driven,
    // body assignments carry state into the next predicate evaluation.
    auto test_pattern = R"(
        module test_mod #(
        )();
            function int unsigned flz_count(input int unsigned v);
                int unsigned bits;
                bits = 0;
                while (v > 1) begin
                    v = v >> 1;
                    bits = bits + 1;
                end
                return bits;
            endfunction

            parameter int unsigned TEST_FLZ = flz_count(8);
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(
        analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    // 8 -> 4 -> 2 -> 1 : three iterations until the predicate fails.
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("TEST_FLZ"), 3}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, repeat_loop_solvable) {
    // Repeat loops: the count is evaluated once at entry and the body runs
    // that many times. Oracle cross-checked with xrun 25.03 ($display -> 9).
    auto test_pattern = R"(
        module test_mod #(
        )();
            function int unsigned bump(input int unsigned v);
                int unsigned r;
                r = v;
                repeat (4) begin
                    r = r + 1;
                end
                return r;
            endfunction

            parameter int unsigned TEST_R = bump(5);
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(
        analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("TEST_R"), 9}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

TEST(parameter_extraction, do_while_loop_solvable) {
    // do…while: the body runs at least once, the post-checked predicate
    // sees the body state each pass (input 0 still produces one iteration).
    auto test_pattern = R"(
        module test_mod #(
        )();
            function int unsigned cnt_down(input int unsigned v);
                int unsigned iters;
                iters = 0;
                do begin
                    v = v >> 1;
                    iters = iters + 1;
                end while (v > 0);
                return iters;
            endfunction

            parameter int unsigned CNT4 = cnt_down(4);
            parameter int unsigned CNT0 = cnt_down(0);
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(
        analyzer.analyze("", test_pattern).value().get_content()[0]);

    parameter_solver::propagate_functions(resource, nullptr);
    auto defaults = parameter_solver::process_parameters(resource->get_parameters(), {});

    // 4 -> 2 -> 1 -> 0: three iterations. Input 0: one iteration by the
    // at-least-once semantics.
    std::map<qualified_identifier, resolved_parameter> check_defaults = {
        {qualified_identifier("CNT4"), 3},
        {qualified_identifier("CNT0"), 1}
    };
    for(const auto& [name, value]:check_defaults){
        ASSERT_TRUE(defaults.contains(name));
        ASSERT_EQ(value, defaults.at(name));
    }
}

