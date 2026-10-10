// Copyright 2026 Filippo Savi
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
#include "Backend/Verilator/verilator_project_generator.hpp"

static const std::string vpg_settings_path = "/tmp/test_verilator_settings_store";
static const std::string vpg_settings_file = vpg_settings_path + "/settings";

std::shared_ptr<settings_store> vpg_setup_settings(const std::string &tool_path = "/usr/bin/verilator") {
    std::filesystem::create_directories(vpg_settings_path);
    std::ofstream ofs(vpg_settings_file);
    ofs << R"({"verilator_path":")" << tool_path << R"(","profiles": {"test_profile": {"hdl_store":"/tmp/rb", "defines": ["FOO", "BAR=1"]}}})";
    ofs.flush();
    ofs.close();
    std::error_code ec;
    std::filesystem::resize_file(vpg_settings_file, std::filesystem::file_size(vpg_settings_file, ec), ec);
    return std::make_shared<settings_store>(false, vpg_settings_path, "test_profile");
}

void vpg_clean_settings() {
    std::filesystem::remove_all(vpg_settings_file);
    std::filesystem::remove_all(vpg_settings_path);
}

static project_data vpg_base_data() {
    project_data d;
    d.name = "test_proj";
    d.synth_sources = {"/test/synth/dut.sv"};
    d.package_synth_sources = {"/test/synth/pkg.sv"};
    d.sim_sources = {"/test/sim/tb_top.sv", "/test/sim/test_tb_top.cpp"};
    d.package_sim_sources = {"/test/sim/sim_pkg.sv"};
    d.tb_tl = "tb_top";
    d.synth_tl = "dut";
    d.commons_dir = {"/include"};
    d.repo_dir = "/tmp/tb";
    return d;
}

TEST(verilator_project_gen, harness_flow) {
    auto s_store = vpg_setup_settings();
    verilator_project_generator gen(s_store);

    project_data d = vpg_base_data();
    // Xilinx-only helper and non-unit files must not reach the command line.
    d.synth_sources.insert("/test/synth/glbl.v");
    d.package_synth_sources.insert("/test/synth/header.vh");
    d.synth_sources.insert("/test/synth/rtl.vhd");
    gen.set_data(d);

    std::ostringstream out;
    gen.generate_sim_script(out);
    auto script = out.str();

    EXPECT_NE(script.find("--cc"), std::string::npos);
    EXPECT_NE(script.find("--exe"), std::string::npos);
    EXPECT_NE(script.find("test_tb_top.cpp"), std::string::npos);
    EXPECT_NE(script.find("--top-module"), std::string::npos);
    EXPECT_NE(script.find("tb_top"), std::string::npos);
    // Verilator predefines VERILATOR itself; passing it trips REDEFMACRO.
    EXPECT_EQ(script.find("+define+VERILATOR"), std::string::npos);
    EXPECT_NE(script.find("+define+FOO"), std::string::npos);
    EXPECT_NE(script.find("+define+BAR=1"), std::string::npos);
    EXPECT_NE(script.find("+incdir+/tmp/rb/include"), std::string::npos);
    EXPECT_NE(script.find("dut.sv"), std::string::npos);
    EXPECT_NE(script.find("-Wno-UNOPTFLAT"), std::string::npos);
    EXPECT_NE(script.find("V${TOP}.mk"), std::string::npos);
    EXPECT_EQ(script.find("glbl.v"), std::string::npos);
    EXPECT_EQ(script.find("header.vh"), std::string::npos);
    EXPECT_EQ(script.find("rtl.vhd"), std::string::npos);
    EXPECT_EQ(script.find("--binary"), std::string::npos);
    vpg_clean_settings();
}

TEST(verilator_project_gen, binary_fallback_without_harness) {
    auto s_store = vpg_setup_settings();
    verilator_project_generator gen(s_store);

    project_data d = vpg_base_data();
    d.sim_sources = {"/test/sim/tb_top.sv"};
    gen.set_data(d);

    std::ostringstream out;
    gen.generate_sim_script(out);
    auto script = out.str();

    EXPECT_NE(script.find("--binary"), std::string::npos);
    EXPECT_NE(script.find("--timing"), std::string::npos);
    EXPECT_EQ(script.find("--exe"), std::string::npos);
    EXPECT_EQ(script.find("--cc"), std::string::npos);
    vpg_clean_settings();
}

TEST(verilator_project_gen, verilator_path_override) {
    {
        auto s_store = vpg_setup_settings("/opt/verilator/bin/verilator");
        verilator_project_generator gen(s_store);
        project_data d = vpg_base_data();
        gen.set_data(d);
        std::ostringstream out;
        gen.generate_sim_script(out);
        EXPECT_NE(out.str().find("/opt/verilator/bin/verilator"), std::string::npos);
        vpg_clean_settings();
    }
    {
        // No verilator_path configured: fall back to PATH resolution.
        std::filesystem::create_directories(vpg_settings_path);
        std::ofstream ofs(vpg_settings_file);
        ofs << R"({"profiles": {"test_profile": {"hdl_store":"/tmp/rb"}}})";
        ofs.flush();
        ofs.close();
        auto s_store = std::make_shared<settings_store>(false, vpg_settings_path, "test_profile");
        verilator_project_generator gen(s_store);
        project_data d = vpg_base_data();
        gen.set_data(d);
        std::ostringstream out;
        gen.generate_sim_script(out);
        EXPECT_NE(out.str().find("${VERILATOR:-\"verilator\"}"), std::string::npos);
        vpg_clean_settings();
    }
}

