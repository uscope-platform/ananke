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

#include "frontend/analysis/system_verilog/sv_semantic_checker.hpp"

#include <spdlog/spdlog.h>

#include <map>
#include <set>

#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_union_type.hpp"

namespace {

void record_violation(
    const std::string &file, unsigned int line, std::vector<semantic_violation> &violations, const std::string &message) {
    spdlog::error("{}:{}: error: {}", file, line, message);
    violations.push_back(semantic_violation{semantic_error_code::illegal_driver, message});
}

// One write to a module-scope name by the tracked logic.
struct logic_write {
    std::string name;
    bool whole = true;          // no index or select: covers the entire variable
    unsigned int line = 0;
    int process = -1;           // -1: continuous assignment, otherwise the block's position
    bool single_writer = false; // written from an always_comb / always_latch / always_ff (single-writer rule)
    bool initial = false;       // written from an initial block
};

struct collector {
    std::vector<logic_write> writes;
    std::set<std::string> shadowed;   // names declared inside the current block
    int process = -1;
    bool single_writer = false;
    bool initial = false;
    unsigned int line = 0;

    void add_lvalue(const hdl_lvalue &lv) {
        const auto &targets = lv.get_targets();
        const auto &indices = lv.get_indices();
        const auto &selects = lv.get_member_selects();
        for (size_t i = 0; i < targets.size(); ++i) {
            const auto &t = targets[i];
            // Hierarchical and package-qualified targets name something outside this module's scope.
            if (!t.get_instance().empty() || !t.get_package_prefix().empty()) continue;
            if (shadowed.contains(t.get_name())) continue;
            logic_write w;
            w.name = t.get_name();
            w.whole = !(i < indices.size() && indices[i]) &&
                      !(i < selects.size() && (selects[i].has_bounds() || selects[i].dynamic_select));
            w.line = line;
            w.process = process;
            w.single_writer = single_writer;
            w.initial = initial;
            writes.push_back(std::move(w));
        }
    }

    void add_body(const hdl_statement_body &body) {
        for (const auto &stmt : body) {
            if (!stmt) continue;
            if (auto a = std::dynamic_pointer_cast<hdl_assignment_statement>(stmt)) {
                add_lvalue(a->get_lvalue());
            } else if (auto c = std::dynamic_pointer_cast<hdl_conditional_statement>(stmt)) {
                for (const auto &b : c->get_branches()) add_body(b.body);
                add_body(c->get_else_body());
            } else if (auto l = std::dynamic_pointer_cast<hdl_loop_statement>(stmt)) {
                // A loop variable may be declared in the loop header, which shadows the module's.
                if (l->get_init()) shadowed.insert(l->get_init()->get_name());
                add_body(l->get_body());
            } else if (auto w = std::dynamic_pointer_cast<hdl_while_statement>(stmt)) {
                add_body(w->get_body());
            } else if (auto r = std::dynamic_pointer_cast<hdl_repeat_statement>(stmt)) {
                add_body(r->get_body());
            } else if (auto d = std::dynamic_pointer_cast<hdl_do_while_statement>(stmt)) {
                add_body(d->get_body());
            }
        }
    }
};

// A whole-variable write collides with any other write; two partial ones are not analysed further.
bool overlap(const logic_write &a, const logic_write &b) { return a.whole || b.whole; }

} // namespace

bool sv_semantic_checker::check_module_logic(const std::string &file, const hdl_resource_statement &module) {
    if (opts_.disable_semantic_checks) return false;

    std::map<std::string, std::shared_ptr<hdl_signal_declaration>> signals;
    std::set<std::string> parameters;
    for (const auto &stmt : module.get_statements()) {
        if (auto decl = std::dynamic_pointer_cast<hdl_signal_declaration>(stmt)) {
            signals.emplace(decl->get_name(), decl);
        } else if (auto param = std::dynamic_pointer_cast<HDL_parameter>(stmt)) {
            // A struct or union initialised at module scope is filed as a parameter although it is a variable.
            auto type = param->get_type();
            const bool composite_variable = type && (type->is<HDL_struct_type>() || type->is<HDL_union_type>());
            if (!composite_variable) parameters.insert(param->get_name());
        }
    }

    collector col;
    int process = 0;
    for (const auto &stmt : module.get_statements()) {
        if (auto driver = std::dynamic_pointer_cast<hdl_continuous_assignment>(stmt)) {
            col.process = -1;
            col.single_writer = false;
            col.initial = false;
            col.line = static_cast<unsigned int>(driver->get_line_n());
            col.shadowed.clear();
            col.add_lvalue(driver->get_lvalue());
        } else if (auto block = std::dynamic_pointer_cast<hdl_procedural_block_statement>(stmt)) {
            col.process = process++;
            const auto kind = block->get_kind();
            col.single_writer = kind == procedural_block_kind_t::always_comb ||
                                kind == procedural_block_kind_t::always_latch ||
                                kind == procedural_block_kind_t::always_ff;
            col.initial = kind == procedural_block_kind_t::initial;
            col.line = static_cast<unsigned int>(block->get_line_n());
            col.shadowed = std::set<std::string>(block->get_local_names().begin(), block->get_local_names().end());
            col.add_body(block->get_body());
            for (const auto &written : block->get_written()) col.add_lvalue(written);
        }
    }

    bool fired = false;
    auto report = [&](unsigned int line, const std::string &message) {
        record_violation(file, line, violations_, message);
        fired = true;
    };

    std::map<std::string, std::vector<const logic_write *>> by_name;
    for (const auto &w : col.writes) by_name[w.name].push_back(&w);

    for (const auto &[name, writes] : by_name) {
        if (parameters.contains(name)) {
            report(writes.front()->line, "cannot assign to parameter '" + name + "'");
            continue;
        }
        auto it = signals.find(name);
        if (it == signals.end()) continue;

        if (it->second->get_storage() == signal_storage_t::net) {
            for (const auto *w : writes) {
                if (w->process < 0) continue;
                report(w->line, "net '" + name + "' cannot be assigned in a procedural block");
                break;
            }
            continue;
        }
        if (it->second->get_storage() != signal_storage_t::variable) continue;

        // Report each variable once, with the most specific rule that applies.
        std::string finding;
        unsigned int line = writes.front()->line;
        for (size_t i = 0; i < writes.size() && finding.empty(); ++i) {
            for (size_t j = i + 1; j < writes.size(); ++j) {
                const auto &a = *writes[i];
                const auto &b = *writes[j];
                if (!overlap(a, b)) continue;
                if (a.process < 0 && b.process < 0) {
                    finding = "variable '" + name + "' has multiple continuous assignments";
                } else if ((a.process < 0) != (b.process < 0)) {
                    finding = "variable '" + name + "' is written by both a continuous assignment and a procedural block";
                } else if (a.process != b.process && (a.single_writer || b.single_writer) && !a.initial && !b.initial) {
                    finding = "variable '" + name + "' written by an always_comb, always_latch or always_ff "
                              "is also written by another process";
                }
                if (!finding.empty()) {
                    line = b.line;
                    break;
                }
            }
        }
        if (!finding.empty()) report(line, finding);
    }
    return fired;
}
