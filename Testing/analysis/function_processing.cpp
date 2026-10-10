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
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/HDL_builtin_function.hpp"
#include "data_model/HDL/parameters/components/Ternary.hpp"
#include "data_model/HDL/parameters/components/Streaming.hpp"
#include "data_model/HDL/statement/hdl_assignment_statement.hpp"
#include "data_model/HDL/statement/hdl_loop_statement.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"
#include "data_model/HDL/statement/hdl_package_statement.hpp"

#include <spdlog/sinks/ostream_sink.h>

namespace {
// Captures spdlog output for the duration of the guard's lifetime.
struct log_capture {
    std::ostringstream stream;
    std::shared_ptr<spdlog::sinks::ostream_sink_mt> sink =
        std::make_shared<spdlog::sinks::ostream_sink_mt>(stream);
    log_capture() { spdlog::default_logger()->sinks().push_back(sink); }
    ~log_capture() {
        auto &sinks = spdlog::default_logger()->sinks();
        sinks.erase(std::remove(sinks.begin(), sinks.end(), sink), sinks.end());
    }
    size_t count(const std::string &needle) const {
        size_t n = 0, pos = 0;
        const auto &s = stream.str();
        while ((pos = s.find(needle, pos)) != std::string::npos) { ++n; pos += needle.size(); }
        return n;
    }
};
}


TEST(function_processing, simple_function_scalar) {
    auto test_pattern = R"(
        module test_mod #(
        )();

            function integer CTRL_ADDR_CALC();
                CTRL_ADDR_CALC = 67;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));

    auto call = HDL_function_call("CTRL_ADDR_CALC");
    call.propagate_function(std::make_shared<const hdl_function_statement>(functions["CTRL_ADDR_CALC"]));

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("CTRL_ADDR_CALC");

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("CTRL_ADDR_CALC");
    stmt->set_value(std::make_shared<Numeric_token>("67"));
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f,functions["CTRL_ADDR_CALC"]);

    auto values = call.evaluate({});
    ASSERT_TRUE(values.has_value());
    EXPECT_TRUE(values.value().is_integer());
    auto result_value = values.value().get_integer();
    EXPECT_EQ(result_value, 67);
}


TEST(function_processing, simple_function_array) {
    auto test_pattern = R"(
        module test_mod #(
        )();

            function integer CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 100;
                CTRL_ADDR_CALC[1] = 200;
                CTRL_ADDR_CALC[2] = 300;
                IGNORED_ASSIGNMENT[2] = 1;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];
    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("CTRL_ADDR_CALC");

    auto s0 = std::make_shared<hdl_assignment_statement>();
    s0->set_target("CTRL_ADDR_CALC"); s0->set_index(std::make_shared<Numeric_token>("0")); s0->set_value(std::make_shared<Numeric_token>("100"));
    check_f.add_statement(s0);
    auto s1 = std::make_shared<hdl_assignment_statement>();
    s1->set_target("CTRL_ADDR_CALC"); s1->set_index(std::make_shared<Numeric_token>("1")); s1->set_value(std::make_shared<Numeric_token>("200"));
    check_f.add_statement(s1);
    auto s2 = std::make_shared<hdl_assignment_statement>();
    s2->set_target("CTRL_ADDR_CALC"); s2->set_index(std::make_shared<Numeric_token>("2")); s2->set_value(std::make_shared<Numeric_token>("300"));
    check_f.add_statement(s2);
    auto s3 = std::make_shared<hdl_assignment_statement>();
    s3->set_target("IGNORED_ASSIGNMENT"); s3->set_index(std::make_shared<Numeric_token>("2")); s3->set_value(std::make_shared<Numeric_token>("1"));
    check_f.add_statement(s3);
    EXPECT_EQ(check_f,result);
}



