// Copyright 2021 Filippo Savi
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.



#include <gtest/gtest.h>

#include "frontend/analysis/system_verilog/sv_analyzer.hpp"
#include "frontend/analysis/vhdl/vhdl_analyzer.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"


TEST( analysis_test , package) {
    auto  test_file = mm_file("check_files/test_package.sv");
    sv_analyzer analyzer;

    
    auto resource = analyzer.analyze("", test_file.view()) .get_content()[0]->as<hdl_resource_statement>();

    Parameters_map parameters = resource.get_parameters();

    Parameters_map check_map;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("bus_base");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e;
    p->set_raw_value(std::make_shared<Numeric_token>("32'h43c00000"));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("timebase");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("bus_base")));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("gpio");
    p->set_type(Type_engine::create_primitive_type("implicit"));

    Expression_v2 e2, e3, e4;
    e4.set_lhs(std::make_shared<Numeric_token>("32'h1000"));
    e4.set_rhs(std::make_shared<Numeric_token>("2"));
    e4.set_operation(Expression_v2::multiply);
    e3.set_rhs(std::make_shared<Numeric_token>("2"));
    e3.set_lhs(std::make_shared<Expression_v2>(e4));
    e3.set_operation(Expression_v2::divide);
    e2.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("timebase")));
    e2.set_rhs(std::make_shared<Expression_v2>(e3));
    e2.set_operation(Expression_v2::add);
    e.set_lhs(std::make_shared<Expression_v2>(e2));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::add);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("scope_mux");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("gpio")));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("out_of_order");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("scope_mux")));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("modulo_parameter");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Numeric_token>("3"));
    e.set_rhs(std::make_shared<Numeric_token>("2"));
    e.set_operation(Expression_v2::modulo);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("subtraction_parameter");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e.set_lhs(std::make_shared<Numeric_token>("'o4"));
    e.set_rhs(std::make_shared<Numeric_token>("'b10"));
    e.set_operation(Expression_v2::subtract);
    p->set_raw_value(std::make_shared<Expression_v2>(e));
    check_map.insert(p);

    ASSERT_EQ(check_map, parameters);

}




