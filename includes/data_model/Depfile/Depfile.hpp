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

#ifndef ANANKE_DEPFILE_HPP
#define ANANKE_DEPFILE_HPP

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "data_model/Script.hpp"
#include "data_model/Constraints.hpp"
#include "data_model/Depfile/depfile_validator.hpp"

#include <nlohmann/json.hpp>

using json = nlohmann::json;


struct  general_settings {
    std::string project_name;
    std::optional<std::string>  target_part = std::nullopt;
    std::optional<std::string>  board = std::nullopt;
    std::string synth_tl;
    std::vector<std::string> synth_modules = {};
    std::string sim_tl;
    std::vector<std::string> sim_modules = {};
    std::vector<std::string> include_paths = {};
};


struct bus_specs {
    std::string type;
    std::string starting_module;
    std::string bus_interface;
};

class Depfile {
public:
    general_settings general;
    std::vector<script_specs> scripts;
    std::vector<std::string> excluded_modules;
    std::vector<Constraints> constraints;
    std::unordered_map<std::string, bus_specs> bus;

    Depfile();
    Depfile(const Depfile &other) = default;
    Depfile(Depfile &&other) noexcept = default;
    Depfile & operator=(const Depfile &other) = default;
    Depfile & operator=(Depfile &&other) noexcept = default;

    explicit Depfile(std::istream &in);
    void set_content(const nlohmann::json &c);

    std::optional<bus_specs> get_bus_section(const std::string &bus_type);

    std::vector<Script> get_scripts();
    bool is_module_excluded(const std::string &s);
    void add_excluded_module(const std::string &s);
private:
};


#endif //ANANKE_DEPFILE_HPP
