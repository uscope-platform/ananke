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

#include "frontend/analysis/system_verilog/documentation_analyzer.hpp"

#include <charconv>

namespace {
    bool json_string(const nlohmann::json &obj, const char *key, std::string &out, const std::string &path) {
        auto it = obj.find(key);
        if (it == obj.end() || !it->is_string()) {
            spdlog::warn("Missing or invalid field '{}' in documentation comment in file: {}", key, path);
            return false;
        }
        out = it->get<std::string>();
        return true;
    }

    std::optional<uint32_t> parse_u32_auto(const std::string &s) {
        if (s.empty()) return std::nullopt;
        std::string_view sv = s;
        unsigned base = 10;
        if (sv.size() > 2 && sv[0] == '0' && (sv[1] == 'x' || sv[1] == 'X')) { base = 16; sv = sv.substr(2); }
        else if (sv.size() > 1 && sv[0] == '0') { base = 8; sv = sv.substr(1); }
        uint32_t val = 0;
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), val, base);
        if (ec != std::errc() || ptr != sv.data() + sv.size()) return std::nullopt;
        return val;
    }
}

documentation_analyzer::documentation_analyzer(const std::vector<std::string> &comments) {
    raw_documentation_comments = comments;
}



void documentation_analyzer::process_documentation() {
    std::vector<nlohmann::json> documentation_comments;
    // At most one warning per file: ordinary prose doc-comments (UVM
    // headers are full of them) may contain braces but are never
    // annotations, so they skip silently below; only plausible
    // annotations that fail to parse deserve a (single) diagnostic.
    bool warned_once = false;
    auto warn_once = [&](const std::string &detail) {
        if (warned_once) return;
        warned_once = true;
        spdlog::warn("Skipping malformed documentation comment(s) in file: {} ({})",
                     path, detail);
    };

    for(auto &content:raw_documentation_comments){
        // Annotations are JSON objects dispatched on their "type" member
        // (see analyze_documentation_object); prose never carries that
        // marker, so this prefilter silences the brace-containing
        // prose comments that used to warn once per comment.
        if (content.find("\"type\"") == std::string::npos) continue;
        // Annotations may be preceded by prose: parse from the first
        // brace instead of requiring the comment to start with JSON
        // (trailing content after the value was and is tolerated).
        auto brace = content.find('{');
        if (brace == std::string::npos) continue;
        nlohmann::json obj;
        std::istringstream ss(content.substr(brace));
        try {
            ss >> obj;
        } catch (nlohmann::json::parse_error& e) {
            warn_once(e.what());
            continue;
        }
        if (!obj.is_object()) {
            warn_once("not a JSON object");
            continue;
        }

        documentation_comments.push_back(obj);
    }

    for(auto &obj:documentation_comments){
        analyze_documentation_object(obj);
    }
}


void documentation_analyzer::analyze_documentation_object(nlohmann::json &obj) {
    std::string type;
    if (!json_string(obj, "type", type, path)) return;
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
    std::string name, target, parent;
    if (!json_string(obj, "name", name, path)) return;
    if (!json_string(obj, "target", target, path)) return;
    if (!json_string(obj, "parent", parent, path)) return;
    processor_instance i(name);
    i.set_target(target);

    if (obj.contains("dma_io") && obj["dma_io"].is_array()) {
        for(auto &item:obj["dma_io"]){
            io tmp_io;
            std::string io_name, io_addr, io_type;
            if (!json_string(item, "name", io_name, path)) continue;
            if (!json_string(item, "address", io_addr, path)) continue;
            auto parsed = parse_u32_auto(io_addr);
            if (!parsed.has_value()) {
                spdlog::warn("Invalid address '{}' in documentation comment in file: {}, skipping io", io_addr, path);
                continue;
            }
            tmp_io.name = io_name;
            tmp_io.address = parsed.value();
            if (!json_string(item, "type", io_type, path)) continue;
            if(io_type=="input"){
                tmp_io.type = input;
            } else if(io_type=="output"){
                tmp_io.type = output;
            } else if(io_type=="memory"){
                tmp_io.type = memory;
            }
            i.add_io(tmp_io);
        }
    }

    if (obj.contains("address") && obj["address"].is_object() && obj["address"].contains("parameter") && obj["address"]["parameter"].is_string()) {
        i.set_address(obj["address"]["parameter"].get<std::string>());
        if(obj["address"].contains("index") && obj["address"]["index"].is_number_integer()){
            i.set_address_idx(obj["address"]["index"].get<int>());
        }
    }

    processors.insert({parent,i});
}


void documentation_analyzer::analyze_peripheral(nlohmann::json &obj) {
    module_documentation mod_doc;
    std::string str_n;
    if (!json_string(obj, "name", str_n, path)) return;
    if(obj.contains("alias") && obj["alias"].is_string()){
        mod_doc.set_alias(obj["alias"].get<std::string>());
    }
    mod_doc.set_name(str_n);
    if (obj.contains("registers") && obj["registers"].is_array()) {
        for(auto &item : obj["registers"]){
            std::string off_s, dir, reg_name, reg_desc;
            if (!json_string(item, "offset", off_s, path)) continue;
            if (!json_string(item, "direction", dir, path)) continue;
            if (!json_string(item, "name", reg_name, path)) continue;
            if (!json_string(item, "description", reg_desc, path)) continue;
            auto parsed_off = parse_u32_auto(off_s);
            if (!parsed_off.has_value()) {
                spdlog::warn("Invalid register offset '{}' in documentation comment in file: {}, skipping register", off_s, path);
                continue;
            }
            uint32_t offset = parsed_off.value();
            bool read_allowed = dir.find('R') != std::string::npos;
            bool write_allowed = dir.find('W') != std::string::npos;
            register_documentation reg_doc(reg_name, offset, reg_desc, read_allowed, write_allowed);

            if(item.contains("fields") && item["fields"].is_array()){
                for(auto &f_obj:item["fields"]){
                    auto field_doc = analyze_register_field(f_obj, false);
                    reg_doc.add_field(field_doc);
                }
            }
            mod_doc.add_register(reg_doc);
        }
    }
    modules_doc[str_n] = mod_doc;
}

