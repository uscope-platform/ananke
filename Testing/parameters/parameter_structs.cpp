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

#include "analysis/HDL_ast_builder_v2.hpp"
#include "data_model/data_store.hpp"
#include "data_model/settings_store.hpp"

using namespace std::string_literals;

TEST(parameter_extraction, struct_typed_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef struct packed {
                int field_a;
                int field_b;
            } my_struct_t;
            parameter my_struct_t struct_param = '{42, 17};
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("struct_param"));

    auto p = parameters.get("struct_param");
    HDL_struct_type check_struct;
    check_struct.packed = true;
    struct_member m;
    m.name = "field_a";
    auto t1 = Type_engine::create_primitive_type("int");
    m.type = t1;
    check_struct.member.emplace_back(m);
    m.name = "field_b";
    auto t2 = Type_engine::create_primitive_type("int");
    m.type = t2;
    check_struct.member.emplace_back(m);

    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    EXPECT_EQ(check_struct, p->get_type()->as<HDL_struct_type>());

    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("42"));
    c.add_component(std::make_shared<Numeric_token>("17"));
    EXPECT_TRUE(p->get_expression()->is<Concatenation>());
    EXPECT_EQ(p->get_expression()->as<Concatenation>(), c);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = qualified_identifier("struct_param");
    EXPECT_EQ(defaults[check_id], static_cast<uint64_t>(180388626449));
}




TEST(parameter_extraction, nested_struct_parameter) {
    auto test_pattern = R"(
        module test_mod #()();

            typedef struct packed {
              reg [7:0] field_a;
              reg [7:0] field_b;
            } inner_struct;

            typedef struct packed {
                inner_struct nested;
              reg [15:0] field_c;
            } outer_struct;

            parameter outer_struct struct_param = '{'{8'hCA, 8'hFE},16'hBEBE};

        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("struct_param"));

    auto p = parameters.get("struct_param");


    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    ASSERT_EQ(p->get_type()->as<HDL_struct_type>().member[0].name, "nested");
    ASSERT_TRUE(p->get_type()->as<HDL_struct_type>().member[0].type->is<HDL_struct_type>());

    Concatenation outer_c, inner_c;
    inner_c.add_component(std::make_shared<Numeric_token>("8'hCA"));
    inner_c.add_component(std::make_shared<Numeric_token>("8'hFE"));
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    outer_c.add_component(std::make_shared<Numeric_token>("16'hBEBE"));
    EXPECT_TRUE(p->get_expression()->is<Concatenation>());
    EXPECT_EQ(p->get_expression()->as<Concatenation>(), outer_c);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = qualified_identifier("struct_param");
    EXPECT_EQ(defaults[check_id], static_cast<uint64_t>(3405692606));
}


TEST(parameter_extraction, packed_struct_access_initialization) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef struct packed {
                int field_a;
                int field_b;
            } my_struct_t;
            parameter my_struct_t struct_param = '{42, 17};
            parameter integer struct_access_param = struct_param.field_b;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = qualified_identifier("struct_access_param");
    EXPECT_EQ(defaults[check_id], 17);
}



TEST(parameter_extraction, nested_packed_struct_access_initialization) {
    auto test_pattern = R"(
        module test_mod #()();

            typedef struct packed {
              reg [7:0] field_a;
              reg [7:0] field_b;
            } inner_struct;

            typedef struct packed {
                inner_struct nested;
              reg [15:0] field_c;
            } outer_struct;

            parameter outer_struct struct_param = '{'{8'hCA, 8'hFE},16'hBEBE};
            parameter integer struct_access_param = struct_param.nested.field_b;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = qualified_identifier("struct_access_param");
    EXPECT_EQ(defaults[check_id], 254);
}


