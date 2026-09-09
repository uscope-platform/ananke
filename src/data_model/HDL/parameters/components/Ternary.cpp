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

#include "data_model/HDL/parameters/components/Ternary.hpp"

#include <spdlog/spdlog.h>

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Ternary)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, Ternary)

void Ternary::propagate_function(const hdl_function_def_ptr &def) {
    if (condition) condition->propagate_function(def);
    if (true_value) true_value->propagate_function(def);
    if (false_value) false_value->propagate_function(def);
}

parameter_deps_t Ternary::get_dependencies() const {
    parameter_deps_t ret_val;
    ret_val.merge(condition->get_dependencies());
    ret_val.merge(true_value->get_dependencies());
    ret_val.merge(false_value->get_dependencies());

    return ret_val;
}

std::expected<resolved_parameter, solver_errors> Ternary::evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type) {
    auto condition_value = condition->evaluate(context);
    if (!condition_value.has_value()) return std::unexpected{missing_value};
    bool cond_true;
    if (condition_value.value().is_integer()) {
        cond_true = condition_value.value().get_integer() != 0;
    } else if (condition_value.value().is_real()) {
        cond_true = condition_value.value().get_real() != 0.0;
    } else {
         spdlog::warn("Ternary condition is of unsupported type");
        return std::unexpected{unsupported};
    }
    if (!cond_true) {
        return false_value->evaluate(context, expected_type);
    } else {
        return true_value->evaluate(context, expected_type);
    }
}

std::optional<resolved_type> Ternary::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) const {
    auto t_t = true_value ? true_value->resolve_expression_type(context) : std::nullopt;
    auto f_t = false_value ? false_value->resolve_expression_type(context) : std::nullopt;

    if (!t_t) return f_t;
    if (!f_t) return t_t;
    if (t_t->is_real || f_t->is_real) {
        resolved_type result;
        result.is_real = true;
        result.packed_sizes.push_back(64);
        result.packed_ascending.push_back(false);
        result.packed_left.push_back(63);
        result.packed_right.push_back(0);
        return result;
    }

    auto width_of = [](const resolved_type &t) -> uint64_t {
        uint64_t w = 1;
        for (auto ps : t.packed_sizes) w *= ps;
        return w;
    };
    if (width_of(*t_t) >= width_of(*f_t)) return t_t;
    return f_t;
}

std::string Ternary::print() const {
    std::ostringstream oss;
    oss << condition->print();
    oss << " ? ";
    if (true_value) oss << true_value->print();
    oss << " : ";
    if (false_value) oss << false_value->print();
    return oss.str();
}

bool Ternary::isEqual(const Expression_base &other) const {
    const auto& rhs = static_cast<const Ternary&>(other);

    bool ret_val = true;
    ret_val &= *condition == *rhs.condition;
    if (true_value && rhs.true_value) ret_val &= *true_value == *rhs.true_value;
    else if (true_value || rhs.true_value) ret_val = false;
    if (false_value && rhs.false_value) ret_val &= *false_value == *rhs.false_value;
    else if (false_value || rhs.false_value) ret_val = false;
    return ret_val;
}
