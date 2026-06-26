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

#include "frontend/analysis/documentation_analyzer.hpp"




documentation_analyzer::documentation_analyzer(const std::vector<std::string> &comments) {
    raw_documentation_comments = comments;
}



void documentation_analyzer::process_documentation(Parameters_map parameters) {
    parameters_dict = std::move(parameters);
    std::vector<nlohmann::json> documentation_comments;

    for(auto &content:raw_documentation_comments){
        if (!(content.contains('{') && content.contains('}'))) continue;
        nlohmann::json obj;
        std::istringstream ss(content);
        try {
            ss >> obj;
        } catch (nlohmann::json::parse_error& e) {
            spdlog::error("A malformed json string was found while parsing documentation comments in file: {}", path);
            exit(2);
        }

        documentation_comments.push_back(obj);
    }

    for(auto &obj:documentation_comments){
        analyze_documentation_object(obj);
    }
}


void documentation_analyzer::analyze_documentation_object(nlohmann::json &obj) {
    std::string type = obj["type"];
    if(type == "peripheral") {
        analyze_peripheral(obj);
    } else if(type == "parametric_peripheral"){
        analyze_parametric_peripheral(obj);
    } else if(type == "variant_peripheral"){
        analyze_variant_peripheral(obj);
    } else if(type == "processor_instance"){
        analyze_processor_instance(obj);
    } else if(type == "channel_groups"){
        analyze_channel_groups(obj);
    }
}

void documentation_analyzer::analyze_processor_instance(nlohmann::json &obj) {
    processor_instance i(obj["name"]);
    i.set_target(obj["target"]);
    for(auto &item:obj["dma_io"]){
        io tmp_io;
        tmp_io.name = item["name"];
        std::string s_addr = item["address"];
        tmp_io.address = std::stoul(s_addr);
        if(item["type"]=="input"){
            tmp_io.type = input;
        } else if(item["type"]=="output"){
            tmp_io.type = output;
        } else if(item["type"]=="memory"){
            tmp_io.type = memory;
        }
        i.add_io(tmp_io);
    }
    std::string t = obj["parent"];

    i.set_address(obj["address"]["parameter"]);
    if(obj["address"].contains("index")){
        int idx = obj["address"]["index"];
        i.set_address_idx(idx);
    }

    processors.insert({t,i});
}


void documentation_analyzer::analyze_peripheral(nlohmann::json &obj) {
    module_documentation mod_doc;
    std::string str_n = obj["name"];
    if(obj.contains("alias")){
        mod_doc.set_alias(obj["alias"]);
    }
    mod_doc.set_name(str_n);
    for(auto &item : obj["registers"]){
        std::string off =item["offset"];
        uint32_t offset = std::stoul(off, nullptr, 0);
        std::string dir = item["direction"];
        bool read_allowed = dir.find('R') != std::string::npos;
        bool write_allowed = dir.find('W') != std::string::npos;
        register_documentation reg_doc(item["name"], offset, item["description"], read_allowed, write_allowed);

        if(item.contains("fields")){
            for(auto &f_obj:item["fields"]){
                auto field_doc = analyze_register_field(f_obj, false);
                reg_doc.add_field(field_doc);
            }
        }
        mod_doc.add_register(reg_doc);
    }
    modules_doc[str_n] = mod_doc;
}

void documentation_analyzer::analyze_parametric_peripheral(nlohmann::json &obj) {
    module_documentation mod_doc;
    std::string str_n = obj["name"];
    if(obj.contains("alias")){
        mod_doc.set_alias(obj["alias"]);
    }
    mod_doc.set_name(str_n);
    mod_doc.set_parametric();


    for(auto &item : obj["registers"]){

        std::string name = item["name"];
        std::string description = item["description"];
        std::string dir = item["direction"];
        bool read_allowed = dir.find('R') != std::string::npos;
        bool write_allowed = dir.find('W') != std::string::npos;

        std::vector<std::string> n_regs;
        if(item.contains("n_regs"))
            n_regs = item["n_regs"];
        else
            n_regs = {};


        register_documentation reg_doc(item["name"], item["description"], read_allowed, write_allowed, n_regs);

        if(item.contains("fields")){
            for(auto &f_obj:item["fields"]){
                auto field_doc = analyze_register_field(f_obj, true);
                reg_doc.add_field(field_doc);
            }
        }
        mod_doc.add_register(reg_doc);
    }
    modules_doc[str_n] = mod_doc;
}



field_documentation documentation_analyzer::analyze_register_field(nlohmann::json &obj, bool parametric) {
    std::string name = obj["name"];
    std::string desc = obj["description"];
    uint8_t start_pos = obj["start_position"];
    uint8_t length = obj["length"];
    field_documentation doc(name, desc, start_pos, length);
    if(parametric){
        if(obj.contains("n_fields")){
            std::vector<std::string> n_fields = obj["n_fields"];
            doc.set_n_fields(n_fields);
        }
    }
    return doc;
}

std::unordered_map<std::string, module_documentation> documentation_analyzer::get_modules_documentation() {
    return modules_doc;
}


std::unordered_map<std::string, processor_instance> documentation_analyzer::get_processors_documentation() {
    return processors;
}

void documentation_analyzer::analyze_channel_groups(nlohmann::json &obj) {
    std::vector<channel_group> g_vect;
    for(auto &item:obj["groups"]){
        channel_group g;
        g.set_name(item["name"]);
        g.set_default(item["default"]);
        g.set_channels(item["channels"]);
        g_vect.push_back(g);
    }
    std::string target = obj["target"];
    groups.insert({target,g_vect});
}

void documentation_analyzer::analyze_variant_peripheral(nlohmann::json &obj) {
    module_documentation mod_doc;
    std::string str_n = obj["name"];
    if(obj.contains("alias")){
        mod_doc.set_alias(obj["alias"]);
    }
    mod_doc.set_name(str_n);
    mod_doc.set_variant();

    mod_doc.set_variant_parameter(obj["variant_parameter"]);

    for(auto &item : obj["registers"]){

        std::string name = item["name"];
        std::string description = item["description"];
        std::string dir = item["direction"];
        bool read_allowed = dir.find('R') != std::string::npos;
        bool write_allowed = dir.find('W') != std::string::npos;

        std::vector<std::string> n_regs;
        if(item.contains("n_regs"))
            n_regs = item["n_regs"];
        else
            n_regs = {};


        register_documentation reg_doc(item["name"], item["description"], read_allowed, write_allowed, n_regs);

        std::unordered_set<std::string> variants = item["variants"];
        reg_doc.set_variant(variants);

        if(item.contains("fields")){
            for(auto &f_obj:item["fields"]){
                auto field_doc = analyze_register_field(f_obj, true);
                reg_doc.add_field(field_doc);
            }
        }
        mod_doc.add_register(reg_doc);
    }
    modules_doc[str_n] = mod_doc;
}
