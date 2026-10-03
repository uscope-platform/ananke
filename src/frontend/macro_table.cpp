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

#include "frontend/macro_table.hpp"

namespace {
std::string rtrim_copy(const std::string &in) {
    const auto end = in.find_last_not_of(" \t\r\n");
    if (end == std::string::npos) return "";
    return in.substr(0, end + 1);
}
} // namespace

stored_macro_def macro_to_stored(
    const std::string &name,
    const preprocessor::macro_definitions_map::mapped_type &def) {
    stored_macro_def stored;
    stored.name = name;
    if (std::holds_alternative<std::string>(def)) {
        stored.is_function = false;
        stored.value = std::get<std::string>(def);
    } else {
        const auto &macro = std::get<preprocessor::function_macro>(def);
        stored.is_function = true;
        stored.value = macro.value;
        for (const auto &arg : macro.arguments) {
            stored.args.push_back({arg.name, arg.default_value, arg.has_default});
        }
    }
    return stored;
}

preprocessor::macro_definitions_map::mapped_type macro_to_live(const stored_macro_def &stored) {
    if (!stored.is_function) return stored.value;
    preprocessor::function_macro macro;
    macro.value = stored.value;
    for (const auto &arg : stored.args) {
        macro.arguments.push_back({arg.name, arg.default_value, arg.has_default});
    }
    return macro;
}

std::string macro_canonical_body(
    const preprocessor::macro_definitions_map::mapped_type &def) {
    if (std::holds_alternative<std::string>(def)) {
        return "S:" + rtrim_copy(std::get<std::string>(def));
    }
    const auto &macro = std::get<preprocessor::function_macro>(def);
    std::string out = "F:";
    for (const auto &arg : macro.arguments) {
        out += arg.name + "=" + arg.default_value + (arg.has_default ? "!" : "?") + ",";
    }
    out += ":" + rtrim_copy(macro.value);
    return out;
}

void macro_table::add_file_definitions(
    const std::string &path, const preprocessor::macro_definitions_map &defs) {
    // Drop this file's previous contributions first: re-parses replace,
    // they never accumulate (stale bodies must not linger as conflicts).
    remove_file(path);
    for (const auto &[name, def] : defs) {
        bodies_[name][path] = macro_canonical_body(def);
        live_[name][path] = def;
    }
}

void macro_table::remove_file(const std::string &path) {
    for (auto it = bodies_.begin(); it != bodies_.end();) {
        it->second.erase(path);
        if (it->second.empty()) it = bodies_.erase(it);
        else ++it;
    }
    for (auto it = live_.begin(); it != live_.end();) {
        it->second.erase(path);
        if (it->second.empty()) it = live_.erase(it);
        else ++it;
    }
}

macro_table::resolution macro_table::resolve(const std::string &name) const {
    resolution result;
    const auto bodies_it = bodies_.find(name);
    if (bodies_it == bodies_.end()) return result;
    const auto live_it = live_.find(name);
    if (live_it == live_.end() || live_it->second.empty()) return result;
    std::set<std::string> distinct;
    for (const auto &[file, body] : bodies_it->second) {
        (void)file;
        distinct.insert(body);
    }
    for (const auto &[file, ignored] : live_it->second) {
        (void)ignored;
        result.definers.push_back(file);
    }
    if (distinct.size() == 1) {
        result.state = resolution::status::unique;
        result.definition = live_it->second.begin()->second;
    } else {
        result.state = resolution::status::conflict;
    }
    return result;
}

macro_table::injection macro_table::build_injection(const std::set<std::string> &needs,
                                                      const std::string &requester) const {
    injection result;
    for (const auto &name : needs) {
        auto resolution = resolve(name);
        if (!requester.empty()) {
            bool self_defined = false;
            for (const auto &definer : resolution.definers) {
                if (definer == requester) {
                    self_defined = true;
                    break;
                }
            }
            // The file defines this macro itself: injecting it would invert
            // include guards and rewrite use-before-define order. Leave it
            // out so the file evaluates exactly as written.
            if (self_defined) continue;
        }
        if (resolution.state == resolution::status::unique) {
            result.definitions[name] = resolution.definition;
            result.resolved.push_back(name);
        } else if (resolution.state == resolution::status::conflict) {
            result.conflicts[name] = resolution.definers;
        } else {
            result.unresolvable.push_back(name);
        }
    }
    return result;
}

bool macro_table::empty() const {
    return bodies_.empty();
}

std::set<std::string> macro_table::names() const {
    std::set<std::string> result;
    for (const auto &[name, ignored] : bodies_) {
        (void)ignored;
        result.insert(name);
    }
    return result;
}

std::string macro_table::canonical_string() const {
    std::string out;
    for (const auto &[name, per_file] : bodies_) {
        out += "@" + name + "\n";
        for (const auto &[file, body] : per_file) {
            out += file + "\n" + body + "\n";
        }
    }
    return out;
}

std::map<std::string, std::string> macro_table::rendered_by_name() const {
    std::map<std::string, std::string> result;
    for (const auto &[name, per_file] : bodies_) {
        std::string rendered = name + "\n";
        for (const auto &[file, body] : per_file) {
            rendered += file + "\n" + body + "\n";
        }
        result[name] = rendered;
    }
    return result;
}
