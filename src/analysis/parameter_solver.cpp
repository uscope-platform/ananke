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
#include "crash_context.hpp"

#include <set>



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
    const std::map<qualified_identifier, resolved_parameter> &context
) {
    std::map<qualified_identifier, resolved_parameter> ctx = context;

    std::map<qualified_identifier, resolved_parameter> solved_parameters;

    topological_sorter s;
    s.analyze(map_in, ctx);

    while (auto next = s.get_next()) {
        auto param = map_in.const_get(next.value().get_name());
        crash_ctx.parameter = next.value().get_name();
        auto res = param->evaluate(ctx);
        if (res) {
            ctx[next.value()] = res.value();
            solved_parameters[next.value()] = res.value();

            auto struct_fields = extract_struct_fields(param, res.value(), next.value(), ctx);
            ctx.insert(struct_fields.begin(), struct_fields.end());
            solved_parameters.insert(struct_fields.begin(), struct_fields.end());
        } else {
            spdlog::warn("The parameter {} can't be solved, defaulting to 0",  next.value().get_name());
            ctx[next.value()] = 0;
            solved_parameters[next.value()] = 0;
        }
        s.purge(next.value());
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
        ast_param->set_value(solved_parameters.at(param->get_identifier()));
        node_parameters.insert(ast_param);
    }

    node->set_parameters(node_parameters);
}

