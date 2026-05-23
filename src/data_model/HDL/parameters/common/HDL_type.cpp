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


#include "../../../../../includes/data_model/HDL/parameters/common/HDL_type.hpp"

void HDL_type::add_dimension(const dimension_t &d) {
    scalar = false;
    if(d.packed){
        packed_dimensions.push_back(d);
    } else{
        unpacked_dimensions.push_back(d);
    }
}

void HDL_type::set_packed_dimensions(const std::vector<dimension_t> &d) {
    packed_dimensions.insert(packed_dimensions.end(), d.begin(), d.end());
}

void HDL_type::set_declared_type(const std::string &type) {
    if (type == "implicit") {
        is_implicit = true;
        packed_dimensions.push_back({
            Expression({Expression_component("31", Expression_component::number)}),
            Expression({Expression_component("0", Expression_component::number)}),
            true
        });
    }
    if (type == "shortint") {
        is_signed = true;
        packed_dimensions.push_back({
            Expression({Expression_component("15", Expression_component::number)}),
            Expression({Expression_component("0", Expression_component::number)}),
            true
        });
    }
    if (type == "int" || type == "integer") {
        is_signed = true;
        packed_dimensions.push_back({
            Expression({Expression_component("31", Expression_component::number)}),
            Expression({Expression_component("0", Expression_component::number)}),
            true
        });
    }
    if (type == "longint") {
        is_signed = true;
        packed_dimensions.push_back({
            Expression({Expression_component("63", Expression_component::number)}),
            Expression({Expression_component("0", Expression_component::number)}),
            true
        });
    }
    if (type == "time") {
        is_signed = false;
        packed_dimensions.push_back({
            Expression({Expression_component("63", Expression_component::number)}),
            Expression({Expression_component("0", Expression_component::number)}),
            true
        });
    }

    if (type == "real" || type == "realtime") {
        is_real = true;
        packed_dimensions.push_back({
            Expression({Expression_component("63", Expression_component::number)}),
            Expression({Expression_component("0", Expression_component::number)}),
            true
        });
    }
    if (type == "shortreal") {
        is_real = true;
        packed_dimensions.push_back({
            Expression({Expression_component("31", Expression_component::number)}),
            Expression({Expression_component("0", Expression_component::number)}),
            true
        });
    }

}

bool HDL_type::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &constant_value) {
    bool retval = true;
    for (auto &dim: packed_dimensions) {
        retval &= dim.first_bound.propagate_constant(constant_id, constant_value);
        retval &= dim.second_bound.propagate_constant(constant_id, constant_value);
    }

    for (auto &dim: unpacked_dimensions) {
        retval &= dim.first_bound.propagate_constant(constant_id, constant_value);
        retval &= dim.second_bound.propagate_constant(constant_id, constant_value);
    }
    return retval;
}

std::set<qualified_identifier> HDL_type::get_dependencies() {
    std::set<qualified_identifier> result;
    for (auto &dim:unpacked_dimensions) {
        auto deps = dim.first_bound.get_dependencies();
        result.insert(deps.begin(), deps.end());
        deps = dim.second_bound.get_dependencies();
        result.insert(deps.begin(), deps.end());
    }

    for (auto &dim:packed_dimensions) {
        auto deps = dim.first_bound.get_dependencies();
        result.insert(deps.begin(), deps.end());
        deps = dim.second_bound.get_dependencies();
        result.insert(deps.begin(), deps.end());
    }
    return result;
}

std::optional<resolved_type> HDL_type::evaluate_type() {
    resolved_type result;
    for (auto &dim: unpacked_dimensions) {
        auto f_b = dim.first_bound.evaluate();
        auto s_b = dim.second_bound.evaluate();
        if (!(f_b.has_value() && s_b.has_value())) return std::nullopt;
        auto diff = std::abs(f_b.value().get_integer() - s_b.value().get_integer())+1;
        result.unpacked_sizes.push_back(diff);
    }
    for (auto &dim: packed_dimensions) {
        auto f_b = dim.first_bound.evaluate();
        auto s_b = dim.second_bound.evaluate();
        if (!(f_b.has_value() && s_b.has_value())) return std::nullopt;
        auto diff = std::abs(f_b.value().get_integer() - s_b.value().get_integer())+1;
        result.packed_sizes.push_back(diff);
    }

    return result;
}
