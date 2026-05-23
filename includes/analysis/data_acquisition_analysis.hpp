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

#ifndef ANANKE_DATA_ACQUISITION_ANALYSIS_HPP
#define ANANKE_DATA_ACQUISITION_ANALYSIS_HPP

#include <memory>

#include "data_model/HDL/HDL_instance_AST.hpp"
#include "data_model/bus_mapping/bus_specs_manager.hpp"
#include "data_model/documentation/channel_group.hpp"
#include "data_model/documentation/channel.hpp"
#include "data_model/scope.hpp"

typedef struct {
    std::string if_name;
    hdl_integer address_offset;
    bool static_remap;
} data_stream;

class data_acquisition_analysis {
public:
    explicit data_acquisition_analysis(bool logging);
    void analyze(std::shared_ptr<HDL_instance_AST> &ast);
    std::vector<channel> get_datapoints(){return data_points;}
    std::vector<channel_group> get_channel_groups(){return groups;};
    scope_data get_scope_data(){return scope;};
private:
    void backtrace_scope_inputs(const std::shared_ptr<HDL_instance_AST> &node,const data_stream &intf);
    void channelize_groups();

    std::optional<std::vector<data_stream>> process_node(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream);
    std::vector<data_stream> process_n_to_1_node(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream);
    std::vector<data_stream> process_1_to_n_node(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream);
    std::vector<data_stream> process_1_to_1_node(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream);
    void process_source(const std::shared_ptr<HDL_instance_AST> &node, const data_stream &in_stream);
    std::set<std::pair<std::string, std::string>> explored_nodes;
    std::vector<std::shared_ptr<HDL_instance_AST>> find_sinks(std::shared_ptr<HDL_instance_AST> &ast);
    hdl_integer find_datapoint_width(const std::shared_ptr<HDL_instance_AST> &node,std::string name);

    std::vector<std::string> parse_datapoint_names(std::string &s){
        std::stringstream ss(s);
        std::vector<std::string> result;

        while( ss.good() )
        {
            std::string substr;
            getline( ss, substr, ',' );
            result.push_back( substr );
        }
        return result;
    }


    bus_specs_manager specs_manager;
    std::vector<channel> data_points;
    std::vector<channel_group> groups;
    bool log = false;
    scope_data scope;
};


#endif //ANANKE_DATA_ACQUISITION_ANALYSIS_HPP
