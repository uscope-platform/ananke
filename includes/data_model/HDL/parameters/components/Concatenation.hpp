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

#ifndef ANANKE_CONCATENATION_HPP
#define ANANKE_CONCATENATION_HPP

#include "Expression_base.hpp"
#include <cereal/types/vector.hpp>

class Concatenation : public Expression_base {
public:
    Concatenation() {
        components = {};
    };
    void add_component(const std::shared_ptr<Expression_base> &expr) {components.push_back(expr);}
    std::vector<std::shared_ptr<Expression_base>> get_components() const { return components; }

    void add_component_key(const std::string &key) {component_keys.push_back(key);}
    std::vector<std::string> get_component_keys() const { return component_keys; }
    void clear_component_keys() {component_keys.clear();}

    Concatenation(const Concatenation &other);
    Concatenation(Concatenation &&other) noexcept;

    void set_default_init() {default_initialization = true;}

    bool reorder_by_member_names(const std::vector<std::string> &member_names);

    Concatenation &operator=(const Concatenation &other) {
        if (this != &other) {
            default_initialization = other.default_initialization;
            components = other.components;
            component_keys = other.component_keys;
        }
        return *this;
    }

    Concatenation &operator=(Concatenation &&other) noexcept {
        if (this != &other) {
            default_initialization = other.default_initialization;
            components = std::move(other.components);
            component_keys = std::move(other.component_keys);
        }
        return *this;
    }

    parameter_deps_t get_dependencies()const override;

    void propagate_function(const hdl_function_def_ptr &def) override;
    std::expected<resolved_parameter, solver_errors> evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) override;
    std::string print() const override;

    friend bool operator==(const Concatenation &lhs, const Concatenation &rhs) {
        auto ret = true;
        if(lhs.components.size() != rhs.components.size()) return false;
        ret &= lhs.default_initialization == rhs.default_initialization;
        ret &= lhs.component_keys == rhs.component_keys;
        for(int i = 0; i < lhs.components.size(); i++) {
            ret &= *lhs.components[i] == *rhs.components[i];
        }
        return ret;
    }

    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(components, default_initialization, component_keys);
    }

private:

    bool default_initialization = false;

    std::vector<std::shared_ptr<Expression_base>> components;
    std::vector<std::string> component_keys;

    bool isEqual(const Expression_base& other) const override {

        auto ret = true;
        const auto& rhs = static_cast<const Concatenation&>(other);

        ret &= std::ranges::equal(
            components, rhs.components,
            [](const std::shared_ptr<Expression_base>& a,
               const std::shared_ptr<Expression_base>& b) {
                auto resp = *a == *b;
                return resp; // Triggers the polymorphic equality check
            }
        );

        if(components.size() != rhs.components.size()) return false;
        ret &= default_initialization == rhs.default_initialization;
        ret &= component_keys == rhs.component_keys;

        return ret;
    }
};

#endif //ANANKE_CONCATENATION_HPP