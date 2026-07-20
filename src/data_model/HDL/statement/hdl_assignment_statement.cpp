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


#include "data_model/HDL/statement/hdl_assignment_statement.hpp"

parameter_deps_t hdl_assignment_statement::get_dependencies() const {
    parameter_deps_t res;

    return res;
}

std::unique_ptr<hdl_statement_base> hdl_assignment_statement::clone() const {
    return std::make_unique<hdl_assignment_statement>();
}

bool hdl_assignment_statement::equals(const hdl_statement_base &other) const {
    return false;
}

std::string hdl_assignment_statement::print() const {
    return "";
}
