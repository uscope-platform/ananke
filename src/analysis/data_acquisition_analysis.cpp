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

#include "analysis/data_acquisition_analysis.hpp"

#include <spdlog/spdlog.h>

data_acquisition_analysis::data_acquisition_analysis(bool logging) : specs_manager("axi_stream"){
    log = logging;
}


void data_acquisition_analysis::analyze(std::shared_ptr<HDL_instance_AST> &ast) {
    auto sinks = find_sinks(ast);
    if(sinks.empty()) return;
    if(sinks.size()>1){
        spdlog::error("Multiple sinks are not supported for data acquisition analysis");
        return;
    }
    auto scope_address = sinks[0]->get_address()[0];

     scope.mux_address = scope_address;

    scope.data_length_address = scope_address + std::stoi(
            specs_manager.get_component_spec(sinks[0]->get_type(), "data_length_offset"));

    scope.buffer_address = scope_address + std::stoi(
            specs_manager.get_component_spec(sinks[0]->get_type(), "dma_buffer_offset"));

    scope.enable_address = scope_address + std::stoi(
            specs_manager.get_component_spec(sinks[0]->get_type(), "enable_offset"));

    auto scope_in_pn = specs_manager.get_component_spec(sinks[0]->get_type(), "in_port");

    std::string data_interface;

    for(auto &[port_name, nets]:sinks[0]->get_ports()){
        if(port_name == scope_in_pn){
            data_interface = nets[0].get_full_name();
        }
    }
    data_stream ds = {data_interface, 0};
    backtrace_scope_inputs(sinks[0]->get_parent(), ds);

    channelize_groups();
}

std::vector<std::shared_ptr<HDL_instance_AST>>
data_acquisition_analysis::find_sinks(std::shared_ptr<HDL_instance_AST> &ast) {
    std::vector<std::shared_ptr<HDL_instance_AST>> ret;

    for(auto &item:ast->get_dependencies()){
        if(specs_manager.is_sink(item->get_type())){
           ret.push_back(item);
           if(!item->get_channel_groups().empty()) {
               for (auto &g: item->get_channel_groups()) {
                   if (g.get_channels().size() > 6) {
                       spdlog::warn("The channel group named: {} contains more than 6 channels, this is not supported on the platform", g.get_name());
                   }
                   groups.push_back(g);
               }
           }
        } else {
            auto ll_res = find_sinks(item);
            ret.insert(ret.end(), ll_res.begin(), ll_res.end());
        }
    }

    return ret;
}

void data_acquisition_analysis::backtrace_scope_inputs(const std::shared_ptr<HDL_instance_AST> &node,const data_stream &intf) {
    auto dbg_n= node->get_name();
    auto dbg_t= node->get_type();
    std::shared_ptr<HDL_instance_AST> if_source;
    std::string if_port;
    for(auto &dep:node->get_dependencies()){
        for(auto [port_name, nets]:dep->get_ports()){
            for(auto &e:nets){
                if(e.get_full_name() == intf.if_name){
                    auto spec = dep->get_if_specs()[port_name];
                    if(specs_manager.is_output_port(spec[1])){
                        if_source = dep;
                        if_port = port_name;
                        goto source_found;
                    }
                }
            }
        }
    }
    source_found:
    if(!if_port.empty()){
        data_stream ds = {if_port, intf.address_offset, intf.static_remap};
        if(auto follow_on_ifs = process_node(if_source, ds)){
            for(auto &ifs:*follow_on_ifs){
                backtrace_scope_inputs(node, ifs);
            }
        } else{
            if(!explored_nodes.contains({if_source->get_name(), if_port})){
                backtrace_scope_inputs(if_source, ds);
                explored_nodes.insert({if_source->get_name(), if_port});
            }
        }
    } else {
        for(auto &[port_name, nets]:node->get_ports()) {
            if(port_name == intf.if_name){
                if(!explored_nodes.contains({node->get_parent()->get_name(), nets[0].get_full_name()})){
                    explored_nodes.insert({node->get_parent()->get_name(), nets[0].get_full_name()});
                    data_stream ds = {nets[0].get_full_name(), intf.address_offset, intf.static_remap};
                    backtrace_scope_inputs(node->get_parent(), ds);
                }

            }
        }
    }

}