TEST(function_processing, simple_loop_function) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 3
        )();

            function logic [31:0] CTRL_ADDR_CALC();
                for(int i = 0; i<3; i++)begin
                    CTRL_ADDR_CALC[i] = 100*i;
                end
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("CTRL_ADDR_CALC");

    auto loop_stmt = std::make_shared<hdl_loop_statement>();
    auto lp = std::make_shared<HDL_parameter>();
    lp->set_name("i"); lp->set_raw_value(std::make_shared<Numeric_token>("0"));
    loop_stmt->set_init(lp);
    Expression_v2 le;
    le.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    le.set_rhs(std::make_shared<Numeric_token>("3"));
    le.set_operation(Expression_v2::less);
    loop_stmt->set_end_condition(std::make_shared<Expression_v2>(le));
    le.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    le.set_rhs(std::make_shared<Numeric_token>("1"));
    le.set_operation(Expression_v2::add);
    loop_stmt->set_iteration(std::make_shared<Expression_v2>(le));
    auto ba = std::make_shared<hdl_assignment_statement>();
    ba->set_target("CTRL_ADDR_CALC");
    ba->set_index(std::make_shared<Identifier_token>(qualified_identifier("i")));
    le.set_lhs(std::make_shared<Numeric_token>("100"));
    le.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    le.set_operation(Expression_v2::multiply);
    ba->set_value(std::make_shared<Expression_v2>(le));
    loop_stmt->add_body_stmt(ba);
    check_f.add_statement(loop_stmt);

    EXPECT_EQ(check_f, result);
}


TEST(function_processing, parametric_loop_function) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 3
        )();

            function logic [31:0] CTRL_ADDR_CALC();
                for(int i = 0; i<N_CORES; i++)begin
                    CTRL_ADDR_CALC[i] = 100*i;
                end
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("CTRL_ADDR_CALC");

    auto ploop = std::make_shared<hdl_loop_statement>();
    auto pp = std::make_shared<HDL_parameter>();
    pp->set_name("i"); pp->set_raw_value(std::make_shared<Numeric_token>("0"));
    ploop->set_init(pp);
    Expression_v2 pe;
    pe.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    pe.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("N_CORES")));
    pe.set_operation(Expression_v2::less);
    ploop->set_end_condition(std::make_shared<Expression_v2>(pe));
    pe.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    pe.set_rhs(std::make_shared<Numeric_token>("1"));
    pe.set_operation(Expression_v2::add);
    ploop->set_iteration(std::make_shared<Expression_v2>(pe));
    auto pba = std::make_shared<hdl_assignment_statement>();
    pba->set_target("CTRL_ADDR_CALC");
    pba->set_index(std::make_shared<Identifier_token>(qualified_identifier("i")));
    pe.set_lhs(std::make_shared<Numeric_token>("100"));
    pe.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    pe.set_operation(Expression_v2::multiply);
    pba->set_value(std::make_shared<Expression_v2>(pe));
    ploop->add_body_stmt(pba);
    check_f.add_statement(ploop);

    EXPECT_EQ(check_f,result);
}

TEST(function_processing, loop_function_with_clog2_local) {
    auto test_pattern = R"(
        module test_mod #(
            INPUT_WIDTH = 16
        )();

            function logic [$clog2(INPUT_WIDTH)-1:0] get_msb_index (input logic [31:0] value);
                integer i;
                logic [$clog2(INPUT_WIDTH)-1:0] msb = 0;
                for (i = 0; i < INPUT_WIDTH; i++)
                    msb = i;
                get_msb_index = msb;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("get_msb_index"));
    auto result = functions["get_msb_index"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("get_msb_index");
    check_f.add_argument("value");

    auto var_i = std::make_shared<HDL_parameter>("i");
    var_i->set_type(Type_engine::create_primitive_type("integer"));
    check_f.add_local_variable(var_i);

    auto var_msb = std::make_shared<HDL_parameter>("msb");
    auto msb_type = std::make_shared<HDL_simple_type>();
    msb_type->set_signed(false);
    msb_type->set_type_name("logic");
    dimension_t dim;
    auto minus_one = std::make_shared<Expression_v2>();
    minus_one->set_lhs(std::make_shared<Numeric_token>("1"));
    minus_one->set_operation(Expression_v2::subtract);
    dim.first_bound = minus_one;
    dim.second_bound = std::make_shared<Numeric_token>("0");
    dim.packed = false;
    msb_type->set_unpacked_dimensions({dim});
    var_msb->set_type(msb_type);
    check_f.add_local_variable(var_msb);

    auto loop_stmt = std::make_shared<hdl_loop_statement>();
    auto lp = std::make_shared<HDL_parameter>();
    lp->set_name("i"); lp->set_raw_value(std::make_shared<Numeric_token>("0"));
    loop_stmt->set_init(lp);
    Expression_v2 le;
    le.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    le.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("INPUT_WIDTH")));
    le.set_operation(Expression_v2::less);
    loop_stmt->set_end_condition(std::make_shared<Expression_v2>(le));
    le.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    le.set_rhs(std::make_shared<Numeric_token>("1"));
    le.set_operation(Expression_v2::add);
    loop_stmt->set_iteration(std::make_shared<Expression_v2>(le));
    auto body_stmt = std::make_shared<hdl_assignment_statement>();
    body_stmt->set_target("msb");
    body_stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("i")));
    loop_stmt->add_body_stmt(body_stmt);
    auto init_msb = std::make_shared<hdl_assignment_statement>();
    init_msb->set_target("msb");
    init_msb->set_value(std::make_shared<Numeric_token>("0"));
    check_f.add_statement(init_msb);
    check_f.add_statement(loop_stmt);

    auto ret_stmt = std::make_shared<hdl_assignment_statement>();
    ret_stmt->set_target("get_msb_index");
    ret_stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("msb")));
    check_f.add_statement(ret_stmt);

    EXPECT_EQ(check_f, result);
}


