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

#ifndef ANANKE_XILINX_PROJECT_GENERATOR_HPP
#define ANANKE_XILINX_PROJECT_GENERATOR_HPP

#include "Backend/project_generator_base.hpp"
#include <spdlog/spdlog.h>


class xilinx_project_generator: public project_generator_base{
public:
    xilinx_project_generator(const std::shared_ptr<settings_store> &s_store) : project_generator_base(s_store) {};
    void write_makefile(std::ostream &output) override;
    void generate_sim_script(std::ostream &output) override;
    void write_sim_control_script(std::ostream &output);
    void generate_synth_script(std::ostream &output) override;
private:
    auto check_result(const std::string &error_message) -> std::string;
    auto open_phase(const std::string &phase_name) -> std::string;
    std::unordered_set<std::string> soc_prefixes = {"xck26", "xck24", "xc7z"};
};


#endif //ANANKE_XILINX_PROJECT_GENERATOR_HPP
