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

#include "Backend/Xilinx/xilinx_project_generator.hpp"

void xilinx_project_generator::write_makefile(std::ostream &output) {

    output << "set project_name " <<  data.name <<std::endl;
    output << "set origin_dir \".\""<<std::endl;
    output << "set base_dir " + base_dir << std::endl;

    output << "set commons_dir [list ";
    for(const auto& str:data.commons_dir){
        output << "\""<< base_dir + str << "\" ";
    }
    output << "]"<< std::endl;

    output << "set synth_sources [list ";
    for(const auto& str:data.synth_sources){

        output << "\""<< std::regex_replace(str, std::regex(base_dir), "${base_dir}") << "\" ";
    }
    for(const auto& str:data.package_synth_sources){
        output << "\""<< std::regex_replace(str, std::regex(base_dir), "${base_dir}") << "\" ";
    }
    for(const auto& str:data.data_synth_sources){
        output << "\""<< std::regex_replace(str, std::regex(base_dir), "${base_dir}") << "\" ";
    }
    output << "]"<< std::endl;

    output << "set sim_sources [list ";
    for(const auto& str:data.sim_sources){
        output << "\""<< std::regex_replace(str, std::regex(base_dir), "${base_dir}") << "\" ";
    }
    for(const auto& str:data.package_sim_sources){
        output << "\""<< std::regex_replace(str, std::regex(base_dir), "${base_dir}") << "\" ";
    }
    for(const auto& str:data.data_sim_sources){
        output << "\""<< std::regex_replace(str, std::regex(base_dir), "${base_dir}") << "\" ";
    }
    output << "]"<< std::endl;

    output << "set constraints_sources [list ";
    for(const auto& str:data.constraints_sources){
        output << "\""<< str << "\" ";
    }
    output << "]"<< std::endl;


    if(!data.board_part.empty()){

        output << "set board_part " << data.board_part << std::endl;
        output << "# Create project\ncreate_project ${project_name} ./${project_name}\n";
        output << "set_property board_part $board_part [current_project]\n";
    } else {
        output << "# Create project\ncreate_project ${project_name} ./${project_name}\n";
        output << "set_property part xc7z020clg400-1 [current_project]\n";
    }

    output<<"# Set the directory path for the new project\nset proj_dir [get_property directory [current_project]]\n";
    output << "set obj [current_project]\n";

    std::set<std::string> sourced_scripts;

    for(const auto& scr:data.scripts){
        if(scr.name.empty() || scr.path.empty()) {
            continue;
        }
        if(scr.function_mode) {
            if(!sourced_scripts.contains(scr.path)) {
                output << "source " << scr.path << std::endl;
            }
            if(!scr.name.empty()) {
                output << scr.name;
                for(const auto &val: scr.variables | std::views::values) {
                    output << " " << val ;
                }
                output << std::endl;
            }
        } else {
            if(!sourced_scripts.contains(scr.path)) {
                output << "source " << scr.path << std::endl;
            }
        }
        sourced_scripts.insert(scr.path);
    }

    output << "add_files -norecurse $synth_sources\n";

    if(!data.synth_tl.empty()){
        output<< "set_property top " <<data.synth_tl<<" [get_filesets sources_1]"<< std::endl;
    }

    output << "set_property include_dirs $commons_dir [get_filesets sources_1]\n";
    output << "set_property SOURCE_SET sources_1 [get_filesets sim_1]\n";

    if(!data.constraints_sources.empty()){
        output << "add_files -fileset constrs_1 -norecurse  $constraints_sources\n";
    }

    output << "add_files -fileset sim_1 -norecurse $sim_sources\n";

    if(!data.tb_tl.empty()){
        output<< "set_property top " <<data.tb_tl<<" [get_filesets sim_1]"<< std::endl;
    }

    output << "update_compile_order\n";
}

void xilinx_project_generator::generate_sim_script(std::ostream &output) {
    auto vivado_dir = settings->get_tool_path("vivado");
    auto sim_dir = data.repo_dir + "/sim";

    output << "FILES=( ";
    for (const auto &file:data.package_synth_sources) {
        output << "\n    " << file;
    }
    for (const auto &file:data.synth_sources) {
        output << "\n    " << file;
    }
    output << "\n    " << vivado_dir << "/data/verilog/src/glbl.v";

    for (const auto &file:data.package_sim_sources) {
        if (!data.package_synth_sources.contains(file)) output << "\n    "  << file;
    }

    for (const auto &file:data.sim_sources) {
        if (!data.synth_sources.contains(file)) output << "\n    "  << file;
    }
    output << "\n)\n" << std::endl;

    output << "mkdir -p " << sim_dir << std::endl;
    output << "cp sim.tcl " << sim_dir + "/sim.tcl"<< std::endl;
    for (const auto &file:data.data_synth_sources) {
        output << "cp "<< file << " "<< sim_dir + "/" + std::filesystem::path(file).filename().string() << std::endl;
    }
    output<< std::endl;

    for (const auto &file:data.data_sim_sources) {
        output << "cp "<< file << " "<< sim_dir + "/" + std::filesystem::path(file).filename().string() << std::endl;
    }

    output<< std::endl;

    output << "(" << std::endl;
    output << "    cd "<< sim_dir<<"|| exit" << std::endl<< std::endl;

    output << open_phase("PHASE 1: XVLOG (Analysis)");

    output << "    xvlog -sv \"${FILES[@]}\" ";
    for (const auto& d: data.commons_dir) {
        output<< "-i " << base_dir + d;
    }
    // add includes for xilinx interface definitions
    output << " -i " << vivado_dir + "/data/rsb/busdef"<< std::endl;

    output << check_result("XVLOG FAILED");


    output << open_phase("PHASE 2: XELAB (Elaboration)");
    output << "    xelab -debug typical --relax -top "<<data.tb_tl<<" -top glbl -L xil_defaultlib -L unisims_ver -L unimacro_ver -L xpm  -snapshot sim_snapshot  -timescale 10ns/1ps" << std::endl;
    output << check_result("XELAB FAILED");


    output <<open_phase("PHASE 3: XSIM (Simulation)");
    output << "    xsim sim_snapshot -tclbatch sim.tcl"  << std::endl;
    output << check_result("XSIM FAILED");


    output << ")" << std::endl;


    output << fmt::format("if [ -f {0}/dump.vcd  ]; then",sim_dir) << std::endl;
    output << fmt::format("    vcd2fst {0}/dump.vcd dump.fst",sim_dir) << std::endl;
    output << fmt::format("    rm {0}/dump.vcd", sim_dir) << std::endl;
    output <<"fi" <<std::endl;
    output << "rm -r " << sim_dir << std::endl;
}