TEST(function_processing, complex_loop_function) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 3
        )();

            function logic [31:0] CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 44;
                for(int i = 1; i<N_CORES+1; i++)begin
                    CTRL_ADDR_CALC[i] = 100*i;
                end
                CTRL_ADDR_CALC[4] = 667;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("CTRL_ADDR_CALC");

    auto cloop = std::make_shared<hdl_loop_statement>();
    auto cp = std::make_shared<HDL_parameter>();
    cp->set_name("i"); cp->set_raw_value(std::make_shared<Numeric_token>("1"));
    cloop->set_init(cp);
    Expression_v2 ce2;
    ce2.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N_CORES")));
    ce2.set_rhs(std::make_shared<Numeric_token>("1"));
    ce2.set_operation(Expression_v2::add);
    Expression_v2 ce;
    ce.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    ce.set_rhs(std::make_shared<Expression_v2>(ce2));
    ce.set_operation(Expression_v2::less);
    cloop->set_end_condition(std::make_shared<Expression_v2>(ce));
    ce.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    ce.set_rhs(std::make_shared<Numeric_token>("1"));
    ce.set_operation(Expression_v2::add);
    cloop->set_iteration(std::make_shared<Expression_v2>(ce));
    auto cba = std::make_shared<hdl_assignment_statement>();
    cba->set_target("CTRL_ADDR_CALC");
    cba->set_index(std::make_shared<Identifier_token>(qualified_identifier("i")));
    ce.set_lhs(std::make_shared<Numeric_token>("100"));
    ce.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    ce.set_operation(Expression_v2::multiply);
    cba->set_value(std::make_shared<Expression_v2>(ce));
    cloop->add_body_stmt(cba);

    auto ca0 = std::make_shared<hdl_assignment_statement>();
    ca0->set_target("CTRL_ADDR_CALC"); ca0->set_index(std::make_shared<Numeric_token>("0")); ca0->set_value(std::make_shared<Numeric_token>("44"));
    check_f.add_statement(ca0);

    check_f.add_statement(cloop);

    auto ca1 = std::make_shared<hdl_assignment_statement>();
    ca1->set_target("CTRL_ADDR_CALC"); ca1->set_index(std::make_shared<Numeric_token>("4")); ca1->set_value(std::make_shared<Numeric_token>("667"));
    check_f.add_statement(ca1);

    EXPECT_EQ(check_f,result);
}


TEST(function_processing, parametrized_function) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 1
        )();

            function logic [31:0] CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 44;
                CTRL_ADDR_CALC[N_CORES] = 33;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("CTRL_ADDR_CALC");

    auto ps0 = std::make_shared<hdl_assignment_statement>();
    ps0->set_target("CTRL_ADDR_CALC"); ps0->set_index(std::make_shared<Numeric_token>("0")); ps0->set_value(std::make_shared<Numeric_token>("44"));
    check_f.add_statement(ps0);
    auto ps1 = std::make_shared<hdl_assignment_statement>();
    ps1->set_target("CTRL_ADDR_CALC"); ps1->set_index(std::make_shared<Identifier_token>(qualified_identifier("N_CORES"))); ps1->set_value(std::make_shared<Numeric_token>("33"));
    check_f.add_statement(ps1);
    EXPECT_EQ(check_f,result);
}