TEST( analysis_test , sv_module) {
    auto test_pattern = R"(
        `timescale 10 ns / 1 ns

        module Decoder (
            input wire clock,
            input wire reset,
            axi_stream.slave data_in,
            axi_stream.master data_out
        );

            parameter module_parameter_1 = 56;
            localparam module_parameter_2 = 74;

            axi_lite if_array[module_parameter_2+1]();

            reg [31:0] memory [5:0];
            initial memory = $readmemh("mem/init/file.dat");

            SyndromeCalculator #(
                .TEST_PARAM(test_package::param)
            ) SC (
                .clock(clock),
                .reset(reset),
                .data_in(data_in),
                .syndrome(data_out)
            );

        endmodule


        interface test_if;

            logic signal_1;
            logic signal_2;
        endinterface
    )";
    sv_analyzer analyzer;
    auto res = analyzer.analyze("/test/file.sv",test_pattern).get_content();
    auto resource = res[0]->as<hdl_resource_statement>();


    std::unordered_map<std::string, HDL_port> test_ports;

    test_ports["clock"] = {input_port};
    test_ports["reset"] = {input_port};
    test_ports["data_in"] = {interface_port, {"axi_stream", "slave"}};
    test_ports["data_out"] = {interface_port, {"axi_stream", "master"}};

    hdl_resource_statement check_res;
    check_res.set_name("Decoder");
    check_res.set_type(module);
    check_res.set_line_n(3);
    check_res.set_ports(test_ports);

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("module_parameter_1");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("56"));
    check_res.add_parameter(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("module_parameter_2");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>("74"));
    check_res.add_parameter(p);

    auto sc = std::make_shared<hdl_instance_statement>();
    sc->set_name("SC");
    sc->set_type("SyndromeCalculator");
    sc->set_dependency_class(module);
    sc->add_port_connection("clock", {HDL_net("clock")});
    sc->add_port_connection("reset", {HDL_net("reset")});
    sc->add_port_connection("data_in", {HDL_net("data_in")});
    sc->add_port_connection("syndrome", {HDL_net("data_out")});
    auto pkg_param = std::make_shared<HDL_parameter>();
    pkg_param->set_name("TEST_PARAM");
    pkg_param->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("test_package", "param")));
    sc->add_parameter(pkg_param);

    auto if_array = std::make_shared<hdl_instance_statement>();
    if_array->set_name("if_array");
    if_array->set_type("axi_lite");
    if_array->set_dependency_class(module);
    auto quant = std::make_shared<HDL_parameter>();
    quant->set_name("instance_array_qualifier");
    Expression_v2 qe;
    qe.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("module_parameter_2")));
    qe.set_rhs(std::make_shared<Numeric_token>("1"));
    qe.set_operation(Expression_v2::add);
    quant->set_raw_value(std::make_shared<Expression_v2>(qe));
    if_array->set_array_quantifier(quant);

    auto init_file = std::make_shared<hdl_instance_statement>();
    init_file->set_name("__init_file__");
    init_file->set_type("file");
    init_file->set_dependency_class(memory_init);

    auto pkg_stmt = std::make_shared<hdl_instance_statement>();
    pkg_stmt->set_name("param");
    pkg_stmt->set_type("test_package");
    pkg_stmt->set_dependency_class(package);

    check_res.add_statement(if_array);
    check_res.add_statement(init_file);
    check_res.add_statement(pkg_stmt);
    check_res.add_statement(sc);


    ASSERT_EQ(resource, check_res);

    resource = res[1]->as<hdl_resource_statement>();
    check_res = hdl_resource_statement();
    check_res.set_name("test_if");
    check_res.set_type(interface);
    check_res.set_line_n(30);
    ASSERT_EQ(resource, check_res);
}

TEST( analysis_test , vhdl_module) {

    vhdl_analyzer analyzer("check_files/test_vhdl_module.vhd");
    analyzer.cleanup_content("`(.*)");
    auto resource = analyzer.analyze().get_content()[0]->as<hdl_resource_statement>();
    hdl_resource_statement check_res;
    check_res.set_name("half_adder");
    check_res.set_type(module);
    check_res.set_line_n(4);
    auto stmt = std::make_shared<hdl_instance_statement>();
    stmt->set_name("and_component");
    stmt->set_type("ANDGATE");
    stmt->set_dependency_class(module);
    check_res.add_statement(stmt);
    ASSERT_EQ(resource, check_res);
}


