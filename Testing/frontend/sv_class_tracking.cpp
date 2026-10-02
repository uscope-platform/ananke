//  Copyright  2026 University of Nottingham
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

#include <sstream>

#include <cereal/archives/binary.hpp>

#include "frontend/analysis/system_verilog/sv_analyzer.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"
#include "data_model/data_store.hpp"
#include "data_model/HDL/statement/hdl_class_statement.hpp"
#include "data_model/HDL/statement/hdl_package_statement.hpp"
#include "data_model/HDL/statement/hdl_resource_statement.hpp"
#include "data_model/HDL/statement/hdl_function_statement.hpp"
#include "data_model/HDL/statement/hdl_assignment_statement.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"

namespace {

std::vector<std::shared_ptr<hdl_class_statement>> get_classes(const hdl_file &f) {
    std::vector<std::shared_ptr<hdl_class_statement>> out;
    for (const auto &e : f.get_content()) {
        if (auto c = std::dynamic_pointer_cast<hdl_class_statement>(e)) out.push_back(c);
    }
    return out;
}

hdl_class_statement make_check_class(const std::string &name, unsigned int line_n) {
    hdl_class_statement check;
    check.set_name(name);
    check.set_line_n(line_n);
    return check;
}

std::shared_ptr<HDL_parameter> make_check_prop(
    const std::string &name, const std::shared_ptr<hdl_type> &type) {
    auto p = std::make_shared<HDL_parameter>();
    p->set_name(name);
    p->set_type(type);
    return p;
}

} // namespace

TEST(sv_class_tracking, basic_class_definition) {
    auto test_pattern = R"(
        class my_class;
            int x;
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    hdl_class_statement check_class = make_check_class("my_class", 2);
    check_class.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));
    ASSERT_EQ(*classes[0], check_class);
    EXPECT_TRUE(classes[0]->get_dependencies().empty());
}

TEST(sv_class_tracking, interface_class_definition) {
    auto test_pattern = R"(
        interface class my_iface;
            pure virtual function void do_it();
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    hdl_class_statement check_class = make_check_class("my_iface", 2);
    ASSERT_EQ(*classes[0], check_class);
}

TEST(sv_class_tracking, virtual_parameterized_class) {
    auto test_pattern = R"(
        virtual class vbase #(parameter int W = 8);
        endclass
        class param_c #(parameter int N = 4) extends vbase #(N);
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 2);

    auto check_w = std::make_shared<HDL_parameter>();
    check_w->set_name("W");
    check_w->set_type(Type_engine::create_primitive_type("int"));
    check_w->set_raw_value(std::make_shared<Numeric_token>("8"));
    hdl_class_statement check_base = make_check_class("vbase", 2);
    check_base.add_property(check_w);

    auto check_n = std::make_shared<HDL_parameter>();
    check_n->set_name("N");
    check_n->set_type(Type_engine::create_primitive_type("int"));
    check_n->set_raw_value(std::make_shared<Numeric_token>("4"));
    hdl_class_statement check_child = make_check_class("param_c", 4);
    check_child.set_base_class("vbase");
    check_child.add_property(check_n);

    ASSERT_EQ(*classes[0], check_base);
    ASSERT_EQ(*classes[1], check_child);
}

TEST(sv_class_tracking, nested_classes) {
    auto test_pattern = R"(
        class outer;
            class nested_inner;
            endclass
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    // The nested class belongs to its parent, not to the top level.
    hdl_class_statement check_inner = make_check_class("nested_inner", 3);
    hdl_class_statement check_outer = make_check_class("outer", 2);
    check_outer.add_nested_class(
        std::make_shared<hdl_class_statement>(check_inner));
    ASSERT_EQ(*classes[0], check_outer);

    const auto &nested = classes[0]->get_nested_classes();
    ASSERT_EQ(nested.size(), 1);
    ASSERT_EQ(*nested[0], check_inner);
}

