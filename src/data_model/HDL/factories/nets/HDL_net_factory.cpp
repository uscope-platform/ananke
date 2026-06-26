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

#include "data_model/HDL/factories/nets/HDL_net_factory.hpp"

void HDL_net_factory::new_net(const std::string &name) {
    if(!current_net.get_name().empty() && !is_in_concatenation()|| current_net.is_array() || current_net.is_replication()) {
        nets.push_back(current_net);
    }
     current_net = HDL_net(name);
}

std::vector<HDL_net> HDL_net_factory::get_nets() {
    auto ret = nets;
    if(!current_net.empty()) ret.push_back(current_net);
    nets.clear();
    current_net = HDL_net();
    return ret;
}

void HDL_net_factory::start_range() {
    range_factory.open_range(true);
}

void HDL_net_factory::close_range() {
    current_net.set_range(range_factory.get_range());
}

void HDL_net_factory::add_component(const std::string &c) {
    Token tok(c, Token::get_type(c));
    if(repetition_factory.is_in_repetition()) {
        repetition_factory.add_component(tok);
    }
    if(in_array) {
        current_net.add_index_component(c);
    }
    if(in_concatenation) {
        if(current_net.get_name().empty() && !repetition_factory.is_in_repetition()) {
            current_net.set_name(c);
        }
    }
}

void HDL_net_factory::add_component(const Token &ec) {
    if(repetition_factory.is_in_repetition()) {
        repetition_factory.add_component(ec);
    }
    if(in_array) {
        current_net.add_index_component(ec);
    }
}

void HDL_net_factory::set_range_type(HDL_range::range_type_t t) {
    range_factory.set_range_type(t);
}

void HDL_net_factory::start_concatenation() {
    in_concatenation = true;
}

void HDL_net_factory::stop_concatenation() {
    in_concatenation = false;
}

void HDL_net_factory::advance_concatenation() {
    if(!current_net.empty()) {
        nets.push_back(current_net);
    }
    current_net = HDL_net();
}

void HDL_net_factory::start_array() {
    in_array = true;
}

void HDL_net_factory::stop_array() {
    in_array = false;
}

void HDL_net_factory::set_name(const std::string &string) {
    current_net.set_name(string);
}

void HDL_net_factory::stop_repetition() {
    auto rep = repetition_factory.get_repetition();
    repetition_factory.stop_repetition();
    current_net.set_replication(rep);
}

void HDL_net_factory::set_operation(Expression_v2::expression_operator op) {
    repetition_factory.set_operation(op);
}

void HDL_net_factory::push_expression(const Expression_v2 &e) {
    range_factory.set_expression_part(e);
}

void HDL_net_factory::push_index_expression(const Expression_v2 &e) {
    current_net.add_index_expression(e);
}

void HDL_net_factory::add_replication_target(const std::string &c) {
    current_net.add_relication_target(c);
}

void HDL_net_factory::add_index_component(const std::string &c) {
    current_net.add_index_component(c);
}

void HDL_net_factory::add_replication_size(const std::string &c) {
    current_net.add_relication_size(c);
}
