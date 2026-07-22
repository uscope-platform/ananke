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

#include "Backend/uplatform/peripheral_definition_generator.hpp"

peripheral_definition_generator::peripheral_definition_generator(std::shared_ptr<data_store> &d, const std::shared_ptr<hdl_ast_node> &l) {

    ver = "1.0";
    d_store = d;

    std::set<std::string> processed_peripherals;
    std::stack<std::shared_ptr<hdl_ast_node>> working_stack;
    working_stack.push(l);

    while(!working_stack.empty()){
        auto current_node = working_stack.top();
        working_stack.pop();
        if(!current_node->get_address().empty()){
            processed_peripherals.insert(current_node->get_type());
            auto parameters = current_node->get_parameters();
            auto periph = d_store->get_HDL_resource(current_node->get_type());
            if (!periph.has_value()) {
                spdlog::error("Definition of module {} while generating peripheral definitions", current_node->get_type());
                continue;
            }
            generate_peripheral(periph.value(), parameters, current_node->get_name());
        }
        if(current_node->get_proxy_ast() != nullptr){
            peripheral_definition_generator proxy_gen(d, current_node->get_proxy_ast());
            auto p = proxy_gen.get_peripheral_definitions();
            auto a_map = proxy_gen.get_alias_map();
            auto v_map = proxy_gen.get_variant_peripherals();
            a_map.erase(current_node->get_proxy_ast()->get_type());
            v_map.erase(current_node->get_proxy_ast()->get_type());
            p.erase(current_node->get_proxy_ast()->get_type());
            proxy_defs.push_back({p, a_map, v_map});
        }

        for(const auto& item:current_node->get_dependencies()) working_stack.push(item);
    }


    for(auto &def:proxy_defs){
        for(auto &p:def.definitions.items()){
            if(!peripheral_defs.contains(p.key())){
                peripheral_defs[p.key()] = p.value();
                if(def.alias_map.contains(p.key())){
                    alias_map[p.key()] = def.alias_map[p.key()];
                }
                if(def.variant_peripherals.contains(p.key())){
                    variant_peripherals[p.key()] = def.variant_peripherals[p.key()];
                }
            }
        }
    }

    std::vector<nlohmann::json> periph_buffer;
    int i = 1;
    for(auto &item:peripheral_defs){
        item["id"] = i;
        periph_buffer.push_back(item);
        i++;
    }
    peripheral_defs = periph_buffer;
}

void peripheral_definition_generator::generate_peripheral(const HDL_Resource &res, Parameters_map &parameters, const std::string& inst_name) {

    nlohmann::json specs;

    if(peripheral_defs.contains(res.getName())) return;
    std::string periph_name = res.getName();

    if(res.get_documentation().is_aliased()){
        periph_name= res.get_documentation().get_alias();
        alias_map[res.getName()] = periph_name;
    }

    specs["version"] = ver;

    auto mod_doc = res.get_documentation();

    if(mod_doc.is_variant()){
        std::string variant = parameters.get(mod_doc.get_variant_parameter())->get_string_value();
        periph_name += "_" + variant;
        variant_peripherals[inst_name] = periph_name;
        specs["registers"] = generate_variant_module_registers(mod_doc.get_registers(), parameters, variant);
    }else if(mod_doc.is_parametric()) {
        specs["registers"] = generate_parametric_module_registers(mod_doc.get_registers(), parameters);
    } else{
        specs["registers"] = generate_simple_module_registers(mod_doc.get_registers());
    }

    specs["name"] = periph_name;
    peripheral_defs[periph_name] = specs;

}

std::vector<nlohmann::json>
peripheral_definition_generator::generate_simple_module_registers(const std::vector<register_documentation> &r) {
    std::vector<nlohmann::json> ret;
    for(auto &doc:r){

        nlohmann::json reg;
        reg["ID"] = doc.get_name();
        reg["register_name"] = doc.get_name();
        reg["description"] = doc.get_description();
        std::string dir;
        if(doc.get_read_allowed())
            dir += "R";
        if(doc.get_write_allowed())
            dir += "W";
        reg["direction"] = dir;


        std::ostringstream off;
        off<< "0x" << std::hex << doc.get_offset();
        reg["offset"] = off.str();


        std::vector<nlohmann::json> fields = {};
        for(auto &item:doc.get_fields()){
            fields.push_back(generate_field(item, {}));
        }
        reg["fields"] = fields;
        ret.push_back(reg);
    }
    return ret;
}

std::vector<nlohmann::json>
peripheral_definition_generator::generate_parametric_module_registers(
        const std::vector<register_documentation> &r,
        const Parameters_map &parameters
) {
    std::vector<nlohmann::json> ret;
    for(int i = 0; i<r.size(); i++){

        auto doc = r[i];

        nlohmann::json reg;
        reg["ID"] = doc.get_name();
        reg["register_name"] = doc.get_name();
        reg["description"] = doc.get_description();
        std::string dir;
        if(doc.get_read_allowed())
            dir += "R";
        if(doc.get_write_allowed())
            dir += "W";
        reg["direction"] = dir;

        std::vector<nlohmann::json> fields = {};
        for(int j = 0; j<doc.get_fields().size(); j++){
            auto item = doc.get_fields()[j];
            auto f = generate_field(item, parameters);
            f["order"] = j;
            fields.push_back(f);
        }
        reg["fields"] = fields;
        reg["n_registers"] = doc.get_n_regs();
        reg["order"] = i;
        ret.push_back(reg);

    }
    return ret;
}



std::vector<nlohmann::json>
peripheral_definition_generator::generate_variant_module_registers(const std::vector<register_documentation> &r,
                                                                   const Parameters_map &parameters,
                                                                   const std::string &variant) {
    std::vector<nlohmann::json> ret;

    int order = 0;
    for(int i = 0; i<r.size(); i++){

        auto doc = r[i];

        if(doc.get_variants().contains(variant)){
            nlohmann::json reg;
            reg["ID"] = doc.get_name();
            reg["register_name"] = doc.get_name();
            reg["description"] = doc.get_description();
            std::string dir;
            if(doc.get_read_allowed())
                dir += "R";
            if(doc.get_write_allowed())
                dir += "W";
            reg["direction"] = dir;

            std::vector<nlohmann::json> fields = {};
            for(int j = 0; j<doc.get_fields().size(); j++){
                auto item = doc.get_fields()[j];
                auto f = generate_field(item, parameters);
                f["order"] = j;
                fields.push_back(f);
            }
            reg["fields"] = fields;
            reg["n_registers"] = doc.get_n_regs();
            reg["order"] = order;
            ret.push_back(reg);
            order++;
        }


    }
    return ret;
}


nlohmann::json peripheral_definition_generator::generate_field(field_documentation &doc, const Parameters_map &parameters) {
    nlohmann::json ret;
    ret["name"] = doc.get_name();
    ret["description"] = doc.get_description();
    ret["offset"] = doc.get_starting_position();
    ret["length"] = doc.get_length();
    if(!doc.get_n_fields().empty()){
        ret["n_fields"] = doc.get_n_fields();
    }
    return ret;
}

void peripheral_definition_generator::write_definition_file(const std::string &path) {

    nlohmann::json periph_obj;
    periph_obj["peripherals"] = peripheral_defs;
    std::string str = periph_obj.dump();
    std::ofstream ss(path);
    ss<<str;
    ss.close();
}

std::string peripheral_definition_generator::get_definition_string() {
    return peripheral_defs.dump();
}