TEST(parameter_extraction, inline_nested_struct_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef struct packed {
                struct packed {
                    reg [7:0] field_a;
                    reg [7:0] field_b;
                } nested;
                reg [15:0] field_c;
            } outer_struct;

            parameter outer_struct struct_param = '{'{8'hCA, 8'hFE},16'hBEBE};

        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("struct_param"));

    auto p = parameters.get("struct_param");

    HDL_struct_type inner_check;
    inner_check.packed = true;
    struct_member m;
    m.name = "field_a";
    HDL_simple_type byte_type;
    byte_type.set_packed_dimensions({
        {std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true}
    });
    m.type = std::make_shared<HDL_simple_type>(byte_type);
    inner_check.member.push_back(m);
    m.name = "field_b";
    m.type = std::make_shared<HDL_simple_type>(byte_type);
    inner_check.member.push_back(m);

    HDL_struct_type outer_check;
    outer_check.packed = true;
    struct_member m2;
    m2.name = "nested";
    m2.type = std::make_shared<HDL_struct_type>(inner_check);
    outer_check.member.push_back(m2);
    m2 = {};
    m2.name = "field_c";
    HDL_simple_type word_type;
    word_type.set_packed_dimensions({
        {std::make_shared<Numeric_token>("15"), std::make_shared<Numeric_token>("0"), true}
    });
    m2.type = std::make_shared<HDL_simple_type>(word_type);
    outer_check.member.push_back(m2);

    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    EXPECT_EQ(outer_check, p->get_type()->as<HDL_struct_type>());

    Concatenation outer_c, inner_c;
    inner_c.add_component(std::make_shared<Numeric_token>("8'hCA"));
    inner_c.add_component(std::make_shared<Numeric_token>("8'hFE"));
    outer_c.add_component(std::make_shared<Concatenation>(inner_c));
    outer_c.add_component(std::make_shared<Numeric_token>("16'hBEBE"));
    EXPECT_TRUE(p->get_expression()->is<Concatenation>());
    EXPECT_EQ(p->get_expression()->as<Concatenation>(), outer_c);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = qualified_identifier("struct_param");
    EXPECT_EQ(defaults[check_id], static_cast<uint64_t>(3405692606));
}


TEST(parameter_extraction, inline_nested_struct_access_initialization) {
    auto test_pattern = R"(
        module test_mod #()();

            typedef struct packed {
                struct packed {
                    reg [7:0] field_a;
                    reg [7:0] field_b;
                } nested;
                reg [15:0] field_c;
            } outer_struct;

            parameter outer_struct struct_param = '{'{8'hCA, 8'hFE},16'hBEBE};
            parameter integer struct_access_param = struct_param.nested.field_b;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = qualified_identifier("struct_access_param");
    EXPECT_EQ(defaults[check_id], 254);
}


TEST(parameter_extraction, struct_unpacked_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef struct {
                int field_a;
                int field_b;
            } my_struct_t;
            parameter my_struct_t struct_param = '{42, 17};
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("struct_param"));

    auto p = parameters.get("struct_param");
    HDL_struct_type check_struct;
    struct_member m;
    m.name = "field_a";
    auto t1 = Type_engine::create_primitive_type("int");
    m.type = t1;
    check_struct.member.emplace_back(m);
    m.name = "field_b";
    auto t2 = Type_engine::create_primitive_type("int");
    m.type = t2;
    check_struct.member.emplace_back(m);

    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    EXPECT_EQ(check_struct, p->get_type()->as<HDL_struct_type>());

    Concatenation c;
    c.add_component(std::make_shared<Numeric_token>("42"));
    c.add_component(std::make_shared<Numeric_token>("17"));
    EXPECT_TRUE(p->get_expression()->is<Concatenation>());
    EXPECT_EQ(p->get_expression()->as<Concatenation>(), c);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {17, 42});

    std::map<qualified_identifier, resolved_parameter> check_defaults  = {
        {qualified_identifier("struct_param"), array_value},
        {qualified_identifier("","struct_param", "field_a"), hdl_integer(42)},
        {qualified_identifier("","struct_param", "field_b"), hdl_integer(17)}
    };
    EXPECT_EQ(defaults, check_defaults);
}




