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

#include "analysis/parameter_solver.hpp"
#include "data_model/data_store.hpp"

using namespace std::string_literals;

void parameter_solver::resolve_interface_chain(
    work_order &work,
    const std::shared_ptr<data_store> &d_store,
    std::shared_ptr<HDL_instance_AST> &examined_node,
    std::string &instance_name
) {
    auto is_interface_at = [&](const std::string &name, const std::shared_ptr<HDL_instance_AST> &node) -> bool {
        if (!node) return false;
        auto node_type = node->get_type();
        if (!d_store->contains_hdl_entity(node_type)) return false;
        auto res = d_store->get_HDL_resource(node_type);
        auto ports = res.get_port_specs();
        return ports.contains(name) && ports.at(name).direction == interface_port;
    };

    examined_node = work.node;
    auto current_instance = instance_name;

    auto parent = examined_node->get_parent();
    if (!parent) return;
    auto ports = parent->get_ports();
    if (!ports.contains(current_instance)) return;
    instance_name = ports.at(current_instance)[0].get_name();
    examined_node = parent;

    while (is_interface_at(instance_name, examined_node->get_parent())) {
        auto next_parent = examined_node->get_parent();
        ports = next_parent->get_ports();
        if (!ports.contains(instance_name)) break;
        instance_name = ports.at(instance_name)[0].get_name();
        examined_node = next_parent;
    }

    auto container = examined_node->get_parent();
    if (container) examined_node = container;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::process_parameters(
    const Parameters_map &map_in,
    const std::string_view &parent_module,
    const std::map<qualified_identifier, resolved_parameter> &default_parameters,
    const std::map<qualified_identifier, resolved_parameter> &context
) {
    std::map<qualified_identifier, resolved_parameter> ctx = context;

    std::map<qualified_identifier, resolved_parameter> solved_parameters;
    auto dependencies_map = get_dependency_map(map_in);


    int rounds_counter = 0;
    while (!dependencies_map.empty() && rounds_counter < 100) {
        // Phase 1: evaluate params with no remaining dependencies
        for (auto &[param_id, dependencies] : dependencies_map ) {
            if (dependencies.empty() && !solved_parameters.contains(param_id)) {
                auto to_solve = map_in.const_get(param_id.name);
                std::optional<resolved_parameter> value = to_solve->evaluate(ctx);

                if (value.has_value()) {
                    solved_parameters.insert({param_id, value.value()});
                    ctx[param_id] = value.value();
                }

            } else {
                for(const auto&[prefix, identifier, name]:dependencies) {
                     if(!prefix.empty() && parent_module.starts_with("module::")) {
                        solved_parameters.insert({param_id, "__RUNTIME_ONLY_PARAMETER__"s});
                    }
                }
            }
        }

        // Phase 2: erase solved param IDs from remaining dependency sets
        for (auto &[param_id, param_value]: solved_parameters) {
            for (auto &dep: dependencies_map) {
                dep.second.erase(param_id);
            }
            dependencies_map.erase(param_id);
        }

        // Phase 3: resolve external deps (parent/package params not in this map)
        std::map<qualified_identifier, std::set<qualified_identifier>> to_erase;
        for (auto &[param_id, dependencies] : dependencies_map ) {
            for (auto &dep: dependencies) {
                if (dep == param_id) {
                    to_erase[param_id].insert(dep);
                } else if (!dependencies_map.contains(dep) && !parent_module.starts_with("module::")) {
                    if (!ctx.contains(dep)) {
                        spdlog::error("The parameter {} does not exist in module {}", dep.name, parent_module);
                        exit(-1);
                    }
                    to_erase[param_id].insert(dep);
                }
            }
        }
        for (auto&e: to_erase) {
            for (auto & dep: e.second) {
                dependencies_map[e.first].erase(dep);
            }
        }
        rounds_counter++;
        spdlog::trace("Process parameters round {}: {} params remaining in deps map", rounds_counter, dependencies_map.size());
    }

    if (rounds_counter >=100) {
        std::string parameters;
        int i = 0;
        for(const auto &name: dependencies_map | std::views::keys) {
            parameters += name.name;
            if(dependencies_map.size()>1 && i<dependencies_map.size()-1) {
                parameters += ", ";
                i++;
            }
        }
        spdlog::warn("Parameters {} could not be solved in 100 passes while processing instance {}",
            parameters,  parent_module);
    }

    return solved_parameters;
}


void parameter_solver::update_parameters_map(
    const std::map<qualified_identifier, resolved_parameter> &solved_parameters,
    const std::shared_ptr<HDL_instance_AST>& node,
    const std::shared_ptr<data_store> &d_store
) {
    auto node_parameters = node->get_parameters();
    auto resource = d_store->get_HDL_resource(node->get_type());

    for(auto &[p_name, param]:resource.get_parameters()) {
        std::shared_ptr<HDL_parameter> ast_param;
        if(node_parameters.contains(p_name))
            ast_param = std::make_shared<HDL_parameter>(*node_parameters.get(p_name));
        else
            ast_param = std::make_shared<HDL_parameter>(*param);
        resolved_parameter param_val;
        param_val = solved_parameters.at(param->get_identifier());
        ast_param->set_value(param_val);
        node_parameters.insert(ast_param);
    }

    node->set_parameters(node_parameters);
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::override_parameters(work_order &work, const std::shared_ptr<data_store> &d_store) {
    auto node_spec = d_store->get_HDL_resource(work.node->get_type());
    auto node_overrides = work.node->get_parameters();
    auto node_parameters = node_spec.get_parameters();

    //retrieve default package parameters
    Parameters_map combined_params = node_parameters;
    for (const auto &[name, param] : node_overrides) {
        combined_params.insert(param);
    }
    std::map<qualified_identifier, resolved_parameter> solved_parameters = retrieve_package_parameters(combined_params, d_store);

    auto overrides_solution = solve_complex_overrides(work, d_store, solved_parameters);
    solved_parameters.insert(overrides_solution.begin(), overrides_solution.end());

    // Handle module-body parameters with instance dependencies (e.g. parameter x = inst.param)
    // Must run BEFORE specialize_runtime_parameters since process_parameters can't resolve
    // instance-qualified deps and will exit(-1) on unresolved ones
    for (auto &[p_name, param] : node_parameters) {
        auto p_id = param->get_identifier();
        if (solved_parameters.contains(p_id)) continue;

        auto deps = param->get_dependencies();
        bool has_instance_dep = false;
        for (auto &dep : deps) {
            if (!dep.instance.empty()) {
                has_instance_dep = true;
                break;
            }
        }
        if (!has_instance_dep) continue;

        std::map<qualified_identifier, resolved_parameter> ctx;
        ctx.insert(solved_parameters.begin(), solved_parameters.end());
        ctx.insert(work.parent_parameters.begin(), work.parent_parameters.end());

        for (auto &dep : deps) {
            if (!dep.instance.empty() && !ctx.contains(dep)) {
                auto instance_name = dep.instance;
                std::shared_ptr<HDL_instance_AST> examined_node = work.node;

                // Check if dep.instance is a port of the current node first
                auto current_ports = work.node->get_ports();
                if (current_ports.contains(dep.instance)) {
                    instance_name = current_ports.at(dep.instance)[0].get_name();
                    examined_node = work.node->get_parent();
                } else if (work.interfaces_map.contains(dep.instance)) {
                    resolve_interface_chain(work, d_store, examined_node, instance_name);
                } else if (examined_node) {
                    examined_node = examined_node->get_parent();
                }

                if (examined_node) {
                    for (const auto &brother_inst : examined_node->get_dependencies()) {
                        if (brother_inst->get_name() == instance_name) {
                            auto inst_param = brother_inst->get_parameters().get(dep.name);
                            auto val = inst_param->get_numeric_value();
                            if (val.has_value()) {
                                ctx[dep] = val.value();
                            } else {
                                spdlog::warn("The instance parameter {}::{} has no value, using 0 as a default", dep.instance, dep.name);
                                ctx[dep] = resolved_parameter(0);
                            }
                            break;
                        }
                    }
                }

                if (!ctx.contains(dep)) {
                    auto path = get_full_path(work.node);
                    spdlog::warn("The instance parameter {}.{}::{} was not found, using 0 as a default", path, dep.instance, dep.name);
                    ctx[dep] = resolved_parameter();
                    ctx[dep].set_undefined();
                }
            }
        }

        auto eval_result = param->evaluate(ctx);
        if (eval_result.has_value()) {
            solved_parameters[p_id] = eval_result.value();
        }
    }

    auto runtime_params = specialize_runtime_parameters(solved_parameters, node_parameters, work.node->get_name());
    solved_parameters.insert(runtime_params.begin(), runtime_params.end());

    std::map<qualified_identifier, resolved_parameter> results = solved_parameters;

    update_parameters_map(solved_parameters, work.node, d_store);
    return results;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::retrieve_package_parameters(const Parameters_map &node_parameters, const std::shared_ptr<data_store> &d_store) {
    std::map<qualified_identifier, resolved_parameter> package_parameters;
    auto deps_map = get_dependency_map(node_parameters);
    for (auto &param_deps: deps_map | std::views::values) {
        for (const auto& identifier:param_deps) {
            if (!identifier.prefix.empty() && !package_parameters.contains(identifier)) {
                auto package = d_store->get_HDL_resource(identifier.prefix);
                auto pkg_solved = process_parameters(package.get_parameters(), "module::" + package.getName(), {}, {});
                for (auto &[pkg_id, pkg_val]: pkg_solved) {
                    qualified_identifier qid{identifier.prefix, "", pkg_id.name};
                    package_parameters[qid] = pkg_val;
                }
            }
        }
    }
    return package_parameters;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::solve_complex_overrides(
    work_order &work,
    const std::shared_ptr<data_store> &d_store,
    const std::map<qualified_identifier, resolved_parameter> &node_defaults
) {

    std::map<qualified_identifier, resolved_parameter> solved_parameters;

    auto node_spec = d_store->get_HDL_resource(work.node->get_type());
    auto node_parameters = node_spec.get_parameters();
    auto node_overrides = work.node->get_parameters();

    Parameters_map to_solve;
    for(const auto &[p_name, param]: node_parameters) {
        if (node_overrides.contains(p_name)) {
            to_solve.insert(node_overrides.get(p_name));
        } else {
            bool has_instance_dep = false;
            for (auto &dep : param->get_dependencies()) {
                if (!dep.instance.empty()) {
                    has_instance_dep = true;
                    break;
                }
            }
            if (!has_instance_dep) {
                to_solve.insert(param);
            }
        }
    }

    for(auto &[override_name, param]:node_overrides) {
        if (node_parameters.contains(override_name)) {
            param->set_packed_dimensions(node_parameters.get(override_name)->get_packed_dimensions());
            param->set_unpacked_dimensions(node_parameters.get(override_name)->get_unpacked_dimensions());
            param->set_type(node_parameters.get(override_name)->get_type());
        }
    }

    std::map<qualified_identifier, resolved_parameter> ctx;
    ctx.insert(work.parent_parameters.begin(), work.parent_parameters.end());
    ctx.insert(node_defaults.begin(), node_defaults.end());

    int solution_rounds = 0;
    std::set<std::string> completed_parameters;
    while(completed_parameters.size() != node_overrides.size()) {
        if(solution_rounds > 100) throw std::runtime_error("Exceded maximum number of iterations when solving a parameter override");
        for(auto &[override_name, param]:node_overrides) {
            if(completed_parameters.contains(override_name)) continue;
            auto deps = param->get_dependencies();
            if(deps.empty()) {
                if(!to_solve.contains(override_name)) {
                    to_solve.insert(param);
                }
                completed_parameters.insert(override_name);
                continue;
            }
            for(auto &dep:deps) {
                resolved_parameter value;
                bool internal_dependency = false;
                if (ctx.contains(dep)) continue;
                if (work.parent_parameters.contains(dep)) {
                    value = work.parent_parameters.at(dep);
                }else if (!dep.instance.empty()){
                    bool inst_param_found = false;
                    auto dep_name = dep.instance;
                    auto instance_name = dep.instance;
                    std::shared_ptr<HDL_instance_AST> examined_node = work.node;

                    if (work.interfaces_map.contains(dep.instance)) {
                        resolve_interface_chain(work, d_store, examined_node, instance_name);
                    } else {
                        examined_node = examined_node->get_parent();
                    }

                    for (const auto &brother_inst:examined_node->get_dependencies()) {
                        if (brother_inst->get_name() == instance_name) {
                            auto inst_param = brother_inst->get_parameters().get(dep.name)->get_numeric_value();
                            if (!inst_param.has_value()) {
                                spdlog::warn("The instance parameter {}::{} has no value, using 0 as a default", dep.instance, dep.name);
                            }
                            value = inst_param.value_or(0);

                            inst_param_found = true;
                            break;
                        }
                    }
                    if (!inst_param_found) {
                        auto path = get_full_path(work.node);
                        spdlog::warn("The instance parameter {}.{}::{} was not found, using 0 as a default", path, dep.instance, dep.name);
                        value.set_undefined();
                    }
                }else if(node_overrides.contains(dep.name)) {
                    internal_dependency = true;
                }else if(dep.prefix.empty() && dep.instance.empty() && to_solve.contains(dep.name)) {
                    continue;
                } else {
                    spdlog::warn("Parameter {}::{} is not defined in the design", dep.prefix, dep.name);
                    value.set_undefined();
                }

                if(!internal_dependency) {
                    ctx[dep] = value;
                }
            }
            // Check if all deps are satisfied
            bool all_solved = true;
            for(auto &dep:deps) {
                if(dep.prefix.empty() && dep.instance.empty() && node_overrides.contains(dep.name) && dep.name != param->get_name()) {
                    if(!completed_parameters.contains(dep.name)) {
                        all_solved = false;
                        break;
                    }
                } else if(dep.prefix.empty() && dep.instance.empty() && to_solve.contains(dep.name)) {
                    continue;
                } else if(!ctx.contains(dep)) {
                    all_solved = false;
                    break;
                }
            }
            if(all_solved) {
                to_solve.insert(param);
                completed_parameters.insert(param->get_name());
            }
        }
        ++solution_rounds;
    }

    solved_parameters = process_parameters(to_solve, work.node->get_name(), node_defaults, ctx);

    return solved_parameters;
}


std::map<qualified_identifier, std::set<qualified_identifier>> parameter_solver::get_dependency_map(const Parameters_map &map) {

    std::map<qualified_identifier, std::set<qualified_identifier>> dependencies_map;
    for (auto &[p_name, param]: map) {
        auto param_id = param->get_identifier();
        dependencies_map[param_id] = {};
        auto deps = param->get_dependencies();
        dependencies_map[param_id].insert(deps.begin(), deps.end());
    }
    return dependencies_map;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::specialize_runtime_parameters(
    const std::map<qualified_identifier, resolved_parameter> &solved_parameters,
    Parameters_map &node_parameters, const std::string_view &parent_module
    ) {

    Parameters_map runtime_to_eval;

    for(auto &[p_name, param]:node_parameters) {
        if(!solved_parameters.contains(param->get_identifier())) {
            runtime_to_eval.insert(param);
        }
    }
    if(runtime_to_eval.empty()) return {};
    return process_parameters(runtime_to_eval, parent_module, {}, solved_parameters);
}

std::string parameter_solver::get_full_path(const std::shared_ptr<HDL_instance_AST> &node) {
    std::string res = "";

    std::shared_ptr<HDL_instance_AST> current_node = node;

    while (current_node != nullptr) {
        res = current_node->get_name() + "." + res;
        current_node = current_node->get_parent();
    }
    res.erase(res.size()-1, 1);

    return res;
}
