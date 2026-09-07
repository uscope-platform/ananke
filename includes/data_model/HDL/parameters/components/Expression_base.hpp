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
#include <expected>
#include <optional>
#include "data_model/HDL/parameters/common/qualified_identifier.hpp"
#include "data_model/HDL/parameters/common/resolved_parameter.hpp"
#include "data_model/HDL/types/resolved_type.hpp"
#include "data_model/HDL/parameters/common/solver_errors.hpp"

class hdl_function_statement;


class Expression_base {
public:

    virtual ~Expression_base() = default;

    virtual parameter_deps_t get_dependencies()const {return {};}
    virtual void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Expression_base> &value){}
    virtual void propagate_function(const hdl_function_statement &def) {}
    virtual std::expected<resolved_parameter, solver_errors> evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) { (void)expected_type; return std::unexpected{missing_value};}
    virtual std::string print() const {return "";}



    virtual void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) = 0;

    virtual std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const {
        return std::nullopt;
    }

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
        int64_t current_shift = 0;

        for (size_t i = 0; i < components.size(); ++i) {
            int64_t size = sizes[i];
            if (size < 0) continue;

            // Build a size-bit all-ones mask as an hdl_integer: (1 << size) - 1.
            hdl_integer masked_component = components[i].truncate_to(size);

            packed_result = packed_result | (masked_component << hdl_integer(current_shift));

            current_shift += size;
        }

        return packed_result;
    }

protected:
    virtual bool isEqual(const Expression_base& other) const = 0;
};


#endif //ANANKE_PARAMETER_VALUE_BASE_HPP