TEST(function_processing, function_in_package) {
    auto test_pattern = R"(
        package test_pkg;

            function logic[31:0] CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 67;
                CTRL_ADDR_CALC[1] = 100;
            endfunction

        endpackage
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto pkg = resources[0]->as<hdl_package_statement>();

    EXPECT_EQ(pkg.getName(), "test_pkg");
    auto functions = pkg.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("CTRL_ADDR_CALC");

    auto fs0 = std::make_shared<hdl_assignment_statement>();
    fs0->set_target("CTRL_ADDR_CALC"); fs0->set_index(std::make_shared<Numeric_token>("0")); fs0->set_value(std::make_shared<Numeric_token>("67"));
    check_f.add_statement(fs0);
    auto fs1 = std::make_shared<hdl_assignment_statement>();
    fs1->set_target("CTRL_ADDR_CALC"); fs1->set_index(std::make_shared<Numeric_token>("1")); fs1->set_value(std::make_shared<Numeric_token>("100"));
    check_f.add_statement(fs1);
    EXPECT_EQ(check_f,result);
}


TEST(function_processing, package_assignment) {
    auto test_pattern = R"(
        module test_mod ();


        function logic[31:0] CTRL_ADDR_CALC();
            CTRL_ADDR_CALC[0] = hil_address_space::bus_base;
        endfunction

        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];


    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("CTRL_ADDR_CALC");

    auto pas0 = std::make_shared<hdl_assignment_statement>();
    pas0->set_target("CTRL_ADDR_CALC"); pas0->set_index(std::make_shared<Numeric_token>("0"));
    pas0->set_value(std::make_shared<Identifier_token>(qualified_identifier("hil_address_space","bus_base")));
    check_f.add_statement(pas0);

    EXPECT_EQ(check_f,result);

}


TEST(function_processing, local_variable_scalar) {
    auto test_pattern = R"(
        module test_mod #(
        )();

            function integer compute();
                int tmp;
                tmp = 42;
                compute = tmp;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
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
}




