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


inline std::optional<std::map<qualified_identifier, resolved_parameter>> overlay_unambiguous_scope(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    std::map<std::string, unsigned> providers;
    for (const auto &entry : context) {
        const auto &key = entry.first;
        if (key.get_package_prefix().empty() || !key.get_instance().empty()) continue;
        auto &count = providers[key.get_name()];
        if (count < 2) ++count;
    }
    std::map<qualified_identifier, resolved_parameter> additions;
    for (const auto &entry : context) {
        const auto &key = entry.first;
        if (key.get_package_prefix().empty() || !key.get_instance().empty()) continue;
        auto prov = providers.find(key.get_name());
        if (prov == providers.end() || prov->second != 1) continue;
        qualified_identifier bare(key.get_name());
        if (context.contains(bare)) continue;
        additions.emplace(bare, entry.second);
    }
    if (additions.empty()) return std::nullopt;
    auto scoped = context;
    scoped.merge(additions);
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
