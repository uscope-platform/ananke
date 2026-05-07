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

#include "data_model/Depfile.hpp"

#include <sys/stat.h>

Depfile::Depfile(const std::string& filename) {
// read a JSON file
    std::ifstream dfstream(filename);
    dfstream >> content;
}

std::string Depfile::get_synth_tl() {
    return content["general"]["synth_tl"];
}

std::string Depfile::get_sim_tl() {
    return content["general"]["sim_tl"];
}

std::vector<std::string> Depfile::get_additional_synth_modules() {
    return content["general"]["synth_modules"];
}

std::vector<std::string> Depfile::get_additional_sim_modules() {
    return content["general"]["sim_modules"];
}

std::vector<std::string> Depfile::get_excluded_modules() {
    return content["excluded_modules"];
}

std::vector<Constraints> Depfile::get_constraints() {
    std::vector<Constraints> retval;
    for(const auto& item : content["constraints"]){
        Constraints constr(item);
        retval.push_back(constr);
    }
    return retval;

}

std::vector<Script> Depfile::get_scripts() {
    std::vector<Script> retval;
    for(auto item : content["scripts"]){
        Script scr(item["name"], item["type"]);
        auto args =  item["arguments"];
        if(args.is_array() && args.size() > 0) {
            if(args[0].is_string()) scr.set_arguments(static_cast<std::vector<std::string>>( item["arguments"]));
            else scr.set_arguments(static_cast<std::vector<nlohmann::json>>(item["arguments"]));
            if(item.contains("function_mode") && item["function_mode"]) {
                scr.set_function_mode(true);
            } else {
                scr.set_function_mode(false);
            }
        }

        if(item.contains("product_include")) {
            scr.set_product(item["product_include"], item["product_type"]);
        }
        retval.push_back(scr);
    }
    return retval;
}

std::string Depfile::get_project_name() {
    return content["general"]["project_name"];
}

std::vector<std::string> Depfile::get_include_directories() {
    return content["general"]["include_paths"];
}

json Depfile::get_bus_section() {
    return content["bus"];
}

bool Depfile::is_module_excluded(const std::string &s) {
    bool ret = false;
    for(auto &item: content["excluded_modules"]){
        ret |= (std::string) item == s;
    }
    return ret;
}

void Depfile::add_excluded_module(const std::string &s) {
    std::vector<std::string> exm = content["excluded_modules"];
    exm.insert(exm.end(), s);
    content["excluded_modules"] = exm;
}

Depfile::Depfile() {
    content["excluded_modules"] = std::vector<std::string>();
}

bool Depfile::has_mappable_bus() {
    if(content.contains("bus")) return !content["bus"].empty();
    return false;
}

std::string Depfile::get_board_def() {
    return content["general"]["board"];
}

std::string Depfile::get_target() {
    return content["general"]["target_part"];
}

bool Depfile::has_board_def() {
    return content["general"].contains("board");
}

