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


#include "Backend/Xilinx/Vivado_manager.hpp"



Vivado_manager::Vivado_manager(std::shared_ptr<settings_store> s, bool del_mkfile, std::string name) : Toolchain_manager(std::move(s), del_mkfile, std::move(name)){
    vivado_path = "";
    vivado_path = s_store->get_tool_path("amd_vivado");
    if(!std::filesystem::exists(vivado_path)){
        spdlog::warn("Current Vivado installation not found:{}", vivado_path);
        vivado_path = "";
    }
}

void Vivado_manager::create_project(const std::string& makefile, bool start_gui) {

    spawn_process(prepare_call(makefile), false, true);

    if(delete_makefile){
        std::filesystem::remove(makefile);
    }

    if(start_gui){

        std::vector<std::string> arg_v;
        arg_v.emplace_back(vivado_path + "/bin/vivado");
        arg_v.emplace_back("-nolog");
        arg_v.emplace_back("-nojournal");
        arg_v.push_back(project_name + "/" + project_name+".xpr");

        spawn_process(arg_v, true, false);
    }
}

std::vector<std::string> Vivado_manager::prepare_call(const std::string &makefile) {
    std::vector<std::string> arg_v;
    arg_v.emplace_back(vivado_path + "/bin/vivado");
    arg_v.emplace_back("-mode");
    arg_v.emplace_back("batch");
    arg_v.emplace_back("-nolog");
    arg_v.emplace_back("-nojournal");
    arg_v.emplace_back("-source");
    arg_v.push_back(makefile);
    return arg_v;
}