void xilinx_project_generator::write_sim_control_script(std::ostream &output) {
    std::ofstream sim_tcl( "sim.tcl");
    output << R"(
open_vcd dump.vcd

log_vcd [get_objects -recursive /*]

run 2ms

flush_vcd
close_vcd
exit
)";
    output.flush();
}

void xilinx_project_generator::generate_synth_script(std::ostream &output) {


    output << "set outputDir ./project_output" << std::endl;
    output << "file mkdir $outputDir" << std::endl;
    output << "cd $outputDir" << std::endl;
    output << "set data_files_set {" << std::endl;
    for (const auto &file:data.data_synth_sources) {
        output << "\t" << file <<std::endl;
    }
    output << "}" << std::endl;

    output << "set sources_set {" << std::endl;
    for (const auto &file:data.package_synth_sources) {
        output << "\t" << file <<std::endl;
    }
    for (const auto &file:data.synth_sources) {
        output << "\t" << file <<std::endl;
    }
    output << "}" << std::endl;
    output << "set inc_dirs {" << std::endl;
        for (const auto &file:data.commons_dir) {
            output << "\t" <<  base_dir + file <<std::endl;
        }
    output << "}" << std::endl;
    output << "set constr_dirs {" << std::endl;
    for (const auto &file:data.constraints_sources) {
        output << "\t" << file <<std::endl;
    }
    output << "}" << std::endl;


    output << "foreach f $data_files_set {" << std::endl;
    output << "    file copy -force $f ." << std::endl;
    output << "}" << std::endl;

    output << "set_part " << data.target_part << std::endl;
    if (!data.board_part.empty()) output << "set board_part " << data.board_part << std::endl;

    std::set<std::string> sourced_scripts;

    for(const auto& scr:data.scripts){
        if(scr.name.empty() || scr.path.empty()) {
            continue;
        }
        if(scr.function_mode) {
            if(!sourced_scripts.contains(scr.path)) {
                output << "source " << scr.path << std::endl;
            }
            if(!scr.name.empty()) {
                output << scr.name;
                for(const auto &val: scr.variables | std::views::values) {
                    output << " " << val ;
                }
                output << std::endl;
            }
            output << "synth_ip [get_ips " << scr.variables.at("name") <<"]" << std::endl;
        } else {
            if(!sourced_scripts.contains(scr.path)) {
                output << "source " << scr.path << std::endl;
            }
        }
        sourced_scripts.insert(scr.path);
    }

    for (const auto &prefix:soc_prefixes) {
        if (data.target_part.starts_with(prefix)) {
            output << "generate_target all [get_files ps.bd]"<< std::endl;
            output << "export_ip_user_files -of_objects [get_files ps.bd] -no_script -force"<< std::endl;
        }
    }

    output << "read_verilog $sources_set" << std::endl;
    output << "if {[llength $constr_dirs] > 0} { read_xdc $constr_dirs }" << std::endl;

    output << "synth_design -top "+data.synth_tl+" -part " + data.target_part + " -include_dirs $inc_dirs" << std::endl;
    output << "write_checkpoint -force $outputDir/post_synth.dcp" << std::endl;

    output << "opt_design" << std::endl;
    output << "place_design" << std::endl;
    output << "write_checkpoint  -force $outputDir/post_place.dcp" << std::endl;


    output << "route_design" << std::endl;
    output << "write_checkpoint  -force $outputDir/post_route.dcp" << std::endl;

    output << "write_bitstream -force $outputDir/top_module.bit" << std::endl;
}

std::string xilinx_project_generator::check_result(const std::string &error_message) {
    std::ostringstream oss;
    oss << "    if [ $? -ne 0 ]; then"<< std::endl;
    oss << fmt::format(R"(        echo -e "\033[1;31m!!! {} !!!\033[0m")", error_message)<< std::endl;
    oss << "        exit 1"<< std::endl;
    oss << "    fi\n"<< std::endl;
    return oss.str();
}

std::string xilinx_project_generator::open_phase(const std::string &phase_name) {
    return fmt::format(R"(    echo -e "\n\033[1;33m>>> {} <<<\033[0m")", phase_name)+ "\n";
}
