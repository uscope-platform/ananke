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
#include "data_model/HDL/parameters/components/Expression_v2.hpp"

TEST(port_extraction, regular_port) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in(buck_merged)
            );
        endmodule
    )";

    sv_analyzer analyzer;
    auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();

    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("buck_merged")};

    ASSERT_TRUE(check_ports["clock"][0].get_name() == HDL_net("clock").get_name());
    ASSERT_TRUE(check_ports["clock"][0] == HDL_net("clock"));

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "buck_merged");
}



TEST(port_extraction, wildcard_ports) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .*
            );
        endmodule
    )";

    sv_analyzer analyzer;

    auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();

    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("buck_merged")};

    ASSERT_TRUE(ports.empty());
    ASSERT_TRUE(stmt->get_wildcard());
}

TEST(port_extraction, concat_port) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in({buck_merged, dab_merged})
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("buck_merged"), HDL_net("dab_merged")};

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "buck_merged");
    ASSERT_EQ(check_ports["stream_in"][1].get_full_name(), "dab_merged");
}



TEST(port_extraction, array_port) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in(stream[5])
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("stream")};

    Expression_v2 idx;
    idx.set_lhs(std::make_shared<Numeric_token>("5"));
    check_ports["stream_in"][0].set_index({idx});

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "stream[5]");
}

TEST(port_extraction, array_range_port) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in(S_AXI_AWADDR[3+:1])
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("S_AXI_AWADDR")};

    HDL_range range;
    {
        Expression_v2 acc;
        acc.set_lhs(std::make_shared<Numeric_token>("3"));
        range.accessor = acc;
    }
    {
        Expression_v2 rng;
        rng.set_lhs(std::make_shared<Numeric_token>("1"));
        range.range = rng;
    }
    range.type = HDL_range::increasing_range;
    check_ports["stream_in"][0].set_range(range);
    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "S_AXI_AWADDR[3+:1]");
}


TEST(port_extraction, concat_literal) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in({1'b0,test})
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("1'b0"), HDL_net("test")};

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "1'b0");
    ASSERT_EQ(check_ports["stream_in"][1].get_full_name(), "test");
}


TEST(port_extraction, concat_simple_slicing) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in({m_wdata[N],m_wstrb[N]})
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("m_wdata"), HDL_net("m_wstrb")};

    {
        Expression_v2 idx;
        idx.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N")));
        check_ports["stream_in"][0].set_index({idx});
    }
    {
        Expression_v2 idx;
        idx.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N")));
        check_ports["stream_in"][1].set_index({idx});
    }

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "m_wdata[N]");
    ASSERT_EQ(check_ports["stream_in"][1].get_full_name(), "m_wstrb[N]");
}

TEST(port_extraction, concat_range) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in({S_AXI_AWADDR[N+:3],S_AXI_AWPROT[C-:1]})
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("S_AXI_AWADDR"), HDL_net("S_AXI_AWPROT")};

    HDL_range range;
    {
        Expression_v2 acc;
        acc.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N")));
        range.accessor = acc;
    }
    {
        Expression_v2 rng;
        rng.set_lhs(std::make_shared<Numeric_token>("3"));
        range.range = rng;
    }
    range.type = HDL_range::increasing_range;

    check_ports["stream_in"][0].set_range(range);

    {
        Expression_v2 acc;
        acc.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("C")));
        range.accessor = acc;
    }
    {
        Expression_v2 rng;
        rng.set_lhs(std::make_shared<Numeric_token>("1"));
        range.range = rng;
    }
    range.type = HDL_range::decreasing_range;

    check_ports["stream_in"][1].set_range(range);


    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "S_AXI_AWADDR[N+:3]");
    ASSERT_EQ(check_ports["stream_in"][1].get_full_name(), "S_AXI_AWPROT[C-:1]");
}


TEST(port_extraction,range_concat_expression) {
    auto test_pattern = R"(
        module test_mod #()();

        address_decoder wraddr(
            .in_data({S_AXI_AWADDR[N*ADDR_WIDTH +: ADDR_WIDTH], S_AXI_AWPROT[N*3 +: 3]}),
            .clock(clock)
        );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["in_data"] = {HDL_net("S_AXI_AWADDR"), HDL_net("S_AXI_AWPROT")};

    HDL_range range;
    {
        Expression_v2 acc;
        acc.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N")));
        acc.set_operation(Expression_v2::multiply);
        acc.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("ADDR_WIDTH")));
        range.accessor = acc;
    }
    {
        Expression_v2 rng;
        rng.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("ADDR_WIDTH")));
        range.range = rng;
    }
    range.type = HDL_range::increasing_range;

    check_ports["in_data"][0].set_range(range);

    {
        Expression_v2 acc;
        acc.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N")));
        acc.set_operation(Expression_v2::multiply);
        acc.set_rhs(std::make_shared<Numeric_token>("3"));
        range.accessor = acc;
    }
    {
        Expression_v2 rng;
        rng.set_lhs(std::make_shared<Numeric_token>("3"));
        range.range = rng;
    }
    range.type = HDL_range::increasing_range;

    check_ports["in_data"][1].set_range(range);

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["in_data"][0].get_full_name(), "S_AXI_AWADDR[N*ADDR_WIDTH+:ADDR_WIDTH]");
    ASSERT_EQ(check_ports["in_data"][1].get_full_name(), "S_AXI_AWPROT[N*3+:3]");
}