TEST(verilator_project_gen, write_makefile_emits_sim_script) {
    auto s_store = vpg_setup_settings();
    verilator_project_generator gen(s_store);

    project_data d = vpg_base_data();
    gen.set_data(d);

    std::ostringstream mk, sim;
    gen.write_makefile(mk);
    gen.generate_sim_script(sim);
    EXPECT_EQ(mk.str(), sim.str());
    vpg_clean_settings();
}

TEST(verilator_project_gen, explicit_harness_from_depfile) {
    auto s_store = vpg_setup_settings();
    verilator_project_generator gen(s_store);

    project_data d = vpg_base_data();
    d.sim_sources = {"/test/sim/tb_top.sv", "/test/sim/ignored.cpp"};
    d.package_sim_sources = {};
    // Absolute entries verbatim, relative entries against the repo base.
    d.sim_harness = {"/test/sim/custom_main.cpp", "rel/other.cpp"};
    gen.set_data(d);

    std::ostringstream out;
    gen.generate_sim_script(out);
    auto script = out.str();

    EXPECT_NE(script.find("--cc"), std::string::npos);
    EXPECT_NE(script.find("--exe"), std::string::npos);
    EXPECT_NE(script.find("custom_main.cpp"), std::string::npos);
    EXPECT_NE(script.find("/tmp/rb/rel/other.cpp"), std::string::npos);
    // Explicit option is authoritative: closure .cpp stays out, no fallback.
    EXPECT_EQ(script.find("ignored.cpp"), std::string::npos);
    EXPECT_EQ(script.find("--binary"), std::string::npos);
    vpg_clean_settings();
}

TEST(verilator_project_gen, define_headers_first) {
    auto s_store = vpg_setup_settings();
    verilator_project_generator gen(s_store);

    project_data d = vpg_base_data();
    d.sim_sources = {"/test/sim/tb_top.sv"};
    d.package_sim_sources = {};
    d.sim_harness = {};
    d.header_units = {"/test/defs/common.vh", "/test/sim/auto.vh"};
    gen.set_data(d);

    std::ostringstream out;
    gen.generate_sim_script(out);
    auto script = out.str();

    auto pos_common = script.find("/test/defs/common.vh");
    auto pos_auto = script.find("auto.vh");
    auto pos_dut = script.find("dut.sv");
    EXPECT_NE(pos_common, std::string::npos);
    EXPECT_NE(pos_auto, std::string::npos);
    // Leading units precede all regular units, in flow-resolved order.
    EXPECT_LT(pos_common, pos_auto);
    EXPECT_LT(pos_auto, pos_dut);
    vpg_clean_settings();
}

TEST(verilator_project_gen, posix_source_safe_script) {
    auto s_store = vpg_setup_settings();
    verilator_project_generator gen(s_store);

    project_data d = vpg_base_data();
    gen.set_data(d);

    std::ostringstream out;
    gen.generate_sim_script(out);
    auto script = out.str();

    // POSIX-only: no bashisms, portable phase banners.
    EXPECT_EQ(script.find("echo -e"), std::string::npos);
    EXPECT_EQ(script.find("\nset -e\n"), std::string::npos);
    EXPECT_EQ(script.find("[["), std::string::npos);
    EXPECT_NE(script.find("printf '"), std::string::npos);
    // No global `set -e` (it leaks into sourcing shells); every fallible
    // command carries an inline source-safe guard instead.
    EXPECT_NE(script.find("return 1 2>/dev/null || exit 1"), std::string::npos);
    EXPECT_NE(script.find("mkdir -p \"$OBJ_DIR\" \"$RUN_DIR\" || { sim_err"), std::string::npos);
    EXPECT_NE(script.find("--Mdir \"$OBJ_DIR\" || { sim_err"), std::string::npos);
    vpg_clean_settings();
}

TEST(verilator_project_gen, explicit_sim_defines_override) {
    auto s_store = vpg_setup_settings();
    verilator_project_generator gen(s_store);

    project_data d = vpg_base_data();
    d.sim_sources = {"/test/sim/tb_top.sv"};
    d.package_sim_sources = {};
    d.sim_harness = {};
    // Explicit list wins over flow-resolved units entirely, and files that
    // are also closure units are emitted exactly once, leading.
    d.sim_defines = {"/test/defs/forced.vh", "/test/synth/dut.sv"};
    d.header_units = {"/test/sim/auto.vh"};
    gen.set_data(d);

    std::ostringstream out;
    gen.generate_sim_script(out);
    auto script = out.str();

    auto pos_forced = script.find("/test/defs/forced.vh");
    auto pos_dut = script.find("dut.sv");
    EXPECT_NE(pos_forced, std::string::npos);
    EXPECT_NE(pos_dut, std::string::npos);
    EXPECT_LT(pos_forced, pos_dut);
    EXPECT_EQ(script.find("auto.vh"), std::string::npos);
    // dut.sv appears exactly once despite being a synth unit too.
    size_t count = 0, at = 0;
    while ((at = script.find("dut.sv", at)) != std::string::npos) { ++count; ++at; }
    EXPECT_EQ(count, 1u);
    vpg_clean_settings();
}

