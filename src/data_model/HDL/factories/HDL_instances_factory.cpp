//  Copyright 2025 Filippo Savi
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

#include "data_model/HDL/factories/HDL_instances_factory.hpp"


void HDL_instances_factory::new_dependency(const std::string &n, const std::string &p, dependency_class dc) {
    current_instance = HDL_instance(n, p, dc);
    valid_instance = true;
}

void HDL_instances_factory::add_parameter(const std::shared_ptr<HDL_parameter> &p) {
    current_instance.add_parameter(p);
}

void HDL_instances_factory::add_port(const std::string &name) {
    auto nets = net_factory.get_nets();
    current_instance.add_port_connection(name, nets);
}

void HDL_instances_factory::start_scalar_net(const std::string &n) {
    if(!net_factory.is_in_array() && !net_factory.is_in_range() && !net_factory.is_in_replication()) {
        net_factory.new_net(n);
    }
}

void HDL_instances_factory::add_scalar_net(const std::string &name) {
    net_factory.new_net(name);
}

HDL_instance HDL_instances_factory::get_dependency() {
    valid_instance = false;
    return current_instance;
}

void HDL_instances_factory::start_concat_port(const std::string &n) {
    net_factory.start_concatenation();
    port_name = n;
}

void HDL_instances_factory::stop_concat_port() {
    net_factory.stop_concatenation();
    current_instance.add_port_connection(port_name, net_factory.get_nets());
}

void HDL_instances_factory::start_replication_port(const std::string &n) {
    port_name = n;
}

void HDL_instances_factory::add_concatenation_net() {
    net_factory.advance_concatenation();
}


void HDL_instances_factory::add_connection_element(const std::string &s) {
    net_factory.add_component(s);
}

void HDL_instances_factory::add_connection_element(const Token &ec) {
    net_factory.add_component(ec);
}

void HDL_instances_factory::start_bit_selection() {
    net_factory.start_array();
}

void HDL_instances_factory::stop_bit_selection() {
    net_factory.stop_array();
}

void HDL_instances_factory::start_replication() {
    net_factory.start_repetition();
    net_factory.new_net("");
}

void HDL_instances_factory::stop_replication() {
    net_factory.stop_repetition();
}

void HDL_instances_factory::advance_replication() {
    net_factory.advance_repetition();
}

void HDL_instances_factory::start_interface() {
    in_interface = true;
}

void HDL_instances_factory::stop_interface() {
    in_interface = false;
}

void HDL_instances_factory::start_array_range() {
    net_factory.start_range();
}

void HDL_instances_factory::advance_array_range_phase(const std::string &op) {
    if(op == "+") net_factory.set_range_type(HDL_range::increasing_range);
    else if(op == "-") net_factory.set_range_type(HDL_range::decreasing_range);
    else net_factory.set_range_type(HDL_range::explicit_range);
}

void HDL_instances_factory::stop_array_range() {
    net_factory.close_range();
}


void HDL_instances_factory::add_array_quantifier(const std::shared_ptr<HDL_parameter> &p) {
    current_instance.add_array_quantifier(p);
}

void HDL_instances_factory::change_array_name(const std::string &s){
    net_factory.set_name(s);
}
