//  Copyright 2025 Filippo Savi
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

#include "analysis/HDL_ast_builder_v2.hpp"
#include "crash_context.hpp"

#include <algorithm>

#include "analysis/loop_solver.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"
#include "data_model/hdl_file.hpp"

namespace {
    std::string format_module_chain(const std::vector<std::string> &chain) {
        std::string ret;
        for (size_t i = 0; i < chain.size(); i++) {
            if (i) ret += " -> ";
            ret += chain[i];
        }
        return ret;
    }
}
HDL_ast_builder_v2::HDL_ast_builder_v2(const std::shared_ptr<settings_store> &s, const std::shared_ptr<data_store> &d,
                                       const Depfile &d_f, int max_recursion_depth)
    : max_recursion_depth(max_recursion_depth) {
    s_store = s;
    d_store = d;
    dep_file = d_f;
    if (d_store) d_store->set_deconfliction(d_f.deconfliction);
}

std::vector<std::shared_ptr<hdl_ast_node>> HDL_ast_builder_v2::build_ast(const std::vector<std::string> &modules) {
    std::vector<std::shared_ptr<hdl_ast_node>> ret;
    ret.reserve(modules.size());
    pass_manager m(d_store);
    for(auto &item:modules){
        auto ast = build_ast(item);
        m.apply_passes(ast);
        ret.push_back(ast);
    }


    return ret;
}

std::shared_ptr<hdl_ast_node> HDL_ast_builder_v2::build_ast(const std::string &top_level_module) {

        auto top = std::make_shared<hdl_ast_node>();
        top->set_name("TL");
        top->set_type(top_level_module);
        top->set_dependency_class(module);

        std::stack< work_order> working_stack;
        working_stack.push({top, {}, "TL", {}, {}, {top_level_module}, {{}}});



        while (!working_stack.empty()) {
            auto wo = working_stack.top();
            auto working_instance = wo.node;
            working_stack.pop();

            auto type = working_instance->get_type();

            if(
                dep_file.is_module_excluded(type) ||
                d_store->is_primitive(type)
            ) continue;

            if(working_instance->get_dependency_class() == module || working_instance->get_dependency_class() == interface ) {
                std::string res_path;
                auto res_opt = d_store->get_HDL_resource(type, res_path);
                // An instance selecting a specific architecture (`entity foo(rtl)`)
                // elaborates that architecture's implementation resource.
                if (res_opt.has_value() && !working_instance->get_architecture().empty()) {
                    auto arch_res = d_store->get_HDL_resource(type, working_instance->get_architecture());
                    if (arch_res.has_value()) res_opt = arch_res;
                }

                if (!res_opt.has_value()) {
                    spdlog::error("Definition of module {} while AST building", type);
                    continue;
                }
                auto res = res_opt.value();
                crash_ctx.set(type, res_path);

                // Resolve file-level `use`/`import` statements: record the package
                // dependency and pull the imported package's constants into the
                // solving context (unqualified), so `N := WIDTH` after `use pkg.all`
                // resolves. Imported constants are never attached to the node.
                std::map<qualified_identifier, resolved_parameter> imported_params;
                std::map<std::string, hdl_function_statement> imported_functions;
                std::map<std::string, std::shared_ptr<hdl_type>> imported_types;
                if (auto file = d_store->get_file<hdl_file>(res_path)) {
                    for (auto &stmt : file.value().get_content()) {
                        auto imp = std::dynamic_pointer_cast<hdl_import_stmt>(stmt);
                        if (!imp) continue;
                        working_instance->add_package_dependency(imp->get_package());
                        auto pkg = d_store->get_HDL_resource(imp->get_package());
                        if (!pkg.has_value()) continue;
                        auto pkg_params = pkg.value()->get_parameters();
                        auto pkg_deps = parameter_solver::retrieve_package_parameters(pkg_params, d_store);
                        auto pkg_solved = parameter_solver::process_parameters(pkg_params, pkg_deps);
                        for (auto &[id, val] : pkg_solved) {
                            if (imp->is_wildcard() || id.get_name() == imp->get_item())
                                imported_params[qualified_identifier(id.get_name())] = val;
                        }
                        for (auto &[name, fn] : pkg.value()->get_functions())
                            if (imp->is_wildcard() || imp->get_item() == name)
                                imported_functions[name] = fn;
                        for (auto &[name, t] : pkg.value()->get_typedefs())
                            if (imp->is_wildcard() || imp->get_item() == name)
                                imported_types[name] = t;
                    }
                }

                spdlog::trace("Processing dependency {} in module {}",working_instance->get_name(), type);
                auto current_param_values = parameter_solver::override_parameters(wo, d_store, imported_params, imported_functions, imported_types);
                if (!wo.param_chain.empty()) wo.param_chain.back() = current_param_values;

                std::vector<work_order> child_wo;
                wo.interfaces_map.clear();
                for (auto &[port_name, port_net] :wo.node->get_ports()) {
                    auto port_spec = res->get_port_specs()[port_name];
                    if (port_spec.direction == interface_port) {
                        wo.interfaces_map[port_name] = port_spec.if_info.type;
                    }
                }
                for (auto &stmt : res->get_statements()) {
                    auto processed = process_statement(stmt, wo);
                    if (!processed) {
                        if (processed.error() == recursive_module) {
                            spdlog::error("Skipping cyclic module hierarchy rooted at {}::{}", working_instance->get_type(), working_instance->get_name());
                        } else {
                            spdlog::warn("An error happened while processing instance {}::{}", working_instance->get_type(), working_instance->get_name());
                        }
                        continue;
                    }
                    child_wo.insert(child_wo.end(), processed.value().begin(), processed.value().end());
                }
                apply_parameter_overrides(res->get_statements(), wo.pending_overrides, child_wo);
                for (const auto &c:child_wo| std::views::reverse) {
                    working_stack.push(c);
                }
            }
        }
    return top;
}

