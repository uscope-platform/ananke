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


#include "analysis/passes/port_solution.hpp"

void port_solution::process_node(const std::shared_ptr<HDL_instance_AST> &node) {
    auto ports = node->get_ports();
    if(node->get_parent() != nullptr) {
        auto parameters = node->get_parent()->get_parameters();
        std::map<qualified_identifier, resolved_parameter> ctx;
        for(const auto &[p_name, param]:parameters) {
            auto val = param->get_value();
            if(val.has_value()) ctx[param->get_identifier()] = val.value();
        }
        for(auto [port_name, nets]:node->get_ports()) {
            for(auto &n:nets) {
                n.evaluate(ctx);
            }
        }
        node->set_ports(ports);
    }


}
