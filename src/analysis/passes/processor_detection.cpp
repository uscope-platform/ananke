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

#include "analysis/passes/processor_detection.hpp"

#include "data_model/data_store.hpp"

processor_detection::processor_detection(const std::shared_ptr<data_store> &d) {
    d_store = d;
}

void processor_detection::process_node(const std::shared_ptr<hdl_ast_node> &node) {
    auto node_spec_opt = d_store->get_HDL_resource(node->get_type());
    auto node_params = node->get_parameters();
    if (!node_spec_opt.has_value()) {
        spdlog::critical("Skipping processor detection for module {} that was not found in the current repositor");
        return;
    }
    auto node_spec = node_spec_opt.value();
    if(node_spec.has_processors()) {
        auto processors = node_spec.get_processor_doc();

        for(auto &proc:processors){
            auto addr_s = proc.get_address();
            auto addr_vals = node_params.get(addr_s);
            hdl_integer address;

            if(addr_vals->is_array()){
                auto addr_idx = proc.get_address_idx();
                auto addr_opt = addr_vals->get_int_array_value();
                if(!addr_opt.has_value()) {
                    spdlog::warn("Processor {} does not have a valid address, thus it will be omitted from the application file", proc.get_name());
                    continue;
                }
                address = addr_opt.value().get_value({0,0,addr_idx}).value();
            } else {
                auto addr = addr_vals->get_numeric_value();
                if(!addr.has_value()) {
                    spdlog::warn("Processor {} does not have a defined address, thus it will be omitted from the application file", proc.get_name());
                    continue;
                }
                address = addr.value();
            }
            proc.set_address_value(address);
        }
        node->set_processors(processors);
    }
}
