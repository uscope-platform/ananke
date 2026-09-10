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
#include <filesystem>
#include <sstream>
#include <thread>

#include "data_model/mm_file.hpp"
#include "frontend/analysis/system_verilog/sv_analyzer.hpp"
#include "frontend/analysis/vhdl/vhdl_analyzer.hpp"
#include "analysis/HDL_ast_builder_v2.hpp"
#include "analysis/passes/pass_manager.hpp"
#include "Backend/Dependency_resolver.hpp"

#include <spdlog/sinks/ostream_sink.h>

#include "test_paths.hpp"

namespace {
// Captures spdlog output for the duration of the guard's lifetime.
struct log_capture {
    std::ostringstream stream;
    std::shared_ptr<spdlog::sinks::ostream_sink_mt> sink =
        std::make_shared<spdlog::sinks::ostream_sink_mt>(stream);
    log_capture() { spdlog::default_logger()->sinks().push_back(sink); }
    ~log_capture() {
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

TEST( hdl_ast_builder, pid_ast_build) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    std::vector<std::string> paths = {
            "Components/controls/PID/rtl",
            "Components/controls/integrator/rtl",
            "Components/Common",
            "Components/system/axi_lite/simple_register_cu/rtl",
            "Components/system/axi_lite/skid_buffer/rtl"
    };

    auto prefix = td_file("check_files/test_data/");
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv"){
                mm_file test_file(f.path());
                sv_analyzer analyzer;


                auto resources = analyzer.analyze("", test_file.view()).value();
                d_store->store_file({f.path(), "file_hash", resources});
            }
        }
    }


    auto reference_structure = "TL:PID\n    CU:axil_simple_register_cu\n        address_read_buffer:axil_skid_buffer\n        address_write_buffer:axil_skid_buffer\n        write_data_buffer:axil_skid_buffer\n    pid_int:Integrator\n";

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto synth_ast = b2.build_ast(std::vector<std::string>({"PID"}))[0];

    auto struct_v2 = synth_ast->dump_structure();
    EXPECT_EQ(struct_v2, reference_structure);

    auto ast_dump = synth_ast->dump();
    nlohmann::json check_obj = nlohmann::json::parse(R"({"children":[{"children":[{"children":[],"instance_name":"address_read_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axil.ARADDR"],"in_ready":["axil.ARREADY"],"in_valid":["axil.ARVALID"],"out_data":["read_address"],"out_ready":["read_ready"],"out_valid":["read_address_valid"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"address_write_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axil.AWADDR"],"in_ready":["axil.AWREADY"],"in_valid":["axil.AWVALID"],"out_data":["write_address"],"out_ready":["write_ready"],"out_valid":["write_address_valid"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"write_data_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[36]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axil.WDATA","axil.WSTRB"],"in_ready":["axil.WREADY"],"in_valid":["axil.WVALID"],"out_data":["write_data","write_strobe"],"out_ready":["write_ready"],"out_valid":["write_data_valid"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"CU","instance_type":"axil_simple_register_cu","parameters":{"ADDRESS_MASK":{"name":"ADDRESS_MASK","type":"numeric_parameter","value":[63]},"INITIAL_OUTPUT_VALUES":{"name":"INITIAL_OUTPUT_VALUES","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":true,"size":0,"value":32767,"wide":false,"wide_value":"0x7fff"},{"signedness":true,"size":0,"value":-32767,"wide":false,"wide_value":"0xffffffffffff8001"},{"signedness":true,"size":0,"value":32767,"wide":false,"wide_value":"0x7fff"},{"signedness":true,"size":0,"value":-32767,"wide":false,"wide_value":"0xffffffffffff8001"},{"signedness":false,"size":0,"value":65793,"wide":false,"wide_value":"0x10101"}]]]},"N_READ_REGISTERS":{"name":"N_READ_REGISTERS","type":"numeric_parameter","value":[9]},"N_TRIGGER_REGISTERS":{"name":"N_TRIGGER_REGISTERS","type":"numeric_parameter","value":[1]},"N_WRITE_REGISTERS":{"name":"N_WRITE_REGISTERS","type":"numeric_parameter","value":[9]},"REGISTERED_BUFFERS":{"name":"REGISTERED_BUFFERS","type":"numeric_parameter","value":[0]},"REGISTERS_WIDTH":{"name":"REGISTERS_WIDTH","type":"numeric_parameter","value":[32]},"TRIGGER_REGISTERS_IDX":{"name":"TRIGGER_REGISTERS_IDX","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]}},"ports_map":{"axil":["axil"],"clock":["clock"],"input_registers":["cu_read_registers"],"output_registers":["cu_write_registers"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"pid_int","instance_type":"Integrator","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clock":["clock"],"error_in":["integrator_in"],"input_valid":["integrator_in_valid"],"limit_int_down":["limit_int_down"],"limit_int_up":["limit_int_up"],"out":["integral_out"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"TL","instance_type":"PID","parameters":{"ADDITIONAL_BITS":{"name":"ADDITIONAL_BITS","type":"numeric_parameter","value":[16]},"INITIAL_OUTPUT_VALUES":{"name":"INITIAL_OUTPUT_VALUES","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":true,"size":0,"value":32767,"wide":false,"wide_value":"0x7fff"},{"signedness":true,"size":0,"value":-32767,"wide":false,"wide_value":"0xffffffffffff8001"},{"signedness":true,"size":0,"value":32767,"wide":false,"wide_value":"0x7fff"},{"signedness":true,"size":0,"value":-32767,"wide":false,"wide_value":"0xffffffffffff8001"},{"signedness":false,"size":0,"value":65793,"wide":false,"wide_value":"0x10101"}]]]},"INPUT_DATA_WIDTH":{"name":"INPUT_DATA_WIDTH","type":"numeric_parameter","value":[12]},"OUTPUT_DATA_WIDTH":{"name":"OUTPUT_DATA_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{},"wildcard_assignment":false})");

    EXPECT_EQ(ast_dump, check_obj);
}


