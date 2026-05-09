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
#include "data_model/HDL/parameters/qualified_identifier.hpp"
#include "data_model/mdarray.hpp"

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
    virtual std::optional<resolved_parameter> evaluate(bool pack_result) {return std::nullopt;}
    virtual std::string print() const {return "";}
    virtual int64_t get_size() {return 0;}

    virtual int64_t get_depth() {return 0;}

    [[nodiscard]] bool empty() const {return false;}

    [[nodiscard]] bool is_expression() const {return type == expression;}
    [[nodiscard]] bool is_replication() const {return type == replication;}
    [[nodiscard]] bool is_concatenation() const {return type == concatenation;}
    [[nodiscard]] bool is_function() const {return type == function;}
    [[nodiscard]] bool is_ternary() const {return type == ternary;}
    [[nodiscard]] bool is_cast() const {return type == cast;}


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

protected:
    param_value_type type = expression;
    virtual bool isEqual(const Parameter_value_base& other) const = 0;
};


#endif //ANANKE_PARAMETER_VALUE_BASE_HPP