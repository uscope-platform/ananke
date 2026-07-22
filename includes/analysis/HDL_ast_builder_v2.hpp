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

#ifndef ANANKE_HDL_AST_BUILDER_V2_HPP
#define ANANKE_HDL_AST_BUILDER_V2_HPP


#include <vector>
#include <memory>

#include "data_model/Depfile/Depfile.hpp"
#include "data_model/HDL/hdl_ast_node.hpp"
#include "data_model/data_store.hpp"
#include "data_model/settings_store.hpp"
#include "analysis/passes/pass_manager.hpp"
#include "parameter_solver.hpp"


class HDL_ast_builder_v2 {
public:
    HDL_ast_builder_v2(const std::shared_ptr<settings_store> &s, const std::shared_ptr<data_store> &d, const Depfile& d_f);
    std::vector<std::shared_ptr<hdl_ast_node>> build_ast(const std::vector<std::string>& modules);
private:
    std::shared_ptr<hdl_ast_node> build_ast(const std::string& top_level_module);
    void process_quantifier(const std::shared_ptr<HDL_parameter> &quantifier, const std::map<qualified_identifier, resolved_parameter> &parameters);
    std::map<qualified_identifier, resolved_parameter> process_runtime_parameters(
        const std::map<qualified_identifier, resolved_parameter> &parameters,
        const std::shared_ptr<hdl_resource_statement> &res);
    std::shared_ptr<data_store> d_store;
    std::shared_ptr<settings_store> s_store;
    Depfile  dep_file;
};


#endif //ANANKE_HDL_AST_BUILDER_V2_HPP