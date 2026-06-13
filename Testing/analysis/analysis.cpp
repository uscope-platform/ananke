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

#include "frontend/analysis/sv_analyzer.hpp"
#include "frontend/analysis/vhdl_analyzer.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"


TEST( analysis_test , package) {
    auto  test_file = mm_file("check_files/test_package.sv");
    sv_analyzer analyzer;

    
    auto resource = analyzer.analyze("", test_file.view())[0];

    Parameters_map parameters = resource.get_parameters();

    Parameters_map check_map;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("bus_base");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression e = {Token("32'h43c00000", Token::number)};
    p->set_raw_value(std::make_shared<Expression>(e));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("timebase");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = { Token("bus_base", Token::identifier)};
    p->set_raw_value(std::make_shared<Expression>(e));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("gpio");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = {
        Token("timebase", Token::identifier), Token(Token::add),
        Token("32'h1000", Token::number), Token(Token::multiply),
        Token("2", Token::number), Token(Token::divide),
        Token("2", Token::number), Token(Token::add),
        Token("1", Token::number)
    };
    p->set_raw_value(std::make_shared<Expression>(e));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("scope_mux");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = { Token("gpio", Token::identifier)};
    p->set_raw_value(std::make_shared<Expression>(e));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("out_of_order");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = { Token("scope_mux", Token::identifier)};
    p->set_raw_value(std::make_shared<Expression>(e));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("modulo_parameter");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = {
        Token("3", Token::number),Token(Token::modulo),Token("2", Token::number)
    };
    p->set_raw_value(std::make_shared<Expression>(e));
    check_map.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("subtraction_parameter");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    e = {
        Token("'o4", Token::number),Token(Token::subtract),Token("'b10", Token::number)
    };
    p->set_raw_value(std::make_shared<Expression>(e));
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
    auto res = analyzer.analyze("/test/file.sv",test_pattern);
    auto resource = res[0];

    HDL_instance d3("SC", "SyndromeCalculator", module);
    d3.add_port_connection("clock", {HDL_net("clock")});
    d3.add_port_connection("reset", {HDL_net("reset")});
    d3.add_port_connection("data_in", {HDL_net("data_in")});
    d3.add_port_connection("syndrome", {HDL_net("data_out")});
    auto p = std::make_shared<HDL_parameter>();
    p->set_name("TEST_PARAM");
    Token ec("param", Token::identifier);
    ec.set_package_prefix("test_package");
    Expression e = {ec};
    p->set_raw_value(std::make_shared<Expression>(e));
    d3.add_parameter(p);

    HDL_instance d2("param", "test_package", package);
    HDL_instance d1("__init_file__", "file", memory_init);
    HDL_instance d0("if_array", "axi_lite", module);
    p = std::make_shared<HDL_parameter>();
    p->set_name("instance_array_qualifier");
    e = {Token("module_parameter_2", Token::identifier),Token(Token::add),Token("1", Token::number)};
    p->set_raw_value(std::make_shared<Expression>(e));
    d0.add_array_quantifier(p);
    std::vector deps = {d0, d1, d2, d3};


    std::unordered_map<std::string, HDL_port> test_ports;

    test_ports["clock"] = {input_port};
    test_ports["reset"] = {input_port};
    test_ports["data_in"] = {interface_port, {"axi_stream", "slave"}};
    test_ports["data_out"] = {interface_port, {"axi_stream", "master"}};

    HDL_Resource check_res;
    check_res.set_name("Decoder");
    check_res.set_type(module);
    check_res.set_path("/test/file.sv");
    check_res.set_line_n(3);
    check_res.add_dependencies(deps);
    check_res.set_ports(test_ports);

    p = std::make_shared<HDL_parameter>();
    p->set_name("module_parameter_1");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->add_component(Token("56", Token::number));
    check_res.add_parameter(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("module_parameter_2");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->add_component(Token("74", Token::number));
    check_res.add_parameter(p);
    ASSERT_EQ(resource, check_res);
    resource = res[1];
    check_res = HDL_Resource();
    check_res.set_name("test_if");
    check_res.set_type(interface);
    check_res.set_path("/test/file.sv");
    check_res.set_line_n(30);
    ASSERT_EQ(resource, check_res);
}

TEST( analysis_test , vhdl_module) {

    vhdl_analyzer analyzer("check_files/test_vhdl_module.vhd");
    analyzer.cleanup_content("`(.*)");
    auto resource = analyzer.analyze()[0];
    HDL_instance dep("and_component", "ANDGATE", module);
    HDL_Resource check_res;
    check_res.set_name("half_adder");
    check_res.set_type(module);
    check_res.set_path("check_files/test_vhdl_module.vhd");
    check_res.set_line_n(4);
    check_res.add_dependency(dep);
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
    
    auto resource = analyzer.analyze("",test_pattern)[0];
    auto parameters = resource.get_parameters();

    auto dep = resource.get_dependencies()[0];

    HDL_instance check_dependency;
    check_dependency.set_type("axil_crossbar_interface");
    check_dependency.set_name("axi_xbar");
    check_dependency.set_dependency_class(module);
    check_dependency.add_port_connection("clock", {HDL_net("clock")});
    check_dependency.add_port_connection("slaves", {HDL_net("axi_in")});
    check_dependency.add_port_connection("masters", {HDL_net("timebase_axi"), HDL_net("gpio_axi"), HDL_net("fcore_axi")});

    ASSERT_EQ(dep, check_dependency);
}


TEST(analysis_test, interfaces_array) {
    auto test_pattern =R"(
    module test_mod ();

        axi_lite  cores_control[3]();

    endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    auto dep = resource.get_dependencies()[0];

    HDL_instance check_dependency;
    check_dependency.set_type("axi_lite");
    check_dependency.set_name("cores_control");
    check_dependency.set_dependency_class(module);
    auto array_qual = std::make_shared<HDL_parameter>();
    array_qual->set_name("instance_array_qualifier");
    array_qual->add_component(Token("3", Token::number));
    check_dependency.add_array_quantifier(array_qual);
    ASSERT_EQ(dep, check_dependency);
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
    auto resource = analyzer.analyze("", test_pattern)[0];
    auto parameters = resource.get_parameters();

    auto param = resource.get_dependencies()[0].get_parameters().get("TEST_PARAM");

    HDL_parameter reference_param;
    reference_param.set_name("TEST_PARAM");
    Token ec("TEST_ARRAY", Token::identifier);
    ec.add_array_index(std::make_shared<Expression>(Token("2", Token::number)));
    auto e = {ec};
    reference_param.set_raw_value(std::make_shared<Expression>(e));

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

    auto resource = analyzer.analyze("/tmp/file.sv",test_pattern);
    EXPECT_EQ(resource[0].getName(), "test_module_2");
    EXPECT_EQ(resource[0].get_path(), "/tmp/include_test.svh");
    EXPECT_EQ(resource[1].getName(), "test_module");
    EXPECT_EQ(resource[1].get_path(), "/tmp/file.sv");
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

    auto resource = analyzer.analyze("/tmp/file.sv",test_pattern);
    EXPECT_EQ(resource[0].getName(), "test_module_3");
    EXPECT_EQ(resource[0].get_path(), "/tmp/include_nested.svh");
    EXPECT_EQ(resource[1].getName(), "test_module_2");
    EXPECT_EQ(resource[1].get_path(), "/tmp/include_test.svh");
    EXPECT_EQ(resource[2].getName(), "test_module");
    EXPECT_EQ(resource[2].get_path(), "/tmp/file.sv");
}