TEST( hdl_ast_builder, spi_ast_build) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    std::vector<std::string> paths = {
            "Components/comms/SPI/rtl",
            "Components/comms/SPI/rtl/master",
            "Components/system/EnableGenerator/rtl/",
            "Components/Common",
            "Components/system/axi_lite/simple_register_cu/rtl",
            "Components/system/axi_lite/skid_buffer/rtl"
    };

    auto prefix = td_file("check_files/test_data/");
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv"){
                auto test_file = mm_file(f.path());
                sv_analyzer analyzer;
                analyzer.set_include_directories({std::string(prefix) + "Components/Common"});

                auto resources = analyzer.analyze("", test_file.view()).value();
                d_store->store_file({f.path(), "file_hash", resources});
            }
        }
    }


    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"SPI"}))[0];

    auto struct_v2 = ast_v2->dump_structure();

    auto reference_structure = "TL:SPI\n    master_module:SPI_master\n        ENG:enable_generator_core\n        CKG:ClockGen\n        SHR:SpiRegister\n        SHR:SpiRegister\n        SHR:SpiRegister\n        STE:TransferEngine\n        SCU:SpiControlUnit\n        axi_if:axil_simple_register_cu\n        address_read_buffer:axil_skid_buffer\n        address_write_buffer:axil_skid_buffer\n        write_data_buffer:axil_skid_buffer\n";

    EXPECT_EQ(struct_v2, reference_structure);

    auto ast_dump = ast_v2->dump();
    nlohmann::json check_obj = nlohmann::json::parse(R"({"children":[{"children":[{"children":[],"instance_name":"ENG","instance_type":"enable_generator_core","parameters":{"CLOCK_MODE":{"name":"CLOCK_MODE","type":"string_parameter","value":["FALSE"]},"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{"clock":["clock"],"enable_out":["internal_start"],"gen_enable_in":["start_generator_enable"],"period":["start_generator_period"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"CKG","instance_type":"ClockGen","parameters":{"S0":{"name":"S0","type":"numeric_parameter","value":[0]},"S1":{"name":"S1","type":"numeric_parameter","value":[1]},"S2":{"name":"S2","type":"numeric_parameter","value":[2]}},"ports_map":{"clockIn":["clock"],"dividerSetting":["divider_setting"],"enable":["enable_clockgen"],"polarity":["clock_polarity"],"reset":["reset"],"sync":["sync"],"timebaseOut":["clock_cont"]},"wildcard_assignment":false},{"children":[],"instance_name":"SHR","instance_type":"SpiRegister","parameters":{"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[3]},"serial_lsb_out_first":{"name":"serial_lsb_out_first","type":"numeric_parameter","value":[1]},"serial_msb_out_first":{"name":"serial_msb_out_first","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"enable":["register_enable"],"latching_edge":["latching_edge"],"parallel_in":["parallel_reg_in[0]"],"parallel_out":["parallel_reg_out[0]"],"parallel_out_valid":["parallel_reg_valid[0]"],"register_direction":["spi_direction"],"register_load":["register_load"],"reset":["reset"],"serial_in":["MISO[0]"],"serial_out":["MOSI[0]"],"shift_clock":["generated_sclk"],"spi_transfer_length":["chosen_spi_transfer_length"]},"wildcard_assignment":false},{"children":[],"instance_name":"SHR","instance_type":"SpiRegister","parameters":{"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[3]},"serial_lsb_out_first":{"name":"serial_lsb_out_first","type":"numeric_parameter","value":[1]},"serial_msb_out_first":{"name":"serial_msb_out_first","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"enable":["register_enable"],"latching_edge":["latching_edge"],"parallel_in":["parallel_reg_in[1]"],"parallel_out":["parallel_reg_out[1]"],"parallel_out_valid":["parallel_reg_valid[1]"],"register_direction":["spi_direction"],"register_load":["register_load"],"reset":["reset"],"serial_in":["MISO[1]"],"serial_out":["MOSI[1]"],"shift_clock":["generated_sclk"],"spi_transfer_length":["chosen_spi_transfer_length"]},"wildcard_assignment":false},{"children":[],"instance_name":"SHR","instance_type":"SpiRegister","parameters":{"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[3]},"serial_lsb_out_first":{"name":"serial_lsb_out_first","type":"numeric_parameter","value":[1]},"serial_msb_out_first":{"name":"serial_msb_out_first","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"enable":["register_enable"],"latching_edge":["latching_edge"],"parallel_in":["parallel_reg_in[2]"],"parallel_out":["parallel_reg_out[2]"],"parallel_out_valid":["parallel_reg_valid[2]"],"register_direction":["spi_direction"],"register_load":["register_load"],"reset":["reset"],"serial_in":["MISO[2]"],"serial_out":["MOSI[2]"],"shift_clock":["generated_sclk"],"spi_transfer_length":["chosen_spi_transfer_length"]},"wildcard_assignment":false},{"children":[],"instance_name":"STE","instance_type":"TransferEngine","parameters":{"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[3]},"OUTPUT_WIDTH":{"name":"OUTPUT_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{"clock":["clock"],"cu_data_in":["cu_in"],"cu_data_out":["cu_out"],"divider_setting":["divider_setting"],"enable_clockgen":["enable_clockgen"],"reg_data_in":["parallel_reg_in"],"reg_data_out":["parallel_reg_out"],"reg_data_out_valid":["parallel_reg_valid[0]"],"register_enable":["register_enable"],"register_load":["register_load"],"reset":["reset"],"spi_delay":["spi_delay"],"spi_start_transfer":["transfer_start"],"spi_transfer_length":["chosen_spi_transfer_length"],"ss_blanking":["ss_blanking"],"ss_deassert_delay_enable":["ss_deassert_delay_enable"],"sync":["sync"],"transfer_done":["transfer_done"]},"wildcard_assignment":false},{"children":[{"children":[{"children":[],"instance_name":"address_read_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axil.ARADDR"],"in_ready":["axil.ARREADY"],"in_valid":["axil.ARVALID"],"out_data":["read_address"],"out_ready":["read_ready"],"out_valid":["read_address_valid"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"address_write_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axil.AWADDR"],"in_ready":["axil.AWREADY"],"in_valid":["axil.AWVALID"],"out_data":["write_address"],"out_ready":["write_ready"],"out_valid":["write_address_valid"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"write_data_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[36]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axil.WDATA","axil.WSTRB"],"in_ready":["axil.WREADY"],"in_valid":["axil.WVALID"],"out_data":["write_data","write_strobe"],"out_ready":["write_ready"],"out_valid":["write_data_valid"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"axi_if","instance_type":"axil_simple_register_cu","parameters":{"ADDRESS_MASK":{"name":"ADDRESS_MASK","type":"numeric_parameter","value":[63]},"INITIAL_OUTPUT_VALUES":{"name":"INITIAL_OUTPUT_VALUES","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":224,"wide":false,"wide_value":"0xe0"},{"signedness":false,"size":0,"value":1,"wide":false,"wide_value":"0x1"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"N_READ_REGISTERS":{"name":"N_READ_REGISTERS","type":"numeric_parameter","value":[7]},"N_TRIGGER_REGISTERS":{"name":"N_TRIGGER_REGISTERS","type":"numeric_parameter","value":[1]},"N_WRITE_REGISTERS":{"name":"N_WRITE_REGISTERS","type":"numeric_parameter","value":[7]},"REGISTERED_BUFFERS":{"name":"REGISTERED_BUFFERS","type":"numeric_parameter","value":[0]},"REGISTERS_WIDTH":{"name":"REGISTERS_WIDTH","type":"numeric_parameter","value":[32]},"TRIGGER_REGISTERS_IDX":{"name":"TRIGGER_REGISTERS_IDX","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":3,"wide":false,"wide_value":"0x3"}]]]}},"ports_map":{"axil":["axi_in"],"clock":["clock"],"input_registers":["cu_read_registers"],"output_registers":["cu_write_registers"],"reset":["reset"],"trigger_out":["trigger_transfer"]},"wildcard_assignment":false}],"instance_name":"SCU","instance_type":"SpiControlUnit","parameters":{"FIXED_REGISTER_VALUES":{"name":"FIXED_REGISTER_VALUES","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":224,"wide":false,"wide_value":"0xe0"},{"signedness":false,"size":0,"value":1,"wide":false,"wide_value":"0x1"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"INITIAL_REGISTER_VALUES":{"name":"INITIAL_REGISTER_VALUES","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":224,"wide":false,"wide_value":"0xe0"},{"signedness":false,"size":0,"value":1,"wide":false,"wide_value":"0x1"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[3]},"N_REGISTERS":{"name":"N_REGISTERS","type":"numeric_parameter","value":[7]},"OUTPUT_WIDTH":{"name":"OUTPUT_WIDTH","type":"numeric_parameter","value":[32]},"SS_POLARITY_DEFAULT":{"name":"SS_POLARITY_DEFAULT","type":"numeric_parameter","value":[0]},"TRIGGER_REGISTERS_IDX":{"name":"TRIGGER_REGISTERS_IDX","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":3,"wide":false,"wide_value":"0x3"}]]]},"VARIABLE_INITIAL_VALUES":{"name":"VARIABLE_INITIAL_VALUES","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]}},"ports_map":{"axi_in":["axi_in"],"clock":["clock"],"clock_polarity":["clock_polarity"],"divider_setting":["divider_setting"],"external_spi_transfer":["external_spi_transfer"],"latching_edge":["latching_edge"],"period":["start_generator_period"],"reset":["reset"],"spi_data_in":["cu_in"],"spi_data_out":["cu_out"],"spi_delay":["spi_delay"],"spi_direction":["spi_direction"],"spi_mode":["spi_mode"],"spi_start_transfer":["int_transfer_start"],"spi_transfer_length":["bus_transfer_length"],"ss_deassert_delay_enable":["ss_deassert_delay_enable"],"ss_polarity":["ss_polarity"],"start_generator_enable":["start_generator_enable"],"transfer_done":["transfer_done"],"transfer_length_choice":["transfer_length_choice"]},"wildcard_assignment":false}],"instance_name":"master_module","instance_type":"SPI_master","parameters":{"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[3]},"OUTPUT_WIDTH":{"name":"OUTPUT_WIDTH","type":"numeric_parameter","value":[32]},"PRAGMA_MKFG_MODULE_TOP":{"name":"PRAGMA_MKFG_MODULE_TOP","type":"string_parameter","value":["SPI"]},"SS_POLARITY_DEFAULT":{"name":"SS_POLARITY_DEFAULT","type":"numeric_parameter","value":[0]}},"ports_map":{"MISO":["MISO"],"MOSI":["MOSI"],"SCLK":["SCLK"],"SS":["SS"],"axi_in":["axi_in"],"clock":["clock"],"data_out":["data_out"],"data_valid":["data_valid"],"external_spi_transfer":["external_spi_transfer"],"external_transfer_length":["external_transfer_length"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"TL","instance_type":"SPI","parameters":{"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[3]},"OUTPUT_WIDTH":{"name":"OUTPUT_WIDTH","type":"numeric_parameter","value":[32]},"PRAGMA_MKFG_MODULE_TOP":{"name":"PRAGMA_MKFG_MODULE_TOP","type":"string_parameter","value":["SPI"]},"SPI_MODE":{"name":"SPI_MODE","type":"string_parameter","value":["MASTER"]},"SS_POLARITY_DEFAULT":{"name":"SS_POLARITY_DEFAULT","type":"numeric_parameter","value":[0]}},"ports_map":{},"wildcard_assignment":false})");

    ASSERT_EQ(ast_dump, check_obj);

}


TEST( hdl_ast_builder, pwm_ast_build) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    std::vector<std::string> paths = {
            "Components/Common",
            "Components/controls/PwmGenerator/rtl",
            "Components/system/axi_lite/crossbar/rtl/",
            "Components/system/axi_lite/external_registers_cu/rtl",
            "Components/system/axi_lite/skid_buffer/rtl"
    };

    auto prefix = td_file("check_files/test_data/");
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv" || f.path().extension() == ".svh"){
                auto test_file = mm_file(f.path());
                sv_analyzer analyzer;

                auto resources = analyzer.analyze("", test_file.view()).value();
                d_store->store_file({f.path(), "file_hash", resources});
            }
        }
    }


    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast_v2 = b.build_ast(std::vector<std::string>({"PwmGenerator"}))[0];

    auto struct_s = ast_v2->dump_structure();
    auto reference_structure = "TL:PwmGenerator\n    internal_bus:axi_stream\n    read_req:axi_stream\n    read_resp:axi_stream\n    write_req:axi_stream\n    CU:axil_external_registers_cu\n        address_read_buffer:axil_skid_buffer\n        address_write_buffer:axil_skid_buffer\n        write_data_buffer:axil_skid_buffer\n    SyncEngine:SyncEngine\n    pwm_cu:PwmControlUnit\n    timebase_generator:TimebaseGenerator\n    chain:pwmChain\n        ControlUnit:ChainControlUnit\n        counter:Counter\n        core:counter_core\n        compare:CompareUnit\n        pinControl:PinControl\n        pinControl:PinControl\n        pinControl:PinControl\n        pinControl:PinControl\n        deadtime_gen:DeadTimeGenerator\n        deadTime_a:timebase_shifter_core\n        deadtime_gen:DeadTimeGenerator\n        deadTime_a:timebase_shifter_core\n        deadtime_gen:DeadTimeGenerator\n        deadTime_a:timebase_shifter_core\n        deadtime_gen:DeadTimeGenerator\n        deadTime_a:timebase_shifter_core\n        res_enhancer_a:ResolutionEnhancer\n        res_enhancer_a:ResolutionEnhancer\n        res_enhancer_a:ResolutionEnhancer\n        res_enhancer_a:ResolutionEnhancer\n        res_enhancer_b:ResolutionEnhancer\n        res_enhancer_b:ResolutionEnhancer\n        res_enhancer_b:ResolutionEnhancer\n        res_enhancer_b:ResolutionEnhancer\n    chain:pwmChain\n        ControlUnit:ChainControlUnit\n        counter:Counter\n        core:counter_core\n        compare:CompareUnit\n        pinControl:PinControl\n        pinControl:PinControl\n        pinControl:PinControl\n        pinControl:PinControl\n        deadtime_gen:DeadTimeGenerator\n        deadTime_a:timebase_shifter_core\n        deadtime_gen:DeadTimeGenerator\n        deadTime_a:timebase_shifter_core\n        deadtime_gen:DeadTimeGenerator\n        deadTime_a:timebase_shifter_core\n        deadtime_gen:DeadTimeGenerator\n        deadTime_a:timebase_shifter_core\n        res_enhancer_a:ResolutionEnhancer\n        res_enhancer_a:ResolutionEnhancer\n        res_enhancer_a:ResolutionEnhancer\n        res_enhancer_a:ResolutionEnhancer\n        res_enhancer_b:ResolutionEnhancer\n        res_enhancer_b:ResolutionEnhancer\n        res_enhancer_b:ResolutionEnhancer\n        res_enhancer_b:ResolutionEnhancer\n";

    EXPECT_EQ(struct_s, reference_structure);

    auto ast_dump = ast_v2->dump();

    std::string res_str = R"lit({"children":[{"children":[],"instance_name":"internal_bus","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[],"instance_name":"read_req","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[],"instance_name":"read_resp","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[],"instance_name":"write_req","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"address_read_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axi_in.ARADDR"],"in_ready":["axi_in.ARREADY"],"in_valid":["axi_in.ARVALID"],"out_data":["internal_read_address"],"out_ready":["read_ready"],"out_valid":["read_address_valid"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"address_write_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axi_in.AWADDR"],"in_ready":["axi_in.AWREADY"],"in_valid":["axi_in.AWVALID"],"out_data":["write_address"],"out_ready":["write_ready"],"out_valid":["write_address_valid"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"write_data_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[36]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axi_in.WDATA","axi_in.WSTRB"],"in_ready":["axi_in.WREADY"],"in_valid":["axi_in.WVALID"],"out_data":["internal_write_data","write_strobe"],"out_ready":["write_ready"],"out_valid":["write_data_valid"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"CU","instance_type":"axil_external_registers_cu","parameters":{"BASE_ADDRESS":{"name":"BASE_ADDRESS","type":"numeric_parameter","value":[0]},"READ_DELAY":{"name":"READ_DELAY","type":"numeric_parameter","value":[1]},"REGISTERED_BUFFERS":{"name":"REGISTERED_BUFFERS","type":"numeric_parameter","value":[0]},"REGISTERS_WIDTH":{"name":"REGISTERS_WIDTH","type":"numeric_parameter","value":[32]},"TRANSPARENT_MODE":{"name":"TRANSPARENT_MODE","type":"string_parameter","value":["TRUE"]}},"ports_map":{"axi_in":["axi_in"],"clock":["clock"],"read_address":["read_req"],"read_data":["read_resp"],"reset":["reset"],"write_data":["write_req"]},"wildcard_assignment":false},{"children":[],"instance_name":"SyncEngine","instance_type":"SyncEngine","parameters":{"N_CHAINS":{"name":"N_CHAINS","type":"numeric_parameter","value":[2]}},"ports_map":{"clock":["clock"],"enable":["counter_run"],"sync_delay":["sync_out_delay"],"sync_in":["chains_sync_out"],"sync_out":["sync_out"],"sync_select":["sync_out_select"]},"wildcard_assignment":false},{"children":[],"instance_name":"pwm_cu","instance_type":"PwmControlUnit","parameters":{"INITIAL_STOPPED_STATE":{"name":"INITIAL_STOPPED_STATE","type":"numeric_parameter","value":[0]},"N_PWM":{"name":"N_PWM","type":"numeric_parameter","value":[16]}},"ports_map":{"clock":["clock"],"counter_run":["counter_run"],"counter_status":["counter_status"],"counter_stopped_state":["counter_stopped_state"],"modulation_in":["internal_bus"],"read_response":["read_global_data"],"reset":["reset"],"sync":["sync"],"sync_out_delay":["sync_out_delay"],"sync_out_select":["sync_out_select"],"timebase_enable":["timebase_enable"],"timebase_external_enable":["ext_timebase_enable"],"timebase_setting":["dividerSetting"]},"wildcard_assignment":false},{"children":[],"instance_name":"timebase_generator","instance_type":"TimebaseGenerator","parameters":{},"ports_map":{"clock":["clock"],"counter_status":["counter_run"],"dividerSetting":["dividerSetting"],"enable":["timebase_enable"],"fast_count":["fast_count"],"reset":["reset"],"timebaseOut":["internal_timebase"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"ControlUnit","instance_type":"ChainControlUnit","parameters":{"CHAIN_ADDRESS":{"name":"CHAIN_ADDRESS","type":"numeric_parameter","value":[1]},"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"DT_IV":{"name":"DT_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"}]]]},"INITIAL_REGISTER_VALUES":{"name":"INITIAL_REGISTER_VALUES","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"OTHER_IV":{"name":"OTHER_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"THRESH_HIGH_IV":{"name":"THRESH_HIGH_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"}]]]},"THRESH_LOW_IV":{"name":"THRESH_LOW_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]}},"ports_map":{"clock":["clock"],"comparator_tresholds":["compare_tresholds"],"counter_mode":["counter_mode"],"counter_running":["counter_stopped"],"counter_start_data":["counter_start_data"],"counter_stop_data":["counter_stop_data"],"deadtime":["deadtime"],"deadtime_enable":["deadtime_enable"],"modulation_in":["modulation_in"],"output_enable":["output_enable"],"read_response":["read_response"],"reset":["reset"],"timebase_shift":["timebase_shift"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"core","instance_type":"counter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clock":["clock"],"count_out":["counter"],"direction":["direction"],"enable":["counter_enable"],"fast_count":["fast_count"],"inhibit_load":["mode[1]"],"reload_value":["cnt_stopValue"],"reset":["reset","sync"],"shift":["shift"],"timebase":["timebase"]},"wildcard_assignment":false}],"instance_name":"counter","instance_type":"Counter","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clock":["clock"],"countOut":["counter_out"],"counter_start_data":["counter_start_data"],"counter_stop_data":["counter_stop_data"],"fast_count":["fast_count"],"mode":["counter_mode"],"reload_compare":["reload_compare"],"reset":["reset"],"run":["external_counter_run","stop_request"],"shift":["timebase_shift"],"sync":["sync"],"timebase":["timebase"]},"wildcard_assignment":false},{"children":[],"instance_name":"compare","instance_type":"CompareUnit","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]}},"ports_map":{"clock":["clock"],"comparator_tresholds":["compare_tresholds"],"counterValue":["counter_out"],"counter_stopped":["counter_stopped"],"matchHigh":["compare_match_high"],"matchLow":["compare_match_low"],"reload_compare":["reload_compare"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"pinControl","instance_type":"PinControl","parameters":{},"ports_map":{"clock":["clock"],"counter_stopped":["counter_stopped"],"enableOutputs":["output_enable[0]"],"matchHigh":["compare_match_high[0]"],"matchLow":["compare_match_low[0]"],"outA":["pin_out_a[0]"],"outB":["pin_out_b[0]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"pinControl","instance_type":"PinControl","parameters":{},"ports_map":{"clock":["clock"],"counter_stopped":["counter_stopped"],"enableOutputs":["output_enable[1]"],"matchHigh":["compare_match_high[1]"],"matchLow":["compare_match_low[1]"],"outA":["pin_out_a[1]"],"outB":["pin_out_b[1]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"pinControl","instance_type":"PinControl","parameters":{},"ports_map":{"clock":["clock"],"counter_stopped":["counter_stopped"],"enableOutputs":["output_enable[2]"],"matchHigh":["compare_match_high[2]"],"matchLow":["compare_match_low[2]"],"outA":["pin_out_a[2]"],"outB":["pin_out_b[2]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"pinControl","instance_type":"PinControl","parameters":{},"ports_map":{"clock":["clock"],"counter_stopped":["counter_stopped"],"enableOutputs":["output_enable[3]"],"matchHigh":["compare_match_high[3]"],"matchLow":["compare_match_low[3]"],"outA":["pin_out_a[3]"],"outB":["pin_out_b[3]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"deadTime_a","instance_type":"timebase_shifter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clockIn":["clock"],"count_in":["deadTime","1"],"count_out":["counter"],"enable":["counter_enable"],"load":["load_counter"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"deadtime_gen","instance_type":"DeadTimeGenerator","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"FIRST_DEADTIME_STATE":{"name":"FIRST_DEADTIME_STATE","type":"numeric_parameter","value":[1]},"FIRST_OUT":{"name":"FIRST_OUT","type":"numeric_parameter","value":[2]},"IDLE_STATE":{"name":"IDLE_STATE","type":"numeric_parameter","value":[0]},"SECOND_DEADTIME_STATE":{"name":"SECOND_DEADTIME_STATE","type":"numeric_parameter","value":[3]}},"ports_map":{"clock":["clock"],"deadTime":["deadtime[0]"],"enable":["deadtime_enable[0]"],"in_a":["pin_out_a[0]"],"in_b":["pin_out_b[0]"],"out_a":["dt_out_a[0]"],"out_b":["dt_out_b[0]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"deadTime_a","instance_type":"timebase_shifter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clockIn":["clock"],"count_in":["deadTime","1"],"count_out":["counter"],"enable":["counter_enable"],"load":["load_counter"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"deadtime_gen","instance_type":"DeadTimeGenerator","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"FIRST_DEADTIME_STATE":{"name":"FIRST_DEADTIME_STATE","type":"numeric_parameter","value":[1]},"FIRST_OUT":{"name":"FIRST_OUT","type":"numeric_parameter","value":[2]},"IDLE_STATE":{"name":"IDLE_STATE","type":"numeric_parameter","value":[0]},"SECOND_DEADTIME_STATE":{"name":"SECOND_DEADTIME_STATE","type":"numeric_parameter","value":[3]}},"ports_map":{"clock":["clock"],"deadTime":["deadtime[1]"],"enable":["deadtime_enable[1]"],"in_a":["pin_out_a[1]"],"in_b":["pin_out_b[1]"],"out_a":["dt_out_a[1]"],"out_b":["dt_out_b[1]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"deadTime_a","instance_type":"timebase_shifter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clockIn":["clock"],"count_in":["deadTime","1"],"count_out":["counter"],"enable":["counter_enable"],"load":["load_counter"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"deadtime_gen","instance_type":"DeadTimeGenerator","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"FIRST_DEADTIME_STATE":{"name":"FIRST_DEADTIME_STATE","type":"numeric_parameter","value":[1]},"FIRST_OUT":{"name":"FIRST_OUT","type":"numeric_parameter","value":[2]},"IDLE_STATE":{"name":"IDLE_STATE","type":"numeric_parameter","value":[0]},"SECOND_DEADTIME_STATE":{"name":"SECOND_DEADTIME_STATE","type":"numeric_parameter","value":[3]}},"ports_map":{"clock":["clock"],"deadTime":["deadtime[2]"],"enable":["deadtime_enable[2]"],"in_a":["pin_out_a[2]"],"in_b":["pin_out_b[2]"],"out_a":["dt_out_a[2]"],"out_b":["dt_out_b[2]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"deadTime_a","instance_type":"timebase_shifter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clockIn":["clock"],"count_in":["deadTime","1"],"count_out":["counter"],"enable":["counter_enable"],"load":["load_counter"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"deadtime_gen","instance_type":"DeadTimeGenerator","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"FIRST_DEADTIME_STATE":{"name":"FIRST_DEADTIME_STATE","type":"numeric_parameter","value":[1]},"FIRST_OUT":{"name":"FIRST_OUT","type":"numeric_parameter","value":[2]},"IDLE_STATE":{"name":"IDLE_STATE","type":"numeric_parameter","value":[0]},"SECOND_DEADTIME_STATE":{"name":"SECOND_DEADTIME_STATE","type":"numeric_parameter","value":[3]}},"ports_map":{"clock":["clock"],"deadTime":["deadtime[3]"],"enable":["deadtime_enable[3]"],"in_a":["pin_out_a[3]"],"in_b":["pin_out_b[3]"],"out_a":["dt_out_a[3]"],"out_b":["dt_out_b[3]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_a","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_a[0]"],"out":["out_a[0]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_a","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_a[1]"],"out":["out_a[1]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_a","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_a[2]"],"out":["out_a[2]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_a","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_a[3]"],"out":["out_a[3]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_b","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_b[0]"],"out":["out_b[0]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_b","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_b[1]"],"out":["out_b[1]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_b","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_b[2]"],"out":["out_b[2]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_b","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_b[3]"],"out":["out_b[3]"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"chain","instance_type":"pwmChain","parameters":{"CHAIN_ADDRESS":{"name":"CHAIN_ADDRESS","type":"numeric_parameter","value":[1]},"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]},"HR_ENABLE":{"name":"HR_ENABLE","type":"string_parameter","value":["FALSE"]},"N_CHAINS":{"name":"N_CHAINS","type":"numeric_parameter","value":[2]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"PRAGMA_MKFG_MODULE_TOP":{"name":"PRAGMA_MKFG_MODULE_TOP","type":"string_parameter","value":["pwmChain"]}},"ports_map":{"clock":["clock"],"counter_status":["counter_status[0]"],"external_counter_run":["counter_run"],"fast_count":["fast_count"],"high_resolution_clock":["high_resolution_clock"],"modulation_in":["internal_bus"],"out_a":["partial_pwm_out_a[0]"],"out_b":["partial_pwm_out_b[0]"],"read_response":["read_chain_data[0]"],"reset":["reset"],"stop_request":["stop_chain[0]"],"sync":["sync"],"sync_out":["chains_sync_out[0]"],"timebase":["selected_timebase"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"ControlUnit","instance_type":"ChainControlUnit","parameters":{"CHAIN_ADDRESS":{"name":"CHAIN_ADDRESS","type":"numeric_parameter","value":[2]},"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"DT_IV":{"name":"DT_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"}]]]},"INITIAL_REGISTER_VALUES":{"name":"INITIAL_REGISTER_VALUES","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":80,"wide":false,"wide_value":"0x50"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"OTHER_IV":{"name":"OTHER_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"THRESH_HIGH_IV":{"name":"THRESH_HIGH_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"},{"signedness":false,"size":0,"value":4294967295,"wide":false,"wide_value":"0xffffffff"}]]]},"THRESH_LOW_IV":{"name":"THRESH_LOW_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]}},"ports_map":{"clock":["clock"],"comparator_tresholds":["compare_tresholds"],"counter_mode":["counter_mode"],"counter_running":["counter_stopped"],"counter_start_data":["counter_start_data"],"counter_stop_data":["counter_stop_data"],"deadtime":["deadtime"],"deadtime_enable":["deadtime_enable"],"modulation_in":["modulation_in"],"output_enable":["output_enable"],"read_response":["read_response"],"reset":["reset"],"timebase_shift":["timebase_shift"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"core","instance_type":"counter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clock":["clock"],"count_out":["counter"],"direction":["direction"],"enable":["counter_enable"],"fast_count":["fast_count"],"inhibit_load":["mode[1]"],"reload_value":["cnt_stopValue"],"reset":["reset","sync"],"shift":["shift"],"timebase":["timebase"]},"wildcard_assignment":false}],"instance_name":"counter","instance_type":"Counter","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clock":["clock"],"countOut":["counter_out"],"counter_start_data":["counter_start_data"],"counter_stop_data":["counter_stop_data"],"fast_count":["fast_count"],"mode":["counter_mode"],"reload_compare":["reload_compare"],"reset":["reset"],"run":["external_counter_run","stop_request"],"shift":["timebase_shift"],"sync":["sync"],"timebase":["timebase"]},"wildcard_assignment":false},{"children":[],"instance_name":"compare","instance_type":"CompareUnit","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]}},"ports_map":{"clock":["clock"],"comparator_tresholds":["compare_tresholds"],"counterValue":["counter_out"],"counter_stopped":["counter_stopped"],"matchHigh":["compare_match_high"],"matchLow":["compare_match_low"],"reload_compare":["reload_compare"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"pinControl","instance_type":"PinControl","parameters":{},"ports_map":{"clock":["clock"],"counter_stopped":["counter_stopped"],"enableOutputs":["output_enable[0]"],"matchHigh":["compare_match_high[0]"],"matchLow":["compare_match_low[0]"],"outA":["pin_out_a[0]"],"outB":["pin_out_b[0]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"pinControl","instance_type":"PinControl","parameters":{},"ports_map":{"clock":["clock"],"counter_stopped":["counter_stopped"],"enableOutputs":["output_enable[1]"],"matchHigh":["compare_match_high[1]"],"matchLow":["compare_match_low[1]"],"outA":["pin_out_a[1]"],"outB":["pin_out_b[1]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"pinControl","instance_type":"PinControl","parameters":{},"ports_map":{"clock":["clock"],"counter_stopped":["counter_stopped"],"enableOutputs":["output_enable[2]"],"matchHigh":["compare_match_high[2]"],"matchLow":["compare_match_low[2]"],"outA":["pin_out_a[2]"],"outB":["pin_out_b[2]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"pinControl","instance_type":"PinControl","parameters":{},"ports_map":{"clock":["clock"],"counter_stopped":["counter_stopped"],"enableOutputs":["output_enable[3]"],"matchHigh":["compare_match_high[3]"],"matchLow":["compare_match_low[3]"],"outA":["pin_out_a[3]"],"outB":["pin_out_b[3]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"deadTime_a","instance_type":"timebase_shifter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clockIn":["clock"],"count_in":["deadTime","1"],"count_out":["counter"],"enable":["counter_enable"],"load":["load_counter"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"deadtime_gen","instance_type":"DeadTimeGenerator","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"FIRST_DEADTIME_STATE":{"name":"FIRST_DEADTIME_STATE","type":"numeric_parameter","value":[1]},"FIRST_OUT":{"name":"FIRST_OUT","type":"numeric_parameter","value":[2]},"IDLE_STATE":{"name":"IDLE_STATE","type":"numeric_parameter","value":[0]},"SECOND_DEADTIME_STATE":{"name":"SECOND_DEADTIME_STATE","type":"numeric_parameter","value":[3]}},"ports_map":{"clock":["clock"],"deadTime":["deadtime[0]"],"enable":["deadtime_enable[0]"],"in_a":["pin_out_a[0]"],"in_b":["pin_out_b[0]"],"out_a":["dt_out_a[0]"],"out_b":["dt_out_b[0]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"deadTime_a","instance_type":"timebase_shifter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clockIn":["clock"],"count_in":["deadTime","1"],"count_out":["counter"],"enable":["counter_enable"],"load":["load_counter"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"deadtime_gen","instance_type":"DeadTimeGenerator","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"FIRST_DEADTIME_STATE":{"name":"FIRST_DEADTIME_STATE","type":"numeric_parameter","value":[1]},"FIRST_OUT":{"name":"FIRST_OUT","type":"numeric_parameter","value":[2]},"IDLE_STATE":{"name":"IDLE_STATE","type":"numeric_parameter","value":[0]},"SECOND_DEADTIME_STATE":{"name":"SECOND_DEADTIME_STATE","type":"numeric_parameter","value":[3]}},"ports_map":{"clock":["clock"],"deadTime":["deadtime[1]"],"enable":["deadtime_enable[1]"],"in_a":["pin_out_a[1]"],"in_b":["pin_out_b[1]"],"out_a":["dt_out_a[1]"],"out_b":["dt_out_b[1]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"deadTime_a","instance_type":"timebase_shifter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clockIn":["clock"],"count_in":["deadTime","1"],"count_out":["counter"],"enable":["counter_enable"],"load":["load_counter"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"deadtime_gen","instance_type":"DeadTimeGenerator","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"FIRST_DEADTIME_STATE":{"name":"FIRST_DEADTIME_STATE","type":"numeric_parameter","value":[1]},"FIRST_OUT":{"name":"FIRST_OUT","type":"numeric_parameter","value":[2]},"IDLE_STATE":{"name":"IDLE_STATE","type":"numeric_parameter","value":[0]},"SECOND_DEADTIME_STATE":{"name":"SECOND_DEADTIME_STATE","type":"numeric_parameter","value":[3]}},"ports_map":{"clock":["clock"],"deadTime":["deadtime[2]"],"enable":["deadtime_enable[2]"],"in_a":["pin_out_a[2]"],"in_b":["pin_out_b[2]"],"out_a":["dt_out_a[2]"],"out_b":["dt_out_b[2]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"deadTime_a","instance_type":"timebase_shifter_core","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clockIn":["clock"],"count_in":["deadTime","1"],"count_out":["counter"],"enable":["counter_enable"],"load":["load_counter"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"deadtime_gen","instance_type":"DeadTimeGenerator","parameters":{"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"FIRST_DEADTIME_STATE":{"name":"FIRST_DEADTIME_STATE","type":"numeric_parameter","value":[1]},"FIRST_OUT":{"name":"FIRST_OUT","type":"numeric_parameter","value":[2]},"IDLE_STATE":{"name":"IDLE_STATE","type":"numeric_parameter","value":[0]},"SECOND_DEADTIME_STATE":{"name":"SECOND_DEADTIME_STATE","type":"numeric_parameter","value":[3]}},"ports_map":{"clock":["clock"],"deadTime":["deadtime[3]"],"enable":["deadtime_enable[3]"],"in_a":["pin_out_a[3]"],"in_b":["pin_out_b[3]"],"out_a":["dt_out_a[3]"],"out_b":["dt_out_b[3]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_a","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_a[0]"],"out":["out_a[0]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_a","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_a[1]"],"out":["out_a[1]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_a","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_a[2]"],"out":["out_a[2]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_a","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_a[3]"],"out":["out_a[3]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_b","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_b[0]"],"out":["out_b[0]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_b","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_b[1]"],"out":["out_b[1]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_b","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_b[2]"],"out":["out_b[2]"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"res_enhancer_b","instance_type":"ResolutionEnhancer","parameters":{"ENABLE":{"name":"ENABLE","type":"string_parameter","value":["FALSE"]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]}},"ports_map":{"clock":["clock"],"count":["timebase_shift[2:0]"],"high_resolution_clock":["high_resolution_clock"],"in":["dt_out_b[3]"],"out":["out_b[3]"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"chain","instance_type":"pwmChain","parameters":{"CHAIN_ADDRESS":{"name":"CHAIN_ADDRESS","type":"numeric_parameter","value":[2]},"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]},"HR_ENABLE":{"name":"HR_ENABLE","type":"string_parameter","value":["FALSE"]},"N_CHAINS":{"name":"N_CHAINS","type":"numeric_parameter","value":[2]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"PRAGMA_MKFG_MODULE_TOP":{"name":"PRAGMA_MKFG_MODULE_TOP","type":"string_parameter","value":["pwmChain"]}},"ports_map":{"clock":["clock"],"counter_status":["counter_status[1]"],"external_counter_run":["counter_run"],"fast_count":["fast_count"],"high_resolution_clock":["high_resolution_clock"],"modulation_in":["internal_bus"],"out_a":["partial_pwm_out_a[1]"],"out_b":["partial_pwm_out_b[1]"],"read_response":["read_chain_data[1]"],"reset":["reset"],"stop_request":["stop_chain[1]"],"sync":["sync"],"sync_out":["chains_sync_out[1]"],"timebase":["selected_timebase"]},"wildcard_assignment":false}],"instance_name":"TL","instance_type":"PwmGenerator","parameters":{"BASE_ADDRESS":{"name":"BASE_ADDRESS","type":"numeric_parameter","value":[1136656384]},"COUNTER_WIDTH":{"name":"COUNTER_WIDTH","type":"numeric_parameter","value":[16]},"ENANCING_MODE":{"name":"ENANCING_MODE","type":"string_parameter","value":["DUTY"]},"HR_ENABLE":{"name":"HR_ENABLE","type":"string_parameter","value":["FALSE"]},"INITIAL_STOPPED_STATE":{"name":"INITIAL_STOPPED_STATE","type":"numeric_parameter","value":[0]},"N_CHAINS":{"name":"N_CHAINS","type":"numeric_parameter","value":[2]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"N_PWM":{"name":"N_PWM","type":"numeric_parameter","value":[16]},"PRAGMA_MKFG_MODULE_TOP":{"name":"PRAGMA_MKFG_MODULE_TOP","type":"string_parameter","value":["PwmGenerator"]}},"ports_map":{},"wildcard_assignment":false})lit";
    nlohmann::json check_obj = nlohmann::json::parse(res_str);

    ASSERT_EQ(ast_dump, check_obj);
}


TEST( hdl_ast_builder, adc_ast_build) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    std::vector<std::string> paths = {
            "Components/Common",
            "Components/signal_chain/AdcProcessing/rtl",
            "Components/signal_chain/decimator/standard_decimator",
            "Components/signal_chain/fir_filter/rtl",
            "Components/system/axi_lite/simple_register_cu/rtl",
            "Components/system/axi_lite/skid_buffer/rtl"
    };

    auto prefix = td_file("check_files/test_data/");
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv" || f.path().extension() == ".svh"){

                auto test_file = mm_file(f.path());
                sv_analyzer analyzer;

                auto resources = analyzer.analyze("", test_file.view()).value();
                d_store->store_file({f.path(), "file_hash", resources});
            }
        }
    }


    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto synth_ast = b.build_ast(std::vector<std::string>({"AdcProcessing"}))[0];

    auto struct_s = synth_ast->dump_structure();
    auto reference_structure = "TL:AdcProcessing\n    AdcCU:AdcProcessingControlUnit\n        CU:axil_simple_register_cu\n        address_read_buffer:axil_skid_buffer\n        address_write_buffer:axil_skid_buffer\n        write_data_buffer:axil_skid_buffer\n    fast_cmp:comparator\n    cal_out:axi_stream\n    calibrator:calibration\n        offset_adder:saturating_adder\n    denoise_out:axi_stream\n    denoise:denoiser\n    lin_out:axi_stream\n    linearizer:linearizer\n    dec:standard_decimator\n    raw_filtered_out:axi_stream\n    filter:fir_filter_serial\n        tap_memory:DP_RAM\n        dl:DP_RAM\n    dec:standard_decimator\n    slow_cmp_in:axi_stream\n    slow_cmp:comparator\n";

    EXPECT_EQ(struct_s, reference_structure);

    auto ast_dump = synth_ast->dump();

    std::string res_str = R"lit({"children":[{"children":[{"children":[{"children":[],"instance_name":"address_read_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axil.ARADDR"],"in_ready":["axil.ARREADY"],"in_valid":["axil.ARVALID"],"out_data":["read_address"],"out_ready":["read_ready"],"out_valid":["read_address_valid"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"address_write_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axil.AWADDR"],"in_ready":["axil.AWREADY"],"in_valid":["axil.AWVALID"],"out_data":["write_address"],"out_ready":["write_ready"],"out_valid":["write_address_valid"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"write_data_buffer","instance_type":"axil_skid_buffer","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[36]},"REGISTER_OUTPUT":{"name":"REGISTER_OUTPUT","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"in_data":["axil.WDATA","axil.WSTRB"],"in_ready":["axil.WREADY"],"in_valid":["axil.WVALID"],"out_data":["write_data","write_strobe"],"out_ready":["write_ready"],"out_valid":["write_data_valid"],"reset":["reset"]},"wildcard_assignment":false}],"instance_name":"CU","instance_type":"axil_simple_register_cu","parameters":{"ADDRESS_MASK":{"name":"ADDRESS_MASK","type":"numeric_parameter","value":[255]},"INITIAL_OUTPUT_VALUES":{"name":"INITIAL_OUTPUT_VALUES","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"N_READ_REGISTERS":{"name":"N_READ_REGISTERS","type":"numeric_parameter","value":[16]},"N_TRIGGER_REGISTERS":{"name":"N_TRIGGER_REGISTERS","type":"numeric_parameter","value":[1]},"N_WRITE_REGISTERS":{"name":"N_WRITE_REGISTERS","type":"numeric_parameter","value":[16]},"REGISTERED_BUFFERS":{"name":"REGISTERED_BUFFERS","type":"numeric_parameter","value":[0]},"REGISTERS_WIDTH":{"name":"REGISTERS_WIDTH","type":"numeric_parameter","value":[32]},"TRIGGER_REGISTERS_IDX":{"name":"TRIGGER_REGISTERS_IDX","type":"array_parameter","value":[[[{"signedness":false,"size":32,"value":15,"wide":false,"wide_value":"0xf"}]]]}},"ports_map":{"axil":["axi_in"],"clock":["clock"],"input_registers":["cu_read_registers"],"output_registers":["cu_write_registers"],"reset":["reset"],"trigger_out":["taps_we"]},"wildcard_assignment":false}],"instance_name":"AdcCU","instance_type":"AdcProcessingControlUnit","parameters":{"COMPARE_IV":{"name":"COMPARE_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"}]]]},"CONTROL_REG":{"name":"CONTROL_REG","type":"numeric_parameter","value":[13]},"DATA_PATH_WIDTH":{"name":"DATA_PATH_WIDTH","type":"numeric_parameter","value":[16]},"DENOISING":{"name":"DENOISING","type":"numeric_parameter","value":[0]},"FLTER_TAP_WIDTH":{"name":"FLTER_TAP_WIDTH","type":"numeric_parameter","value":[16]},"IV":{"name":"IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":4294901760,"wide":false,"wide_value":"0xffff0000"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"N_COMPARE_REGS":{"name":"N_COMPARE_REGS","type":"numeric_parameter","value":[4]},"N_DENOISE_REGS":{"name":"N_DENOISE_REGS","type":"numeric_parameter","value":[4]},"N_OFFSET_REGS":{"name":"N_OFFSET_REGS","type":"numeric_parameter","value":[4]},"N_REGISTERS":{"name":"N_REGISTERS","type":"numeric_parameter","value":[16]},"N_SHIFT_REGS":{"name":"N_SHIFT_REGS","type":"numeric_parameter","value":[1]},"N_ZERO_IV":{"name":"N_ZERO_IV","type":"numeric_parameter","value":[12]},"STICKY_FAULT":{"name":"STICKY_FAULT","type":"numeric_parameter","value":[0]},"TAP_ADDR_REG":{"name":"TAP_ADDR_REG","type":"numeric_parameter","value":[15]},"TAP_DATA_REG":{"name":"TAP_DATA_REG","type":"numeric_parameter","value":[14]},"ZERO_IV":{"name":"ZERO_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]}},"ports_map":{"axi_in":["axi_in"],"clear_latch":["clear_latch"],"clock":["clock"],"comparator_thresholds":["comparator_thresholds"],"data_in_valid":["data_in.valid"],"decimation_ratio":["decimation_ratio"],"denoise_enable":["denoise_enable"],"denoise_tresh_n":["denoise_tresh_n"],"denoise_tresh_p":["denoise_tresh_p"],"fault":["fault"],"latch_mode":["latch_mode"],"linearizer_enable":["linearizer_enable"],"n_taps":["n_taps"],"offset":["offset"],"reset":["reset"],"shift":["shift"],"shift_enable":["shift_enable"],"taps_addr":["taps_addr"],"taps_data":["taps_data"],"taps_we":["taps_we"],"trip_high":["trip_high"],"trip_low":["trip_low"]},"wildcard_assignment":false},{"children":[],"instance_name":"fast_cmp","instance_type":"comparator","parameters":{"DATA_PATH_WIDTH":{"name":"DATA_PATH_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clear_latch":["clear_latch[0]"],"clock":["clock"],"data_in":["data_in"],"latching_mode":["latch_mode[0]"],"reset":["reset"],"thresholds":["comparator_thresholds[0:3]"],"trip_high":["trip_high[0]"],"trip_low":["trip_low[0]"]},"wildcard_assignment":false},{"children":[],"instance_name":"cal_out","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"offset_adder","instance_type":"saturating_adder","parameters":{"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"a":["data_in.data"],"b":["offset[data_in.dest-DATA_BLOCK_BASE_ADDR]"],"out":["raw_data_out"],"satn":["OUTPUT_SIGNED[data_in.dest-DATA_BLOCK_BASE_ADDR]",""],"satp":["1'b0",""]},"wildcard_assignment":false}],"instance_name":"calibrator","instance_type":"calibration","parameters":{"DATA_BLOCK_BASE_ADDR":{"name":"DATA_BLOCK_BASE_ADDR","type":"numeric_parameter","value":[0]},"DATA_PATH_WIDTH":{"name":"DATA_PATH_WIDTH","type":"numeric_parameter","value":[16]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"OUTPUT_SIGNED":{"name":"OUTPUT_SIGNED","type":"numeric_parameter","value":[15]}},"ports_map":{"clock":["clock"],"data_in":["data_in"],"data_out":["cal_out"],"offset":["offset"],"reset":["reset"],"shift":["shift"],"shift_enable":["shift_enable"]},"wildcard_assignment":false},{"children":[],"instance_name":"denoise_out","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[],"instance_name":"denoise","instance_type":"denoiser","parameters":{"DATA_BLOCK_BASE_ADDR":{"name":"DATA_BLOCK_BASE_ADDR","type":"numeric_parameter","value":[0]},"DATA_PATH_WIDTH":{"name":"DATA_PATH_WIDTH","type":"numeric_parameter","value":[16]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]}},"ports_map":{"clock":["clock"],"data_in":["cal_out"],"data_out":["denoise_out"],"enable":["denoise_enable"],"reset":["reset"],"thresh_n":["denoise_tresh_n"],"thresh_p":["denoise_tresh_p"]},"wildcard_assignment":false},{"children":[],"instance_name":"lin_out","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[],"instance_name":"linearizer","instance_type":"linearizer","parameters":{"BOUNDS":{"name":"BOUNDS","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"DATA_BLOCK_BASE_ADDR":{"name":"DATA_BLOCK_BASE_ADDR","type":"numeric_parameter","value":[0]},"DATA_PATH_WIDTH":{"name":"DATA_PATH_WIDTH","type":"numeric_parameter","value":[16]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[8]},"GAINS":{"name":"GAINS","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"N_SEGMENTS":{"name":"N_SEGMENTS","type":"numeric_parameter","value":[4]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clock":["clock"],"data_in":["denoise_out"],"data_out":["lin_out"],"enable":["linearizer_enable"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"dec","instance_type":"standard_decimator","parameters":{"AVERAGING":{"name":"AVERAGING","type":"numeric_parameter","value":[0]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]},"MAX_DECIMATION_RATIO":{"name":"MAX_DECIMATION_RATIO","type":"numeric_parameter","value":[16]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"SOURCE_DEST":{"name":"SOURCE_DEST","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"data_in":["lin_out"],"data_out":["filtered_data_out"],"decimation_ratio":["decimation_ratio"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"raw_filtered_out","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[{"children":[],"instance_name":"tap_memory","instance_type":"DP_RAM","parameters":{"ADDR_WIDTH":{"name":"ADDR_WIDTH","type":"numeric_parameter","value":[8]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]},"INIT_LEN":{"name":"INIT_LEN","type":"numeric_parameter","value":[257]},"MEM_INIT":{"name":"MEM_INIT","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]}},"ports_map":{"addr_a":["tap_addr"],"addr_b":["tap_counter"],"clk":["clock"],"data_a":["scaled_tap_data"],"data_b":["current_tap"],"en_b":["1"],"we_a":["tap_write"]},"wildcard_assignment":false},{"children":[],"instance_name":"dl","instance_type":"DP_RAM","parameters":{"ADDR_WIDTH":{"name":"ADDR_WIDTH","type":"numeric_parameter","value":[8]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]},"INIT_LEN":{"name":"INIT_LEN","type":"numeric_parameter","value":[1]},"MEM_INIT":{"name":"MEM_INIT","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]}},"ports_map":{"addr_a":["dl_write_addr"],"addr_b":["dl_read_addr"],"clk":["clock"],"data_a":["scaled_data_in"],"data_b":["dl_out"],"en_b":["1"],"we_a":["data_in.valid"]},"wildcard_assignment":false}],"instance_name":"filter","instance_type":"fir_filter_serial","parameters":{"DATA_PATH_WIDTH":{"name":"DATA_PATH_WIDTH","type":"numeric_parameter","value":[16]},"MAX_N_TAPS":{"name":"MAX_N_TAPS","type":"numeric_parameter","value":[256]},"TAPS_IV":{"name":"TAPS_IV","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"TAP_WIDTH":{"name":"TAP_WIDTH","type":"numeric_parameter","value":[16]},"WORKING_WIDTH":{"name":"WORKING_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clock":["clock"],"data_in":["lin_out"],"data_out":["raw_filtered_out"],"n_taps":["n_taps"],"reset":["reset"],"tap_addr":["taps_addr"],"tap_data":["taps_data"],"tap_write":["taps_we"]},"wildcard_assignment":false},{"children":[],"instance_name":"dec","instance_type":"standard_decimator","parameters":{"AVERAGING":{"name":"AVERAGING","type":"numeric_parameter","value":[0]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]},"MAX_DECIMATION_RATIO":{"name":"MAX_DECIMATION_RATIO","type":"numeric_parameter","value":[16]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"SOURCE_DEST":{"name":"SOURCE_DEST","type":"numeric_parameter","value":[0]}},"ports_map":{"clock":["clock"],"data_in":["raw_filtered_out"],"data_out":["filtered_data_out"],"decimation_ratio":["decimation_ratio"],"reset":["reset"]},"wildcard_assignment":false},{"children":[],"instance_name":"slow_cmp_in","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[16]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[],"instance_name":"slow_cmp","instance_type":"comparator","parameters":{"DATA_PATH_WIDTH":{"name":"DATA_PATH_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{"clear_latch":["clear_latch[1]"],"clock":["clock"],"data_in":["slow_cmp_in"],"latching_mode":["latch_mode[1]"],"reset":["reset"],"thresholds":["comparator_thresholds[4:7]"],"trip_high":["trip_high[1]"],"trip_low":["trip_low[1]"]},"wildcard_assignment":false}],"instance_name":"TL","instance_type":"AdcProcessing","parameters":{"DATA_BLOCK_BASE_ADDR":{"name":"DATA_BLOCK_BASE_ADDR","type":"numeric_parameter","value":[0]},"DATA_PATH_WIDTH":{"name":"DATA_PATH_WIDTH","type":"numeric_parameter","value":[16]},"DECIMATED":{"name":"DECIMATED","type":"numeric_parameter","value":[1]},"DENOISING":{"name":"DENOISING","type":"numeric_parameter","value":[0]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[8]},"ENABLE_AVERAGE":{"name":"ENABLE_AVERAGE","type":"numeric_parameter","value":[0]},"FAST_DATA_OFFSET":{"name":"FAST_DATA_OFFSET","type":"numeric_parameter","value":[0]},"FILTER_TAPS":{"name":"FILTER_TAPS","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"FILTER_WORKING_WIDTH":{"name":"FILTER_WORKING_WIDTH","type":"numeric_parameter","value":[16]},"FLTER_TAP_WIDTH":{"name":"FLTER_TAP_WIDTH","type":"numeric_parameter","value":[16]},"LINEARIZER_BOUNDS":{"name":"LINEARIZER_BOUNDS","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"LINEARIZER_GAINS":{"name":"LINEARIZER_GAINS","type":"array_parameter","value":[[[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}],[{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"},{"signedness":false,"size":0,"value":0,"wide":false,"wide_value":"0x0"}]]]},"LINEARIZER_SEGMENTS":{"name":"LINEARIZER_SEGMENTS","type":"numeric_parameter","value":[4]},"MAX_FILTER_TAPS":{"name":"MAX_FILTER_TAPS","type":"numeric_parameter","value":[256]},"N_CHANNELS":{"name":"N_CHANNELS","type":"numeric_parameter","value":[4]},"OUTPUT_SIGNED":{"name":"OUTPUT_SIGNED","type":"numeric_parameter","value":[15]},"PRAGMA_MKFG_DATAPOINT_NAMES":{"name":"PRAGMA_MKFG_DATAPOINT_NAMES","type":"string_parameter","value":[""]},"PRAGMA_MKFG_MODULE_TOP":{"name":"PRAGMA_MKFG_MODULE_TOP","type":"string_parameter","value":["AdcProcessing"]},"STICKY_FAULT":{"name":"STICKY_FAULT","type":"numeric_parameter","value":[0]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[16]}},"ports_map":{},"wildcard_assignment":false})lit";
    nlohmann::json check_obj = nlohmann::json::parse(res_str);

    ASSERT_EQ(ast_dump, check_obj);

}



TEST( hdl_ast_builder, interface_parameter) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    std::vector<std::string> paths = {
            "Components/Common"
    };

    auto prefix = td_file("check_files/test_data/");
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv" || f.path().extension() == ".svh"){
                auto test_file = mm_file(f.path());
                sv_analyzer analyzer;

                auto resources = analyzer.analyze("", test_file.view()).value();
                d_store->store_file({f.path(), "file_hash", resources});
            }
        }
    }


    auto test_pattern = R"(
        module test_mod #()();
            parameter test_param = 3;
            axi_stream test_defaults();
            axi_stream #(.DATA_WIDTH(15), .USER_WIDTH(test_param)) test_overload_params();
        endmodule
    )";

    sv_analyzer analyzer;
    
    auto resource = analyzer.analyze("",test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", resource});

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto synth_ast = b.build_ast(std::vector<std::string>({"test_mod"}))[0];


    auto struct_s = synth_ast->dump_structure();
    auto reference_structure = "TL:test_mod\n    test_defaults:axi_stream\n    test_overload_params:axi_stream\n";

    EXPECT_EQ(struct_s, reference_structure);

    auto ast_dump = synth_ast->dump();

    std::string res_str = R"lit({"children":[{"children":[],"instance_name":"test_defaults","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[32]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[32]}},"ports_map":{},"wildcard_assignment":false},{"children":[],"instance_name":"test_overload_params","instance_type":"axi_stream","parameters":{"CLOCK_PERIOD":{"name":"CLOCK_PERIOD","type":"numeric_parameter","value":[1]},"DATA_WIDTH":{"name":"DATA_WIDTH","type":"numeric_parameter","value":[15]},"DEST_WIDTH":{"name":"DEST_WIDTH","type":"numeric_parameter","value":[32]},"USER_WIDTH":{"name":"USER_WIDTH","type":"numeric_parameter","value":[3]}},"ports_map":{},"wildcard_assignment":false}],"instance_name":"TL","instance_type":"test_mod","parameters":{"test_param":{"name":"test_param","type":"numeric_parameter","value":[3]}},"ports_map":{},"wildcard_assignment":false})lit";
    nlohmann::json check_obj = nlohmann::json::parse(res_str);

    ASSERT_EQ(ast_dump, check_obj);

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto ast_v2 = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];

    auto struct_v2 = ast_v2->dump_structure();
    ASSERT_EQ(struct_v2, struct_s);
}

