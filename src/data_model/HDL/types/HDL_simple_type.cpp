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


#include "data_model/HDL/types/HDL_simple_type.hpp"

void HDL_simple_type::add_dimension(const dimension_t &d) {
    scalar = false;
    if(d.packed){
        packed_dimensions.push_back(d);
    } else{
        unpacked_dimensions.push_back(d);
    }
}

void HDL_simple_type::set_packed_dimensions(const std::vector<dimension_t> &d) {
    packed_dimensions.insert(packed_dimensions.end(), d.begin(), d.end());
}

void HDL_simple_type::set_unpacked_dimensions(const std::vector<dimension_t> &d) {
    unpacked_dimensions.insert(unpacked_dimensions.end(), d.begin(), d.end());
}

std::set<qualified_identifier> HDL_simple_type::get_dependencies() {
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

std::optional<resolved_type> HDL_simple_type::evaluate_type(const std::map<qualified_identifier, resolved_parameter> &context) {
    resolved_type result;
    for (auto &dim: unpacked_dimensions) {
        auto f_b = dim.first_bound.evaluate(context);
        auto s_b = dim.second_bound.evaluate(context);
        if (!(f_b.has_value() && s_b.has_value())) return std::nullopt;
        auto diff = std::abs(f_b.value().get_integer() - s_b.value().get_integer())+1;
        result.unpacked_sizes.push_back(diff.get_value());
    }
    for (auto &dim: packed_dimensions) {
        auto f_b = dim.first_bound.evaluate(context);
        auto s_b = dim.second_bound.evaluate(context);
        if (!(f_b.has_value() && s_b.has_value())) return std::nullopt;
        auto diff = std::abs(f_b.value().get_integer() - s_b.value().get_integer())+1;
        result.packed_sizes.push_back(diff.get_value());
    }

    return result;
}
