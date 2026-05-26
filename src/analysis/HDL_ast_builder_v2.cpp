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

#include "analysis/loop_solver.hpp"

HDL_ast_builder_v2::HDL_ast_builder_v2(const std::shared_ptr<settings_store> &s, const std::shared_ptr<data_store> &d,
                                       const Depfile &d_f){
    s_store = s;
    d_store = d;
    dep_file = d_f;
}

std::vector<std::shared_ptr<HDL_instance_AST>> HDL_ast_builder_v2::build_ast(const std::vector<std::string> &modules) {
    std::vector<std::shared_ptr<HDL_instance_AST>> ret;
    ret.reserve(modules.size());
    pass_manager m(d_store);
    for(auto &item:modules){
        auto ast = build_ast(item);
        m.apply_passes(ast);
        ret.push_back(ast);
    }


    return ret;
}

std::shared_ptr<HDL_instance_AST> HDL_ast_builder_v2::build_ast(const std::string &top_level_module) {

        auto top = std::make_shared<HDL_instance_AST>();
        top->set_name("TL");
        top->set_type(top_level_module);
        top->set_dependency_class(module);

        std::stack< work_order> working_stack;
        working_stack.push({top, {}, "TL"});



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

                if (!d_store->contains_hdl_entity(type)) {
                    std::cerr << "ERROR:\n HDL entity: " + type + " Not found\n";
                }
                auto res = d_store->get_HDL_resource(type);

                spdlog::trace("Processing dependency {} in module {}",working_instance->get_name(), type);

                auto current_param_values = parameter_solver::override_parameters(wo, d_store);

                for (auto &[name, value]: process_runtime_parameters(current_param_values, res)) {
                    current_param_values[name] = value;
                }

                std::vector<work_order> child_wo;
                for (auto &dep: res.get_dependencies()) {
                    if(dep.get_dependency_class() == interface || dep.get_dependency_class() == module) {
                        auto child = std::make_shared<HDL_instance_AST>(dep);
                        child->set_parent(working_instance);

                        // The loop structure is attached to the looped instances, that need to be repeated,
                        // But the parent parameters only need to be propagated in its expressions
                        auto loop_idx = loop_solver::solve_loop(child, current_param_values);
                        process_quantifier(child->get_array_quantifier(), current_param_values);

                        if (!loop_idx.empty()) {
                            for (auto &idx:loop_idx) {
                                auto new_child = std::make_shared<HDL_instance_AST>(*child);
                                auto specialized_child = specialize_instance(*new_child, idx, child->get_inner_loop().get_init().get_name());
                                working_instance->add_child(specialized_child);
                                auto parent_params = current_param_values;
                                parent_params[{"", "", child->get_inner_loop().get_init().get_name()}] = resolved_parameter(idx);
                                child_wo.push_back({
                                    specialized_child,
                                    parent_params,
                                    wo.path + "." + working_instance->get_name()
                                });
                            }
                        } else {
                            working_instance->add_child(child);
                            child_wo.push_back({
                                child,
                                current_param_values,
                                wo.path + "." + working_instance->get_name()
                            });
                        }
                    } else if(dep.get_dependency_class() == package) {
                        auto path = d_store->get_HDL_resource(dep.get_type()).get_path();
                        working_instance->add_package_dependency(path);
                    } else if(dep.get_dependency_class() == memory_init) {
                        auto path = d_store->get_data_file(dep.get_type()).get_path();
                        working_instance->add_data_dependency(path);
                    }
                }
                for (const auto &c:child_wo| std::views::reverse) {
                    working_stack.push(c);
                }
            }
        }
    return top;
}

std::shared_ptr<HDL_instance_AST> HDL_ast_builder_v2::specialize_instance(const HDL_instance_AST &i, hdl_integer idx, std::string idx_name) {
    HDL_instance_AST specialized_d = i;
    auto specialized_parameters = specialized_d.get_parameters().clone();
    specialized_d.set_parameters(specialized_parameters);
    specialized_d.set_repeated(true);
    specialized_d.set_repetition_idx(idx);

    std::unordered_map<std::string, std::vector<HDL_net>> new_ports;

    std::string accessor = "[" + idx_name + "]";

    for(auto &[port_name, nets]:specialized_d.get_ports()){
        std::vector<HDL_net> port_content;
        for(auto &n:nets){
            if(n.is_array()) {
                auto new_net = n;
                Expression n_idx;
                n_idx.emplace_back(idx);
                new_net.set_index(n_idx);
                port_content.emplace_back(new_net);
            } else {
                port_content.push_back(n);
            }
        }
        new_ports[port_name] = port_content;
    }
    specialized_d.set_ports(new_ports);
    return std::make_shared<HDL_instance_AST>(specialized_d);
}

void HDL_ast_builder_v2::process_quantifier(const std::shared_ptr<HDL_parameter> &quantifier, const std::map<qualified_identifier, resolved_parameter> &parameters) {

    if (quantifier != nullptr) {
        auto value = quantifier->evaluate(parameters);
        if (!value.has_value()) throw std::runtime_error("unknown identifiers remain in an array quantifier");
        quantifier->set_value(value.value());
    }
}

std::map<qualified_identifier, resolved_parameter> HDL_ast_builder_v2::process_runtime_parameters(const std::map<qualified_identifier, resolved_parameter> &parameters, const HDL_Resource &res) {
    std::map<qualified_identifier, resolved_parameter> runtime_parameters;
    for ( auto &[name, value]: parameters) {
        if (value.is_string()) {
            if (value.get_string() == "__RUNTIME_ONLY_PARAMETER__") {
                auto raw_param = res.get_parameters().get(name.name);
                std::map<qualified_identifier, resolved_parameter> pkg_ctx;
                for (const auto dep: raw_param->get_dependencies()) {
                    if (!dep.prefix.empty()) {
                        auto package_params= d_store->get_HDL_resource(dep.prefix).get_default_parameters();
                        pkg_ctx[dep] = package_params[{"", "", dep.name}];
                    }
                }
                auto val = raw_param->evaluate(pkg_ctx);
                if (val.has_value()) runtime_parameters.insert({name, val.value()});
            }
        }
    }
    return runtime_parameters;
}
