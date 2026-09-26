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
#include "analysis/HDL_ast_builder_v2.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "analysis/parameter_solver.hpp"
#include "data_model/HDL/parameters/components/Replication.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"
#include "data_model/HDL/parameters/components/Cast.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"
#include "data_model/settings_store.hpp"
#include "data_model/data_store.hpp"
#include "data_model/Depfile/Depfile.hpp"


TEST(parameter_processing, override_after_fatal) {
    auto test_pattern = R"(

        module dependency #(
            PARAM_1 = 5
        )();

        endmodule

        module test_module #(
           ENABLE_ASSERTIONS = "false"
        )(
            input wire core_clock,
            input wire start
        );



        start_request_pulse: assert property(@(posedge core_clock) start |=> !start) else begin
            $fatal();
        end

        dependency #(
            .PARAM_1(256)
        )dep();


        endmodule
    )";


    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_module"}))[0];

    auto dep = ast_v2->get_dependencies()[0];
    ASSERT_TRUE(dep->has_parameter("PARAM_1"));
    EXPECT_EQ(dep->find_parameter("PARAM_1")->get_numeric_value(),  256);
}



TEST(parameter_processing, mixed_dep_override) {
    auto test_pattern = R"(
        module dependency #(
            parameter [7:0] ACCESS_ALLOWED = -1,
            parameter [31:0] SLAVE_MASK [7:0] =  '{8{'hffff}}
        )();

            localparam [0:0] OPT_NONESEL = (!ACCESS_ALLOWED[0]) || (SLAVE_MASK[0] != 0);
        endmodule

        module test_mod #(
            parameter [31:0] SLAVE_MASK [7:0] =  '{8{'hffff}}
        ) ();

            dependency #(
                .SLAVE_MASK(SLAVE_MASK)
            ) test_instance ();

        endmodule
    )";


    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto dep = ast_v2->get_dependencies()[0];
    ASSERT_TRUE(dep->has_parameter("OPT_NONESEL"));
    EXPECT_EQ(dep->find_parameter("OPT_NONESEL")->get_numeric_value(),  1);
}


TEST(parameter_processing, package_parameters_in_array_init) {
    auto test_pattern = R"(

    package test_package;

        parameter bus_base = 1;
        parameter timebase = 2;
        parameter gpio = 3;

    endpackage


    module dependency #(
        parameter integer TEST_PARAM [2:0] = '{0, 0, 0} )(
    );
    endmodule

    module test_mod #()(
    );

        localparam [31:0] AXI_ADDRESSES [2:0] = '{
            test_package::bus_base,
            test_package::timebase,
            test_package::gpio
        };

        dependency #(
            .TEST_PARAM(AXI_ADDRESSES)
        ) test_instance ();

    endmodule
    )";


    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto& pkg = resources[0]->as<hdl_resource_statement>();
    auto& mod = resources[2]->as<hdl_resource_statement>();;

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameter_statements(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_package",  id.get_name())] = val;
    }

    auto solved = parameter_solver::process_parameters(mod.get_parameter_statements(), ctx);

    auto param_val = solved.at(qualified_identifier("AXI_ADDRESSES")).get_int_array();

    mdarray<hdl_integer> av;
    av.set_1d_slice({0,0}, {3,2,1});
    ASSERT_EQ(param_val, av);
}


TEST(parameter_processing, struct_member_unresolvable_range_no_crash) {
    // A struct whose member range references an undefined identifier must make
    // evaluate_type return nullopt (handled as "missing value") instead of
    // dereferencing a disengaged optional and crashing.
    auto test_pattern = R"(

        package test_pkg;
            typedef struct packed {
                logic [EXT_PARAM-1:0] field;
                logic [63:0] DmBaseAddress;
            } cva6_cfg_t;

            localparam cva6_cfg_t Cfg = '0;
        endpackage
    )";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto& pkg = resources[0]->as<hdl_resource_statement>();

    auto solved = parameter_solver::process_parameters(pkg.get_parameter_statements(), {});

    ASSERT_TRUE(solved.contains(qualified_identifier("Cfg")));
}


TEST(parameter_processing, replication_after_cast_in_assignment_pattern) {
    // A replication member value following a cast member value in an assignment
    // pattern used to produce a Replication with a null size, crashing the
    // topological sorter (Replication::get_dependencies). The tree must be
    // well-formed and the parameter must still be solvable.
    auto test_pattern = R"(
        package pkg;
            typedef struct packed {
                logic [7:0] a;
                logic [1:0] b;
            } cfg_t;
            localparam cfg_t c = '{a: unsigned'(8'd7), b: {2{1'b1}}};
        endpackage
    )";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto& pkg = resources[0]->as<hdl_resource_statement>();

    std::vector<std::shared_ptr<Expression_base>> stack;
    std::shared_ptr<HDL_parameter> c_param;
    for (const auto &d : pkg.get_parameter_statements())
        if (d->get_name() == "c") c_param = d;
    ASSERT_NE(c_param, nullptr);
    stack.push_back(c_param->get_expression());
    while (!stack.empty()) {
        auto node = stack.back();
        stack.pop_back();
        if (!node) continue;
        if (auto rep = std::dynamic_pointer_cast<Replication>(node)) {
            ASSERT_NE(rep->get_size(), nullptr) << "Replication has null size";
            ASSERT_NE(rep->get_item(), nullptr) << "Replication has null item";
            stack.push_back(rep->get_size());
            stack.push_back(rep->get_item());
        } else if (auto e = std::dynamic_pointer_cast<Expression_v2>(node)) {
            stack.push_back(e->get_lhs());
            stack.push_back(e->get_rhs());
        } else if (auto conc = std::dynamic_pointer_cast<Concatenation>(node)) {
            for (auto &comp : conc->get_components()) stack.push_back(comp);
        } else if (auto cast = std::dynamic_pointer_cast<Cast>(node)) {
            stack.push_back(cast->get_content());
            stack.push_back(cast->get_size_expr());
        }
    }

    auto solved = parameter_solver::process_parameters(pkg.get_parameter_statements(), {});
    ASSERT_TRUE(solved.contains(qualified_identifier("c")));
}


TEST(parameter_processing, package_parameters_use) {
    auto test_pattern = R"(

        package test_package;
            parameter bus_base = 67;
        endpackage

        module test_mod #(
             parameter package_param = test_package::bus_base
        )();

        endmodule
    )";


    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto& pkg = resources[0]->as<hdl_resource_statement>();;
    auto& mod = resources[1]->as<hdl_resource_statement>();;

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameter_statements(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_package", id.get_name())] = val;
    }

    auto solved = parameter_solver::process_parameters(mod.get_parameter_statements(), ctx);

    ASSERT_TRUE(solved.contains(qualified_identifier("package_param")));
    ASSERT_EQ(67, solved.at(qualified_identifier("package_param")).get_integer());
}


TEST(parameter_processing, array_instance_parameter_override) {
    auto test_pattern = R"(
        module dependency #(
            parameter param_1 = 4,
            parameter param_2 [1:0]= '{0,0},
            param_3 = 6
        )();

            parameter p1_t = param_2[0];
            parameter p2_t = param_2[1];

        endmodule

        module test_mod #(
            parameter test_param = 4
        )();

            localparam override_array [1:0] = '{8,9};

            dependency #(
                .param_1(test_param),
                .param_2(override_array),
                .param_3((test_param + 7)*1)
            ) test_instance ();

        endmodule
    )";


    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto dep = ast_v2->get_dependencies()[0];

    std::vector<std::shared_ptr<HDL_parameter>> check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("param_1");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>(4, 0));
    p->set_value(4);

    check_params.push_back(p);

    p = std::make_shared<HDL_parameter>();
    mdarray<hdl_integer> av;
    av.set_1d_slice({0,0}, {9,8});
    auto tk = Numeric_token(0, 0);
    tk.set_value(av);
    p->set_raw_value(std::make_shared<Numeric_token>(tk));
    p->set_name("param_2");
    HDL_simple_type t;
    t = Type_engine::create_primitive_type("implicit")->as<HDL_simple_type>();
    t.add_dimension({
        std::make_shared<Numeric_token>("1"),
        std::make_shared<Numeric_token>("0"),
        false
    });
    p->set_type(std::make_shared<HDL_simple_type>(t));
    p->set_value(av);
    check_params.push_back(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("param_3");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    Expression_v2 e1, e2;
    e2.set_lhs(std::make_shared<Numeric_token>(4, 0));
    e2.set_rhs(std::make_shared<Numeric_token>(7, 3));
    e2.set_operation(Expression_v2::add);
    e1.set_lhs(std::make_shared<Expression_v2>(e2));
    e1.set_rhs(std::make_shared<Numeric_token>(1,1));
    e1.set_operation(Expression_v2::multiply);
    p->set_raw_value(std::make_shared<Expression_v2>(e1));
    p->set_value(11);
    check_params.push_back(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("p1_t");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    auto tk2 = Identifier_token(qualified_identifier("param_2"));
    tk2.add_array_index(std::make_shared<Numeric_token>(0, 1));
    p->set_raw_value(std::make_shared<Numeric_token>(tk));
    p->set_value(9);
    check_params.push_back(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("p2_t");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    tk2 = Identifier_token(qualified_identifier("param_2"));
    tk2.add_array_index(std::make_shared<Numeric_token>(1, 1));
    p->set_raw_value(std::make_shared<Identifier_token>(tk2));
    p->set_value(8);
    check_params.push_back(p);

    ASSERT_EQ(check_params.size(), dep->get_parameters().size());

    for (size_t i = 0; i < check_params.size(); ++i) {
        ASSERT_EQ(*check_params[i], *dep->get_parameters()[i]);
    }

}


TEST(parameter_processing, packed_array_initialization_expression_override) {
    auto test_pattern = R"(

        module dependency #(
            N_TRIGGER_REGISTERS = 1,
            parameter [31:0] TRIGGER_REGISTERS_IDX [N_TRIGGER_REGISTERS-1:0] = '{N_TRIGGER_REGISTERS{0}}
        )();
            parameter p1_t = TRIGGER_REGISTERS_IDX[0];
        endmodule

        module test_mod #(
        )();

            parameter TAP_ADDR_REG = 5;
            dependency #(
                .TRIGGER_REGISTERS_IDX('{TAP_ADDR_REG})
            ) d ();
        endmodule
    )";


    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto dep = ast_v2->get_dependencies()[0];

    auto array_param = dep->find_parameter("TRIGGER_REGISTERS_IDX");
    ASSERT_NE(array_param, nullptr);
    auto array_val = array_param->get_int_array_value();
    ASSERT_TRUE(array_val.has_value());
    ASSERT_EQ(array_val.value().get_value({0,0,0}), 5);

}


