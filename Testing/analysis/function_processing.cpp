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
#include "data_model/HDL/statement/hdl_assignment_statement.hpp"


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
    
    auto resource = analyzer.analyze("",test_pattern)[0];
    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));

    auto call = HDL_function_call("CTRL_ADDR_CALC");
    call.propagate_function(functions["CTRL_ADDR_CALC"]);

    hdl_function_statement check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    assignment a("CTRL_ADDR_CALC", std::nullopt, std::make_shared<Numeric_token>("67"));
    check_f.add_assignment(a);

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
    
    auto resource = analyzer.analyze("", test_pattern)[0];
    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];
    hdl_function_statement check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    assignment a(
        "CTRL_ADDR_CALC",
        std::make_shared<Numeric_token>("0"),
        std::make_shared<Numeric_token>("100")
        );
    check_f.add_assignment(a);
    a = assignment(
        "CTRL_ADDR_CALC",
        std::make_shared<Numeric_token>("1"),
        std::make_shared<Numeric_token>("200")
        );
    check_f.add_assignment(a);
    a = assignment(
        "CTRL_ADDR_CALC",
        std::make_shared<Numeric_token>("2"),
        std::make_shared<Numeric_token>("300")
        );
    check_f.add_assignment(a);
    a = assignment(
        "IGNORED_ASSIGNMENT",
        std::make_shared<Numeric_token>("2"),
        std::make_shared<Numeric_token>("1")
        );
    check_f.add_assignment(a);
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
    
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    auto metadata = HDL_loop_metadata();

    HDL_parameter p;
    p.set_name("i");
    p.set_raw_value(std::make_shared<Numeric_token>("0"));
    metadata.set_init(p);

    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    e.set_rhs(std::make_shared<Numeric_token>("3"));
    e.set_operation(Expression_v2::less);
    metadata.set_end_c(e);

    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::add);
    metadata.set_iter(e);

    e.set_lhs(std::make_shared<Numeric_token>("100"));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    e.set_operation(Expression_v2::multiply);
    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Identifier_token>(qualified_identifier("i")),
        std::make_shared<Expression_v2>(e)
    };

    metadata.add_assignment(a);
    check_f.add_loop_metadata(metadata);

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
    
    auto resource = analyzer.analyze("",test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    auto metadata = HDL_loop_metadata();

    HDL_parameter p;
    p.set_name("i");
    p.set_raw_value(std::make_shared<Numeric_token>("0"));
    metadata.set_init(p);

    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("N_CORES")));
    e.set_operation(Expression_v2::less);
    metadata.set_end_c(e);

    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::add);
    metadata.set_iter(e);


    e.set_lhs(std::make_shared<Numeric_token>("100"));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    e.set_operation(Expression_v2::multiply);
    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Identifier_token>(qualified_identifier("i")),
        std::make_shared<Expression_v2>(e)
    };

    metadata.add_assignment(a);
    check_f.add_loop_metadata(metadata);

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
    
    auto resource = analyzer.analyze("",test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    auto metadata = HDL_loop_metadata();

    HDL_parameter p;
    p.set_name("i");
    p.set_raw_value(std::make_shared<Numeric_token>("1"));
    metadata.set_init(p);
    Expression_v2 e, e2;
    e2.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N_CORES")));
    e2.set_rhs(std::make_shared<Numeric_token>("1"));
    e2.set_operation(Expression_v2::add);
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    e.set_rhs(std::make_shared<Expression_v2>(e2));
    e.set_operation(Expression_v2::less);
    metadata.set_end_c(e);

    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::add);
    metadata.set_iter(e);

    e.set_lhs(std::make_shared<Numeric_token>("100"));
    e.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("i")));
    e.set_operation(Expression_v2::multiply);

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Identifier_token>(qualified_identifier("i")),
        std::make_shared<Expression_v2>(e)
    };

    metadata.add_assignment(a);
    check_f.add_loop_metadata(metadata);

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

    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Numeric_token>("0"),
        std::make_shared<Numeric_token>("44")
    };
    check_f.add_assignment(a);
    auto ca0 = std::make_shared<hdl_assignment_statement>();
    ca0->set_target("CTRL_ADDR_CALC"); ca0->set_index(std::make_shared<Numeric_token>("0")); ca0->set_value(std::make_shared<Numeric_token>("44"));
    check_f.add_statement(ca0);

    check_f.add_statement(cloop);

    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Numeric_token>("4"),
        std::make_shared<Numeric_token>("667")
    };
    check_f.add_assignment(a);
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
    
    auto resource = analyzer.analyze("",test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_name("CTRL_ADDR_CALC");

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Numeric_token>("0"),
        std::make_shared<Numeric_token>("44")
    };
    check_f.add_assignment(a);
    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Identifier_token>(qualified_identifier("N_CORES")),
        std::make_shared<Numeric_token>("33")
    };
    check_f.add_assignment(a);
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

    auto resources = analyzer.analyze("", test_pattern);
    auto& pkg = resources[0];

    EXPECT_EQ(pkg.get_type(), package);
    EXPECT_EQ(pkg.getName(), "test_pkg");
    auto functions = pkg.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];

    hdl_function_statement check_f;
    check_f.set_name("CTRL_ADDR_CALC");
    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Numeric_token>("0"),
        std::make_shared<Numeric_token>("67")
    };
    check_f.add_assignment(a);
    a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Numeric_token>("1"),
        std::make_shared<Numeric_token>("100")
    };
    check_f.add_assignment(a);
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
    
    auto resource = analyzer.analyze("", test_pattern)[0];

    auto functions = resource.get_functions();

    EXPECT_EQ(functions.size(), 1);
    EXPECT_TRUE(functions.contains("CTRL_ADDR_CALC"));
    auto result = functions["CTRL_ADDR_CALC"];


    hdl_function_statement check_f;
    check_f.set_name("CTRL_ADDR_CALC");

    assignment a = {
        "CTRL_ADDR_CALC",
        std::make_shared<Numeric_token>("0"),
        std::make_shared<Identifier_token>(qualified_identifier("hil_address_space","bus_base"))
    };
    check_f.add_assignment(a);

    auto pas0 = std::make_shared<hdl_assignment_statement>();
    pas0->set_target("CTRL_ADDR_CALC"); pas0->set_index(std::make_shared<Numeric_token>("0"));
    pas0->set_value(std::make_shared<Identifier_token>(qualified_identifier("hil_address_space","bus_base")));
    check_f.add_statement(pas0);

    EXPECT_EQ(check_f,result);

}