TEST( hdl_ast_builder, package_dependency) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

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
    
    auto entities = analyzer.analyze("/tmp/dep.sv", test_pattern).value();
    d_store->store_file({"/tmp/dep.sv", "file_hash", entities});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto synth_ast = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];
    Dependency_resolver_v2 resolver({synth_ast}, d_store);
    auto deps = resolver.get_packages();
    std::set<std::string> deps_check = {"/tmp/dep.sv"};
    EXPECT_EQ(deps, deps_check);
}


TEST( hdl_ast_builder, memory_dependency) {


    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto test_pattern = R"(

        module test_mod #(
        )();

        reg [31:0] memory [99:0];

        initial begin
            $readmemb("/tmp/mem.dat", memory);
        end

        endmodule
    )";


    sv_analyzer analyzer;
    
    auto entities = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", entities});

    DataFile d("mem", "/tmp/mem.dat");
    d_store->store_file({"/tmp/mem.dat", "hash", {d}});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto synth_ast = b2.build_ast(std::vector<std::string>({"test_mod"}))[0];
    Dependency_resolver_v2 resolver({synth_ast}, d_store);
    auto deps = resolver.get_data();
    std::set<std::string> deps_check = {"/tmp/mem.dat"};
    EXPECT_EQ(deps, deps_check);
}