TEST(parameter_extraction, unpacked_struct_access_initialization) {
    auto test_pattern = R"(
        module test_mod #()();
            typedef struct {
                int field_a;
                int field_b;
            } my_struct_t;
            parameter my_struct_t struct_param = '{12, 32};
            parameter integer struct_access_param = struct_param.field_b;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = qualified_identifier("struct_access_param");
    EXPECT_EQ(defaults[check_id], 32);

}

TEST(parameter_extraction, packed_struct_parametrized_member_width) {
    auto test_pattern = R"(
        module test_mod #(
            parameter WIDTH = 8
        )();
            typedef struct packed {
                int field_a;
                logic [WIDTH-1:0] field_b;
            } my_struct_t;
            parameter my_struct_t s = '{42, 17};
            parameter integer extracted = s.field_b;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    qualified_identifier s_id = qualified_identifier("s");
    ASSERT_TRUE(defaults.contains(s_id));
    EXPECT_EQ(defaults[s_id], static_cast<uint64_t>(10769));

    qualified_identifier ext_id = qualified_identifier("extracted");
    ASSERT_TRUE(defaults.contains(ext_id));
    EXPECT_EQ(defaults[ext_id], 17);
}

TEST(parameter_extraction, packed_struct_parametrized_member_width_wide) {
    auto test_pattern = R"(
        module test_mod #(
            parameter WIDTH = 16
        )();
            typedef struct packed {
                logic [WIDTH-1:0] field_a;
                logic [WIDTH-1:0] field_b;
            } my_struct_t;
            parameter my_struct_t s = '{1000, 50000};
            parameter integer extracted = s.field_a;
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});

    qualified_identifier s_id = qualified_identifier("s");
    ASSERT_TRUE(defaults.contains(s_id));
    uint64_t expected = 65586000;
    EXPECT_EQ(defaults[s_id], expected);

    qualified_identifier ext_id = qualified_identifier("extracted");
    ASSERT_TRUE(defaults.contains(ext_id));
    EXPECT_EQ(defaults[ext_id], 1000);

    // Verify struct field entries in solved parameters
    qualified_identifier fa_id("field_a");
    fa_id.set_instance_prefix({"s"});
    ASSERT_TRUE(defaults.contains(fa_id));
    EXPECT_EQ(defaults[fa_id], 1000);

    qualified_identifier fb_id("field_b");
    fb_id.set_instance_prefix({"s"});
    ASSERT_TRUE(defaults.contains(fb_id));
    EXPECT_EQ(defaults[fb_id], 50000);
}


TEST(parameter_extraction, anonymous_packed_struct_typed_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            struct packed {
                int field_a;
                int field_b;
            } anon_struct = '{42, 17};
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("anon_struct"));

    auto p = parameters.get("anon_struct");
    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    auto &st = p->get_type()->as<HDL_struct_type>();
    EXPECT_TRUE(st.packed);
    ASSERT_EQ(st.member.size(), 2);
    EXPECT_EQ(st.member[0].name, "field_a");
    ASSERT_TRUE(st.member[0].type != nullptr);
    dimension_t check_dim;
    check_dim.packed = true;
    check_dim.first_bound = std::make_shared<Numeric_token>(31, 32);
    check_dim.second_bound = std::make_shared<Numeric_token>(0, 32);
    ASSERT_EQ(st.member[0].type->as<HDL_simple_type>().get_packed_dimensions()[0],check_dim);
    EXPECT_EQ(st.member[1].name, "field_b");
    ASSERT_TRUE(st.member[1].type != nullptr);
    ASSERT_EQ(st.member[1].type->as<HDL_simple_type>().get_packed_dimensions()[0],check_dim);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    qualified_identifier check_id = qualified_identifier("anon_struct");
    EXPECT_EQ(defaults[check_id], static_cast<uint64_t>(180388626449));
}

