//  Copyright 2026 Filippo Savi
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
#include <atomic>
#include <filesystem>
#include <string>
#include <unistd.h>

#include "ananke.hpp"

#include "test_paths.hpp"

// Create a unique, per-process test directory under the OS temp dir, so that
// concurrent test runs (or stale leftovers from a crashed run) never collide.
std::string unique_temp_dir(const std::string &tag) {
    static std::atomic<int> counter{0};
    auto dir = std::filesystem::temp_directory_path() /
               ("ananke_test_" + tag + "_" + std::to_string(::getpid()) + "_" + std::to_string(counter++));
    return dir.string();
}

// Replace every occurrence of `from` with `to` in `text` (literal, not regex).
std::string replace_base(std::string text, const std::string &from, const std::string &to) {
    size_t pos = 0;
    while ((pos = text.find(from, pos)) != std::string::npos) {
        text.replace(pos, from.size(), to);
        pos += to.size();
    }
    return text;
}

std::string e2e_settings_path = unique_temp_dir("cache");
std::string e2e_settings_file = e2e_settings_path + "/settings";

void e2e_setup_settings(bool include_auto_discovery = true) {
    std::filesystem::create_directories(e2e_settings_path);
    // Resolve any symlinks in the temp path (e.g. /tmp -> /private/tmp,
    // /var -> /private/var) so that paths recorded by the repository walker,
    // std::filesystem::current_path() and the expected strings all agree.
    e2e_settings_path = std::filesystem::canonical(e2e_settings_path).string();
    e2e_settings_file = e2e_settings_path + "/settings";
    std::ofstream ofs(e2e_settings_file);
    ofs << "{\"profiles\":{\"test_profile\":{\"hdl_store\":\""
        << e2e_settings_path
        << "\",\"include_auto_discovery\":" << (include_auto_discovery ? "true" : "false")
        << "}},\"amd_vivado_path\":\"/tmp/vivado\",\"lattice_radiant_path\":\"/tmp/lscc\", \"default_profile\":\"test_profile\"}";
    ofs.flush();
    ofs.close();
    std::error_code ec;
    std::filesystem::resize_file(e2e_settings_file, std::filesystem::file_size(e2e_settings_file, ec), ec);
}

void e2e_clean_settings() {
    std::filesystem::remove_all(e2e_settings_file);
    std::filesystem::remove_all(e2e_settings_path);
}

TEST( end_to_end , clear_cache) {
    e2e_setup_settings();

    ananke::CLI_opt opts;
    opts.clear_cache = true;
    opts.cache_dir = e2e_settings_path;
    std::ofstream ofs(opts.cache_dir + "/unified_cache");
    ofs << "test2";
    ofs.flush();

    const ananke uut(opts);
    auto rc = uut.clear_cache();
    EXPECT_EQ(rc, 0);

    EXPECT_FALSE(std::filesystem::exists(opts.cache_dir + "/unified_cache"));
    e2e_clean_settings();

}


TEST( end_to_end , new_sv_application) {

    e2e_setup_settings();

    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.cache_dir = e2e_settings_path;
    opts.new_app_name = "test_app";
    opts.new_app_lang = "sv";

    std::string test_dir = unique_temp_dir("app");
    auto wd = std::filesystem::current_path();
    std::filesystem::create_directory(test_dir);
    std::filesystem::current_path(test_dir);

    ananke uut(opts);
    auto rc = uut.generate_new_app();
    std::filesystem::current_path(wd);

    auto app_dir = test_dir + "/test_app";
    EXPECT_EQ(rc, 0);


    EXPECT_TRUE(std::filesystem::exists(app_dir));
    EXPECT_TRUE(std::filesystem::is_directory(app_dir));
    EXPECT_TRUE(std::filesystem::exists(app_dir + "/Depfile"));

    std::ifstream ifs(app_dir + "/Depfile");
    std::string result;
    ifs >> result;
    EXPECT_EQ(result, "{\"constraints\":[\"test_app.xdc\"],\"excluded_modules\":[],\"general\":{\"include_paths\":[\"/public/Components/Common\"],\"project_name\":\"test_app\",\"sim_modules\":[],\"sim_tl\":\"test_app_tb\",\"synth_modules\":[],\"synth_tl\":\"test_app\",\"target_part\":\"xc7z020clg400\"},\"scripts\":[]}");

    EXPECT_TRUE(std::filesystem::exists(app_dir + "/rtl"));
    EXPECT_TRUE(std::filesystem::is_directory(app_dir + "/rtl"));
    EXPECT_TRUE(std::filesystem::exists(app_dir + "/rtl/test_app.sv"));

    ifs = std::ifstream(app_dir + "/rtl/test_app.sv");
    std::stringstream ss;
    ss << ifs.rdbuf();
    EXPECT_EQ(ss.str(), "`timescale 10ns / 1ns\n\nmodule test_app (\n);\n\nendmodule");


    EXPECT_TRUE(std::filesystem::exists(app_dir + "/tb"));
    EXPECT_TRUE(std::filesystem::exists(app_dir + "/tb/test_app_tb.sv"));
    ss = std::stringstream();
    ifs = std::ifstream(app_dir + "/tb/test_app_tb.sv");
    ss << ifs.rdbuf();
    EXPECT_EQ(ss.str(), "`timescale 10ns / 1ns\n\nmodule test_app_tb ();\n\n\nendmodule");

    EXPECT_TRUE(std::filesystem::is_directory(app_dir + "/tb"));
    EXPECT_TRUE(std::filesystem::exists(app_dir + "/test_app.xdc"));


    std::filesystem::remove_all(app_dir);
    e2e_clean_settings();
}