TEST(parameter_processing, simple_for_array_parameter) {
    auto test_pattern = R"(

        module dependency #(
            N_TRIGGER_REGISTERS = 1
        )();
        endmodule

        module test_mod #(
        )();

            parameter  [31:0] TAP_ADDR_REG [2:0] = '{6,2,4};
            genvar n;
            generate
                for(n = 0; n<3; n=n+1)begin
                    dependency #(
                        .N_TRIGGER_REGISTERS(TAP_ADDR_REG[n])
                    ) d ();
                end
            endgenerate
        endmodule
    )";


    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    Depfile df;

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto deps = ast_v2->get_dependencies();

    ASSERT_EQ(deps.size(), 3);
    auto trig0 = deps[0]->find_parameter("N_TRIGGER_REGISTERS");
    auto trig1 = deps[1]->find_parameter("N_TRIGGER_REGISTERS");
    auto trig2 = deps[2]->find_parameter("N_TRIGGER_REGISTERS");
    ASSERT_NE(trig0, nullptr);
    ASSERT_NE(trig1, nullptr);
    ASSERT_NE(trig2, nullptr);
    ASSERT_EQ(trig0->get_numeric_value(), 4);
    ASSERT_EQ(trig1->get_numeric_value(), 2);
    ASSERT_EQ(trig2->get_numeric_value(), 6);

}


TEST(parameter_processing, complex_for_array_parameter) {
    auto test_pattern = R"(

        module dependency #(
            N_TRIGGER_REGISTERS = 1
        )();
        endmodule

        module test_mod #(
        )();

            parameter  [31:0] TAP_ADDR_REG [2:0] = '{6,2,4};

            parameter  TEST_PARAM = 1;
            genvar n;
            generate
                for(n = 0; n<3; n=n+1)begin
                    dependency #(
                        .N_TRIGGER_REGISTERS(TAP_ADDR_REG[(TEST_PARAM+1)-n])
                    ) d ();
                end
            endgenerate
        endmodule
    )";


    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];
    auto deps = ast_v2->get_dependencies();

    ASSERT_EQ(deps.size(), 3);
    auto trig0 = deps[0]->find_parameter("N_TRIGGER_REGISTERS");
    auto trig1 = deps[1]->find_parameter("N_TRIGGER_REGISTERS");
    auto trig2 = deps[2]->find_parameter("N_TRIGGER_REGISTERS");
    ASSERT_NE(trig0, nullptr);
    ASSERT_NE(trig1, nullptr);
    ASSERT_NE(trig2, nullptr);
    ASSERT_EQ(trig0->get_numeric_value(), 6);
    ASSERT_EQ(trig1->get_numeric_value(), 2);
    ASSERT_EQ(trig2->get_numeric_value(), 4);

}



TEST(parameter_processing, complex_vector_function_parameter) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 3
        )();

            parameter ADDR_WIDTH = 32;
            parameter N_AXI_LITE = N_CORES + 2;


            typedef logic [ADDR_WIDTH-1:0] ctrl_addr_init_t [N_AXI_LITE-1:0];
            function ctrl_addr_init_t CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 44;
                for(int i = 1; i<N_CORES+1; i++)begin
                    CTRL_ADDR_CALC[i] = 100*i;
                end
                CTRL_ADDR_CALC[4] = 667;
            endfunction

            parameter [ADDR_WIDTH-1:0] AXI_ADDRESSES [N_AXI_LITE-1:0] = CTRL_ADDR_CALC();

        endmodule
    )";


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto param = ast_v2->find_parameter("AXI_ADDRESSES");
    ASSERT_NE(param, nullptr);
    ASSERT_TRUE(param->get_int_array_value().has_value());
    auto param_value = param->get_int_array_value().value().get_1d_slice({0, 0});
    mdarray<hdl_integer>::md_1d_array reference = {44, 100, 200, 300, 667};
    ASSERT_EQ(param_value, reference);

}

TEST(parameter_processing, complex_vector_function_parameter_endiannes_mismatch) {
    auto test_pattern = R"(
        module test_mod #(
            N_CORES = 3
        )();

            parameter ADDR_WIDTH = 32;
            parameter N_AXI_LITE = N_CORES + 2;


            typedef logic [ADDR_WIDTH-1:0] ctrl_addr_init_t [0:N_AXI_LITE-1];
            function ctrl_addr_init_t CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 44;
                for(int i = 1; i<N_CORES+1; i++)begin
                    CTRL_ADDR_CALC[i] = 100*i;
                end
                CTRL_ADDR_CALC[4] = 667;
            endfunction

            parameter [ADDR_WIDTH-1:0] AXI_ADDRESSES [N_AXI_LITE-1:0] = CTRL_ADDR_CALC();

        endmodule
    )";


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto param = ast_v2->find_parameter("AXI_ADDRESSES");
    ASSERT_NE(param, nullptr);
    auto param_value = param->get_int_array_value().value().get_1d_slice({0, 0});
    mdarray<hdl_integer>::md_1d_array reference = {667, 300, 200, 100, 44};
    ASSERT_EQ(param_value, reference);

}

TEST(parameter_processing, simple_package_in_function_initialization) {
    auto test_pattern = R"(



        package hil_address_space;

            parameter bus_base = 32'h43c00000;

        endpackage


        module test_mod #(
            parameter ADDR_WIDTH = 32,
            parameter N_CORES =2
        )();

            localparam N_AXI_LITE = N_CORES;
            localparam BASE_ADDR = 32'h43c00000 + 'h30000;

          typedef logic [ADDR_WIDTH-1:0] ctrl_addr_init_t [N_AXI_LITE-1:0];
            function automatic ctrl_addr_init_t CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = hil_address_space::bus_base;
                CTRL_ADDR_CALC[1] = BASE_ADDR + 4;
            endfunction

            localparam ctrl_addr_init_t AXI_ADDRESSES = CTRL_ADDR_CALC();

        endmodule


    )";


    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameter_statements(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("hil_address_space",  id.get_name())] = val;
    }

    parameter_solver::propagate_functions(mod, nullptr);
    auto solved = parameter_solver::process_parameters(mod->get_parameter_statements(), ctx);

    auto param = solved.at(qualified_identifier("AXI_ADDRESSES")).get_int_array();
    auto param_value = param.get_1d_slice({0, 0});
    mdarray<hdl_integer>::md_1d_array reference = {0x43c00000,0x43c30004};
    ASSERT_EQ(param_value, reference);
}


