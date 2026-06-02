// Copyright 2021 Filippo Savi
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
#include "Backend/Xilinx/xilinx_project_generator.hpp"

#include "frontend/analysis/sv_analyzer.hpp"
#include "analysis/HDL_ast_builder_v2.hpp"
#include "Backend/Dependency_resolver.hpp"

std::string xpg_settings_path = "/tmp/test_settings_store";
auto xpg_settings_file = xpg_settings_path + "/settings";

std::shared_ptr<settings_store> xpg_setup_settings() {
    std::filesystem::create_directories(xpg_settings_path);
    std::ofstream ofs(xpg_settings_file);
    ofs << "{\"hdl_store\":\"/tmp/rb\",\"amd_vivado_path\":\"/dev/zero\"}";
    ofs.flush();
    ofs.close();
    std::error_code ec;
    std::filesystem::resize_file(xpg_settings_file, std::filesystem::file_size(xpg_settings_file, ec), ec);
    return std::make_shared<settings_store>(false, xpg_settings_path);
}

void xpg_clean_settings() {
    std::filesystem::remove_all(xpg_settings_file);
    std::filesystem::remove_all(xpg_settings_path);
}



TEST(xilinx_project_gen, simple_gen){


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = xpg_setup_settings();
    std::vector<std::string> paths = {
            "Components/Common",
            "Components/ExternalDrivers/AD2S1210/rtl",
            "Components/ExternalDrivers/AD2S1210/tb",
            "Components/ExternalDrivers/AD2S1210/tb",
            "Components/system/axi_stream/combiner/rtl",
            "Components/comms/SPI/rtl/master",
            "Components/system/EnableGenerator/rtl",
            "Components/comms/SPI/rtl",
            "Components/system/axi_lite/crossbar/rtl/",
            "Components/system/axi_lite/simple_register_cu/rtl",
            "Components/system/axi_lite/skid_buffer/rtl"
    };

    auto prefix = "check_files/test_data/";
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv" || f.path().extension() == ".svh"){
                auto test_file = mm_file(f.path());
                sv_analyzer analyzer;
                analyzer.set_include_directories({std::string(prefix) + "Components/Common"});
                auto res = analyzer.analyze(f.path(), test_file.view());
                d_store->store_hdl_entity(res, "", "");
            }
        }
    }

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast_v2 = b.build_ast(std::vector<std::string>({"AD2S1210_tb"}))[0];


    Dependency_resolver_v2 sim_r({ast_v2}, d_store);
    auto sources = sim_r.get_dependencies();

    xilinx_project_generator generator(s_store);

    project_data d;
    d.name = "test_proj";
    d.synth_sources = {};
    d.sim_sources = sources;
    d.tb_tl = "AD2S1210_tb";
    d.commons_dir = {};
    d.repo_dir = "/tmp/tb";
    generator.set_data(d);


    std::ostringstream result_tcl;
    generator.write_makefile(result_tcl);

    const std::string expected_tcl = "set project_name test_proj\nset origin_dir \".\"\nset base_dir /tmp/rb\nset commons_dir [list ]\nset synth_sources [list ]\nset sim_sources [list \"check_files/test_data/Components/Common/interfaces.sv\" \"check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/AD2S1210.sv\" \"check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_CU.sv\" \"check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_configurator.sv\" \"check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_fault_handler.sv\" \"check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_reader.sv\" \"check_files/test_data/Components/ExternalDrivers/AD2S1210/tb/AD2S1210_tb.sv\" \"check_files/test_data/Components/ExternalDrivers/AD2S1210/tb/ad2s1210_tl_test.sv\" \"check_files/test_data/Components/comms/SPI/rtl/Spi.sv\" \"check_files/test_data/Components/comms/SPI/rtl/master/ClockGen.sv\" \"check_files/test_data/Components/comms/SPI/rtl/master/SpiControlUnit.sv\" \"check_files/test_data/Components/comms/SPI/rtl/master/SpiRegister.sv\" \"check_files/test_data/Components/comms/SPI/rtl/master/Spi_master.sv\" \"check_files/test_data/Components/comms/SPI/rtl/master/TransferEngine.sv\" \"check_files/test_data/Components/system/EnableGenerator/rtl/Enable_Generator_2.sv\" \"check_files/test_data/Components/system/EnableGenerator/rtl/enable_comparator.v\" \"check_files/test_data/Components/system/EnableGenerator/rtl/enable_generator_core.v\" \"check_files/test_data/Components/system/EnableGenerator/rtl/enable_generator_counter.v\" \"check_files/test_data/Components/system/axi_lite/crossbar/rtl/address_decoder.sv\" \"check_files/test_data/Components/system/axi_lite/crossbar/rtl/axil_crossbar.sv\" \"check_files/test_data/Components/system/axi_lite/crossbar/rtl/axil_crossbar_wrapper.sv\" \"check_files/test_data/Components/system/axi_lite/simple_register_cu/rtl/axil_simple_register_cu.sv\" \"check_files/test_data/Components/system/axi_lite/skid_buffer/rtl/axil_skid_buffer.sv\" \"check_files/test_data/Components/system/axi_stream/combiner/rtl/axi_stream_combiner_3.sv\" ]\nset constraints_sources [list ]\n# Create project\ncreate_project ${project_name} ./${project_name}\nset_property part xc7z020clg400-1 [current_project]\n# Set the directory path for the new project\nset proj_dir [get_property directory [current_project]]\nset obj [current_project]\nadd_files -norecurse $synth_sources\nset_property include_dirs $commons_dir [get_filesets sources_1]\nset_property SOURCE_SET sources_1 [get_filesets sim_1]\nadd_files -fileset sim_1 -norecurse $sim_sources\nset_property top AD2S1210_tb [get_filesets sim_1]\nupdate_compile_order\n";

    EXPECT_EQ(expected_tcl, result_tcl.str());
    xpg_clean_settings();
}

