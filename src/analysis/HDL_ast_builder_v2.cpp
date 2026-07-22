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

#include "analysis/loop_solver.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"

HDL_ast_builder_v2::HDL_ast_builder_v2(const std::shared_ptr<settings_store> &s, const std::shared_ptr<data_store> &d,
                                       const Depfile &d_f){
    s_store = s;
    d_store = d;
    dep_file = d_f;
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

                auto res_opt = d_store->get_HDL_resource(type);

                if (!res_opt.has_value()) {
                    spdlog::error("Definition of module {} while AST building", type);
                    continue;
                }
                auto res = res_opt.value();
                crash_ctx.set(type, res.get_path());

                spdlog::trace("Processing dependency {} in module {}",working_instance->get_name(), type);
                auto current_param_values = parameter_solver::override_parameters(wo, d_store);

                for (auto &[name, value]: process_runtime_parameters(current_param_values, res)) {
                    current_param_values[name] = value;
                }

                std::vector<work_order> child_wo;

                std::unordered_map<std::string, std::string> interfaces_map;
                for (auto &[port_name, port_net] :wo.node->get_ports()) {
                    auto port_spec = res.get_port_specs()[port_name];
                    if (port_spec.direction == interface_port) {
                        interfaces_map[port_name] = port_spec.if_info.type;
                    }
                }
                for (auto &stmt : res.get_statements()) {
                    if (auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(stmt)) {
                        auto dc = inst->get_dependency_class();
                        if (dc == module || dc == interface) {
                            auto child = std::make_shared<hdl_ast_node>(*inst);
                            child->set_parent(working_instance);
                            process_quantifier(child->get_array_quantifier(), current_param_values);
                            working_instance->add_child(child);
                            child_wo.push_back({child, current_param_values, wo.path + "." + working_instance->get_name(), interfaces_map});
                        } else if (dc == package) {
                            auto pkg = d_store->get_HDL_resource(inst->get_type());
                            if (!pkg.has_value()) {
                                spdlog::error("Definition of package {} not found while AST building", inst->get_type());
                                continue;
                            }
                            auto path = pkg.value().get_path();
                            working_instance->add_package_dependency(path);
                        } else if (dc == memory_init) {
                            auto df = d_store->get_data_file(inst->get_type());
                            if (!df.has_value()) {
                                spdlog::error("Data file {} mot found while AST building", inst->get_type());
                                continue;
                            }
                            working_instance->add_data_dependency(df.value().get_path());
                        }
                    } else if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmt)) {
                        auto indices = loop_solver::solve_loop(*loop, current_param_values);
                        auto loop_var_name = loop->get_init()->get_name();
                        for (auto &body_stmt : loop->get_body()) {
                            for (auto &idx : indices) {
                                auto body_inst = std::dynamic_pointer_cast<hdl_instance_statement>(body_stmt);
                                if (!body_inst) continue;
                                auto child = std::make_shared<hdl_ast_node>(*body_inst);
                                child->set_parent(working_instance);
                                process_quantifier(child->get_array_quantifier(), current_param_values);

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

                                working_instance->add_child(child);
                                auto parent_params = current_param_values;
                                parent_params[qualified_identifier(loop_var_name)] = resolved_parameter(idx);
                                child_wo.push_back({child, parent_params, wo.path + "." + working_instance->get_name(), interfaces_map});
                            }
                        }
                    }
                }
                for (const auto &c:child_wo| std::views::reverse) {
                    working_stack.push(c);
                }
            }
        }
    return top;
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
                auto raw_param = res.get_parameters().get(name.get_name());
                auto val = raw_param->evaluate({});
                if (val.has_value()) runtime_parameters.insert({name, val.value()});
            }
        }
    }
    return runtime_parameters;
}
