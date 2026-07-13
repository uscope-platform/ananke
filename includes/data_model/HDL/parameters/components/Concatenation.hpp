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

#include "Parameter_value_base.hpp"
#include <cereal/types/vector.hpp>

class Concatenation : public Parameter_value_base {
public:
    Concatenation() {
        components = {};
    };
    void add_component(const std::shared_ptr<Parameter_value_base> &expr) {components.push_back(expr);}

    Concatenation(const Concatenation &other);
    Concatenation(Concatenation &&other) noexcept;


    void set_default_init() {default_initialization = true;}

    Concatenation &operator=(const Concatenation &other) {
        if (this != &other) {
            container_size = other.container_size;
            unpacked_dimension = other.unpacked_dimension;
            unpacked_ascending = other.unpacked_ascending;
            packing = other.packing;
            default_initialization = other.default_initialization;
            components = other.components;
        }
        return *this;
    }

    Concatenation &operator=(Concatenation &&other) noexcept {
        if (this != &other) {
            container_size = other.container_size;
            unpacked_dimension = other.unpacked_dimension;
            unpacked_ascending = other.unpacked_ascending;
            packing = other.packing;
            default_initialization = other.default_initialization;
            components = std::move(other.components);
        }
        return *this;
    }

    parameter_deps_t get_dependencies()const override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Parameter_value_base> &value) override;

    void propagate_function(const HDL_function_def &def) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;
    std::string print() const override;


    friend bool operator==(const Concatenation &lhs, const Concatenation &rhs) {
        auto ret = true;
        if(lhs.components.size() != rhs.components.size()) return false;
        ret &= lhs.container_size == rhs.container_size;
        ret &= lhs.packing == rhs.packing;
        ret &= lhs.default_initialization == rhs.default_initialization;
        ret &= lhs.unpacked_dimension == rhs.unpacked_dimension;
        ret &= lhs.unpacked_ascending == rhs.unpacked_ascending;
        for(int i = 0; i < lhs.components.size(); i++) {
            ret &= *lhs.components[i] == *rhs.components[i];
        }
        return ret;
    }

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(components, default_initialization);
    }

private:

    void process_struct_size(
        const std::vector<struct_member_resolved_type> &members,
        uint64_t size, const std::map<qualified_identifier, resolved_parameter> &context);

    bool packing = false;
    bool default_initialization = false;
    std::vector<uint64_t> unpacked_dimension  = {};
    std::vector<bool> unpacked_ascending = {};
    std::vector<struct_member_resolved_type> fields_sizes;
    int64_t container_size = 0;

    std::vector<std::shared_ptr<Parameter_value_base>> components;

    bool isEqual(const Parameter_value_base& other) const override {

        auto ret = true;
        const auto& rhs = static_cast<const Concatenation&>(other);

        ret &= std::ranges::equal(
            components, rhs.components,
            [](const std::shared_ptr<Parameter_value_base>& a,
               const std::shared_ptr<Parameter_value_base>& b) {
                auto resp = *a == *b;
                return resp; // Triggers the polymorphic equality check
            }
        );

        if(components.size() != rhs.components.size()) return false;
        ret &= container_size == rhs.container_size;
        ret &= packing == rhs.packing;
        ret &= default_initialization == rhs.default_initialization;
        ret &= unpacked_dimension == rhs.unpacked_dimension;
        ret &= unpacked_ascending == rhs.unpacked_ascending;

        return ret;
    }
};


#endif //ANANKE_CONCATENATION_HPP