TEST(sv_class_tracking, deeply_nested_classes) {
    auto test_pattern = R"(
        class level_1;
            int a;
            class level_2;
                int b;
                class level_3;
                    int c;
                endclass
            endclass
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    auto int_type = Type_engine::create_primitive_type("int");
    hdl_class_statement check_3 = make_check_class("level_3", 6);
    check_3.add_property(make_check_prop("c", int_type));
    hdl_class_statement check_2 = make_check_class("level_2", 4);
    check_2.add_property(make_check_prop("b", int_type));
    check_2.add_nested_class(std::make_shared<hdl_class_statement>(check_3));
    hdl_class_statement check_1 = make_check_class("level_1", 2);
    check_1.add_property(make_check_prop("a", int_type));
    check_1.add_nested_class(std::make_shared<hdl_class_statement>(check_2));
    ASSERT_EQ(*classes[0], check_1);
}

TEST(sv_class_tracking, class_in_package) {
    auto test_pattern = R"(
        package my_pkg;
            class pkg_class;
                int x;
            endclass
        endpackage
        module m;
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());

    // The class is scoped to the package: nothing leaks to the top level.
    EXPECT_TRUE(get_classes(result.value()).empty());

    hdl_class_statement check_class = make_check_class("pkg_class", 3);
    check_class.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));

    hdl_package_statement check_pkg;
    check_pkg.set_name("my_pkg");
    check_pkg.set_language(hdl_language::system_verilog);
    check_pkg.set_line_n(2);
    check_pkg.add_statement(std::make_shared<hdl_class_statement>(check_class));

    hdl_resource_statement check_mod;
    check_mod.set_name("m");
    check_mod.set_language(hdl_language::system_verilog);
    check_mod.set_line_n(7);

    int n_pkg = 0, n_mod = 0;
    for (const auto &e : result->get_content()) {
        if (e->is<hdl_package_statement>()) {
            ASSERT_EQ(e->as<hdl_package_statement>(), check_pkg);
            n_pkg++;
        }
        if (e->is<hdl_resource_statement>()) {
            ASSERT_EQ(e->as<hdl_resource_statement>(), check_mod);
            n_mod++;
        }
    }
    EXPECT_EQ(n_pkg, 1);
    EXPECT_EQ(n_mod, 1);
}

TEST(sv_class_tracking, class_in_module) {
    auto test_pattern = R"(
        module m;
            class mod_class;
                int x;
            endclass
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());

    // Same scoping rule as packages: the class lives on the module.
    EXPECT_TRUE(get_classes(result.value()).empty());

    hdl_class_statement check_class = make_check_class("mod_class", 3);
    check_class.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));

    hdl_resource_statement check_mod;
    check_mod.set_name("m");
    check_mod.set_language(hdl_language::system_verilog);
    check_mod.set_line_n(2);
    check_mod.add_statement(std::make_shared<hdl_class_statement>(check_class));

    ASSERT_EQ(result->get_content().size(), 1);
    ASSERT_TRUE(result->get_content()[0]->is<hdl_resource_statement>());
    ASSERT_EQ(result->get_content()[0]->as<hdl_resource_statement>(), check_mod);
}

TEST(sv_class_tracking, class_body_params_do_not_leak) {
    auto test_pattern = R"(
        class my_class;
            int x;
        endclass
        module m #()();
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    hdl_class_statement check_class = make_check_class("my_class", 2);
    check_class.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));
    ASSERT_EQ(*classes[0], check_class);

    // The module must not pick up the class body contents.
    hdl_resource_statement check_mod;
    check_mod.set_name("m");
    check_mod.set_language(hdl_language::system_verilog);
    check_mod.set_line_n(5);
    for (const auto &e : result->get_content()) {
        if (e->is<hdl_resource_statement>()) {
            ASSERT_EQ(e->as<hdl_resource_statement>(), check_mod);
        }
    }
}

TEST(sv_class_tracking, data_store_lookup) {
    auto test_pattern = R"(
        class tracked_class;
        endclass
        module m;
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("/src/tracked.sv", test_pattern);
    ASSERT_TRUE(result.has_value());

    data_store store(true, "/tmp/test_sv_class_tracking");
    store.store_file({"/src/tracked.sv", "hash", result.value()});

    hdl_class_statement check_class = make_check_class("tracked_class", 2);

    auto hit = store.get_class("tracked_class");
    ASSERT_TRUE(hit.has_value());
    ASSERT_EQ(*hit.value(), check_class);

    std::string path;
    auto hit_path = store.get_class("tracked_class", path);
    ASSERT_TRUE(hit_path.has_value());
    ASSERT_EQ(*hit_path.value(), check_class);
    EXPECT_EQ(path, "/src/tracked.sv");

    EXPECT_EQ(store.get_all_classes("tracked_class").size(), 1);
    EXPECT_FALSE(store.get_class("missing_class").has_value());
    EXPECT_TRUE(store.get_all_classes("missing_class").empty());
}

