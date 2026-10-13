// Copyright 2021 University of Nottingham Ningbo China
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

#include "data_model/Depfile/Depfile.hpp"
#include "data_model/include_config.hpp"

#include <cstring>
#include <spdlog/spdlog.h>
#include <sys/stat.h>

namespace {
// Replacement text for a `defines` dictionary value: strings as-is,
// numbers/booleans stringified, null = bare define ("1").
std::string define_value_text(const nlohmann::json &v) {
    if (v.is_string()) return v.get<std::string>();
    if (v.is_null()) return "1";
    return v.dump();
}
void parse_defines_section(const nlohmann::json &def, depfile_defines &out) {
    for (auto &[key, value] : def.items()) {
        out[key] = define_value_text(value);
    }
}
void parse_includes_section(const nlohmann::json &inc, includes_settings &out) {
    if (inc.contains("ignored")) {
        for (const auto &item : inc["ignored"]) {
            out.ignored.emplace_back(item.get<std::string>());
        }
    }
    if (inc.contains("deconfliction")) {
        for (auto &[key, value] : inc["deconfliction"].items()) {
            include_header_resolution_opt res;
            if (value.is_string()) {
                res.global_target = value.get<std::string>();
            } else if (value.is_object()) {
                for (auto &[scope, target] : value.items()) {
                    res.scoped_targets.emplace(
                        include_config::normalize_repo_path(scope),
                        target.get<std::string>());
                }
            }
            out.deconfliction.emplace(
                include_config::normalize_repo_path(key), std::move(res));
        }
    }
}
} // namespace

Depfile::Depfile(std::istream &in) {

    nlohmann::json file_content;
    try {
        in >> file_content;
        set_content(file_content);
    } catch (const std::exception &e) {
        validation_error = std::string("could not parse Depfile: ") + e.what();
    }
}

void Depfile::set_content(const nlohmann::json &file_content) {

    auto validation_err = depfile_validator::validate(file_content);
    if (validation_err.has_value()) {
        validation_error = "Depfile schema validation failed: " + validation_err.value();
        return;
    }
    auto gen = file_content["general"];
    general.project_name = gen["project_name"];
    if ( gen.contains("target_part"))
        general.target_part = gen["target_part"];
    if ( gen.contains("board"))
        general.board = gen["board"];
    if (!general.board && !general.target_part){
        spdlog::warn("The selected Depfile does not define a target");
    }
    general.synth_tl = gen["synth_tl"];
    general.sim_tl = gen["sim_tl"];
    if (gen.contains("sim_modules")) general.sim_modules = gen["sim_modules"];
    if (gen.contains("synth_modules")) general.synth_modules = gen["synth_modules"];
    if (gen.contains("include_paths")) general.include_paths = gen["include_paths"];
    if (gen.contains("sim_harness")) general.sim_harness = gen["sim_harness"];
    if (gen.contains("sim_defines")) general.sim_defines = gen["sim_defines"];
    if (file_content.contains("constraints")) {
        for(const auto& item : file_content["constraints"]){
            constraints.emplace_back(item);
        }
    }

    if (file_content.contains("excluded_modules")) excluded_modules = file_content["excluded_modules"];
    if (file_content.contains("defines") && file_content["defines"].is_object()) {
        parse_defines_section(file_content["defines"], defines);
    }
    if (file_content.contains("scripts")) {
        for (auto &scr:file_content["scripts"]) {
            script_specs s;
            s.name = scr["name"];
            s.type = scr["type"];

            if (scr.contains("function_mode")) s.function_mode = scr["function_mode"];
            else s.function_mode = false;

            if (scr.contains("include_products")) s.include_products = scr["include_products"];
            else s.include_products = false;
            if (scr.contains("products_type")) s.products_type = scr["products_type"];
            if (scr.contains("arguments") && !scr["arguments"].empty()) {
                if (scr["arguments"][0].is_string()) {
                    s.positional_arguments =  scr["arguments"];
                } else {
                    for (auto &arg:scr["arguments"]) {
                        s.named_arguments.emplace_back(arg["name"], arg["value"]);
                    }

                }
            }
            scripts.emplace_back(s);
        }
    }

    if (file_content.contains("bus")) {
        for (auto &[bus_name, bus_obj]:file_content["bus"].items()) {
            bus.insert({
                bus_name,
                {bus_obj["type"], bus_obj["starting_module"], bus_obj["bus_interface"]}
            });
        }
    }
    if (file_content.contains("deconfliction")) {
        deconfliction = file_content.at("deconfliction");
        // Guard against the silent-misconfiguration trap: top-level
        // `deconfliction` only applies to module names. A key that looks
        // like an include file (subpath or HDL/header extension — never a
        // legal module name) almost certainly belongs in
        // `includes.deconfliction` and would otherwise be ignored without a
        // word. Behavior is unchanged; this just points at the right knob.
        for (const auto &[key, ignored] : deconfliction) {
            (void)ignored;
            bool file_like = key.find('/') != std::string::npos ||
                             key.find('\\') != std::string::npos;
            for (const char *ext : {".svh", ".sv", ".vh", ".v", ".h"}) {
                if (key.size() >= std::strlen(ext) &&
                    key.compare(key.size() - std::strlen(ext), std::strlen(ext), ext) == 0) {
                    file_like = true;
                    break;
                }
            }
            if (file_like) {
                spdlog::warn("Depfile top-level deconfliction entry '{}' looks like an include file: "
                             "include resolution uses the `includes.deconfliction` section instead",
                             key);
            }
        }
    }
    if (file_content.contains("includes")) {
        parse_includes_section(file_content["includes"], includes);
    }
    if (file_content.contains("verilator")) {
        const auto &v = file_content["verilator"];
        if (v.contains("cflags")) verilator.cflags = v["cflags"];
        if (v.contains("waivers")) verilator.waivers = v["waivers"];
        if (v.contains("make_args")) verilator.make_args = v["make_args"];
        if (v.contains("autoflush")) verilator.autoflush = v["autoflush"];
        if (v.contains("extra_args")) verilator.extra_args = v["extra_args"];
    }
}



std::vector<Script> Depfile::get_scripts() const {
    std::vector<Script> retval;
    for(const auto& item : scripts){
        retval.emplace_back(item);
    }
    return retval;
}



std::optional<bus_specs> Depfile::get_bus_section(const std::string &bus_type) {
    if (bus.contains(bus_type)) return bus.at(bus_type);
    else return std::nullopt;
}

bool Depfile::is_module_excluded(const std::string &s) {
    return std::ranges::contains(excluded_modules, s);
}

void Depfile::add_excluded_module(const std::string &s) {
    excluded_modules.push_back(s);
}

Depfile::Depfile() {
    excluded_modules = std::vector<std::string>();
}

