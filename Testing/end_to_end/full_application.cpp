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

#include "ananke.hpp"

std::string e2e_settings_path = "/tmp/ananke_test_cache";
auto e2e_settings_file = e2e_settings_path + "/settings";

void e2e_setup_settings() {
    std::filesystem::create_directories(e2e_settings_path);
    std::ofstream ofs(e2e_settings_file);
    ofs << "{\"hdl_store\":\"/tmp/ananke_test_cache\",\"amd_vivado_path\":\"/tmp/vivado\"}";
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

    std::string test_dir = "/tmp/test_generate_app";
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
    opts.parse_targets = {"check_files/test_data/Components/controls/PID/rtl/PID.sv"};


    ananke uut(opts);
    auto rc = uut.directed_parsing();

    std::unordered_map<std::string,std::string> expected =  {{"PID", "check_files/test_data/Components/controls/PID/rtl/PID.sv"}};

    ASSERT_TRUE(rc);
    EXPECT_EQ(rc, expected);
    e2e_clean_settings();
}


TEST( end_to_end , directed_parsing_file_not_found) {
    e2e_setup_settings();

    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.cache_dir = e2e_settings_path;
    opts.parse_targets = {"check_files/test_data/Components/controls/PID/rtl/PID.sv.error"};


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
    EXPECT_EQ(rc.error(), 51);
    std::filesystem::remove_all(test_file);
    e2e_clean_settings();
}



