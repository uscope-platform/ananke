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

#ifndef ANANKE_PARAMETER_VALUE_BASE_HPP
#define ANANKE_PARAMETER_VALUE_BASE_HPP

#include <set>
#include <string>
#include <vector>
#include <bitset>
#include "data_model/HDL/parameters/qualified_identifier.hpp"
#include "data_model/mdarray.hpp"
#include "data_model/HDL/parameters/resolved_type.hpp"

class HDL_function_def;

class Parameter_value_base {
public:
    enum param_value_type {
        concatenation,
        expression,
        replication,
        function,
        ternary,
        cast
    };

    virtual ~Parameter_value_base() = default;

    virtual std::set<qualified_identifier> get_dependencies()const {return {};}
    virtual bool propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value) {return true;}
    virtual void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Parameter_value_base> &value){}
    virtual void propagate_function(const HDL_function_def &def) {}
    virtual std::optional<resolved_parameter> evaluate() {return std::nullopt;}
    virtual std::string print() const {return "";}
    virtual int64_t get_size() {return 0;}


    [[nodiscard]] bool is_expression() const {return type == expression;}
    [[nodiscard]] bool is_replication() const {return type == replication;}
    [[nodiscard]] bool is_concatenation() const {return type == concatenation;}
    [[nodiscard]] bool is_function() const {return type == function;}
    [[nodiscard]] bool is_ternary() const {return type == ternary;}
    [[nodiscard]] bool is_cast() const {return type == cast;}

    virtual void set_container_sizes(const resolved_type &s) = 0;
    virtual std::shared_ptr<Parameter_value_base> clone_ptr() const = 0;

    template<typename T>
        T& as() { return static_cast<T&>(*this); }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(type);
    }
    // Public equality operator using the enum discriminator
    bool operator==(const Parameter_value_base& other) const {
        // Use your existing member variable instead of RTTI
        if (this->type != other.type) return false;

        // Delegate to the virtual implementation
        return isEqual(other);
    }


    int64_t pack_values(const std::vector<int64_t> &components, const std::vector<int64_t> &sizes) {
        uint64_t packed_result = 0;
        int current_shift = 0;

        for (size_t i = 0; i < components.size(); ++i) {
            int64_t size = sizes[i];

            uint64_t mask = (size >= 64) ? ~0ULL : (1ULL << size) - 1;
            uint64_t masked_component = components[i] & mask;

            packed_result |= (masked_component << current_shift);

            current_shift += size;
        }

        return static_cast<int64_t>(packed_result);
    }

protected:
    param_value_type type = expression;
    virtual bool isEqual(const Parameter_value_base& other) const = 0;
};


#endif //ANANKE_PARAMETER_VALUE_BASE_HPP