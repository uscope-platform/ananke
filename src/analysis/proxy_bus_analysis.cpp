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


#include "analysis/proxy_bus_analysis.hpp"

proxy_bus_analysis::proxy_bus_analysis(std::shared_ptr<settings_store> &s, std::shared_ptr<data_store> &d, const Depfile& d_f) :dep_file(d_f){
 d_store = d;
 s_store = s;
}



void proxy_bus_analysis::analyze(const std::shared_ptr<hdl_ast_node> &node) {
    std::stack<std::shared_ptr<hdl_ast_node>> working_stack;
    working_stack.push(node);


    while(!working_stack.empty()){
        auto current_node = working_stack.top();
        working_stack.pop();

        if(!current_node->get_proxy_specs().module.empty()){
            HDL_ast_builder_v2 b(s_store, d_store, dep_file);
            auto bus_ast = b.build_ast(std::vector<std::string>({current_node->get_proxy_specs().module}))[0];
            control_bus_analysis bus_analyzer(dep_file);
            bus_analyzer.analyze_bus(bus_ast, current_node->get_proxy_specs().interface);
            current_node->set_proxy_ast(bus_ast);
        }

        for(const auto& item:current_node->get_dependencies()) working_stack.push(item);
    }
}