TEST(analysis_test, port_concat_assignment) {
    auto test_pattern = R"(
    module test_mod ();


        axil_crossbar_interface  axi_xbar (
            .clock(clock),
            .slaves('{axi_in}),
            .masters('{timebase_axi, gpio_axi, fcore_axi})
        );


    endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(resource.get_statements()[0]);
    ASSERT_NE(stmt, nullptr);

    auto check_stmt = std::make_shared<hdl_instance_statement>();
    check_stmt->set_type("axil_crossbar_interface");
    check_stmt->set_name("axi_xbar");
    check_stmt->set_dependency_class(module);
    check_stmt->add_port_connection("clock", {HDL_net("clock")});
    check_stmt->add_port_connection("slaves", {HDL_net("axi_in")});
    check_stmt->add_port_connection("masters", {HDL_net("timebase_axi"), HDL_net("gpio_axi"), HDL_net("fcore_axi")});

    ASSERT_EQ(*stmt, *check_stmt);
}


TEST(analysis_test, interfaces_array) {
    auto test_pattern =R"(
    module test_mod ();

        axi_lite  cores_control[3]();

    endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(resource.get_statements()[0]);
    ASSERT_NE(stmt, nullptr);

    auto check_stmt = std::make_shared<hdl_instance_statement>();
    check_stmt->set_type("axi_lite");
    check_stmt->set_name("cores_control");
    check_stmt->set_dependency_class(module);
    auto array_qual = std::make_shared<HDL_parameter>();
    array_qual->set_name("instance_array_qualifier");
    array_qual->set_raw_value(std::make_shared<Numeric_token>("3"));
    check_stmt->set_array_quantifier(array_qual);
    ASSERT_EQ(*stmt, *check_stmt);
}



TEST(analysis_test, parameter_array_assignment) {
    auto test_pattern = R"(
    module test_mod ();
        integer i;

            SyndromeCalculator #(
                .TEST_PARAM(TEST_ARRAY[2])
            ) SC (
                .clock(clock)
            );

    endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto parameters = resource.get_parameters();

    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(resource.get_statements()[0]);
    ASSERT_NE(stmt, nullptr);
    auto param = stmt->get_parameters().const_get("TEST_PARAM");

    HDL_parameter reference_param;
    reference_param.set_name("TEST_PARAM");
    Identifier_token t(qualified_identifier("TEST_ARRAY"));
    t.add_array_index(std::make_shared<Numeric_token>("2"));
    reference_param.set_raw_value(std::make_shared<Identifier_token>(t));

    ASSERT_EQ(reference_param, *param);
}


TEST(analysis_test, included_declaration) {
    auto test_pattern = R"(
        `include "/tmp/include_test.svh"
        module test_module ();
            parameter TEST_PARAM = 6;
        endmodule
    )";

    std::ofstream ofs("/tmp/include_test.svh");
    if (!ofs.is_open()) {
        std::cerr << "CRITICAL ERROR: Could not create the file in /tmp!" << std::endl;
    }
    ofs<< "module test_module_2 ();\n wire test_wire;\nassign test_wire = 5;\nendmodule;"<< std::endl;

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("/tmp/file.sv",test_pattern).get_content();
    EXPECT_EQ(resource[0]->as<hdl_resource_statement>().getName(), "test_module_2");
    EXPECT_EQ(resource[1]->as<hdl_resource_statement>().getName(), "test_module");
}




TEST(analysis_test, nested_included_declaration) {
    auto test_pattern = R"(
        `include "/tmp/include_test.svh"
        module test_module ();
            parameter TEST_PARAM = 6;
        endmodule
    )";

    std::ofstream ofs("/tmp/include_test.svh");

    ofs<< "\n\n`include \"/tmp/include_nested.svh\"\n\n\nmodule test_module_2 ();\n wire test_wire;\nassign test_wire = 5;\nendmodule;";
    ofs.close();

    ofs = std::ofstream("/tmp/include_nested.svh");

    ofs<< "module test_module_3 ();\n wire test_wire;\nassign test_wire = 4;\nendmodule;";
    ofs.close();

    sv_analyzer analyzer;

    auto resource = analyzer.analyze("/tmp/file.sv",test_pattern).get_content();
    EXPECT_EQ(resource[0]->as<hdl_resource_statement>().getName(), "test_module_3");
    EXPECT_EQ(resource[1]->as<hdl_resource_statement>().getName(), "test_module_2");
    EXPECT_EQ(resource[2]->as<hdl_resource_statement>().getName(), "test_module");
}


TEST(analysis_test, generate_for_loop) {
    auto test_pattern = R"(
        module dependency();
        endmodule

        module test_mod();
            genvar n;
            generate
                for (n = 0; n < 3; n = n + 1) begin
                    dependency d();
                end
            endgenerate
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).get_content()[1]->as<hdl_resource_statement>();

    hdl_loop_statement expected;

    auto init = std::make_shared<HDL_parameter>();
    init->set_name("n");
    init->set_raw_value(std::make_shared<Numeric_token>("0"));
    expected.set_init(init);

    Expression_v2 end_cond;
    end_cond.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("n")));
    end_cond.set_rhs(std::make_shared<Numeric_token>("3"));
    end_cond.set_operation(Expression_v2::less);
    expected.set_end_condition(std::make_shared<Expression_v2>(end_cond));

    Expression_v2 iter;
    iter.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("n")));
    iter.set_rhs(std::make_shared<Numeric_token>("1"));
    iter.set_operation(Expression_v2::add);
    expected.set_iteration(std::make_shared<Expression_v2>(iter));

    auto inst = std::make_shared<hdl_instance_statement>();
    inst->set_name("d");
    inst->set_type("dependency");
    inst->set_dependency_class(module);
    expected.add_body_stmt(inst);

    const auto& stmts = resource.get_statements();
    ASSERT_EQ(stmts.size(), 1);
    auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmts[0]);
    ASSERT_NE(loop, nullptr);
    ASSERT_EQ(*loop, expected);
}