TEST(parameter_processing, nested_package_in_function_initialization) {
    auto test_pattern = R"(

        package hil_address_space;
            parameter bus_base = 32'h43c00000;
        endpackage


        module test_mod #(
            parameter ADDR_WIDTH = 32,
            parameter N_CORES = 3
        )();

            localparam N_AXI_LITE = N_CORES+3;
            localparam BASE_ADDR = hil_address_space::bus_base + 'h30000;

            typedef logic [ADDR_WIDTH-1:0] ctrl_addr_init_t [N_AXI_LITE];
            function ctrl_addr_init_t CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = hil_address_space::bus_base;
                CTRL_ADDR_CALC[1] = BASE_ADDR + 4;
            endfunction

            localparam [ADDR_WIDTH-1:0] AXI_ADDRESSES [N_AXI_LITE-1:0] = CTRL_ADDR_CALC();

        endmodule


    )";

    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameter_statements(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("hil_address_space", id.get_name())] = val;
    }

    parameter_solver::propagate_functions(mod, nullptr);
    auto solved = parameter_solver::process_parameters(mod->get_parameter_statements(), ctx);

    auto param = solved.at(qualified_identifier("AXI_ADDRESSES")).get_int_array();
    auto param_value = param.get_1d_slice({0, 0});
    mdarray<hdl_integer>::md_1d_array reference = {0x43c00000,0x43c30004};
    ASSERT_EQ(param_value, reference);
}


TEST(parameter_processing, function_in_package_initialization) {
    auto test_pattern = R"(
        package test_pkg;

            function integer CALC();
                CALC = 42;
            endfunction

            localparam integer RESULT = CALC();

        endpackage
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto pkg = std::static_pointer_cast<hdl_resource_statement>(resources[0]);


    parameter_solver::propagate_functions(pkg, nullptr);
    auto solved = parameter_solver::process_parameters(pkg->get_parameter_statements(), {});
    auto param = solved.at(qualified_identifier("RESULT")).get_integer();
    ASSERT_EQ(param, 42);
}


TEST(parameter_processing, package_function_called_from_module) {
    auto test_pattern = R"(
        package test_pkg;

            function integer CALC();
                CALC = 42;
            endfunction

        endpackage

        module test_mod #(
        )();

            localparam integer RESULT = test_pkg::CALC();

        endmodule
    )";

    sv_analyzer analyzer;


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");

    auto file =  analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    std::shared_ptr<HDL_parameter> param;
    for (const auto &d : mod->get_parameter_statements())
        if (d->get_name() == "RESULT") param = d;
    ASSERT_NE(param, nullptr);

    HDL_parameter p;
    p.set_name("RESULT");
    p.set_type(Type_engine::create_primitive_type("integer"));
    HDL_function_call c;
    c.set_name("CALC");
    c.add_package_prefix("test_pkg");
    p.set_raw_value(std::make_shared<HDL_function_call>(c));
    ASSERT_EQ(p, *param);
    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameter_statements(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_pkg", id.get_name())] = val;
    }

    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameter_statements(), ctx);
    auto solve_param = solved.at(qualified_identifier("RESULT")).get_integer();
    ASSERT_EQ(solve_param, 42);
}



TEST(parameter_processing, package_function_called_from_module_and_typedef) {
    auto test_pattern = R"(
        package test_pkg;
            typedef logic [31:0] test_type [15:0];
        endpackage

        package test_pkg_2;
            function test_pkg::test_type build_config();
                build_config = 42;
            endfunction

        endpackage

        module test_mod #(
            parameter test_pkg::test_type RESULT = test_pkg_2::build_config()
        )();

        endmodule
    )";

    sv_analyzer analyzer;


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");


    auto file = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto resources = file.get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto pkg2 = resources[1]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[2]);

    std::shared_ptr<HDL_parameter> param;
    for (const auto &d : mod->get_parameter_statements())
        if (d->get_name() == "RESULT") param = d;
    ASSERT_NE(param, nullptr);

    HDL_parameter p;
    p.set_name("RESULT");
    p.set_type(std::make_shared<HDL_external_type>(qualified_identifier("test_pkg", "test_type")));
    HDL_function_call c;
    c.set_name("build_config");
    c.add_package_prefix("test_pkg_2");
    p.set_raw_value(std::make_shared<HDL_function_call>(c));
    ASSERT_EQ(p, *param);
    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameter_statements(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_pkg", id.get_name())] = val;
    }

    parameter_solver::propagate_types(mod, d_store);
    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameter_statements(), ctx);
    auto solve_param = solved.at(qualified_identifier("RESULT")).get_integer();
    ASSERT_EQ(solve_param, 42);
}



TEST(parameter_processing, override_with_system_task) {
    auto test_pattern = R"(

        module dependency #(
            parameter param_1 = 4
        )();

            parameter p1_t = param_1+2;

        endmodule

        module test_mod #(
        )();

            dependency #(
                .param_1($clog2(64))
            ) dep ();

        endmodule
    )";



    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto dep = ast_v2->get_dependencies()[0];
    auto param_1 = dep->find_parameter("param_1");
    ASSERT_NE(param_1, nullptr);
    EXPECT_EQ(param_1->get_numeric_value(), 6);
    auto p1_t = dep->find_parameter("p1_t");
    ASSERT_NE(p1_t, nullptr);
    EXPECT_EQ(p1_t->get_numeric_value(), 8);
}

TEST(parameter_processing, interface_default_parameters) {
    auto test_pattern = R"(
        interface test_if #(DATA_WIDTH = 32);
        endinterface

        module test_mod #(
        )();

            test_if iface();

        endmodule
    )";



    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;


    auto file = analyzer.analyze("", test_pattern).value();
    auto resource = file.get_content()[1]->as<hdl_resource_statement>();
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(resource.get_statements()[0]);
    auto raw_param = inst->get_parameters();


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto dep = ast_v2->get_dependencies()[0];
    auto param_1 = dep->find_parameter("DATA_WIDTH");
    ASSERT_NE(param_1, nullptr);
    EXPECT_EQ(param_1->get_numeric_value(), 32);
}

TEST(parameter_processing, override_with_interface_param) {
    auto test_pattern = R"(


    interface test_if #(DATA_WIDTH = 32);
    endinterface

        module dependency #(
            parameter param_1 = 4
        )();

            parameter p1_t = param_1+2;

        endmodule

        module test_mod #(
        )();

            test_if #(.DATA_WIDTH(32)) iface();

            dependency #(
                .param_1(iface.DATA_WIDTH)
            ) dep ();

        endmodule
    )";



    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto file = analyzer.analyze("", test_pattern).value();
    auto resource = file.get_content()[2]->as<hdl_resource_statement>();
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(resource.get_statements()[1]);
    auto raw_param = inst->get_parameters();


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto dep = ast_v2->get_dependencies()[1];
    auto param_1 = dep->find_parameter("param_1");
    ASSERT_NE(param_1, nullptr);
    EXPECT_EQ(param_1->get_numeric_value(), 32);
}


TEST(parameter_processing, nested_override_with_interface_param) {
    auto test_pattern = R"(


    interface test_if #(DATA_WIDTH = 32);
    endinterface
        module outer_dependency #()(
            test_if iface,
            axi_stream.slave test
        );
            inner_dependency #(
                .param_1(iface.DATA_WIDTH)
            ) dep ();

        endmodule
        module inner_dependency #(
            parameter param_1 = 4
        )();

            parameter p1_t = param_1+2;

        endmodule

        module test_mod #(
        )();

            test_if #(.DATA_WIDTH(32)) intf();

            outer_dependency dep (
                .iface(intf)
            );

        endmodule
    )";



    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto dep = ast_v2->get_dependencies()[1]->get_dependencies()[0];
    auto param_1 = dep->find_parameter("param_1");
    ASSERT_NE(param_1, nullptr);
    EXPECT_EQ(param_1->get_numeric_value()->get_value(), 32);
}

TEST(parameter_processing, double_nested_override_with_interface_param) {
    auto test_pattern = R"(


    interface test_if #(DATA_WIDTH = 32);
    endinterface

        module inner_most #(
            parameter param_1 = 4
        )();

            parameter p1_t = param_1+2;

        endmodule

        module mid_inner (
            test_if iface
        );
            inner_most #(
                .param_1(iface.DATA_WIDTH)
            ) dep ();

        endmodule

        module mid_outer (
            test_if iface
        );
            mid_inner inst (
                .iface(iface)
            );

        endmodule

        module top #(
        )();

            test_if #(.DATA_WIDTH(55)) intf();

            mid_outer dep (
                .iface(intf)
            );

        endmodule
    )";



    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];


    auto dep = ast_v2->get_dependencies()[1]->get_dependencies()[0]->get_dependencies()[0];
    auto param_1 = dep->find_parameter("param_1");
    ASSERT_NE(param_1, nullptr);
    EXPECT_EQ(param_1->get_numeric_value()->get_value(), 55);
}

