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


#ifndef ANANKE_PARAMETER_SOLVING_PASS_HPP
#define ANANKE_PARAMETER_SOLVING_PASS_HPP

#include "analysis/passes/pass_base.hpp"
#include "data_model/HDL/statement/hdl_function_statement.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_union_type.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"
#include "analysis/topological_sorter.hpp"
#include "data_model/data_store.hpp"

#include <map>
#include <memory>
#include <set>


class data_store;
class hdl_resource_statement;
class hdl_package_statement;
class hdl_statement_base;
class package_solver;

struct pending_parameter_override {
    std::vector<std::string> path;
    std::string parameter_name;
    std::shared_ptr<Expression_base> value;
};

// File-level `use`/`import` statement resolved by the builder: the imported
// package (null when unresolvable — skipped) plus the wildcard/item filter.
// Solved package values enter the instance scope unqualified per this filter.
struct package_import {
    std::string package_name;
    std::shared_ptr<hdl_package_statement> package;
    bool wildcard = false;
    std::string item;
};

using param_map_t = std::map<qualified_identifier, resolved_parameter>;

// One scope frame per module on the instantiation path: the module name plus
// its solved parameter set. A single vector (not parallel arrays) so the
// lockstep invariant holds by construction.
struct scope_frame {
    std::string module;
    std::shared_ptr<param_map_t> params;
};

struct work_order {
    std::shared_ptr<hdl_ast_node> node;
    std::string path;
    std::unordered_map<std::string, std::string> interfaces_map;
    std::vector<pending_parameter_override> pending_overrides;
    std::vector<scope_frame> scope_chain;
};

class parameter_solver {
public:
    static std::map<qualified_identifier, resolved_parameter> process_parameters(
        const std::vector<std::shared_ptr<HDL_parameter>> &params,
        const std::map<qualified_identifier, resolved_parameter> &context
    );
    static void update_parameters_map(
        const std::map<qualified_identifier, resolved_parameter> &parameters,
        const std::shared_ptr<hdl_ast_node>& node,
        const std::shared_ptr<data_store> &d_store
        );

    static std::map<qualified_identifier, resolved_parameter> override_parameters(work_order &work, const std::shared_ptr<data_store> &d_store,
        const std::vector<package_import> &imports = {},
        const std::map<std::string, hdl_function_statement> &imported_functions = {},
        const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types = {});
    static void propagate_functions(std::shared_ptr<hdl_resource_statement> &resource, const std::shared_ptr<data_store> &d_store);
    static void propagate_functions(std::shared_ptr<hdl_package_statement> &resource, const std::shared_ptr<data_store> &d_store);
    static void propagate_types(std::shared_ptr<hdl_resource_statement> &resource, const std::shared_ptr<data_store> &d_store);
    static void propagate_types(std::shared_ptr<hdl_package_statement> &resource, const std::shared_ptr<data_store> &d_store);
    static void propagate_port_types(std::shared_ptr<hdl_resource_statement> &resource,
        const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types,
        const std::shared_ptr<data_store> &d_store);
    static void propagate_imports(std::shared_ptr<hdl_resource_statement> &resource,
        const std::map<std::string, hdl_function_statement> &imported_functions,
        const std::map<std::string, std::shared_ptr<hdl_type>> &imported_types);
    static void remap_keyed_literals(const std::shared_ptr<Expression_base> &expr,
                              const std::shared_ptr<hdl_type> &type);
    static std::map<qualified_identifier, resolved_parameter> solve_complex_overrides(
            work_order &work,
            const std::shared_ptr<data_store> &d_store,
            const std::map<qualified_identifier, resolved_parameter> &node_defaults
        );
    static std::string get_full_path(const std::shared_ptr<hdl_ast_node> &node);

    // Flow-scoped package solver: one instance per elaboration flow, shared
    // by every instance in it (packages take no instance overrides).
    // Single-threaded use only.
    static package_solver & packages();
    static void reset_packages();

private:
    static std::shared_ptr<package_solver> pkg_solver_;

private:
    static void resolve_interface_chain(
        work_order &work,
        const std::shared_ptr<data_store> &d_store,
        std::shared_ptr<hdl_ast_node> &examined_node,
        std::string &instance_name
    );
    static resolved_parameter resolve_instance_dependency(
        const qualified_identifier &dep,
        work_order &work,
        const std::shared_ptr<data_store> &d_store
    );
    static std::shared_ptr<hdl_type> resolve_dtype_reference(
        const qualified_identifier &ref,
        const std::map<qualified_identifier, std::shared_ptr<hdl_type>> &parent_type_ctx,
        const std::shared_ptr<hdl_resource_statement> &parent_resource,
        const std::shared_ptr<data_store> &d_store
    );
    static std::map<qualified_identifier, resolved_parameter> extract_struct_fields(
        const std::shared_ptr<HDL_parameter> &param,
        const resolved_parameter &res,
        const qualified_identifier &id,
        const std::map<qualified_identifier, resolved_parameter> &ctx
    );
    static std::map<qualified_identifier, resolved_parameter> extract_enum_values(
        const std::shared_ptr<HDL_parameter> &param
    );
    // Bare enum members reachable through a type tree (struct/union members
    // typed by an enum): package literals reference them unqualified, so
    // they must seed the solving context alongside the type's own members.
    static void collect_type_enum_values(
        const std::shared_ptr<hdl_type> &type,
        std::map<qualified_identifier, resolved_parameter> &fields
    );
};


#endif //ANANKE_PARAMETER_SOLVING_PASS_HPP