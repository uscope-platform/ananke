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

#include "data_model/HDL/statement/hdl_import_stmt.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(hdl_import_stmt)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_import_stmt)

parameter_deps_t hdl_import_stmt::get_dependencies() const {
    return {};
}

bool hdl_import_stmt::equals(const hdl_statement_base &other) const {
    const auto &rhs = static_cast<const hdl_import_stmt &>(other);
    return library == rhs.library
        && package == rhs.package
        && item == rhs.item
        && wildcard == rhs.wildcard;
}

std::string hdl_import_stmt::print() const {
    std::string result;
    if (!library.empty()) result += library + ".";
    result += package;
    if (wildcard) result += ".all";
    else if (!item.empty()) result += "." + item;
    return result;
}

void PrintTo(const hdl_import_stmt &s, std::ostream *os) {
    *os << s.print();
}