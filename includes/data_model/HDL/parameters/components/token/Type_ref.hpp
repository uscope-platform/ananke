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

#ifndef ANANKE_TYPE_REF_HPP
#define ANANKE_TYPE_REF_HPP

#include "../Expression_base.hpp"

class Type_ref: public Expression_base {
public:
    Type_ref() = default;

    explicit Type_ref(const qualified_identifier &t) : target(t) {}

    parameter_deps_t get_dependencies() const override {
        parameter_deps_t deps;
        deps.data.insert(target);
        return deps;
    }

    std::expected<resolved_parameter, solver_errors> evaluate(
        const std::map<qualified_identifier, resolved_parameter> &, const std::optional<resolved_type> & = std::nullopt) override {
        return std::unexpected{missing_value};
    }

    std::string print() const override {
        return target.print();
    }

    qualified_identifier get_target() const { return target; }

    friend bool operator==(const Type_ref &lhs, const Type_ref &rhs) {
        return lhs.target == rhs.target;
    }

    template<class Archive>
    void serialize(Archive & ar) {
        ar(target);
    }

private:
    bool isEqual(const Expression_base& other) const override {
        return target == static_cast<const Type_ref&>(other).target;
    }

    qualified_identifier target;
};

#endif //ANANKE_TYPE_REF_HPP
