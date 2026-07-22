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

#ifndef ANANKE_EXPRESSION_BASE_HPP
#define ANANKE_EXPRESSION_BASE_HPP

#include <string>
#include <vector>
#include "data_model/HDL/parameters/common/qualified_identifier.hpp"
#include "data_model/HDL/parameters/common/resolved_parameter.hpp"
#include "data_model/HDL/types/resolved_type.hpp"

class hdl_function_statement;


class Expression_base {
public:

    virtual ~Expression_base() = default;

    virtual parameter_deps_t get_dependencies()const {return {};}
    virtual void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Expression_base> &value){}
    virtual void propagate_function(const hdl_function_statement &def) {}
    virtual std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {return std::nullopt;}
    virtual std::string print() const {return "";}



    virtual void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) = 0;

    template<typename T>
        T& as() { return static_cast<T&>(*this); }

    template<typename T>
        bool is() const {
        // dynamic_cast with pointers returns nullptr if the cast fails
        return dynamic_cast<const T*>(this) != nullptr;
    }

    bool operator==(const Expression_base& other) const {
        return typeid(*this) == typeid(other) && isEqual(other);
    }


    hdl_integer pack_values(const std::vector<hdl_integer> &components, const std::vector<int64_t> &sizes) {
        hdl_integer packed_result = 0;
        hdl_integer current_shift = 0;

        for (size_t i = 0; i < components.size(); ++i) {
            int64_t size = sizes[i];

            hdl_integer mask = (size >= 64) ? ~0ULL : (1ULL << size) - 1;
            auto masked_component = components[i] & mask;

            packed_result |= masked_component << current_shift;

            current_shift += size;
        }

        return static_cast<hdl_integer>(packed_result);
    }

protected:
    virtual bool isEqual(const Expression_base& other) const = 0;
};


#endif //ANANKE_PARAMETER_VALUE_BASE_HPP