TEST(parameter_extraction, anonymous_unpacked_struct_parameter) {
    auto test_pattern = R"(
        module test_mod #()();
            struct {
                int field_a;
                int field_b;
            } anon_struct =  '{42, 17};
        endmodule
    )";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto parameters = resource.get_parameters();
    ASSERT_TRUE(parameters.contains("anon_struct"));

    auto p = parameters.get("anon_struct");
    ASSERT_TRUE(p->get_type()->is<HDL_struct_type>());
    auto &st = p->get_type()->as<HDL_struct_type>();
    EXPECT_FALSE(st.packed);
    ASSERT_EQ(st.member.size(), 2);
    EXPECT_EQ(st.member[0].name, "field_a");
    ASSERT_TRUE(st.member[0].type != nullptr);
    EXPECT_EQ(st.member[1].name, "field_b");
    ASSERT_TRUE(st.member[1].type != nullptr);

    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    mdarray<hdl_integer> array_value;
    array_value.set_1d_slice({0, 0}, {17, 42});
    qualified_identifier sid = qualified_identifier("anon_struct");
    EXPECT_EQ(defaults[sid], array_value);
}

TEST(parameter_extraction, enum_typedef_parameter) {
    auto test_pattern = R"(
        module test_mod ();
            typedef enum { IDLE, RUN, DONE } state_t;
            parameter state_t S = IDLE;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto typedefs = resource.get_typedefs();
    ASSERT_TRUE(typedefs.contains("state_t"));
    EXPECT_TRUE(typedefs.at("state_t")->is<HDL_enum_type>());
    auto &et = typedefs.at("state_t")->as<HDL_enum_type>();
    EXPECT_EQ(et.members.size(), 3);
    EXPECT_EQ(et.members[0].name, "IDLE");
    EXPECT_EQ(et.members[1].name, "RUN");
    EXPECT_EQ(et.members[2].name, "DONE");
    EXPECT_EQ(et.members[0].value.value(), 0);
    EXPECT_EQ(et.members[1].value.value(), 1);
    EXPECT_EQ(et.members[2].value.value(), 2);
}

TEST(parameter_extraction, enum_parameter_evaluation) {
    auto test_pattern = R"(
        module test_mod ();
            typedef enum { IDLE, RUN, DONE } state_t;
            parameter state_t S = RUN;
            parameter V = S + 1;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("S")).get_integer(), 1);
    EXPECT_EQ(defaults.at(qualified_identifier("V")).get_integer(), 2);
}

TEST(parameter_extraction, enum_with_explicit_values) {
    auto test_pattern = R"(
        module test_mod ();
            typedef enum { A = 10, B = 20, C = 30 } vals_t;
            parameter vals_t X = B;
            parameter Y = X + A;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("X")).get_integer(), 20);
    EXPECT_EQ(defaults.at(qualified_identifier("Y")).get_integer(), 30);
}

TEST(parameter_extraction, union_typedef_parameter) {
    auto test_pattern = R"(
        module test_mod ();
            typedef union { logic [31:0] raw; struct packed { logic [15:0] hi; logic [15:0] lo; } split; } word_t;
        endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto typedefs = resource.get_typedefs();
    ASSERT_TRUE(typedefs.contains("word_t"));
    EXPECT_TRUE(typedefs.at("word_t")->is<HDL_union_type>());
}

TEST(parameter_extraction, package_union_parameter) {
    auto test_pattern = R"(
        package my_types_pkg;
            typedef union packed {
                logic [15:0] raw;
                struct packed {
                    logic [7:0] hi;
                    logic [7:0] lo;
                } bytes;
            } bus_val_u;
        endpackage

        module test_mod #(
            parameter my_types_pkg::bus_val_u DEFAULT_VAL = 16'hABCD,
            parameter test_field_access = DEFAULT_VAL.bytes.hi
        )();
        endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    d_store->store_file({"/dev/zero", "file_hash", resources});
    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto params = ast_v2->get_parameters();
    ASSERT_TRUE(params.contains("DEFAULT_VAL"));
    auto val = params.get("DEFAULT_VAL")->get_numeric_value();
    ASSERT_TRUE(val.has_value());
    EXPECT_EQ(val.value().get_value(), 0xABCD);

    ASSERT_TRUE(params.contains("test_field_access"));
    auto field_val = params.get("test_field_access")->get_numeric_value();
    ASSERT_TRUE(field_val.has_value());
    EXPECT_EQ(field_val.value().get_value(), 0xAB);
}