TEST( end_to_end , directed_parsing ) {


    e2e_setup_settings();

    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.cache_dir = e2e_settings_path;
    opts.parse_targets = {td_file("check_files/test_data/Components/controls/PID/rtl/PID.sv")};


    ananke uut(opts);
    auto rc = uut.directed_parsing();

    std::unordered_map<std::string,std::string> expected =  {{"PID", td_file("check_files/test_data/Components/controls/PID/rtl/PID.sv")}};

    ASSERT_TRUE(rc);
    EXPECT_EQ(rc, expected);
    e2e_clean_settings();
}


TEST( end_to_end , directed_parsing_file_not_found) {
    e2e_setup_settings();

    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.cache_dir = e2e_settings_path;
    opts.parse_targets = {td_file("check_files/test_data/Components/controls/PID/rtl/PID.sv.error")};


    ananke uut(opts);
    auto rc = uut.directed_parsing();

    ASSERT_FALSE(rc);
    EXPECT_EQ(rc.error(), 50);
    e2e_clean_settings();
}


TEST( end_to_end , directed_parsing_preprocessor_error) {
    e2e_setup_settings();

    auto test_file = "/tmp/test_preproc.sv";
    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.cache_dir = e2e_settings_path;
    opts.parse_targets = {test_file};

    std::ofstream ofs(test_file);

    ofs << "`define ADD(a, b) a+b\nmodule test_module();\n integer a = `ADD();\nendmodule\n";
    ofs.close();

    ananke uut(opts);
    auto rc = uut.directed_parsing();

    ASSERT_FALSE(rc);
    EXPECT_EQ(rc.error(), 52);
    std::filesystem::remove_all(test_file);
    e2e_clean_settings();
}



