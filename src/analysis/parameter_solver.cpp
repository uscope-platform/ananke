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
#include "analysis/package_solver.hpp"
#include "crash_context.hpp"

std::shared_ptr<package_solver> parameter_solver::pkg_solver_;

package_solver & parameter_solver::packages() {
    if (!pkg_solver_) pkg_solver_ = std::make_shared<package_solver>();
    return *pkg_solver_;
}

void parameter_solver::reset_packages() {
    pkg_solver_.reset();
}

#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/components/expression_traversal.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/parameters/components/Replication.hpp"
#include "data_model/HDL/parameters/components/Ternary.hpp"
#include "data_model/HDL/parameters/components/Cast.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/HDL_builtin_function.hpp"
#include "data_model/HDL/parameters/components/Streaming.hpp"
#include "data_model/HDL/parameters/components/token/Type_ref.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/statement/hdl_import_stmt.hpp"
#include "data_model/hdl_file.hpp"
#include "data_model/HDL/types/hdl_type.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"
#include "data_model/mdarray.hpp"
#include "data_model/HDL/statement/hdl_statements.hpp"
#include "frontend/analysis/system_verilog/type_engine.hpp"

#include <algorithm>
#include <atomic>
#include <functional>
#include <set>
#include <unordered_set>
#include <sstream>
#include <unordered_map>

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
    if (auto it = ports.find(current_instance); it != ports.end())
        instance_name = it->second[0].get_name();
    else return;
    examined_node = parent;

    while (is_interface_at(instance_name, examined_node->get_parent())) {
        auto next_parent = examined_node->get_parent();
        ports = next_parent->get_ports();
        if (auto it = ports.find(instance_name); it != ports.end())
            instance_name = it->second[0].get_name();
        else break;
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
        } else {
            node->visit_subexpressions([&](const std::shared_ptr<Expression_base> &c) {
                stack.push_back(c);
            });
        }
    }
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::process_parameters(
    const std::vector<std::shared_ptr<HDL_parameter>> &params_in,
    const std::map<qualified_identifier, resolved_parameter> &context
) {
    std::map<qualified_identifier, resolved_parameter> ctx = context;

    std::map<qualified_identifier, resolved_parameter> solved_parameters;

    std::unordered_map<std::string, std::shared_ptr<HDL_parameter>> by_name;
    std::map<qualified_identifier, std::shared_ptr<hdl_type>> type_map;
    std::map<qualified_identifier, std::shared_ptr<hdl_type>> type_ctx;
    for (const auto &param : params_in) {
        if (!param) continue;
        by_name[param->get_name()] = param;
        type_map[qualified_identifier(param->get_name())] = param->get_type();
        if (param->is_type_param && param->get_type()) {
            type_ctx[qualified_identifier(param->get_name())] = param->get_type();
        }
        auto ev = extract_enum_values(param);
        ctx.insert(ev.begin(), ev.end());
    }

    topological_sorter s;
    s.analyze(params_in, ctx);

    while (auto next = s.get_next()) {
        auto param = by_name.at(next.value().get_name());
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
    std::vector<std::shared_ptr<HDL_parameter>> node_parameters = node->get_parameters();
    std::unordered_map<std::string, size_t> node_idx;
    for (size_t i = 0; i < node_parameters.size(); ++i) {
        if (node_parameters[i]) node_idx[node_parameters[i]->get_name()] = i;
    }
    auto resource = d_store->get_HDL_resource(node->get_type());
    for (const auto &param : resource.value()->get_parameter_statements()) {
        const auto &p_name = param->get_name();
        auto solved_it = solved_parameters.find(param->get_identifier());
        // Loop locals have no parent-scope value (solved per iteration during
        // unrolling instead): skip them here rather than throwing on .at().
        if (solved_it == solved_parameters.end()) continue;
        std::shared_ptr<HDL_parameter> ast_param;
        auto it = node_idx.find(p_name);
        if (it != node_idx.end())
            ast_param = std::make_shared<HDL_parameter>(*node_parameters[it->second]);
        else
            ast_param = std::make_shared<HDL_parameter>(*param);
        ast_param->set_value(solved_it->second);
        if (it != node_idx.end()) node_parameters[it->second] = ast_param;
        else {
            node_idx[p_name] = node_parameters.size();
            node_parameters.push_back(ast_param);
        }
    }

    node->set_parameters(node_parameters);
}

resolved_parameter parameter_solver::resolve_instance_dependency(
    const qualified_identifier &dep,
    work_order &work,
    const std::shared_ptr<data_store> &d_store
) {
    const auto &inst_path = dep.get_instance();
    const auto &inst_key = inst_path.back();
    std::string instance_name = inst_key;
    std::shared_ptr<hdl_ast_node> examined_node = work.node;

    auto current_ports = work.node->get_ports();
    if (auto it = current_ports.find(inst_key); it != current_ports.end()) {
        instance_name = it->second[0].get_name();
        examined_node = work.node->get_parent();
    } else if (work.interfaces_map.contains(inst_key)) {
        resolve_interface_chain(work, d_store, examined_node, instance_name);
    } else {
        examined_node = examined_node->get_parent();
    }

    if (examined_node) {
        for (const auto &brother_inst : examined_node->get_dependencies()) {
            if (brother_inst->get_name() == instance_name) {
                auto inst_param = brother_inst->find_parameter(dep.get_name());
                if (!inst_param) return resolved_parameter(0);
                auto val = inst_param->get_numeric_value();
                if (val.has_value()) {
                    return val.value();
                }
                spdlog::warn("The instance parameter {}::{} has no value, using 0 as a default", inst_key, dep.get_name());
                return resolved_parameter(0);
            }
        }
    }

    auto path = get_full_path(work.node);
    spdlog::warn("The instance parameter {}.{}::{} was not found, using 0 as a default", path, inst_key, dep.get_name());
    resolved_parameter value;
    value.set_undefined();
    return value;
}

std::map<qualified_identifier, resolved_parameter> parameter_solver::override_parameters(
    work_order &work, const std::shared_ptr<data_store> &d_store,
    const std::vector<package_import> &imports,
    const std::map<std::string, hdl_function_statement> &imported_functions,
    const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types) {
    auto node_spec = d_store->get_HDL_resource(work.node->get_type());
    if (!node_spec.has_value()) {
        spdlog::critical("Definition for module {} not found while solving parameters of instance {}",
            work.node->get_type(), work.node->get_name());
        return {};
    }
    auto node_overrides = work.node->get_parameters();
    auto node_parameters = node_spec.value()->get_parameter_statements();

    std::vector<std::shared_ptr<HDL_parameter>> retrieve_inputs = node_parameters;
    for (const auto &param : node_overrides) {
        if (param) retrieve_inputs.push_back(param);
    }

    propagate_functions(node_spec.value(), d_store);
    propagate_imports(node_spec.value(), imported_functions, imported_types);

    // Flow-scoped package solving via the shared package_solver: every
    // instance in the flow reuses solved packages.
    auto solved_parameters = packages().retrieve(retrieve_inputs, d_store);

    propagate_types(node_spec.value(), d_store);
    propagate_port_types(node_spec.value(), imported_types, d_store);

    // Bare-only imports (`use pkg.all` / `import pkg::ITEM` with no qualified
    // reference) leave no package-prefixed dep to discover by, so their
    // packages are solved in a separate retrieve whose pkg::x keys NEVER enter
    // the solving context — only the filter-stripped bare values do. Merging
    // them into the union above would widen the overlay's provider set and leak
    // non-imported members (e.g. selective `import pkg::ITEM` would expose bare
    // `OTHER` via the single-provider alias).
    std::vector<std::pair<std::string, std::shared_ptr<hdl_resource_statement>>> explicit_packages;
    for (const auto &imp : imports) {
        if (imp.package) explicit_packages.emplace_back(imp.package_name, imp.package);
    }
    std::map<qualified_identifier, resolved_parameter> import_solved;
    if (!explicit_packages.empty())
        import_solved = packages().retrieve({}, d_store, explicit_packages);
    for (const auto &imp : imports) {
        if (!imp.package) continue;
        const std::vector<std::string> prefix{imp.package_name};
        for (auto &[id, val] : import_solved) {
            if (id.get_package_prefix() != prefix) continue;
            if (imp.wildcard || id.get_name() == imp.item)
                solved_parameters[qualified_identifier(id.get_name())] = val;
        }
    }
    auto solution = solve_complex_overrides(work, d_store, solved_parameters);
    solved_parameters.insert(solution.begin(), solution.end());

    update_parameters_map(solved_parameters, work.node, d_store);

    return solved_parameters;
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
    if (expr->is<Expression_v2>() || expr->is<Ternary>() || expr->is<Cast>() ||
        expr->is<Replication>()) {
        expr->visit_subexpressions([&](const std::shared_ptr<Expression_base> &c) {
            remap_keyed_literals(c, type);
        });
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
    for (const auto &param : resource->get_parameter_statements()) {
        const auto deps = param->get_dependencies();
        for (auto &fcn : deps.functions) {
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
        for (auto &type : deps.types) {
            if (!type.get_package_prefix().empty()) continue;
            if (imported_types.contains(type.get_name()))
                param->set_type(imported_types.at(type.get_name()));
        }
        // A declared type name that wasn't resolvable at parse time keeps its
        // name on the (dimensionless) placeholder type; swap in the imported
        // typedef so the real width/orientation is used.
        if (auto cur = param->get_type(); cur && cur->is<HDL_simple_type>()) {
            auto &simple = cur->as<HDL_simple_type>();
            if (imported_types.contains(simple.get_type_name()))
                param->set_type(imported_types.at(simple.get_type_name()));
        }
    }
}

void parameter_solver::propagate_types(std::shared_ptr<hdl_resource_statement> &resource, const std::shared_ptr<data_store> &d_store) {
    for (const auto &param : resource->get_parameter_statements()) {
        // If the parameter type itself is directly an external type
        if (param->get_type()->is<HDL_external_type>()) {
            auto &ext = param->get_type()->as<HDL_external_type>();
            if (ext.get_value().get_package_prefix().empty()) continue;
            auto pkg_name = ext.get_value().get_package_prefix().back();
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
                    ext.get_value().get_package_prefix().back(), ext.get_value().get_name());
                if (!res.has_value()) continue;
                auto type_def = res.value()->get_typedefs()[ext.get_value().get_name()];
                if (type_def) id_token.set_expression_type(type_def);
            } else {
                // Cast-size typedef special (see above): resolve before the
                // shared child walk pushes size + content.
                if (node->is<Cast>()) {
                    auto &c = node->as<Cast>();
                    if (auto size = c.get_size_expr()) {
                        // Typedef in cast-size position (e.g. fp_format_e in
                        // `pkg::T'(x)`, which the frontend builds as a size cast
                        // when the typedef lives in another file): resolve it like
                        // a type placeholder so the dep classifies as a type, not
                        // a value. Otherwise it is walked past and the size can
                        // never evaluate.
                        if (size->is<Identifier_token>()) {
                            auto &size_id = size->as<Identifier_token>();
                            const auto pfx = size_id.get_value().get_package_prefix();
                            if (!pfx.empty()) {
                                auto res = d_store->get_package_typedef_owner(
                                    pfx.back(), size_id.get_value().get_name());
                                if (res.has_value()) {
                                    auto type_def = res.value()->get_typedefs()[size_id.get_value().get_name()];
                                    if (type_def) {
                                        size_id.set_expression_type(type_def);
                                        size_id.set_type_placeholder(true);
                                    }
                                }
                            }
                        }
                    }
                }
                node->visit_subexpressions([&](const std::shared_ptr<Expression_base> &c) {
                    stack.push_back(c);
                });
            }
            }
        }
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
    for (const auto &pp : resource->get_parameter_statements()) {
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

// Resolve param-dependent dimension bounds of a type bound across scopes.
// A typedef's bare bounds (e.g. [WIDTH-1:0]) belong to the scope where the
// typedef was declared. When the type is passed through a type-param override
// into another module, re-resolving those bounds at a later $bits site would
// pick up shadowing names (or warn as undefined). Evaluating evaluable bounds
// now, with the binding scope's values, carries the owner scope in the type.
// Bounds that do not evaluate stay symbolic, preserving today's behavior.
std::shared_ptr<Expression_base> resolve_bound(
    const std::shared_ptr<Expression_base> &bound,
    const std::map<qualified_identifier, resolved_parameter> &ctx) {
    if (!bound) return bound;
    auto val = bound->evaluate(ctx);
    if (!val.has_value() || !val->is_integer()) return bound;
    return std::make_shared<Numeric_token>(std::to_string(val->get_integer().get_value()));
}

dimension_t resolve_dimension(
    const dimension_t &d,
    const std::map<qualified_identifier, resolved_parameter> &ctx) {
    dimension_t out = d;
    out.first_bound = resolve_bound(d.first_bound, ctx);
    out.second_bound = resolve_bound(d.second_bound, ctx);
    return out;
}

std::vector<dimension_t> resolve_dimensions(
    const std::vector<dimension_t> &dims,
    const std::map<qualified_identifier, resolved_parameter> &ctx) {
    std::vector<dimension_t> out;
    out.reserve(dims.size());
    for (const auto &d : dims) out.push_back(resolve_dimension(d, ctx));
    return out;
}

std::shared_ptr<hdl_type> resolve_type_dims(
    const std::shared_ptr<hdl_type> &t,
    const std::map<qualified_identifier, resolved_parameter> &ctx) {
    if (!t) return t;
    if (t->is<HDL_simple_type>()) {
        auto copy = t->as<HDL_simple_type>();
        // NOTE: setters append, so rebuild into a fresh object instead of
        // clear-then-set on the copy.
        HDL_simple_type fresh;
        fresh.set_signed(copy.get_signed());
        fresh.set_real(copy.get_real());
        fresh.set_implicit(copy.get_implicit());
        fresh.set_type_name(copy.get_type_name());
        fresh.set_packed_dimensions(resolve_dimensions(copy.get_packed_dimensions(), ctx));
        fresh.set_unpacked_dimensions(resolve_dimensions(copy.get_unpacked_dimensions(), ctx));
        return std::make_shared<HDL_simple_type>(fresh);
    }
    if (t->is<HDL_struct_type>()) {
        auto copy = t->as<HDL_struct_type>();
        HDL_struct_type fresh;
        fresh.packed = copy.packed;
        fresh.member.reserve(copy.member.size());
        for (const auto &m : copy.member) {
            struct_member fm;
            fm.name = m.name;
            fm.type = resolve_type_dims(m.type, ctx);
            fresh.member.push_back(std::move(fm));
        }
        fresh.set_unpacked_dimensions(resolve_dimensions(copy.get_unpacked_dimensions(), ctx));
        return std::make_shared<HDL_struct_type>(fresh);
    }
    if (t->is<HDL_union_type>()) {
        auto copy = t->as<HDL_union_type>();
        HDL_union_type fresh;
        fresh.packed = copy.packed;
        fresh.members.reserve(copy.members.size());
        for (const auto &m : copy.members) {
            struct_member fm;
            fm.name = m.name;
            fm.type = resolve_type_dims(m.type, ctx);
            fresh.members.push_back(std::move(fm));
        }
        return std::make_shared<HDL_union_type>(fresh);
    }
    if (t->is<HDL_enum_type>()) {
        auto copy = t->as<HDL_enum_type>();
        HDL_enum_type fresh;
        fresh.members = copy.members;
        fresh.base_type = resolve_type_dims(copy.base_type, ctx);
        fresh.set_unpacked_dimensions(resolve_dimensions(copy.get_unpacked_dimensions(), ctx));
        return std::make_shared<HDL_enum_type>(fresh);
    }
    return t;
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
        for (const auto &p : parent_node->get_parameters()) {
            if (p && p->is_type_param && p->get_type()) scope[p->get_name()] = p->get_type();
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
            auto pkg = imp->is_wildcard()
                ? d_store->get_HDL_resource(imp->get_package())
                : d_store->get_package_member_owner(imp->get_package(), imp->get_item());
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
    // Reused across calls: this runs per override per instance, so keep the
    // buffer instead of heap-allocating a fresh vector every time. Moved into
    // a frame-local for the walk (a thread_local touched per iteration costs
    // more than the malloc it saves) and handed back at the end. Safe: the
    // loop body never re-enters this function; entries are always consumed.
    // An exception mid-walk just drops the buffer; the static stays valid.
    thread_local static std::vector<std::shared_ptr<Expression_base>> retained;
    std::vector<std::shared_ptr<Expression_base>> stack = std::move(retained);
    stack.push_back(expr);
    while (!stack.empty()) {
        auto node = std::move(stack.back());
        stack.pop_back();
        if (!node) continue;
        if (node->is<HDL_builtin_function>()) {
            auto &b = node->as<HDL_builtin_function>();
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
                        if (id.is_bare()) {
                            auto it = scope_types.find(id.get_name());
                            if (it != scope_types.end() && it->second) {
                                tok.set_expression_type(it->second);
                                tok.set_type_placeholder(true);
                            }
                        }
                    }
                    for (auto &idx : tok.get_array_index()) {
                        if (idx) stack.push_back(idx);
                    }
                } else if (first) {
                    stack.push_back(first);
                }
                for (size_t i = 1; i < args.size(); ++i) {
                    if (args[i]) stack.push_back(args[i]);
                }
                continue;
            }
            for (auto &arg : args) {
                if (arg) stack.push_back(arg);
            }
            continue;
        }
        if (node->is<HDL_function_call>()) {
            for (auto &arg : node->as<HDL_function_call>().get_arguments()) {
                if (arg) stack.push_back(arg);
            }
            continue;
        }
        if (node->is<Identifier_token>()) {
            for (auto &idx : node->as<Identifier_token>().get_array_index()) {
                if (idx) stack.push_back(idx);
            }
            continue;
        }
        if (node->is<Expression_v2>() || node->is<Concatenation>() || node->is<Replication>() ||
            node->is<Ternary>() || node->is<Cast>() || node->is<Streaming>()) {
            node->visit_subexpressions([&](const std::shared_ptr<Expression_base> &c) {
                if (c) stack.push_back(c);
            });
        }
    }
    retained = std::move(stack);
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

    struct work_item {
        std::shared_ptr<Expression_base> node;
        bool in_type;
    };
    // Same reuse discipline as annotate_override_type_queries above: steal
    // the retained buffer into a frame-local (TLS per iteration costs more
    // than the malloc it saves), hand it back at the end.
    thread_local static std::vector<work_item> retained;
    std::vector<work_item> stack = std::move(retained);
    stack.push_back({expr, in_type_operand});
    while (!stack.empty()) {
        auto [node, in_type] = std::move(stack.back());
        stack.pop_back();
        if (!node) continue;
        if (node->is<HDL_builtin_function>()) {
            auto &b = node->as<HDL_builtin_function>();
            bool tq = is_type_query_builtin(b.get_function());
            const auto &args = b.get_arguments();
            for (size_t i = 0; i < args.size(); ++i) {
                // Only the first argument is the type operand; dimension
                // arguments (e.g. $size(v, K)) stay value positions. A nested
                // type query re-enters the type operand regardless of the
                // outer context (e.g. $clog2($bits(port.field))).
                if (args[i]) stack.push_back({args[i], tq && i == 0});
            }
            continue;
        }
        if (node->is<HDL_function_call>()) {
            for (auto &arg : node->as<HDL_function_call>().get_arguments()) {
                if (arg) stack.push_back({arg, false});
            }
            continue;
        }
        if (node->is<Identifier_token>()) {
            auto &tok = node->as<Identifier_token>();
            // Array indices are value positions, never the type operand.
            for (auto &idx : tok.get_array_index()) {
                if (idx) stack.push_back({idx, false});
            }
            auto id = tok.get_value();
            const auto &inst = id.get_instance();
            std::string root;
            std::vector<std::string> fields;
            bool whole_port = false;
            if (!inst.empty()) {
                root = inst[0];
                if (!ports.contains(root)) continue;
                fields.insert(fields.end(), inst.begin() + 1, inst.end());
                fields.push_back(id.get_name());
            } else {
                // Bare port name: only when it does not shadow a parameter.
                if (!ports.contains(id.get_name()) || param_names.contains(id.get_name())) continue;
                root = id.get_name();
                whole_port = true;
            }
            auto base = ports.at(root);
            auto mt = whole_port ? resolve_port_type_ref(base, resource, imported_types, d_store)
                                 : walk_port_field(base, fields, resource, imported_types, d_store);
            if (!mt || mt->is<HDL_external_type>()) continue;
            // NOTE: dependencies are computed from the raw_value tree, so this
            // only feeds type queries ($bits(port.field)); value resolution of
            // port references still misses as before. The instance-dependency
            // skip in solve_complex_overrides keeps those misses quiet.
            if (in_type) tok.set_expression_type(mt);
            continue;
        }
        if (node->is<Expression_v2>() || node->is<Concatenation>() || node->is<Replication>() ||
            node->is<Ternary>() || node->is<Cast>() || node->is<Streaming>()) {
            node->visit_subexpressions([&](const std::shared_ptr<Expression_base> &c) {
                if (c) stack.push_back({c, in_type});
            });
        }
    }
    retained = std::move(stack);
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
    std::vector<std::shared_ptr<HDL_parameter>> params;
    for (const auto &p : resource->get_parameter_statements()) {
        param_names.insert(p->get_name());
        params.push_back(p);
    }
    for (const auto &param : params) {
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
    auto res = d_store->get_package_typedef_owner(ext.get_value().get_package_prefix().back(),
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
    // The pair is keyed by parameter identity, not name: same-name
    // declarations from different generate branches are distinct objects and
    // each needs its own propagation.
    std::map<const HDL_parameter*, std::set<qualified_identifier>> attempted;
    bool progress = true;
    while (progress) {
        progress = false;
        for (const auto &param : resource->get_parameter_statements()) {
            auto deps = param->get_dependencies();
            for (const auto& fcn:deps.functions) {
                if (attempted[param.get()].contains(fcn)) continue;
                attempted[param.get()].insert(fcn);
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
    // Spec declarations in order, with a name index for the membership and
    // spec-object lookups below (to_solve covers exactly the spec names, so
    // one index serves both). Overrides indexed once instead of a linear
    // find_parameter per spec.
    auto spec_decls = node_spec.value()->get_parameter_statements();
    std::unordered_map<std::string, std::shared_ptr<HDL_parameter>> spec_by_name;
    for (const auto &p : spec_decls) {
        if (p) spec_by_name[p->get_name()] = p;
    }
    std::unordered_map<std::string, std::shared_ptr<HDL_parameter>> override_by_name;
    for (const auto &o : work.node->get_parameters()) {
        if (o) override_by_name[o->get_name()] = o;
    }

    std::vector<std::shared_ptr<HDL_parameter>> to_solve;
    for (const auto &param : spec_decls) {
        if (!param) continue;
        auto oit = override_by_name.find(param->get_name());
        to_solve.push_back(oit != override_by_name.end() ? oit->second : param);
    }

    // Loop locals (params depending on generate loop vars) have no parent-scope
    // value: they are solved per iteration during loop unrolling
    // (elaborate_loop_locals) and evaluated in the child solve via the parent
    // scope frame. Exclude them here so this function is a single
    // straight solve over parent-scope params. Instance overrides are always
    // solved (concrete per-instance values).
    std::unordered_map<std::string, parameter_deps_t> dep_cache;
    for (const auto &param : to_solve) {
        if (param) dep_cache.emplace(param->get_name(), param->get_dependencies());
    }
    std::set<std::string> skipped;
    bool progress = true;
    while (progress) {
        progress = false;
        for (const auto &param : to_solve) {
            const auto &p_name = param->get_name();
            if (override_by_name.contains(p_name)) continue;
            if (skipped.contains(p_name)) continue;
            if (depends_on_loop(dep_cache.at(p_name), skipped)) {
                skipped.insert(p_name);
                progress = true;
            }
        }
    }
    to_solve.erase(std::remove_if(to_solve.begin(), to_solve.end(),
        [&](const auto &p) { return skipped.contains(p->get_name()); }), to_solve.end());

    std::map<qualified_identifier, std::shared_ptr<hdl_type>> parent_type_ctx;
    std::shared_ptr<hdl_resource_statement> parent_resource;
    auto parent_node = work.node->get_parent();
    if (parent_node) {
        auto parent_spec = d_store->get_HDL_resource(parent_node->get_type());
        if (parent_spec.has_value()) {
            parent_resource = parent_spec.value();
            for (const auto &pp : parent_resource->get_parameter_statements()) {
                if (pp->is_type_param && pp->get_type()) {
                    parent_type_ctx[qualified_identifier(pp->get_name())] = pp->get_type();
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
            for (const auto &param : work.node->get_parameters()) {
                annotate_override_type_queries(param->get_expression(), scope_types, d_store);
            }
        }
    }

    for (const auto &param : work.node->get_parameters()) {
        const auto &override_name = param->get_name();
        auto spec_it = spec_by_name.find(override_name);
        if (spec_it != spec_by_name.end()) {
            auto spec_param = spec_it->second;
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
    if (work.scope_chain.size() > 1) {
        const auto &parent_frame = work.scope_chain[work.scope_chain.size() - 2].params;
        if (parent_frame) ctx.insert(parent_frame->begin(), parent_frame->end());
    }
    ctx.insert(node_defaults.begin(), node_defaults.end());
    if (auto overlaid = overlay_unambiguous_scope(ctx)) {
        ctx = std::move(*overlaid);
    }

    // type parameters should be resolved at the binding stage, not where used (for example in $bits() calls)
    for (const auto &param : work.node->get_parameters()) {
        if (!param || !param->is_type_param || !param->get_type()) continue;
        if (!override_by_name.contains(param->get_name())) continue;
        param->set_type(resolve_type_dims(param->get_type(), ctx));
    }

    std::set<std::string> type_derived_bare;
    for (const auto &ts_param : to_solve) {
        auto t = ts_param->get_type();
        if (!t) continue;
        for (auto &td : t->get_dependencies().data) {
            if (td.is_bare())
                type_derived_bare.insert(td.get_name());
        }
    }

    for (const auto &param : work.node->get_parameters()) {
        std::optional<qualified_identifier> dtype_ref;
        if (auto expr = param->get_expression(); param->is_type_param && expr) {
            if (expr->is<Identifier_token>())
                dtype_ref = expr->as<Identifier_token>().get_value();
        }
        for(auto &dep: param->get_dependencies().data) {
            if (ctx.contains(dep)) continue;
            if (dtype_ref.has_value() && dep == dtype_ref.value()) continue;
            if (!dep.get_instance().empty()) {
                auto first_inst = dep.get_instance()[0];
                if (spec_by_name.contains(first_inst)) {
                    continue;
                }
                ctx[dep] = resolve_instance_dependency(dep, work, d_store);
            } else if (dep.is_bare() && spec_by_name.contains(dep.get_name())) {
                continue;
            } else if(!override_by_name.contains(dep.get_name())) {
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

    // Iterate the filtered to_solve set (not spec_decls): skipped loop locals
    // are never solved, so their deps must not pollute ctx with spurious
    // "not found" resolutions. Overridden entries appear here as the override
    // objects, whose deps are the ones actually evaluated.
    for (const auto &param : to_solve) {
        auto p_id = param->get_identifier();
        if (ctx.contains(p_id)) continue;
        for (auto &dep : param->get_dependencies().data) {
            if (!dep.get_instance().empty() && !ctx.contains(dep)) {
                auto first_inst = dep.get_instance()[0];
                if (spec_by_name.contains(first_inst)) {
                    continue;
                }
                if (own_ports.contains(first_inst)) {
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
    // Explicit work stack instead of recursion: struct/union nesting can be
    // arbitrarily deep. Frames are just shared_ptrs (no per-frame state),
    // so a plain local vector does no heap traffic worth reusing.
    std::vector<std::shared_ptr<hdl_type>> stack{type};
    while (!stack.empty()) {
        auto node = std::move(stack.back());
        stack.pop_back();
        if (!node) continue;
        if (node->is<HDL_enum_type>()) {
            auto &et = node->as<HDL_enum_type>();
            for (const auto &m : et.members) {
                if (m.value.has_value())
                    fields[qualified_identifier(m.name)] = static_cast<hdl_integer>(m.value.value());
            }
            continue;
        }
        // Recurse into member types so aliases typed by enums (possibly through
        // more structs) seed bare members too. Simple/external leaves terminate.
        const std::vector<struct_member> *members = nullptr;
        if (node->is<HDL_struct_type>())
            members = &node->as<HDL_struct_type>().member;
        else if (node->is<HDL_union_type>())
            members = &node->as<HDL_union_type>().members;
        if (!members) continue;
        for (const auto &m : *members) {
            if (m.type) stack.push_back(m.type);
        }
    }
}
