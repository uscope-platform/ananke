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

#include "Backend/new_app_generator.hpp"

new_app_generator::new_app_generator(const std::string &app_name, std::string &app_language) {
    name = app_name;
    lang = app_language;


    std::filesystem::create_directories(app_name + "/tb");
    std::filesystem::create_directories(app_name + "/rtl");
    write_depfile();
    write_synth_hdl();
    write_sim_hdl();
    write_constraints();

}

void new_app_generator::write_depfile() {
    nlohmann::json depfile;
    depfile["general"] = nlohmann::json();
    depfile["general"]["project_name"] = name;
    depfile["general"]["target_part"] = "xc7z020clg400";
    depfile["general"]["synth_modules"] = std::vector<std::string>();
    depfile["general"]["synth_tl"] = name;
    depfile["general"]["sim_modules"] = std::vector<std::string>();
    depfile["general"]["sim_tl"] = name + "_tb";

    depfile["general"]["include_paths"] = {"Components/Common"};
    std::vector<nlohmann::json> scripts;
    nlohmann::json sc;
    sc["name"] = "set_properties.tcl";
    sc["type"] = "tcl";
    sc["arguments"] = std::vector<std::string>();
    scripts.push_back(sc);
    depfile["scripts"] = scripts;

    depfile["excluded_modules"] = std::vector<std::string>();
    std::vector<std::string> constraints = {name + ".xdc"};
    depfile["constraints"] = constraints;

    std::ofstream stream(name+"/Depfile");
    stream<<depfile;

}

void new_app_generator::write_synth_hdl() {
    std::string file_content = "`timescale 10ns / 1ns\n\nmodule " + name + " (\n);\n\nendmodule";

    std::ofstream stream(name+"/rtl/"+name+"."+lang);
    stream<<file_content;
}

void new_app_generator::write_sim_hdl() {
    std::string file_content = "`timescale 10ns / 1ns\n\nmodule " + name + "_tb ();\n\n\nendmodule";

    std::ofstream stream(name+"/tb/"+name+"_tb."+lang);
    stream<<file_content;
}


void new_app_generator::write_constraints() {
    std::ofstream ofs(name+"/"+name+".xdc");
    ofs.close();
}

