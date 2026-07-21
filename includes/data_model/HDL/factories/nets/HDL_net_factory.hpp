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


#ifndef ANANKE_HDL_NET_FACTORY_HPP
#define ANANKE_HDL_NET_FACTORY_HPP

#include "data_model/HDL/HDL_net.hpp"
#include "data_model/HDL/factories/nets/HDL_repetition_factory.hpp"
#include "data_model/HDL/factories/nets/HDL_range_factory.hpp"

class HDL_net_factory {
public:
    void new_net(const std::string &name);
    std::vector<HDL_net> get_nets();
    void start_range();
    void close_range();
    void add_component(const std::string &c);
    void add_component(const std::shared_ptr<Expression_base> &ec);
    void add_replication_size(const std::string &c);
    void add_index_component(const std::string &c);
    void set_range_type(HDL_range::range_type_t t);

    void start_concatenation();
    void stop_concatenation();
    bool is_in_concatenation() const {return in_concatenation;}
    void advance_concatenation();

    void start_array();
    void stop_array();
    bool is_in_array() const {return in_array;}
    bool is_in_range() const {return range_factory.is_active();}

    void set_name(const std::string & string);


    void start_repetition() {repetition_factory.start_repetition();}
    void stop_repetition();
    void advance_repetition() {repetition_factory.advance_phase();}
    bool is_in_replication() const {return repetition_factory.is_in_repetition();}

    void set_operation(Expression_v2::expression_operator op);
    void push_expression(const Expression_v2 &e);
    void push_index_expression(const Expression_v2 &e);
private:
    bool in_concatenation = false;
    bool in_array = false;

    std::vector<HDL_net> nets;
    HDL_repetition_factory repetition_factory;
    HDL_range_factory range_factory;
    HDL_net current_net;
};



#endif //ANANKE_HDL_NET_FACTORY_HPP
