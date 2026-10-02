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
#include "data_model/data_store.hpp"
#include "data_model/HDL/statement/hdl_class_statement.hpp"
#include "data_model/HDL/statement/hdl_package_statement.hpp"
#include "data_model/HDL/statement/hdl_resource_statement.hpp"

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
    check.set_language(hdl_language::system_verilog);
    check.set_line_n(line_n);
    return check;
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
    ASSERT_EQ(*classes[0], check_class);
    EXPECT_TRUE(classes[0]->get_dependencies().empty());
}

TEST(sv_class_tracking, class_with_extends) {
    auto test_pattern = R"(
        class my_base;
        endclass
        class my_child extends my_base;
        endclass
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 2);

    hdl_class_statement check_base = make_check_class("my_base", 2);
    hdl_class_statement check_child = make_check_class("my_child", 4);
    ASSERT_EQ(*classes[0], check_base);
    ASSERT_EQ(*classes[1], check_child);
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

    hdl_class_statement check_base = make_check_class("vbase", 2);
    hdl_class_statement check_child = make_check_class("param_c", 4);
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
    ASSERT_EQ(classes.size(), 2);

    // Inner declaration closes first.
    hdl_class_statement check_inner = make_check_class("nested_inner", 3);
    hdl_class_statement check_outer = make_check_class("outer", 2);
    ASSERT_EQ(*classes[0], check_inner);
    ASSERT_EQ(*classes[1], check_outer);
}

TEST(sv_class_tracking, class_in_package) {
    auto test_pattern = R"(
        package my_pkg;
        endpackage
        class pkg_class;
        endclass
        module m;
        endmodule
    )";

    sv_analyzer analyzer;
    auto result = analyzer.analyze("test.sv", test_pattern);
    ASSERT_TRUE(result.has_value());
    auto classes = get_classes(result.value());
    ASSERT_EQ(classes.size(), 1);

    hdl_class_statement check_class = make_check_class("pkg_class", 4);
    ASSERT_EQ(*classes[0], check_class);

    // Sibling definitions are unaffected.
    hdl_package_statement check_pkg;
    check_pkg.set_name("my_pkg");
    check_pkg.set_language(hdl_language::system_verilog);
    check_pkg.set_line_n(2);
    hdl_resource_statement check_mod;
    check_mod.set_name("m");
    check_mod.set_language(hdl_language::system_verilog);
    check_mod.set_line_n(6);

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

TEST(sv_class_tracking, serialize_round_trip) {
    hdl_class_statement check_class = make_check_class("my_class", 42);

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