TEST(port_extraction, concat_complex_slicing) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in({S_AXI_AWADDR[N*ADDR_WIDTH+:ADDR_WIDTH],S_AXI_AWPROT[N*3+:3]})
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("S_AXI_AWADDR"), HDL_net("S_AXI_AWPROT")};

    HDL_range range;
    {
        Expression_v2 acc;
        acc.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N")));
        acc.set_operation(Expression_v2::multiply);
        acc.set_rhs(std::make_shared<Identifier_token>(qualified_identifier("ADDR_WIDTH")));
        range.accessor = acc;
    }
    {
        Expression_v2 rng;
        rng.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("ADDR_WIDTH")));
        range.range = rng;
    }
    range.type = HDL_range::increasing_range;

    check_ports["stream_in"][0].set_range(range);

    {
        Expression_v2 acc;
        acc.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("N")));
        acc.set_operation(Expression_v2::multiply);
        acc.set_rhs(std::make_shared<Numeric_token>("3"));
        range.accessor = acc;
    }
    {
        Expression_v2 rng;
        rng.set_lhs(std::make_shared<Numeric_token>("3"));
        range.range = rng;
    }
    range.type = HDL_range::increasing_range;

    check_ports["stream_in"][1].set_range(range);

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "S_AXI_AWADDR[N*ADDR_WIDTH+:ADDR_WIDTH]");
    ASSERT_EQ(check_ports["stream_in"][1].get_full_name(), "S_AXI_AWPROT[N*3+:3]");
}


TEST(port_extraction, concat_interface_component) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in({axil.WDATA,axil.WSTRB})
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("axil.WDATA"), HDL_net("axil.WSTRB")};
    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "axil.WDATA");
    ASSERT_EQ(check_ports["stream_in"][1].get_full_name(), "axil.WSTRB");
}


TEST(port_extraction, repetition_port) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in({5{1'b1}})
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("")};

    HDL_replication rep;
    {
        Expression_v2 sz;
        sz.set_lhs(std::make_shared<Numeric_token>("5"));
        rep.size = sz;
    }
    {
        Expression_v2 tgt;
        tgt.set_lhs(std::make_shared<Numeric_token>("1'b1"));
        rep.target = tgt;
    }
    check_ports["stream_in"][0].set_replication(rep);

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "");
}


TEST(port_extraction, complex_nested_concat_port) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in({OUTPUT_SIGNED[data_in.dest],{DATA_PATH_WIDTH-1{1'b0}}, test})
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net("OUTPUT_SIGNED"), HDL_net(""), HDL_net("test")};

    {
        Expression_v2 idx;
        idx.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("data_in.dest")));
        check_ports["stream_in"][0].set_index({idx});
    }

    HDL_replication rep;
    {
        Expression_v2 sz;
        sz.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("DATA_PATH_WIDTH")));
        sz.set_operation(Expression_v2::subtract);
        sz.set_rhs(std::make_shared<Numeric_token>("1"));
        rep.size = sz;
    }
    {
        Expression_v2 tgt;
        tgt.set_lhs(std::make_shared<Numeric_token>("1'b0"));
        rep.target = tgt;
    }
    check_ports["stream_in"][1].set_replication(rep);

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "OUTPUT_SIGNED[data_in.dest]");
    ASSERT_EQ(check_ports["stream_in"][1].get_full_name(), "");
}



