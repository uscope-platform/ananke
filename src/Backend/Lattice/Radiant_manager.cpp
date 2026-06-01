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

#include "Backend/Lattice/Radiant_manager.hpp"

#include <spdlog/spdlog.h>


Radiant_manager::Radiant_manager(std::shared_ptr<settings_store> s, bool del_mkfile, std::string name) : Toolchain_manager(std::move(s), del_mkfile, std::move(name)) {
    radiant_path = "";
    radiant_path = s_store->get_tool_path("lattice_radiant");

}

void Radiant_manager::create_project(const std::string &makefile, bool start_gui) {
    std::vector<std::string> arg_v;
    arg_v.emplace_back(radiant_path + "/bin/lin64/radiantc");
    arg_v.push_back(makefile);

    spawn_process(arg_v, false, true);

    if(delete_makefile){
        std::filesystem::remove(makefile);
    }

    if(start_gui){

        arg_v.clear();
        arg_v.emplace_back(radiant_path + "/bin/lin64/radiant");
        arg_v.push_back(project_name + "_lscc/" + project_name+"_lscc.rdf");

        spawn_process(arg_v, true, false);
    }
}
