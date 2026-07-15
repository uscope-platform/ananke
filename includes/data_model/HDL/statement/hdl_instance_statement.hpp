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

#ifndef ANANKE_HDL_INSTANCE_STATEMENT_HPP
#define ANANKE_HDL_INSTANCE_STATEMENT_HPP

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/HDL_definitions.hpp"
#include "data_model/HDL/parameters/Parameters_map.hpp"
#include "data_model/HDL/HDL_net.hpp"
#include "data_model/documentation/channel_group.hpp"

class hdl_instance_statement : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    std::unique_ptr<hdl_statement_base> clone() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_name(const std::string& n) { name = n; }
    std::string get_name() const { return name; }

    void set_type(const std::string& t) { type = t; }
    std::string get_type() const { return type; }

    void set_dependency_class(dependency_class dc) { dep_class = dc; }
    dependency_class get_dependency_class() const { return dep_class; }

    void add_parameter(const std::shared_ptr<HDL_parameter>& p) { parameters.insert(p); }
    void set_parameters(const Parameters_map& p) { parameters = p; }
    const Parameters_map& get_parameters() const { return parameters; }
    bool has_parameter(const std::string& s) const { return parameters.contains(s); }

    void add_port_connection(const std::string& port_name, std::vector<HDL_net> value);
    void set_ports(const std::unordered_map<std::string, std::vector<HDL_net>>& p) { ports_map = p; }
    const std::unordered_map<std::string, std::vector<HDL_net>>& get_ports() const { return ports_map; }

    void set_wildcard(bool w) { wildcard_assignment = w; }
    bool get_wildcard() const { return wildcard_assignment; }

    void set_channel_groups(const std::vector<channel_group>& g) { groups = g; }
    const std::vector<channel_group>& get_channel_groups() const { return groups; }

    void set_array_quantifier(const std::shared_ptr<HDL_parameter>& p) { array_quantifier = p; }
    std::shared_ptr<HDL_parameter> get_array_quantifier() const { return array_quantifier; }

private:
    std::string name;
    std::string type;
    dependency_class dep_class = module;
    Parameters_map parameters;
    std::unordered_map<std::string, std::vector<HDL_net>> ports_map;
    bool wildcard_assignment = false;
    std::vector<channel_group> groups;
    std::shared_ptr<HDL_parameter> array_quantifier;
};

#endif //ANANKE_HDL_INSTANCE_STATEMENT_HPP
