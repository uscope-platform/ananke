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

    propagate_functions(node_spec.value(), d_store);
    propagate_types(node_spec.value(), d_store);
    propagate_imports(node_spec.value(), imported_functions, imported_types);

    auto solved_parameters = retrieve_package_parameters(combined_params, d_store);
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

    for (auto &[p_name, param] : node_parameters) {
        for (const auto& dep : param->get_dependencies().data) {
            std::string dbg_p = p_name;
            if (!dep.get_package_prefix().empty()) {
                auto pkg_name = dep.get_package_prefix().back();
                auto package = d_store->get_package_param_owner(pkg_name, dep);
                if (!package.has_value()) continue;

                // 1. FIRST: Scan sub-package dependencies recursively
                auto sub_pkg_deps = retrieve_package_parameters(package.value()->get_parameters(), d_store);
                package_parameters.insert(sub_pkg_deps.begin(), sub_pkg_deps.end());

                // 2. SECOND: Extract typedef enums from THIS package into the context BEFORE processing its parameters
                for (const auto &[_, hdl_t] : package.value()->get_typedefs()) {
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
                propagate_types(package.value(), d_store);
                // 3. THIRD: Now solve the package parameters with ALL enums and sub-package deps present in package_parameters!
                auto pkg_solved = process_parameters(package.value()->get_parameters(), package_parameters);

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
            }
        }
    }
    return package_parameters;
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
                    param->set_type(type_def);
                }
            }
        }

        auto deps = param->get_dependencies();
        for (const auto& type:deps.types) {
            if (!type.get_package_prefix().empty()) {
                auto res = d_store->get_package_typedef_owner(type.get_package_prefix().back(), type.get_name());
                if (!res.has_value()) {
                    spdlog::critical("Definition of package {} not found while propagating types",type.get_package_prefix().back());
                    return;
                }
                auto type_def = res.value()->get_typedefs()[type.get_name()];
                param->set_type(type_def);
            }
        }
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

    std::map<qualified_identifier, std::shared_ptr<hdl_type>> parent_type_ctx;
    auto parent_node = work.node->get_parent();
    if (parent_node) {
        auto parent_spec = d_store->get_HDL_resource(parent_node->get_type());
        if (parent_spec.has_value()) {
            for (const auto &[name, pp] : parent_spec.value()->get_parameters()) {
                if (pp->is_type_param && pp->get_type()) {
                    parent_type_ctx[qualified_identifier(name)] = pp->get_type();
                }
            }
        }
    }

    for(auto &[override_name, param]:node_overrides) {
        if (node_parameters.contains(override_name)) {
            auto spec_param = node_parameters.get(override_name);
            if (spec_param->is_type_param) {
                param->is_type_param = true;
                if (param->get_expression() && param->get_expression()->is<Identifier_token>()) {
                    auto ref_name = param->get_expression()->as<Identifier_token>().get_value();
                    auto it = parent_type_ctx.find(ref_name);
                    if (it != parent_type_ctx.end()) {
                        param->set_type(it->second);
                        continue;
                    }
                }
                param->set_type(spec_param->get_type());
            } else {
                param->set_type(spec_param->get_type());
            }
        }
    }

    std::map<qualified_identifier, resolved_parameter> ctx;
    ctx.insert(work.parent_parameters.begin(), work.parent_parameters.end());
    ctx.insert(node_defaults.begin(), node_defaults.end());

    for(auto &[override_name, param]:node_overrides) {
        for(auto &dep: param->get_dependencies().data) {
            if (ctx.contains(dep)) continue;
            if (!dep.get_instance().empty()) {
                auto first_inst = dep.get_instance()[0];
                if (to_solve.contains(first_inst) || node_parameters.contains(first_inst)) {
                    continue;
                }
                ctx[dep] = resolve_instance_dependency(dep, work, d_store);
            } else if (dep.get_package_prefix().empty() && dep.get_instance().empty() && to_solve.contains(dep.get_name())) {
                continue;
            } else if(!node_overrides.contains(dep.get_name())) {
                spdlog::warn("Parameter {}::{} is not defined in the design", dep.get_package_prefix().empty() ? "" : dep.get_package_prefix().back(), dep.get_name());
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
                auto first_inst = dep.get_instance()[0];
                if (to_solve.contains(first_inst) || node_parameters.contains(first_inst)) {
                    continue;
                }
                ctx[dep] = resolve_instance_dependency(dep, work, d_store);
            }
        }
    }

    return process_parameters(to_solve, ctx);
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
                    wide_integer mask = (w >= 1024) ? wide_integer(-1) : (wide_integer(1) << w) - 1;
                    emit_field(st.member[i].name,
                               make_hdl((raw >> static_cast<size_t>(offset)) & mask),
                               st.member[i].type);
                    offset += w;
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
                wide_integer mask = (w >= 1024) ? wide_integer(-1) : (wide_integer(1) << w) - 1;
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
    if (!type || !type->is<HDL_enum_type>()) return fields;
    auto &et = type->as<HDL_enum_type>();
    for (const auto &m : et.members) {
        if (m.value.has_value())
            fields[qualified_identifier(m.name)] = static_cast<hdl_integer>(m.value.value());
    }
    return fields;
}