std::optional<std::vector<data_stream>> data_acquisition_analysis::process_node(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream) {
    auto node_type = specs_manager.get_component_spec(node->get_type(), "type");

    if(specs_manager.is_interconnect(node->get_type())){
        if(node_type == "passthrough"){
            return process_1_to_1_node(node,in_stream);
        } else if (node_type == "1To2"){
            return process_1_to_n_node(node, in_stream);
        } else if (node_type == "nTo1"){
            return process_n_to_1_node(node, in_stream);
        } else{
            throw std::runtime_error("Error: unknown axi stream insterconnect type found");
        }
    } else if(specs_manager.is_source(node->get_type())){
        process_source(node, in_stream);
        return std::vector<data_stream>();
    } else{
        return std::nullopt;
    }
}

void data_acquisition_analysis::process_source(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream) {
    spdlog::trace("Found data source at node: {}", node->get_name());
    std::string node_names;
    if(node->has_parameter("PRAGMA_MKFG_DATAPOINT_NAMES")){
        node_names = node->get_parameter_value("PRAGMA_MKFG_DATAPOINT_NAMES")->get_string_value();
    } else {
        node_names = node->get_name() + "data_out";
    }

    auto n_points_params = specs_manager.get_component_spec(node->get_type(), "n_points");

    if(!node->has_parameter(n_points_params)){
        spdlog::warn("parameter named {} not found on module : {}.{}", n_points_params, node->get_type(), node->get_name());
        return;
    }
    auto n_params = node->get_parameter_value(n_points_params)->get_numeric_value();

    std::string port_suffix = specs_manager.get_component_spec(node->get_type(), in_stream.if_name);
    auto names = parse_datapoint_names(node_names);

    std::vector<hdl_integer> addresses;
    if(specs_manager.has_component_spec(node->get_type(), "initial_addresses")){
        auto addr_param_name = specs_manager.get_component_spec(node->get_type(), "initial_addresses");
        if(!node->has_parameter(addr_param_name)){
            spdlog::warn("parameter named {} not found on module : {}.{}", addr_param_name, node->get_type(), node->get_name());
            return;
        }
        auto addr_value_opt = node->get_parameter_value(addr_param_name)->get_int_array_value();
        if(!addr_value_opt.has_value()) {
            spdlog::warn("data source in module  {}.{}, does not have a valid address", node->get_type(), node->get_name());
            return;
        }
        addresses = addr_value_opt.value().get_1d_slice({0,0});
    } else{
        for(int i =0; i<n_params; i++){
            addresses.push_back(i);
        }
    }

    auto nets = node->get_ports()[in_stream.if_name];
    auto width = find_datapoint_width(node->get_parent(),  nets[0].get_full_name());

    std::bitset<1024> output_signs;
    if(node->has_parameter("OUTPUT_SIGNED")){
        auto val = node->get_parameter_value("OUTPUT_SIGNED")->get_numeric_value();
        if(!val.has_value()) {
            spdlog::warn("The OUTPUT_SIGNED parameter for data source {}, does not have a valid value, assuming unsigned data",node->get_name());
            output_signs = std::bitset<1024>(val.value().get_value());
        } else {
            output_signs =  std::bitset<1024>(0);
        }

    } else {
        output_signs.set();
    }

    for(int i = 0; i<n_params; i++){
        std::string channel_name;
        if((names.size() == 1 )& n_params>1){
            channel_name = names[0] + "_" + std::to_string(i);
        } else{
            channel_name = names[i];
        }
        channel c;

        bool data_sign = output_signs[i];
        c.set_signed(data_sign);

        if (!port_suffix.empty()) {
            c.set_name(channel_name + "_" +port_suffix);
        }else{
            c.set_name(channel_name);
        }
        c.set_phys_width(width);
        c.set_channel_number(0);

       hdl_integer addr_base = 0;
       if(node->has_parameter("OUTPUT_DESTINATION_BASE")){
           auto val = node->get_parameter_value("OUTPUT_DESTINATION_BASE")->get_numeric_value();
           if(val.has_value()) {
               addr_base = node->get_parameter_value("OUTPUT_DESTINATION_BASE")->get_numeric_value().value();
           } else {
               spdlog::warn("The OUTPUT_DESTINATION_BASE parameter for data source {}, does not have a valid value, assuming it to be 0",node->get_name());
           }
       }

        if(in_stream.static_remap){
            c.set_mux_setting(in_stream.address_offset + addr_base);
        } else{
            c.set_mux_setting(in_stream.address_offset + addresses[i] + addr_base);
        }
        c.set_enabled(true);
        data_points.push_back(c);
    }

}

