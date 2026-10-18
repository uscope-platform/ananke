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

#include "data_model/HDL/statement/hdl_signal_declaration.hpp"

#include <sstream>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>

#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"

CEREAL_REGISTER_TYPE(hdl_signal_declaration)
CEREAL_REGISTER_POLYMORPHIC_RELATION(hdl_statement_base, hdl_signal_declaration)

parameter_deps_t hdl_signal_declaration::get_dependencies() const {
    parameter_deps_t deps;
    if (type) deps.merge(type->get_dependencies());
    if (initializer) deps.merge(initializer->get_dependencies());
    return deps;
}

bool hdl_signal_declaration::equals(const hdl_statement_base& other) const {
    const auto& rhs = static_cast<const hdl_signal_declaration&>(other);
    if (name != rhs.name || storage != rhs.storage || net_type != rhs.net_type) return false;
    if (type && rhs.type) {
        if (!type->is_equal(*rhs.type)) return false;
    } else if (type != rhs.type) {
        return false;
    }
    return (!initializer && !rhs.initializer) ||
           (initializer && rhs.initializer && *initializer == *rhs.initializer);
}

static const char *net_type_keyword(net_type_t t) {
    switch (t) {
        case net_type_t::wire:    return "wire";
        case net_type_t::tri:     return "tri";
        case net_type_t::wand:    return "wand";
        case net_type_t::wor:     return "wor";
        case net_type_t::triand:  return "triand";
        case net_type_t::trior:   return "trior";
        case net_type_t::tri0:    return "tri0";
        case net_type_t::tri1:    return "tri1";
        case net_type_t::trireg:  return "trireg";
        case net_type_t::uwire:   return "uwire";
        case net_type_t::supply0: return "supply0";
        case net_type_t::supply1: return "supply1";
        case net_type_t::none:    break;
    }
    return "";
}

std::string hdl_signal_declaration::print() const {
    std::ostringstream oss;
    switch (storage) {
        case signal_storage_t::net:         oss << net_type_keyword(net_type); break;
        case signal_storage_t::variable:    oss << "var"; break;
        case signal_storage_t::vhdl_signal: oss << "signal"; break;
    }
    if (type) oss << " " << type->to_print();
    oss << " " << name;
    if (initializer) oss << " = " << initializer->print();
    return oss.str();
}