TEST( end_to_end , sim_script_generation) {
    e2e_setup_settings();
    ananke::CLI_opt opts;
    opts.no_cache = true;
    opts.generate_xilinx = true;
    opts.generate_sim_script = true;

    opts.cache_dir = "/tmp/ananke_test_cache";
    std::filesystem::create_directory(opts.cache_dir);
    opts.cache_dir = std::filesystem::canonical(opts.cache_dir).string();
    auto test_dir = opts.cache_dir + "/PID";


    auto wd = std::filesystem::current_path();
    const auto copyOptions = std::filesystem::copy_options::recursive |
                             std::filesystem::copy_options::overwrite_existing;

    auto components = wd / "check_files/test_data/Components";
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
    uut.load_data_cache();
    uut.build_flow();

    EXPECT_TRUE(std::filesystem::exists(opts.cache_dir +"/PID/sim.tcl"));
    EXPECT_TRUE(std::filesystem::exists(opts.cache_dir +"/PID/sim.sh"));

    auto ifs = std::ifstream(opts.cache_dir +"/PID/sim.sh");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();

    std::string const placeholder = "/tmp/ananke_test_cache/PID/sim";
    std::string replacement = std::filesystem::canonical("/tmp/ananke_test_cache").string() + "/PID/sim";
    std::string expected = "FILES=( \n    /tmp/ananke_test_cache/PID/rtl/PID.sv\n    /tmp/ananke_test_cache/integrator/rtl/Integrator.v\n    /tmp/ananke_test_cache/simple_register_cu/rtl/axil_simple_register_cu.sv\n    /tmp/ananke_test_cache/skid_buffer/rtl/axil_skid_buffer.sv\n    /tmp/vivado/data/verilog/src/glbl.v\n    /tmp/ananke_test_cache/Common/interfaces.sv\n    /tmp/ananke_test_cache/PID/tb/PID_tb.sv\n)\n\nmkdir -p /tmp/ananke_test_cache/PID/sim\ncp sim.tcl /tmp/ananke_test_cache/PID/sim/sim.tcl\n\n\n(\n    cd /tmp/ananke_test_cache/PID/sim|| exit\n\n    echo -e \"\\n\\033[1;33m>>> PHASE 1: XVLOG (Analysis) <<<\\033[0m\"\n    xvlog -sv \"${FILES[@]}\" -i /tmp/ananke_test_cache/public/Components/Common -i /tmp/vivado/data/rsb/busdef\n    if [ $? -ne 0 ]; then\n        echo -e \"\\033[1;31m!!! XVLOG FAILED !!!\\033[0m\"\n        exit 1\n    fi\n\n    echo -e \"\\n\\033[1;33m>>> PHASE 2: XELAB (Elaboration) <<<\\033[0m\"\n    xelab -debug typical --relax -top PID_tb -top glbl -L xil_defaultlib -L unisims_ver -L unimacro_ver -L xpm  -snapshot sim_snapshot  -timescale 10ns/1ps\n    if [ $? -ne 0 ]; then\n        echo -e \"\\033[1;31m!!! XELAB FAILED !!!\\033[0m\"\n        exit 1\n    fi\n\n    echo -e \"\\n\\033[1;33m>>> PHASE 3: XSIM (Simulation) <<<\\033[0m\"\n    xsim sim_snapshot -tclbatch sim.tcl\n    if [ $? -ne 0 ]; then\n        echo -e \"\\033[1;31m!!! XSIM FAILED !!!\\033[0m\"\n        exit 1\n    fi\n\n)\nif [ -f /tmp/ananke_test_cache/PID/sim/dump.vcd  ]; then\n    vcd2fst /tmp/ananke_test_cache/PID/sim/dump.vcd dump.fst\n    rm /tmp/ananke_test_cache/PID/sim/dump.vcd\nfi\nrm -r /tmp/ananke_test_cache/PID/sim\n";
    for (size_t pos = 0; (pos = expected.find(placeholder, pos)) != std::string::npos; pos += opts.cache_dir.size()) {
        expected.replace(pos, placeholder.size(), replacement);
    }
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

    opts.cache_dir = "/tmp/ananke_test_cache";
    auto test_dir = opts.cache_dir + "/PID";


    auto wd = std::filesystem::current_path();
    const auto copyOptions = std::filesystem::copy_options::recursive |
                             std::filesystem::copy_options::overwrite_existing;

    auto components = wd / "check_files/test_data/Components";
    std::filesystem::copy(components/"controls/PID", opts.cache_dir +"/PID", copyOptions);
    std::filesystem::remove_all(opts.cache_dir +"/PID/synth.tcl");
    EXPECT_FALSE(std::filesystem::exists(opts.cache_dir +"/PID/synth.tcl"));
    std::filesystem::copy(components/"Common", opts.cache_dir +"/Common", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/simple_register_cu",  opts.cache_dir +"/simple_register_cu", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/skid_buffer", opts.cache_dir +"/skid_buffer", copyOptions);
    std::filesystem::copy(components/"controls/integrator", opts.cache_dir +"/integrator", copyOptions);



    std::filesystem::current_path(test_dir);

    ananke uut(opts);
    uut.load_data_cache();
    uut.build_flow();

    EXPECT_TRUE(std::filesystem::exists(opts.cache_dir +"/PID/synth.tcl"));


    auto ifs = std::ifstream(opts.cache_dir +"/PID/synth.tcl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();
    EXPECT_EQ(result, "set outputDir ./project_output\nfile mkdir $outputDir\ncd $outputDir\nset data_files_set {\n}\nset sources_set {\n\t/tmp/ananke_test_cache/PID/rtl/PID.sv\n\t/tmp/ananke_test_cache/integrator/rtl/Integrator.v\n\t/tmp/ananke_test_cache/simple_register_cu/rtl/axil_simple_register_cu.sv\n\t/tmp/ananke_test_cache/skid_buffer/rtl/axil_skid_buffer.sv\n}\nset inc_dirs {\n\t/tmp/ananke_test_cache/public/Components/Common\n}\nset constr_dirs {\n}\nforeach f $data_files_set {\n    file copy -force $f .\n}\nset_part xc7z020clg400\ngenerate_target all [get_files ps.bd]\nexport_ip_user_files -of_objects [get_files ps.bd] -no_script -force\nread_verilog $sources_set\nif {[llength $constr_dirs] > 0} { read_xdc $constr_dirs }\nsynth_design -top PID -part xc7z020clg400 -include_dirs $inc_dirs\nwrite_checkpoint -force $outputDir/post_synth.dcp\nopt_design\nplace_design\nwrite_checkpoint  -force $outputDir/post_place.dcp\nroute_design\nwrite_checkpoint  -force $outputDir/post_route.dcp\nwrite_bitstream -force $outputDir/top_module.bit\n");

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

    opts.cache_dir = "/tmp/ananke_test_cache";
    auto test_dir = opts.cache_dir + "/PID";


    auto wd = std::filesystem::current_path();
    const auto copyOptions = std::filesystem::copy_options::recursive |
                             std::filesystem::copy_options::overwrite_existing;


    std::filesystem::create_directory("/tmp/vivado");

    auto components = wd / "check_files/test_data/Components";
    std::filesystem::copy(components/"controls/PID", opts.cache_dir +"/PID", copyOptions);
    std::filesystem::remove_all(opts.cache_dir +"/PID/makefile.tcl");
    EXPECT_FALSE(std::filesystem::exists(opts.cache_dir +"/PID/makefile.tcl"));
    std::filesystem::copy(components/"Common", opts.cache_dir +"/Common", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/simple_register_cu",  opts.cache_dir +"/simple_register_cu", copyOptions);
    std::filesystem::copy(components/"system/axi_lite/skid_buffer", opts.cache_dir +"/skid_buffer", copyOptions);
    std::filesystem::copy(components/"controls/integrator", opts.cache_dir +"/integrator", copyOptions);



    std::filesystem::current_path(test_dir);

    ananke uut(opts);
    uut.load_data_cache();
    uut.build_flow();

    EXPECT_TRUE(std::filesystem::exists(opts.cache_dir +"/PID/makefile.tcl"));


    auto ifs = std::ifstream(opts.cache_dir +"/PID/makefile.tcl");
    std::stringstream ss;
    ss << ifs.rdbuf();
    std::string result = ss.str();
    EXPECT_EQ(result, "set project_name PID\nset origin_dir \".\"\nset base_dir /tmp/ananke_test_cache\nset commons_dir [list \"/tmp/ananke_test_cache/public/Components/Common\" ]\nset synth_sources [list \"${base_dir}/PID/rtl/PID.sv\" \"${base_dir}/integrator/rtl/Integrator.v\" \"${base_dir}/simple_register_cu/rtl/axil_simple_register_cu.sv\" \"${base_dir}/skid_buffer/rtl/axil_skid_buffer.sv\" ]\nset sim_sources [list \"${base_dir}/Common/interfaces.sv\" \"${base_dir}/PID/tb/PID_tb.sv\" ]\nset constraints_sources [list ]\n# Create project\ncreate_project ${project_name} ./${project_name}\nset_property part xc7z020clg400-1 [current_project]\n# Set the directory path for the new project\nset proj_dir [get_property directory [current_project]]\nset obj [current_project]\nadd_files -norecurse $synth_sources\nset_property top PID [get_filesets sources_1]\nset_property include_dirs $commons_dir [get_filesets sources_1]\nset_property SOURCE_SET sources_1 [get_filesets sim_1]\nadd_files -fileset sim_1 -norecurse $sim_sources\nset_property top PID_tb [get_filesets sim_1]\nupdate_compile_order\n");

    std::filesystem::current_path(wd);
    std::filesystem::remove_all("/tmp/vivado");
    e2e_clean_settings();
}