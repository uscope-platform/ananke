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

#ifndef ANANKE_PROJECT_GENERATOR_BASE_HPP
#define ANANKE_PROJECT_GENERATOR_BASE_HPP


#include <string>
#include <vector>
#include <unordered_set>
#include <set>
#include <utility>
#include <regex>
#include <nlohmann/json.hpp>

#include "Backend/backend_types.hpp"
#include "data_model/settings_store.hpp"

using json = nlohmann::json;


class project_generator_base {
public:
    explicit project_generator_base(const std::shared_ptr<settings_store> &s_store);
    virtual ~project_generator_base() = default;
    // Generators validate their data here, before any output exists: an invalid Depfile makes this throw
    // std::runtime_error, which ends the run with a message (see main.cpp).
    virtual void set_data(const project_data &d);
    virtual void write_makefile(std::ostream &output) = 0;
    virtual void generate_synth_script(std::ostream &output) = 0;
    virtual void generate_sim_script(std::ostream &output) = 0;
protected:
    std::shared_ptr<settings_store> settings;
    std::string base_dir;
    project_data data;
};


#endif //ANANKE_PROJECT_GENERATOR_BASE_HPP