TEST(port_extraction, concat_of_repetitions) {
    auto test_pattern = R"(
        module test_mod #()();

            axi_stream_combiner #(
            ) scope_combinator (
                .clock(clock),
                .stream_in({{3{1'b1}},{DATA_PATH_WIDTH-1{1'b0}}})
            );
        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["stream_in"] = {HDL_net(""), HDL_net("")};


    HDL_replication rep;
    {
        Expression_v2 sz;
        sz.set_lhs(std::make_shared<Numeric_token>("3"));
        rep.size = sz;
    }
    {
        Expression_v2 tgt;
        tgt.set_lhs(std::make_shared<Numeric_token>("1'b1"));
        rep.target = tgt;
    }

    check_ports["stream_in"][0].set_replication(rep);

    {
        Expression_v2 sz;
        sz.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("DATA_PATH_WIDTH")));
        sz.set_operation(Expression_v2::subtract);
        sz.set_rhs(std::make_shared<Numeric_token>("1"));
        rep.size = sz;
    }
    {
        Expression_v2 tgt;
        tgt.set_lhs(std::make_shared<Numeric_token>("1'b0"));
        rep.target = tgt;
    }

    check_ports["stream_in"][1].set_replication(rep);

    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["stream_in"][0].get_full_name(), "");
    ASSERT_EQ(check_ports["stream_in"][1].get_full_name(), "");
}

TEST(port_extraction, port_extraction_with_declarations) {
    auto test_pattern = R"(

        module test_mod #()();

        reg [31:0] memory [5:0];

        SyndromeCalculator #(
            .TEST_PARAM(test_package::param)
        ) SC (
            .clock(clock),
            .reset(reset),
            .data_in(data_in),
            .syndrome(data_out)
        );

        endmodule
    )";

    sv_analyzer analyzer;

    auto res = analyzer.analyze("",test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[1]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["reset"] = {HDL_net("reset")};
    check_ports["data_in"] = {HDL_net("data_in")};
    check_ports["syndrome"] = {HDL_net("data_out")};
    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["reset"][0].get_full_name(), "reset");
    ASSERT_EQ(check_ports["data_in"][0].get_full_name(), "data_in");
    ASSERT_EQ(check_ports["syndrome"][0].get_full_name(), "data_out");
}

TEST(port_extraction, other_port_concat) {
    auto test_pattern = R"(
        module test_mod #()();

        axil_crossbar_interface  axi_xbar (
            .clock(clock),
            .slaves('{axi_in}),
            .masters('{timebase_axi, gpio_axi, fcore_axi})
        );
        endmodule
    )";

    sv_analyzer analyzer;

    auto res = analyzer.analyze("",test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["slaves"] = {HDL_net("axi_in")};
    check_ports["masters"] = {HDL_net("timebase_axi"), HDL_net("gpio_axi"), HDL_net("fcore_axi")};
    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["slaves"][0].get_full_name(), "axi_in");
    ASSERT_EQ(check_ports["masters"][0].get_full_name(), "timebase_axi");
    ASSERT_EQ(check_ports["masters"][1].get_full_name(), "gpio_axi");
    ASSERT_EQ(check_ports["masters"][2].get_full_name(), "fcore_axi");
}


TEST(port_extraction, replication_with_parameter) {
    auto test_pattern = R"(
        module test_mod #()();

            axil_crossbar_interface #(
                .DATA_WIDTH(32),
                .ADDR_WIDTH(32),
                .NM(1),
                .NS(2),
                .SLAVE_ADDR('{CONTROLLER_ADDRESS, SPI_ADDRESS}),
                .SLAVE_MASK('{2{32'h040}})
            ) axi_xbar (
                .clock(clock),
                .reset(reset),
                .slaves('{axi_in}),
                .masters('{controller_axi, spi_axi})
            );

        endmodule
    )";

    sv_analyzer analyzer;

        auto res = analyzer.analyze("", test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["clock"] = {HDL_net("clock")};
    check_ports["reset"] = {HDL_net("reset")};
    check_ports["slaves"] = {HDL_net("axi_in")};
    check_ports["masters"] = {HDL_net("controller_axi"), HDL_net("spi_axi")};
    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["clock"][0].get_full_name(), "clock");
    ASSERT_EQ(check_ports["reset"][0].get_full_name(), "reset");
    ASSERT_EQ(check_ports["slaves"][0].get_full_name(), "axi_in");
    ASSERT_EQ(check_ports["masters"][0].get_full_name(), "controller_axi");
    ASSERT_EQ(check_ports["masters"][1].get_full_name(), "spi_axi");
}


TEST(port_extraction, interface_component_port ) {
    auto test_pattern = R"(
        module test_mod #()();

        axil_skid_buffer address_read_buffer (
            .in_valid(axil.ARVALID)
        );
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto res = analyzer.analyze("",test_pattern).get_content()[0]->as<hdl_resource_statement>();
    auto stmt = std::dynamic_pointer_cast<hdl_instance_statement>(res.get_statements()[0]);
    auto ports = stmt->get_ports();
    std::unordered_map<std::string, std::vector<HDL_net>> check_ports;
    check_ports["in_valid"] = {HDL_net("axil.ARVALID")};
    ASSERT_EQ(ports, check_ports);
    ASSERT_EQ(check_ports["in_valid"][0].get_full_name(), "axil.ARVALID");
}