TEST(parameter_extraction, inline_enum_variable) {
    auto test_pattern = R"(
module test_mod (
    input wire clock,
    input wire reset
);
    enum logic [2:0] {
        idle_state,
        active_state,
        done_state
    } state;
endmodule
    )";
    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value();
    EXPECT_FALSE(resources.get_content().empty());
}

TEST(parameter_extraction, inline_enum_with_parameters) {
    auto test_pattern = R"(
module test_mod (
    input wire clock,
    input wire reset
);
    parameter W = 8;
    enum logic [1:0] {
        idle_state,
        active_state
    } state;
    localparam V = W + 1;
endmodule
    )";
    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();
    auto defaults = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(defaults.at(qualified_identifier("W")).get_integer(), 8);
    EXPECT_EQ(defaults.at(qualified_identifier("V")).get_integer(), 9);
}



TEST(parameter_extraction, int_to_struct_cast) {
    auto test_pattern = R"(
    package pkg;
        typedef struct packed {
            int unsigned a;
            bit          b;
        } my_struct_t;

        localparam my_struct_t TEST_ZERO = my_struct_t'(0);
        localparam my_struct_t TEST_VAL  = my_struct_t'(5);
    endpackage
)";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", test_pattern).value().get_content();
    auto& pkg = resources[0]->as<hdl_resource_statement>();
    auto solved = parameter_solver::process_parameters(pkg.get_parameters(), {});

    ASSERT_EQ(0, solved.at(qualified_identifier("TEST_ZERO")).get_integer());
    ASSERT_EQ(5, solved.at(qualified_identifier("TEST_VAL")).get_integer());
}


TEST(parameter_extraction, cross_package_enum_init) {
    auto test_pattern = R"(

        package type_pkg;
            /// Cache type parameter
            typedef enum logic [2:0] {
                WB = 0,
                WT = 1,
                HPDCACHE_WT = 2,
                HPDCACHE_WB = 3,
                HPDCACHE_WT_WB = 4
              } cache_type_t;

        endpackage

    package pkg;


        /// Cache type parameter
        typedef enum logic [2:0] {
            WB = 0,
            WT = 1,
            HPDCACHE_WT = 2,
            HPDCACHE_WB = 3,
            HPDCACHE_WT_WB = 4
          } cache_type_t;

        localparam type_pkg::cache_type_t CVA6ConfigDcacheType = type_pkg::WT;

    endpackage
)";

    sv_analyzer analyzer;

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    d_store->store_file({"/dev/zero", "file_hash", file});
    std::shared_ptr<hdl_resource_statement> p_pkg = std::static_pointer_cast<hdl_resource_statement>(resources[1]);

    parameter_solver::propagate_types(p_pkg, d_store);
    auto pkg_context = parameter_solver::retrieve_package_parameters(p_pkg->get_parameters(), d_store);
    auto solved = parameter_solver::process_parameters(p_pkg->get_parameters(), pkg_context);

    ASSERT_EQ(1, solved.at(qualified_identifier("CVA6ConfigDcacheType")).get_integer().get_value());
}



TEST(parameter_extraction, array_of_struct_nested_literals) {
    // Unpacked array of structs with nested literals: the outer '{...} is an
    // array literal (one entry per element), not a struct literal for the
    // element type. Pre-fix the [0:N-1] dims on the typedef reference were
    // dropped, sizing the outer literal as the 2-member struct ("5 components
    // for 2 members") and defaulting the parameter to 0 (wrong type).
    auto test_pattern = R"(
package fp_pkg;
    typedef struct packed {
        int unsigned exp_bits;
        int unsigned man_bits;
    } fp_encoding_t;
    localparam int unsigned NUM = 5;
endpackage

module test_mod #()();
    localparam fp_pkg::fp_encoding_t [0:fp_pkg::NUM-1] TAB = '{
        '{8, 23}, '{11, 52}, '{5, 10}, '{5, 2}, '{8, 7}
    };