TEST(parameter_processing, override_with_package_parameter) {
    auto test_pattern = R"(
        package test_package;
            parameter base = 33;
        endpackage

        module dependency #(
            parameter param_1 = 4
        )();

            parameter p1_t = param_1+2;

        endmodule

        module test_mod #(
        )();

            dependency #(
                .param_1(test_package::base)
            ) dep ();

        endmodule
    )";



    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto pkg = std::static_pointer_cast<hdl_resource_statement>(resources[0]);
    auto dep = std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    auto pkg_defaults = parameter_solver::process_parameters(pkg->get_parameter_statements(),  {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_package", id.get_name())] = val;
    }

    auto dep_params = dep->get_parameter_statements();
    std::vector<std::shared_ptr<HDL_parameter>> to_solve;
    for (auto &param : dep_params) {
        if (param->get_name() == "param_1") {
            auto override_param = std::make_shared<HDL_parameter>();
            override_param->set_name("param_1");
            override_param->set_type(Type_engine::create_primitive_type("implicit"));
            override_param->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("test_package", "base")));
            override_param->set_type(param->get_type());
            to_solve.push_back(override_param);
        } else {
            to_solve.push_back(param);
        }
    }

    auto solved = parameter_solver::process_parameters(to_solve, ctx);

    EXPECT_EQ(33, solved.at(qualified_identifier("param_1")).get_integer());
    EXPECT_EQ(35, solved.at(qualified_identifier("p1_t")).get_integer());
}


TEST(parameter_processing, override_with_function_parameter) {
    auto test_pattern = R"(

        module dependency #(
            parameter param_1 = 4
        )();

            parameter p1_t = param_1+2;

        endmodule

        module test_mod #(
        )();

            parameter ADDR_WIDTH = 32;
            parameter N_AXI_LITE = 3;


            typedef logic [ADDR_WIDTH-1:0] ctrl_addr_init_t [N_AXI_LITE-1:0];
            function ctrl_addr_init_t CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 100;
                CTRL_ADDR_CALC[1] = 130;
                CTRL_ADDR_CALC[2] = 356;
            endfunction

            parameter logic [ADDR_WIDTH-1:0] FUNCTION_PARAM [N_AXI_LITE-1:0] = CTRL_ADDR_CALC();


            dependency #(
                .param_1(FUNCTION_PARAM[2])
            ) dep ();

        endmodule
    )";



    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto fcn_param = ast_v2->find_parameter("FUNCTION_PARAM");
    ASSERT_NE(fcn_param, nullptr);
    mdarray<hdl_integer>::md_1d_array reference = {100, 130, 356};
    ASSERT_TRUE(fcn_param->get_int_array_value().has_value());
    EXPECT_EQ(fcn_param->get_int_array_value().value().get_1d_slice({0, 0}), reference);

    auto dep = ast_v2->get_dependencies()[0];
    auto param_1 = dep->find_parameter("param_1");
    ASSERT_NE(param_1, nullptr);
    EXPECT_EQ(param_1->get_numeric_value(), 356);
    auto p1_t = dep->find_parameter("p1_t");
    ASSERT_NE(p1_t, nullptr);
    EXPECT_EQ(p1_t->get_numeric_value(), 358);
}



TEST(parameter_processing, override_package_function) {
    auto test_pattern = R"(
        package  test_pack;
            parameter test_param = 100;
        endpackage

        module dependency #(
            parameter param_1 = 4
        )();

            parameter ADDR_WIDTH = 32;
            parameter N_AXI_LITE = 3;

            typedef logic [31:0] ctrl_addr_init_t [2:0];
            function ctrl_addr_init_t CTRL_ADDR_CALC();
                for(int i = 0; i<3; i++)begin
                    CTRL_ADDR_CALC[i] = param_1*i;
                end
            endfunction

            parameter [31:0] p1_t [2:0] = CTRL_ADDR_CALC();

        endmodule

        module test_mod #(
        )();

            parameter ADDR_WIDTH = 32;
            parameter N_AXI_LITE = 3;


            dependency #(
                .param_1(test_pack::test_param)
            ) dep ();

        endmodule
    )";



    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto pkg = std::static_pointer_cast<hdl_resource_statement>(resources[0]);
    auto dep =  std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    parameter_solver::propagate_functions(pkg, nullptr);
    parameter_solver::propagate_functions(dep, nullptr);
    auto pkg_defaults = parameter_solver::process_parameters(pkg->get_parameter_statements(),  {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_pack", id.get_name())] = val;
    }

    auto dep_params = dep->get_parameter_statements();
    std::vector<std::shared_ptr<HDL_parameter>> to_solve;
    for (auto &param : dep_params) {
        if (param->get_name() == "param_1") {
            auto override_param = std::make_shared<HDL_parameter>();
            override_param->set_name("param_1");
            override_param->set_type(Type_engine::create_primitive_type("implicit"));
            override_param->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("test_pack", "test_param")));
            override_param->set_type(param->get_type());
            to_solve.push_back(override_param);
        } else {
            to_solve.push_back(param);
        }
    }

    auto solved = parameter_solver::process_parameters(to_solve, ctx);

    mdarray<hdl_integer>av;
    av.set_1d_slice({0,0}, {0, 100, 200});
    EXPECT_EQ(solved.at(qualified_identifier("p1_t")).get_int_array(), av);
}


TEST(parameter_processing, parameter_with_for_loop) {
    auto test_pattern = R"(

        module dependency #(
            parameter DMA_BASE_ADDRESS = 4
        )();

            parameter p1_t = DMA_BASE_ADDRESS+2;

        endmodule

        module test_mod #(
            parameter N_CORES = 2
        )();
          	localparam N_REGISTERS = 4;

            typedef logic [31:0] ctrl_addr_init_t [N_REGISTERS-1:0];
            function ctrl_addr_init_t CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 100;
                CTRL_ADDR_CALC[1] = 130;
                CTRL_ADDR_CALC[2] = 356;
                CTRL_ADDR_CALC[3] = 62;
            endfunction

            parameter ctrl_addr_init_t AXI_ADDRESSES = CTRL_ADDR_CALC();
            genvar n;
            for(n = 0; n<N_CORES; n=n+1)begin
                dependency #(
                    .DMA_BASE_ADDRESS(AXI_ADDRESSES[(N_CORES+1)-n])
                ) dep ();
            end
        endmodule
    )";



    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];
    auto deps = ast_v2->get_dependencies();

    std::vector<hdl_integer> param_1;
    std::vector<hdl_integer> p1_t;

    for(auto dep : deps) {
        auto dma_param = dep->find_parameter("DMA_BASE_ADDRESS");
        auto p1t_param = dep->find_parameter("p1_t");
        ASSERT_NE(dma_param, nullptr);
        ASSERT_NE(p1t_param, nullptr);
        auto val_1 = dma_param->get_numeric_value();
        auto val_2 =  p1t_param->get_numeric_value();
        ASSERT_TRUE(val_1.has_value());
        param_1.push_back(val_1.value());
        ASSERT_TRUE(val_2.has_value());
        p1_t.push_back(val_2.value());
    }

    std::vector<hdl_integer> expected_param_1 = {62, 356};
    EXPECT_EQ(param_1, expected_param_1);
    std::vector<hdl_integer> expected_p1_t = {64, 358};
    EXPECT_EQ(p1_t, expected_p1_t);

}

TEST(parameter_processing, parameter_with_for_loop_incr) {
    auto test_pattern = R"(

        module dependency #(
            parameter DMA_BASE_ADDRESS = 4
        )();

            parameter p1_t = DMA_BASE_ADDRESS+2;

        endmodule

        module test_mod #(
            parameter N_CORES = 2
        )();
          	localparam N_REGISTERS = 4;

            typedef logic [31:0] ctrl_addr_init_t [N_REGISTERS-1:0];
            function ctrl_addr_init_t CTRL_ADDR_CALC();
                CTRL_ADDR_CALC[0] = 100;
                CTRL_ADDR_CALC[1] = 130;
                CTRL_ADDR_CALC[2] = 356;
                CTRL_ADDR_CALC[3] = 62;
            endfunction

            parameter ctrl_addr_init_t AXI_ADDRESSES = CTRL_ADDR_CALC();
            genvar n;
            for(n = 0; n<N_CORES; n++)begin
                dependency #(
                    .DMA_BASE_ADDRESS(AXI_ADDRESSES[(N_CORES+1)-n])
                ) dep ();
            end
        endmodule
    )";



    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];
    auto deps = ast_v2->get_dependencies();

    std::vector<hdl_integer> param_1;
    std::vector<hdl_integer> p1_t;

    for(auto dep : deps) {
        auto dma_param = dep->find_parameter("DMA_BASE_ADDRESS");
        auto p1t_param = dep->find_parameter("p1_t");
        ASSERT_NE(dma_param, nullptr);
        ASSERT_NE(p1t_param, nullptr);
        auto val_1 = dma_param->get_numeric_value();
        auto val_2 =  p1t_param->get_numeric_value();
        ASSERT_TRUE(val_1.has_value());
        param_1.push_back(val_1.value());
        ASSERT_TRUE(val_2.has_value());
        p1_t.push_back(val_2.value());
    }

    std::vector<hdl_integer> expected_param_1 = {62, 356};
    EXPECT_EQ(param_1, expected_param_1);
    std::vector<hdl_integer> expected_p1_t = {64, 358};
    EXPECT_EQ(p1_t, expected_p1_t);

}




