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

std::map<qualified_identifier, resolved_parameter> parameter_solver::process_parameters(
    const Parameters_map &map_in,
    const std::string_view &parent_module,
    const std::map<qualified_identifier, resolved_parameter> &default_parameters
) {
    auto map = map_in.clone();

    std::map<qualified_identifier, resolved_parameter> solved_parameters;
    auto dependencies_map = get_dependency_map(map_in);


    int rounds_counter = 0;
    while (!dependencies_map.empty() && rounds_counter < 100) {
        for (auto &[param_id, dependencies] : dependencies_map ) {
            if (dependencies.empty() && !solved_parameters.contains(param_id)) {
                auto to_solve = map.const_get(param_id.name);
                std::optional<resolved_parameter> value = to_solve->evaluate();

                if (value.has_value()) {
                    solved_parameters.insert({param_id, value.value()});
                }

            } else {
                for(const auto&[prefix, identifier, name]:dependencies) {
                     if(!prefix.empty()) {
                        // At parse time, package parameters are not yet known. Insert a sentinel so downstream code knows to defer evaluation.
                        solved_parameters.insert({param_id, "__RUNTIME_ONLY_PARAMETER__"s});
                    }
                }
            }
        }

        for (auto &[param_id, param_value]: solved_parameters) {
            bool propagation_complete = true;
            for (auto &dep: dependencies_map) {
                if (dep.second.contains(param_id)) {
                    auto target = map.get(dep.first.name);
                    propagation_complete &= target->propagate_constant(param_id, param_value);
                    if (propagation_complete) dep.second.erase(param_id);
                }
            }
                dependencies_map.erase(param_id);
        }
        std::map<qualified_identifier, std::set<qualified_identifier>> to_erase;
        for (auto &[param_id, dependencies] : dependencies_map ) {

            for (auto &dep: dependencies) {
                if (!dependencies_map.contains(dep) && !parent_module.starts_with("module::")) {
                    auto target = map.get(param_id.name);
                    if (!default_parameters.contains(dep)) {
                        spdlog::error("The parameter {} does not exist in module {}", dep.name, parent_module);
                        exit(-1);
                    }
                    target->propagate_constant(dep, default_parameters.at(dep));
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

    for(auto &param:resource.get_parameters()) {
        std::shared_ptr<HDL_parameter> ast_param;
        if(node_parameters.contains(param->get_name()))
            ast_param = node_parameters.get(param->get_name());
        else
            ast_param = std::make_shared<HDL_parameter>(*param);
        resolved_parameter param_val;
        if(solved_parameters.contains(param->get_identifier())) {
            param_val = solved_parameters.at(param->get_identifier());
        } else {
            param_val = resource.get_default_parameters()[param->get_identifier()];
        }
        ast_param->set_value(param_val);
        if(!node_parameters.contains(param->get_name())) node_parameters.insert(ast_param);
    }

    node->set_parameters(node_parameters);
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::override_parameters(work_order &work, const std::shared_ptr<data_store> &d_store) {
    auto node_spec = d_store->get_HDL_resource(work.node->get_type());
    auto node_defaults = node_spec.get_default_parameters();
    auto node_overrides = work.node->get_parameters();
    auto node_parameters = node_spec.get_parameters();

    //retrieve default package parameters

    std::map<qualified_identifier, resolved_parameter> solved_parameters = retrieve_package_parameters(node_parameters, d_store);

    auto overrides_solution = solve_complex_overrides(work, d_store, node_defaults);
    solved_parameters.insert(overrides_solution.begin(), overrides_solution.end());

    // Handle overridden parameters

    auto runtime_params = specialize_runtime_parameters(solved_parameters, node_parameters, work.node->get_name());
    solved_parameters.insert(runtime_params.begin(), runtime_params.end());

    std::map<qualified_identifier, resolved_parameter> results;
    for (auto &name: std::views::keys(node_defaults)) {
        if (runtime_params.contains(name)) {
            results.insert({name, runtime_params.at(name)});
        } else {
            results.insert({name, solved_parameters.at(name)});
        }
    }

    update_parameters_map(solved_parameters, work.node, d_store);
    return results;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::retrieve_package_parameters(const Parameters_map &node_parameters, const std::shared_ptr<data_store> &d_store) {
    std::map<qualified_identifier, resolved_parameter> package_parameters;
    auto deps_map = get_dependency_map(node_parameters);
    for (auto &param_deps: deps_map | std::views::values) {
        for (const auto& identifier:param_deps) {
            if (!identifier.prefix.empty()) {
                auto package = d_store->get_HDL_resource(identifier.prefix);
                auto param_value = package.get_default_parameters();
                package_parameters[identifier] = param_value[{"","", identifier.name}];
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

    auto deps_map = get_dependency_map(node_overrides);

    Parameters_map to_solve;
    for(const auto& override:node_overrides) {
        for(const auto &param: node_parameters) {
            auto deps = param->get_dependencies();
            if(deps.contains(override->get_identifier()) && !node_overrides.contains(param->get_name())) {
                to_solve.insert(param);
            }
        }
    }

    for(auto &param:node_overrides) {
        if (node_parameters.contains(param->get_name())) {
            param->set_packed_dimensions(node_parameters.get(param->get_name())->get_packed_dimensions());
            param->set_unpacked_dimensions(node_parameters.get(param->get_name())->get_unpacked_dimensions());
            param->set_type(node_parameters.get(param->get_name())->get_type());
        }
    }

    int solution_rounds = 0;
    std::unordered_map<std::string, uint32_t> parameters_progress;
    std::set<std::string> completed_parameters;
    while(completed_parameters.size() != node_overrides.size()) {
        if(solution_rounds > 100) throw std::runtime_error("Exceded maximum number of iterations when solving a parameter override");
        for(auto &param:node_overrides) {
            if(completed_parameters.contains(param->get_name())) continue;
            auto deps = param->get_dependencies();
            if(deps.empty()) {
                if(!to_solve.contains(param->get_name())) {
                    completed_parameters.insert(param->get_name());
                    to_solve.insert(param);
                }
            }
            for(auto &dep:deps) {
                resolved_parameter value;
                bool internal_dependency = false;
                if (work.parent_parameters.contains(dep)) {
                    value = work.parent_parameters.at(dep);
                }else if(!dep.prefix.empty()) {
                    auto package = d_store->get_HDL_resource(dep.prefix);
                    value = package.get_default_parameters()[{"", "", dep.name}];
                }else if (!dep.instance.empty()){
                    bool inst_param_found = false;
                    for (const auto &brother_inst:work.node->get_parent()->get_dependencies()) {
                        if (brother_inst->get_name() == dep.instance) {
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
                        value = 0;
                    }
                }else if(node_overrides.contains(dep.name)) {
                    internal_dependency = true;
                }else if(node_defaults.contains({"", "", dep.name})) {
                    value = node_defaults.at({"", "", dep.name});
                } else {
                    spdlog::warn("Parameter {}::{} is not defined in the design", dep.prefix, dep.name);
                }

                bool propagation_done = false;
                if(!internal_dependency) propagation_done = param->propagate_constant(dep, value);
                if(internal_dependency || propagation_done) {
                    parameters_progress[param->get_name()]++;
                    if(parameters_progress[param->get_name()]>= deps_map[param->get_identifier()].size()) {
                        to_solve.insert(param);
                        completed_parameters.insert(param->get_name());
                    }
                }
            }
        }
        ++solution_rounds;
    }

    solved_parameters = process_parameters(to_solve, work.node->get_name(), node_defaults);

    return solved_parameters;
}


std::map<qualified_identifier, std::set<qualified_identifier>> parameter_solver::get_dependency_map(const Parameters_map &map) {

    std::map<qualified_identifier, std::set<qualified_identifier>> dependencies_map;
    for (auto &param: map) {
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

    for(auto &param:node_parameters) {
        if(!solved_parameters.contains(param->get_identifier())) {
            runtime_to_eval.insert(param);
            for(auto &[solution_name, solution_value]:solved_parameters) {
                runtime_to_eval.get(param->get_name())->propagate_constant(solution_name, solution_value);
            }
        }
    }
    if(runtime_to_eval.empty()) return {};
    // Substitute runtime only parameters
    return process_parameters(runtime_to_eval, parent_module, {});
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
