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

#include "analysis/package_solver.hpp"

#include <functional>
#include <set>

#include "analysis/parameter_solver.hpp"
#include "data_model/data_store.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"

namespace {

// A package dependency edge: package name plus the owning resource that
// declares the referenced member (duplicate package names may resolve to
// distinct owners per member).
struct package_ref {
    std::string name;
    std::shared_ptr<hdl_resource_statement> owner;
};
struct package_ref_less {
    bool operator()(const package_ref &a, const package_ref &b) const {
        if (a.name != b.name) return a.name < b.name;
        return a.owner < b.owner;
    }
};

// Package references of a single parameter: data, function and typedef
// references.
std::vector<package_ref> param_package_refs(
    const std::shared_ptr<HDL_parameter> &param,
    const std::shared_ptr<data_store> &d_store) {
    std::vector<package_ref> refs;
    if (!param || !d_store) return refs;
    auto deps = param->get_dependencies();
    for (const auto &dep : deps.data) {
        if (dep.get_package_prefix().empty()) continue;
        auto pkg_name = dep.get_package_prefix().back();
        auto package = d_store->get_package_param_owner(pkg_name, dep);
        if (!package.has_value()) continue;
        refs.push_back({pkg_name, package.value()});
    }
    // Package functions (p::f()) need their owner package's constants
    // (e.g. TAB for TAB[i].field reads inside the body) seeded, or the
    // nested evaluation misses and defaults to 0.
    for (const auto &fdep : deps.functions) {
        if (fdep.get_package_prefix().empty()) continue;
        auto pkg_name = fdep.get_package_prefix().back();
        auto package = d_store->get_package_function_owner(pkg_name, fdep.get_name());
        if (!package.has_value()) {
            auto res = d_store->get_HDL_resource(pkg_name);
            if (!res.has_value()) continue;
            package = res.value();
        }
        refs.push_back({pkg_name, package.value()});
    }
    // Typedef packages (e.g. config_pkg::cfg_t): the struct's bare
    // dimension deps (NrMaxRules) live in the typedef owner package.
    // Solving it here seeds pkg::NrMaxRules so the overlay in
    // HDL_simple_type::evaluate_type and solve_complex_overrides can
    // resolve the bare name in its defining context.
    for (const auto &tdep : deps.types) {
        if (tdep.get_package_prefix().empty()) continue;
        auto pkg_name = tdep.get_package_prefix().back();
        auto package = d_store->get_package_typedef_owner(pkg_name, tdep.get_name());
        if (!package.has_value()) continue;
        refs.push_back({pkg_name, package.value()});
    }
    return refs;
}

// Transitive dependency closure of a package over memoized direct edges.
void collect_package_closure(
    const package_ref &ref,
    const std::map<package_ref, std::vector<package_ref>, package_ref_less> &edges,
    std::set<package_ref, package_ref_less> &out) {
    auto it = edges.find(ref);
    if (it == edges.end()) return;
    for (const auto &d : it->second) {
        if (out.insert(d).second) collect_package_closure(d, edges, out);
    }
}

}  // namespace

