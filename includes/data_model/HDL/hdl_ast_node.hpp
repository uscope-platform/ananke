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

#ifndef ANANKE_HDL_AST_NODE_HPP
#define ANANKE_HDL_AST_NODE_HPP

#include <vector>
#include <memory>
#include <string>
#include <unordered_map>
#include <array>
#include <nlohmann/json.hpp>

#include "data_model/HDL/HDL_definitions.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/parameters/Parameters_map.hpp"
#include "data_model/HDL/HDL_net.hpp"
#include "data_model/documentation/channel_group.hpp"
#include "data_model/documentation/processor_instance.hpp"


class hdl_instance_statement;

struct proxy_target {
    std::string module;
    std::string interface;
    bool operator==(const proxy_target& a) const
    {
        return (module == a.interface && interface == a.interface);
    }
} ;


class hdl_ast_node {
public:
    hdl_ast_node() = default;
    hdl_ast_node(const hdl_ast_node &c );
    explicit hdl_ast_node(const hdl_instance_statement &stmt);

    void add_port_connection(const std::string& port_name, std::vector<HDL_net> value);
    void set_ports(const std::unordered_map<std::string, std::vector<HDL_net>> &p) { ports_map = p; }
    std::unordered_map<std::string, std::vector<HDL_net>> get_ports() { return ports_map; }

    void add_parameter(const std::shared_ptr<HDL_parameter> &p);

    void set_parameters(Parameters_map &p);
    Parameters_map get_parameters();
    bool has_parameter(const std::string &s) { return parameters.contains(s); }
    std::shared_ptr<HDL_parameter> get_parameter_value(const std::string& parameter_name) { return parameters.get(parameter_name); }

    std::string get_name() const { return name; }
    void set_name(const std::string &n) { name = n; }
    std::string get_type() const { return type; }
    void set_type(const std::string &t) { type = t; }
    dependency_class get_dependency_class() const { return dep_class; }
    void set_dependency_class(dependency_class dc) { dep_class = dc; }

    void set_wildcard(bool w) { wildcard_assignment = w; }
    bool get_wildcard() const { return wildcard_assignment; }

    void set_channel_groups(const std::vector<channel_group> &g) { groups = g; }
    std::vector<channel_group> get_channel_groups() { return groups; }

    void add_array_quantifier(const std::shared_ptr<HDL_parameter> &p) { array_quantifier = p; }
    std::shared_ptr<HDL_parameter> get_array_quantifier() const { return array_quantifier; }

    std::vector<std::shared_ptr<hdl_ast_node>> get_dependencies() { return child_instances; }
    void add_child(const std::shared_ptr<hdl_ast_node> &i) {
        i->set_array_index(array_index);
        child_instances.push_back(i);
    }
    void set_parent(const std::shared_ptr<hdl_ast_node> &p) { parent = p; }
    std::shared_ptr<hdl_ast_node> get_parent() { return parent; }

    void add_address(const hdl_integer &i) { bus_address.push_back(i); }
    std::vector<hdl_integer> get_address() { return bus_address; }
    void clear_address() { bus_address.clear(); }

    void add_data_dependency(const std::string &p) { data_dependencies.push_back(p); }
    std::vector<std::string> get_data_dependencies() { return data_dependencies; }
    void add_package_dependency(const std::string &p) { package_dependencies.push_back(p); }
    std::vector<std::string> get_package_dependencies() { return package_dependencies; }

    void set_leaf_module_top(const std::string &s) {
        std::string_view sv = s;
        if(s.length() >=2 && s.starts_with('"') && s.ends_with('"')) {
            sv.remove_prefix(1);
            sv.remove_suffix(1);
        }
        leaf_module_top = std::string(sv);
    }
    std::string get_leaf_module_top() { return leaf_module_top; }

    void set_leaf_module_prefix(const std::string &s) {
        std::string_view sv = s;
        if(s.length() >=2 && s.starts_with('"') && s.ends_with('"')) {
            sv.remove_prefix(1);
            sv.remove_suffix(1);
        }
        leaf_module_prefix = std::string(sv);
    }
    std::string get_leaf_module_prefix() { return leaf_module_prefix; }

    void set_if_specs(const std::unordered_map<std::string, std::array<std::string, 2>> &if_s) { if_specs = if_s; }
    std::unordered_map<std::string, std::array<std::string, 2>> get_if_specs() { return if_specs; }

    void set_processors(std::vector<processor_instance> &p) { processors = p; }
    std::vector<processor_instance> get_processors() const { return processors; }

    void set_proxy_specs(const proxy_target &p) { proxy_specs = p; }
    proxy_target get_proxy_specs() const { return proxy_specs; }
    void set_proxy_ast(const std::shared_ptr<hdl_ast_node> &p) { proxy_ast = p; }
    std::shared_ptr<hdl_ast_node> get_proxy_ast() const { return proxy_ast; }

    void set_array_index(int16_t i) { array_index = i; }
    uint32_t get_array_index() const { return array_index; }


    nlohmann::json dump();
    std::string dump_structure();

    friend bool operator==(const hdl_ast_node&lhs, const hdl_ast_node&rhs);

    template<class Archive>
    void serialize(Archive & ar) {
        ar(name, type, dep_class, ports_map, parameters, groups, array_quantifier,
           bus_address, child_instances, data_dependencies, package_dependencies,
           leaf_module_top, leaf_module_prefix, proxy_specs, proxy_ast,
           array_index, if_specs, processors, parent);
    }

private:
    static std::string dump_structure(const std::shared_ptr<hdl_ast_node>&ast, const std::string &prefix);

    std::string name;
    std::string type;
    dependency_class dep_class = module;
    Parameters_map parameters;
    std::unordered_map<std::string, std::vector<HDL_net>> ports_map;
    bool wildcard_assignment = false;
    std::vector<channel_group> groups;
    std::shared_ptr<HDL_parameter> array_quantifier;

    std::vector<hdl_integer> bus_address;
    std::shared_ptr<hdl_ast_node> parent;
    std::vector<std::shared_ptr<hdl_ast_node>> child_instances;

    std::vector<std::string> data_dependencies;
    std::vector<std::string> package_dependencies;

    std::vector<processor_instance> processors;
    int32_t array_index = -1;
    std::unordered_map<std::string, std::array<std::string, 2>> if_specs;
    std::string leaf_module_top;
    std::string leaf_module_prefix;
    proxy_target proxy_specs;
    std::shared_ptr<hdl_ast_node> proxy_ast = nullptr;
};


#endif //ANANKE_HDL_AST_NODE_HPP