TEST(function_processing, conditional_in_function) {
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
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    hdl_conditional_statement check_cond;

    auto expr = std::make_shared<Expression_v2>();
    expr->set_lhs(std::make_shared<Identifier_token>(qualified_identifier("CONDITION")));
    expr->set_rhs(std::make_shared<Numeric_token>("2"));
    expr->set_operation(Expression_v2::equal);
    check_cond.add_branch(expr);
    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(std::make_shared<Numeric_token>("32"));
    check_cond.add_to_branch(stmt);
    stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(std::make_shared<Numeric_token>("47"));
    check_cond.add_to_else(stmt);
    check_f.add_statement(std::make_shared<hdl_conditional_statement>(check_cond));

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, struct_returning_function) {
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
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("compute_addr"));
    auto result = functions["compute_addr"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute_addr");
    check_f.set_return_type(result.get_return_type());

    auto s0 = std::make_shared<hdl_assignment_statement>();
    qualified_identifier t0("base");
    t0.set_instance_prefix({"compute_addr"});
    s0->set_target(t0);
    s0->set_value(std::make_shared<Numeric_token>("32'h1000"));
    check_f.add_statement(s0);
    auto s1 = std::make_shared<hdl_assignment_statement>();
    qualified_identifier t1("size");
    t1.set_instance_prefix({"compute_addr"});
    s1->set_target(t1);
    s1->set_value(std::make_shared<Numeric_token>("32'h400"));
    check_f.add_statement(s1);

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, repro_system_task_in_function_body) {
    // Repro for KNOWN_ISSUES.md #1: `$clog2(a)` parses as bare `a`;
    // the system-task wrapper is dropped.
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                compute = $clog2(a);
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");

    auto field = std::make_shared<Identifier_token>(qualified_identifier("a"));
    auto clog2 = std::make_shared<HDL_builtin_function>(HDL_builtin_function::function::clog2);
    clog2->add_argument(field);

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(clog2);
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, repro_swrite_in_function_body) {
    // Repro for the UVM parse spam (`Unknown system function $swrite ...`):
    // `$swrite(msg, ...)` (uvm_comparer.svh) is a procedural task call in a
    // function body, not a parameter expression.
    auto test_pattern = R"(
        module test_mod #(
        )();
            function void check(string name, int lhs, int rhs);
                string msg;
                $swrite(msg, "%s: lhs=%0d rhs=%0d", name, lhs, rhs);
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    log_capture logs;
    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    EXPECT_EQ(logs.count("Unknown system function $swrite"), 0u);

    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("check"));
    auto result = functions["check"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("check");
    check_f.add_argument("name");
    check_f.add_argument("lhs");
    check_f.add_argument("rhs");

    auto lv = std::make_shared<HDL_parameter>("msg");
    lv->set_type(Type_engine::create_primitive_type("string"));
    check_f.add_local_variable(lv);

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, repro_sformat_in_function_body) {
    // Repro for the UVM parse spam (`Unknown system function $sformat ...`):
    // `$sformat(convert2string, ...)` (uvm_vreg.svh) is a procedural task
    // call in a function body, not a parameter expression.
    auto test_pattern = R"(
        module test_mod #(
        )();
            function string convert2string(string prefix);
                string image;
                $sformat(image, "%sBlock %s", prefix, "foo");
                return image;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    log_capture logs;
    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    EXPECT_EQ(logs.count("Unknown system function $sformat"), 0u);

    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("convert2string"));
    auto result = functions["convert2string"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("convert2string");
    check_f.add_argument("prefix");

    auto lv = std::make_shared<HDL_parameter>("image");
    lv->set_type(Type_engine::create_primitive_type("string"));
    check_f.add_local_variable(lv);

    auto s_ret = std::make_shared<hdl_assignment_statement>();
    s_ret->set_target("convert2string");
    s_ret->set_value(std::make_shared<Identifier_token>(qualified_identifier("image")));
    check_f.add_statement(s_ret);

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, repro_relational_in_function_body) {
    // Repro for KNOWN_ISSUES.md #2: a relational in a non-first statement
    // loses its operator and rhs (`if(a > 32)` -> `if(a)`).
    // As the only statement it parses fine; state seems to leak across statements.
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                compute = a;
                if(a > 32)begin
                    compute = 1;
                end else begin
                    compute = 0;
                end
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");

    auto first = std::make_shared<hdl_assignment_statement>();
    first->set_target("compute");
    first->set_value(std::make_shared<Identifier_token>(qualified_identifier("a")));
    check_f.add_statement(first);

    hdl_conditional_statement check_cond;

    auto cond = std::make_shared<Expression_v2>();
    auto field = std::make_shared<Identifier_token>(qualified_identifier("a"));
    cond->set_lhs(field);
    cond->set_rhs(std::make_shared<Numeric_token>("32"));
    cond->set_operation(Expression_v2::greater);
    check_cond.add_branch(cond);
    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(std::make_shared<Numeric_token>("1"));
    check_cond.add_to_branch(stmt);
    stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(std::make_shared<Numeric_token>("0"));
    check_cond.add_to_else(stmt);
    check_f.add_statement(std::make_shared<hdl_conditional_statement>(check_cond));

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, repro_user_call_in_function_body) {
    // Repro for KNOWN_ISSUES.md #4: `helper(a)` parses as bare `helper`;
    // no call node is built and the argument is lost.
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer helper(input integer x);
                helper = x * 2;
            endfunction

            function integer compute(input integer a);
                compute = helper(a);
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");

    HDL_function_call call("helper");
    call.add_argument(std::make_shared<Identifier_token>(qualified_identifier("a")));

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(std::make_shared<HDL_function_call>(call));
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, repro_ternary_in_function_body) {
    // Repro for KNOWN_ISSUES.md #3: the ternary collapses to its (mangled)
    // condition (`(a > 32) ? b : 0` -> `a>0`).
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a, input integer b);
                compute = (a > 32) ? b : 0;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");
    check_f.add_argument("b");

    auto cond = std::make_shared<Expression_v2>();
    auto cond_field = std::make_shared<Identifier_token>(qualified_identifier("a"));
    cond->set_lhs(cond_field);
    cond->set_rhs(std::make_shared<Numeric_token>("32"));
    cond->set_operation(Expression_v2::greater);
    auto true_field = std::make_shared<Identifier_token>(qualified_identifier("b"));
    auto ternary = std::make_shared<Ternary>();
    ternary->set_condition(cond);
    ternary->set_true_value(true_field);
    ternary->set_false_value(std::make_shared<Numeric_token>("0"));

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(ternary);
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, initialized_local_in_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                int t = a + 10;
                compute = t;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");

    auto lv = std::make_shared<HDL_parameter>("t");
    lv->set_type(Type_engine::create_primitive_type("int"));
    check_f.add_local_variable(lv);

    auto init = std::make_shared<Expression_v2>();
    init->set_lhs(std::make_shared<Identifier_token>(qualified_identifier("a")));
    init->set_rhs(std::make_shared<Numeric_token>("10"));
    init->set_operation(Expression_v2::add);
    auto s_init = std::make_shared<hdl_assignment_statement>();
    s_init->set_target("t");
    s_init->set_value(init);
    check_f.add_statement(s_init);

    auto s_ret = std::make_shared<hdl_assignment_statement>();
    s_ret->set_target("compute");
    s_ret->set_value(std::make_shared<Identifier_token>(qualified_identifier("t")));
    check_f.add_statement(s_ret);

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, case_statement_in_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                case (a)
                    0, 1: compute = 10;
                    2: compute = 20;
                    default: compute = 30;
                endcase
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");
    hdl_conditional_statement check_cond;

    auto sel = std::make_shared<Identifier_token>(qualified_identifier("a"));
    auto eq0 = std::make_shared<Expression_v2>();
    eq0->set_lhs(sel);
    eq0->set_rhs(std::make_shared<Numeric_token>("0"));
    eq0->set_operation(Expression_v2::case_equal);
    auto eq1 = std::make_shared<Expression_v2>();
    eq1->set_lhs(sel);
    eq1->set_rhs(std::make_shared<Numeric_token>("1"));
    eq1->set_operation(Expression_v2::case_equal);
    auto or_cond = std::make_shared<Expression_v2>();
    or_cond->set_lhs(eq0);
    or_cond->set_rhs(eq1);
    or_cond->set_operation(Expression_v2::logical_or);
    check_cond.add_branch(or_cond);
    auto s0 = std::make_shared<hdl_assignment_statement>();
    s0->set_target("compute");
    s0->set_value(std::make_shared<Numeric_token>("10"));
    check_cond.add_to_branch(s0);

    auto eq2 = std::make_shared<Expression_v2>();
    eq2->set_lhs(sel);
    eq2->set_rhs(std::make_shared<Numeric_token>("2"));
    eq2->set_operation(Expression_v2::case_equal);
    check_cond.add_branch(eq2);
    auto s1 = std::make_shared<hdl_assignment_statement>();
    s1->set_target("compute");
    s1->set_value(std::make_shared<Numeric_token>("20"));
    check_cond.add_to_branch(s1);

    check_cond.add_branch(std::make_shared<Numeric_token>("1"));
    auto s2 = std::make_shared<hdl_assignment_statement>();
    s2->set_target("compute");
    s2->set_value(std::make_shared<Numeric_token>("30"));
    check_cond.add_to_branch(s2);

    check_f.add_statement(std::make_shared<hdl_conditional_statement>(check_cond));

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, streaming_in_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function logic [15:0] compute(input logic [7:0] a, input logic [7:0] b);
                compute = {<<{a, b}};
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");
    check_f.add_argument("b");

    auto stream = std::make_shared<Streaming>();
    stream->set_direction(Streaming::left);
    stream->add_component(std::make_shared<Identifier_token>(qualified_identifier("a")));
    stream->add_component(std::make_shared<Identifier_token>(qualified_identifier("b")));

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(stream);
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, result);
}

