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

#include <spdlog/spdlog.h>

namespace {
using live_def = preprocessor::macro_definitions_map::mapped_type;

std::string rtrim_copy(const std::string &in) {
    const auto end = in.find_last_not_of(" \t\r\n");
    if (end == std::string::npos) return "";
    return in.substr(0, end + 1);
}

macro_table::candidate_info measure_candidate(
    const std::string &path,
    const live_def &def) {
    macro_table::candidate_info info;
    info.path = path;
    if (std::holds_alternative<preprocessor::function_macro>(def)) {
        const auto &macro = std::get<preprocessor::function_macro>(def);
        info.is_function = true;
        info.total_params = static_cast<int>(macro.arguments.size());
        for (const auto &arg : macro.arguments) {
            if (!arg.has_default) ++info.required_params;
        }
    }
    return info;
}

// A call-shaped use (observed arity k) can only expand a function macro
// whose parameter list accepts k arguments. Bare uses carry no shape
// information and never filter.
bool arity_compatible(const macro_table::candidate_info &candidate,
                      const std::set<int> &arities) {
    if (arities.empty() || !candidate.is_function) {
        return arities.empty();
    }
    for (int arity : arities) {
        if (arity < candidate.required_params || arity > candidate.total_params) return false;
    }
    return true;
}

} // namespace

stored_macro_def macro_to_stored(
    const std::string &name,
    const live_def &def) {
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

live_def macro_to_live(const stored_macro_def &stored) {
    if (!stored.is_function) return stored.value;
    preprocessor::function_macro macro;
    macro.value = stored.value;
    for (const auto &arg : stored.args) {
        macro.arguments.push_back({arg.name, arg.default_value, arg.has_default});
    }
    return macro;
}

std::string macro_canonical_body(
    const live_def &def) {
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

std::string macro_table::describe_candidate(const candidate_info &candidate) {
    if (!candidate.is_function) return candidate.path + " (simple)";
    return candidate.path + " (" + std::to_string(candidate.total_params) + " params, " +
           std::to_string(candidate.required_params) + " required)";
}

macro_table::verdict macro_table::judge(const std::string &name,
                                         const std::set<int> &arities,
                                         const std::string &requester) const {
    verdict result;
    const auto bodies_it = bodies_.find(name);
    if (bodies_it == bodies_.end()) return result;
    const auto live_it = live_.find(name);
    if (live_it == live_.end() || live_it->second.empty()) return result;

    bool self_defined = false;
    std::vector<std::pair<candidate_info, live_def>> scored;
    for (const auto &[file, def] : live_it->second) {
        if (!requester.empty() && file == requester) {
            self_defined = true;
            continue;
        }
        scored.emplace_back(measure_candidate(file, def), def);
    }
    if (self_defined) {
        // The file defines this macro itself: injecting anything would invert
        // include guards (same guard name copy-pasted across headers is
        // common) and rewrite use-before-define order. The file evaluates
        // exactly as written; a use the file cannot satisfy itself is a
        // use-before-local-define, which no injection can satisfy (forward
        // references are illegal).
        result.state = verdict::kind::shadowed;
        auto self_live = live_it->second.find(requester);
        if (self_live != live_it->second.end()) {
            result.candidates.push_back(measure_candidate(requester, self_live->second));
        }
        return result;
    }
    if (scored.empty()) {
        return result;
    }
    // Hard filter: keep definitions whose parameter lists accept every
    // observed call-site arity.
    std::vector<std::pair<candidate_info, live_def>> compatible;
    for (const auto &entry : scored) {
        if (arity_compatible(entry.first, arities)) compatible.push_back(entry);
    }
    if (compatible.empty()) {
        result.state = verdict::kind::mismatch;
        for (const auto &entry : scored) result.candidates.push_back(entry.first);
        return result;
    }
    std::set<std::string> distinct;
    for (const auto &entry : compatible) {
        distinct.insert(bodies_it->second.at(entry.first.path));
    }
    if (distinct.size() == 1) {
        result.state = verdict::kind::unique;
        result.definition = compatible.front().second;
        result.candidates.push_back(compatible.front().first);
        return result;
    }
    // More than one compatible definition with different bodies: genuinely
    // ambiguous. A 3-argument call against (3/3) and (3/5) is satisfiable by
    // both, so no pick is honest here — only an arity no other definition
    // accepts (e.g. the full 5) resolves unambiguously, and that already
    // reduced to the single-compatible case above.
    result.state = verdict::kind::conflict;
    for (const auto &entry : compatible) result.candidates.push_back(entry.first);
    return result;
}

macro_table::injection macro_table::build_injection(const preprocessor::undefined_uses_map &needs,
                                                      const std::string &requester) const {
    injection result;
    for (const auto &[name, arities] : needs) {
        auto judgement = judge(name, arities, requester);
        switch (judgement.state) {
            case verdict::kind::unique:
                result.definitions[name] = judgement.definition;
                result.resolved.push_back(name);
                break;
            case verdict::kind::conflict:
                for (const auto &candidate : judgement.candidates) {
                    result.conflicts[name].push_back(candidate.path);
                }
                break;
            case verdict::kind::mismatch: {
                for (const auto &candidate : judgement.candidates) {
                    result.mismatches[name].push_back(candidate.path);
                }
                break;
            }
            case verdict::kind::absent:
                result.unresolvable.push_back(name);
                break;
            case verdict::kind::shadowed:
                break;
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