TEST(xilinx_project_gen, argumented_script_gen){

    script_source s;
    s.name = "scr_name";
    s.path = "/test/script/source.sv";
    s.function_mode = true;
    s.variables = {{"A","1"}};

    std::shared_ptr<settings_store> s_store = xpg_setup_settings();
    xilinx_project_generator gen(s_store);

    project_data d;
    d.name = "proj_name";
    d.synth_sources = {"/test/synth/source.sv"};
    d.sim_sources = {"/test/sim/source.sv"};
    d.scripts = {s, s};
    d.constraints_sources = {"/test/constr/source.sv"};
    d.tb_tl = "sim_tl";
    d.synth_tl = "synth_tl";
    d.commons_dir = {"/include"};
    d.repo_dir = "/test/root_dir";

    gen.set_data(d);


    std::stringstream test_stream;
    gen.write_makefile(test_stream);
    auto result = test_stream.str();

    const std::string check = "set project_name proj_name\nset origin_dir \".\"\nset base_dir /tmp/rb\nset commons_dir [list \"/tmp/rb/include\" ]\nset synth_sources [list \"/test/synth/source.sv\" ]\nset sim_sources [list \"/test/sim/source.sv\" ]\nset constraints_sources [list \"/test/constr/source.sv\" ]\n# Create project\ncreate_project ${project_name} ./${project_name}\nset_property part xc7z020clg400-1 [current_project]\n# Set the directory path for the new project\nset proj_dir [get_property directory [current_project]]\nset obj [current_project]\nsource /test/script/source.sv\nscr_name 1\nscr_name 1\nadd_files -norecurse $synth_sources\nset_property top synth_tl [get_filesets sources_1]\nset_property include_dirs $commons_dir [get_filesets sources_1]\nset_property SOURCE_SET sources_1 [get_filesets sim_1]\nadd_files -fileset constrs_1 -norecurse  $constraints_sources\nadd_files -fileset sim_1 -norecurse $sim_sources\nset_property top sim_tl [get_filesets sim_1]\nupdate_compile_order\n";
    ASSERT_EQ(result, check);
    xpg_clean_settings();
}



