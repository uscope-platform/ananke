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

#ifndef ANANKE_LATTICE_PROJECT_GENERATOR_HPP
#define ANANKE_LATTICE_PROJECT_GENERATOR_HPP


#include "../project_generator_base.hpp"

class lattice_project_generator : public project_generator_base{
public:
    lattice_project_generator(const std::shared_ptr<settings_store> &s_store): project_generator_base(s_store) {};
    void write_makefile(std::ostream &output) override;
    void generate_sim_script(std::ostream &output) override;
    void generate_synth_script(std::ostream &output) override;
private:
    void generate_project(std::ostream &output);
};

#endif //ANANKE_LATTICE_PROJECT_GENERATOR_HPP