bool HDL_ast_builder_v2::evaluate_condition(const std::shared_ptr<Expression_base> &cond,
    const std::map<qualified_identifier, resolved_parameter> &parameters) {
    if (!cond) return false;
    auto result = cond->evaluate(parameters);
    return result.has_value() && result.value().is_integer() && result.value().get_integer() != 0;
}

std::expected<void, solver_errors> HDL_ast_builder_v2::process_quantifier(const std::shared_ptr<HDL_parameter> &quantifier, const std::map<qualified_identifier, resolved_parameter> &parameters) {

    if (quantifier != nullptr) {
        auto value = quantifier->evaluate(parameters);
        if (!value.has_value()) return std::unexpected{value.error()};
        quantifier->set_value(value.value());
    }
    return {};
}

std::expected<std::vector<work_order>, solver_errors> HDL_ast_builder_v2::process_statement(
    const std::shared_ptr<hdl_statement_base> &stmt,
    work_order &wo
) {
    if (auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(stmt))
        return process_instance(inst, wo);
    if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmt))
        return process_loop(*loop, wo);
    if (auto cond = std::dynamic_pointer_cast<hdl_conditional_statement>(stmt))
        return process_conditional(*cond, wo);
    return {};
}

std::expected<std::vector<work_order>, solver_errors> HDL_ast_builder_v2::process_instance(
    const std::shared_ptr<hdl_instance_statement> &inst,
    work_order &wo,
    bool active
) {
    std::vector<work_order> orders;
    auto dc = inst->get_dependency_class();
    if (dc != module && dc != interface) return orders;
    auto child = std::make_shared<hdl_ast_node>(*inst);
    child->set_parent(wo.node);
    child->set_active(active);
    auto type = inst->get_type();
    auto &module_chain = wo.module_chain;
    auto &param_chain = wo.param_chain;
    const auto &params = param_chain.back();
    auto recursion_depth = std::count(module_chain.begin(), module_chain.end(), type);
    if (recursion_depth >= max_recursion_depth) {
        spdlog::error("Recursive module hierarchy detected at depth {} (limit {}): {} -> {}",
                      recursion_depth + 1, max_recursion_depth, format_module_chain(module_chain), type);
        return std::unexpected{recursive_module};
    }
    if (dep_file.is_module_excluded(type) || d_store->is_primitive(type)) child->set_dependency_class(primitive);
    if (auto err = process_quantifier(child->get_array_quantifier(), params); !err) return std::unexpected{err.error()};
    auto child_chain = module_chain;
    child_chain.push_back(type);
    auto child_param_chain = param_chain;
    child_param_chain.push_back({});
    wo.node->add_child(child);
    bool recurses = std::count(module_chain.begin(), module_chain.end(), type) > 0;
    // A recursive instantiation whose resolved parameters repeat an ancestor of
    // the same type makes no progress and would never terminate (e.g. a module
    // recursing on an unresolved parameter that stays 0). Keep it as a terminal.
    // Parameterless recursion is left to the depth guard: an empty parameter set
    // cannot show whether the recursion is progressing.
    if (recurses && !params.empty()) {
        for (size_t i = 0; i + 1 < module_chain.size(); ++i) {
            if (module_chain[i] == type && i < param_chain.size() && param_chain[i] == params)
                return orders;
        }
    }
    // Instances from non-selected generate branches are normally elaborated too
    // (so their internals are available for analysis). The one exception is a
    // recursive instantiation through such an inactive branch: that would never
    // terminate (the generate chain only terminates on the selected leaf), so
    // it is kept as a terminal node instead.
    if (!active && recurses) return orders;
    orders.push_back({child, params, wo.path + "." + wo.node->get_name(), wo.interfaces_map, {}, child_chain, child_param_chain});
    return orders;
}