TEST( xilinx_project_gen, sim_script) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = xpg_setup_settings();

    std::vector<std::string> paths = {
            "Components/Common",
            "Components/ExternalDrivers/AD2S1210/rtl",
            "Components/ExternalDrivers/AD2S1210/tb",
            "Components/ExternalDrivers/AD2S1210/tb",
            "Components/system/axi_stream/combiner/rtl",
            "Components/comms/SPI/rtl/master",
            "Components/system/EnableGenerator/rtl",
            "Components/comms/SPI/rtl",
            "Components/system/axi_lite/crossbar/rtl/",
            "Components/system/axi_lite/simple_register_cu/rtl",
            "Components/system/axi_lite/skid_buffer/rtl"
    };

    auto prefix = "check_files/test_data/";
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv" || f.path().extension() == ".svh"){
                auto test_file = mm_file(f.path());
                sv_analyzer analyzer;

                analyzer.set_include_directories({std::string(prefix) + "Components/Common"});
                auto res = analyzer.analyze(f.path(), test_file.view());
                d_store->store_hdl_entity(res, "", "");
            }
        }
    }


    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast_v2 = b.build_ast(std::vector<std::string>({"AD2S1210_tb"}))[0];


    Dependency_resolver_v2 sim_r({ast_v2}, d_store);
    auto sources = sim_r.get_dependencies();

    xilinx_project_generator generator(s_store);

    project_data d;
    d.name = "test_proj";
    d.synth_sources = {};
    d.sim_sources = sources;
    d.tb_tl = "AD2S1210_tb";
    d.commons_dir = {};
    d.repo_dir = "/tmp/tb";
    generator.set_data(d);

    std::ostringstream result_sh;
    generator.generate_sim_script(result_sh);

    std::ostringstream result_tcl;
    generator.write_sim_control_script(result_tcl);

    const std::string expected_sh = "FILES=( \n    /dev/zero/data/verilog/src/glbl.v\n    check_files/test_data/Components/Common/interfaces.sv\n    check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/AD2S1210.sv\n    check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_CU.sv\n    check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_configurator.sv\n    check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_fault_handler.sv\n    check_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_reader.sv\n    check_files/test_data/Components/ExternalDrivers/AD2S1210/tb/AD2S1210_tb.sv\n    check_files/test_data/Components/ExternalDrivers/AD2S1210/tb/ad2s1210_tl_test.sv\n    check_files/test_data/Components/comms/SPI/rtl/Spi.sv\n    check_files/test_data/Components/comms/SPI/rtl/master/ClockGen.sv\n    check_files/test_data/Components/comms/SPI/rtl/master/SpiControlUnit.sv\n    check_files/test_data/Components/comms/SPI/rtl/master/SpiRegister.sv\n    check_files/test_data/Components/comms/SPI/rtl/master/Spi_master.sv\n    check_files/test_data/Components/comms/SPI/rtl/master/TransferEngine.sv\n    check_files/test_data/Components/system/EnableGenerator/rtl/Enable_Generator_2.sv\n    check_files/test_data/Components/system/EnableGenerator/rtl/enable_comparator.v\n    check_files/test_data/Components/system/EnableGenerator/rtl/enable_generator_core.v\n    check_files/test_data/Components/system/EnableGenerator/rtl/enable_generator_counter.v\n    check_files/test_data/Components/system/axi_lite/crossbar/rtl/address_decoder.sv\n    check_files/test_data/Components/system/axi_lite/crossbar/rtl/axil_crossbar.sv\n    check_files/test_data/Components/system/axi_lite/crossbar/rtl/axil_crossbar_wrapper.sv\n    check_files/test_data/Components/system/axi_lite/simple_register_cu/rtl/axil_simple_register_cu.sv\n    check_files/test_data/Components/system/axi_lite/skid_buffer/rtl/axil_skid_buffer.sv\n    check_files/test_data/Components/system/axi_stream/combiner/rtl/axi_stream_combiner_3.sv\n)\n\nmkdir -p /tmp/tb/sim\ncp sim.tcl /tmp/tb/sim/sim.tcl\n\n\n(\n    cd /tmp/tb/sim|| exit\n\n    echo -e \"\\n\\033[1;33m>>> PHASE 1: XVLOG (Analysis) <<<\\033[0m\"\n    xvlog -sv \"${FILES[@]}\"  -i /dev/zero/data/rsb/busdef\n    if [ $? -ne 0 ]; then\n        echo -e \"\\033[1;31m!!! XVLOG FAILED !!!\\033[0m\"\n        exit 1\n    fi\n\n    echo -e \"\\n\\033[1;33m>>> PHASE 2: XELAB (Elaboration) <<<\\033[0m\"\n    xelab -debug typical --relax -top AD2S1210_tb -top glbl -L xil_defaultlib -L unisims_ver -L unimacro_ver -L xpm  -snapshot sim_snapshot  -timescale 10ns/1ps\n    if [ $? -ne 0 ]; then\n        echo -e \"\\033[1;31m!!! XELAB FAILED !!!\\033[0m\"\n        exit 1\n    fi\n\n    echo -e \"\\n\\033[1;33m>>> PHASE 3: XSIM (Simulation) <<<\\033[0m\"\n    xsim sim_snapshot -tclbatch sim.tcl\n    if [ $? -ne 0 ]; then\n        echo -e \"\\033[1;31m!!! XSIM FAILED !!!\\033[0m\"\n        exit 1\n    fi\n\n)\nif [ -f /tmp/tb/sim/dump.vcd  ]; then\n    vcd2fst /tmp/tb/sim/dump.vcd dump.fst\n    rm /tmp/tb/sim/dump.vcd\nfi\nrm -r /tmp/tb/sim\n";
    const std::string expected_tcl = "\nopen_vcd dump.vcd\n\nlog_vcd [get_objects -recursive /*]\n\nrun 2ms\n\nflush_vcd\nclose_vcd\nexit\n";
    auto dbg = result_sh.str();
    EXPECT_EQ(expected_sh, result_sh.str());
    EXPECT_EQ(expected_tcl, result_tcl.str());
    xpg_clean_settings();
}



