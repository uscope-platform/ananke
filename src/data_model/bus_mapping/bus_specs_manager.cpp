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

#include "data_model/bus_mapping/bus_specs_manager.hpp"

#include <cstdlib>
#include <stdexcept>

bus_specs_manager::bus_specs_manager(const bus_specs_manager &b) {
    bus_specs = b.bus_specs;
    port_dir_specs = b.port_dir_specs;
}

bus_specs_manager::bus_specs_manager(const std::string &bus_type) :
    bus_specs_manager(bus_type, []{
        // Test and development runs resolve the in-tree specs without
        // requiring `make install` (which is what the baked SPECS_FOLDER
        // points at in non-debug builds).
        if (const char *env = std::getenv("ANANKE_SPECS_DIR"); env && *env)
            return std::string(env);
        return std::string(SPECS_FOLDER);
    }()) {
}

bus_specs_manager::bus_specs_manager(const std::string &bus_type, const std::string &specs_folder) {
    std::string spec_file = specs_folder;
    spec_file += "/" + bus_type + ".json";
    load_specs(spec_file);
}


void bus_specs_manager::load_specs(std::string &b) {
    std::ifstream ifs(b);
    if (!ifs.is_open()) {
        throw std::runtime_error("bus_specs_manager: specification file not found: " + b);
    }
    nlohmann::json bus_spec_obj = nlohmann::json::parse(ifs);
    std::string name = bus_spec_obj["name"];

    std::vector<mapper_bus_component> components;
    for(auto &comp: bus_spec_obj["components"].items()){
        mapper_bus_component new_comp(comp.key(), comp.value());
        new_comp.set_defaults(bus_spec_obj["default_specs"]);
        if(new_comp.get_class()==interconnect){
            interconnect_modules.insert(new_comp.get_name());
        } else if(new_comp.get_class()==sink){
            sink_modules.insert(new_comp.get_name());
        } else if(new_comp.get_class()==source){
            source_modules.insert(new_comp.get_name());
        }
        components.push_back(new_comp);
    }
    std::unordered_map<if_port_dir, std::string> dir_map = {{if_port_input, bus_spec_obj["modports"]["in"]}, {if_port_output, bus_spec_obj["modports"]["out"]}};
    port_dir_specs = dir_map;
    bus_specs = components;
}


bool bus_specs_manager::is_sink(const std::string &s) {
    return sink_modules.contains(s);
}

bool bus_specs_manager::is_interconnect(const std::string &s) {
    return interconnect_modules.contains(s);
}

bool bus_specs_manager::is_source(const std::string &s) {
    return source_modules.contains(s);
}

std::string bus_specs_manager::get_interconnect_source_port(const std::string &module_n) {
    for(auto &item:bus_specs){
        if(item.get_name()==module_n){
            return item.get_spec("out_port");
        }
    }
    return "";
}

std::string
bus_specs_manager::get_component_spec(const std::string &c, const std::string &s) {
    for(auto &item:bus_specs){
        if(item.get_name() == c){
            return item.get_spec(s);
        }
    }
    return "";
}

bool bus_specs_manager::has_component_spec(const std::string &c, const std::string &s) {
    for(auto &item:bus_specs){
        if(item.get_name() == c){
            return item.has_spec(s);
        }
    }
    return false;
}

bool bus_specs_manager::is_output_port(const std::string &type) {
    return port_dir_specs[if_port_output] == type;
}

std::pair<std::string, bool> bus_specs_manager::get_input_port(const std::string &c) {
    std::string in_name;
    for(auto &comp:bus_specs){
        if(comp.get_name() == c){
            if(comp.has_spec("in_port")){
                in_name = comp.get_spec("in_port");
            } else {
                in_name = port_dir_specs[if_port_input];
            }
        }
    }
    return {in_name, false};
}

std::pair<std::string, bool> bus_specs_manager::get_output_port(const std::string &c) {
    std::string out_name;
    bool templated =false;
    for(auto &comp:bus_specs){
        if(comp.get_name() == c){
            if(comp.has_spec("out_port_template")){
                out_name = comp.get_spec("out_port_template");
                templated = true;
            } else if(comp.has_spec("out_port")){
                out_name = comp.get_spec("out_port");
            } else {
                out_name = port_dir_specs[if_port_input];
            }
        }
    }
    return {out_name, templated};
}