TEST( end_to_end , sim_script_generation) {
    e2e_setup_settings();
    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.generate_xilinx = true;
    opts.generate_sim_script = true;

    opts.cache_dir = e2e_settings_path;
    std::filesystem::create_directory(opts.cache_dir);
    opts.cache_dir = std::filesystem::canonical(opts.cache_dir).string();
    auto test_dir = opts.cache_dir + "/PID";


    auto wd = std::filesystem::current_path();
    const auto copyOptions = std::filesystem::copy_options::recursive |
                             std::filesystem::copy_options::overwrite_existing;

    auto components = td_path("check_files/test_data/Components");
    std::filesystem::copy(components/"controls/PID", opts.cache_dir +"/PID", copyOptions);
    std::filesystem::remove_all(opts.cache_dir +"/PID/sim.tcl");
    std::filesystem::remove_all(opts.cache_dir +"/PID/sim.sh");
    EXPECT_FALSE(std::filesystem::exists(opts.cache_dir +"/PID/sim.tcl"));
    EXPECT_FALSE(std::filesystem::exists(opts.cache_dir +"/PID/sim.sh"));
    std::filesystem::copy(components/"Common", opts.cache_dir +"/Common", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/simple_register_cu",  opts.cache_dir +"/simple_register_cu", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/skid_buffer", opts.cache_dir +"/skid_buffer", copyOptions);
    std::filesystem::copy(components/"controls/integrator", opts.cache_dir +"/integrator", copyOptions);



    std::filesystem::current_path(test_dir);

    ananke uut(opts);
    auto cache_rc = uut.load_data_cache();
    ASSERT_FALSE(cache_rc.has_value());
    auto build_rc = uut.build_flow();
    ASSERT_FALSE(build_rc.has_value());

    EXPECT_TRUE(std::filesystem::exists(opts.cache_dir +"/PID/sim.tcl"));
    EXPECT_TRUE(std::filesystem::exists(opts.cache_dir +"/PID/sim.sh"));

    auto ifs = std::ifstream(opts.cache_dir +"/PID/sim.sh");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();

    std::string expected = replace_base("FILES=( \n    /tmp/ananke_test_cache/PID/rtl/PID.sv\n    /tmp/ananke_test_cache/integrator/rtl/Integrator.v\n    /tmp/ananke_test_cache/simple_register_cu/rtl/axil_simple_register_cu.sv\n    /tmp/ananke_test_cache/skid_buffer/rtl/axil_skid_buffer.sv\n    /tmp/vivado/data/verilog/src/glbl.v\n    /tmp/ananke_test_cache/Common/interfaces.sv\n    /tmp/ananke_test_cache/PID/tb/PID_tb.sv\n)\n\nmkdir -p /tmp/ananke_test_cache/PID/sim\ncp sim.tcl /tmp/ananke_test_cache/PID/sim/sim.tcl\n\n\n(\n    cd /tmp/ananke_test_cache/PID/sim|| exit\n\n    echo -e \"\\n\\033[1;33m>>> PHASE 1: XVLOG (Analysis) <<<\\033[0m\"\n    xvlog -sv \"${FILES[@]}\" -i /tmp/ananke_test_cache/public/Components/Common -i /tmp/ananke_test_cache/Common -i /tmp/vivado/data/rsb/busdef\n    if [ $? -ne 0 ]; then\n        echo -e \"\\033[1;31m!!! XVLOG FAILED !!!\\033[0m\"\n        exit 1\n    fi\n\n    echo -e \"\\n\\033[1;33m>>> PHASE 2: XELAB (Elaboration) <<<\\033[0m\"\n    xelab -debug typical --relax -top PID_tb -top glbl -L xil_defaultlib -L unisims_ver -L unimacro_ver -L xpm  -snapshot sim_snapshot  -timescale 10ns/1ps\n    if [ $? -ne 0 ]; then\n        echo -e \"\\033[1;31m!!! XELAB FAILED !!!\\033[0m\"\n        exit 1\n    fi\n\n    echo -e \"\\n\\033[1;33m>>> PHASE 3: XSIM (Simulation) <<<\\033[0m\"\n    xsim sim_snapshot -tclbatch sim.tcl\n    if [ $? -ne 0 ]; then\n        echo -e \"\\033[1;31m!!! XSIM FAILED !!!\\033[0m\"\n        exit 1\n    fi\n\n)\nif [ -f /tmp/ananke_test_cache/PID/sim/dump.vcd  ]; then\n    vcd2fst /tmp/ananke_test_cache/PID/sim/dump.vcd dump.fst\n    rm /tmp/ananke_test_cache/PID/sim/dump.vcd\nfi\nrm -r /tmp/ananke_test_cache/PID/sim\n",
        "/tmp/ananke_test_cache", opts.cache_dir);
    EXPECT_EQ(result, expected);

    ifs = std::ifstream(opts.cache_dir +"/PID/sim.tcl");
    ss = std::stringstream();
    ss << ifs.rdbuf();
    result = ss.str();
    EXPECT_EQ(result, "\nopen_vcd dump.vcd\n\nlog_vcd [get_objects -recursive /*]\n\nrun 2ms\n\nflush_vcd\nclose_vcd\nexit\n");

    std::filesystem::current_path(wd);
    e2e_clean_settings();
}



