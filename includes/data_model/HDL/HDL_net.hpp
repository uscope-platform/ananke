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


#ifndef ANANKE_HDL_NET_HPP
#define ANANKE_HDL_NET_HPP

#include<string>
#include <nlohmann/json.hpp>

#include "data_model/HDL/parameters/HDL_parameter.hpp"



struct HDL_replication {
    Expression target;
    Expression size;
    template<class Archive>
    void serialize( Archive & ar ) {
        ar(size, target);
    }


    friend bool operator==(const HDL_replication &lhs, const HDL_replication &rhs) {
        bool retval =true;
        retval &= lhs.target == rhs.target;
        retval &= lhs.size == rhs.size;
        return retval;
    }

};



struct HDL_range {
    Expression accessor;
    Expression range;
    enum range_type_t{ explicit_range, increasing_range, decreasing_range};
    range_type_t type = explicit_range;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(accessor, range,type);
    }


    friend bool operator==(const HDL_range &lhs, const HDL_range &rhs) {
        bool retval =true;
        retval &= lhs.accessor == rhs.accessor;
        retval &= lhs.range == rhs.range;
        retval &= lhs.type == rhs.type;
        return retval;
    }

};


class HDL_net {
public:
    HDL_net();
    explicit HDL_net(const std::string &s);
    bool empty() const {return name.empty() && range.accessor.empty() && replication.size.empty();}

    std::string get_full_name() const;
    std::string get_base_name() {return name;}

    bool is_replication() {
        return !replication.size.empty();
    }
    bool is_array() {
        return !range.accessor.empty() || !index->as<Expression>().empty();
    }


    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name,index, range,replication);
    }

    friend bool operator==(const HDL_net &lhs, const HDL_net &rhs) {
        bool retval =true;
        retval &= lhs.name == rhs.name;
        retval &= lhs.range == rhs.range;
        retval &= lhs.replication == rhs.replication;
        return retval;
    }
    std::string get_name() const {
        return name;
    }
    std::shared_ptr<Parameter_value_base> get_index_at(uint32_t i) {
        if (!index->is<Expression>()) {
            spdlog::error("Non expression indexes for hdl nets are not supported");
            return nullptr;
        }
        return index->as<Expression>().components[i];
    }
    std::shared_ptr<Parameter_value_base> get_index() const {return index;}
    HDL_range get_range() const {return range;}
    HDL_replication get_replication() const {return replication;}
    void evaluate(const std::map<qualified_identifier, resolved_parameter> &context);

    void set_name(const std::string &s) {
        name = s;
    }
    void set_index(std::shared_ptr<Parameter_value_base> i) {
        index = i;
    }
    void add_index_component(const std::string &ec) {
        index->as<Expression>().emplace_back(ec, Token::get_type(ec));
    }
    void add_index_component(const Token &ec) {
        index->as<Expression>().push_back(ec);
    }
    void set_range(HDL_range r) {
        range = r;
    }
    void add_relication_size(const std::string &ec) {
        replication.size.emplace_back(ec, Token::get_type(ec));
    }
    void add_relication_target(const std::string &ec) {
        replication.target.emplace_back(ec, Token::get_type(ec));
    }
    void set_replication(HDL_replication r) {
        replication = r;
    }

    friend bool operator!=(const HDL_net &lhs, const HDL_net &rhs) {
        return !(lhs == rhs);
    }

    virtual nlohmann::json dump();
private:
    std::string name;
    std::shared_ptr<Parameter_value_base> index;
    HDL_range range;
    HDL_replication replication;
};


#endif //ANANKE_HDL_NET_HPP
