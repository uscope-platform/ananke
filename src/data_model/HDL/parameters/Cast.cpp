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

#include "data_model/HDL/parameters/Cast.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Cast)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Parameter_value_base, Cast)


Cast::Cast() {
    type = cast;
}

Cast::Cast(const Cast &other) {
    size = other.size;
    type = other.type;
    type_cast = other.type_cast;
    target_type = other.target_type;
    if (other.content != nullptr) content = other.content->clone_ptr();
}

Cast::Cast(Cast &&other) noexcept {
    size = other.size;
    type = other.type;
    type_cast = other.type_cast;
    target_type = other.target_type;
    if (other.content != nullptr) content = other.content->clone_ptr();
}

std::set<qualified_identifier> Cast::get_dependencies() const {
    std::set<qualified_identifier> deps;
    auto content_deps = content->get_dependencies();
    deps.insert(content_deps.begin(), content_deps.end());
    auto size_deps = size.get_dependencies();
    deps.insert(size_deps.begin(), size_deps.end());
    return deps;
}

bool Cast::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value) {
    bool ret = true;
    ret &= content->propagate_constant(constant_id, value);
    ret &= size.propagate_constant(constant_id, value);
    return ret;
}

void Cast::propagate_expression(const qualified_identifier &constant_id,
    const std::shared_ptr<Parameter_value_base> &value) {

    content->propagate_expression(constant_id, value);
    size.propagate_expression(constant_id, value);
}

std::optional<resolved_parameter> Cast::evaluate(bool pack_result) {
    auto content_val = content->evaluate(pack_result);
    if (!content_val.has_value()) return std::nullopt;
    if (!std::holds_alternative<int64_t>(content_val.value())) return content_val.value();
    auto raw_cast_size = size.evaluate(pack_result);
    if (!raw_cast_size.has_value()) return std::nullopt;
    if (!std::holds_alternative<int64_t>(raw_cast_size.value())) {
        spdlog::warn("Cast size evaluates to a non integer");
        return content_val.value();
    }
    auto raw_value = std::get<int64_t>(content_val.value());
    auto cast_size = std::get<int64_t>(raw_cast_size.value());
    int64_t mask = (1ULL << cast_size) - 1;
    return raw_value &  mask;
}

std::string Cast::print() const {
    std::string prefix;
    if (type_cast) prefix = target_type;
    else prefix = size.print();
    return  prefix + "'(" + content->print() + ")";
}

int64_t Cast::get_size() {
    return std::get<int64_t>(size.evaluate(true).value());
}

int64_t Cast::get_depth() {
    return std::max(content->get_depth(), size.get_depth()) + 1;
}

std::shared_ptr<Parameter_value_base> Cast::clone_ptr() const {
    Cast c;
    if (content != nullptr) c.content = content->clone_ptr();
    c.size = size;
    c.type_cast = type_cast;
    c.target_type = target_type;
    return std::make_shared<Cast>(c);
}

bool Cast::isEqual(const Parameter_value_base &other) const {

    const auto& rhs = static_cast<const Cast&>(other);
    bool res = true;
    res &= *content == *rhs.content;
    res &= size == rhs.size;
    res &= type_cast == rhs.type_cast;
    res &= target_type == rhs.target_type;
    return res;
}