TEST( xilinx_project_gen, fpga_synth_script) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = xpg_setup_settings();
    std::vector<std::string> paths = {
            "Components/Common",
            "Components/ExternalDrivers/AD2S1210/rtl",
            "Components/ExternalDrivers/AD2S1210/tb",
            "Components/ExternalDrivers/AD2S1210/tb",
            "Components/system/axi_stream/combiner/rtl",
            "Components/comms/SPI/rtl/master",
            "Components/system/EnableGenerator/rtl",
            "Components/comms/SPI/rtl",
            "Components/system/axi_lite/crossbar/rtl/",
            "Components/system/axi_lite/simple_register_cu/rtl",
            "Components/system/axi_lite/skid_buffer/rtl"
    };

    auto prefix = "check_files/test_data/";
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv" || f.path().extension() == ".svh"){
                auto test_file = mm_file(f.path());
                sv_analyzer analyzer;

                analyzer.set_include_directories({std::string(prefix) + "Components/Common"});
                auto res = analyzer.analyze(f.path(), test_file.view());
                d_store->store_hdl_entity(res, "", "");
            }
        }
    }


    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast_v2 = b.build_ast(std::vector<std::string>({"ad2s1210_tl"}))[0];


    Dependency_resolver_v2 sim_r({ast_v2}, d_store);
    auto sources = sim_r.get_dependencies();

    xilinx_project_generator generator(s_store);

    project_data d;
    d.name = "test_proj";
    d.synth_sources = sources;
    d.sim_sources = {};
    d.synth_tl = "ad2s1210_tl";
    d.commons_dir = {};
    d.target_part = "xc7s25ftgb196-1";
    d.repo_dir = "/tmp/tb";
    generator.set_data(d);


    std::ostringstream result_tcl;
    generator.generate_synth_script(result_tcl);

    const std::string expected_tcl = "set outputDir ./project_output\nfile mkdir $outputDir\ncd $outputDir\nset data_files_set {\n}\nset sources_set {\n\tcheck_files/test_data/Components/Common/interfaces.sv\n\tcheck_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/AD2S1210.sv\n\tcheck_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_CU.sv\n\tcheck_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_configurator.sv\n\tcheck_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_fault_handler.sv\n\tcheck_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_reader.sv\n\tcheck_files/test_data/Components/ExternalDrivers/AD2S1210/rtl/ad2s1210_tl.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/Spi.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/ClockGen.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/SpiControlUnit.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/SpiRegister.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/Spi_master.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/TransferEngine.sv\n\tcheck_files/test_data/Components/system/EnableGenerator/rtl/Enable_Generator_2.sv\n\tcheck_files/test_data/Components/system/EnableGenerator/rtl/enable_comparator.v\n\tcheck_files/test_data/Components/system/EnableGenerator/rtl/enable_generator_core.v\n\tcheck_files/test_data/Components/system/EnableGenerator/rtl/enable_generator_counter.v\n\tcheck_files/test_data/Components/system/axi_lite/crossbar/rtl/address_decoder.sv\n\tcheck_files/test_data/Components/system/axi_lite/crossbar/rtl/axil_crossbar.sv\n\tcheck_files/test_data/Components/system/axi_lite/crossbar/rtl/axil_crossbar_wrapper.sv\n\tcheck_files/test_data/Components/system/axi_lite/simple_register_cu/rtl/axil_simple_register_cu.sv\n\tcheck_files/test_data/Components/system/axi_lite/skid_buffer/rtl/axil_skid_buffer.sv\n\tcheck_files/test_data/Components/system/axi_stream/combiner/rtl/axi_stream_combiner_3.sv\n}\nset inc_dirs {\n}\nset constr_dirs {\n}\nforeach f $data_files_set {\n    file copy -force $f .\n}\nset_part xc7s25ftgb196-1\nread_verilog $sources_set\nif {[llength $constr_dirs] > 0} { read_xdc $constr_dirs }\nsynth_design -top ad2s1210_tl -part xc7s25ftgb196-1 -include_dirs $inc_dirs\nwrite_checkpoint -force $outputDir/post_synth.dcp\nopt_design\nplace_design\nwrite_checkpoint  -force $outputDir/post_place.dcp\nroute_design\nwrite_checkpoint  -force $outputDir/post_route.dcp\nwrite_bitstream -force $outputDir/top_module.bit\n";

    EXPECT_EQ(expected_tcl, result_tcl.str());
    xpg_clean_settings();
}