TEST(parameter_processing, parent_parameter_collision) {
    auto test_pattern = R"(

        module inner_dep();

        endmodule

        module dependency #(
            INNER_PARAMETER = 1
        )();
            genvar n;
            for(n = 0; n<INNER_PARAMETER; n=n+1)begin
                inner_dep #() dep ();
            end
        endmodule

        module test_mod #(
        )();

            parameter  [31:0] INNER_PARAMETER = 5;
            dependency #(
                .INNER_PARAMETER(2)
            ) d ();
        endmodule
    )";


    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto deps = ast_v2->get_dependencies();
    auto inner_param = deps[0]->find_parameter("INNER_PARAMETER");
    ASSERT_NE(inner_param, nullptr);
    ASSERT_EQ(inner_param->get_numeric_value(), 2);

}


TEST(parameter_processing, override_after_function_localparam) {
    auto test_pattern = R"(

    module PwmControlUnit #(
        INITIAL_STOPPED_STATE = 0
    )();

    endmodule

    module PwmGenerator #(
        parameter INITIAL_STOPPED_STATE = 52
    )();

        function core_addr_init_t CORE_ADDR_CALC();
                CORE_ADDR_CALC[0] = 10;
                CORE_ADDR_CALC[1] = 20;
        endfunction

        localparam [31:0] AXI_ADDRESSES [1:0] = CORE_ADDR_CALC();

        PwmControlUnit #(
            .INITIAL_STOPPED_STATE(INITIAL_STOPPED_STATE)
        ) pwm_cu();

    endmodule


    )";

    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"PwmGenerator"}))[0];

    auto deps = ast_v2->get_dependencies();
    auto stopped_param = deps[0]->find_parameter("INITIAL_STOPPED_STATE");
    ASSERT_NE(stopped_param, nullptr);
    ASSERT_EQ(stopped_param->get_numeric_value(), 52);
    auto i_l = stopped_param->get_expression();
    ASSERT_TRUE(i_l->is<Identifier_token>());
}




TEST(parameter_processing, init_list_override) {
    auto test_pattern = R"(

    module axil_crossbar_interface #(
        parameter NS = 8,
        parameter [31:0] SLAVE_ADDR [NS-1:0] = '{NS{0}},
        parameter [31:0] SLAVE_MASK [NS-1:0] =  '{NS{0}}
    ) (
    );


    endmodule

    module top_module #(
        parameter BASE_ADDRESS = 32'h43c00000
    )();

        localparam CONTROLLER_ADDRESS = BASE_ADDRESS;
        localparam SPI_ADDRESS = BASE_ADDRESS+'h40;

        axil_crossbar_interface #(
            .NS(2),
            .SLAVE_ADDR('{CONTROLLER_ADDRESS, SPI_ADDRESS}),
            .SLAVE_MASK('{2{32'h040}})
        ) axi_xbar (
        );

    endmodule
    )";

    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top_module"}))[0];

    auto dep = ast_v2->get_dependencies()[0];

    std::vector<std::shared_ptr<HDL_parameter>> check_params;
    HDL_parameter p;
    p.set_name("NS");
    p.set_type(Type_engine::create_primitive_type("implicit"));
    p.set_raw_value(std::make_shared<Numeric_token>("2"));
    p.set_value(2);
    check_params.push_back(std::make_shared<HDL_parameter>(p));
    p = HDL_parameter();
    p.set_name("SLAVE_ADDR");
    auto param_type = HDL_simple_type();
    Expression_v2 e;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("NS")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::subtract);
    param_type.add_dimension({
        std::make_shared<Expression_v2>(e),
        std::make_shared<Numeric_token>("0"),
        false});
    param_type.add_dimension({
        std::make_shared<Numeric_token>("31"),
        std::make_shared<Numeric_token>("0"),
    true});
    Concatenation c;

    c.add_component(std::make_shared<Numeric_token>(1136656384, 0));
    c.add_component(std::make_shared<Numeric_token>(1136656448, 0));

    p.set_type(std::make_shared<HDL_simple_type>(param_type));
    p.set_raw_value(std::make_shared<Concatenation>(c));

    mdarray<hdl_integer> av;
    av.set_1d_slice({0,0}, {1136656448,1136656384});
    p.set_value(av);

    check_params.push_back(std::make_shared<HDL_parameter>(p));
    p = HDL_parameter();
    p.set_name("SLAVE_MASK");
    Replication r;
    r.set_item(std::make_shared<Numeric_token>(64, 32));
    r.set_size(std::make_shared<Numeric_token>(2, 1));
    HDL_simple_type param_type_2;
    e.set_lhs(std::make_shared<Identifier_token>(qualified_identifier("NS")));
    e.set_rhs(std::make_shared<Numeric_token>("1"));
    e.set_operation(Expression_v2::subtract);
    param_type_2.add_dimension({
    std::make_shared<Expression_v2>(e),
    std::make_shared<Numeric_token>("0"),
    false});
    param_type_2.add_dimension({
    std::make_shared<Numeric_token>("31"),
    std::make_shared<Numeric_token>("0"),
    true});
    p.set_type(std::make_shared<HDL_simple_type>(param_type_2));
    p.set_raw_value(std::make_shared<Replication>(r));

    av.set_1d_slice({0,0}, {64,64});
    p.set_value(av);

    check_params.push_back(std::make_shared<HDL_parameter>(p));


    ASSERT_EQ(check_params.size(), dep->get_parameters().size());

    for (size_t i = 0; i < check_params.size(); ++i) {
        ASSERT_EQ(*check_params[i], *dep->get_parameters()[i]);
    }
}


TEST(parameter_processing, override_as_dependency) {
    auto test_pattern = R"(

    module filter #(
        parameter MAIN_DECIMATION_RATIO = 128
    ) (
    );

    localparam main_clock_selector = $clog2(MAIN_DECIMATION_RATIO)-2;
    localparam [7:0] filter_width_map [7:0] = '{  27, 25, 22, 20, 16, 13, 10, 7};
    localparam [7:0] main_filter_resolution = filter_width_map[main_clock_selector];

    endmodule

    module top_module #(
    )();


        filter #(
            .MAIN_DECIMATION_RATIO(256)
        ) flt_instance (
        );

    endmodule
    )";

    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top_module"}))[0];

    auto dep = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(dep->has_parameter("main_filter_resolution"));

    auto mf_param = dep->find_parameter("main_filter_resolution");
    ASSERT_NE(mf_param, nullptr);
    auto value = mf_param->get_numeric_value();
    EXPECT_EQ(value.value(), 13);
}




TEST(parameter_processing, override_parameter_in_loop_index) {
    auto test_pattern = R"(

module top_module();

    hil_base_logic #(
        .ADDR_WIDTH(36),
        .N_CORES(7)
    ) sim_core();

endmodule


module hil_base_logic#(
    parameter ADDR_WIDTH = 32,
    parameter N_CORES = 3
)(
);

    typedef logic [ADDR_WIDTH-1:0] core_addr_init_t [N_CORES-1:0];
    function core_addr_init_t CORE_ADDR_CALC();
        for(int i = 0; i<N_CORES; i++)begin
            CORE_ADDR_CALC[N_CORES-1-i] = 1200+'h10000*i;
        end
    endfunction

    localparam [ADDR_WIDTH-1:0] CORES_AXI_ADDRESSES [N_CORES-1:0] = CORE_ADDR_CALC();


    for(n = 0; n<N_CORES; n=n+1)begin
        fcore_complex #(
            .DMA_BASE_ADDRESS(CORES_AXI_ADDRESSES[n])
        )core_c(
        );
    end

endmodule


module fcore_complex #(
    parameter DMA_BASE_ADDRESS = 32'h43c00000
)(
);
    parameter addr2 = DMA_BASE_ADDRESS*1;
endmodule
    )";

    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top_module"}))[0];
    auto dep_0 = ast_v2->get_dependencies()[0]->get_dependencies()[0];
    EXPECT_TRUE(dep_0->has_parameter("addr2"));
    auto addr2_0 = dep_0->find_parameter("addr2");
    ASSERT_NE(addr2_0, nullptr);
    EXPECT_EQ(addr2_0->get_numeric_value().value().get_value(), 394416);
    auto dep_6 = ast_v2->get_dependencies()[0]->get_dependencies()[6];
    EXPECT_TRUE(dep_6->has_parameter("addr2"));
    auto addr2_6 = dep_6->find_parameter("addr2");
    ASSERT_NE(addr2_6, nullptr);
    EXPECT_EQ(addr2_6->get_numeric_value().value().get_value(), 1200);
}

TEST(parameter_processing, override_parameter_in_loop_index_with_package) {
    auto test_pattern = R"(
package test_package;
    parameter test_parameter = 1200;
endpackage;


module top_module();

    hil_base_logic #(
        .ADDR_WIDTH(36),
        .N_CORES(7)
    ) sim_core();

endmodule


