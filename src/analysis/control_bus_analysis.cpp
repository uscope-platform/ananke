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

#include "analysis/control_bus_analysis.hpp"



control_bus_analysis::control_bus_analysis(const Depfile &df) : specs_manager("axi_lite"){
    dfile = df;
}

void control_bus_analysis::analyze_bus(std::shared_ptr<HDL_instance_AST> &ast) {

    if (!dfile.get_bus_section("control")) return;
    auto bus = dfile.get_bus_section("control").value();
    std::string bus_if = bus.bus_interface;

    analysis_context top = {ast, bus.bus_interface, 0, "",
        {}, "","",-1};
    analize_node({top});
}

void control_bus_analysis::analyze_bus(std::shared_ptr<HDL_instance_AST> &ast,const std::string &intf) {

    std::string bus_if = intf;

    analysis_context top = {ast, bus_if, 0, "",
        {},"","", -1};
    analize_node({top});
}


void control_bus_analysis::analize_node(const std::vector<analysis_context> &n) {
    for(auto &leaf:n){
        current_path.push_back(leaf.node->get_name());
        auto type = leaf.node->get_type();
        if(specs_manager.is_sink(type)){
            //manage_sink
            process_leaf_node(leaf);
        }else if(specs_manager.is_interconnect(type)){
            //manage interconnects
            analize_node(process_interconnect(leaf));
        }else {
            // manage_nested_modules
            analize_node(process_nested_module(leaf));
        }
        current_path.erase(current_path.end()-1);
        if(!modules_array_stack.empty()) {
            modules_array_size = modules_array_stack.back();
            modules_array_stack.pop_back();
        }
    }
}



std::vector<analysis_context> control_bus_analysis::process_interconnect(const analysis_context &inst) {
    std::vector<analysis_context> ret_val;
    auto ic = inst.node;
    auto addr_opt = ic->get_parameter_value("SLAVE_ADDR")->get_int_array_value();
    if(!addr_opt.has_value()) {
        spdlog::warn("The SLAVE_ADDR parameter for interconnect {}, holds an invalid value, skipping the subtree", ic->get_name());
        return ret_val;
    }
    auto addresses = addr_opt.value().get_1d_slice({0,0});
    auto masters_ifs = ic->get_ports()[specs_manager.get_interconnect_source_port(ic->get_type())];

    auto masters = expand_bus_array(masters_ifs, addresses);

    for(auto & master : masters){
        auto dependencies=  inst.node->get_parent()->get_dependencies();
        for(auto &dep:dependencies){
            for(auto &[port_name, nets]:dep->get_ports()){

                if(nets.size()==1){
                    auto net_name = nets.front().get_base_name();


                    if(net_name == master.name){
                        if(specs_manager.is_interconnect(dep->get_type())) {
                            if(specs_manager.get_interconnect_source_port(dep->get_type()) == port_name) {
                                continue;
                            }
                        }
                        if(!master.in_array) {
                            analysis_context ctx = {dep, port_name, master.address,
                                                  inst.current_module_top, inst.current_module_prefix, inst.proxy, inst.array_index};
                            ret_val.push_back(ctx);
                        } else {
                            auto port_index = nets[0].get_index_at(0)->as<Token>().get_value();
                            if (!port_index.has_value()) throw std::runtime_error("The port index must have a deefined value");
                            if(port_index.value().is_string()) throw std::runtime_error("The port index cannot be a string during control bus analysis");
                            if(port_index.value().get_integer() == master.idx) {
                                int32_t index = master.in_array && inst.array_index == -1 ? master.idx : inst.array_index;
                                dep->set_array_index(index);
                                analysis_context ctx = {dep, port_name, master.address,
                                                  inst.current_module_top, inst.current_module_prefix, inst.proxy, index};
                                ret_val.push_back(ctx);
                                goto break2;
                            }

                        }
                    }
                }

            }
        }
        break2:
        continue;
    }
    return ret_val;
}