TEST(sv_class_tracking, properties_basic) {
    auto test_pattern = R"(
        class my_class;
            int x;
            logic [7:0] y;
            rand bit z;
            int m1, m2;
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    auto logic_byte = Type_engine::create_primitive_type("logic");
    logic_byte->as<HDL_simple_type>().set_packed_dimensions({
        {std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true}
    });

    hdl_class_statement check_class = make_check_class("my_class", 2);
    check_class.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));
    check_class.add_property(make_check_prop("y", logic_byte));
    check_class.add_property(
        make_check_prop("z", Type_engine::create_primitive_type("bit")));
    check_class.add_property(
        make_check_prop("m1", Type_engine::create_primitive_type("int")));
    check_class.add_property(
        make_check_prop("m2", Type_engine::create_primitive_type("int")));
    ASSERT_EQ(*classes[0], check_class);

    auto props = classes[0]->get_properties();
    ASSERT_EQ(props.size(), 5);
    EXPECT_EQ(props[0]->get_name(), "x");
    EXPECT_EQ(props[3]->get_name(), "m1");
    EXPECT_EQ(props[4]->get_name(), "m2");
}

TEST(sv_class_tracking, property_with_default) {
    auto test_pattern = R"(
        class my_class;
            int x = 5;
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    // Default values are not captured yet: only name and type are filed.
    hdl_class_statement check_class = make_check_class("my_class", 2);
    check_class.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));
    ASSERT_EQ(*classes[0], check_class);
}

TEST(sv_class_tracking, class_header_parameters) {
    auto test_pattern = R"(
        class my_class #(parameter int W = 8);
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    // Header parameters keep their values, like module parameters do.
    auto check_w = std::make_shared<HDL_parameter>();
    check_w->set_name("W");
    check_w->set_type(Type_engine::create_primitive_type("int"));
    check_w->set_raw_value(std::make_shared<Numeric_token>("8"));

    hdl_class_statement check_class = make_check_class("my_class", 2);
    check_class.add_property(check_w);
    ASSERT_EQ(*classes[0], check_class);
}

TEST(sv_class_tracking, methods_basic) {
    auto test_pattern = R"(
        class my_class;
            int x;
            function integer get_x();
                get_x = x;
            endfunction
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    hdl_class_statement check_class = make_check_class("my_class", 2);
    check_class.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("get_x");
    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("get_x");
    stmt->set_value(std::make_shared<Identifier_token>(qualified_identifier("x")));
    check_f.add_statement(stmt);
    check_class.add_method(check_f);

    ASSERT_EQ(*classes[0], check_class);
    ASSERT_TRUE(classes[0]->get_function_shared("get_x") != nullptr);
}

TEST(sv_class_tracking, task_and_constructor_locals_ignored) {
    auto test_pattern = R"(
        class my_class;
            int x;
            task do_it();
                int tmp;
            endtask
            int y;
            function new();
                int ctor_local;
            endfunction
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    // Task/constructor bodies have no data-model node and their locals must
    // not leak into the property list, including declarations after them.
    hdl_class_statement check_class = make_check_class("my_class", 2);
    check_class.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));
    check_class.add_property(
        make_check_prop("y", Type_engine::create_primitive_type("int")));
    ASSERT_EQ(*classes[0], check_class);
    EXPECT_TRUE(classes[0]->get_functions().empty());
}

TEST(sv_class_tracking, method_in_nested_class) {
    auto test_pattern = R"(
        class outer;
            class inner;
                function integer get();
                    get = 1;
                endfunction
            endclass
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("get");
    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("get");
    stmt->set_value(std::make_shared<Numeric_token>("1"));
    check_f.add_statement(stmt);

    hdl_class_statement check_inner = make_check_class("inner", 3);
    check_inner.add_method(check_f);

    hdl_class_statement check_outer = make_check_class("outer", 2);
    check_outer.add_nested_class(
        std::make_shared<hdl_class_statement>(check_inner));
    ASSERT_EQ(*classes[0], check_outer);
    ASSERT_TRUE(classes[0]->get_function_shared("get") == nullptr);
}