endmodule
)";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});
    std::shared_ptr<hdl_resource_statement> mod;
    for (auto &r : resources) {
        auto res = std::static_pointer_cast<hdl_resource_statement>(r);
        if (res->getName() == "test_mod") mod = res;
    }
    ASSERT_TRUE(mod);

    parameter_solver::propagate_types(mod, d_store);
    auto mod_context = parameter_solver::retrieve_package_parameters(mod->get_parameters(), d_store);
    auto solved = parameter_solver::process_parameters(mod->get_parameters(), mod_context);

    mdarray<hdl_integer> expected;
    expected.set_1d_slice({0, 0}, {
        static_cast<hdl_integer>((8LL << 32) | 23),
        static_cast<hdl_integer>((11LL << 32) | 52),
        static_cast<hdl_integer>((5LL << 32) | 10),
        static_cast<hdl_integer>((5LL << 32) | 2),
        static_cast<hdl_integer>((8LL << 32) | 7)
    });
    EXPECT_EQ(solved.at(qualified_identifier("TAB")), expected);
}

TEST(parameter_extraction, packed_struct_two_dimensional_member_arrays) {
    // PMA shape (cva6_cfg_t): packed members whose two dimensions both extend
    // the primitive base type. Both must stay packed so the member widths
    // multiply; routing the second to unpacked collapses every member to
    // width 1 and the whole struct solves to garbage.
    auto test_pattern = R"(
package pma_pkg;
    localparam int unsigned NR = 2;
    typedef struct packed {
        logic [NR-1:0][7:0] base;
        logic [NR-1:0][7:0] len;
    } pma_t;
    localparam pma_t CFG = '{
        '{8'h11, 8'h22},
        '{8'h33, 8'h44}
    };
endpackage
)";

    sv_analyzer analyzer;
    auto file = analyzer.analyze("", test_pattern).value();
    auto resources = file.get_content();
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    d_store->store_file({"/dev/zero", "file_hash", file});
    std::shared_ptr<hdl_resource_statement> pkg;
    for (auto &r : resources) {
        auto res = std::static_pointer_cast<hdl_resource_statement>(r);
        if (res->getName() == "pma_pkg") pkg = res;
    }
    ASSERT_TRUE(pkg);

    parameter_solver::propagate_types(pkg, d_store);
    auto pkg_context = parameter_solver::retrieve_package_parameters(pkg->get_parameters(), d_store);
    auto solved = parameter_solver::process_parameters(pkg->get_parameters(), pkg_context);

    EXPECT_EQ(solved.at(qualified_identifier("CFG")).get_integer().get_value(), 0x11223344);
    qualified_identifier base_id("base");
    base_id.set_instance_prefix({"CFG"});
    qualified_identifier len_id("len");
    len_id.set_instance_prefix({"CFG"});
    EXPECT_EQ(solved.at(base_id).get_integer().get_value(), 0x1122);
    EXPECT_EQ(solved.at(len_id).get_integer().get_value(), 0x3344);
}


TEST(parameter_extraction, array_of_struct_same_file_typedef) {
    // Same as array_of_struct_nested_literals but with a bare same-file
    // typedef reference: exercises the registry-clone path in finalize_type
    // (shared typedef object must not be polluted with dimensions).
    auto test_pattern = R"(
module test_mod #()();
    typedef struct packed {
        int unsigned a;
        int unsigned b;
    } my_t;
    localparam my_t [0:1] ARR = '{'{1, 2}, '{3, 4}};
endmodule
)";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto solved = parameter_solver::process_parameters(resource.get_parameters(), {});

    mdarray<hdl_integer> expected;
    expected.set_1d_slice({0, 0}, {
        static_cast<hdl_integer>((1LL << 32) | 2),
        static_cast<hdl_integer>((3LL << 32) | 4)
    });
    EXPECT_EQ(solved.at(qualified_identifier("ARR")), expected);
}

