//  Copyright  2026 University of Nottingham
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

#include "data_model/HDL/statement/hdl_class_statement.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_class_statement)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_class_statement)

parameter_deps_t hdl_class_statement::get_dependencies() const {
    return {};
}

bool hdl_class_statement::equals(const hdl_statement_base& other) const {
    return *this == static_cast<const hdl_class_statement&>(other);
}

std::string hdl_class_statement::print() const {
    return name;
}

bool hdl_class_statement::is_empty() {
    return name.empty();
}

bool operator==(const hdl_class_statement &lhs, const hdl_class_statement &rhs) {
    return lhs.name == rhs.name &&
           lhs.line_n == rhs.line_n &&
           lhs.language == rhs.language;
}

bool operator<(const hdl_class_statement &lhs, const hdl_class_statement &rhs) {
    return lhs.name < rhs.name;
}

void PrintTo(const hdl_class_statement &res, std::ostream *os) {
    *os << "\n----------------------------------------------------"
        << "\nHDL Class:\n  NAME: " + res.name
        << "\n  LINE: " + std::to_string(res.line_n)
        << "\n----------------------------------------------------";
}
