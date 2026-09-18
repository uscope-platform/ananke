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

#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/parameters/components/Replication.hpp"
#include "data_model/HDL/parameters/components/Ternary.hpp"
#include "data_model/HDL/parameters/components/Cast.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/HDL_builtin_function.hpp"
#include "data_model/HDL/parameters/components/Streaming.hpp"
#include "data_model/HDL/parameters/components/token/Type_ref.hpp"
#include "data_model/HDL/statement/hdl_import_stmt.hpp"
#include "data_model/hdl_file.hpp"
#include "data_model/HDL/types/hdl_type.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"
#include "data_model/mdarray.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"

#include <set>
#include <sstream>

void parameter_solver::resolve_interface_chain(
    work_order &work,
    const std::shared_ptr<data_store> &d_store,
    std::shared_ptr<hdl_ast_node> &examined_node,
    std::string &instance_name
) {
    auto is_interface_at = [&](const std::string &name, const std::shared_ptr<hdl_ast_node> &node) -> bool {
        if (!node) return false;
        auto node_type = node->get_type();
        auto res = d_store->get_HDL_resource(node_type);
        if (!res.has_value())return false;
        auto ports = res.value()->get_port_specs();
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

static void annotate_identifier_types(
    const std::shared_ptr<Expression_base> &root,
    const std::map<qualified_identifier, std::shared_ptr<hdl_type>> &type_map
) {
    if (!root) return;

    std::vector<std::shared_ptr<Expression_base>> stack;
    stack.push_back(root);

    while (!stack.empty()) {
        auto node = stack.back();
        stack.pop_back();
        if (!node) continue;

        if (node->is<Identifier_token>()) {
            auto &id_token = node->as<Identifier_token>();
            auto it = type_map.find(id_token.get_value());
            if (it != type_map.end()) {
                id_token.set_expression_type(it->second);
            }
        } else if (node->is<Expression_v2>()) {
            auto &e = node->as<Expression_v2>();
            if (e.get_rhs()) stack.push_back(e.get_rhs());
            if (e.get_lhs()) stack.push_back(e.get_lhs());
        } else if (node->is<Concatenation>()) {
            for (auto &comp : node->as<Concatenation>().get_components()) {
                if (comp) stack.push_back(comp);
            }
        } else if (node->is<Replication>()) {
            auto &r = node->as<Replication>();
            if (r.get_size()) stack.push_back(r.get_size());
            if (r.get_item()) stack.push_back(r.get_item());
        } else if (node->is<Ternary>()) {
            auto &t = node->as<Ternary>();
            if (t.get_false_value()) stack.push_back(t.get_false_value());
            if (t.get_true_value()) stack.push_back(t.get_true_value());
            if (t.get_condition()) stack.push_back(t.get_condition());
        } else if (node->is<Cast>()) {
            auto &c = node->as<Cast>();
            if (c.get_size_expr()) stack.push_back(c.get_size_expr());
            if (c.get_content()) stack.push_back(c.get_content());
        } else if (node->is<HDL_function_call>()) {
            for (auto &arg : node->as<HDL_function_call>().get_arguments()) {
                if (arg) stack.push_back(arg);
            }
        } else if (node->is<HDL_builtin_function>()) {
            for (auto &arg : node->as<HDL_builtin_function>().get_arguments()) {
                if (arg) stack.push_back(arg);
            }
        } else if (node->is<Streaming>()) {
            auto &st = node->as<Streaming>();
            if (st.get_slice_size()) stack.push_back(st.get_slice_size());
            for (auto &comp : st.get_components()) {
                if (comp) stack.push_back(comp);
            }
        }
    }
}

static std::map<qualified_identifier, std::shared_ptr<hdl_type>> build_type_map(const Parameters_map &map) {
    std::map<qualified_identifier, std::shared_ptr<hdl_type>> types;
    for (const auto &[name, param] : map) {
        types[qualified_identifier(name)] = param->get_type();
    }
    return types;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::process_parameters(
    const Parameters_map &map_in,
    const std::map<qualified_identifier, resolved_parameter> &context
) {
    std::map<qualified_identifier, resolved_parameter> ctx = context;

    std::map<qualified_identifier, resolved_parameter> solved_parameters;

    topological_sorter s;
    s.analyze(map_in, ctx);

    auto type_map = build_type_map(map_in);

    std::map<qualified_identifier, std::shared_ptr<hdl_type>> type_ctx;
    for (const auto &[name, param] : map_in) {
        if (param->is_type_param && param->get_type()) {
            type_ctx[qualified_identifier(name)] = param->get_type();
        }
    }

    for (const auto &[name, param] : map_in) {
        auto ev = extract_enum_values(param);
        ctx.insert(ev.begin(), ev.end());
    }

    while (auto next = s.get_next()) {
        auto param = map_in.const_get(next.value().get_name());
        crash_ctx.parameter = next.value().get_name();
        if (param->is_type_param) {
            std::shared_ptr<hdl_type> resolved_type;
            if (param->get_expression() && param->get_expression()->is<Type_ref>()) {
                auto &ref = param->get_expression()->as<Type_ref>();
                auto it = type_ctx.find(ref.get_target());
                if (it != type_ctx.end()) {
                    resolved_type = it->second;
                    param->set_type(resolved_type);
                }
            }
            if (!resolved_type) resolved_type = param->get_type();
            if (!resolved_type) {
                spdlog::warn("Type parameter {} has no resolved type, defaulting to implicit", next.value().get_name());
                resolved_type = Type_engine::create_primitive_type("implicit");
                param->set_type(resolved_type);
            }
            type_ctx[next.value()] = resolved_type;
            ctx[next.value()] = 0;
            solved_parameters[next.value()] = 0;
            s.purge(next.value());
            continue;
        }

        if (param->get_expression()) {
            annotate_identifier_types(param->get_expression(), type_map);
        }
        auto res = param->evaluate(ctx);
        if (res) {
            if (res.value().is_undefined()) {
                spdlog::warn("The parameter {} is undefined, using 0 as a default", next.value().get_name());
            }
            ctx[next.value()] = res.value();
            solved_parameters[next.value()] = res.value();

            auto struct_fields = extract_struct_fields(param, res.value(), next.value(), ctx);
            ctx.insert(struct_fields.begin(), struct_fields.end());
            solved_parameters.insert(struct_fields.begin(), struct_fields.end());
            if (!struct_fields.empty()) {
                std::ostringstream summary;
                bool first = true;
                for (const auto &[fid, fval] : struct_fields) {
                    if (!first) summary << " ";
                    first = false;
                    summary << fid.print() << "=" << fval;
                }
                spdlog::trace("Solved struct parameter {}: {}", next.value().print(), summary.str());
            }
            auto enum_values = extract_enum_values(param);
            ctx.insert(enum_values.begin(), enum_values.end());
            solved_parameters.insert(enum_values.begin(), enum_values.end());
        } else {
            spdlog::warn("The parameter {} can't be solved ({}), defaulting to 0", next.value().get_name(), solver_error_name(res.error()));
            ctx[next.value()] = 0;
            solved_parameters[next.value()] = 0;
        }
        s.purge(next.value());
    }

    if (!s.empty()) {
        for (const auto &rem : s.get_remaining_nodes()) {
            spdlog::warn("The parameter {} is part of a circular dependency, defaulting to 0", rem.get_name());
            ctx[rem] = 0;
            solved_parameters[rem] = 0;
        }
    }

    return solved_parameters;
}


void parameter_solver::update_parameters_map(
    const std::map<qualified_identifier, resolved_parameter> &solved_parameters,
    const std::shared_ptr<hdl_ast_node>& node,
    const std::shared_ptr<data_store> &d_store
) {
    auto node_parameters = node->get_parameters();
    auto resource = d_store->get_HDL_resource(node->get_type());
    for(auto &[p_name, param]:resource.value()->get_parameters()) {
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
    auto instance_name = dep.get_instance().back();
    std::shared_ptr<hdl_ast_node> examined_node = work.node;

    auto current_ports = work.node->get_ports();
    if (current_ports.contains(dep.get_instance().back())) {
        instance_name = current_ports.at(dep.get_instance().back())[0].get_name();
        examined_node = work.node->get_parent();
    } else if (work.interfaces_map.contains(dep.get_instance().back())) {
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
                spdlog::warn("The instance parameter {}::{} has no value, using 0 as a default", dep.get_instance().back(), dep.get_name());
                return resolved_parameter(0);
            }
        }
    }

    auto path = get_full_path(work.node);
    spdlog::warn("The instance parameter {}.{}::{} was not found, using 0 as a default", path, dep.get_instance().back(), dep.get_name());
    resolved_parameter value;
    value.set_undefined();
    return value;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::override_parameters(
    work_order &work, const std::shared_ptr<data_store> &d_store,
    const std::map<qualified_identifier, resolved_parameter> &imported,
    const std::map<std::string, hdl_function_statement> &imported_functions,
    const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types) {
    auto node_spec = d_store->get_HDL_resource(work.node->get_type());
    if (!node_spec.has_value()) {
        spdlog::critical("Definition for module {} not found while solving parameters of instance {}",
            work.node->get_type(), work.node->get_name());
        return {};
    }
    auto node_overrides = work.node->get_parameters();
    auto node_parameters = node_spec.value()->get_parameters();

    //retrieve default package parameters
    Parameters_map combined_params = node_parameters;
    for (const auto &[name, param] : node_overrides) {
        combined_params.insert(param);
    }

    // Pre-propagate retrieve: spec params still carry external typedef
    // references (e.g. config_pkg::cfg_t). Overriding (.Cfg(Cfg)) hides the
    // spec default, so without this the typedef owner package (and its
    // bare dimension constants like NrMaxRules) would never be seeded.
    auto solved_parameters = retrieve_package_parameters(node_parameters, d_store);

    propagate_functions(node_spec.value(), d_store);
    propagate_types(node_spec.value(), d_store);
    propagate_imports(node_spec.value(), imported_functions, imported_types);
    propagate_port_types(node_spec.value(), imported_types, d_store);

    auto combined_solved = retrieve_package_parameters(combined_params, d_store);
    solved_parameters.insert(combined_solved.begin(), combined_solved.end());
    // Imported (`use pkg.all` / `import pkg::*`) constants enter scope unqualified.
    solved_parameters.insert(imported.begin(), imported.end());
    auto solution = solve_complex_overrides(work, d_store, solved_parameters);
    solved_parameters.insert(solution.begin(), solution.end());

    update_parameters_map(solved_parameters, work.node, d_store);

    return solved_parameters;
}
std::map<qualified_identifier, resolved_parameter> parameter_solver::retrieve_package_parameters(
    const Parameters_map &node_parameters,
    const std::shared_ptr<data_store> &d_store
) {
    std::map<qualified_identifier, resolved_parameter> package_parameters;
    std::set<std::string> resolved_packages;

    // Shared body: solve one package and export its params as pkg::name.
    // Returns false when the package was already handled (dedup).
    auto fetch_package = [&](const std::string &pkg_name,
                             std::shared_ptr<hdl_resource_statement> package) -> bool {
        if (!resolved_packages.insert(pkg_name).second) return false;

        // 1. FIRST: Scan sub-package dependencies recursively
        auto sub_pkg_deps = retrieve_package_parameters(package->get_parameters(), d_store);
        package_parameters.insert(sub_pkg_deps.begin(), sub_pkg_deps.end());

        // 2. SECOND: Extract typedef enums from THIS package into the context BEFORE processing its parameters
        for (const auto &[_, hdl_t] : package->get_typedefs()) {
            if (hdl_t && hdl_t->is<HDL_enum_type>()) {
                auto &et = hdl_t->as<HDL_enum_type>();
                for (const auto &m : et.members) {
                    if (m.value.has_value()) {
                        qualified_identifier qid{pkg_name, "", m.name};
                        package_parameters[qid] = static_cast<hdl_integer>(m.value.value());
                    }
                }
            }
        }
        propagate_types(package, d_store);
        // 3. THIRD: Now solve the package parameters with ALL enums and sub-package deps present in package_parameters!
        auto pkg_solved = process_parameters(package->get_parameters(), package_parameters);

        for (auto &[pkg_id, pkg_val] : pkg_solved) {
            // Canonical instance-preserving form (pkg::S.F): the
            // instance path survives so structured reads resolve.
            qualified_identifier qid(pkg_id.get_name());
            qid.set_package_prefix({pkg_name});
            const auto inst = pkg_id.get_instance();
            if (!inst.empty()) qid.set_instance_prefix(inst);
            package_parameters[qid] = pkg_val;
            // Flat legacy alias for bare pkg::FIELD reads (no struct
            // root). First-wins: mirrors the old flattening.
            if (!inst.empty()) {
                qualified_identifier flat(pkg_name, pkg_id.get_name());
                if (!package_parameters.contains(flat))
                    package_parameters[flat] = pkg_val;
            }
        }
        return true;
    };

    for (auto &[p_name, param] : node_parameters) {
        auto deps = param->get_dependencies();
        for (const auto& dep : deps.data) {
            std::string dbg_p = p_name;
            if (!dep.get_package_prefix().empty()) {
                auto pkg_name = dep.get_package_prefix().back();
                auto package = d_store->get_package_param_owner(pkg_name, dep);
                if (!package.has_value()) continue;
                fetch_package(pkg_name, package.value());
            }
        }
        // Package functions (p::f()) need their owner package's constants
        // (e.g. TAB for TAB[i].field reads inside the body) seeded, or the
        // nested evaluation misses and defaults to 0.
        for (const auto& fdep : deps.functions) {
            if (fdep.get_package_prefix().empty()) continue;
            auto pkg_name = fdep.get_package_prefix().back();
            auto package = d_store->get_package_function_owner(pkg_name, fdep.get_name());
            if (!package.has_value()) {
                auto res = d_store->get_HDL_resource(pkg_name);
                if (!res.has_value()) continue;
                package = res.value();
            }
            fetch_package(pkg_name, package.value());
        }
        // Typedef packages (e.g. config_pkg::cfg_t): the struct's bare
        // dimension deps (NrMaxRules) live in the typedef owner package.
        // Solving it here seeds pkg::NrMaxRules so the overlay in
        // HDL_simple_type::evaluate_type and solve_complex_overrides can
        // resolve the bare name in its defining context.
        for (const auto& tdep : deps.types) {
            if (tdep.get_package_prefix().empty()) continue;
            auto pkg_name = tdep.get_package_prefix().back();
            auto package = d_store->get_package_typedef_owner(pkg_name, tdep.get_name());
            if (!package.has_value()) continue;
            fetch_package(pkg_name, package.value());
        }
    }
    return package_parameters;
}

void parameter_solver::remap_keyed_literals(const std::shared_ptr<Expression_base> &expr,
    const std::shared_ptr<hdl_type> &type) {
    if (!expr) return;
    if (expr->is<Concatenation>()) {
        auto &concat = expr->as<Concatenation>();
        const bool was_keyed = !concat.get_component_keys().empty();
        bool reordered = false;
        std::shared_ptr<HDL_struct_type> st;
        if (type && type->is<HDL_struct_type>())
            st = std::dynamic_pointer_cast<HDL_struct_type>(type);
        if (was_keyed && st) {
            std::vector<std::string> names;
            for (auto &m : st->member) names.push_back(m.name);
            reordered = concat.reorder_by_member_names(names);
        }
        const bool aligned = !was_keyed || (reordered && concat.get_component_keys().empty());
        auto comps = concat.get_components();
        for (size_t i = 0; i < comps.size(); ++i) {
            std::shared_ptr<hdl_type> child;
            if (aligned && st && i < st->member.size()) child = st->member[i].type;
            remap_keyed_literals(comps[i], child);
        }
        return;
    }
    if (expr->is<Expression_v2>()) {
        auto &e = expr->as<Expression_v2>();
        remap_keyed_literals(e.get_lhs(), type);
        remap_keyed_literals(e.get_rhs(), type);
        return;
    }
    if (expr->is<Ternary>()) {
        auto &t = expr->as<Ternary>();
        remap_keyed_literals(t.get_condition(), type);
        remap_keyed_literals(t.get_true_value(), type);
        remap_keyed_literals(t.get_false_value(), type);
        return;
    }
    if (expr->is<Cast>()) {
        auto &c = expr->as<Cast>();
        remap_keyed_literals(c.get_content(), type);
        remap_keyed_literals(c.get_size_expr(), type);
        return;
    }
    if (expr->is<Replication>()) {
        auto &r = expr->as<Replication>();
        remap_keyed_literals(r.get_item(), type);
        remap_keyed_literals(r.get_size(), type);
        return;
    }
    if (expr->is<Streaming>()) {
        for (auto &c : expr->as<Streaming>().get_components()) remap_keyed_literals(c, type);
        return;
    }
    // Call arguments initialize formals whose types are unknown here: start
    // a fresh (untyped) context instead of carrying the outer struct type.
    if (expr->is<HDL_function_call>()) {
        for (auto &a : expr->as<HDL_function_call>().get_arguments()) remap_keyed_literals(a, nullptr);
        return;
    }
    if (expr->is<HDL_builtin_function>()) {
        for (auto &a : expr->as<HDL_builtin_function>().get_arguments()) remap_keyed_literals(a, nullptr);
        return;
    }

}

void parameter_solver::propagate_imports(std::shared_ptr<hdl_resource_statement> &resource,
                                         const std::map<std::string, hdl_function_statement> &imported_functions,
                                         const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types) {
    std::map<std::string, hdl_function_def_ptr> linked_definitions;
    for (auto &[_, param] : resource->get_parameters()) {
        for (auto &fcn : param->get_dependencies().functions) {
            if (!fcn.get_package_prefix().empty()) continue;
            if (!imported_functions.contains(fcn.get_name())) continue;
            auto &linked_definition = linked_definitions.try_emplace(fcn.get_name()).first->second;
            if (!linked_definition) {
                auto definition_snapshot =
                    std::make_shared<hdl_function_statement>(imported_functions.at(fcn.get_name()));
                auto return_type = definition_snapshot->get_return_type();
                if (return_type && return_type->is<HDL_external_type>()) {
                    auto external_name = return_type->as<HDL_external_type>().get_value().get_name();
                    if (imported_types.contains(external_name) && imported_types.at(external_name))
                        definition_snapshot->set_return_type(imported_types.at(external_name));
                }
                linked_definition = std::move(definition_snapshot);
            }
            param->propagate_function(linked_definition);
        }
        for (auto &type : param->get_dependencies().types) {
            if (!type.get_package_prefix().empty()) continue;
            if (imported_types.contains(type.get_name()))
                param->set_type(imported_types.at(type.get_name()));
        }
        // A declared type name that wasn't resolvable at parse time keeps its
        // name on the (dimensionless) placeholder type; swap in the imported
        // typedef so the real width/orientation is used.
        if (param->get_type() && param->get_type()->is<HDL_simple_type>()) {
            auto &simple = param->get_type()->as<HDL_simple_type>();
            if (imported_types.contains(simple.get_type_name()))
                param->set_type(imported_types.at(simple.get_type_name()));
        }
    }
}

void parameter_solver::propagate_types(std::shared_ptr<hdl_resource_statement> &resource, const std::shared_ptr<data_store> &d_store) {
    for (auto &[_, param] : resource->get_parameters()) {
        // If the parameter type itself is directly an external type
        if (param->get_type()->is<HDL_external_type>()) {
            auto &ext = param->get_type()->as<HDL_external_type>();
            auto pkg_name = ext.get_value().get_package_prefix()[0];
            auto type_name = ext.get_value().get_name();
            auto res = d_store->get_package_typedef_owner(pkg_name, type_name);
            if (res.has_value()) {
                auto type_def = res.value()->get_typedefs()[type_name];
                if (type_def) {
                    auto ext_unpacked = ext.get_unpacked_dimensions();
                    if (!ext_unpacked.empty()) {
                        if (type_def->is<HDL_simple_type>()) {
                            auto fused = type_def->as<HDL_simple_type>();
                            auto unpacked = fused.get_unpacked_dimensions();
                            unpacked.insert(unpacked.end(), ext_unpacked.begin(), ext_unpacked.end());
                            fused.set_unpacked_dimensions(unpacked);
                            param->set_type(std::make_shared<HDL_simple_type>(fused));
                        } else if (type_def->is<HDL_struct_type>()) {
                            auto fused = type_def->as<HDL_struct_type>();
                            auto unpacked = fused.get_unpacked_dimensions();
                            unpacked.insert(unpacked.end(), ext_unpacked.begin(), ext_unpacked.end());
                            fused.set_unpacked_dimensions(unpacked);
                            param->set_type(std::make_shared<HDL_struct_type>(fused));
                        } else {
                            param->set_type(type_def);
                        }
                    } else {
                        param->set_type(type_def);
                    }
                }
            }
        }

        auto deps = param->get_dependencies();
        for (const auto& type:deps.types) {
            if (!type.get_package_prefix().empty()) {
                auto cur = param->get_type();
                if (!cur || !cur->is<HDL_external_type>()) continue;
                if (!(type == cur->as<HDL_external_type>().get_value())) continue;
                auto res = d_store->get_package_typedef_owner(type.get_package_prefix().back(), type.get_name());
                if (!res.has_value()) {
                    spdlog::critical("Definition of package {} not found while propagating types",type.get_package_prefix().back());
                    return;
                }
                auto type_def = res.value()->get_typedefs()[type.get_name()];
                param->set_type(type_def);
            }
        }
        if (d_store) {
            auto root = param->get_expression();
            if (!root) continue;
            std::vector<std::shared_ptr<Expression_base>> stack{root};
            while (!stack.empty()) {
            auto node = stack.back();
            stack.pop_back();
            if (!node) continue;
            if (node->is<Identifier_token>()) {
                auto &id_token = node->as<Identifier_token>();
                if (!id_token.is_type_placeholder()) continue;
                auto t = id_token.get_expression_type();
                if (!t || !t->is<HDL_external_type>()) continue;
                auto &ext = t->as<HDL_external_type>();
                if (ext.get_value().get_package_prefix().empty()) continue;
                auto res = d_store->get_package_typedef_owner(
                    ext.get_value().get_package_prefix()[0], ext.get_value().get_name());
                if (!res.has_value()) continue;
                auto type_def = res.value()->get_typedefs()[ext.get_value().get_name()];
                if (type_def) id_token.set_expression_type(type_def);
            } else if (node->is<Expression_v2>()) {
                auto &e = node->as<Expression_v2>();
                if (e.get_rhs()) stack.push_back(e.get_rhs());
                if (e.get_lhs()) stack.push_back(e.get_lhs());
            } else if (node->is<Concatenation>()) {
                for (auto &comp : node->as<Concatenation>().get_components()) {
                    if (comp) stack.push_back(comp);
                }
            } else if (node->is<Replication>()) {
                auto &r = node->as<Replication>();
                if (r.get_size()) stack.push_back(r.get_size());
                if (r.get_item()) stack.push_back(r.get_item());
            } else if (node->is<Ternary>()) {
                auto &t = node->as<Ternary>();
                if (t.get_false_value()) stack.push_back(t.get_false_value());
                if (t.get_true_value()) stack.push_back(t.get_true_value());
                if (t.get_condition()) stack.push_back(t.get_condition());
            } else if (node->is<Cast>()) {
                auto &c = node->as<Cast>();
                if (c.get_size_expr()) stack.push_back(c.get_size_expr());
                if (c.get_content()) stack.push_back(c.get_content());
            } else if (node->is<HDL_function_call>()) {
                for (auto &arg : node->as<HDL_function_call>().get_arguments()) {
                    if (arg) stack.push_back(arg);
                }
            } else if (node->is<HDL_builtin_function>()) {
                for (auto &arg : node->as<HDL_builtin_function>().get_arguments()) {
                    if (arg) stack.push_back(arg);
                }
            } else if (node->is<Streaming>()) {
                auto &st = node->as<Streaming>();
                if (st.get_slice_size()) stack.push_back(st.get_slice_size());
                for (auto &comp : st.get_components()) {
                    if (comp) stack.push_back(comp);
                }
            }
            }
        }
    }

    for (auto &[_, param] : resource->get_parameters()) {
        remap_keyed_literals(param->get_expression(), param->get_type());
    }
}

namespace {
// Single-step resolution of a port (or member) type reference: package
// qualified via the store, bare via local typedefs, type parameters and the
// file's imports. Returns the input when unresolvable.
std::shared_ptr<hdl_type> resolve_port_type_ref(
    const std::shared_ptr<hdl_type> &t,
    const std::shared_ptr<hdl_resource_statement> &resource,
    const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types,
    const std::shared_ptr<data_store> &d_store) {
    if (!t || !t->is<HDL_external_type>()) return t;
    auto q = t->as<HDL_external_type>().get_value();
    if (!q.get_package_prefix().empty()) {
        if (!d_store) return t;
        auto res = d_store->get_package_typedef_owner(q.get_package_prefix().back(), q.get_name());
        if (!res.has_value()) return t;
        auto tds = res.value()->get_typedefs();
        auto it = tds.find(q.get_name());
        if (it != tds.end() && it->second) return it->second;
        return t;
    }
    auto tds = resource->get_typedefs();
    auto it = tds.find(q.get_name());
    if (it != tds.end() && it->second) return it->second;
    for (const auto &[_, pp] : resource->get_parameters()) {
        if (pp->is_type_param && pp->get_name() == q.get_name() && pp->get_type())
            return pp->get_type();
    }
    auto ii = imported_types.find(q.get_name());
    if (ii != imported_types.end() && ii->second) return ii->second;
    return t;
}

// Walks a struct/union member path (e.g. stride in csr_param_i.stride)
// starting from a port's declared type.
std::shared_ptr<hdl_type> walk_port_field(
    const std::shared_ptr<hdl_type> &base, const std::vector<std::string> &fields,
    const std::shared_ptr<hdl_resource_statement> &resource,
    const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types,
    const std::shared_ptr<data_store> &d_store) {
    auto cur = resolve_port_type_ref(base, resource, imported_types, d_store);
    for (const auto &f : fields) {
        if (!cur) return nullptr;
        cur = resolve_port_type_ref(cur, resource, imported_types, d_store);
        if (cur->is<HDL_struct_type>()) {
            std::shared_ptr<hdl_type> next;
            for (const auto &m : cur->as<HDL_struct_type>().member) {
                if (m.name == f) { next = m.type; break; }
            }
            if (!next) return nullptr;
            cur = next;
        } else if (cur->is<HDL_union_type>()) {
            std::shared_ptr<hdl_type> next;
            for (const auto &m : cur->as<HDL_union_type>().members) {
                if (m.name == f) { next = m.type; break; }
            }
            if (!next) return nullptr;
            cur = next;
        } else {
            return nullptr;
        }
    }
    return resolve_port_type_ref(cur, resource, imported_types, d_store);
}

bool is_type_query_builtin(HDL_builtin_function::function f) {
    using function = HDL_builtin_function::function;
    switch (f) {
        case function::bits: case function::size:
        case function::left: case function::right:
        case function::high: case function::low:
        case function::dimensions: case function::unpacked_dimensions:
        case function::typename_fn:
            return true;
        default:
            return false;
    }
}

// Typedefs visible at an instance-override site (i.e. in the instantiating
// parent scope): the parent's local typedefs, the solved types of its type
// parameters, then the typedefs pulled in by the parent file's imports.
// Locals shadow imports, so imports only fill names not already present.
std::map<std::string, std::shared_ptr<hdl_type>> collect_override_scope_types(
    const std::shared_ptr<hdl_ast_node> &parent_node,
    const std::shared_ptr<hdl_resource_statement> &parent_resource,
    const std::shared_ptr<data_store> &d_store) {
    std::map<std::string, std::shared_ptr<hdl_type>> scope;
    if (parent_resource) {
        for (const auto &[name, t] : parent_resource->get_typedefs()) {
            if (t) scope[name] = t;
        }
    }
    if (parent_node) {
        for (const auto &[name, p] : parent_node->get_parameters()) {
            if (p && p->is_type_param && p->get_type()) scope[name] = p->get_type();
        }
    }
    if (parent_node && d_store && !parent_node->get_type().empty()) {
        std::string parent_path;
        if (!d_store->get_HDL_resource(parent_node->get_type(), parent_path).has_value()) return scope;
        auto file = d_store->get_file<hdl_file>(parent_path);
        if (!file.has_value()) return scope;
        for (const auto &stmt : file.value().get_content()) {
            auto imp = std::dynamic_pointer_cast<hdl_import_stmt>(stmt);
            if (!imp) continue;
            auto pkg = d_store->get_HDL_resource(imp->get_package());
            if (!pkg.has_value()) continue;
            for (const auto &[name, t] : pkg.value()->get_typedefs()) {
                if (!t) continue;
                if (!imp->is_wildcard() && imp->get_item() != name) continue;
                if (!scope.contains(name)) scope[name] = t;
            }
        }
    }
    return scope;
}

// Annotates bare typedef names used as the direct type operand of a type
// query ($bits(T), $size(T), ...) inside override expressions with the type
// from the parent scope, and resolves package-qualified placeholders
// ($bits(pkg::T)) to the real typedef. Bare type names otherwise parse as
// plain value identifiers and end up as bogus data dependencies ("Parameter
// ::T is not defined in the design"). Only the direct first argument is a
// type position; everything else (including array indices) stays a value
// position so real value dependencies are preserved.
void annotate_override_type_queries(
    const std::shared_ptr<Expression_base> &expr,
    const std::map<std::string, std::shared_ptr<hdl_type>> &scope_types,
    const std::shared_ptr<data_store> &d_store) {
    if (!expr) return;
    if (expr->is<HDL_builtin_function>()) {
        auto &b = expr->as<HDL_builtin_function>();
        const auto &args = b.get_arguments();
        if (is_type_query_builtin(b.get_function()) && !args.empty()) {
            if (auto first = args[0]; first && first->is<Identifier_token>()) {
                auto &tok = first->as<Identifier_token>();
                auto held = tok.get_expression_type();
                if (held && held->is<HDL_external_type>()) {
                    const auto q = held->as<HDL_external_type>().get_value();
                    std::shared_ptr<hdl_type> real;
                    if (!q.get_package_prefix().empty() && d_store) {
                        auto res = d_store->get_package_typedef_owner(
                            q.get_package_prefix().back(), q.get_name());
                        if (res.has_value()) {
                            auto tds = res.value()->get_typedefs();
                            auto it = tds.find(q.get_name());
                            if (it != tds.end()) real = it->second;
                        }
                    } else {
                        auto it = scope_types.find(q.get_name());
                        if (it != scope_types.end()) real = it->second;
                    }
                    if (real) {
                        tok.set_expression_type(real);
                        tok.set_type_placeholder(true);
                    }
                } else if (!tok.is_type_placeholder()) {
                    const auto id = tok.get_value();
                    if (id.get_package_prefix().empty() && id.get_instance().empty()) {
                        auto it = scope_types.find(id.get_name());
                        if (it != scope_types.end() && it->second) {
                            tok.set_expression_type(it->second);
                            tok.set_type_placeholder(true);
                        }
                    }
                }
                for (auto &idx : tok.get_array_index()) {
                    if (idx) annotate_override_type_queries(idx, scope_types, d_store);
                }
            } else if (first) {
                annotate_override_type_queries(first, scope_types, d_store);
            }
            for (size_t i = 1; i < args.size(); ++i) {
                if (args[i]) annotate_override_type_queries(args[i], scope_types, d_store);
            }
            return;
        }
        for (auto &arg : args) {
            if (arg) annotate_override_type_queries(arg, scope_types, d_store);
        }
        return;
    }
    if (expr->is<HDL_function_call>()) {
        for (auto &arg : expr->as<HDL_function_call>().get_arguments()) {
            if (arg) annotate_override_type_queries(arg, scope_types, d_store);
        }
        return;
    }
    if (expr->is<Identifier_token>()) {
        for (auto &idx : expr->as<Identifier_token>().get_array_index()) {
            if (idx) annotate_override_type_queries(idx, scope_types, d_store);
        }
        return;
    }
    if (expr->is<Expression_v2>()) {
        auto &e = expr->as<Expression_v2>();
        if (e.get_lhs()) annotate_override_type_queries(e.get_lhs(), scope_types, d_store);
        if (e.get_rhs()) annotate_override_type_queries(e.get_rhs(), scope_types, d_store);
    } else if (expr->is<Concatenation>()) {
        for (auto &comp : expr->as<Concatenation>().get_components()) {
            if (comp) annotate_override_type_queries(comp, scope_types, d_store);
        }
    } else if (expr->is<Replication>()) {
        auto &r = expr->as<Replication>();
        if (r.get_size()) annotate_override_type_queries(r.get_size(), scope_types, d_store);
        if (r.get_item()) annotate_override_type_queries(r.get_item(), scope_types, d_store);
    } else if (expr->is<Ternary>()) {
        auto &t = expr->as<Ternary>();
        if (t.get_condition()) annotate_override_type_queries(t.get_condition(), scope_types, d_store);
        if (t.get_true_value()) annotate_override_type_queries(t.get_true_value(), scope_types, d_store);
        if (t.get_false_value()) annotate_override_type_queries(t.get_false_value(), scope_types, d_store);
    } else if (expr->is<Cast>()) {
        auto &c = expr->as<Cast>();
        if (c.get_size_expr()) annotate_override_type_queries(c.get_size_expr(), scope_types, d_store);
        if (c.get_content()) annotate_override_type_queries(c.get_content(), scope_types, d_store);
    } else if (expr->is<Streaming>()) {
        auto &st = expr->as<Streaming>();
        if (st.get_slice_size()) annotate_override_type_queries(st.get_slice_size(), scope_types, d_store);
        for (auto &comp : st.get_components()) {
            if (comp) annotate_override_type_queries(comp, scope_types, d_store);
        }
    }
}

void annotate_port_tokens_expr(
    const std::shared_ptr<Expression_base> &expr,
    const std::map<std::string, std::shared_ptr<hdl_type>> &ports,
    const std::set<std::string> &param_names,
    const std::shared_ptr<hdl_resource_statement> &resource,
    const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types,
    const std::shared_ptr<data_store> &d_store,
    bool in_type_operand) {
    if (!expr) return;
    if (expr->is<HDL_builtin_function>()) {
        auto &b = expr->as<HDL_builtin_function>();
        bool tq = is_type_query_builtin(b.get_function());
        const auto &args = b.get_arguments();
        for (size_t i = 0; i < args.size(); ++i) {
            // Only the first argument is the type operand; dimension
            // arguments (e.g. $size(v, K)) stay value positions. A nested
            // type query re-enters the type operand regardless of the
            // outer context (e.g. $clog2($bits(port.field))).
            annotate_port_tokens_expr(args[i], ports, param_names, resource,
                imported_types, d_store, tq && i == 0);
        }
        return;
    }
    if (expr->is<HDL_function_call>()) {
        for (auto &arg : expr->as<HDL_function_call>().get_arguments()) {
            if (arg) annotate_port_tokens_expr(arg, ports, param_names, resource,
                imported_types, d_store, false);
        }
        return;
    }
    if (expr->is<Identifier_token>()) {
        auto &tok = expr->as<Identifier_token>();
        // Array indices are value positions, never the type operand.
        for (auto &idx : tok.get_array_index()) {
            if (idx) annotate_port_tokens_expr(idx, ports, param_names, resource,
                imported_types, d_store, false);
        }
        auto id = tok.get_value();
        const auto &inst = id.get_instance();
        std::string root;
        std::vector<std::string> fields;
        bool whole_port = false;
        if (!inst.empty()) {
            root = inst[0];
            if (!ports.contains(root)) return;
            fields.insert(fields.end(), inst.begin() + 1, inst.end());
            fields.push_back(id.get_name());
        } else {
            // Bare port name: only when it does not shadow a parameter.
            if (!ports.contains(id.get_name()) || param_names.contains(id.get_name())) return;
            root = id.get_name();
            whole_port = true;
        }
        auto base = ports.at(root);
        auto mt = whole_port ? resolve_port_type_ref(base, resource, imported_types, d_store)
                             : walk_port_field(base, fields, resource, imported_types, d_store);
        if (!mt || mt->is<HDL_external_type>()) return;
        // NOTE: dependencies are computed from the raw_value tree, so this
        // only feeds type queries ($bits(port.field)); value resolution of
        // port references still misses as before. The instance-dependency
        // skip in solve_complex_overrides keeps those misses quiet.
        if (in_type_operand) tok.set_expression_type(mt);
        return;
    }
    if (expr->is<Expression_v2>()) {
        auto &e = expr->as<Expression_v2>();
        if (e.get_lhs()) annotate_port_tokens_expr(e.get_lhs(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
        if (e.get_rhs()) annotate_port_tokens_expr(e.get_rhs(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
    } else if (expr->is<Concatenation>()) {
        for (auto &comp : expr->as<Concatenation>().get_components()) {
            if (comp) annotate_port_tokens_expr(comp, ports, param_names, resource,
                imported_types, d_store, in_type_operand);
        }
    } else if (expr->is<Replication>()) {
        auto &r = expr->as<Replication>();
        if (r.get_size()) annotate_port_tokens_expr(r.get_size(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
        if (r.get_item()) annotate_port_tokens_expr(r.get_item(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
    } else if (expr->is<Ternary>()) {
        auto &t = expr->as<Ternary>();
        if (t.get_condition()) annotate_port_tokens_expr(t.get_condition(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
        if (t.get_true_value()) annotate_port_tokens_expr(t.get_true_value(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
        if (t.get_false_value()) annotate_port_tokens_expr(t.get_false_value(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
    } else if (expr->is<Cast>()) {
        auto &c = expr->as<Cast>();
        if (c.get_size_expr()) annotate_port_tokens_expr(c.get_size_expr(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
        if (c.get_content()) annotate_port_tokens_expr(c.get_content(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
    } else if (expr->is<Streaming>()) {
        auto &st = expr->as<Streaming>();
        if (st.get_slice_size()) annotate_port_tokens_expr(st.get_slice_size(), ports, param_names,
            resource, imported_types, d_store, in_type_operand);
        for (auto &comp : st.get_components()) {
            if (comp) annotate_port_tokens_expr(comp, ports, param_names, resource,
                imported_types, d_store, in_type_operand);
        }
    }
}
} // namespace

void parameter_solver::propagate_port_types(
    std::shared_ptr<hdl_resource_statement> &resource,
    const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types,
    const std::shared_ptr<data_store> &d_store) {
    std::map<std::string, std::shared_ptr<hdl_type>> ports;
    for (const auto &[pname, pspec] : resource->get_port_specs()) {
        if (pspec.direction == interface_port) continue;
        if (pspec.type) ports[pname] = pspec.type;
    }
    if (ports.empty()) return;
    std::set<std::string> param_names;
    for (const auto &[pname, _] : resource->get_parameters()) param_names.insert(pname);
    for (auto &[_, param] : resource->get_parameters()) {
        annotate_port_tokens_expr(param->get_expression(), ports, param_names,
            resource, imported_types, d_store, false);
    }
}

namespace {
// Resolves a function definition's external return type in place (no-op if
// already resolved or unresolvable). Owner-side, pre-solve bookkeeping like
// the param set_type writes in propagate_types: deterministic (same typedef
// object every time), idempotent, and independent of call sites — it never
// carries per-site values, so sharing stays safe. Needed because evaluation
// packs struct returns per the return type's member order, which an
// unresolved external type cannot provide.
void resolve_function_return_type(const std::shared_ptr<hdl_function_statement> &def,
                                  const std::shared_ptr<data_store> &d_store) {
    if (!def || !d_store) return;
    auto rt = def->get_return_type();
    if (!rt || !rt->is<HDL_external_type>()) return;
    auto &ext = rt->as<HDL_external_type>();
    if (ext.get_value().get_package_prefix().empty()) return;
    auto res = d_store->get_package_typedef_owner(ext.get_value().get_package_prefix()[0],
                                                    ext.get_value().get_name());
    if (!res.has_value()) return;
    auto type_def = res.value()->get_typedefs()[ext.get_value().get_name()];
    if (type_def) def->set_return_type(type_def);
}



// Mutable-handle variant of get_function_shared for the resolution above:
// links keep using the const handle; only this owner-side pass writes.
std::shared_ptr<hdl_function_statement> find_function_def(
    const std::shared_ptr<hdl_resource_statement> &owner, const std::string &fname) {
    if (!owner) return nullptr;
    for (const auto &stmt : owner->get_statements()) {
        auto f = std::dynamic_pointer_cast<hdl_function_statement>(stmt);
        if (f && f->get_name() == fname) return f;
    }
    return nullptr;
}
}

void parameter_solver::propagate_functions(std::shared_ptr<hdl_resource_statement> &resource, const std::shared_ptr<data_store> &d_store) {

    // Bodies populated below can reveal further (nested) calls, so repeat
    // until no new definition is propagated. Each (parameter, function) pair
    // is attempted once, which also bounds (mutually) recursive functions.
    std::map<std::string, std::set<qualified_identifier>> attempted;
    bool progress = true;
    while (progress) {
        progress = false;
        for (auto &[name, param] : resource->get_parameters()) {

            auto deps = param->get_dependencies();
            for (const auto& fcn:deps.functions) {
                if (attempted[name].contains(fcn)) continue;
                attempted[name].insert(fcn);
                if (!fcn.get_package_prefix().empty()) {
                    if (!d_store) continue;
                    const auto pkg = fcn.get_package_prefix().back();
                    auto res = d_store->get_package_function_owner(pkg, fcn.get_name());
                    if (!res.has_value()) {
                        if (!d_store->get_HDL_resource(pkg).has_value()) {
                            spdlog::critical("Definition of package {} not found while propagating functions", pkg);
                            return;
                        }
                        spdlog::critical("Function {}::{}, not found in the specified package", pkg, fcn.get_name());
                        continue;
                    }
                    auto fcn_def = res.value()->get_function_shared(fcn.get_name());
                    if (!fcn_def) {
                        spdlog::critical("Function {}::{}, not found in the specified package", pkg, fcn.get_name());
                        continue;
                    }
                    resolve_function_return_type(find_function_def(res.value(), fcn.get_name()), d_store);
                    param->propagate_function(fcn_def);
                    // Package-internal nesting: bare calls inside fcn_def's
                    // body (e.g. fget inside p::fcall_mul) resolve against
                    // the same package, which the module-scope fixpoint
                    // below would never find. Link every package function
                    // into every other package body (idempotent shared
                    // links, safe by construction) so any depth resolves.
                    for (auto &[other_name, _] : res.value()->get_functions()) {
                        auto other_def = res.value()->get_function_shared(other_name);
                        if (!other_def) continue;
                        resolve_function_return_type(
                            find_function_def(res.value(), other_name), d_store);
                        for (auto &stmt : fcn_def->get_body()) {
                            if (stmt) stmt->propagate_function(other_def);
                        }
                    }
                    progress = true;
                } else {
                    if (auto local_fcn = resource->get_function_shared(fcn.get_name())) {
                        resolve_function_return_type(find_function_def(resource, fcn.get_name()), d_store);
                        param->propagate_function(local_fcn);
                        progress = true;
                    } else if (d_store) {
                        std::string path;
                        d_store->get_HDL_resource(resource->getName(), path);
                        auto standalone_function = d_store->get_standalone_function(fcn.get_name(), path);
                        if (standalone_function) {
                            // No stable owner exists for standalone copies, so
                            // snapshot once per attempt; all of this
                            // parameter's sites share the immutable snapshot.
                            auto definition_snapshot =
                                std::make_shared<hdl_function_statement>(standalone_function.value());
                            resolve_function_return_type(definition_snapshot, d_store);
                            hdl_function_def_ptr linked_definition = definition_snapshot;
                            param->propagate_function(linked_definition);
                            progress = true;
                        }
                    }

                }
            }

        }
    }
}


std::shared_ptr<hdl_type> parameter_solver::resolve_dtype_reference(
    const qualified_identifier &ref,
    const std::map<qualified_identifier, std::shared_ptr<hdl_type>> &parent_type_ctx,
    const std::shared_ptr<hdl_resource_statement> &parent_resource,
    const std::shared_ptr<data_store> &d_store
) {
    if (!ref.get_package_prefix().empty()) {
        auto res = d_store->get_package_typedef_owner(
            ref.get_package_prefix().back(), ref.get_name());
        if (res.has_value()) {
            auto tds = res.value()->get_typedefs();
            auto tit = tds.find(ref.get_name());
            if (tit != tds.end() && tit->second) return tit->second;
        }
        return nullptr;
    }
    auto it = parent_type_ctx.find(ref);
    if (it != parent_type_ctx.end()) return it->second;
    if (parent_resource) {
        auto tds = parent_resource->get_typedefs();
        auto tit = tds.find(ref.get_name());
        if (tit != tds.end() && tit->second) return tit->second;
    }
    return nullptr;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::solve_complex_overrides(
    work_order &work,
    const std::shared_ptr<data_store> &d_store,
    const std::map<qualified_identifier, resolved_parameter> &node_defaults
) {
    auto node_spec = d_store->get_HDL_resource(work.node->get_type());
    if (!node_spec.has_value()) {
        spdlog::critical("Definition for module {} not found while solving parameters of instance {}",
           work.node->get_type(), work.node->get_name());
        return {};
    }
    auto node_parameters = node_spec.value()->get_parameters();
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

    Parameters_map loop_locals;
        std::set<std::string> deferred;
        bool progress = true;
        while (progress) {
            progress = false;
            for (const auto &[p_name, param] : to_solve) {
                if (node_overrides.contains(p_name)) continue;
                if (deferred.contains(p_name)) continue;
                const auto deps = param->get_dependencies();
                bool loop_local = !deps.loop_vars.empty();
                if (!loop_local) {
                    for (const auto &dep : deps.data) {
                        if (dep.get_package_prefix().empty() && dep.get_instance().empty() &&
                            deferred.contains(dep.get_name())) {
                            loop_local = true;
                            break;
                        }
                    }
                }
                if (loop_local) {
                    loop_locals.insert(param);
                    deferred.insert(p_name);
                    progress = true;
                }
            }
        }
        for (const auto &[p_name, param] : loop_locals) to_solve.erase(p_name);

    std::map<qualified_identifier, std::shared_ptr<hdl_type>> parent_type_ctx;
    std::shared_ptr<hdl_resource_statement> parent_resource;
    auto parent_node = work.node->get_parent();
    if (parent_node) {
        auto parent_spec = d_store->get_HDL_resource(parent_node->get_type());
        if (parent_spec.has_value()) {
            parent_resource = parent_spec.value();
            for (const auto &[name, pp] : parent_resource->get_parameters()) {
                if (pp->is_type_param && pp->get_type()) {
                    parent_type_ctx[qualified_identifier(name)] = pp->get_type();
                }
            }
        }
    }

    // Bare typedef names in type queries ($bits(pma_t)) parse as plain value
    // identifiers when the defining package is only visible through the
    // parent's imports. Resolve them against the parent scope here so they
    // evaluate through the typedef instead of warning as missing parameters.
    if (parent_node) {
        auto scope_types = collect_override_scope_types(parent_node, parent_resource, d_store);
        if (!scope_types.empty()) {
            for (auto &[override_name, param] : node_overrides) {
                annotate_override_type_queries(param->get_expression(), scope_types, d_store);
            }
        }
    }

    for(auto &[override_name, param]:node_overrides) {
        if (node_parameters.contains(override_name)) {
            auto spec_param = node_parameters.get(override_name);
            if (spec_param->is_type_param) {
                param->is_type_param = true;
                bool type_resolved = false;
                if (param->get_expression() && param->get_expression()->is<Identifier_token>()) {
                    auto ref_name = param->get_expression()->as<Identifier_token>().get_value();
                    if (auto t = resolve_dtype_reference(ref_name, parent_type_ctx, parent_resource, d_store)) {
                        param->set_type(t);
                        type_resolved = true;
                    }
                } else if (param->get_expression() && param->get_expression()->is<Type_ref>()) {
                    auto ref_name = param->get_expression()->as<Type_ref>().get_target();
                    if (auto t = resolve_dtype_reference(ref_name, parent_type_ctx, parent_resource, d_store)) {
                        param->set_type(t);
                        type_resolved = true;
                    }
                }
                if (type_resolved) continue;
                spdlog::trace("dtype override {} keeps default type", override_name);
                param->set_type(spec_param->get_type());
            } else {
                param->set_type(spec_param->get_type());
            }
        }
    }

    std::map<qualified_identifier, resolved_parameter> ctx;
    ctx.insert(work.parent_parameters.begin(), work.parent_parameters.end());
    ctx.insert(node_defaults.begin(), node_defaults.end());
    if (auto overlaid = overlay_unambiguous_scope(ctx)) {
        ctx.insert(overlaid->begin(), overlaid->end());
    }

    std::set<std::string> type_derived_bare;
    for (auto &[ts_name, ts_param] : to_solve) {
        auto t = ts_param->get_type();
        if (!t) continue;
        for (auto &td : t->get_dependencies().data) {
            if (td.get_package_prefix().empty() && td.get_instance().empty())
                type_derived_bare.insert(td.get_name());
        }
    }

    for(auto &[override_name, param]:node_overrides) {
        auto p = param;
        std::optional<qualified_identifier> dtype_ref;
        if (param->is_type_param && param->get_expression()) {
            if (param->get_expression()->is<Identifier_token>())
                dtype_ref = param->get_expression()->as<Identifier_token>().get_value();
            else if (param->get_expression()->is<Type_ref>())
                dtype_ref = param->get_expression()->as<Type_ref>().get_target();
        }
        for(auto &dep: param->get_dependencies().data) {
            if (ctx.contains(dep)) continue;
            if (dtype_ref.has_value() && dep == dtype_ref.value()) continue;
            if (!dep.get_instance().empty()) {
                auto first_inst = dep.get_instance()[0];
                if (to_solve.contains(first_inst) || node_parameters.contains(first_inst)) {
                    continue;
                }
                ctx[dep] = resolve_instance_dependency(dep, work, d_store);
            } else if (dep.get_package_prefix().empty() && dep.get_instance().empty() && to_solve.contains(dep.get_name())) {
                continue;
            } else if(!node_overrides.contains(dep.get_name())) {
                if (type_derived_bare.contains(dep.get_name())) {
                    std::vector<std::map<qualified_identifier, resolved_parameter>::const_iterator> providers;
                    for (auto it = ctx.begin(); it != ctx.end(); ++it) {
                        if (!it->first.get_package_prefix().empty() &&
                            it->first.get_instance().empty() &&
                            it->first.get_name() == dep.get_name())
                            providers.push_back(it);
                    }
                    if (!providers.empty()) {
                        if (providers.size() > 1) {
                            spdlog::warn("Parameter {} is ambiguous ({} providers), picking {}.{}",
                                dep.get_name(), providers.size(),
                                providers.front()->first.get_package_prefix().back(),
                                providers.front()->first.get_name());
                        }
                        ctx[qualified_identifier(dep.get_name())] = providers.front()->second;
                        continue;
                    }
                }
                spdlog::warn("Parameter {}::{} is not defined in the design", dep.get_package_prefix().empty() ? "" : dep.get_package_prefix().back(), dep.get_name());
                resolved_parameter value;
                value.set_undefined();
                ctx[dep] = value;
            }
        }
    }

    // Own (non-interface) ports are not instance parameters: their values
    // are never constant, and type queries on them ($bits(port.field))
    // resolve through the port's declared type (see propagate_port_types).
    // Routing them through instance resolution only produces "not found"
    // warnings, so they are left out of the solving context.
    std::set<std::string> own_ports;
    for (const auto &[pname, pspec] : node_spec.value()->get_port_specs()) {
        if (pspec.direction != interface_port) own_ports.insert(pname);
    }

    for (const auto &[p_name, param] : node_parameters) {
        auto p_id = param->get_identifier();
        if (ctx.contains(p_id)) continue;
        for (auto &dep : param->get_dependencies().data) {
            if (!dep.get_instance().empty() && !ctx.contains(dep)) {
                auto first_inst = dep.get_instance()[0];
                if (to_solve.contains(first_inst) || node_parameters.contains(first_inst)) {
                    continue;
                }
                if (own_ports.contains(first_inst)) {
                    continue;
                }
                ctx[dep] = resolve_instance_dependency(dep, work, d_store);
            }
        }
    }

    auto solution = process_parameters(to_solve, ctx);
    // Silent placeholders for the deferred loop locals: no single
    // module-scope value exists, and solving them here would warn
    // "missing value" and default to 0. Per-iteration values are injected
    // during loop unrolling instead.
    for (const auto &[p_name, param] : loop_locals)
        solution[param->get_identifier()] = resolved_parameter(0);
    return solution;
}

std::string parameter_solver::get_full_path(const std::shared_ptr<hdl_ast_node> &node) {
    std::string res;

    std::shared_ptr<hdl_ast_node> current_node = node;

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
    if (!type || (!type->is<HDL_struct_type>() && !type->is<HDL_union_type>())) return fields;
    // Array-of-struct (unpacked dims on the struct reference): the value is an
    // element array, not one packed struct. Emit one field array per member
    // (TAB.exp_bits[i]) by slicing every element, so indexed field reads
    // (TAB[i].field) resolve via direct lookup + array indexing.
    if (type->is<HDL_struct_type>() && !type->as<HDL_struct_type>().get_unpacked_dimensions().empty()) {
        if (!res.is_int_array()) return fields;
        auto &st = type->as<HDL_struct_type>();
        auto type_info = st.evaluate_type(ctx);
        if (!type_info) return fields;
        auto elems = res.get_int_array().get_1d_slice({0, 0});
        if (elems.empty()) return fields;
        auto make_hdl = [](const wide_integer &v) {
            hdl_integer h;
            h.set_value(v);
            return h;
        };
        // Member offsets from LSB (first member is MSB, mirroring the
        // single-struct packing above).
        std::vector<uint64_t> widths(st.member.size(), 0);
        for (size_t i = 0; i < st.member.size(); ++i) {
            widths[i] = packed_width(type_info->struct_sizes[i].packed_sizes);
        }
        for (size_t i = 0; i < st.member.size(); ++i) {
            if (st.member[i].type && (st.member[i].type->is<HDL_struct_type>() ||
                                      st.member[i].type->is<HDL_union_type>())) {
                continue;
            }
            uint64_t offset = 0;
            for (size_t j = st.member.size(); j-- > i + 1;) {
                uint64_t reps = 1;
                for (auto &us : type_info->struct_sizes[j].unpacked_sizes) reps *= us;
                offset += widths[j] * reps;
            }
            wide_integer mask = hdl_integer::width_mask(static_cast<int64_t>(widths[i])).to_wide();
            std::vector<hdl_integer> field_elems;
            field_elems.reserve(elems.size());
            for (auto &e : elems) {
                wide_integer raw = e.to_wide();
                field_elems.push_back(make_hdl((raw >> static_cast<size_t>(offset)) & mask));
            }
            auto prefix = id.get_instance();
            prefix.push_back(id.get_name());
            qualified_identifier kid(st.member[i].name);
            kid.set_instance_prefix(prefix);
            if (!id.get_package_prefix().empty()) {
                kid.set_package_prefix(id.get_package_prefix());
            }
            mdarray<hdl_integer> field_arr;
            field_arr.set_1d_slice({0, 0}, field_elems);
            fields[kid] = resolved_parameter(field_arr);
        }
        return fields;
    }

    auto emit_field = [&](const std::string &member_name, hdl_integer member_value,
                          const std::shared_ptr<hdl_type> &member_type) {
        auto prefix = id.get_instance();
        prefix.push_back(id.get_name());
        qualified_identifier kid(member_name);
        kid.set_instance_prefix(prefix);
        if (member_type && (member_type->is<HDL_struct_type>() || member_type->is<HDL_union_type>())) {
            auto sub = std::make_shared<HDL_parameter>();
            sub->set_name(member_name);
            sub->set_type(member_type);
            sub->set_value(member_value);
            auto sp = id.get_instance();
            sp.push_back(id.get_name());
            qualified_identifier sid(member_name);
            sid.set_instance_prefix(sp);
            auto sub_fields = extract_struct_fields(sub, member_value, sid, ctx);
            fields.insert(sub_fields.begin(), sub_fields.end());
        } else {
            fields[kid] = member_value;
        }
    };

    if (res.is_integer()) {
        auto make_hdl = [](const wide_integer &v) {
            hdl_integer h;
            h.set_value(v);
            return h;
        };
        wide_integer raw = res.get_integer().to_wide();
        if (type->is<HDL_struct_type>()) {
            auto &st = type->as<HDL_struct_type>();
            auto type_info = st.evaluate_type(ctx);
            if (type_info) {
                uint64_t offset = 0;
                for (int i = st.member.size() - 1; i >= 0; i--) {
                    uint64_t w = packed_width(type_info->struct_sizes[i].packed_sizes);
                    uint64_t reps = 1;
                    for (auto &us : type_info->struct_sizes[i].unpacked_sizes) reps *= us;
                    // Exact mask at any width: the old >=1024 fallback (-1,
                    // i.e. no mask) leaked all higher members into wide
                    // fields such as the 1024-bit PMA regions.
                    wide_integer mask = hdl_integer::width_mask(static_cast<int64_t>(w)).to_wide();
                    if (reps == 1) {
                        emit_field(st.member[i].name,
                                   make_hdl((raw >> static_cast<size_t>(offset)) & mask),
                                   st.member[i].type);
                        offset += w;
                        continue;
                    }
                    // Array member: one entry per element. Element 0 sits at
                    // the member's high bits (mirror of packing consumption).
                    // Storage left-padded so indexed reads hit data[0][0][i].
                    const auto &usizes = type_info->struct_sizes[i].unpacked_sizes;
                    const size_t rank = usizes.size();
                    if (rank == 0 || rank > 3) {
                        offset += reps * w;
                        continue;
                    }
                    std::vector<uint64_t> shape(3 - rank, 1);
                    shape.insert(shape.end(), usizes.begin(), usizes.end());
                    mdarray<hdl_integer> arr(shape, make_hdl(0));
                    for (uint64_t n = 0; n < reps; n++) {
                        wide_integer piece =
                            (raw >> static_cast<size_t>(offset + (reps - 1 - n) * w)) & mask;
                        size_t tmp = static_cast<size_t>(n);
                        std::vector<int64_t> at(3, 0);
                        for (size_t d = rank; d-- > 0;) {
                            at[3 - rank + d] = static_cast<int64_t>(tmp % usizes[d]);
                            tmp /= usizes[d];
                        }
                        arr.set_value(at, make_hdl(piece));
                    }
                    auto prefix = id.get_instance();
                    prefix.push_back(id.get_name());
                    qualified_identifier kid(st.member[i].name);
                    kid.set_instance_prefix(prefix);
                    fields[kid] = resolved_parameter(arr);
                    offset += reps * w;
                }
            }
        } else {
            auto &ut = type->as<HDL_union_type>();
            for (const auto &m : ut.members) {
                uint64_t w = 1;
                if (m.type) {
                    auto s = m.type->evaluate_type(ctx);
                    if (s) w = packed_width(*s);
                }
                wide_integer mask = hdl_integer::width_mask(static_cast<int64_t>(w)).to_wide();
                emit_field(m.name, make_hdl(raw & mask), m.type);
            }
        }
    } else if (res.is_int_array()) {
        if (type->is<HDL_struct_type>()) {
            auto &st = type->as<HDL_struct_type>();
            auto arr = res.get_int_array();
            for (size_t i = 0; i < st.member.size(); i++) {
                int arr_idx = st.member.size() - 1 - i;
                auto field_val = arr.get_value({static_cast<int64_t>(arr_idx)});
                if (field_val) {
                    auto prefix = id.get_instance();
                    prefix.push_back(id.get_name());
                    qualified_identifier kid(st.member[i].name);
                    kid.set_instance_prefix(prefix);
                    fields[kid] = field_val.value();
                }
            }
        }
    }
    return fields;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::extract_enum_values(
    const std::shared_ptr<HDL_parameter> &param
) {
    std::map<qualified_identifier, resolved_parameter> fields;
    auto type = param->get_type();
    if (!type) return fields;
    collect_type_enum_values(type, fields);
    return fields;
}

void parameter_solver::collect_type_enum_values(
    const std::shared_ptr<hdl_type> &type,
    std::map<qualified_identifier, resolved_parameter> &fields
) {
    if (!type) return;
    if (type->is<HDL_enum_type>()) {
        auto &et = type->as<HDL_enum_type>();
        for (const auto &m : et.members) {
            if (m.value.has_value())
                fields[qualified_identifier(m.name)] = static_cast<hdl_integer>(m.value.value());
        }
        return;
    }
    // Recurse into member types so aliases typed by enums (possibly through
    // more structs) seed bare members too. Simple/external leaves terminate.
    const std::vector<struct_member> *members = nullptr;
    if (type->is<HDL_struct_type>())
        members = &type->as<HDL_struct_type>().member;
    else if (type->is<HDL_union_type>())
        members = &type->as<HDL_union_type>().members;
    if (!members) return;
    for (const auto &m : *members) {
        if (m.type) collect_type_enum_values(m.type, fields);
    }
}
