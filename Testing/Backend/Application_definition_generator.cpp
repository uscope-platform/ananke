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
#include <filesystem>

#include "../../includes/data_model/Depfile/Depfile.hpp"
#include "data_model/data_store.hpp"
#include "data_model/settings_store.hpp"
#include "frontend/analysis/sv_analyzer.hpp"
#include "analysis/HDL_ast_builder_v2.hpp"
#include "analysis/control_bus_analysis.hpp"
#include "Backend/uplatform/peripheral_definition_generator.hpp"
#include "Backend/uplatform/application_definition_generator.hpp"

TEST( app_def_generation , generate_app_def) {

    std::shared_ptr<data_store> d_store = std::make_shared<data_store>(true, "/tmp/test_data_store");
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store");

    std::vector<std::string> paths = {
            "Components/controls/PID/rtl",
            "Components/controls/integrator/rtl",
            "Components/Common",
            "Components/system/axi_lite/simple_register_cu/rtl",
            "Components/system/axi_lite/skid_buffer/rtl"
    };

    auto prefix = "check_files/test_data/";
    for(auto &p:paths){
        for(auto &f:std::filesystem::directory_iterator(prefix + p)){
            if(f.path().extension() == ".v" || f.path().extension() == ".sv"){
                auto test_file = mm_file(f.path());
                sv_analyzer analyzer;


                for(auto &entity:analyzer.analyze("", test_file.view())){
                    d_store->store_hdl_entity(entity);
                }
            }
        }
    }


    nlohmann::json df_content;
    df_content["general"] = nlohmann::json();
    df_content["general"]["project_name"] = "project";
    df_content["general"]["sim_tl"] = "PID";
    df_content["general"]["synth_tl"] = "PID";
    df_content["general"]["board"] = "test";
    df_content["bus"] = nlohmann::json();
    df_content["bus"]["control"] = nlohmann::json();
    df_content["bus"]["control"]["bus_interface"] = "axil";
    df_content["bus"]["control"]["starting_module"] = "PID";
    df_content["bus"]["control"]["type"] = "axi_lite";

    Depfile df;
    df.set_content(df_content);

    HDL_ast_builder_v2 b(s_store, d_store, Depfile());
    auto synth_ast = b.build_ast(std::vector<std::string>({"PID"}))[0];


    control_bus_analysis bus_analyzer(df);
    bus_analyzer.analyze_bus(synth_ast);


    peripheral_definition_generator periph_def_gen(d_store, synth_ast);
    application_definition_generator app_def_gen(
            synth_ast,
            periph_def_gen.get_peripheral_definitions(),
            periph_def_gen.get_alias_map(),
            std::unordered_map<std::string, std::string>()
    );

    app_def_gen.construct_application("test_app");

    auto res = app_def_gen.get_definition_string();
    std::string check_string = R"({"application_name":"test_app_new","bitstream":"","channel_groups":[],"channels":[],"clock_frequency":100000000.0,"filters":[],"initial_registers_values":[],"macro":[],"miscellaneous":{"n_enables":0,"timebase_address":""},"parameters":[],"peripherals":[{"base_address":"0x0","hdl_parameters":{},"name":"TL","peripheral_id":"TL","proxied":false,"proxy_address":"0","spec_id":1}],"pl_clocks":{"0":100000000.0,"1":100000000.0,"2":100000000.0,"3":100000000.0},"programs":[],"scripts":[],"soft_cores":[]})";
    ASSERT_EQ(res, check_string);
}