std::vector<data_stream>
data_acquisition_analysis::process_n_to_1_node(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream) {
    std::vector<data_stream> ret;
    auto in_port = specs_manager.get_input_port(node->get_type());
    for(auto &[port_name, nets]:node->get_ports()){
        if(port_name== in_port.first){
            for(auto &n:nets){
                data_stream ds = {n.get_full_name(), in_stream.address_offset, in_stream.static_remap};
                ret.push_back(ds);
            }
        }
    }
    if(log){
        std::stringstream ss;
        ss<< fmt::format("Analyzing n-to-1 data node: {} of Instance {}, found sources: [", in_stream.if_name,node->get_name());
        for(int i =0 ; i<ret.size(); i++ ){
            ss << ret[i].if_name;
            if(i<ret.size()-1) ss << ", ";
        }
        ss << "]";
        spdlog::trace(ss.str());
    }

    return ret;
}

std::vector<data_stream>
data_acquisition_analysis::process_1_to_n_node(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream) {
    data_stream ret;
    auto in_port = specs_manager.get_input_port(node->get_type());
    for(auto &[port_name, nets]:node->get_ports()){
        if( port_name == in_port.first){
            ret.if_name = nets[0].get_full_name();
            ret.address_offset = in_stream.address_offset;
            ret.static_remap = in_stream.static_remap;
        }
    }
    if(log) spdlog::trace("Analyzing 1-to-n data node: {} of Instance {}, found sources: {}", in_stream.if_name, node->get_name(), ret.if_name);
    return {ret};
}

std::vector<data_stream>
data_acquisition_analysis::process_1_to_1_node(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream) {
    data_stream ret;

    auto in_port = specs_manager.get_input_port(node->get_type());
    hdl_integer remapping_addr = 0;
    std::string remapping_type;
    if(specs_manager.get_component_spec(node->get_type(), "remapping") == "true"){
        remapping_type = node->get_parameter_value("REMAP_TYPE")->get_string_value();
        auto val = node->get_parameter_value("REMAP_OFFSET")->get_numeric_value();
        if(val.has_value()) {
            remapping_addr = node->get_parameter_value("REMAP_OFFSET")->get_numeric_value().value();
        } else {
            spdlog::warn("The REMAP_OFFSET parameter for data source {}, does not have a valid value, assuming it to be 0", node->get_name());
        }

    }

    for(auto &[port_name, nets]:node->get_ports()){
        if(port_name == in_port.first){
            ret.if_name = nets[0].get_full_name();
            if(remapping_type == "STATIC"){
                ret.address_offset = remapping_addr;
                ret.static_remap = true;
            } else if(remapping_type == "DYNAMIC"){
                ret.static_remap = false;
                ret.address_offset = in_stream.address_offset+remapping_addr;
            }

        }
    }


    if(log) spdlog::trace("Analyzing 1-to-1 data node: {} of Instance {}, found sources: {}", in_stream.if_name, node->get_name(), ret.if_name);
    return {ret};
}

hdl_integer
data_acquisition_analysis::find_datapoint_width(const std::shared_ptr<HDL_instance_AST> &node, std::string name) {
    for(auto &item:node->get_dependencies()){
        if(item->get_name() == name){
            auto val = item->get_parameter_value("DATA_WIDTH")->get_numeric_value();
            if(val.has_value()) {
                return val.value();
            } else {
                spdlog::warn("The DATA_WIDTH parameter for data source {}, does not have a valid value, assuming it to be 32", node->get_name());
                return 32;
            }

        }
    }
    return 32;
}

void data_acquisition_analysis::channelize_groups() {
    for(auto &g:groups){
        int channel_progressive = 0;
        for(std::string &c_n:g.get_channels()){
            for(auto &c:data_points){
                std::string data_point_name = c.get_name();
                if(data_point_name==c_n){
                    c.set_channel_number(channel_progressive);
                    channel_progressive++;
                }
            }
        }
    }
}