std::map<qualified_identifier, resolved_parameter> package_solver::retrieve(
    const std::vector<std::shared_ptr<HDL_parameter>> &node_parameters,
    const std::shared_ptr<data_store> &d_store,
    const std::vector<std::pair<std::string, std::shared_ptr<hdl_resource_statement>>> &explicit_packages
) {
    // Each owner's exports are solved once (in dependency-closure context)
    // and shared by every later call: packages take no instance overrides.
    std::map<package_ref, std::vector<package_ref>, package_ref_less> edges;
    std::set<package_ref, package_ref_less> in_progress;

    std::function<void(const package_ref &)> ensure_solved;
    ensure_solved = [&](const package_ref &ref) {
        package_solver::cache_key key{ref.name, ref.owner.get()};
        if (solved_.contains(key)) return;
        if (!in_progress.insert(ref).second) return;  // cyclic refs: solve without the back edge
        auto eit = edges.find(ref);
        if (eit == edges.end()) {
            std::vector<package_ref> edge_list;
            for (const auto &sub_param : ref.owner->get_parameter_statements()) {
                auto sub_refs = param_package_refs(sub_param, d_store);
                edge_list.insert(edge_list.end(), sub_refs.begin(), sub_refs.end());
            }
            eit = edges.emplace(ref, std::move(edge_list)).first;
        }
        for (const auto &d : eit->second) ensure_solved(d);
        in_progress.erase(ref);

        std::map<qualified_identifier, resolved_parameter> ctx;
        std::set<package_ref, package_ref_less> closure;
        collect_package_closure(ref, edges, closure);
        for (const auto &dep_key : closure) {
            auto cit = solved_.find({dep_key.name, dep_key.owner.get()});
            if (cit == solved_.end()) continue;  // cycle back-edge still solving
            ctx.insert(cit->second.begin(), cit->second.end());
        }

        auto package = ref.owner;
        export_map block;
        // Extract typedef enums into the context BEFORE processing parameters
        for (const auto &[_, hdl_t] : package->get_typedefs()) {
            if (hdl_t && hdl_t->is<HDL_enum_type>()) {
                auto &et = hdl_t->as<HDL_enum_type>();
                for (const auto &m : et.members) {
                    if (m.value.has_value()) {
                        qualified_identifier qid{ref.name, "", m.name};
                        block[qid] = static_cast<hdl_integer>(m.value.value());
                    }
                }
            }
        }
        parameter_solver::propagate_types(package, d_store);
        auto pkg_solved = parameter_solver::process_parameters(package->get_parameter_statements(), ctx);

        for (auto &[pkg_id, pkg_val] : pkg_solved) {
            // Canonical instance-preserving form (pkg::S.F): the
            // instance path survives so structured reads resolve.
            qualified_identifier qid(pkg_id.get_name());
            qid.set_package_prefix({ref.name});
            const auto inst = pkg_id.get_instance();
            if (!inst.empty()) qid.set_instance_prefix(inst);
            block[qid] = pkg_val;
            // Flat legacy alias for bare pkg::FIELD reads (no struct
            // root). First-wins: mirrors the old flattening.
            if (!inst.empty()) {
                qualified_identifier flat(ref.name, pkg_id.get_name());
                if (!block.contains(flat))
                    block[flat] = pkg_val;
            }
        }
        solved_[key] = std::move(block);
    };

    // Discovery (post-order): explicit packages first, then node parameters.
    // Owner lookups only, no solving. Explicitly imported packages (e.g.
    // `import pkg::*`) are solved even when no qualified reference triggers
    // their fetch: a bare-only use leaves no package-prefixed dep to
    // discover them by.
    std::vector<package_ref> order;
    std::set<package_ref, package_ref_less> visited;
    std::function<void(const std::string &, const std::shared_ptr<hdl_resource_statement> &)> visit;
    visit = [&](const std::string &pkg_name, const std::shared_ptr<hdl_resource_statement> &package) {
        package_ref ref{pkg_name, package};
        if (!visited.insert(ref).second) return;
        std::vector<package_ref> edge_list;
        for (const auto &sub_param : package->get_parameter_statements()) {
            auto sub_refs = param_package_refs(sub_param, d_store);
            for (const auto &r : sub_refs) visit(r.name, r.owner);
            edge_list.insert(edge_list.end(), sub_refs.begin(), sub_refs.end());
        }
        edges[ref] = std::move(edge_list);
        order.push_back(std::move(ref));
    };

    for (const auto &[pkg_name, package] : explicit_packages) {
        if (package) visit(pkg_name, package);
    }
    for (const auto &param : node_parameters) {
        for (const auto &r : param_package_refs(param, d_store)) visit(r.name, r.owner);
    }

    for (const auto &ref : order) ensure_solved(ref);

    // Blocks carry disjoint package-prefixed keys, so first-wins in discovery
    // order matches the old first-fetched-wins behavior for duplicate names.
    std::map<qualified_identifier, resolved_parameter> package_parameters;
    for (const auto &ref : order) {
        auto cit = solved_.find({ref.name, ref.owner.get()});
        if (cit == solved_.end()) continue;
        package_parameters.insert(cit->second.begin(), cit->second.end());
    }
    return package_parameters;
}