std::vector<analysis_context> control_bus_analysis::process_nested_module(const analysis_context &inst) {
    std::vector<analysis_context> ret_stack;

    proxy_target tgt;
    if(inst.node->has_parameter("PRAGMA_MKFG_PROXY")){
        if(inst.node->get_parameter_value("PRAGMA_MKFG_PROXY")->get_string_value()=="TRUE") {
            tgt.module = inst.node->get_parameter_value("PRAGMA_MKFG_PROXY_TL")->get_string_value();
            tgt.interface = inst.node->get_parameter_value("PRAGMA_MKFG_PROXY_IF")->get_string_value();

        }
    }
    modules_array_stack.push_back(modules_array_size);
    modules_array_size.clear();
    for(auto &d:inst.node->get_dependencies()) {
        if(d->get_array_quantifier() != nullptr) {
            auto val = d->get_array_quantifier()->get_numeric_value();
            if(val.has_value())
                modules_array_size[d->get_name()] = val.value();
            else {
                spdlog::warn("Invalid array quantifier value when processing module: {}, defaulting to 1", d->get_name());
                modules_array_size[d->get_name()] = 1;
            }
        } else {
            modules_array_size[d->get_name()] = 1;
        }
    }
    for(auto &dep:inst.node->get_dependencies()){
        for(auto &[port_name, nets]:dep->get_ports()){
            for(auto &item:nets){
                if(item.get_base_name() == inst.interface){
                    analysis_context ctx = {dep, port_name, inst.address,
                        inst.current_module_top, inst.current_module_prefix, tgt, inst.array_index};
                    ret_stack.push_back(ctx);
                    goto breakNested;
                }
            }
        }
        breakNested:;
    }

    for(auto &[item_name, item]:inst.node->get_parameters()){
        if(item_name == "PRAGMA_MKFG_MODULE_TOP"){
            auto top = inst.node->get_parameter_value("PRAGMA_MKFG_MODULE_TOP")->get_string_value();
            for(auto &node:ret_stack){
                node.current_module_top = top;
            }
        }
        if(item_name == "PRAGMA_MKFG_CHILD_PREFIX"){
            auto prefix = inst.node->get_parameter_value("PRAGMA_MKFG_CHILD_PREFIX")->get_string_value();
            for(auto &node:ret_stack){
                node.current_module_prefix = prefix;
            }
        }
    }

    return ret_stack;
}

void control_bus_analysis::process_leaf_node(const analysis_context &leaf) {

    auto leaf_parent = leaf.node->get_parent();
    leaf.node->get_parent()->add_address(leaf.address);
    if(!leaf.current_module_top.empty()){
        leaf.node->get_parent()->set_leaf_module_top(leaf.current_module_top);
    }
    if(!leaf.current_module_prefix.empty()){
        leaf.node->get_parent()->set_leaf_module_prefix(leaf.current_module_prefix);
    }
    leaf.node->set_proxy_specs(leaf.proxy);


    if(leaf.array_index >=0) {
            spdlog::info("Found module: {0}[{3}] Type: {1} Address: 0x{2:08x}",
                get_current_path() + leaf.node->get_name(), leaf.node->get_type(), leaf.address.get_value(), leaf.array_index);
    } else {
        spdlog::info("Found module: {0} Type: {1} Address: 0x{2:08x}",get_current_path() + leaf.node->get_name(), leaf.node->get_type(), leaf.address.get_value());
    }

}

std::vector<bus_context>
control_bus_analysis::expand_bus_array( const std::vector<HDL_net> &masters, const std::vector<hdl_integer> &addresses ) {
    std::vector<bus_context> ret;
    uint16_t current_bus = 0;
    for(auto &m:masters) {
        auto current_array_idx = 0;
        while(current_array_idx < modules_array_size[m.get_name()]) {
            bus_context b;
            b.address = addresses[addresses.size()-current_bus-1];
            b.name = m.get_name();
            b.in_array = modules_array_size[m.get_name()]>1;
            b.idx = current_array_idx;
            ret.push_back(b);
            current_array_idx++;
            current_bus++;
        }
    }


    return ret;
}

