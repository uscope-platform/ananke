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


#ifndef ANANKE_HDL_TYPE_BASE_HPP
#define ANANKE_HDL_TYPE_BASE_HPP

#include <map>
#include <optional>
#include <string>
#include <vector>

#include "data_model/HDL/types/resolved_type.hpp"
#include "data_model/HDL/parameters/common/resolved_parameter.hpp"
#include "data_model/HDL/parameters/common/qualified_identifier.hpp"

class parameter_deps_t;
struct resolved_type;

// Alias package-qualified values (`pkg::NAME`) under their bare names when
// unambiguous: exactly one package provides the name and no bare entry
// exists (ambient bare names — the current scope — always win). Lets
// dimensions written in a typedef's own package scope (e.g.
// `[NrMaxRules-1:0]`) resolve when the type is evaluated in a foreign
// context. Ambiguous or absent names stay missing (clean failure, never a
// guess). Instance-keyed (field-split) entries are never aliased. Pure.
inline std::map<qualified_identifier, resolved_parameter> with_unambiguous_scope(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    std::map<std::string, int> providers;
    for (const auto &[key, val] : context) {
        if (!key.get_package_prefix().empty() && key.get_instance().empty())
            providers[key.get_name()]++;
    }
    bool need_overlay = false;
    for (const auto &[name, count] : providers) {
        if (count != 1 || context.contains(qualified_identifier(name))) continue;
        need_overlay = true;
        break;
    }
    if (!need_overlay) return context;
    auto scoped = context;
    for (const auto &[key, val] : context) {
        if (key.get_package_prefix().empty() || !key.get_instance().empty()) continue;
        if (providers[key.get_name()] == 1) scoped[qualified_identifier(key.get_name())] = val;
    }
    return scoped;
}

class hdl_type {
public:
    virtual ~hdl_type() = default;
    template<typename T>
      T& as() { return static_cast<T&>(*this); }

    template<typename T>
    bool is() const {
        // dynamic_cast with pointers returns nullptr if the cast fails
        return dynamic_cast<const T*>(this) != nullptr;
    }

    virtual parameter_deps_t get_dependencies() = 0;
    [[nodiscard]] virtual bool is_scalar()const = 0;
     virtual std::optional<resolved_type> evaluate_type(const std::map<qualified_identifier, resolved_parameter> &context) = 0;
    [[nodiscard]] virtual std::string to_print() const = 0;
    [[nodiscard]] virtual bool is_equal(const hdl_type &other) const = 0;
};

#endif //ANANKE_HDL_TYPE_BASE_HPP
