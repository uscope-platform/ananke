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

#include "Backend/project_generator_base.hpp"

#include <fmt/format.h>

project_generator_base::project_generator_base(const std::shared_ptr<settings_store> &s_store) {
    data.board_part = "";
    settings = s_store;
    base_dir = settings->get_hdl_store();
}

void project_generator_base::set_data(const project_data &d) {
    data = d;
    std::set<std::string> diff;
    std::set_difference(data.sim_sources.begin(), data.sim_sources.end(), data.synth_sources.begin(), data.synth_sources.end(),
                        std::inserter(diff, diff.begin()));

    data.sim_sources = diff;
}
