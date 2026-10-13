// Copyright 2021 University of Nottingham Ningbo China
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
#include <gmock/gmock.h>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "ananke.hpp"

#include "data_model/Constraints.hpp"
#include "data_model/Script.hpp"
#include "data_model/Depfile/Depfile.hpp"

#include "test_paths.hpp"


class DepfileTest : public ::testing::Test {
protected:
    void SetUp(const std::string &filename) {
        auto stream = std::ifstream(filename);
        file = new Depfile(stream);
    }

    virtual void TearDown() {
        delete file;
    }

    Depfile *file;
};


TEST_F( DepfileTest , general_section_parsing) {
    SetUp(td_file("Depfile"));

    ASSERT_EQ(file->general.project_name, "test_project_name");
    ASSERT_EQ(file->general.synth_tl, "test_synth_tl");
    ASSERT_EQ(file->general.sim_tl, "test_sim_tl");
    ASSERT_THAT(file->general.include_paths,
                testing::ElementsAre("test_include_1","test_include_2"));
    ASSERT_THAT(file->general.sim_modules, testing::ElementsAre("test_sim_module_add_1", "test_sim_module_add_2"));
    ASSERT_THAT(file->general.synth_modules, testing::ElementsAre("test_synth_module_add_1", "test_synth_module_add_2"));
    ASSERT_TRUE(file->general.sim_harness.empty());
    ASSERT_TRUE(file->general.sim_defines.empty());


}


TEST_F( DepfileTest , Depfile_excluded_modules) {
    SetUp(td_file("Depfile"));

    ASSERT_THAT(file->excluded_modules,
                testing::ElementsAre( "test_excluded_module_1", "test_excluded_module_2"));

}


TEST_F( DepfileTest , Depfile_constraints) {
    SetUp(td_file("Depfile"));
    std::vector<Constraints> correct_answer;
    correct_answer.emplace_back("test_constraints_1");
    correct_answer.emplace_back("test_constraints_2");
    ASSERT_THAT(file->constraints, testing::ContainerEq(correct_answer));
}


TEST_F( DepfileTest , Depfile_scripts) {
    SetUp(td_file("Depfile"));
    std::vector<Script> correct_answer;
    script_specs s;

    s.name = "test_script.tcl";
    s.type = "tcl";
    correct_answer.emplace_back(s);

    s.name = "test_script.py";
    s.type = "py";
    s.positional_arguments = {"B"};
    correct_answer.emplace_back(s);

    s.name = "test_script.py";
    s.type = "py";
    s.positional_arguments = {"A"};
    correct_answer.emplace_back(s);

    s.name = "test_script_args.tcl";
    s.type = "tcl";
    s.named_arguments.emplace_back("A", "1");
    s.named_arguments.emplace_back("B", "2");
    correct_answer.emplace_back(s);


    auto res = file->get_scripts();
    ASSERT_EQ(res.size(), correct_answer.size());
    for (int i = 0; i<res.size(); i++)
        EXPECT_EQ(res[i], correct_answer[i]);
}

TEST( DepfileHarnessTest , sim_harness_parsing) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "harness_proj",
            "synth_tl": "dut",
            "sim_tl": "tb_top",
            "sim_harness": ["testbench/test_tb_top.cpp", "/abs/harness.cpp"],
            "sim_defines": ["snapshots/default/common_defines.vh"]
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    ASSERT_THAT(f.general.sim_harness,
                testing::ElementsAre("testbench/test_tb_top.cpp", "/abs/harness.cpp"));
    ASSERT_THAT(f.general.sim_defines,
                testing::ElementsAre("snapshots/default/common_defines.vh"));
}

TEST(DepfileVerilatorTest, tool_section_parsing) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        },
        "verilator": {
            "cflags": ["-std=c++17", "-O2"],
            "waivers": [],
            "make_args": ["OPT_FAST=-O2", "VERBOSE=1"],
            "autoflush": false,
            "extra_args": ["--x-initial", "fast"]
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    ASSERT_THAT(f.verilator.cflags, testing::ElementsAre("-std=c++17", "-O2"));
    ASSERT_TRUE(f.verilator.waivers.empty());
    ASSERT_THAT(f.verilator.make_args, testing::ElementsAre("OPT_FAST=-O2", "VERBOSE=1"));
    ASSERT_FALSE(f.verilator.autoflush);
    ASSERT_THAT(f.verilator.extra_args, testing::ElementsAre("--x-initial", "fast"));
}

TEST(DepfileVerilatorTest, tool_section_defaults) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    ASSERT_THAT(f.verilator.cflags, testing::ElementsAre("-std=c++14"));
    ASSERT_THAT(f.verilator.waivers, testing::ElementsAre("UNOPTFLAT"));
    ASSERT_THAT(f.verilator.make_args, testing::ElementsAre("OPT_FAST=-Os"));
    ASSERT_TRUE(f.verilator.autoflush);
    ASSERT_TRUE(f.verilator.extra_args.empty());
}
TEST(DepfileIncludesTest, defaults_empty) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    EXPECT_TRUE(f.includes.ignored.empty());
    EXPECT_TRUE(f.includes.deconfliction.empty());
}