module hil_base_logic#(
    parameter ADDR_WIDTH = 32,
    parameter N_CORES = 3
)(
);

    typedef logic [ADDR_WIDTH-1:0] core_addr_init_t [N_CORES-1:0];
    function core_addr_init_t CORE_ADDR_CALC();
        for(int i = 0; i<N_CORES; i++)begin
            CORE_ADDR_CALC[N_CORES-1-i] = test_package::test_parameter+'h10000*i;
        end
    endfunction

    localparam [ADDR_WIDTH-1:0] CORES_AXI_ADDRESSES [N_CORES-1:0] = CORE_ADDR_CALC();


    for(n = 0; n<N_CORES; n=n+1)begin
        fcore_complex #(
            .DMA_BASE_ADDRESS(CORES_AXI_ADDRESSES[n])
        )core_c(
        );
    end

endmodule


module fcore_complex #(
    parameter DMA_BASE_ADDRESS = 32'h43c00000
)(
);
    parameter addr2 = DMA_BASE_ADDRESS*1;
endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resource = file.get_content()[0]->as<hdl_resource_statement>();
    auto pkg_solved = parameter_solver::process_parameters(
        resource.get_parameter_statements(), {});
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", file});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top_module"}))[0];
    auto dep_0 = ast_v2->get_dependencies()[0]->get_dependencies()[0];
    EXPECT_TRUE(dep_0->has_parameter("addr2"));
    auto addr2_0 = dep_0->find_parameter("addr2");
    ASSERT_NE(addr2_0, nullptr);
    EXPECT_EQ(addr2_0->get_numeric_value().value().get_value(), 394416);
    auto dep_6 = ast_v2->get_dependencies()[0]->get_dependencies()[6];
    EXPECT_TRUE(dep_6->has_parameter("addr2"));
    auto addr2_6 = dep_6->find_parameter("addr2");
    ASSERT_NE(addr2_6, nullptr);
    EXPECT_EQ(addr2_6->get_numeric_value().value().get_value(), 1200);
}


TEST(parameter_processing, override_with_package_ref_in_array_init) {
    auto test_pattern = R"(

    package pkg;
        parameter a = 10;
        parameter b = 20;
        parameter c = 30;
    endpackage

    module child #(
        parameter int X = 0
    )();
        parameter Y = X * 2;
    endmodule

    module top();
        child #(
            .X({pkg::a, pkg::b, pkg::c})
        ) inst();
    endmodule
    )";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto inst = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(inst->has_parameter("X"));
    auto x_param = inst->find_parameter("X");
    ASSERT_NE(x_param, nullptr);
    EXPECT_TRUE(x_param->get_numeric_value().has_value());
    EXPECT_EQ(x_param->get_numeric_value().value().get_value(), 30);

    EXPECT_TRUE(inst->has_parameter("Y"));
    auto y_param = inst->find_parameter("Y");
    ASSERT_NE(y_param, nullptr);
    EXPECT_TRUE(y_param->get_numeric_value().has_value());
    EXPECT_EQ(y_param->get_numeric_value().value().get_value(), 60);
}

TEST(parameter_processing, intermediate_interface_param) {
    auto test_pattern = R"(

    interface axis_if #(DATA_WIDTH = 32, DEST_WIDTH = 32, USER_WIDTH = 32);
    endinterface

        module child #(
            parameter DATA_WIDTH_2 = 32
        )();

        endmodule

        module middle #()(
            axis_if.data_out tsif
        );

            parameter local_dw = tsif.DATA_WIDTH;

            child #(
                .DATA_WIDTH_2(local_dw)
            ) inner_inst ();

        endmodule

        module top #(
        )();

            axis_if #(.DATA_WIDTH(8)) iface();

            middle mid_inst (
                .tsif(iface)
            );

        endmodule
    )";

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto param = ast_v2->get_dependencies()[1]->find_parameter("local_dw");
    ASSERT_NE(param, nullptr);
    EXPECT_EQ(param->get_numeric_value().value(), 8);
}


TEST(parameter_processing, intermediate_interface_in_override_param) {
    auto test_pattern = R"(

    interface axis_if #(DATA_WIDTH = 32, DEST_WIDTH = 32, USER_WIDTH = 32);
    endinterface

        module child #(
            parameter DATA_WIDTH_2 = 32
        )();

        endmodule

        module middle #()(
            axis_if.data_out tsif
        );

            child #(
                .DATA_WIDTH_2(tsif.DATA_WIDTH)
            ) inner_inst ();

        endmodule

        module top #(
        )();

            axis_if #(.DATA_WIDTH(8)) iface();

            middle mid_inst (
                .tsif(iface)
            );

        endmodule
    )";

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto param = ast_v2->get_dependencies()[1]->get_dependencies()[0]->find_parameter("DATA_WIDTH_2");
    ASSERT_NE(param, nullptr);
    EXPECT_EQ(param->get_numeric_value().value(), 8);
}


TEST(parameter_processing, interface_param_same_name_as_child_param_with_loopback) {
    // This test reproduces the issue seen in noise_generator.sv where the axis_skid_buffer
    // child module has parameters DATA_WIDTH, DEST_WIDTH, USER_WIDTH that match the
    // axi_stream interface parameter names, and the child's output loops back to the same
    // interface port, creating a circular dependency that can't be resolved in 100 passes.
    auto test_pattern = R"(

    interface axi_stream #(DATA_WIDTH = 32, DEST_WIDTH = 32, USER_WIDTH = 32);
        logic [DATA_WIDTH-1:0] data;
        logic [DEST_WIDTH-1:0] dest;
        logic [USER_WIDTH-1:0] user;
        logic valid;
        logic ready;
        logic tlast;
        modport master(input ready, output data, valid, tlast, user, dest);
        modport slave(output ready, input data, valid, tlast, user, dest);
    endinterface

        module axis_skid_buffer #(
            parameter DATA_WIDTH = 16,
            parameter DEST_WIDTH = 16,
            parameter USER_WIDTH = 16,
            REGISTER_OUTPUT = 1,
            LATCHING = 0
        )(
            input wire clock,
            input wire reset,
            axi_stream.slave axis_in,
            axi_stream.master axis_out
        );


        endmodule

        module noise_generator #(
            FLOAT_OUT = 0
        )(
            input wire clock,
            input wire reset,
            axi_stream.master data_out
        );

            axi_stream reg_in();

            axis_skid_buffer #(
                .REGISTER_OUTPUT(1),
                .LATCHING(0),
                .DATA_WIDTH(data_out.DATA_WIDTH),
                .DEST_WIDTH(data_out.DEST_WIDTH),
                .USER_WIDTH(data_out.USER_WIDTH)
            ) output_reg (
                .clock(clock),
                .reset(reset),
                .axis_in(reg_in),
                .axis_out(data_out)
            );

        endmodule

        module top #(
        )();

            axi_stream #(.DATA_WIDTH(14), .DEST_WIDTH(14), .USER_WIDTH(14)) iface();

            noise_generator ng (
                .clock(1'b0),
                .reset(1'b0),
                .data_out(iface)
            );

        endmodule
    )";

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    // noise_generator is dependency[1] in top (after the interface instance)
    // axis_skid_buffer is dependency[1] in noise_generator (after the reg_in interface instance)
    auto dep = ast_v2->get_dependencies()[1]->get_dependencies()[1];

    // These should resolve to 32 from the interface default/override,
    // but the 100-pass bug prevents resolution
    EXPECT_TRUE(dep->has_parameter("DATA_WIDTH"));
    EXPECT_TRUE(dep->has_parameter("DEST_WIDTH"));
    EXPECT_TRUE(dep->has_parameter("USER_WIDTH"));

    auto dw_p = dep->find_parameter("DATA_WIDTH");
    auto dew_p = dep->find_parameter("DEST_WIDTH");
    auto uw_p = dep->find_parameter("USER_WIDTH");
    ASSERT_NE(dw_p, nullptr);
    ASSERT_NE(dew_p, nullptr);
    ASSERT_NE(uw_p, nullptr);
    auto dw = dw_p->get_numeric_value();
    auto dew = dew_p->get_numeric_value();
    auto uw = uw_p->get_numeric_value();

    EXPECT_TRUE(dw.has_value());
    EXPECT_TRUE(dew.has_value());
    EXPECT_TRUE(uw.has_value());
    if(dw.has_value()) EXPECT_EQ(dw.value().get_value(), 14);
    if(dew.has_value()) EXPECT_EQ(dew.value().get_value(), 14);
    if(uw.has_value()) EXPECT_EQ(uw.value().get_value(), 14);
}