TEST( end_to_end , synth_script_generation) {
    e2e_setup_settings();

    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.generate_xilinx = true;
    opts.generate_synth_script = true;

    opts.cache_dir = e2e_settings_path;
    auto test_dir = opts.cache_dir + "/PID";


    auto wd = std::filesystem::current_path();
    const auto copyOptions = std::filesystem::copy_options::recursive |
                             std::filesystem::copy_options::overwrite_existing;

    auto components = td_path("check_files/test_data/Components");
    std::filesystem::copy(components/"controls/PID", opts.cache_dir +"/PID", copyOptions);
    std::filesystem::remove_all(opts.cache_dir +"/PID/synth.tcl");
    EXPECT_FALSE(std::filesystem::exists(opts.cache_dir +"/PID/synth.tcl"));
    std::filesystem::copy(components/"Common", opts.cache_dir +"/Common", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/simple_register_cu",  opts.cache_dir +"/simple_register_cu", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/skid_buffer", opts.cache_dir +"/skid_buffer", copyOptions);
    std::filesystem::copy(components/"controls/integrator", opts.cache_dir +"/integrator", copyOptions);



    std::filesystem::current_path(test_dir);

    ananke uut(opts);
    auto cache_rc = uut.load_data_cache();
    ASSERT_FALSE(cache_rc.has_value());
    auto build_rc = uut.build_flow();
    ASSERT_FALSE(build_rc.has_value());

    EXPECT_TRUE(std::filesystem::exists(opts.cache_dir +"/PID/synth.tcl"));


    auto ifs = std::ifstream(opts.cache_dir +"/PID/synth.tcl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();
    EXPECT_EQ(result, replace_base("set outputDir ./project_output\nfile mkdir $outputDir\ncd $outputDir\nset data_files_set {\n}\nset sources_set {\n\t/tmp/ananke_test_cache/PID/rtl/PID.sv\n\t/tmp/ananke_test_cache/integrator/rtl/Integrator.v\n\t/tmp/ananke_test_cache/simple_register_cu/rtl/axil_simple_register_cu.sv\n\t/tmp/ananke_test_cache/skid_buffer/rtl/axil_skid_buffer.sv\n}\nset inc_dirs {\n\t/tmp/ananke_test_cache/public/Components/Common\n\t/tmp/ananke_test_cache/Common\n}\nset constr_dirs {\n}\nforeach f $data_files_set {\n    file copy -force $f .\n}\nset_part xc7z020clg400\ngenerate_target all [get_files ps.bd]\nexport_ip_user_files -of_objects [get_files ps.bd] -no_script -force\nread_verilog $sources_set\nif {[llength $constr_dirs] > 0} { read_xdc $constr_dirs }\nsynth_design -top PID -part xc7z020clg400 -include_dirs $inc_dirs\nwrite_checkpoint -force $outputDir/post_synth.dcp\nopt_design\nplace_design\nwrite_checkpoint  -force $outputDir/post_place.dcp\nroute_design\nwrite_checkpoint  -force $outputDir/post_route.dcp\nwrite_bitstream -force $outputDir/top_module.bit\n", "/tmp/ananke_test_cache", opts.cache_dir));

    std::filesystem::current_path(wd);

    e2e_clean_settings();
}




TEST( end_to_end , vivado_project_generation) {
    e2e_setup_settings();
    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.generate_xilinx = true;
    opts.keep_makefile = true;
    opts.makefile_only = true;
    opts.no_open = true;

    opts.cache_dir = e2e_settings_path;
    auto test_dir = opts.cache_dir + "/PID";


    auto wd = std::filesystem::current_path();
    const auto copyOptions = std::filesystem::copy_options::recursive |
                             std::filesystem::copy_options::overwrite_existing;


    std::filesystem::create_directory("/tmp/vivado");

    auto components = td_path("check_files/test_data/Components");
    std::filesystem::copy(components/"controls/PID", opts.cache_dir +"/PID", copyOptions);
    std::filesystem::remove_all(opts.cache_dir +"/PID/makefile.tcl");
    EXPECT_FALSE(std::filesystem::exists(opts.cache_dir +"/PID/makefile.tcl"));
    std::filesystem::copy(components/"Common", opts.cache_dir +"/Common", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/simple_register_cu",  opts.cache_dir +"/simple_register_cu", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/skid_buffer", opts.cache_dir +"/skid_buffer", copyOptions);
    std::filesystem::copy(components/"controls/integrator", opts.cache_dir +"/integrator", copyOptions);



    std::filesystem::current_path(test_dir);

    ananke uut(opts);
    auto cache_rc = uut.load_data_cache();
    ASSERT_FALSE(cache_rc.has_value());
    auto build_rc = uut.build_flow();
    ASSERT_FALSE(build_rc.has_value());

    EXPECT_TRUE(std::filesystem::exists(opts.cache_dir +"/PID/makefile.tcl"));


    auto ifs = std::ifstream(opts.cache_dir +"/PID/makefile.tcl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();
    EXPECT_EQ(result, replace_base(
        "set project_name PID\nset origin_dir \".\"\nset base_dir /tmp/ananke_test_cache\nset commons_dir [list \"/tmp/ananke_test_cache/public/Components/Common\" \"/tmp/ananke_test_cache/Common\" ]\nset synth_sources [list \"${base_dir}/PID/rtl/PID.sv\" \"${base_dir}/integrator/rtl/Integrator.v\" \"${base_dir}/simple_register_cu/rtl/axil_simple_register_cu.sv\" \"${base_dir}/skid_buffer/rtl/axil_skid_buffer.sv\" ]\nset sim_sources [list \"${base_dir}/Common/interfaces.sv\" \"${base_dir}/PID/tb/PID_tb.sv\" ]\nset constraints_sources [list ]\n# Create project\ncreate_project ${project_name} ./${project_name}\nset_property part xc7z020clg400 [current_project]\n# Set the directory path for the new project\nset proj_dir [get_property directory [current_project]]\nset obj [current_project]\nadd_files -norecurse $synth_sources\nset_property top PID [get_filesets sources_1]\nset_property include_dirs $commons_dir [get_filesets sources_1]\nset_property SOURCE_SET sources_1 [get_filesets sim_1]\nadd_files -fileset sim_1 -norecurse $sim_sources\nset_property top PID_tb [get_filesets sim_1]\nupdate_compile_order\n",
        "/tmp/ananke_test_cache",
        opts.cache_dir
        )
    );

    std::filesystem::current_path(wd);
    std::filesystem::remove_all("/tmp/vivado");
    e2e_clean_settings();
}

TEST( end_to_end , lattice_project_generation) {
    e2e_setup_settings();
    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.generate_lattice = true;
    opts.keep_makefile = true;
    opts.makefile_only = true;
    opts.no_open = true;

    opts.cache_dir = e2e_settings_path;
    auto test_dir = opts.cache_dir + "/PID";


    auto wd = std::filesystem::current_path();
    const auto copyOptions = std::filesystem::copy_options::recursive |
                             std::filesystem::copy_options::overwrite_existing;


    std::filesystem::create_directory("/tmp/radiant");

    // Resolve fixtures through the build-tree test-data root (as the vivado
    // test does): depending on the launch working directory breaks standalone
    // runs and throws a bare filesystem error instead of a readable failure.
    auto components = td_path("check_files/test_data/Components");
    ASSERT_TRUE(std::filesystem::exists(components)) << "missing test fixtures: " << components;
    std::filesystem::copy(components/"controls/PID", opts.cache_dir +"/PID", copyOptions);
    std::filesystem::remove_all(opts.cache_dir +"/PID/makefile.tcl");
    EXPECT_FALSE(std::filesystem::exists(opts.cache_dir +"/PID/makefile.tcl"));
    std::filesystem::copy(components/"Common", opts.cache_dir +"/Common", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/simple_register_cu",  opts.cache_dir +"/simple_register_cu", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/skid_buffer", opts.cache_dir +"/skid_buffer", copyOptions);
    std::filesystem::copy(components/"controls/integrator", opts.cache_dir +"/integrator", copyOptions);



    std::filesystem::current_path(test_dir);

    ananke uut(opts);
    auto cache_rc = uut.load_data_cache();
    ASSERT_FALSE(cache_rc.has_value());
    auto build_rc = uut.build_flow();
    ASSERT_FALSE(build_rc.has_value());


    EXPECT_TRUE(std::filesystem::exists(opts.cache_dir +"/PID/makefile.tcl"));


    auto ifs = std::ifstream(opts.cache_dir +"/PID/makefile.tcl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();

    std::string expected_res  = replace_base("set build_dir \"build\"\nif {![file exists $build_dir]} {\n    file mkdir $build_dir\n}\nprj_create -name \"PID\" -impl \"impl1\" -dev \"xc7z020clg400\" -dir \"$build_dir\"\nprj_add_source /tmp/ananke_test_cache/PID/rtl/PID.sv\nprj_add_source /tmp/ananke_test_cache/integrator/rtl/Integrator.v\nprj_add_source /tmp/ananke_test_cache/simple_register_cu/rtl/axil_simple_register_cu.sv\nprj_add_source /tmp/ananke_test_cache/skid_buffer/rtl/axil_skid_buffer.sv\nprj_set_impl_opt -impl \"impl1\" {top} {PID}\nprj_set_impl_opt -impl \"impl1\" {VerilogStandard} {System Verilog}\nprj_set_impl_opt -impl \"impl1\" {include path} {/tmp/ananke_test_cache/public/Components/Common;/tmp/ananke_test_cache/Common}\nprj_save\nprj_close\n",
        "/tmp/ananke_test_cache", opts.cache_dir);

    EXPECT_EQ(result, expected_res);

    std::filesystem::current_path(wd);
    std::filesystem::remove_all("/tmp/radiant");
    e2e_clean_settings();
}

std::string setup_auto_discovery_repo() {
    std::filesystem::create_directories(e2e_settings_path + "/rtl");
    std::filesystem::create_directories(e2e_settings_path + "/Common");

    std::ofstream ofs(e2e_settings_path + "/rtl/top.sv");
    ofs << "`include \"defs.svh\"\n"
           "module top #(\n"
           "    parameter W = `BUS_WIDTH\n"
           ")(\n"
           "    input logic clock,\n"
           "    input logic reset,\n"
           "    output logic [`BUS_WIDTH-1:0] data\n"
           ");\n"
           "endmodule\n";
    ofs.close();

    ofs = std::ofstream(e2e_settings_path + "/Common/defs.svh");
    ofs << "`define BUS_WIDTH 8\n";
    ofs.close();

    ofs = std::ofstream(e2e_settings_path + "/Depfile");
    ofs << "{\"general\":{\"project_name\":\"auto_disc\",\"target_part\":\"xc7z020clg400\","
           "\"synth_tl\":\"top\",\"sim_tl\":\"top\",\"include_paths\":[]},\"scripts\":[]}";
    ofs.close();

    return e2e_settings_path;
}

// Expected synth.tcl for the auto-discovery repository: `discovered` toggles
// whether the top source and its discovered include directory are present.
std::string auto_disc_synth_script(const std::string &base, bool discovered) {
    std::string sources = discovered ? "\t" + base + "/rtl/top.sv\n" : "";
    std::string inc_dirs = discovered ? "\t" + base + "/Common\n" : "";
    return "set outputDir ./project_output\n"
           "file mkdir $outputDir\n"
           "cd $outputDir\n"
           "set data_files_set {\n"
           "}\n"
           "set sources_set {\n" + sources + "}\n"
           "set inc_dirs {\n" + inc_dirs + "}\n"
           "set constr_dirs {\n"
           "}\n"
           "foreach f $data_files_set {\n"
           "    file copy -force $f .\n"
           "}\n"
           "set_part xc7z020clg400\n"
           "generate_target all [get_files ps.bd]\n"
           "export_ip_user_files -of_objects [get_files ps.bd] -no_script -force\n"
           "read_verilog $sources_set\n"
           "if {[llength $constr_dirs] > 0} { read_xdc $constr_dirs }\n"
           "synth_design -top top -part xc7z020clg400 -include_dirs $inc_dirs\n"
           "write_checkpoint -force $outputDir/post_synth.dcp\n"
           "opt_design\n"
           "place_design\n"
           "write_checkpoint  -force $outputDir/post_place.dcp\n"
           "route_design\n"
           "write_checkpoint  -force $outputDir/post_route.dcp\n"
           "write_bitstream -force $outputDir/top_module.bit\n";
}

TEST( end_to_end , include_auto_discovery) {
    e2e_setup_settings();
    auto repo = setup_auto_discovery_repo();

    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.generate_xilinx = true;
    opts.generate_synth_script = true;
    opts.cache_dir = e2e_settings_path;

    auto wd = std::filesystem::current_path();
    std::filesystem::current_path(repo);

    ananke uut(opts);
    auto cache_rc = uut.load_data_cache();
    ASSERT_FALSE(cache_rc.has_value());
    auto build_rc = uut.build_flow();
    ASSERT_FALSE(build_rc.has_value());

    std::ifstream ifs("synth.tcl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();

    // The unique header must have been auto-discovered: the top module parses
    // (its BUS_WIDTH macro resolves) and its directory is emitted as an include
    // path for the toolchain.
    EXPECT_EQ(result, auto_disc_synth_script(repo, true));

    std::filesystem::current_path(wd);
    e2e_clean_settings();
}

TEST( end_to_end , include_auto_discovery_disabled) {
    e2e_setup_settings(false);
    auto repo = setup_auto_discovery_repo();

    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.generate_xilinx = true;
    opts.generate_synth_script = true;
    opts.cache_dir = e2e_settings_path;

    auto wd = std::filesystem::current_path();
    std::filesystem::current_path(repo);

    ananke uut(opts);
    auto cache_rc = uut.load_data_cache();
    ASSERT_FALSE(cache_rc.has_value());
    auto build_rc = uut.build_flow();
    ASSERT_FALSE(build_rc.has_value());

    std::ifstream ifs("synth.tcl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();

    // With the toggle off the walker never builds the index, the include stays
    // unresolved, the macro is undefined and the top module is not analyzed:
    // the synthesis sources and include dirs are empty.
    EXPECT_EQ(result, auto_disc_synth_script(repo, false));

    std::filesystem::current_path(wd);
    e2e_clean_settings();
}

// The discovered headers are cached per file, so a fully-cached second run
// (no re-parse) must still emit the discovered include directory.
TEST( end_to_end , include_auto_discovery_cached) {
    e2e_setup_settings();
    auto repo = setup_auto_discovery_repo();

    ananke::CLI_opt opts;
    opts.generate_xilinx = true;
    opts.generate_synth_script = true;
    opts.cache_dir = e2e_settings_path;

    auto wd = std::filesystem::current_path();
    std::filesystem::current_path(repo);

    // First run: parse everything, persist the discovered headers in the cache.
    {
        ananke uut(opts);
        ASSERT_FALSE(uut.load_data_cache().has_value());
        ASSERT_FALSE(uut.build_flow().has_value());
    }

    std::string expected = auto_disc_synth_script(repo, true);

    // Second run: nothing changed, everything is a cache hit (no re-parse).
    ananke uut(opts);
    ASSERT_FALSE(uut.load_data_cache().has_value());
    ASSERT_FALSE(uut.build_flow().has_value());

    std::ifstream ifs("synth.tcl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();
    // The discovered include directory survives the cache round-trip without
    // re-parsing the sources.
    EXPECT_EQ(result, expected);

    std::filesystem::current_path(wd);
    e2e_clean_settings();
}

// A cache-hit file whose (transitive) include file changed must be evicted and
// re-parsed, even though its own content hash is unchanged.
TEST( end_to_end , include_auto_discovery_invalidation) {
    e2e_setup_settings();
    auto repo = setup_auto_discovery_repo();

    ananke::CLI_opt opts;
    opts.generate_xilinx = true;
    opts.generate_synth_script = true;
    opts.cache_dir = e2e_settings_path;

    auto wd = std::filesystem::current_path();
    std::filesystem::current_path(repo);

    // First run: parse and cache the top module and its header.
    {
        ananke uut(opts);
        ASSERT_FALSE(uut.load_data_cache().has_value());
        ASSERT_FALSE(uut.build_flow().has_value());
    }
    {
        std::ifstream ifs("synth.tcl");
        std::stringstream ss;
        ss << ifs.rdbuf();
        EXPECT_EQ(ss.str(), auto_disc_synth_script(repo, true));
    }

    // Break the header: BUS_WIDTH becomes undefined, so the top module can no
    // longer parse. The top module's own hash is unchanged.
    std::ofstream ofs(e2e_settings_path + "/Common/defs.svh");
    ofs << "`undef BUS_WIDTH\n";
    ofs.close();

    // Second run: the top module is a cache hit, but its include changed, so it
    // must be invalidated and re-parsed -- and now fail to analyze.
    ananke uut(opts);
    ASSERT_FALSE(uut.load_data_cache().has_value());
    ASSERT_FALSE(uut.build_flow().has_value());

    std::ifstream ifs("synth.tcl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    EXPECT_EQ(ss.str(), auto_disc_synth_script(repo, false));

    std::filesystem::current_path(wd);
    e2e_clean_settings();
}