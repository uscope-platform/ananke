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

#include "data_model/Depfile/Depfile.hpp"

#include <spdlog/spdlog.h>
#include <sys/stat.h>

Depfile::Depfile(std::istream &in) {


    nlohmann::json file_content;
    in >> file_content;
    set_content(file_content);
}

void Depfile::set_content(const nlohmann::json &file_content) {

    depfile_validator::validate(file_content);
    auto gen = file_content["general"];
    general.project_name = gen["project_name"];
    if ( gen.contains("target_part"))
        general.target_part = gen["target_part"];
    else if ( gen.contains("board"))
        general.board = gen["board"];
    else {
        spdlog::warn("The selected Depfile does not define a target");
    }
    general.synth_tl = gen["synth_tl"];
    general.sim_tl = gen["sim_tl"];
    if (gen.contains("sim_modules")) general.sim_modules = gen["sim_modules"];
    if (gen.contains("synth_modules")) general.synth_modules = gen["synth_modules"];
    if (gen.contains("include_paths")) general.include_paths = gen["include_paths"];
    if (file_content.contains("constraints")) {
        for(const auto& item : file_content["constraints"]){
            constraints.emplace_back(item);
        }
    }

    if (file_content.contains("excluded_modules")) excluded_modules = file_content["excluded_modules"];
    if (file_content.contains("scripts")) {
        for (auto &scr:file_content["scripts"]) {
            script_specs s;
            s.name = scr["name"];
            s.type = scr["type"];

            if (scr.contains("function_mode")) s.function_mode = scr["function_mode"];
            else s.function_mode = false;

            if (scr.contains("include_products")) s.include_products = scr["include_products"];
            else s.include_products = false;
            if (scr.contains("products_type")) s.products_type = scr["products_type"];
            if (scr.contains("arguments") && !scr["arguments"].empty()) {
                if (scr["arguments"][0].is_string()) {
                    s.positional_arguments =  scr["arguments"];
                } else {
                    for (auto &arg:scr["arguments"]) {
                        s.named_arguments.emplace_back(arg["name"], arg["value"]);
                    }

                }
            }
            scripts.emplace_back(s);
        }
    }

    if (file_content.contains("bus")) {
        for (auto &[bus_name, bus_obj]:file_content["bus"].items()) {
            bus.insert({
                bus_name,
                {bus_obj["type"], bus_obj["starting_module"], bus_obj["bus_interface"]}
            });
        }
    }
}



std::vector<Script> Depfile::get_scripts() {
    std::vector<Script> retval;
    for(const auto& item : scripts){
        retval.emplace_back(item);
    }
    return retval;
}



std::optional<bus_specs> Depfile::get_bus_section(const std::string &bus_type) {
    if (bus.contains(bus_type)) return bus.at(bus_type);
    else return std::nullopt;
}

bool Depfile::is_module_excluded(const std::string &s) {
    return std::ranges::contains(excluded_modules, s);
}

void Depfile::add_excluded_module(const std::string &s) {
    excluded_modules.push_back(s);
}

Depfile::Depfile() {
    excluded_modules = std::vector<std::string>();
}