TEST(parameter_extraction, struct_enum_member_packed_width) {
    // A struct member typed by an enum must size from the enum, not from a
    // blind simple-type reinterpretation of the enum object.
    auto test_pattern = R"(
package p;
    typedef enum logic [1:0] {A, B, C} e_t;
    typedef struct packed {
        e_t f;
        logic g;
    } s_t;
    localparam W = $bits(s_t);
endpackage
)";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto solved = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(solved.at(qualified_identifier("W")).get_integer().get_value(), 3);
}

TEST(parameter_extraction, bits_of_struct_with_array_member) {
    // Container width must include unpacked member dims (16 + 1, not 8 + 1).
    // NOTE: the unpacked member hides behind a typedef name, as in CVA6 —
    // dims written directly on a packed-struct member are illegal SV
    // (xrun SVBPSE) and must not be used in tests.
    auto test_pattern = R"(
package p;
    typedef logic [7:0] byte_t;
    typedef byte_t [0:1] bytes_t;
    typedef struct packed {
        bytes_t m;
        logic g;
    } s_t;
    localparam W = $bits(s_t);
endpackage
)";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto solved = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(solved.at(qualified_identifier("W")).get_integer().get_value(), 17);
}

TEST(parameter_extraction, packed_struct_literal_with_array_member) {
    // An array-valued member inside a struct literal must flatten into the
    // pack (whole value), and per-element splits must read back in order.
    // Same typedef-hidden legal shape as above.
    auto test_pattern = R"(
package p;
    typedef logic [7:0] byte_t;
    typedef byte_t [0:1] bytes_t;
    typedef struct packed {
        bytes_t m;
        logic g;
    } s_t;
    localparam s_t V = '{'{8'h11, 8'h22}, 1'b1};
endpackage
)";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto solved = parameter_solver::process_parameters(resource.get_parameters(), {});
    // m = 0x1122 (16 bits) at the high end, g = 1: (0x1122 << 1) | 1 = 0x2245.
    // Ground truth cross-checked with QuestaSim 2025.2 ($display %h -> 02245).
    EXPECT_EQ(solved.at(qualified_identifier("V")).get_integer().get_value(), 0x2245);

    qualified_identifier m_id("m");
    m_id.set_instance_prefix({"V"});
    ASSERT_TRUE(solved.contains(m_id));
    auto arr = solved.at(m_id).get_int_array();
    EXPECT_EQ(arr.get_value({0}).value().get_value(), 0x11);
    EXPECT_EQ(arr.get_value({1}).value().get_value(), 0x22);
}

TEST(parameter_extraction, package_literal_bare_enum_member) {
    // A bare enum member inside a same-package struct literal must resolve
    // without staying missing.
    auto test_pattern = R"(
package p;
    typedef enum logic [1:0] {A, B, C} e_t;
    typedef struct packed {
        e_t f;
        logic g;
    } s_t;
    localparam s_t V = '{B, 1'b1};
endpackage
)";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto solved = parameter_solver::process_parameters(resource.get_parameters(), {});
    EXPECT_EQ(solved.at(qualified_identifier("V")).get_integer().get_value(), 3);
}

TEST(parameter_extraction, single_bound_dimension_completed) {
    // Single-bound dimensions (`[4]` with no colon) must not leave
    // second_bound null: the first dependency walk over such a type
    // segfaults (proven: SIGSEGV at HDL_simple_type.cpp:47). Parse-level
    // check so the repro itself can't take down the suite.
    auto test_pattern = R"(
package p;
    typedef logic [7:0] arr_t [4];
endpackage
)";

    sv_analyzer analyzer;
    auto resource = analyzer.analyze("", test_pattern).value().get_content()[0]->as<hdl_resource_statement>();

    auto typedefs = resource.get_typedefs();
    ASSERT_TRUE(typedefs.contains("arr_t"));
    auto dims = typedefs["arr_t"]->as<HDL_simple_type>().get_unpacked_dimensions();
    ASSERT_EQ(dims.size(), 1);
    EXPECT_TRUE(dims[0].first_bound != nullptr);
    EXPECT_TRUE(dims[0].second_bound != nullptr);
}