// Streaming concatenation as an assignment *target* (e.g. UVM do_pack
// bodies: `{ << bit { mem } } = v;`) has no bit-scatter write semantics in
// the evaluator, so the file is gracefully dropped with a warning instead
// of crashing or silently producing a partial function body.
TEST(function_processing, streaming_lvalue_unpack_kept) {
    // UVM do_pack/do_unpack shape: `{<<bit{mem}} = v`.
    auto test_pattern = R"(
        module test_mod #(
        )();
            logic [31:0] mem;
            logic [31:0] v;
            function integer do_pack();
                { << bit { mem } } = v;
                do_pack = 0;
            endfunction
        endmodule
    )";

    log_capture logs;
    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    EXPECT_EQ(logs.count("dropping file"), 0u);

    auto functions = resource.get_functions();
    ASSERT_TRUE(functions.contains("do_pack"));

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("do_pack");

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("mem");
    stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("v")));
    check_f.add_statement(stmt);

    auto ret = std::make_shared<hdl_assignment_statement>();
    ret->set_target("do_pack");
    ret->set_value(std::make_shared<Numeric_token>("0"));
    check_f.add_statement(ret);

    EXPECT_EQ(check_f, functions["do_pack"]);
}

TEST(function_processing, streaming_lvalue_multi_member_approximated) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            logic [31:0] a;
            logic [31:0] b;
            logic [63:0] v;
            function integer do_pack();
                { << bit { a, b } } = v;
                do_pack = 0;
            endfunction
        endmodule
    )";

    log_capture logs;
    sv_analyzer analyzer;

    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    EXPECT_EQ(logs.count("dropping file"), 0u);
    EXPECT_EQ(logs.count("approximating as assignment"), 1u);

    auto functions = resource.get_functions();
    ASSERT_TRUE(functions.contains("do_pack"));

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("do_pack");

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("a");
    stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("v")));
    check_f.add_statement(stmt);

    auto ret = std::make_shared<hdl_assignment_statement>();
    ret->set_target("do_pack");
    ret->set_value(std::make_shared<Numeric_token>("0"));
    check_f.add_statement(ret);

    EXPECT_EQ(check_f, functions["do_pack"]);
}

