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


#ifndef ANANKE_PACKAGE_SOLVER_HPP
#define ANANKE_PACKAGE_SOLVER_HPP

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "data_model/HDL/parameters/common/qualified_identifier.hpp"
#include "data_model/HDL/parameters/common/resolved_parameter.hpp"

class data_store;
class hdl_package_statement;
class HDL_parameter;

// Solves package parameters once per flow and shares the results across all
// instances. Packages take no instance overrides, so each owner's pkg::
// entries are solved a single time (in dependency-closure context) no matter
// how many instances reference them. One instance lives in the data_store
// (flow scope); it carries no per-instance state.
class package_solver {
public:
    using export_map = std::map<qualified_identifier, resolved_parameter>;
    using cache_key = std::pair<std::string, const hdl_package_statement*>;

    std::map<qualified_identifier, resolved_parameter> retrieve(
        const std::vector<std::shared_ptr<HDL_parameter>> &node_parameters,
        const std::shared_ptr<data_store> &d_store,
        const std::vector<std::pair<std::string, std::shared_ptr<hdl_package_statement>>> &explicit_packages = {});

    void clear() { solved_.clear(); }
    [[nodiscard]] size_t cached_count() const { return solved_.size(); }

private:
    // Keyed by (package name, owner resource) since duplicate package names
    // may resolve to distinct owners per member.
    std::map<cache_key, export_map> solved_;
};

#endif //ANANKE_PACKAGE_SOLVER_HPP
