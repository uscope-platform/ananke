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
    if (node->get_dependency_class() == primitive) return;
    // Interfaces carry no processor documentation: skip silently instead of
    // warning about a missing module definition.
    if (node->get_dependency_class() == interface) return;
    auto node_spec_opt = d_store->get_HDL_resource(node->get_type());
    if (!node_spec_opt.has_value()) {
        spdlog::warn("Skipping processor detection for module {} that was not found in the current repository", node->get_type());
        return;
    }
    auto node_spec = node_spec_opt.value();
    if(node_spec->has_processors()) {
        auto processors = node_spec->get_processor_doc();

        for(auto &proc:processors){
            auto addr_s = proc.get_address();
            auto addr_vals = node->find_parameter(addr_s);
            if (!addr_vals) {
                spdlog::warn("Processor {} does not have a valid address, thus it will be omitted from the application file", proc.get_name());
                continue;
            }
            hdl_integer address;

            auto addr_value = addr_vals->get_value();
            if (!addr_value.has_value()) {
                spdlog::warn("Processor {} does not have a valid address, thus it will be omitted from the application file", proc.get_name());
                continue;
            }
            if(addr_vals->is_array()){
                if (!addr_value.value().is_int_array()) {
                    spdlog::warn("Processor {} does not have a valid array address, thus it will be omitted from the application file", proc.get_name());
                    continue;
                }
                auto addr_idx = proc.get_address_idx();
                if (addr_idx < 0) {
                    spdlog::warn("Processor {} has a negative address index, thus it will be omitted from the application file", proc.get_name());
                    continue;
                }
                auto addr_opt = addr_value.value().get_int_array().get_value({0,0,addr_idx});
                if(!addr_opt.has_value()) {
                    spdlog::warn("Processor {} address index {} is out of range, thus it will be omitted from the application file", proc.get_name(), addr_idx);
                    continue;
                }
                address = addr_opt.value();
            } else {
                if (!addr_value.value().is_integer()) {
                    spdlog::warn("Processor {} does not have a defined integer address, thus it will be omitted from the application file", proc.get_name());
                    continue;
                }
                address = addr_value.value().get_integer();
            }
            proc.set_address_value(address);
        }
        node->set_processors(processors);
    }
}