TEST(function_processing, anonymous_struct_local_in_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                struct packed { logic [7:0] hi; logic [7:0] lo; } tmp;
                compute = a;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    for (const auto &d : resource.get_parameter_statements())
        EXPECT_FALSE(d->get_name() == "tmp");
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");

    HDL_struct_type check_struct;
    check_struct.packed = true;
    HDL_simple_type byte_type;
    byte_type.set_packed_dimensions({
        {std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true}
    });
    struct_member m;
    m.name = "hi";
    m.type = std::make_shared<HDL_simple_type>(byte_type);
    check_struct.member.push_back(m);
    m.name = "lo";
    m.type = std::make_shared<HDL_simple_type>(byte_type);
    check_struct.member.push_back(m);
    auto lv = std::make_shared<HDL_parameter>("tmp");
    lv->set_type(std::make_shared<HDL_struct_type>(check_struct));
    check_f.add_local_variable(lv);

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("a")));
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, anonymous_enum_local_in_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                enum {IDLE, RUN, DONE} state;
                compute = a;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    for (const auto &d : resource.get_parameter_statements())
        EXPECT_FALSE(d->get_name() == "state");
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");

    HDL_enum_type check_enum;
    enum_member m;
    m.name = "IDLE";
    m.value = 0;
    check_enum.members.push_back(m);
    m.name = "RUN";
    m.value = 1;
    check_enum.members.push_back(m);
    m.name = "DONE";
    m.value = 2;
    check_enum.members.push_back(m);
    auto lv = std::make_shared<HDL_parameter>("state");
    lv->set_type(std::make_shared<HDL_enum_type>(check_enum));
    check_f.add_local_variable(lv);

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("a")));
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, result);
}

TEST(function_processing, return_statement_in_function) {
    auto test_pattern = R"(
        module test_mod #(
        )();
            function integer compute(input integer a);
                return a + 1;
            endfunction
        endmodule
    )";

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("",test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto functions = resource.get_functions();

    ASSERT_TRUE(functions.contains("compute"));
    auto result = functions["compute"];

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("compute");
    check_f.add_argument("a");

    auto ret = std::make_shared<Expression_v2>();
    ret->set_lhs(std::make_shared<Identifier_token>(qualified_identifier("a")));
    ret->set_rhs(std::make_shared<Numeric_token>("1"));
    ret->set_operation(Expression_v2::add);

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("compute");
    stmt->set_value(ret);
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, result);
}





TEST(function_processing, foreach_with_indexed_array) {
    // Reduced from riscv_pmp_cfg.sv: `foreach (pmp_cfg[i])` with an explicit
    // loop variable must parse, both as a procedural statement and inside a
    // constraint block (the actual failing shape: the constraint-foreach
    // alternative ate `[i]` as a primary index and then demanded another
    // `[` at `)`).
    auto test_pattern = R"(
        module test_mod;
            int pmp_cfg[16];
            initial begin
                foreach (pmp_cfg[i]) begin
                    pmp_cfg[i] = i;
                end
            end
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(function_processing, foreach_with_indexed_array_in_constraint) {
    auto test_pattern = R"(
        class test_class;
            int pmp_cfg[16];
            constraint xwr_c {
                foreach (pmp_cfg[i]) {
                    solve pmp_cfg[i] before pmp_cfg[i];
                    !(pmp_cfg[i] && !pmp_cfg[i]);
                }
            }
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_FALSE(analyzer.has_syntax_errors());
}

TEST(function_processing, streaming_lvalue_in_foreach_body_kept) {
    // UVM array-field macros expand unpacks inside foreach bodies.
    auto test_pattern = R"(
        module test_mod();
            logic [31:0] arr[4];
            logic [31:0] v;
            function void do_pack();
                foreach (arr[i]) begin
                    bit __array[];
                    { << bit { __array}} = v;
                    arr[i] = __array[0];
                end
            endfunction
        endmodule
    )";

    log_capture logs;
    sv_analyzer analyzer;
    auto result = analyzer.analyze("", test_pattern);
    ASSERT_TRUE(result.has_value());
    EXPECT_EQ(logs.count("dropping file"), 0u);
}