TEST(parameter_processing, interface_param_same_name_as_child_param_no_loopback) {
    // Isolates whether the issue is the same parameter names (DATA_WIDTH = data_out.DATA_WIDTH)
    // or the loopback connection. This variant has same-name parameters but NO loopback.
    auto test_pattern = R"(

    interface axi_stream #(DATA_WIDTH = 32, DEST_WIDTH = 32, USER_WIDTH = 32);
        logic [DATA_WIDTH-1:0] data;
        logic [DEST_WIDTH-1:0] dest;
        logic [USER_WIDTH-1:0] user;
        modport master(output data, dest, user);
        modport slave(input data, dest, user);
    endinterface

        module child #(
            parameter DATA_WIDTH = 32
        )();

        endmodule

        module parent #()(
            axi_stream.master data_out
        );

            child #(
                .DATA_WIDTH(data_out.DATA_WIDTH)
            ) ch ();

        endmodule

        module top #(
        )();

            axi_stream #(.DATA_WIDTH(8), .DEST_WIDTH(16), .USER_WIDTH(4)) iface();

            parent p (
                .data_out(iface)
            );

        endmodule
    )";

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dep = ast_v2->get_dependencies()[1]->get_dependencies()[0];

    EXPECT_TRUE(dep->has_parameter("DATA_WIDTH"));
    auto dw_p = dep->find_parameter("DATA_WIDTH");
    ASSERT_NE(dw_p, nullptr);
    auto dw = dw_p->get_numeric_value();
    EXPECT_TRUE(dw.has_value());
    if(dw.has_value()) EXPECT_EQ(dw.value().get_value(), 8);
}

TEST(parameter_processing, ternary_with_package_override) {
    auto test_pattern = R"(

    package pkg;
        parameter a = 16'h100;
        parameter b = 16'h200;
    endpackage;

    module child #(
        parameter string CONVERTER_SELECTION = "",
        parameter N_PWM_CHANNELS = 4,
        parameter N_PARAMETERS = CONVERTER_SELECTION=="VSI" ? N_PWM_CHANNELS: 13,
        parameter [31:0] INITIAL_PARAMETERS_VALUES [N_PARAMETERS+1:0] = '{default:0}
    )();
    endmodule

    module top();
        child #(
            .CONVERTER_SELECTION("DAB"),
            .BASE_ADDRESS(pkg::a),
            .PWM_BASE_ADDR(pkg::b),
            .N_PWM_CHANNELS(2)
        ) inst();
    endmodule
    )";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto inst = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(inst->has_parameter("N_PARAMETERS"));
    EXPECT_TRUE(inst->has_parameter("INITIAL_PARAMETERS_VALUES"));
}

TEST(parameter_processing, override_dep_on_local_param_chain) {
    auto test_pattern = R"(

    module child #(
        parameter W = 8,
        parameter X = W+1,
        parameter [X-1:0] P = 0
    )();

    endmodule

    module top #()();
        child #(.P(X+1)) inst();
    endmodule
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dep = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(dep->has_parameter("W"));
    auto w_param = dep->find_parameter("W");
    ASSERT_NE(w_param, nullptr);
    auto w_val = w_param->get_numeric_value();
    EXPECT_EQ(w_val.value(), 8);

    EXPECT_TRUE(dep->has_parameter("X"));
    auto x_param = dep->find_parameter("X");
    ASSERT_NE(x_param, nullptr);
    auto x_val = x_param->get_numeric_value();
    EXPECT_EQ(x_val.value(), 9);

    EXPECT_TRUE(dep->has_parameter("P"));
    auto p_param = dep->find_parameter("P");
    ASSERT_NE(p_param, nullptr);
    auto p_val = p_param->get_numeric_value();
    EXPECT_EQ(p_val.value(), 10);
}

TEST(parameter_processing, packed_struct_override) {
    auto test_pattern = R"(

    module child #(
        parameter integer W = 8
    )();
        typedef struct packed {
            int field_a;
            int field_b;
        } my_struct_t;
        parameter my_struct_t s = '{1, 2};
    endmodule

    module top #()();
        child #(.s('{100, 200})) inst();
    endmodule
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dep = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(dep->has_parameter("s"));
    auto val_param = dep->find_parameter("s");
    ASSERT_NE(val_param, nullptr);
    auto val = val_param->get_numeric_value();
    ASSERT_TRUE(val.has_value());
    // packed '{100, 200} => field_a=100 (MSB), field_b=200 (LSB) => (100 << 32) | 200
    EXPECT_EQ(val.value().get_value(), 429496729800ULL);
}

TEST(parameter_processing, packed_struct_field_override_child_param) {
    auto test_pattern = R"(

    module child #(
        parameter integer field_val = 0
    )();
    endmodule

    module top #()();
        typedef struct packed {
            int field_a;
            int field_b;
        } my_struct_t;
        parameter my_struct_t s = '{42, 17};
        child #(.field_val(s.field_b)) inst();
    endmodule
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dep = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(dep->has_parameter("field_val"));
    auto field_param = dep->find_parameter("field_val");
    ASSERT_NE(field_param, nullptr);
    auto val = field_param->get_numeric_value();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value().get_value(), 17);
}

TEST(parameter_processing, unpacked_struct_field_override_child_param) {
    auto test_pattern = R"(

    module child #(
        parameter integer field_val = 0
    )();
    endmodule

    module top #()();
        typedef struct {
            int field_a;
            int field_b;
        } my_struct_t;
        parameter my_struct_t s = '{42, 17};
        child #(.field_val(s.field_b)) inst();
    endmodule
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dep = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(dep->has_parameter("field_val"));
    auto field_param = dep->find_parameter("field_val");
    ASSERT_NE(field_param, nullptr);
    auto val = field_param->get_numeric_value();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value().get_value(), 17);
}

TEST(parameter_processing, unpacked_struct_override) {
    auto test_pattern = R"(

    module child ();
        typedef struct {
            int field_a;
            int field_b;
        } my_struct_t;
        parameter my_struct_t s = '{1, 2};
    endmodule

    module top #()();
        child #(.s('{100, 200})) inst();
    endmodule
    )";

    sv_analyzer analyzer;

    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dep = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(dep->has_parameter("s"));
    auto val_param = dep->find_parameter("s");
    ASSERT_NE(val_param, nullptr);
    auto val = val_param->get_int_array_value();
    ASSERT_TRUE(val.has_value());
    // unpacked struct => array, last member at index 0
    EXPECT_EQ(val->get_value({0}), hdl_integer(200));
    EXPECT_EQ(val->get_value({1}), hdl_integer(100));
}

TEST(parameter_processing, circular_self_reference) {
    auto test_pattern = R"(
        module test_mod ();
            parameter A = A + 1;
            parameter B = 42;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    ASSERT_TRUE(ast_v2->has_parameter("A"));
    auto a_param = ast_v2->find_parameter("A");
    ASSERT_NE(a_param, nullptr);
    EXPECT_EQ(a_param->get_numeric_value(), 0);
    ASSERT_TRUE(ast_v2->has_parameter("B"));
    auto b_param = ast_v2->find_parameter("B");
    ASSERT_NE(b_param, nullptr);
    EXPECT_EQ(b_param->get_numeric_value(), 42);
}

TEST(parameter_processing, circular_mutual_dependency) {
    auto test_pattern = R"(
        module test_mod ();
            parameter A = B + 1;
            parameter B = A - 1;
            parameter C = 99;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    ASSERT_TRUE(ast_v2->has_parameter("A"));
    auto a_param = ast_v2->find_parameter("A");
    ASSERT_NE(a_param, nullptr);
    EXPECT_EQ(a_param->get_numeric_value(), 0);
    ASSERT_TRUE(ast_v2->has_parameter("B"));
    auto b_param = ast_v2->find_parameter("B");
    ASSERT_NE(b_param, nullptr);
    EXPECT_EQ(b_param->get_numeric_value(), 0);
    ASSERT_TRUE(ast_v2->has_parameter("C"));
    auto c_param = ast_v2->find_parameter("C");
    ASSERT_NE(c_param, nullptr);
    EXPECT_EQ(c_param->get_numeric_value(), 99);
}

TEST(parameter_processing, circular_three_way_cycle) {
    auto test_pattern = R"(
        module test_mod ();
            parameter A = B + 1;
            parameter B = C * 2;
            parameter C = A - 3;
            parameter D = 77;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    ASSERT_TRUE(ast_v2->has_parameter("A"));
    auto a_param = ast_v2->find_parameter("A");
    ASSERT_NE(a_param, nullptr);
    EXPECT_EQ(a_param->get_numeric_value(), 0);
    ASSERT_TRUE(ast_v2->has_parameter("B"));
    auto b_param = ast_v2->find_parameter("B");
    ASSERT_NE(b_param, nullptr);
    EXPECT_EQ(b_param->get_numeric_value(), 0);
    ASSERT_TRUE(ast_v2->has_parameter("C"));
    auto c_param = ast_v2->find_parameter("C");
    ASSERT_NE(c_param, nullptr);
    EXPECT_EQ(c_param->get_numeric_value(), 0);
    ASSERT_TRUE(ast_v2->has_parameter("D"));
    auto d_param = ast_v2->find_parameter("D");
    ASSERT_NE(d_param, nullptr);
    EXPECT_EQ(d_param->get_numeric_value(), 77);
}

