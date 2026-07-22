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
#include "data_model/HDL/types/HDL_external_type.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"


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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_module"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();
    ASSERT_TRUE(dependency_parameters.contains("PARAM_1"));
    EXPECT_EQ(dependency_parameters.get("PARAM_1")->get_numeric_value(), 256);
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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();
    ASSERT_TRUE(dependency_parameters.contains("OPT_NONESEL"));
    EXPECT_EQ(dependency_parameters.get("OPT_NONESEL")->get_numeric_value(), 1);
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

    auto resources = analyzer.analyze("", test_pattern).get_content();
    auto& pkg = resources[0]->as<hdl_resource_statement>();
    auto& mod = resources[2]->as<hdl_resource_statement>();;

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_package",  id.get_name())] = val;
    }

    auto solved = parameter_solver::process_parameters(mod.get_parameters(), ctx);

    auto param_val = solved.at(qualified_identifier("AXI_ADDRESSES")).get_int_array();

    mdarray<hdl_integer> av;
    av.set_1d_slice({0,0}, {3,2,1});
    ASSERT_EQ(param_val, av);
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

    auto resources = analyzer.analyze("", test_pattern).get_content();
    auto& pkg = resources[0]->as<hdl_resource_statement>();;
    auto& mod = resources[1]->as<hdl_resource_statement>();;

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_package", id.get_name())] = val;
    }

    auto solved = parameter_solver::process_parameters(mod.get_parameters(), ctx);

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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();

    Parameters_map check_params;

    auto p = std::make_shared<HDL_parameter>();
    p->set_name("param_1");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    p->set_raw_value(std::make_shared<Numeric_token>(4, 0));
    p->set_value(4);

    check_params.insert(p);

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
    check_params.insert(p);

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
    check_params.insert(p);

    p = std::make_shared<HDL_parameter>();
    p->set_name("p1_t");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    auto tk2 = Identifier_token(qualified_identifier("param_2"));
    tk2.add_array_index(std::make_shared<Numeric_token>(0, 1));
    p->set_raw_value(std::make_shared<Numeric_token>(tk));
    p->set_value(9);
    check_params.insert(p);


    p = std::make_shared<HDL_parameter>();
    p->set_name("p2_t");
    p->set_type(Type_engine::create_primitive_type("implicit"));
    tk2 = Identifier_token(qualified_identifier("param_2"));
    tk2.add_array_index(std::make_shared<Numeric_token>(1, 1));
    p->set_raw_value(std::make_shared<Identifier_token>(tk2));
    p->set_value(8);
    check_params.insert(p);

    ASSERT_EQ(check_params.size(), dependency_parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(dependency_parameters.contains(name));
        ASSERT_EQ(*item, *dependency_parameters.get(name));
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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();

    auto array_val = dependency_parameters.get("TRIGGER_REGISTERS_IDX")->get_int_array_value();
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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    Depfile df;

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto deps = ast_v2->get_dependencies();

    ASSERT_EQ(deps.size(), 3);
    ASSERT_EQ(deps[0]->get_parameters().get("N_TRIGGER_REGISTERS")->get_numeric_value(), 4);
    ASSERT_EQ(deps[1]->get_parameters().get("N_TRIGGER_REGISTERS")->get_numeric_value(), 2);
    ASSERT_EQ(deps[2]->get_parameters().get("N_TRIGGER_REGISTERS")->get_numeric_value(), 6);

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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];
    auto deps = ast_v2->get_dependencies();

    auto a = deps[0]->get_parameters().get("N_TRIGGER_REGISTERS")->get_numeric_value();
    ASSERT_EQ(deps.size(), 3);
    ASSERT_EQ(deps[0]->get_parameters().get("N_TRIGGER_REGISTERS")->get_numeric_value(), 6);
    ASSERT_EQ(deps[1]->get_parameters().get("N_TRIGGER_REGISTERS")->get_numeric_value(), 2);
    ASSERT_EQ(deps[2]->get_parameters().get("N_TRIGGER_REGISTERS")->get_numeric_value(), 4);

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

    auto resources = analyzer.analyze("", test_pattern);

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto param = ast_v2->get_parameters().get("AXI_ADDRESSES");
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

    auto resources = analyzer.analyze("", test_pattern);

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto param = ast_v2->get_parameters().get("AXI_ADDRESSES");
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

    auto resources = analyzer.analyze("", test_pattern).get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("hil_address_space",  id.get_name())] = val;
    }

    parameter_solver::propagate_functions(mod, nullptr);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), ctx);

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


    auto resources = analyzer.analyze("", test_pattern).get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("hil_address_space", id.get_name())] = val;
    }

    parameter_solver::propagate_functions(mod, nullptr);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), ctx);

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

    auto resources = analyzer.analyze("", test_pattern).get_content();
    auto pkg = std::static_pointer_cast<hdl_resource_statement>(resources[0]);


    parameter_solver::propagate_functions(pkg, nullptr);
    auto solved = parameter_solver::process_parameters(pkg->get_parameters(), {});
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

    auto file =  analyzer.analyze("", test_pattern);
    auto resources = file.get_content();
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    ASSERT_TRUE(mod->get_parameters().contains("RESULT"));
    auto param = mod->get_parameters().get("RESULT");

    HDL_parameter p;
    p.set_name("RESULT");
    p.set_type(Type_engine::create_primitive_type("integer"));
    HDL_function_call c;
    c.set_name("CALC");
    c.add_package_prefix("test_pkg");
    p.set_raw_value(std::make_shared<HDL_function_call>(c));
    ASSERT_EQ(p, *param);
    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_pkg", id.get_name())] = val;
    }

    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), ctx);
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


    auto file = analyzer.analyze("", test_pattern);
    d_store->store_file({"/dev/zero", "file_hash", file});
    auto resources = file.get_content();
    auto pkg = resources[0]->as<hdl_resource_statement>();
    auto pkg2 = resources[1]->as<hdl_resource_statement>();
    auto mod = std::static_pointer_cast<hdl_resource_statement>(resources[2]);

    ASSERT_TRUE(mod->get_parameters().contains("RESULT"));
    auto param = mod->get_parameters().get("RESULT");

    HDL_parameter p;
    p.set_name("RESULT");
    p.set_type(std::make_shared<HDL_external_type>(qualified_identifier("test_pkg", "test_type")));
    HDL_function_call c;
    c.set_name("build_config");
    c.add_package_prefix("test_pkg_2");
    p.set_raw_value(std::make_shared<HDL_function_call>(c));
    ASSERT_EQ(p, *param);
    auto pkg_defaults = parameter_solver::process_parameters(pkg.get_parameters(), {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_pkg", id.get_name())] = val;
    }

    parameter_solver::propagate_types(mod, d_store);
    parameter_solver::propagate_functions(mod, d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), ctx);
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

    auto resources = analyzer.analyze("", test_pattern);

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto params = ast_v2->get_dependencies()[0]->get_parameters();
    auto param_1 = params.get("param_1");
    EXPECT_EQ(param_1->get_numeric_value(), 6);
    auto p1_t = params.get("p1_t");
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


    auto file = analyzer.analyze("", test_pattern);
    auto resource = file.get_content()[1]->as<hdl_resource_statement>();
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(resource.get_statements()[0]);
    auto raw_param = inst->get_parameters();


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto dep = ast_v2->get_dependencies()[0];
    auto params = dep->get_parameters();
    auto param_1 = params.get("DATA_WIDTH");
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

    auto file = analyzer.analyze("", test_pattern);
    auto resource = file.get_content()[2]->as<hdl_resource_statement>();
    d_store->store_file({"/dev/zero", "file_hash", file});

    auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(resource.get_statements()[1]);
    auto raw_param = inst->get_parameters();


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto dep = ast_v2->get_dependencies()[1];
    auto params = dep->get_parameters();
    auto param_1 = params.get("param_1");
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

    auto resources = analyzer.analyze("", test_pattern);

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto dep = ast_v2->get_dependencies()[1]->get_dependencies()[0];
    auto params = dep->get_parameters();
    auto param_1 = params.get("param_1");
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

    auto resources = analyzer.analyze("", test_pattern);

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];


    auto dep = ast_v2->get_dependencies()[1]->get_dependencies()[0]->get_dependencies()[0];
    auto params = dep->get_parameters();
    auto param_1 = params.get("param_1");
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

    auto resources = analyzer.analyze("", test_pattern).get_content();
    auto pkg = std::static_pointer_cast<hdl_resource_statement>(resources[0]);
    auto dep = std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    auto pkg_defaults = parameter_solver::process_parameters(pkg->get_parameters(),  {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_package", id.get_name())] = val;
    }

    auto dep_params = dep->get_parameters();
    Parameters_map to_solve;
    for (auto& [name, param] : dep_params) {
        if (name == "param_1") {
            auto override_param = std::make_shared<HDL_parameter>();
            override_param->set_name("param_1");
            override_param->set_type(Type_engine::create_primitive_type("implicit"));
            override_param->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("test_package", "base")));
            override_param->set_type(param->get_type());
            to_solve.insert(override_param);
        } else {
            to_solve.insert(param);
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

    auto resources = analyzer.analyze("", test_pattern);

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto fcn_param = ast_v2->get_parameters().get("FUNCTION_PARAM");
    mdarray<hdl_integer>::md_1d_array reference = {100, 130, 356};
    ASSERT_TRUE(fcn_param->get_int_array_value().has_value());
    EXPECT_EQ(fcn_param->get_int_array_value().value().get_1d_slice({0, 0}), reference);

    auto params = ast_v2->get_dependencies()[0]->get_parameters();
    auto param_1 = params.get("param_1");
    EXPECT_EQ(param_1->get_numeric_value(), 356);
    auto p1_t = params.get("p1_t");
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

    auto resources = analyzer.analyze("", test_pattern).get_content();
    auto pkg = std::static_pointer_cast<hdl_resource_statement>(resources[0]);
    auto dep =  std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    parameter_solver::propagate_functions(pkg, nullptr);
    parameter_solver::propagate_functions(dep, nullptr);
    auto pkg_defaults = parameter_solver::process_parameters(pkg->get_parameters(),  {});
    std::map<qualified_identifier, resolved_parameter> ctx;
    for (auto& [id, val] : pkg_defaults) {
        ctx[qualified_identifier("test_pack", id.get_name())] = val;
    }

    auto dep_params = dep->get_parameters();
    Parameters_map to_solve;
    for (auto& [name, param] : dep_params) {
        if (name == "param_1") {
            auto override_param = std::make_shared<HDL_parameter>();
            override_param->set_name("param_1");
            override_param->set_type(Type_engine::create_primitive_type("implicit"));
            override_param->set_raw_value(std::make_shared<Identifier_token>(qualified_identifier("test_pack", "test_param")));
            override_param->set_type(param->get_type());
            to_solve.insert(override_param);
        } else {
            to_solve.insert(param);
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

    auto resources = analyzer.analyze("", test_pattern);

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];
    auto deps = ast_v2->get_dependencies();

    std::vector<hdl_integer> param_1;
    std::vector<hdl_integer> p1_t;

    for(auto dep : deps) {
        auto val_1 = dep->get_parameters().get("DMA_BASE_ADDRESS")->get_numeric_value();
        auto val_2 =  dep->get_parameters().get("p1_t")->get_numeric_value();
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

    auto resources = analyzer.analyze("", test_pattern);

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];
    auto deps = ast_v2->get_dependencies();

    std::vector<hdl_integer> param_1;
    std::vector<hdl_integer> p1_t;

    for(auto dep : deps) {
        auto val_1 = dep->get_parameters().get("DMA_BASE_ADDRESS")->get_numeric_value();
        auto val_2 =  dep->get_parameters().get("p1_t")->get_numeric_value();
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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto deps = ast_v2->get_dependencies();
    ASSERT_EQ(deps[0]->get_parameters().get("INNER_PARAMETER")->get_numeric_value(), 2);

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

        function core_addr_init_t ADDR_CALC();
                CORE_ADDR_CALC[0] = 10;
                CORE_ADDR_CALC[1] = 20;
        endfunction

        localparam [31:0] AXI_ADDRESSES [1:0] = ADDR_CALC();

        PwmControlUnit #(
            .INITIAL_STOPPED_STATE(INITIAL_STOPPED_STATE)
        ) pwm_cu();

    endmodule


    )";

    sv_analyzer analyzer;


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"PwmGenerator"}))[0];

    auto deps = ast_v2->get_dependencies();
    ASSERT_EQ(deps[0]->get_parameters().get("INITIAL_STOPPED_STATE")->get_numeric_value(), 52);
    auto i_l = deps[0]->get_parameters().get("INITIAL_STOPPED_STATE")->get_expression();
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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top_module"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();

    Parameters_map check_params;
    HDL_parameter p;
    p.set_name("NS");
    p.set_type(Type_engine::create_primitive_type("implicit"));
    p.set_raw_value(std::make_shared<Numeric_token>("2"));
    p.set_value(2);
    check_params.insert(std::make_shared<HDL_parameter>(p));
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

    check_params.insert(std::make_shared<HDL_parameter>(p));
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

    check_params.insert(std::make_shared<HDL_parameter>(p));


    ASSERT_EQ(check_params.size(), dependency_parameters.size());

    for(const auto& [name, item]:check_params){
        ASSERT_TRUE(dependency_parameters.contains(name));
        ASSERT_EQ(*item, *dependency_parameters.get(name));
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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top_module"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();
    EXPECT_TRUE(dependency_parameters.contains("main_filter_resolution"));

    auto value = dependency_parameters.get("main_filter_resolution")->get_numeric_value();
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


    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top_module"}))[0];
    auto dep_0 = ast_v2->get_dependencies()[0]->get_dependencies()[0];
    EXPECT_TRUE(dep_0->get_parameters().contains("addr2"));
    EXPECT_EQ(dep_0->get_parameters().get("addr2")->get_numeric_value().value().get_value(), 394416);
    auto dep_6 = ast_v2->get_dependencies()[0]->get_dependencies()[6];
    EXPECT_TRUE(dep_6->get_parameters().contains("addr2"));
    EXPECT_EQ(dep_6->get_parameters().get("addr2")->get_numeric_value().value().get_value(), 1200);
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
    auto file = analyzer.analyze("", test_pattern);
    auto resource = file.get_content()[0]->as<hdl_resource_statement>();
    auto pkg_solved = parameter_solver::process_parameters(
        resource.get_parameters(), {});
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", file});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top_module"}))[0];
    auto dep_0 = ast_v2->get_dependencies()[0]->get_dependencies()[0];
    EXPECT_TRUE(dep_0->get_parameters().contains("addr2"));
    EXPECT_EQ(dep_0->get_parameters().get("addr2")->get_numeric_value().value().get_value(), 394416);
    auto dep_6 = ast_v2->get_dependencies()[0]->get_dependencies()[6];
    EXPECT_TRUE(dep_6->get_parameters().contains("addr2"));
    EXPECT_EQ(dep_6->get_parameters().get("addr2")->get_numeric_value().value().get_value(), 1200);
}