TEST( hdl_ast_builder, nested_generate_loop) {

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto test_pattern = R"(
        module leaf();
            wire w;
        endmodule

        module top #(
            parameter N = 2,
            parameter M = 3
        )();
            generate
                for (genvar i = 0; i < N; i = i + 1) begin
                    for (genvar j = 0; j < M; j = j + 1) begin
                        leaf l();
                    end
                end
            endgenerate
        endmodule
    )";

    sv_analyzer analyzer;
    auto entities = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", entities});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto synth_ast = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto structure = synth_ast->dump_structure();
    auto reference = "TL:top\n    l:leaf\n    l:leaf\n    l:leaf\n    l:leaf\n    l:leaf\n    l:leaf\n";
    EXPECT_EQ(structure, reference);
    EXPECT_EQ(synth_ast->get_dependencies().size(), 6);
}


TEST( hdl_ast_builder, loop_around_conditional) {

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto test_pattern = R"(
        module leaf();
            wire w;
        endmodule

        module top #(
            parameter N = 2,
            parameter FLAG = 1
        )();
            generate
                for (genvar i = 0; i < N; i = i + 1) begin
                    if (FLAG) begin
                        leaf l_then();
                    end else begin
                        leaf l_else();
                    end
                end
            endgenerate
        endmodule
    )";

    sv_analyzer analyzer;
    auto entities = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", entities});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto synth_ast = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto structure = synth_ast->dump_structure();
    auto reference = "TL:top\n    l_then:leaf\n    l_else:leaf\n    l_then:leaf\n    l_else:leaf\n";
    EXPECT_EQ(structure, reference);
    EXPECT_EQ(synth_ast->get_dependencies().size(), 4);
}