TEST(DepfileIncludesTest, ignored_and_mixed_deconfliction) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        },
        "includes": {
            "ignored": ["avery_defines.svh", "avery_vip.svh"],
            "deconfliction": {
                "shared.svh": "rtl/shared/shared.svh",
                "defs.svh": {
                    "rtl/foo": "rtl/foo/defs.svh",
                    "rtl/bar": "rtl/bar/defs.svh"
                }
            }
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    ASSERT_THAT(f.includes.ignored,
                testing::ElementsAre("avery_defines.svh", "avery_vip.svh"));
    ASSERT_TRUE(f.includes.deconfliction.contains("shared.svh"));
    EXPECT_EQ(f.includes.deconfliction.at("shared.svh").global_target,
              std::optional<std::string>("rtl/shared/shared.svh"));
    EXPECT_TRUE(f.includes.deconfliction.at("shared.svh").scoped_targets.empty());
    ASSERT_TRUE(f.includes.deconfliction.contains("defs.svh"));
    EXPECT_FALSE(f.includes.deconfliction.at("defs.svh").global_target.has_value());
    EXPECT_EQ(f.includes.deconfliction.at("defs.svh").scoped_targets.size(), 2u);
    EXPECT_EQ(f.includes.deconfliction.at("defs.svh").scoped_targets.at("rtl/foo"),
              "rtl/foo/defs.svh");
    EXPECT_EQ(f.includes.deconfliction.at("defs.svh").scoped_targets.at("rtl/bar"),
              "rtl/bar/defs.svh");
}

TEST(DepfileIncludesTest, invalid_shape_rejected) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        },
        "includes": {
            "ignored": "avery_defines.svh"
        }
    })");
    Depfile f(ss);
    EXPECT_TRUE(f.has_error());
}

TEST(DepfileIncludesTest, file_like_top_level_deconfliction_warns_not_applies) {
    // Top-level `deconfliction` is module-only: a file-like key must not
    // leak into include resolution (it warns pointing at `includes`).
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        },
        "deconfliction": {
            "x.svh": "rtl/x.svh",
            "dup_mod": "rtl/dup_mod.sv"
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    EXPECT_TRUE(f.includes.deconfliction.empty());
    ASSERT_TRUE(f.deconfliction.contains("x.svh"));
    EXPECT_EQ(f.deconfliction.at("x.svh"), "rtl/x.svh");
}

TEST(DepfileDefinesTest, defaults_empty) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    EXPECT_TRUE(f.defines.empty());
}

TEST(DepfileDefinesTest, dictionary_parsing) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        },
        "defines": {
            "CALIPTRA_PRIM_MODULE_PREFIX": "caliptra_prim_generic",
            "BOARD_ID": 42,
            "USE_FEATURE": true,
            "EMPTY_VAL": "",
            "BARE": null
        }
    })");
    Depfile f(ss);
    ASSERT_FALSE(f.has_error()) << f.get_error();
    EXPECT_EQ(f.defines.at("CALIPTRA_PRIM_MODULE_PREFIX"), "caliptra_prim_generic");
    EXPECT_EQ(f.defines.at("BOARD_ID"), "42");
    EXPECT_EQ(f.defines.at("USE_FEATURE"), "true");
    EXPECT_EQ(f.defines.at("EMPTY_VAL"), "");
    EXPECT_EQ(f.defines.at("BARE"), "1");
}

TEST(DepfileDefinesTest, invalid_shape_rejected) {
    std::stringstream ss(R"({
        "general": {
            "project_name": "vproj",
            "synth_tl": "dut",
            "sim_tl": "tb"
        },
        "defines": ["NOT_A_DICT"]
    })");
    Depfile f(ss);
    EXPECT_TRUE(f.has_error());
}

namespace {
std::string dep_abs(const std::filesystem::path &p) {
    return std::filesystem::absolute(p).lexically_normal().string();
}
} // namespace

TEST(DepfileDiscoveryTest, precedence_explicit_cwd_hdl_store) {
    const std::filesystem::path root = "/tmp/ananke_dep_disc";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root / "cwd");
    std::filesystem::create_directories(root / "hdl");
    {
        std::ofstream ofs(root / "cwd" / "Depfile");
        ofs << "{}";
    }
    {
        std::ofstream ofs(root / "hdl" / "Depfile");
        ofs << "{}";
    }
    {
        std::ofstream ofs(root / "explicit_dep");
        ofs << "{}";
    }
    {
        std::ofstream ofs(root / "cwd" / "rel_dep");
        ofs << "{}";
    }

    // Explicit absolute path wins over everything.
    EXPECT_EQ(ananke::resolve_depfile_candidate((root / "explicit_dep").string(), root / "cwd", root / "hdl"),
              dep_abs(root / "explicit_dep"));
    // Relative explicit paths resolve against the cwd.
    EXPECT_EQ(ananke::resolve_depfile_candidate("rel_dep", root / "cwd", root / "hdl"),
              dep_abs(root / "cwd" / "rel_dep"));
    // Missing explicit falls through to ./Depfile.
    EXPECT_EQ(ananke::resolve_depfile_candidate((root / "missing").string(), root / "cwd", root / "hdl"),
              dep_abs(root / "cwd" / "Depfile"));
    // ./Depfile wins over <hdl_store>/Depfile.
    EXPECT_EQ(ananke::resolve_depfile_candidate("", root / "cwd", root / "hdl"),
              dep_abs(root / "cwd" / "Depfile"));
    // hdl_store fallback when cwd has none.
    EXPECT_EQ(ananke::resolve_depfile_candidate("", root / "nowhere", root / "hdl"),
              dep_abs(root / "hdl" / "Depfile"));
    // Nothing anywhere.
    EXPECT_FALSE(ananke::resolve_depfile_candidate("", root / "nowhere", root / "alsonowhere").has_value());
    EXPECT_FALSE(ananke::resolve_depfile_candidate("", root / "nowhere", std::nullopt).has_value());

    std::filesystem::remove_all(root);
}
