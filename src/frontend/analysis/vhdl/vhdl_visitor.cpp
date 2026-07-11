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

#include "frontend/analysis/vhdl/vhdl_visitor.hpp"



vhdl_visitor::vhdl_visitor(std::string p) {
    path = std::move(p);
}

std::vector<HDL_Resource> vhdl_visitor::get_entities() {
    return entities;
}

void vhdl_visitor::enterEntity_declaration(mgp_vh::vhdlParser::Entity_declarationContext *ctx) {
    std::string module_name = ctx->identifier()[0]->getText();
    size_t line_number = ctx->getStart()->getLine();
    modules_factory.new_module(module_name, path,module, line_number);
}

void vhdl_visitor::exitArchitecture_body(mgp_vh::vhdlParser::Architecture_bodyContext *ctx) {
    std::string name = ctx->name()->getText();
    for(auto &item:entities){
        if(item.getName() == name){
            item.add_dependencies(dependency_map[item.getName()]);
        }
    }
}


void vhdl_visitor::exitEntity_declaration(mgp_vh::vhdlParser::Entity_declarationContext *ctx) {
    entities.push_back(modules_factory.get_module());
}

void vhdl_visitor::exitConcurrent_statement(mgp_vh::vhdlParser::Concurrent_statementContext *ctx) {
    if(ctx->component_instantiation_statement()!= nullptr){
        auto instantiation = ctx->component_instantiation_statement();
        std::string module_name;
        if(instantiation->instantiated_unit()->name()->suffix() != nullptr){
            module_name = instantiation->instantiated_unit()->name()->suffix()->getText();
        } else{
            module_name = instantiation->instantiated_unit()->name()->name_literal()->identifier()->getText();
        }
        HDL_instance dep(ctx->label()->getText(), module_name, module);
        dependency_map[current_architecture].push_back(dep);
    }
}

void vhdl_visitor::enterArchitecture_body(mgp_vh::vhdlParser::Architecture_bodyContext *ctx) {
    current_architecture = ctx->name()->getText();
}

