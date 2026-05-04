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

#include "Backend/Lattice/lattice_project_generator.hpp"

#include <fmt/format.h>

void lattice_project_generator::write_makefile(std::ostream &output) {
    generate_project(output);
    output << R"(prj_close)" <<std::endl;
}

void lattice_project_generator::generate_sim_script(std::ostream &output) {

    output << R"(vlib work)" <<std::endl;
    for(const auto& str:data.synth_sources){
        output << fmt::format(R"(vlog {})", str) <<std::endl;
    }
    for(const auto& str:data.package_synth_sources){
        output << fmt::format(R"(vlog {})", str) <<std::endl;
    }

    for (const auto &file:data.package_sim_sources) {
        if (!data.package_synth_sources.contains(file))
            output << fmt::format(R"(vlog {})", file) <<std::endl;;
    }

    for (const auto &file:data.sim_sources) {
        if (!data.synth_sources.contains(file))
            output << fmt::format(R"(vlog {})", file) <<std::endl;
    }

    output << R"(vlib vsim -c -voptargs="+acc" work.)"<<data.tb_tl <<std::endl;
    output << R"(add log -r /*)"<<data.tb_tl <<std::endl;
    output << R"(run 500us)"<<data.tb_tl <<std::endl;
}

void lattice_project_generator::generate_synth_script(std::ostream &output) {
    generate_project(output);
    output << R"(prj_run Synthesis -impl "impl1")" <<std::endl;
    output << R"(prj_run Map -impl "impl1")" <<std::endl;
    output << R"(prj_run PAR -impl "impl1")" <<std::endl;
    output << R"(prj_run Export -impl "impl1")" <<std::endl;
    output << R"(prj_save)" <<std::endl;
    output << R"(prj_close)" <<std::endl;
}

void lattice_project_generator::generate_project(std::ostream &output) {

    output << "set build_dir \"build\"" <<std::endl;
    output << R"(if {![file exists $build_dir]} {)" <<std::endl;
    output << R"(    file mkdir $build_dir)" <<std::endl;
    output << R"(})" <<std::endl;
    output << R"()" << data.name << "\"" <<std::endl;
    output << fmt::format(R"(prj_create -name "{}" -impl "impl1" -dev "{}" -dir "./build_dir")", data.name, data.target_part) <<std::endl;
    for(const auto& str:data.synth_sources){
        output << fmt::format(R"(prj_add_source {})", str) <<std::endl;
    }
    for(const auto& str:data.package_synth_sources){
        output << fmt::format(R"(prj_add_source {})", str) <<std::endl;
    }
    for(const auto& str:data.constraints_sources){
        output << fmt::format(R"(prj_add_source {})", str) <<std::endl;
    }
    output << R"(prj_set_impl_opt -impl "impl1" {top} {)"<<data.synth_tl<< "}" <<std::endl;
    output << R"(prj_set_impl_opt -impl "impl1" {VerilogStandard} {System Verilog})" <<std::endl;
    output << R"(prj_save)" <<std::endl;

}
