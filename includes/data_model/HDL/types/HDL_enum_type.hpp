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


#ifndef ANANKE_HDL_ENUM_TYPE_HPP
#define ANANKE_HDL_ENUM_TYPE_HPP

#include <string>
#include <sstream>
#include <cereal/types/optional.hpp>
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/parameters/common/dimension.hpp"

struct enum_member {
    std::string name;
    std::optional<uint64_t> value;

    template<class Archive>
    void serialize(Archive & ar) {
        ar(name, value);
    }

    friend bool operator==(const enum_member &lhs, const enum_member &rhs) {
        return lhs.name == rhs.name && lhs.value == rhs.value;
    }
};

class HDL_enum_type : public hdl_type {
public:
    virtual ~HDL_enum_type() = default;

    std::shared_ptr<hdl_type> base_type;
    std::vector<enum_member> members;

    [[nodiscard]] std::optional<resolved_type> evaluate_type(
        const std::map<qualified_identifier, resolved_parameter> &context) override;

    [[nodiscard]] bool is_scalar() const override { return unpacked_dimensions.empty(); }

    void set_unpacked_dimensions(const std::vector<dimension_t> &d) { unpacked_dimensions = d; }
    [[nodiscard]] std::vector<dimension_t> get_unpacked_dimensions() const { return unpacked_dimensions; }

    parameter_deps_t get_dependencies() override;

    [[nodiscard]] std::string to_print() const override;

    [[nodiscard]] bool is_equal(const hdl_type &other) const override {
        if (auto *o = dynamic_cast<const HDL_enum_type *>(&other)) {
            return *this == *o;
        }
        return false;
    }

    friend bool operator==(const HDL_enum_type &lhs, const HDL_enum_type &rhs) {
        if (lhs.members != rhs.members) return false;
        if (lhs.unpacked_dimensions.size() != rhs.unpacked_dimensions.size()) return false;
        for (size_t i = 0; i < lhs.unpacked_dimensions.size(); ++i) {
            if (!(lhs.unpacked_dimensions[i] == rhs.unpacked_dimensions[i])) return false;
        }
        return true;
    }

    template<class Archive>
    void serialize(Archive & ar) {
        ar(members, base_type, unpacked_dimensions);
    }

private:
    std::vector<dimension_t> unpacked_dimensions;
};

#endif //ANANKE_HDL_ENUM_TYPE_HPP