TEST(parameter_processing, shift_of_clog2_roundtrip) {
    auto test_pattern = R"(
        package pkg;
            localparam int A = 1 << $clog2(8);
            localparam int B = $clog2(8);
            localparam int C = 1 << 3;
        endpackage
    )";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto& pkg = resources[0]->as<hdl_resource_statement>();
    auto solved = parameter_solver::process_parameters(pkg.get_parameter_statements(), {});
    ASSERT_EQ(8, solved.at(qualified_identifier("A")).get_integer());
    ASSERT_EQ(3, solved.at(qualified_identifier("B")).get_integer());
    ASSERT_EQ(8, solved.at(qualified_identifier("C")).get_integer());
}

TEST(parameter_processing, generate_localparam_genvar_index) {
    auto test_pattern = R"(
        module leaf #(
            parameter ELEM = 0,
            parameter UI = 0,
            parameter UIP1 = 0
        )();
        endmodule

        module test_mod #()();
            parameter logic [31:0] TAB [0:1] = '{10, 20};
            for (genvar i = 0; i < 2; i++) begin : g
                localparam ELEM = TAB[i];
                localparam int unsigned UI = unsigned'(i);
                localparam int unsigned UIP1 = UI + 10;
                leaf #(
                    .ELEM(ELEM),
                    .UI(UI),
                    .UIP1(UIP1)
                ) inst();
            end
        endmodule
    )";


    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];
    auto deps = ast_v2->get_dependencies();

    ASSERT_EQ(deps.size(), 2);
    auto elem0 = deps[0]->find_parameter("ELEM");
    ASSERT_NE(elem0, nullptr);
    ASSERT_EQ(elem0->get_numeric_value(), 10);
    auto elem1 = deps[1]->find_parameter("ELEM");
    ASSERT_NE(elem1, nullptr);
    ASSERT_EQ(elem1->get_numeric_value(), 20);
    auto ui0 = deps[0]->find_parameter("UI");
    ASSERT_NE(ui0, nullptr);
    ASSERT_EQ(ui0->get_numeric_value(), 0);
    auto ui1 = deps[1]->find_parameter("UI");
    ASSERT_NE(ui1, nullptr);
    ASSERT_EQ(ui1->get_numeric_value(), 1);
    auto uip0 = deps[0]->find_parameter("UIP1");
    ASSERT_NE(uip0, nullptr);
    ASSERT_EQ(uip0->get_numeric_value(), 10);
    auto uip1 = deps[1]->find_parameter("UIP1");
    ASSERT_NE(uip1, nullptr);
    ASSERT_EQ(uip1->get_numeric_value(), 11);

}

TEST(parameter_processing, diamond_package_solved_once) {
    auto test_pattern = R"(
        package pkg_d;
            parameter d = 5;
        endpackage
        package pkg_b;
            parameter b = pkg_d::d + 1;
        endpackage
        package pkg_c;
            parameter c = pkg_d::d + 2;
        endpackage
        module top #(parameter X = pkg_b::b + pkg_c::c)();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", file});

    // End to end: X resolves through both branches of the diamond.
    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto x_param = ast->find_parameter("X");
    ASSERT_NE(x_param, nullptr);
    ASSERT_EQ(x_param->get_numeric_value().value().get_value(), 13);

    // Direct retrieve through the diamond: pkg_d is reachable via pkg_b and
    // pkg_c; all three solve exactly once per call (single shared
    // visited-set in retrieve_package_parameters) with correct values.
    std::shared_ptr<hdl_resource_statement> top;
    for (auto &stmt : file.get_content()) {
        auto res = std::dynamic_pointer_cast<hdl_resource_statement>(stmt);
        if (res && res->getName() == "top") top = res;
    }
    ASSERT_NE(top, nullptr);
    auto solved = parameter_solver::retrieve_package_parameters(top->get_parameter_statements(), d_store);
    EXPECT_EQ(solved.at(qualified_identifier("pkg_d", "d")).get_integer(), 5);
    EXPECT_EQ(solved.at(qualified_identifier("pkg_b", "b")).get_integer(), 6);
    EXPECT_EQ(solved.at(qualified_identifier("pkg_c", "c")).get_integer(), 7);
}

TEST(parameter_processing, selective_import_resolves_item_only) {
    auto test_pattern = R"(
        package pkg_a;
            parameter ITEM = 42;
            parameter OTHER = 99;
        endpackage
        package pkg_b;
            parameter ITEM = 7;
        endpackage
        import pkg_a::ITEM;
        module top #(
            parameter Y = ITEM,
            parameter Z = OTHER
        )();
        endmodule
    )";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", file});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto y_param = ast->find_parameter("Y");
    ASSERT_NE(y_param, nullptr);
    // Selective import wins over the same-named member of pkg_b: no ambiguity.
    EXPECT_EQ(y_param->get_numeric_value().value().get_value(), 42);
    // OTHER was not imported: stays undefined, defaults to 0 (99 would leak).
    auto z_param = ast->find_parameter("Z");
    ASSERT_NE(z_param, nullptr);
    EXPECT_EQ(z_param->get_numeric_value().value().get_value(), 0);
}

TEST(parameter_processing, selective_import_picks_member_owner) {
    // Two same-named packages: only the second declares EXTRA. The selective
    // import must resolve to the declaring owner, not first-match.
    auto file_a = R"(
        package dup;
            parameter ITEM = 1;
        endpackage
    )";
    auto file_b = R"(
        package dup;
            parameter ITEM = 2;
            parameter EXTRA = 20;
        endpackage
        import dup::EXTRA;
        module top #(parameter Y = EXTRA)();
        endmodule
    )";

    sv_analyzer analyzer;
    auto res_a = analyzer.analyze("", file_a).value();
    auto res_b = analyzer.analyze("", file_b).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/a.sv", "hash_a", res_a});
    d_store->store_file({"/dev/b.sv", "hash_b", res_b});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto y_param = ast->find_parameter("Y");
    ASSERT_NE(y_param, nullptr);
    EXPECT_EQ(y_param->get_numeric_value().value().get_value(), 20);
}

#include <sstream>
#include <spdlog/sinks/ostream_sink.h>
namespace {
struct warn_counter {
    std::ostringstream stream;
    std::shared_ptr<spdlog::sinks::ostream_sink_mt> sink =
        std::make_shared<spdlog::sinks::ostream_sink_mt>(stream);
    warn_counter() { spdlog::default_logger()->sinks().push_back(sink); }
    ~warn_counter() {
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

TEST(parameter_processing, typedef_cast_of_loop_var_in_loop_local) {
    // Loop-local `F` shares its name with the child param it overrides, and
    // its `pkg::T'(genvar)` initializer parses as a size cast holding the
    // typedef reference. The cast must evaluate per iteration (F=i); without
    // that the child override self-loops in the sorter (circular warnings)
    // and every iteration silently solves to 0.
    // NOTE: package and design are analyzed in separate calls (as in real
    // per-file flows), so the typedef is unknown at design-parse time and the
    // cast takes the size-cast path. A single analyze call would resolve it as
    // a type cast and never exercise this.
    auto pkg_pattern = R"(
        package p;
            typedef enum logic [1:0] { A, B, C } fmt_e;
        endpackage
    )";
    auto test_pattern = R"(
        module child #(parameter p::fmt_e F = p::fmt_e'(0))();
            localparam int W = int'(F);
        endmodule
        module top();
            for (genvar i = 0; i < 2; i++) begin : g
                if (1) begin : b
                    localparam p::fmt_e F = p::fmt_e'(i);
                    child #(.F(F)) inst();
                end
            end
        endmodule
    )";

    sv_analyzer analyzer;
    auto pkg_file = analyzer.analyze("/dev/pkg.sv", pkg_pattern).value();
    auto file = analyzer.analyze("/dev/design.sv", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/pkg.sv", "hash_pkg", pkg_file});
    d_store->store_file({"/dev/design.sv", "hash_design", file});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    warn_counter logs;
    auto ast = b2.build_ast(std::vector<std::string>({"top"}))[0];
    EXPECT_EQ(logs.count("circular dependency"), 0u);
    auto deps = ast->get_dependencies();
    ASSERT_EQ(deps.size(), 2u);
    auto f0 = deps[0]->find_parameter("F");
    auto f1 = deps[1]->find_parameter("F");
    ASSERT_NE(f0, nullptr);
    ASSERT_NE(f1, nullptr);
    EXPECT_EQ(f0->get_numeric_value().value().get_value(), 0);
    EXPECT_EQ(f1->get_numeric_value().value().get_value(), 1);
    auto w0 = deps[0]->find_parameter("W");
    auto w1 = deps[1]->find_parameter("W");
    ASSERT_NE(w0, nullptr);
    ASSERT_NE(w1, nullptr);
    EXPECT_EQ(w0->get_numeric_value().value().get_value(), 0);
    EXPECT_EQ(w1->get_numeric_value().value().get_value(), 1);
}