static stored_macro_def vpg_def(const std::string &name, const std::string &value) {
    stored_macro_def d;
    d.name = name;
    d.is_function = false;
    d.value = value;
    return d;
}

TEST(verilator_project_gen, resolve_leading_units_from_macro_table) {
    // VeeR miniature: common_defines.vh defines RV_X plus TEC, which
    // pd_defines.vh also defines with a different body (genuine conflict).
    // veer_types.sv consumes RV_X, TEC, its own guard and a profile global.
    std::unordered_map<std::string, std::vector<stored_macro_def>> defs = {
        {"/repo/snapshots/common_defines.vh", {vpg_def("RV_X", "8"), vpg_def("TEC", "clockhdr")}},
        {"/repo/snapshots/pd_defines.vh", {vpg_def("TEC", "CKLNQD12")}},
        {"/repo/design/veer_types.sv", {vpg_def("VT_GUARD", "")}},
        {"/repo/tb/tb_top.sv", {}},
    };
    std::unordered_map<std::string, std::vector<std::string>> needs = {
        {"/repo/design/veer_types.sv", {"RV_X", "TEC", "VT_GUARD", "G"}},
        {"/repo/tb/tb_top.sv", {"RV_X"}},
    };
    std::unordered_map<std::string, std::vector<std::string>> includes = {
        {"/repo/tb/tb_top.sv", {"/repo/tb/dasm.svi"}},
    };
    std::set<std::string> closure = {"/repo/design/veer_types.sv", "/repo/tb/tb_top.sv"};
    std::set<std::string> profile = {"G"};

    auto leading = verilator_project_generator::resolve_leading_units(defs, needs, includes, closure, profile);

    // Only common_defines.vh: TEC is conflicted, the guard is self-satisfied,
    // the profile global needs no file.
    ASSERT_EQ(leading.size(), 1u);
    EXPECT_EQ(leading[0], "/repo/snapshots/common_defines.vh");
}

TEST(verilator_project_gen, resolve_leading_units_order_and_skips) {    std::unordered_map<std::string, std::vector<stored_macro_def>> defs = {
        {"/repo/defs/b.vh", {vpg_def("M_B", "1")}},
        {"/repo/defs/a.vh", {vpg_def("M_A", "2")}},
        {"/repo/defs/hdr.vh", {vpg_def("M_H", "3")}},
    };
    std::unordered_map<std::string, std::vector<std::string>> needs = {
        {"/repo/rtl/user.sv", {"M_B", "M_A", "M_H"}},
    };
    // hdr.vh is `included by the closure: +incdir covers it, no leading slot.
    // mod.vh is in the closure and never included: leads as a closure header.
    std::unordered_map<std::string, std::vector<std::string>> includes = {
        {"/repo/rtl/user.sv", {"/repo/defs/hdr.vh"}},
    };
    std::set<std::string> closure = {"/repo/rtl/user.sv", "/repo/rtl/mod.vh"};

    auto leading = verilator_project_generator::resolve_leading_units(
        defs, needs, includes, closure, {});

    ASSERT_EQ(leading.size(), 3u);
    EXPECT_EQ(leading[0], "/repo/defs/a.vh");
    EXPECT_EQ(leading[1], "/repo/defs/b.vh");
    EXPECT_EQ(leading[2], "/repo/rtl/mod.vh");
}

TEST(verilator_project_gen, extra_include_dirs_covers_includers) {
    // VeeR shape: tb_top.sv includes dasm.svi relatively; testbench/ must
    // end up on +incdir even though no auto-discovered edge points there.
    std::set<std::string> closure = {
        "/repo/design/veer.sv",
        "/repo/design/include/veer_types.sv",
        "/repo/testbench/tb_top.sv",
    };
    std::vector<std::string> existing = {"design/include", "/repo_snapshots"};
    auto extra = verilator_project_generator::extra_include_dirs("/repo", closure, existing);

    // design/include is already covered (both path conventions match);
    // design/ and testbench/ are added, sorted and deterministic.
    ASSERT_EQ(extra.size(), 2u);
    EXPECT_EQ(extra[0], "/design");
    EXPECT_EQ(extra[1], "/testbench");
}

TEST(verilator_project_gen, extra_include_dirs_skips_covered_and_outside) {
    std::set<std::string> closure = {
        "/repo/design/a.sv",
        "/other/tree/b.sv",
        "/repo/top.sv",
    };
    // design/ already covered (both conventions); /other is outside the repo.
    auto extra = verilator_project_generator::extra_include_dirs(
        "/repo", closure, {"/design"});

    ASSERT_EQ(extra.size(), 1u);
    EXPECT_EQ(extra[0], "/");
}
