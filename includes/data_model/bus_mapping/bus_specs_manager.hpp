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

#ifndef ANANKE_BUS_SPECS_MANAGER_HPP
#define ANANKE_BUS_SPECS_MANAGER_HPP

#include <unordered_map>
#include <string>
#include <vector>
#include <filesystem>
#include <fstream>
#include <set>
#include <unordered_set>

#include "data_model/bus_mapping/bus_component.hpp"

 typedef enum {
    if_port_input = 0,
    if_port_output = 1
} if_port_dir;

class bus_specs_manager {
public:
    bus_specs_manager() = delete;
    explicit bus_specs_manager(const std::string &bus_type);
    bus_specs_manager(const std::string &bus_type, const std::string &specs_folder);
    bus_specs_manager(const bus_specs_manager &b);
    void load_specs(std::string &b);
    std::vector<mapper_bus_component> get_bus_specs(){return bus_specs;};
    std::string get_component_spec(const std::string &c, const std::string &s);
    bool has_component_spec(const std::string &c, const std::string &s);
    std::string get_port_dir_specs(if_port_dir dir){return port_dir_specs[dir];};
    std::string get_interconnect_source_port(const std::string &module_n);
    bool is_output_port(const std::string  &type);

    bool is_sink(const std::string &s);
    bool is_interconnect(const std::string &s);
    bool is_source(const std::string &s);

    std::pair<std::string, bool> get_input_port(const std::string &c);
    std::pair<std::string, bool>  get_output_port(const std::string &c);

private:
    std::vector<mapper_bus_component> bus_specs;
    std::unordered_map<if_port_dir, std::string> port_dir_specs;

    std::unordered_set<std::string> interconnect_modules;
    std::unordered_set<std::string> sink_modules;
    std::unordered_set<std::string> source_modules;
};



#endif //ANANKE_BUS_SPECS_MANAGER_HPP