TEST( hdl_ast_builder, conditional_around_loop) {

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto test_pattern = R"(
        module leaf();
            wire w;
        endmodule

        module top #(
            parameter N = 3,
            parameter FEAT_A = 1,
            parameter FEAT_B = 0
        )();
            generate
                if (FEAT_A) begin
                    leaf l_a();
                    if (FEAT_B) begin
                        leaf l_b();
                    end else begin
                        leaf l_not_b();
                    end
                end else begin
                    leaf l_not_a();
                end
            endgenerate
        endmodule
    )";

    sv_analyzer analyzer;
    auto entities = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", entities});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto synth_ast = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto structure = synth_ast->dump_structure();
    auto reference = "TL:top\n    l_a:leaf\n    l_b:leaf\n    l_not_b:leaf\n    l_not_a:leaf\n";
    EXPECT_EQ(structure, reference);
    EXPECT_EQ(synth_ast->get_dependencies().size(), 4);
}

TEST( hdl_ast_builder, triple_nested_conditional) {

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto test_pattern = R"(
        module leaf();
            wire w;
        endmodule

        module top #(
            parameter A = 1,
            parameter B = 0,
            parameter C = 1
        )();
            generate
                if (A) begin
                    leaf l_a();
                    if (B) begin
                        leaf l_b();
                    end else begin
                        if (C) begin
                            leaf l_c();
                        end else begin
                            leaf l_d();
                        end
                    end
                end else begin
                    leaf l_not_a();
                end
            endgenerate
        endmodule
    )";

    sv_analyzer analyzer;
    auto entities = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", entities});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto synth_ast = b2.build_ast(std::vector<std::string>({"top"}))[0];

    auto structure = synth_ast->dump_structure();
    auto reference = "TL:top\n    l_a:leaf\n    l_b:leaf\n    l_c:leaf\n    l_d:leaf\n    l_not_a:leaf\n";
    EXPECT_EQ(structure, reference);
    EXPECT_EQ(synth_ast->get_dependencies().size(), 5);
}

