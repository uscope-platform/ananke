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

#include "data_model/HDL/parameters/Expression.hpp"
#include "data_model/HDL/parameters/Parameter_value_base.hpp"

#include <cereal/types/vector.hpp>

class Concatenation : public Parameter_value_base {
public:
    Concatenation() {
        components = {};
        type = concatenation;
    };
    Concatenation(std::initializer_list<std::shared_ptr<Parameter_value_base>> list)
        : components(list) {
        components = {};
        type = concatenation;
    }
    void add_component(const std::shared_ptr<Parameter_value_base> &expr) {components.push_back(expr);}

    Concatenation(const Concatenation &other);
    Concatenation(Concatenation &&other) noexcept;

    Concatenation clone() const;

    Concatenation &operator=(const Concatenation &other) {
        if (this != &other) {
            container_size = other.container_size;
            packing = other.packing;
            components.clear();
            components.reserve(other.components.size());
            for(auto &item: other.components) components.push_back(item->clone_ptr());
            type = other.type;
        }
        return *this;
    }

    Concatenation &operator=(Concatenation &&other) noexcept {
        if (this != &other) {
            container_size = other.container_size;
            packing = other.packing;
            components.clear();
            components.reserve(other.components.size());
            for(auto &item: other.components) components.push_back(item->clone_ptr());
            type = other.type;
        }
        return *this;
    }

    std::set<qualified_identifier> get_dependencies()const override;
    bool empty() const {return components.empty();}
    bool propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value) override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Parameter_value_base> &value) override;

    void propagate_function(const HDL_function_def &def) override;
    std::optional<resolved_parameter> evaluate() override;
    std::string print() const override;
    int64_t get_depth() override;

    friend bool operator==(const Concatenation &lhs, const Concatenation &rhs) {
        auto ret = true;
        if(lhs.components.size() != rhs.components.size()) return false;
        ret &= lhs.container_size == rhs.container_size;
        ret &= lhs.packing == rhs.packing;
        for(int i = 0; i < lhs.components.size(); i++) {
            ret &= *lhs.components[i] == *rhs.components[i];
        }
        return ret;
    }

    friend bool operator!=(const Concatenation &lhs, const Concatenation &rhs) {
        return !(lhs == rhs);
    }
    void set_container_sizes(const resolved_type &s) override;

    std::shared_ptr<Parameter_value_base> clone_ptr() const override {
        return std::make_shared<Concatenation>(*this);  // Copy constructor
    }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(components);
    }

private:

    bool packing = false;
    int64_t container_size = 0;

    std::vector<std::shared_ptr<Parameter_value_base>> components;

    bool isEqual(const Parameter_value_base& other) const override {
        const auto& rhs = static_cast<const Concatenation&>(other);

        return std::ranges::equal(
            components, rhs.components,
            [](const std::shared_ptr<Parameter_value_base>& a,
               const std::shared_ptr<Parameter_value_base>& b) {
                auto resp = *a == *b;
                return resp; // Triggers the polymorphic equality check
            }
        );
    }
};


#endif //ANANKE_CONCATENATION_HPP