std::expected<std::vector<work_order>, solver_errors> HDL_ast_builder_v2::process_loop(
    const hdl_loop_statement &loop,
    work_order &wo
) {
    if (!loop.get_init() || loop.get_init()->get_name().empty())
        return {};

    std::vector<work_order> orders;
    auto indices = loop_solver::solve_loop(loop, wo.param_chain.back());
    auto loop_var_name = loop.get_init()->get_name();
    for (auto &body_stmt : loop.get_body()) {
        for (auto &idx : indices) {
            // Per-iteration elaboration context: same frame as wo, with the
            // loop variable bound to the current index.
            work_order iter_wo = wo;
            iter_wo.param_chain.back()[qualified_identifier(loop_var_name)] = resolved_parameter(idx);

            if (auto body_inst = std::dynamic_pointer_cast<hdl_instance_statement>(body_stmt)) {
                auto child = std::make_shared<hdl_ast_node>(*body_inst);
                child->set_parent(wo.node);
                auto inst_type = body_inst->get_type();
                auto dc = body_inst->get_dependency_class();
                if ((dc == module || dc == interface) &&
                    std::count(wo.module_chain.begin(), wo.module_chain.end(), inst_type) >= max_recursion_depth) {
                    spdlog::error("Recursive module hierarchy detected at depth {} (limit {}): {} -> {}",
                                  std::count(wo.module_chain.begin(), wo.module_chain.end(), inst_type) + 1,
                                  max_recursion_depth, format_module_chain(wo.module_chain), inst_type);
                    return std::unexpected{recursive_module};
                }
                if (dep_file.is_module_excluded(inst_type) || d_store->is_primitive(inst_type)) child->set_dependency_class(primitive);
                if (auto err = process_quantifier(child->get_array_quantifier(), iter_wo.param_chain.back()); !err) return std::unexpected{err.error()};

                std::unordered_map<std::string, std::vector<HDL_net>> new_ports;
                for (auto &[port_name, nets] : child->get_ports()) {
                    std::vector<HDL_net> port_content;
                    for (auto &n : nets) {
                        if (n.is_array()) {
                            auto new_net = n;
                            Expression_v2 n_idx;
                            n_idx.set_lhs(std::make_shared<Numeric_token>(std::variant<hdl_integer, double>(idx), 0));
                            new_net.set_index({n_idx});
                            port_content.emplace_back(new_net);
                        } else {
                            port_content.push_back(n);
                        }
                    }
                    new_ports[port_name] = port_content;
                }
                child->set_ports(new_ports);

                auto child_chain = iter_wo.module_chain;
                child_chain.push_back(inst_type);
                auto child_param_chain = iter_wo.param_chain;
                child_param_chain.push_back({});
                wo.node->add_child(child);
                orders.push_back({child, iter_wo.param_chain.back(), iter_wo.path + "." + wo.node->get_name(),
                                  iter_wo.interfaces_map, {}, child_chain, child_param_chain});
            } else {
                auto w = process_statement(body_stmt, iter_wo);
                if (!w) return std::unexpected{w.error()};
                orders.insert(orders.end(), w.value().begin(), w.value().end());
            }
        }
    }
    return orders;
}