TEST(function_processing, streaming_lvalue_strict_drops) {
    auto test_pattern = R"(
        module test_mod();
            logic [31:0] mem;
            logic [31:0] v;
            function integer do_pack();
                { << bit { mem } } = v;
                do_pack = 0;
            endfunction
        endmodule
    )";

    parse_options opts;
    opts.strict = true;

    sv_analyzer analyzer;
    analyzer.set_options(opts);

    EXPECT_FALSE(analyzer.analyze("", test_pattern).has_value());
}

TEST(function_processing, concatenation_lvalue_recorded) {
    // dasm bit-scatter shape (`{imm[5:4],...} = ...`): members behave exactly
    // like the equivalent sequence of single assignments (whole-var, selects
    // as today), so the file is kept instead of dropped.
    auto test_pattern = R"(
        module test_mod();
            int imm;
            logic [31:0] opcode;
            function void scatter();
                {imm[5:4],imm[9:6],imm[2],imm[3]} = opcode;
            endfunction
        endmodule
    )";


    log_capture logs;
    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto fns = resource->get_functions();
    ASSERT_TRUE(fns.contains("scatter"));

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("scatter");

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_targets({qualified_identifier("imm"), qualified_identifier("imm"),
                       qualified_identifier("imm"), qualified_identifier("imm")});
    stmt->set_indices({nullptr, nullptr, std::make_shared<Numeric_token>("2"),
                       std::make_shared<Numeric_token>("3")});
    stmt->set_member_selects({{5, 4}, {9, 6}, {-1, -1}, {-1, -1}});
    stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("opcode")));
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, fns.at("scatter"));
    EXPECT_EQ(logs.count("dropping file"), 0u);
    EXPECT_EQ(logs.count("recording 4 member(s)"), 1u);
}

TEST(function_processing, concatenation_lvalue_vector_targets) {
    // One statement holds every member: the multi-target proof.
    auto test_pattern = R"(
        module test_mod();
            int a;
            int b;
            logic [31:0] v;
            function void both();
                {a, b} = v;
            endfunction
        endmodule
    )";


    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto fns = resource->get_functions();
    ASSERT_TRUE(fns.contains("both"));

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("both");

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_targets({qualified_identifier("a"), qualified_identifier("b")});
    stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("v")));
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, fns.at("both"));
}

TEST(function_processing, concatenation_lvalue_nested) {
    auto test_pattern = R"(
        module test_mod();
            int a;
            int b;
            int c;
            logic [31:0] v;
            function void nested();
                {{a, b}, c} = v;
            endfunction
        endmodule
    )";


    log_capture logs;
    sv_analyzer analyzer;

    auto resource = std::static_pointer_cast<hdl_resource_statement>(analyzer.analyze("", test_pattern).value().get_content()[0]);
    auto fns = resource->get_functions();
    ASSERT_TRUE(fns.contains("nested"));

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("nested");

    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_targets({qualified_identifier("a"), qualified_identifier("b"), qualified_identifier("c")});
    stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("v")));
    check_f.add_statement(stmt);

    EXPECT_EQ(check_f, fns.at("nested"));
    EXPECT_EQ(logs.count("dropping file"), 0u);
}

TEST(function_processing, concatenation_lvalue_exotic_member_drops) {
    // A streaming member cannot be represented: the whole statement declines
    // to the pre-existing drop path.
    auto test_pattern = R"(
        module test_mod();
            int a;
            int b;
            function integer exotic(input[31:0] v);
                {{<<bit{a}}, b} = v;
                return a + b;
            endfunction
        endmodule
    )";


    sv_analyzer analyzer;
    EXPECT_FALSE(analyzer.analyze("", test_pattern).has_value());
}

TEST(function_processing, concatenation_lvalue_strict_drops) {
    auto test_pattern = R"(
        module test_mod();
            int imm;
            function integer scatter(input[31:0] opcode);
                {imm[5:4],imm[9:6],imm[2],imm[3]} = opcode[12:5];
                return imm;
            endfunction
        endmodule
    )";


    parse_options opts;
    opts.strict = true;

    sv_analyzer analyzer;
    analyzer.set_options(opts);

    EXPECT_FALSE(analyzer.analyze("", test_pattern).has_value());
}