TEST(parameter_processing, override_with_package_ref_in_array_init) {
    auto test_pattern = R"(

    package pkg;
        parameter a = 10;
        parameter b = 20;
        parameter c = 30;
    endpackage;

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
    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto inst = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(inst->get_parameters().contains("X"));
    EXPECT_TRUE(inst->get_parameters().get("X")->get_numeric_value().has_value());
    EXPECT_EQ(inst->get_parameters().get("X")->get_numeric_value().value().get_value(), 30);
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

    auto resources = analyzer.analyze("", test_pattern);
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto param = ast_v2->get_dependencies()[1]->get_parameters().get("local_dw");
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

    auto resources = analyzer.analyze("", test_pattern);
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto param = ast_v2->get_dependencies()[1]->get_dependencies()[0]->get_parameters().get("DATA_WIDTH_2");
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

    auto resources = analyzer.analyze("", test_pattern);
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    // noise_generator is dependency[1] in top (after the interface instance)
    // axis_skid_buffer is dependency[1] in noise_generator (after the reg_in interface instance)
    auto params = ast_v2->get_dependencies()[1]->get_dependencies()[1]->get_parameters();

    // These should resolve to 32 from the interface default/override,
    // but the 100-pass bug prevents resolution
    EXPECT_TRUE(params.contains("DATA_WIDTH"));
    EXPECT_TRUE(params.contains("DEST_WIDTH"));
    EXPECT_TRUE(params.contains("USER_WIDTH"));

    auto dw = params.get("DATA_WIDTH")->get_numeric_value();
    auto dew = params.get("DEST_WIDTH")->get_numeric_value();
    auto uw = params.get("USER_WIDTH")->get_numeric_value();

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

    auto resources = analyzer.analyze("", test_pattern);
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto params = ast_v2->get_dependencies()[1]->get_dependencies()[0]->get_parameters();

    EXPECT_TRUE(params.contains("DATA_WIDTH"));
    auto dw = params.get("DATA_WIDTH")->get_numeric_value();
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
    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];
    auto inst = ast_v2->get_dependencies()[0];
    EXPECT_TRUE(inst->get_parameters().contains("N_PARAMETERS"));
    EXPECT_TRUE(inst->get_parameters().contains("INITIAL_PARAMETERS_VALUES"));
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

    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();
    EXPECT_TRUE(dependency_parameters.contains("W"));
    auto w_val = dependency_parameters.get("W")->get_numeric_value();
    EXPECT_EQ(w_val.value(), 8);

    EXPECT_TRUE(dependency_parameters.contains("X"));
    auto x_val = dependency_parameters.get("X")->get_numeric_value();
    EXPECT_EQ(x_val.value(), 9);

    EXPECT_TRUE(dependency_parameters.contains("P"));
    auto p_val = dependency_parameters.get("P")->get_numeric_value();
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

    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();
    EXPECT_TRUE(dependency_parameters.contains("s"));
    auto val = dependency_parameters.get("s")->get_numeric_value();
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

    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();
    EXPECT_TRUE(dependency_parameters.contains("field_val"));
    auto val = dependency_parameters.get("field_val")->get_numeric_value();
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

    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();
    EXPECT_TRUE(dependency_parameters.contains("field_val"));
    auto val = dependency_parameters.get("field_val")->get_numeric_value();
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

    auto resources = analyzer.analyze("", test_pattern);
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    d_store->store_file({"/dev/zero", "file_hash", resources});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto dependency_parameters = ast_v2->get_dependencies()[0]->get_parameters();
    EXPECT_TRUE(dependency_parameters.contains("s"));
    auto val = dependency_parameters.get("s")->get_int_array_value();
    ASSERT_TRUE(val.has_value());
    // unpacked struct => array, last member at index 0
    EXPECT_EQ(val->get_value({0}), hdl_integer(200));
    EXPECT_EQ(val->get_value({1}), hdl_integer(100));
}