std::expected<std::vector<work_order>, solver_errors> HDL_ast_builder_v2::process_conditional(
    const hdl_conditional_statement &cond,
    work_order &wo
) {
    std::vector<work_order> orders;
    bool any_matched = false;
    for (auto &branch : cond.get_branches()) {
        bool selected = evaluate_condition(branch.condition, wo.param_chain.back());
        any_matched = any_matched || selected;
        for (auto &body_stmt : branch.body) {
            if (auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(body_stmt)) {
                auto w = process_instance(inst, wo, selected);
                if (!w)return std::unexpected{w.error()};
                orders.insert(orders.end(), w.value().begin(), w.value().end());
            } else {
                auto w = process_statement(body_stmt, wo);
                if (!w) return std::unexpected{w.error()};
                orders.insert(orders.end(), w.value().begin(), w.value().end());
            }
        }
    }
    for (auto &body_stmt : cond.get_else_body()) {
        if (auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(body_stmt)) {
            auto w = process_instance(inst, wo, !any_matched);
            if (!w) return std::unexpected{w.error()};
            orders.insert(orders.end(), w.value().begin(), w.value().end());
        } else {
            auto w = process_statement(body_stmt, wo);
            if (!w) return std::unexpected{w.error()};
            orders.insert(orders.end(), w.value().begin(), w.value().end());
        }
    }
    return orders;
}

void HDL_ast_builder_v2::apply_parameter_overrides(
    const std::vector<std::shared_ptr<hdl_statement_base>> &statements,
    const std::vector<pending_parameter_override> &inherited,
    std::vector<work_order> &child_wo
) {
    // Resolve each override one level at a time against the child work orders.
    // If the remaining path is exhausted, the override is injected onto the
    // target instance's node; otherwise it is forwarded to the matching child
    // work order for the next level.
    auto find_child = [&](const std::string &name) -> work_order* {
        for (auto &wo : child_wo) {
            if (wo.node->get_name() == name) return &wo;
        }
        return nullptr;
    };

    auto handle = [&](const std::vector<std::string> &path, const std::string &pname,
                      const std::shared_ptr<Expression_base> &value) {
        if (path.empty()) return;
        auto child = find_child(path[0]);
        if (!child) {
            spdlog::warn("defparam target path not found, ignoring");
            return;
        }
        if (path.size() == 1) {
            auto p = std::make_shared<HDL_parameter>(pname);
            p->set_raw_value(value);
            child->node->add_parameter(p);
        } else {
            pending_parameter_override po;
            po.path.assign(path.begin() + 1, path.end());
            po.parameter_name = pname;
            po.value = value;
            child->pending_overrides.push_back(po);
        }
    };

    for (const auto &stmt : statements) {
        auto dp = std::dynamic_pointer_cast<hdl_parameter_override_statement>(stmt);
        if (!dp) continue;
        handle(dp->get_instance_path(), dp->get_parameter_name(), dp->get_value());
    }

    for (const auto &po : inherited) {
        handle(po.path, po.parameter_name, po.value);
    }
}