TEST(sv_class_tracking, class_typedef) {
    auto test_pattern = R"(
        class my_class;
            typedef logic [7:0] byte_t;
            byte_t data;
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);
    EXPECT_EQ(classes[0]->getName(), "my_class");

    // Typedef maps compare by pointer, so compare the resolved value.
    auto tdefs = classes[0]->get_typedefs();
    ASSERT_TRUE(tdefs.contains("byte_t"));
    HDL_simple_type check_t;
    check_t.set_type_name("logic");
    check_t.set_packed_dimensions({
        {std::make_shared<Numeric_token>("7"), std::make_shared<Numeric_token>("0"), true}
    });
    EXPECT_EQ(check_t, tdefs.at("byte_t")->as<HDL_simple_type>());

    auto props = classes[0]->get_properties();
    ASSERT_EQ(props.size(), 1);
    EXPECT_EQ(props[0]->get_name(), "data");
}

TEST(sv_class_tracking, package_scoped_property_dependency) {
    auto test_pattern = R"(
        class my_class;
            my_pkg::my_type data;
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    qualified_identifier check_qi("my_type");
    check_qi.set_package_prefix({"my_pkg"});

    hdl_class_statement check_class = make_check_class("my_class", 2);
    auto check_prop = std::make_shared<HDL_parameter>();
    check_prop->set_name("data");
    check_prop->set_type(std::make_shared<HDL_external_type>(check_qi));
    check_class.add_property(check_prop);
    ASSERT_EQ(*classes[0], check_class);

    // No duplicate package edge is filed: the reference is already carried by
    // the property type and reported through the class dependencies.
    EXPECT_TRUE(classes[0]->get_dependencies().types.contains(check_qi));
}

TEST(sv_class_tracking, extends_plain) {
    auto test_pattern = R"(
        class my_base;
        endclass
        class my_child extends my_base;
            int x;
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 2);

    hdl_class_statement check_base = make_check_class("my_base", 2);
    ASSERT_EQ(*classes[0], check_base);
    EXPECT_FALSE(classes[0]->has_base_class());

    hdl_class_statement check_child = make_check_class("my_child", 4);
    check_child.set_base_class("my_base");
    check_child.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));
    ASSERT_EQ(*classes[1], check_child);

    // The base class shows up as a type dependency of the child.
    auto deps = classes[1]->get_dependencies();
    EXPECT_TRUE(deps.types.contains(qualified_identifier("my_base")));
}

TEST(sv_class_tracking, extends_package_qualified) {
    auto test_pattern = R"(
        class my_child extends my_pkg::my_base;
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    hdl_class_statement check_child = make_check_class("my_child", 2);
    check_child.set_base_class("my_pkg::my_base");
    ASSERT_EQ(*classes[0], check_child);

    qualified_identifier check_dep("my_base");
    check_dep.set_package_prefix({"my_pkg"});
    EXPECT_TRUE(classes[0]->get_dependencies().types.contains(check_dep));
}

TEST(sv_class_tracking, extends_parameterized_stripped) {
    auto test_pattern = R"(
        class my_child extends my_base #(8);
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    // Specializations are stripped: only the base name is tracked.
    hdl_class_statement check_child = make_check_class("my_child", 2);
    check_child.set_base_class("my_base");
    ASSERT_EQ(*classes[0], check_child);
}

TEST(sv_class_tracking, serialize_round_trip) {
    hdl_class_statement check_class = make_check_class("my_class", 42);
    check_class.add_property(
        make_check_prop("x", Type_engine::create_primitive_type("int")));

    hdl_function_statement check_f;
    check_f.set_language(hdl_language::system_verilog);
    check_f.set_name("get_x");
    auto stmt = std::make_shared<hdl_assignment_statement>();
    stmt->set_target("get_x");
    stmt->set_value(std::make_shared<Numeric_token>("1"));
    check_f.add_statement(stmt);
    check_class.add_method(check_f);

    std::stringstream ss;
    {
        cereal::BinaryOutputArchive archive_out(ss);
        archive_out(check_class);
    }
    hdl_class_statement result;
    {
        cereal::BinaryInputArchive archive_in(ss);
        archive_in(result);
    }
    EXPECT_EQ(result, check_class);
}
