//  Copyright 2026 Filippo Savi
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
#include "data_model/HDL/statement/hdl_conditional_statement.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"


TEST(conditional_processing, simple_if_generate) {
    auto test_pattern = R"(
        module test_mod #(
            parameter ENABLE = 1
        )();
            generate
                if (ENABLE == 1) begin
                    child if_inst();
                end else begin
                    child2 else_inst();
                end
            endgenerate
        endmodule
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    auto content = resources.get_content()[0]->as<hdl_resource_statement>();
    ASSERT_TRUE(content.get_statements()[0]->is<HDL_parameter>());
    ASSERT_EQ(content.get_statements()[0]->as<HDL_parameter>().get_name(), "ENABLE");
    auto result = content.get_statements()[1]->as<hdl_conditional_statement>();

    hdl_conditional_statement check_cond;
    Expression_v2 condition;
    condition.set_operation(Expression_v2::equal);
    condition.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("ENABLE")));
    condition.set_rhs(std::make_shared<Numeric_token>(1, 32));
    check_cond.add_branch(std::make_shared<Expression_v2>(condition));

    hdl_instance_statement inst;
    inst.set_name("if_inst");
    inst.set_type("child");
    check_cond.add_to_branch(std::make_shared<hdl_instance_statement>(inst));
    inst.set_name("else_inst");
    inst.set_type("child2");
    check_cond.add_to_else(std::make_shared<hdl_instance_statement>(inst));

    EXPECT_EQ(result, check_cond);
    EXPECT_EQ(result, check_cond);

}


TEST(conditional_processing, else_if_chain) {
    auto test_pattern = R"(
        module test_mod #(
            parameter MODE = 0
        )();
            generate
                if (MODE == 0) begin
                    child_a inst_0();
                end else if (MODE == 1) begin
                    child_b inst_1();
                end else begin
                    child_c inst_2();
                end
            endgenerate
        endmodule
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    auto content = resources.get_content()[0]->as<hdl_resource_statement>();
    ASSERT_TRUE(content.get_statements()[0]->is<HDL_parameter>());
    ASSERT_EQ(content.get_statements()[0]->as<HDL_parameter>().get_name(), "MODE");
    auto result = content.get_statements()[1]->as<hdl_conditional_statement>();

    hdl_conditional_statement check_cond;
    Expression_v2 condition;
    condition.set_operation(Expression_v2::equal);
    condition.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("MODE")));
    condition.set_rhs(std::make_shared<Numeric_token>(0, 32));
    check_cond.add_branch(std::make_shared<Expression_v2>(condition));

    hdl_instance_statement inst;
    inst.set_name("inst_0");
    inst.set_type("child_a");
    check_cond.add_to_branch(std::make_shared<hdl_instance_statement>(inst));

    condition.set_rhs(std::make_shared<Numeric_token>(1, 32));
    check_cond.add_branch(std::make_shared<Expression_v2>(condition));
    inst.set_name("inst_1");
    inst.set_type("child_b");
    check_cond.add_to_branch(std::make_shared<hdl_instance_statement>(inst));

    inst.set_name("inst_2");
    inst.set_type("child_c");
    check_cond.add_to_else(std::make_shared<hdl_instance_statement>(inst));

    EXPECT_EQ(result, check_cond);
}


TEST(conditional_processing, nested_if_in_then) {
    auto test_pattern = R"(
        module test_mod #(
            parameter A = 1,
            parameter B = 1
        )();

            generate
                if (A == 1) begin
                    child_a inst_0();
                    if (B == 1) begin
                        child_b inst_1();
                    end
                end
            endgenerate

        endmodule
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    auto content = resources.get_content()[0]->as<hdl_resource_statement>();
    ASSERT_TRUE(content.get_statements()[0]->is<HDL_parameter>());
    ASSERT_EQ(content.get_statements()[0]->as<HDL_parameter>().get_name(), "A");
    ASSERT_TRUE(content.get_statements()[1]->is<HDL_parameter>());
    ASSERT_EQ(content.get_statements()[1]->as<HDL_parameter>().get_name(), "B");
    auto result = content.get_statements()[2]->as<hdl_conditional_statement>();

    hdl_conditional_statement check_cond;

    Expression_v2 cond_a;
    cond_a.set_operation(Expression_v2::equal);
    cond_a.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("A")));
    cond_a.set_rhs(std::make_shared<Numeric_token>(1, 32));
    check_cond.add_branch(std::make_shared<Expression_v2>(cond_a));

    hdl_instance_statement inst_0;
    inst_0.set_name("inst_0");
    inst_0.set_type("child_a");
    check_cond.add_to_branch(std::make_shared<hdl_instance_statement>(inst_0));

    hdl_conditional_statement inner;
    Expression_v2 cond_b;
    cond_b.set_operation(Expression_v2::equal);
    cond_b.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("B")));
    cond_b.set_rhs(std::make_shared<Numeric_token>(1, 32));
    inner.add_branch(std::make_shared<Expression_v2>(cond_b));

    hdl_instance_statement inst_1;
    inst_1.set_name("inst_1");
    inst_1.set_type("child_b");
    inner.add_to_branch(std::make_shared<hdl_instance_statement>(inst_1));

    check_cond.add_to_branch(std::make_shared<hdl_conditional_statement>(inner));

    EXPECT_EQ(result, check_cond);
}