resolved_parameter parameter_solver::resolve_instance_dependency(
    const qualified_identifier &dep,
    work_order &work,
    const std::shared_ptr<data_store> &d_store
) {
    auto instance_name = dep.get_instance();
    std::shared_ptr<HDL_instance_AST> examined_node = work.node;

    auto current_ports = work.node->get_ports();
    if (current_ports.contains(dep.get_instance())) {
        instance_name = current_ports.at(dep.get_instance())[0].get_name();
        examined_node = work.node->get_parent();
    } else if (work.interfaces_map.contains(dep.get_instance())) {
        resolve_interface_chain(work, d_store, examined_node, instance_name);
    } else if (examined_node) {
        examined_node = examined_node->get_parent();
    }

    if (examined_node) {
        for (const auto &brother_inst : examined_node->get_dependencies()) {
            if (brother_inst->get_name() == instance_name) {
                auto inst_param = brother_inst->get_parameters().get(dep.get_name());
                auto val = inst_param->get_numeric_value();
                if (val.has_value()) {
                    return val.value();
                }
                spdlog::warn("The instance parameter {}::{} has no value, using 0 as a default", dep.get_instance(), dep.get_name());
                return resolved_parameter(0);
            }
        }
    }

    auto path = get_full_path(work.node);
    spdlog::warn("The instance parameter {}.{}::{} was not found, using 0 as a default", path, dep.get_instance(), dep.get_name());
    resolved_parameter value;
    value.set_undefined();
    return value;
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

    propagate_functions(node_spec, d_store);

    auto solved_parameters = retrieve_package_parameters(combined_params, d_store);
    auto solution = solve_complex_overrides(work, d_store, solved_parameters);
    solved_parameters.insert(solution.begin(), solution.end());

    update_parameters_map(solved_parameters, work.node, d_store);
    return solved_parameters;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::retrieve_package_parameters(const Parameters_map &node_parameters, const std::shared_ptr<data_store> &d_store) {
    std::map<qualified_identifier, resolved_parameter> package_parameters;
    for (auto &[p_name, param]: node_parameters) {
        for (const auto& dep: param->get_dependencies().data) {
            if (!dep.get_package_prefix().empty() && !package_parameters.contains(dep)) {
                auto package = d_store->get_HDL_resource(dep.get_package_prefix().back());
                auto pkg_solved = process_parameters(package.get_parameters(), {});
                for (auto &[pkg_id, pkg_val]: pkg_solved) {
                    qualified_identifier qid{dep.get_package_prefix().back(), "", pkg_id.get_name()};
                    package_parameters[qid] = pkg_val;
                }
            }
        }
    }
    return package_parameters;
}

void parameter_solver::propagate_functions(HDL_Resource &resource, const std::shared_ptr<data_store> &d_store) {

    for (auto &[_, param] : resource.get_parameters()) {

        auto deps = param->get_dependencies();
        for (const auto& fcn:deps.functions) {
            if (!fcn.get_package_prefix().empty()) {
                auto res = d_store->get_HDL_resource(fcn.get_package_prefix().back());
                auto fcn_def = res.get_function(fcn.get_name());
                param->propagate_function(fcn_def);
            } else {
                param->propagate_function(resource.get_function(fcn.get_name()));
            }
        }

    }
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::solve_complex_overrides(
    work_order &work,
    const std::shared_ptr<data_store> &d_store,
    const std::map<qualified_identifier, resolved_parameter> &node_defaults
) {
    auto node_spec = d_store->get_HDL_resource(work.node->get_type());
    auto node_parameters = node_spec.get_parameters();
    auto node_overrides = work.node->get_parameters();

    Parameters_map to_solve;
    for(const auto &[p_name, param]: node_parameters) {
        auto i = p_name;
        if (node_overrides.contains(p_name)) {
            to_solve.insert(node_overrides.get(p_name));
        } else {
            to_solve.insert(param);
        }
    }

    for(auto &[override_name, param]:node_overrides) {
        if (node_parameters.contains(override_name)) {
            param->set_type(node_parameters.get(override_name)->get_type());
        }
    }

    std::map<qualified_identifier, resolved_parameter> ctx;
    ctx.insert(work.parent_parameters.begin(), work.parent_parameters.end());
    ctx.insert(node_defaults.begin(), node_defaults.end());

    for(auto &[override_name, param]:node_overrides) {
        for(auto &dep: param->get_dependencies().data) {
            if (ctx.contains(dep)) continue;
            if (!dep.get_instance().empty()) {
                ctx[dep] = resolve_instance_dependency(dep, work, d_store);
            } else if (dep.get_package_prefix().empty() && dep.get_instance().empty() && to_solve.contains(dep.get_name())) {
                continue;
            } else if(!node_overrides.contains(dep.get_name())) {
                spdlog::warn("Parameter {}::{} is not defined in the design", dep.get_package_prefix().back(), dep.get_name());
                resolved_parameter value;
                value.set_undefined();
                ctx[dep] = value;
            }
        }
    }

    for (const auto &[p_name, param] : node_parameters) {
        auto p_id = param->get_identifier();
        if (ctx.contains(p_id)) continue;
        for (auto &dep : param->get_dependencies().data) {
            if (!dep.get_instance().empty() && !ctx.contains(dep)) {
                ctx[dep] = resolve_instance_dependency(dep, work, d_store);
            }
        }
    }

    return process_parameters(to_solve, ctx);
}

std::string parameter_solver::get_full_path(const std::shared_ptr<HDL_instance_AST> &node) {
    std::string res;

    std::shared_ptr<HDL_instance_AST> current_node = node;

    while (current_node != nullptr) {
        res = current_node->get_name() + "." + res;
        current_node = current_node->get_parent();
    }
    res.pop_back();

    return res;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::extract_struct_fields(
    const std::shared_ptr<HDL_parameter> &param,
    const resolved_parameter &res,
    const qualified_identifier &id,
    const std::map<qualified_identifier, resolved_parameter> &ctx
) {
    std::map<qualified_identifier, resolved_parameter> fields;
    auto type = param->get_type();
    if (!type || !type->is<HDL_struct_type>()) return fields;
    auto &st = type->as<HDL_struct_type>();

    if (res.is_integer()) {
        auto type_info = st.evaluate_type(ctx);
        if (type_info) {
            uint64_t raw = res.get_integer().get_value();
            uint64_t offset = 0;
            for (int i = st.member.size() - 1; i >= 0; i--) {
                uint64_t w = 1;
                for (auto &ps : type_info->struct_sizes[i].packed_sizes) w *= ps;
                uint64_t mask = (w >= 64) ? ~0ULL : (1ULL << w) - 1;
                hdl_integer field_val = static_cast<uint64_t>((raw >> offset) & mask);
                qualified_identifier new_identifier(st.member[i].name);
                new_identifier.set_instance_prefix({id.get_name()});
                fields[new_identifier] = field_val;
                offset += w;
            }
        }
    } else if (res.is_int_array()) {
        auto arr = res.get_int_array();
        for (size_t i = 0; i < st.member.size(); i++) {
            int arr_idx = st.member.size() - 1 - i;
            auto field_val = arr.get_value({static_cast<int64_t>(arr_idx)});
            if (field_val) {
                qualified_identifier new_identifier(st.member[i].name);
                new_identifier.set_instance_prefix({id.get_name()});
                fields[new_identifier] = field_val.value();
            }
        }
    }
    return fields;
}