TEST( xilinx_project_gen, soc_synth_script) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = xpg_setup_settings();

    std::vector<std::string> paths = {
            "Components/Common",
            "Components/comms/SPI/rtl/master",
            "Components/system/EnableGenerator/rtl",
            "Components/system/axi_lite/crossbar/rtl/",
            "Components/comms/SPI/rtl",
            "Components/comms/SPI/rtl/master",
            "Components/system/axi_stream/combiner/rtl",
            "Components/PS_interfaces/zynqmp",
            "Components/system/axi_lite/external_registers_cu/rtl",
            "Components/system/axi_terminator/rtl",
            "Components/system/axi_lite/simple_register_cu/rtl",
            "Components/system/axi_lite/skid_buffer/rtl",
            "Applications/simple_vsi_base/rtl",
            "Components/system/axi_stream/fifo/rtl",
            "Components/system/interrupt_controller/rtl",
            "Components/controls/gpio/rtl",
            "Components/controls/PwmGenerator/rtl",
            "Components/signal_chain/encoder_interface/rtl",
            "Components/signal_chain/spi_adc_interface/rtl",
            "Components/signal_chain/AdcProcessing/rtl",
            "Components/signal_chain/fir_filter/rtl",
            "Components/signal_chain/decimator/standard_decimator"

    };

    auto prefix = "check_files/test_data/";
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv" || f.path().extension() == ".svh"){
                auto test_file = mm_file(f.path());
                sv_analyzer analyzer;

                analyzer.set_include_directories({std::string(prefix) + "Components/Common"});
                auto res = analyzer.analyze(f.path(), test_file.view());
                d_store->store_hdl_entity(res, "", "");
            }
        }
    }

    auto d_f = std::stringstream(R"(
        {
            "general":{
                "project_name":"simple_vsi_base",
                "target_part":"xc7s25ftgb196-1",
                "synth_modules":[],
                "synth_tl": "simple_vsi_base",
                "sim_modules":[],
                "sim_tl":"simple_vsi_base",
                "include_paths":[]
            },
            "scripts":[
                {
                    "name":"base_zynqmp_xvc.tcl",
                    "type": "tcl",
                    "arguments":[]
                },
                {
                    "name":"adder.tcl",
                    "type": "tcl",
                    "function_mode": true,
                    "arguments":[
                        {"name":"name","value":"adder"},
                        {"name":"value","value":"5"}
                    ]
                }
            ],
            "excluded_modules":["ps"],
            "constraints": []
        }
    )");

    Depfile dep_file(d_f);
    HDL_ast_builder_v2 b(s_store, d_store, dep_file);
    auto ast_v2 = b.build_ast(std::vector<std::string>({"simple_vsi_base"}))[0];


    Dependency_resolver_v2 sim_r({ast_v2}, d_store);
    auto sources = sim_r.get_dependencies();

    xilinx_project_generator generator(s_store);

    project_data d;
    d.name = dep_file.general.project_name;
    d.synth_sources = sources;
    d.sim_sources = {};
    d.synth_tl = dep_file.general.synth_tl;
    d.commons_dir = {};
    d.scripts = {
        {
            "function_script",
            "/tmp/function_script.tcl",
            true,
            {
                {"name", "fcn"},
                {"value", "1"}
            }
        },
        {
            "simple_script",
            "/tmp/simple_script.tcl",
            false
        }
    };
    d.target_part = "xc7s25ftgb196-1";
    d.board_part = "xilinx.com:kd240_som:part0:1.1";
    d.repo_dir = "/tmp/tb";
    generator.set_data(d);


    std::ostringstream result_tcl;
    generator.generate_synth_script(result_tcl);

    const std::string expected_tcl = "set outputDir ./project_output\nfile mkdir $outputDir\ncd $outputDir\nset data_files_set {\n}\nset sources_set {\n\tcheck_files/test_data/Applications/simple_vsi_base/rtl/simple_vsi_base.sv\n\tcheck_files/test_data/Applications/simple_vsi_base/rtl/simple_vsi_logic.sv\n\tcheck_files/test_data/Applications/simple_vsi_base/rtl/simple_vsi_mmio.sv\n\tcheck_files/test_data/Components/Common/DP_RAM.sv\n\tcheck_files/test_data/Components/Common/interfaces.sv\n\tcheck_files/test_data/Components/Common/saturating_adder.v\n\tcheck_files/test_data/Components/PS_interfaces/zynqmp/Zynq_us_axis_wrapper.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/Spi.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/ClockGen.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/SpiControlUnit.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/SpiRegister.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/Spi_master.sv\n\tcheck_files/test_data/Components/comms/SPI/rtl/master/TransferEngine.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/ChainControlUnit.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/CompareUnit.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/Counter.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/CounterCore.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/CounterEnableDelayCore.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/DeadTimeGenerator.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/PinControl.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/PwmControlUnit.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/PwmGenerator.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/ResolutionEnhancer.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/SyncEngine.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/TimebaseGenerator.sv\n\tcheck_files/test_data/Components/controls/PwmGenerator/rtl/pwmChain.sv\n\tcheck_files/test_data/Components/controls/gpio/rtl/gpio.sv\n\tcheck_files/test_data/Components/signal_chain/AdcProcessing/rtl/AdcProcessing.sv\n\tcheck_files/test_data/Components/signal_chain/AdcProcessing/rtl/AdcProcessingControlUnit.sv\n\tcheck_files/test_data/Components/signal_chain/AdcProcessing/rtl/calibration.sv\n\tcheck_files/test_data/Components/signal_chain/AdcProcessing/rtl/comparator.sv\n\tcheck_files/test_data/Components/signal_chain/AdcProcessing/rtl/denoiser.sv\n\tcheck_files/test_data/Components/signal_chain/AdcProcessing/rtl/linearizer.sv\n\tcheck_files/test_data/Components/signal_chain/decimator/standard_decimator/standard_decimator.sv\n\tcheck_files/test_data/Components/signal_chain/encoder_interface/rtl/angle_sensing.sv\n\tcheck_files/test_data/Components/signal_chain/encoder_interface/rtl/encoder_interface.sv\n\tcheck_files/test_data/Components/signal_chain/encoder_interface/rtl/speed_sensing.sv\n\tcheck_files/test_data/Components/signal_chain/fir_filter/rtl/fir_filter_serial.sv\n\tcheck_files/test_data/Components/signal_chain/spi_adc_interface/rtl/spi_adc_interface.sv\n\tcheck_files/test_data/Components/system/EnableGenerator/rtl/EnableGenerator.sv\n\tcheck_files/test_data/Components/system/EnableGenerator/rtl/Enable_Generator_2.sv\n\tcheck_files/test_data/Components/system/EnableGenerator/rtl/enable_comparator.v\n\tcheck_files/test_data/Components/system/EnableGenerator/rtl/enable_generator_core.v\n\tcheck_files/test_data/Components/system/EnableGenerator/rtl/enable_generator_counter.v\n\tcheck_files/test_data/Components/system/axi_lite/crossbar/rtl/address_decoder.sv\n\tcheck_files/test_data/Components/system/axi_lite/crossbar/rtl/axil_crossbar.sv\n\tcheck_files/test_data/Components/system/axi_lite/crossbar/rtl/axil_crossbar_wrapper.sv\n\tcheck_files/test_data/Components/system/axi_lite/external_registers_cu/rtl/axil_external_registers_cu.sv\n\tcheck_files/test_data/Components/system/axi_lite/simple_register_cu/rtl/axil_simple_register_cu.sv\n\tcheck_files/test_data/Components/system/axi_lite/skid_buffer/rtl/axil_skid_buffer.sv\n\tcheck_files/test_data/Components/system/axi_stream/combiner/rtl/axi_stream_combiner.sv\n\tcheck_files/test_data/Components/system/axi_stream/fifo/rtl/axis_fifo.sv\n\tcheck_files/test_data/Components/system/axi_stream/fifo/rtl/axis_fifo_xpm.sv\n\tcheck_files/test_data/Components/system/axi_terminator/rtl/axi_terminator.sv\n\tcheck_files/test_data/Components/system/interrupt_controller/rtl/interrupt_controller.sv\n}\nset inc_dirs {\n}\nset constr_dirs {\n}\nforeach f $data_files_set {\n    file copy -force $f .\n}\nset_part xc7s25ftgb196-1\nset board_part xilinx.com:kd240_som:part0:1.1\nsource /tmp/function_script.tcl\nfunction_script fcn 1\nsynth_ip [get_ips fcn]\nsource /tmp/simple_script.tcl\nread_verilog $sources_set\nif {[llength $constr_dirs] > 0} { read_xdc $constr_dirs }\nsynth_design -top simple_vsi_base -part xc7s25ftgb196-1 -include_dirs $inc_dirs\nwrite_checkpoint -force $outputDir/post_synth.dcp\nopt_design\nplace_design\nwrite_checkpoint  -force $outputDir/post_place.dcp\nroute_design\nwrite_checkpoint  -force $outputDir/post_route.dcp\nwrite_bitstream -force $outputDir/top_module.bit\n";

    EXPECT_EQ(expected_tcl, result_tcl.str());
    xpg_clean_settings();
}