void documentation_analyzer::analyze_parametric_peripheral(nlohmann::json &obj) {
    module_documentation mod_doc;
    std::string str_n;
    if (!json_string(obj, "name", str_n, path)) return;
    if(obj.contains("alias") && obj["alias"].is_string()){
        mod_doc.set_alias(obj["alias"].get<std::string>());
    }
    mod_doc.set_name(str_n);
    mod_doc.set_parametric();


    if (obj.contains("registers") && obj["registers"].is_array()) {
        for(auto &item : obj["registers"]){

            std::string name, description, dir;
            if (!json_string(item, "name", name, path)) continue;
            if (!json_string(item, "description", description, path)) continue;
            if (!json_string(item, "direction", dir, path)) continue;
            bool read_allowed = dir.find('R') != std::string::npos;
            bool write_allowed = dir.find('W') != std::string::npos;

            std::vector<std::string> n_regs;
            if(item.contains("n_regs") && item["n_regs"].is_array())
                n_regs = item["n_regs"].get<std::vector<std::string>>();
            else
                n_regs = {};


            register_documentation reg_doc(name, description, read_allowed, write_allowed, n_regs);

            if(item.contains("fields") && item["fields"].is_array()){
                for(auto &f_obj:item["fields"]){
                    auto field_doc = analyze_register_field(f_obj, true);
                    reg_doc.add_field(field_doc);
                }
            }
            mod_doc.add_register(reg_doc);
        }
    }
    modules_doc[str_n] = mod_doc;
}



field_documentation documentation_analyzer::analyze_register_field(nlohmann::json &obj, bool parametric) {
    std::string name, desc;
    uint8_t start_pos = 0;
    uint8_t length = 0;
    if (!json_string(obj, "name", name, path)) return field_documentation();
    if (!json_string(obj, "description", desc, path)) return field_documentation();
    if (obj.contains("start_position") && obj["start_position"].is_number_integer()) start_pos = obj["start_position"].get<uint8_t>();
    if (obj.contains("length") && obj["length"].is_number_integer()) length = obj["length"].get<uint8_t>();
    field_documentation doc(name, desc, start_pos, length);
    if(parametric){
        if(obj.contains("n_fields") && obj["n_fields"].is_array()){
            std::vector<std::string> n_fields = obj["n_fields"].get<std::vector<std::string>>();
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
    if (obj.contains("groups") && obj["groups"].is_array()) {
        for(auto &item:obj["groups"]){
            channel_group g;
            std::string name;
            if (!json_string(item, "name", name, path)) continue;
            g.set_name(name);
            if (item.contains("default") && item["default"].is_boolean()) g.set_default(item["default"].get<bool>());
            if (item.contains("channels") && item["channels"].is_array())
                g.set_channels(item["channels"].get<std::vector<std::string>>());
            g_vect.push_back(g);
        }
    }
    std::string target;
    if (!json_string(obj, "target", target, path)) return;
    groups.insert({target,g_vect});
}

void documentation_analyzer::analyze_variant_peripheral(nlohmann::json &obj) {
    module_documentation mod_doc;
    std::string str_n;
    if (!json_string(obj, "name", str_n, path)) return;
    if(obj.contains("alias") && obj["alias"].is_string()){
        mod_doc.set_alias(obj["alias"].get<std::string>());
    }
    mod_doc.set_name(str_n);
    mod_doc.set_variant();

    std::string variant_parameter;
    if (!json_string(obj, "variant_parameter", variant_parameter, path)) return;
    mod_doc.set_variant_parameter(variant_parameter);

    if (obj.contains("registers") && obj["registers"].is_array()) {
        for(auto &item : obj["registers"]){

            std::string name, description, dir;
            if (!json_string(item, "name", name, path)) continue;
            if (!json_string(item, "description", description, path)) continue;
            if (!json_string(item, "direction", dir, path)) continue;
            bool read_allowed = dir.find('R') != std::string::npos;
            bool write_allowed = dir.find('W') != std::string::npos;

            std::vector<std::string> n_regs;
            if(item.contains("n_regs") && item["n_regs"].is_array())
                n_regs = item["n_regs"].get<std::vector<std::string>>();
            else
                n_regs = {};


            register_documentation reg_doc(name, description, read_allowed, write_allowed, n_regs);

            if (item.contains("variants") && item["variants"].is_array()) {
                std::unordered_set<std::string> variants = item["variants"].get<std::unordered_set<std::string>>();
                reg_doc.set_variant(variants);
            }

            if(item.contains("fields") && item["fields"].is_array()){
                for(auto &f_obj:item["fields"]){
                    auto field_doc = analyze_register_field(f_obj, true);
                    reg_doc.add_field(field_doc);
                }
            }
            mod_doc.add_register(reg_doc);
        }
    }
    modules_doc[str_n] = mod_doc;
}
