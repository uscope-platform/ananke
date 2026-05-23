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
#include "data_model/HDL/parameters/components/Expression.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Replication)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Parameter_value_base, Replication)

Replication::Replication(const Expression &size, std::shared_ptr<Parameter_value_base> item)  {
    repeated_item = std::move(item);
    repetition_size = size;
    type = replication;
}

Replication::Replication(const Replication &other) {
    repetition_size = other.repetition_size;
    if(other.repeated_item != nullptr) repeated_item = other.repeated_item->clone_ptr();
    type = other.type;
}

Replication::Replication(Replication &&other) noexcept {
    repetition_size = other.repetition_size;
    if(other.repeated_item != nullptr) repeated_item = other.repeated_item->clone_ptr();
    type = other.type;
}

Replication Replication::clone()  const{
    Replication result;
    result.repetition_size = repetition_size;
    if(repeated_item != nullptr) result.repeated_item = repeated_item->clone_ptr();
    result.type = type;
    return result;
}


Replication & Replication::operator=(const Replication &other) {
    if (this != &other) {
        repetition_size = other.repetition_size;
        if(other.repeated_item != nullptr) repeated_item = other.repeated_item->clone_ptr();
        type = other.type;
    }
    return *this;
}

Replication & Replication::operator=(Replication &&other) noexcept {
    if (this != &other) {
        repetition_size = other.repetition_size;
        if(other.repeated_item != nullptr) repeated_item = other.repeated_item->clone_ptr();
        type = other.type;
    }
    return *this;
}

void Replication::set_size(const Expression &size) { repetition_size = size;}

std::set<qualified_identifier> Replication::get_dependencies()const {
    std::set<qualified_identifier> result, deps;
    deps = repetition_size.get_dependencies();
    result.insert(deps.begin(), deps.end());
    deps = repeated_item->get_dependencies();
    result.insert(deps.begin(), deps.end());
    return result;
}

bool Replication::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value) {
    bool result = true;

    result &= repetition_size.propagate_constant(constant_id, value);
    result &= repeated_item->propagate_constant(constant_id, value);
    return result;
}

void Replication::propagate_expression(const qualified_identifier &constant_id,
    const std::shared_ptr<Parameter_value_base> &value) {
    repetition_size.propagate_expression(constant_id, value);
    repeated_item->propagate_expression(constant_id, value);
}

void Replication::propagate_function(const HDL_function_def &def) {
    repetition_size.propagate_function(def);
    repeated_item->propagate_function(def);
}

std::optional<resolved_parameter> Replication::evaluate() {
    mdarray<int64_t> result;
    auto raw_size = repetition_size.evaluate();
    if (!raw_size.has_value()) return false;
    if (!raw_size.value().is_integer()) return false;
    auto size = raw_size.value().get_integer();
    mdarray<int64_t>::md_1d_array repeated_value;
    if (repeated_item->is_expression()) {
        auto item = repeated_item->as<Expression>().evaluate();
        int64_t repeated_size = repeated_item->as<Expression>().get_size();
        if (!item.has_value()) return false;
        if (!item.value().is_integer()) throw std::runtime_error("Tried to replicate non integer");
        if (!packing) {
            repeated_value = std::vector(size, item.value().get_integer());
        } else {
            return pack_repetition(item.value().get_integer(), repeated_size, size);
        }
    } else if (repeated_item->is_concatenation()) {

        auto raw_item = repeated_item->as<Concatenation>().evaluate();
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
    } else if (repeated_item->is_replication()) {
        // TODO: Probably i just need to call evaluate again "nested style" with packed st toet to true;
        throw std::runtime_error("Nested repetitions are not supported yet");
    } else {
        throw std::runtime_error("Encountered a unknown parameter value type");
    }

    result.set_1d_slice({0,0}, repeated_value);
    return result;
}

int64_t Replication::pack_repetition(int64_t value, int64_t width, int64_t count) {
    int64_t packed_result = 0;

    int64_t mask = (static_cast<int64_t>(1) << width) - 1;
    int64_t clean_value = value & mask;

    for (int i = 0; i < count; i++) {
        int64_t shift_amount = static_cast<int64_t>(i) * width;
        packed_result |= (clean_value << shift_amount);
    }

    return packed_result;
}

std::string Replication::print() const {
    std::ostringstream oss;
    oss << "{" << repetition_size.print()<<"{";
    oss << repeated_item->print();
    oss << "}}";
    return oss.str();
}

void Replication::set_container_sizes(const resolved_type &s) {
    packing = s.unpacked_sizes.empty();
}
