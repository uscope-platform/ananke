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
#include <memory>
#include <set>
#include <utility>
#include <vector>

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

// A name scope: the module body, or the body of one generate branch or loop. Declarations are local to the
// scope that holds them; a write finds its target by walking outwards.
struct scope {
    const scope *parent = nullptr;
    std::map<std::string, std::shared_ptr<hdl_signal_declaration>> signals;
    std::set<std::string> parameters;
};

// One step of the generate path to a statement: branch `branch` of a generate conditional, or the body of a
// generate loop (branch 0). Whether a branch exists depends on parameters this pass does not evaluate, so two
// writes are only known to be elaborated together when their paths are identical: sibling constructs
// (`if (N == 2) ... if (N == 3) ...`) usually exclude each other, and a write inside a branch next to one outside
// it only collides for some parameter values.
struct branch_step {
    const void *construct = nullptr;
    size_t branch = 0;
    bool operator==(const branch_step &o) const { return construct == o.construct && branch == o.branch; }
};

// One write to a name by the tracked logic.
struct logic_write {
    std::string name;
    bool whole = true;          // no index or select: covers the entire variable
    unsigned int line = 0;
    int process = -1;           // -1: continuous assignment, otherwise the block's position
    bool single_writer = false; // written from an always_comb / always_latch / always_ff (single-writer rule)
    bool initial = false;       // written from an initial block
    const scope *where = nullptr;
    std::vector<branch_step> path;
};

struct collector {
    std::vector<logic_write> writes;
    std::set<std::string> shadowed;   // names declared inside the current block
    int process = -1;
    bool single_writer = false;
    bool initial = false;
    unsigned int line = 0;
    const scope *where = nullptr;
    std::vector<branch_step> path;

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
            w.where = where;
            w.path = path;
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

// Builds the scope tree of a module (generate branches and loop bodies are child scopes) and collects every
// write with the scope it sits in and the generate branches that lead to it.
struct module_walk {
    std::vector<std::unique_ptr<scope>> scopes;
    collector col;
    int process = 0;

    scope *new_scope(const scope *parent) {
        scopes.push_back(std::make_unique<scope>());
        scopes.back()->parent = parent;
        return scopes.back().get();
    }

    void walk(const hdl_statement_body &body, scope *sc, std::vector<branch_step> &path) {
        // Declarations first: a write may precede the declaration it resolves to.
        for (const auto &stmt : body) {
            if (auto decl = std::dynamic_pointer_cast<hdl_signal_declaration>(stmt)) {
                sc->signals.emplace(decl->get_name(), decl);
            } else if (auto param = std::dynamic_pointer_cast<HDL_parameter>(stmt)) {
                // A struct or union initialised at module scope is filed as a parameter although it is a variable.
                auto type = param->get_type();
                const bool composite_variable = type && (type->is<HDL_struct_type>() || type->is<HDL_union_type>());
                if (!composite_variable) sc->parameters.insert(param->get_name());
            }
        }
        for (const auto &stmt : body) {
            if (!stmt) continue;
            if (auto driver = std::dynamic_pointer_cast<hdl_continuous_assignment>(stmt)) {
                col.process = -1;
                col.single_writer = false;
                col.initial = false;
                col.line = static_cast<unsigned int>(driver->get_line_n());
                col.shadowed.clear();
                col.where = sc;
                col.path = path;
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
                col.where = sc;
                col.path = path;
                col.add_body(block->get_body());
                for (const auto &written : block->get_written()) col.add_lvalue(written);
            } else if (auto cond = std::dynamic_pointer_cast<hdl_conditional_statement>(stmt)) {
                const auto &branches = cond->get_branches();
                for (size_t i = 0; i < branches.size(); ++i) {
                    path.push_back({cond.get(), i});
                    walk(branches[i].body, new_scope(sc), path);
                    path.pop_back();
                }
                path.push_back({cond.get(), branches.size()});
                walk(cond->get_else_body(), new_scope(sc), path);
                path.pop_back();
            } else if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmt)) {
                path.push_back({loop.get(), 0});
                walk(loop->get_body(), new_scope(sc), path);
                path.pop_back();
            }
        }
    }
};

// What a written name refers to: the nearest enclosing declaration or parameter of that name.
struct resolved_target {
    std::shared_ptr<hdl_signal_declaration> signal;
    const scope *parameter_scope = nullptr;
};

resolved_target resolve(const scope *from, const std::string &name) {
    for (const scope *sc = from; sc; sc = sc->parent) {
        if (auto it = sc->signals.find(name); it != sc->signals.end()) return {it->second, nullptr};
        if (sc->parameters.contains(name)) return {nullptr, sc};
    }
    return {};
}

} // namespace

bool sv_semantic_checker::check_module_logic(const std::string &file, const hdl_resource_statement &module) {
    if (opts_.disable_semantic_checks) return false;

    module_walk walk;
    scope *root = walk.new_scope(nullptr);
    std::vector<branch_step> path;
    walk.walk(module.get_statements(), root, path);

    bool fired = false;
    auto report = [&](unsigned int line, const std::string &message) {
        record_violation(file, line, violations_, message);
        fired = true;
    };

    // Group the writes by what they resolve to, in first-write order: a signal by its declaration, a
    // parameter by its scope and name.
    using target_key = std::pair<const void *, std::string>;
    std::vector<target_key> order;
    std::map<target_key, std::vector<const logic_write *>> by_target;
    std::map<target_key, resolved_target> resolved;
    for (const auto &w : walk.col.writes) {
        const auto target = resolve(w.where, w.name);
        if (!target.signal && !target.parameter_scope) continue;
        const target_key key = target.signal ? target_key{target.signal.get(), ""}
                                             : target_key{target.parameter_scope, w.name};
        if (!by_target.contains(key)) order.push_back(key);
        by_target[key].push_back(&w);
        resolved.emplace(key, target);
    }

    for (const auto &key : order) {
        const auto &writes = by_target[key];
        const auto &target = resolved[key];
        const std::string &name = writes.front()->name;

        if (!target.signal) {
            report(writes.front()->line, "cannot assign to parameter '" + name + "'");
            continue;
        }
        if (target.signal->get_storage() == signal_storage_t::net) {
            for (const auto *w : writes) {
                if (w->process < 0) continue;
                report(w->line, "net '" + name + "' cannot be assigned in a procedural block");
                break;
            }
            continue;
        }
        if (target.signal->get_storage() != signal_storage_t::variable) continue;

        // Report each variable once, with the most specific rule that applies. Only writes under the same
        // generate path are known to be elaborated together (see branch_step).
        std::string finding;
        unsigned int line = writes.front()->line;
        for (size_t i = 0; i < writes.size() && finding.empty(); ++i) {
            for (size_t j = i + 1; j < writes.size(); ++j) {
                const auto &a = *writes[i];
                const auto &b = *writes[j];
                if (a.path != b.path) continue;
                if (!(a.whole || b.whole)) continue;
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
