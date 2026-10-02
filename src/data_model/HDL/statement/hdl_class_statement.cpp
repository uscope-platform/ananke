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

hdl_class_statement::hdl_class_statement() {
    name = "";
}

hdl_class_statement::hdl_class_statement(const hdl_class_statement &c) {
    name = c.name;
    line_n = c.line_n;
    base_class = c.base_class;
    typedefs = c.typedefs;
    properties = c.properties;
    methods = c.methods;
    nested_classes = c.nested_classes;
}

parameter_deps_t hdl_class_statement::get_dependencies() const {
    parameter_deps_t deps;
    for (const auto &p : properties)
        if (p) deps.merge(p->get_dependencies());
    for (const auto &m : methods)
        if (m) deps.merge(m->get_dependencies());
    for (const auto &n : nested_classes)
        if (n) deps.merge(n->get_dependencies());
    for (const auto &[_, type] : typedefs)
        deps.merge(type->get_dependencies());
    if (!base_class.empty()) {
        // The base class is a type dependency: `pkg::base` keeps its package
        // qualifier, a bare name stays unqualified for get_class lookups.
        if (auto sep = base_class.find("::"); sep != std::string::npos) {
            qualified_identifier qi(base_class.substr(sep + 2));
            qi.set_package_prefix({base_class.substr(0, sep)});
            deps.types.insert(qi);
        } else {
            deps.types.insert(qualified_identifier(base_class));
        }
    }
    return deps;
}

bool hdl_class_statement::equals(const hdl_statement_base& other) const {
    return *this == static_cast<const hdl_class_statement&>(other);
}

std::string hdl_class_statement::print() const {
    return name;
}

bool hdl_class_statement::is_empty() {
    bool ret = true;

    ret &= name.empty();
    ret &= base_class.empty();
    ret &= properties.empty();
    ret &= methods.empty();
    ret &= nested_classes.empty();
    ret &= typedefs.empty();

    return ret;
}

std::unordered_map<std::string, hdl_function_statement> hdl_class_statement::get_functions() {
    std::unordered_map<std::string, hdl_function_statement> result;
    for (auto &m : methods) {
        if (m) result[m->get_name()] = *m;
    }
    return result;
}

std::optional<hdl_function_statement> hdl_class_statement::get_function(const std::string &fname) {
    if (auto f = get_function_shared(fname)) return *f;
    return std::nullopt;
}

std::shared_ptr<const hdl_function_statement> hdl_class_statement::get_function_shared(const std::string &fname) const {
    for (const auto &m : methods) {
        if (m && m->get_name() == fname) return m;
    }
    return nullptr;
}

namespace {

template<class T>
bool members_equal(const std::vector<std::shared_ptr<T>> &lhs,
                   const std::vector<std::shared_ptr<T>> &rhs) {
    if (lhs.size() != rhs.size()) return false;
    for (size_t i = 0; i < lhs.size(); i++) {
        const auto &l = lhs[i];
        const auto &r = rhs[i];
        if (!((!l && !r) || (l && r && *l == *r))) return false;
    }
    return true;
}

} // namespace

bool operator==(const hdl_class_statement &lhs, const hdl_class_statement &rhs) {
    bool ret = true;

    ret &= lhs.name == rhs.name;
    ret &= lhs.line_n == rhs.line_n;
    ret &= lhs.base_class == rhs.base_class;
    ret &= lhs.typedefs == rhs.typedefs;
    ret &= members_equal(lhs.properties, rhs.properties);
    ret &= members_equal(lhs.methods, rhs.methods);
    ret &= members_equal(lhs.nested_classes, rhs.nested_classes);

    return ret;
}

bool operator<(const hdl_class_statement &lhs, const hdl_class_statement &rhs) {
    return lhs.name < rhs.name;
}

void PrintTo(const hdl_class_statement &res, std::ostream *os) {
    std::string result = "\n----------------------------------------------------";
    result += "\nHDL Class:\n  NAME: " + res.name;
    result += "\n  LINE: "  + std::to_string(res.line_n);
    if (!res.base_class.empty()) result += "\n  BASE: " + res.base_class;
    result += "\n  PROPERTIES: \n";
    for (const auto& item : res.properties) {
        if (item) result += item->to_string() + "\n";
    }
    result += "\n----------------------------------------------------";

    *os << result;
}
