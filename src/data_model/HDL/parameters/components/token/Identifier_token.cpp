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

#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/types/hdl_type.hpp"

#include <spdlog/spdlog.h>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Identifier_token)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, Identifier_token)

Identifier_token::Identifier_token(const Identifier_token &c) {
    id = c.id;
    array_index = c.array_index;
    type_placeholder = c.type_placeholder;
}

Identifier_token::Identifier_token(const qualified_identifier &q_i) {
    id =q_i;
}

parameter_deps_t Identifier_token::get_dependencies() const {
    parameter_deps_t result;
    if (!type_placeholder) result.data.insert(id);
    for (const auto &idx : array_index) {
        result.merge(idx->get_dependencies());
    }
    return result;
}

void Identifier_token::propagate_function(const hdl_function_def_ptr &def) {
    for (auto &component : array_index) {
        component->propagate_function(def);
    }
}

std::expected<resolved_parameter, solver_errors> Identifier_token::evaluate(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) {
    auto it = context.find(id);
    if (it != context.end()) {
        const auto &resolved = it->second;
        if (resolved.is_undefined()) {
            spdlog::warn("Parameter {} is undefined, using 0 as a default", id.print());
        }
        if (array_index.empty()) return resolved;

        std::vector<int64_t> indices;
        for (const auto &idx_expr : array_index) {
            auto idx_val = idx_expr->evaluate(context);
            if (!idx_val.has_value()) return std::unexpected{missing_value};
            if (!idx_val.value().is_integer()) return std::unexpected{wrong_type};
            indices.push_back(idx_val.value().get_integer().get_value());
        }

        if (resolved.is_int_array()) {
            auto values = resolved.get_int_array();
            auto array_val = values.get_value(indices);
            if (array_val.has_value()) return array_val.value();
            return static_cast<hdl_integer>(0);
        } else if (resolved.is_string_array()) {
            auto values = resolved.get_string_array();
            auto array_val = values.get_value(indices);
            if (array_val.has_value()) return resolved_parameter(array_val.value());
            return resolved_parameter("");
        } else if (resolved.is_integer() && !indices.empty()) {
            int64_t bit = indices[0];
            if (bit < 0 || bit >= hdl_integer::MAX_BIT_WIDTH) return static_cast<hdl_integer>(0);
            auto shifted = resolved.get_integer() >> hdl_integer(bit);
            auto b = shifted & hdl_integer(1);
            return b;
        }
         return std::unexpected{wrong_type};
    }
    return std::unexpected{missing_value};
}

std::string Identifier_token::print() const {

    if(!array_index.empty()){
        return id.print() + print_index(array_index);
    } else {
        return id.print();
    }
}

bool operator==(const Identifier_token &lhs, const Identifier_token &rhs) {
    bool ret_val = true;
    ret_val &= lhs.id == rhs.id;
    ret_val &= lhs.array_index == rhs.array_index;
    return ret_val;
}

std::optional<resolved_type> Identifier_token::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) const {
    if (expression_type) {
        return expression_type->evaluate_type(context);
    }
    return std::nullopt;
}

std::string Identifier_token::print_index(const std::vector<std::shared_ptr<Expression_base>> &index) const {
    std::string ret_val;
    for(const auto &item:index){
        ret_val += "[" + item->print() + "]";
    }
    return ret_val;
}

bool Identifier_token::isEqual(const Expression_base &other) const {
    const auto& rhs = static_cast<const Identifier_token&>(other);
    bool res = id == rhs.id;
    res &= type_placeholder == rhs.type_placeholder;
    if (array_index.size() != rhs.array_index.size()) return false;
    for (size_t i = 0; i < array_index.size(); i++) {
        res &= *array_index[i] == *rhs.array_index[i];
    }
    return res;
}
