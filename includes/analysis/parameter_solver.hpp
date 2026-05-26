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
#include "data_model/HDL/parameters/Expression_evaluator.hpp"


class data_store;

struct work_order {
    std::shared_ptr<HDL_instance_AST> node;
    std::map<qualified_identifier, resolved_parameter> parent_parameters;
    std::string path;
};

class parameter_solver {
public:
    static std::map<qualified_identifier, resolved_parameter> process_parameters(
        const Parameters_map &map,
        const std::string_view &parent_module,
        const std::map<qualified_identifier, resolved_parameter> &default_parameters,
        const std::map<qualified_identifier, resolved_parameter> &context
    );
    static void update_parameters_map(
        const std::map<qualified_identifier, resolved_parameter> &parameters,
        const std::shared_ptr<HDL_instance_AST>& node,
        const std::shared_ptr<data_store> &d_store
        );

    static std::map<qualified_identifier, resolved_parameter> override_parameters(work_order &work, const std::shared_ptr<data_store> &d_store);
    static std::map<qualified_identifier, resolved_parameter> retrieve_package_parameters(const Parameters_map &node_parameters, const std::shared_ptr<data_store> &d_store);

    static std::map<qualified_identifier, resolved_parameter> solve_complex_overrides(
            work_order &work,
            const std::shared_ptr<data_store> &d_store,
            const std::map<qualified_identifier, resolved_parameter> &node_defaults
        );


    static std::map<qualified_identifier, std::set<qualified_identifier>>get_dependency_map(const Parameters_map &map);

    static std::map<qualified_identifier, resolved_parameter> specialize_runtime_parameters(
        const std::map<qualified_identifier, resolved_parameter> &solved_parameters,
        Parameters_map &node_parameters,const std::string_view &parent_module
    );

    static std::string get_full_path(const std::shared_ptr<HDL_instance_AST> &node);
};


#endif //ANANKE_PARAMETER_SOLVING_PASS_HPP