TEST( hdl_ast_builder, primitive_classification) {

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto test_pattern = R"(
        module leaf();
            wire w;
        endmodule

        module top();
            FDRE f1 (.C(1'b0), .D(1'b0), .Q());
            leaf l1();
            BUFG b1 (.I(1'b0), .O());
        endmodule
    )";

    sv_analyzer analyzer;
    auto entities = analyzer.analyze("", test_pattern).value();
    d_store->store_file({"/dev/zero", "file_hash", entities});

    hdl_resource_statement leaf_mod;
    leaf_mod.set_name("leaf");
    leaf_mod.set_type(module);
    hdl_file f;
    f.set_content({std::make_shared<hdl_resource_statement>(leaf_mod)});
    d_store->store_file({"/dev/zero/leaf.sv", "file_hash", f});

    HDL_ast_builder_v2 b2(s_store, d_store, Depfile());
    auto synth_ast = b2.build_ast(std::vector<std::string>({"top"}))[0];

    ASSERT_EQ(synth_ast->get_dependencies().size(), 3);
    auto fdre = synth_ast->get_dependencies()[0];
    auto leaf = synth_ast->get_dependencies()[1];
    auto bufg = synth_ast->get_dependencies()[2];

    EXPECT_EQ(fdre->get_type(), "FDRE");
    EXPECT_EQ(fdre->get_dependency_class(), primitive);
    EXPECT_EQ(bufg->get_type(), "BUFG");
    EXPECT_EQ(bufg->get_dependency_class(), primitive);
    EXPECT_EQ(leaf->get_type(), "leaf");
    EXPECT_EQ(leaf->get_dependency_class(), module);

    pass_manager pm(d_store);
    pm.apply_passes(synth_ast);
}

TEST( hdl_ast_builder, recursion_limit_tunable) {

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    std::string module_a = "module recursive_a ();\n    recursive_b b_inst ();\nendmodule\n";
    std::string module_b = "module recursive_b ();\n    recursive_a a_inst ();\nendmodule\n";

    sv_analyzer analyzer;
    auto res_a = analyzer.analyze("", module_a).value();
    d_store->store_file({"/tmp/recursive_a.sv", "file_hash", res_a});

    auto res_b = analyzer.analyze("", module_b).value();
    d_store->store_file({"/tmp/recursive_b.sv", "file_hash", res_b});

    HDL_ast_builder_v2 strict_builder(s_store, d_store, Depfile(), 1);
    auto strict_ast = strict_builder.build_ast(std::vector<std::string>({"recursive_a"}))[0];
    EXPECT_EQ(strict_ast->dump_structure(), "TL:recursive_a\n    b_inst:recursive_b\n");

    HDL_ast_builder_v2 relaxed_builder(s_store, d_store, Depfile(), 3);
    auto relaxed_ast = relaxed_builder.build_ast(std::vector<std::string>({"recursive_a"}))[0];
    EXPECT_EQ(relaxed_ast->dump_structure(), "TL:recursive_a\n"
                                             "    b_inst:recursive_b\n"
                                             "        a_inst:recursive_a\n"
                                             "        b_inst:recursive_b\n"
                                             "        a_inst:recursive_a\n"
                                             "        b_inst:recursive_b\n");
}



TEST(hdl_ast_builder, vhdl_imported_constant_in_generic) {
    // The package lives in its own file; the entity imports it from another.
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    {
        auto pkg_content = R"(package params_pkg is
    constant WIDTH : integer := 8;
end params_pkg;)";
        vhdl_analyzer pkg_analyzer("test/params_pkg.vhd");
        d_store->store_file({"test/params_pkg.vhd", "hash_pkg", pkg_analyzer.analyze_content(pkg_content, "test/params_pkg.vhd")});
    }
    {
        auto top_content = R"(library work;
use work.params_pkg.all;

entity top is
    generic (
        N : integer := WIDTH
    );
end top;
architecture rtl of top is
begin
end rtl;)";
        vhdl_analyzer top_analyzer("test/top.vhd");
        d_store->store_file({"test/top.vhd", "hash_top", top_analyzer.analyze_content(top_content, "test/top.vhd")});
    }

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast = b.build_ast(std::vector<std::string>({"top"}));
    ASSERT_EQ(ast.size(), 1u);
    auto param = ast[0]->get_parameters().get("n");
    ASSERT_TRUE(param->get_value().has_value());
    ASSERT_EQ(param->get_value()->get_integer().get_value(), 8);
}

