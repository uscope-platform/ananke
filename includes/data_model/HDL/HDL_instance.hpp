// Copyright 2021 Filippo Savi
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ANANKE_HDL_INSTANCE_HPP
#define ANANKE_HDL_INSTANCE_HPP

#include <unordered_map>
#include <string>
#include <utility>
#include <nlohmann/json.hpp>

#include "data_model/HDL/HDL_definitions.hpp"
#include "data_model/HDL/HDL_loop.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/parameters/Parameters_map.hpp"
#include "data_model/documentation/channel_group.hpp"
#include "data_model/HDL/HDL_net.hpp"

#include <cereal/archives/binary.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/array.hpp>
#include <cereal/types/memory.hpp>

#include <spdlog/spdlog.h>


class HDL_instance {
public:
    virtual ~HDL_instance() = default;
    HDL_instance clone();
    HDL_instance(std::string dep_name, std::string dep_type, dependency_class d_c);
    HDL_instance() = default;
    HDL_instance(const HDL_instance &c );

    void add_parameter(const std::shared_ptr<HDL_parameter> &p);
    void add_parameters(Parameters_map &p);
    void set_parameters(Parameters_map &p);
    Parameters_map get_parameters();
    bool has_parameter(const std::string &s){return parameters.contains(s);};
    std::shared_ptr<HDL_parameter> get_parameter_value(const std::string& parameter_name) {return parameters.get(parameter_name);};

    void set_wildcard(bool w){wildcard_assignment = true;}
    bool get_wildcard() const {return  wildcard_assignment;}

    void add_port_connection(const std::string& port_name, std::vector<HDL_net> value);
    void set_ports(const std::unordered_map<std::string, std::vector<HDL_net>> &p) {
        ports_map = p;
    };
    std::unordered_map<std::string, std::vector<HDL_net>> get_ports() { return ports_map;};

    std::string get_name() const {return name;};
    void set_name(const std::string &n) {
        name = n;
    }

    std::string get_type() const {return type;};
    void set_type(const std::string &t) {
        type = t;
    }

    dependency_class get_dependency_class() const {return dep_class;};
    void set_dependency_class(dependency_class dc) {
        dep_class = dc;
    };

    void add_loop(const HDL_loop_metadata &l) {
        loop_specs.push_back(l);
    };
    void update_loop(const HDL_loop_metadata &l, int i) {
        loop_specs[i] = l;
    }
    HDL_loop_metadata get_inner_loop() {return loop_specs.empty() ? HDL_loop_metadata{} : loop_specs[0];};
    unsigned int get_n_loops() {return loop_specs.size();};

    void set_channel_groups(const std::vector<channel_group> &g) {
        groups = g;
    };
    std::vector<channel_group> get_channel_groups(){ return groups;};

    void add_array_quantifier(const std::shared_ptr<HDL_parameter> &p) {
        array_quantifier = p;
    };
    std::shared_ptr<HDL_parameter> get_array_quantifier() const {return array_quantifier;};

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, type, dep_class, ports_map, parameters, groups, loop_specs, array_quantifier);
    }

    virtual nlohmann::json dump();

    friend bool operator==(const HDL_instance&lhs, const HDL_instance&rhs);


protected:

    Parameters_map parameters;
    std::unordered_map<std::string, std::vector<HDL_net>> ports_map;
    bool wildcard_assignment = false;
    dependency_class dep_class;
    std::string type;
    std::string name;
    std::vector<HDL_loop_metadata> loop_specs;

    std::vector<channel_group> groups;
    std::shared_ptr<HDL_parameter> array_quantifier;
};


#endif //ANANKE_HDL_INSTANCE_HPP
