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

#include "data_model/HDL/parameters/components/Replication.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Replication)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, Replication)


Replication::Replication(const Replication &other) {
    repetition_size = other.repetition_size;
    repeated_item = other.repeated_item;
}

Replication::Replication(Replication &&other) noexcept {
    repetition_size = other.repetition_size;
    repeated_item = other.repeated_item;
}

Replication & Replication::operator=(const Replication &other) {
    if (this != &other) {
        repetition_size = other.repetition_size;
        repeated_item = other.repeated_item;
    }
    return *this;
}

Replication & Replication::operator=(Replication &&other) noexcept {
    if (this != &other) {
        repetition_size = other.repetition_size;
        repeated_item = other.repeated_item;
    }
    return *this;
}

void Replication::set_size(const std::shared_ptr<Expression_base> &size) { repetition_size = size;}

parameter_deps_t Replication::get_dependencies()const {
    parameter_deps_t result;
    result.merge(repetition_size->get_dependencies());
    result.merge(repeated_item->get_dependencies());
    return result;
}


void Replication::propagate_expression(const qualified_identifier &constant_id,
    const std::shared_ptr<Expression_base> &value) {
    repetition_size->propagate_expression(constant_id, value);
    repeated_item->propagate_expression(constant_id, value);
}

void Replication::propagate_function(const hdl_function_statement &def) {
    repetition_size->propagate_function(def);
    repeated_item->propagate_function(def);
}

std::optional<resolved_parameter> Replication::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    mdarray<hdl_integer> result;
    auto raw_size = repetition_size->evaluate(context);
    if (!raw_size.has_value()) return false;
    if (!raw_size.value().is_integer()) return false;
    auto size = raw_size.value().get_integer().get_value();
    mdarray<hdl_integer>::md_1d_array repeated_value;
    if (repeated_item->is<Expression_v2>()) {
        auto item = repeated_item->as<Expression_v2>().evaluate(context);
        int64_t repeated_size = repeated_item->as<Expression_v2>().get_size();
        if (!item.has_value()) return false;
        if (!item.value().is_integer()) throw std::runtime_error("Tried to replicate non integer");
        if (!packing) {
            repeated_value = std::vector(size, item.value().get_integer());
        } else {
            return pack_repetition(item.value().get_integer() , repeated_size, size);
        }
    } else if (repeated_item->is<Concatenation>()) {

        auto raw_item = repeated_item->as<Concatenation>().evaluate(context);
        if (!raw_item.has_value()) return std::nullopt;
        auto item = raw_item.value();
        if (item.is_integer())
            repeated_value = std::vector(size, item.get_integer());
        else {
            auto  item_vect = item.get_int_array().get_1d_slice({0,0});
            for (int i = 0; i< size; i++) {
                repeated_value.insert(repeated_value.end(), item_vect.begin(), item_vect.end());
            }
        }
    } else if (!repeated_item->is<Expression_v2>() && !repeated_item->is<Concatenation>()){
        auto item = repeated_item->evaluate(context);
        if (!item.has_value()) return false;
        if (!item.value().is_integer()) throw std::runtime_error("Tried to replicate non integer");
        int64_t repeated_size = repeated_item->get_size();
        if (repeated_size <= 0) repeated_size = item.value().get_integer().get_size();
        if (!packing) {
            repeated_value = std::vector(size, item.value().get_integer());
        } else {
            return pack_repetition(item.value().get_integer() , repeated_size, size);
        }
    } else {
        throw std::runtime_error("Encountered a unknown parameter value type");
    }

    result.set_1d_slice({0,0}, repeated_value);
    return result;
}

hdl_integer Replication::pack_repetition(hdl_integer value, int64_t width, int64_t count) {
    hdl_integer packed_result = 0;

    int64_t mask = (static_cast<int64_t>(1) << width) - 1;
    hdl_integer clean_value = value & mask;

    for (int i = 0; i < count; i++) {
        int64_t shift_amount = static_cast<int64_t>(i) * width;
        packed_result |= (clean_value << shift_amount);
    }

    return packed_result;
}

std::string Replication::print() const {
    std::ostringstream oss;
    oss << "{" << repetition_size->print()<<"{";
    oss << repeated_item->print();
    oss << "}}";
    return oss.str();
}

void Replication::set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context) {
    packing = s.unpacked_sizes.empty();
}