TEST(hdl_ast_builder, sv_imported_constant_in_param) {
    // The package lives in its own file; the module imports it from another.
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    {
        auto pkg_content = R"(package test_pkg;
    parameter int WIDTH = 8;
endpackage)";
        sv_analyzer pkg_analyzer;
        d_store->store_file({"test/test_pkg.sv", "hash_pkg", pkg_analyzer.analyze("", pkg_content).value()});
    }
    {
        auto top_content = R"(import test_pkg::*;

module top
    #(parameter int N = WIDTH)();
endmodule)";
        sv_analyzer top_analyzer;
        d_store->store_file({"test/top.sv", "hash_top", top_analyzer.analyze("", top_content).value()});
    }

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast = b.build_ast(std::vector<std::string>({"top"}));
    ASSERT_EQ(ast.size(), 1u);
    auto param = ast[0]->get_parameters().get("N");
    ASSERT_TRUE(param->get_value().has_value());
    ASSERT_EQ(param->get_value()->get_integer().get_value(), 8);
}

TEST(hdl_ast_builder, vhdl_imported_specific_item) {
    // `use work.params_pkg.WIDTH` pulls only that constant, across files.
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    {
        auto pkg_content = R"(package params_pkg is
    constant WIDTH : integer := 8;
    constant DEPTH : integer := 4;
end params_pkg;)";
        vhdl_analyzer pkg_analyzer("test/params_pkg.vhd");
        d_store->store_file({"test/params_pkg.vhd", "hash_pkg", pkg_analyzer.analyze_content(pkg_content, "test/params_pkg.vhd")});
    }
    {
        auto top_content = R"(library work;
use work.params_pkg.WIDTH;

entity top is
    generic (
        N : integer := WIDTH
    );
end top;
architecture rtl of top is
begin
end rtl;)";
        vhdl_analyzer top_analyzer("test/top.vhd");
        d_store->store_file({"test/top.vhd", "hash_top", top_analyzer.analyze_content(top_content, "test/top.vhd")});
    }

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast = b.build_ast(std::vector<std::string>({"top"}));
    ASSERT_EQ(ast.size(), 1u);
    auto param = ast[0]->get_parameters().get("n");
    ASSERT_TRUE(param->get_value().has_value());
    ASSERT_EQ(param->get_value()->get_integer().get_value(), 8);
}

TEST(hdl_ast_builder, sv_imported_function) {
    // `import test_pkg::*` pulls a package function into scope (unqualified call).
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    {
        auto pkg_content = R"(package test_pkg;
    function int calc(int x);
        calc = x * 2;
    endfunction
endpackage)";
        sv_analyzer pkg_analyzer;
        d_store->store_file({"test/test_pkg.sv", "hash_pkg", pkg_analyzer.analyze("", pkg_content).value()});
    }
    {
        auto top_content = R"(import test_pkg::*;

module top
    #(parameter int RESULT = calc(21))();
endmodule)";
        sv_analyzer top_analyzer;
        d_store->store_file({"test/top.sv", "hash_top", top_analyzer.analyze("", top_content).value()});
    }

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast = b.build_ast(std::vector<std::string>({"top"}));
    ASSERT_EQ(ast.size(), 1u);
    auto param = ast[0]->get_parameters().get("RESULT");
    ASSERT_TRUE(param->get_value().has_value());
    ASSERT_EQ(param->get_value()->get_integer().get_value(), 42);
}

TEST(hdl_ast_builder, sv_module_header_import) {
    // `import` inside the module header must parse and be captured as a
    // file-level import statement.
    auto content = R"(module top import test_pkg::*;
    #(parameter int RESULT = 1)();
endmodule)";
    sv_analyzer analyzer;
    auto file = analyzer.analyze("", content).value();
    bool found = false;
    for (auto &stmt : file.get_content()) {
        auto imp = std::dynamic_pointer_cast<hdl_import_stmt>(stmt);
        if (imp && imp->get_package() == "test_pkg" && imp->is_wildcard()) found = true;
    }
    EXPECT_TRUE(found);
}

