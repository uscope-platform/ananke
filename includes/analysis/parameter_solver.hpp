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
#include "data_model/HDL/HDL_Resource.hpp"
#include "data_model/HDL/parameters/HDL_function_def.hpp"
#include "analysis/topological_sorter.hpp"
#include "data_model/data_store.hpp"


class data_store;

struct work_order {
    std::shared_ptr<hdl_ast_node> node;
    std::map<qualified_identifier, resolved_parameter> parent_parameters;
    std::string path;
    std::unordered_map<std::string, std::string> interfaces_map;
};

class parameter_solver {
public:
    static std::map<qualified_identifier, resolved_parameter> process_parameters(
        const Parameters_map &map,
        const std::map<qualified_identifier, resolved_parameter> &context
    );
    static void update_parameters_map(
        const std::map<qualified_identifier, resolved_parameter> &parameters,
        const std::shared_ptr<hdl_ast_node>& node,
        const std::shared_ptr<data_store> &d_store
        );

    static std::map<qualified_identifier, resolved_parameter> override_parameters(work_order &work, const std::shared_ptr<data_store> &d_store);
    static void propagate_functions(HDL_Resource &resource, const std::shared_ptr<data_store> &d_store);
    static void propagate_types(HDL_Resource &resource, const std::shared_ptr<data_store> &d_store);
    static std::map<qualified_identifier, resolved_parameter> retrieve_package_parameters(const Parameters_map &node_parameters, const std::shared_ptr<data_store> &d_store);

    static std::map<qualified_identifier, resolved_parameter> solve_complex_overrides(
            work_order &work,
            const std::shared_ptr<data_store> &d_store,
            const std::map<qualified_identifier, resolved_parameter> &node_defaults
        );

    static std::string get_full_path(const std::shared_ptr<hdl_ast_node> &node);

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
    static std::map<qualified_identifier, resolved_parameter> extract_struct_fields(
        const std::shared_ptr<HDL_parameter> &param,
        const resolved_parameter &res,
        const qualified_identifier &id,
        const std::map<qualified_identifier, resolved_parameter> &ctx
    );
};


#endif //ANANKE_PARAMETER_SOLVING_PASS_HPP