TEST(hdl_ast_builder, sv_imported_type) {
    // `import test_pkg::*` pulls a package typedef into scope (unqualified type).
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    {
        auto pkg_content = R"(package test_pkg;
    typedef logic [7:0] byte_t;
endpackage)";
        sv_analyzer pkg_analyzer;
        d_store->store_file({"test/test_pkg.sv", "hash_pkg", pkg_analyzer.analyze("", pkg_content).value()});
    }
    {
        auto top_content = R"(import test_pkg::*;

module top
    #(parameter byte_t DATA = 8'hFF)();
endmodule)";
        sv_analyzer top_analyzer;
        d_store->store_file({"test/top.sv", "hash_top", top_analyzer.analyze("", top_content).value()});
    }

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast = b.build_ast(std::vector<std::string>({"top"}));
    ASSERT_EQ(ast.size(), 1u);
    auto param = ast[0]->get_parameters().get("DATA");
    ASSERT_TRUE(param->get_value().has_value());
    ASSERT_EQ(param->get_value()->get_integer().get_value(), 255);

    // The imported package typedef actually resolved onto the parameter.
    auto type = param->get_type();
    ASSERT_NE(type, nullptr);
    ASSERT_TRUE(type->is<HDL_simple_type>());
    auto &simple = type->as<HDL_simple_type>();
    auto packed = simple.get_packed_dimensions();
    ASSERT_EQ(packed.size(), 1u);
    ASSERT_EQ(packed[0].first_bound->print(), "7");
    ASSERT_EQ(packed[0].second_bound->print(), "0");
}

TEST(hdl_ast_builder, vhdl_multiple_architectures) {
    // Two instances of the same entity select different architectures.
    auto test_pattern = R"(
entity a is
    port ( clk : in std_logic );
end a;
entity b is
    port ( clk : in std_logic );
end b;

entity foo is
    port ( clk : in std_logic );
end foo;
architecture rtl of foo is
begin
    u_a : entity work.a port map ( CLK => clk );
end rtl;
architecture behavioral of foo is
begin
    u_b : entity work.b port map ( CLK => clk );
end behavioral;

entity top is
end top;
architecture rtl of top is
begin
    u1 : entity work.foo(rtl) port map ( CLK => clk );
    u2 : entity work.foo(behavioral) port map ( CLK => clk );
end rtl;
)";
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");
    vhdl_analyzer analyzer("test/multi_arch.vhd");
    d_store->store_file({"test/multi_arch.vhd", "hash", analyzer.analyze_content(test_pattern, "test/multi_arch.vhd")});

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast = b.build_ast(std::vector<std::string>({"top"}));
    ASSERT_EQ(ast.size(), 1u);
    auto children = ast[0]->get_dependencies();
    ASSERT_EQ(children.size(), 2u);

    // u1 selects the rtl architecture → its implementation contains u_a.
    auto u1 = children[0];
    ASSERT_EQ(u1->get_name(), "u1");
    ASSERT_EQ(u1->get_architecture(), "rtl");
    auto u1_deps = u1->get_dependencies();
    ASSERT_EQ(u1_deps.size(), 1u);
    ASSERT_EQ(u1_deps[0]->get_name(), "u_a");

    // u2 selects the behavioral architecture → its implementation contains u_b.
    auto u2 = children[1];
    ASSERT_EQ(u2->get_name(), "u2");
    ASSERT_EQ(u2->get_architecture(), "behavioral");
    auto u2_deps = u2->get_dependencies();
    ASSERT_EQ(u2_deps.size(), 1u);
    ASSERT_EQ(u2_deps[0]->get_name(), "u_b");
}


TEST(hdl_ast_builder, recursive_module_half_width_terminates) {
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto source = R"(
module popcount #(
    parameter int unsigned INPUT_WIDTH = 256,
    localparam int unsigned PopcountWidth = $clog2(INPUT_WIDTH)+1
) (
    input logic [INPUT_WIDTH-1:0] data_i,
    output logic [PopcountWidth-1:0] popcount_o
);
    localparam int unsigned PaddedWidth = 1 << $clog2(INPUT_WIDTH);
    if (INPUT_WIDTH == 1) begin : single_node
    end else if (INPUT_WIDTH == 2) begin : leaf_node
    end else begin : non_leaf_node
        popcount #(.INPUT_WIDTH(PaddedWidth / 2))
            left_child(.data_i(data_i[INPUT_WIDTH-1:INPUT_WIDTH/2]), .popcount_o(popcount_o));
        popcount #(.INPUT_WIDTH(PaddedWidth / 2))
            right_child(.data_i(data_i[INPUT_WIDTH/2-1:0]), .popcount_o(popcount_o));
    end
endmodule

module top #(parameter int unsigned INPUT_WIDTH = 8) ();
    popcount #(.INPUT_WIDTH(INPUT_WIDTH)) i_popcount();
endmodule
)";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", source);
    ASSERT_TRUE(resources.has_value());
    d_store->store_file({"/tmp/popcount.sv", "h", resources.value()});

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast = b.build_ast(std::vector<std::string>{"top"})[0];

    // Count every popcount node in the tree. The active chain halves the width
    // 8 -> 4 -> 2 and stops at the leaf; the inactive recursive children of the
    // leaf are terminals. Without the termination guard this runs away to 256.
    std::vector<std::shared_ptr<hdl_ast_node>> stack;
    stack.push_back(ast);
    int popcounts = 0;
    while (!stack.empty()) {
        auto node = stack.back();
        stack.pop_back();
        if (!node) continue;
        if (node->get_type() == "popcount") popcounts++;
        auto deps = node->get_dependencies();
        stack.insert(stack.end(), deps.begin(), deps.end());
    }
    // 15 popcounts: 7 in the active chain (1@8, 2@4, 4@2) plus 8 inactive
    // terminal children of the width-2 leaves (their non-selected recursive
    // branch). None of the terminals is elaborated, so the build terminates.
    ASSERT_EQ(popcounts, 15);
}

TEST(hdl_ast_builder, recursive_module_struct_field_override_terminates) {
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto source = R"(
package cfg_pkg;
    typedef struct packed {
        int unsigned INSTR_PER_FETCH;
    } cva6_cfg_t;
endpackage

module popcount #(
    parameter int unsigned INPUT_WIDTH = 256,
    localparam int unsigned PaddedWidth = 1 << $clog2(INPUT_WIDTH)
) ();
    if (INPUT_WIDTH == 1) begin : single_node
    end else if (INPUT_WIDTH == 2) begin : leaf_node
    end else begin : non_leaf_node
        popcount #(.INPUT_WIDTH(PaddedWidth / 2)) left_child();
        popcount #(.INPUT_WIDTH(PaddedWidth / 2)) right_child();
    end
endmodule

module instr_queue #(parameter cfg_pkg::cva6_cfg_t CVA6Cfg) ();
    popcount #(.INPUT_WIDTH(CVA6Cfg.INSTR_PER_FETCH)) i_popcount();
endmodule

module top #(parameter cfg_pkg::cva6_cfg_t CVA6Cfg = '{INSTR_PER_FETCH: 4}) ();
    instr_queue #(.CVA6Cfg(CVA6Cfg)) i_instr();
endmodule
)";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", source);
    ASSERT_TRUE(resources.has_value());
    d_store->store_file({"/tmp/rec.sv", "h", resources.value()});

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto ast = b.build_ast(std::vector<std::string>{"top"})[0];

    std::vector<std::shared_ptr<hdl_ast_node>> stack;
    stack.push_back(ast);
    int popcounts = 0;
    while (!stack.empty()) {
        auto node = stack.back();
        stack.pop_back();
        if (!node) continue;
        if (node->get_type() == "popcount") popcounts++;
        auto deps = node->get_dependencies();
        stack.insert(stack.end(), deps.begin(), deps.end());
    }
    // popcount(4) -> {popcount(2), popcount(2)}; the non-selected recursive
    // branches of the width-2 leaves add 4 inactive terminal popcounts.
    ASSERT_EQ(popcounts, 7);
}

TEST(hdl_ast_builder, package_localparam_in_struct_type) {

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto source = R"(
package config_pkg;
    localparam test_param = 16;
    typedef struct packed {
        logic [test_param-1:0] field;
    } cfg_t;
endpackage

module child #(parameter config_pkg::cfg_t Cfg) ();
endmodule

module top #(parameter config_pkg::cfg_t Cfg = 1000) ();
    child #(.Cfg(Cfg)) i_child();
endmodule
)";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", source);
    ASSERT_TRUE(resources.has_value());
    d_store->store_file({"/tmp/repro.sv", "h", resources.value()});

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    log_capture logs;
    auto ast = b.build_ast(std::vector<std::string>{"top"})[0];
    EXPECT_EQ(logs.count("is not defined in the design"), 0u);
    EXPECT_EQ(logs.count("test_param is undefined"), 0u);
    ASSERT_EQ(ast->get_dependencies().size(), 1u);
    auto child = ast->get_dependencies()[0];
    ASSERT_EQ(child->get_name(), "i_child");


    auto top_cfg = ast->get_parameters().get("Cfg")->get_numeric_value();
    ASSERT_TRUE(top_cfg.has_value());
    EXPECT_EQ(top_cfg->get_value(), 1000);

    auto child_cfg = child->get_parameters().get("Cfg")->get_numeric_value();
    ASSERT_TRUE(child_cfg.has_value());
    EXPECT_EQ(child_cfg->get_value(), 1000);
}

TEST(hdl_ast_builder, bits_of_overridden_local_struct_type) {
    // Mirrors frontend/btb.sv: child computes $bits(T) where T is a type
    // parameter overridden with the parent's *local* struct type.
    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    auto source = R"(
module btb #(
    parameter type btb_prediction_t = logic,
    parameter int unsigned BRAM_WORD_BITS = $bits(btb_prediction_t)
) ();
endmodule

module frontend ();
    localparam type btb_prediction_t = struct packed {
        logic valid;
        logic [7:0] target;
    };
    btb #(.btb_prediction_t(btb_prediction_t)) i_btb();
endmodule
)";

    sv_analyzer analyzer;
    auto resources = analyzer.analyze("", source);
    ASSERT_TRUE(resources.has_value());
    d_store->store_file({"/tmp/btb_repro.sv", "h", resources.value()});

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    log_capture logs;
    auto ast = b.build_ast(std::vector<std::string>{"frontend"})[0];
    EXPECT_EQ(logs.count("$bits argument is of unsupported type"), 0u);
    ASSERT_EQ(ast->get_dependencies().size(), 1u);
    auto btb = ast->get_dependencies()[0];
    ASSERT_EQ(btb->get_name(), "i_btb");

    // struct { valid + 8-bit target } = 9 bits
    auto w = btb->get_parameters().get("BRAM_WORD_BITS")->get_numeric_value();
    ASSERT_TRUE(w.has_value());
    EXPECT_EQ(w->get_value(), 9);
}
