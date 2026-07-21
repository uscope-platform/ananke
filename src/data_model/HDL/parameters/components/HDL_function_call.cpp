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

#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/Replication.hpp"

#include "analysis/loop_solver.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <algorithm>


CEREAL_REGISTER_TYPE(HDL_function_call)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, HDL_function_call)

void HDL_function_call::add_argument(const std::shared_ptr<Expression_base> &p) {
    arguments.push_back(p);
}

parameter_deps_t HDL_function_call::get_dependencies() const {
    parameter_deps_t retval;
    for (auto &arg:arguments) {
        retval.merge(arg->get_dependencies());
    }
    for(auto &s:body) {
        retval.merge(s->get_dependencies());
    }
    retval.functions.insert(qualified_identifier(package_prefix, function_name));
    return retval;
}


void HDL_function_call::propagate_function(const hdl_function_statement &def) {
    if(def.get_name() == function_name) {
        body.clear();
        for (const auto &stmt : def.get_body())
            body.push_back(stmt->clone());
        auto arg_names = def.get_arguments_names();
        for (int i =0;i<arg_names.size(); i++) {
            auto arg_val = arguments[i];
            for (auto &stmt : body) {
                if (auto asgn = std::dynamic_pointer_cast<hdl_assignment_statement>(stmt)) {
                    if (asgn->get_value())
                        asgn->get_value()->propagate_expression(qualified_identifier(arg_names[i]), arg_val);
                    if (asgn->get_index())
                        asgn->get_index()->propagate_expression(qualified_identifier(arg_names[i]), arg_val);
                }
            }
        }
    }
}

static void walk_body(
    std::vector<hdl_integer> &values,
    std::vector<int64_t> &value_sizes,
    const std::string &fcn_name,
    const std::vector<std::shared_ptr<hdl_statement_base>> &stmts,
    std::map<qualified_identifier, resolved_parameter> ctx
) {
    for (const auto &stmt : stmts) {
        if (auto asgn = std::dynamic_pointer_cast<hdl_assignment_statement>(stmt)) {
            if (!asgn->get_value()) continue;
            auto val = asgn->get_value()->evaluate(ctx);
            if (!val.has_value()) continue;
            if (asgn->get_target() == fcn_name) {
                int64_t idx_val = 0;
                if (asgn->get_index()) {
                    auto idx_res = asgn->get_index()->evaluate(ctx);
                    if (!idx_res.has_value() || !idx_res.value().is_integer()) continue;
                    idx_val = idx_res.value().get_integer().get_value();
                }
                if (idx_val >= 0 && static_cast<size_t>(idx_val) < values.size()) {
                    if (val.value().is_integer()) {
                        values[idx_val] = val.value().get_integer();
                        value_sizes[idx_val] = val.value().get_integer().get_size();
                    } else if (val.value().is_int_array()) {
                        values[idx_val] = val.value().get_int_array().get_1d_slice({0, 0})[0];
                        value_sizes[idx_val] = 0;
                    }
                }
            } else {
                ctx[qualified_identifier(asgn->get_target())] = val.value();
            }
        } else if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmt)) {
            auto indices = loop_solver::solve_loop(*loop, ctx);
            auto loop_var = loop->get_init()->get_identifier();
            for (auto &idx : indices) {
                auto loop_ctx = ctx;
                loop_ctx[loop_var] = resolved_parameter(idx);
                walk_body(values, value_sizes, fcn_name, loop->get_body(), loop_ctx);
            }
        }
    }
}

std::optional<resolved_parameter> HDL_function_call::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    if (function_name.starts_with("$")) {
        return evaluate_system_task(context);
    }
    if (body.empty()) return std::nullopt;

    if (body.size() == 1) {
        auto asgn = std::dynamic_pointer_cast<hdl_assignment_statement>(body[0]);
        if (asgn && asgn->get_target() == function_name && !asgn->get_index()) {
            auto raw_value = asgn->get_value()->evaluate(context);
            if (!raw_value) return std::nullopt;
            if (packing) {
                if (!raw_value.value().is_int_array()) return raw_value;
                auto components = raw_value.value().get_int_array().get_1d_slice({0, 0});
                auto size = asgn->get_value()->get_size();
                std::vector<int64_t> packing_sizes(components.size(), size);
                return pack_values(components, packing_sizes);
            }
            return raw_value;
        }
    }

    size_t max_idx = 0;
    for (const auto &stmt : body) {
        if (auto asgn = std::dynamic_pointer_cast<hdl_assignment_statement>(stmt)) {
            if (asgn->get_target() != function_name) continue;
            if (asgn->get_index()) {
                auto idx_res = asgn->get_index()->evaluate(context);
                if (idx_res.has_value() && idx_res.value().is_integer())
                    max_idx = std::max(max_idx, static_cast<size_t>(idx_res.value().get_integer().get_value() + 1));
            } else {
                max_idx = std::max(max_idx, static_cast<size_t>(1));
            }
        } else if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmt)) {
            auto indices = loop_solver::solve_loop(*loop, context);
            if (!indices.empty())
                max_idx = std::max(max_idx, static_cast<size_t>(indices.back().get_value() + 1));
        }
    }

    if (max_idx == 0) return std::nullopt;
    std::vector<hdl_integer> values(max_idx);
    std::vector<int64_t> value_sizes(max_idx);
    walk_body(values, value_sizes, function_name, body, context);

    apply_return_order_reversal(values, value_sizes, context);

    mdarray<hdl_integer> result;
    result.set_1d_slice({0, 0}, values);
    if (packing) {
        result.set_1d_slice({0,0}, {pack_values(values, value_sizes)});
    }
    return result;
}

void HDL_function_call::apply_return_order_reversal(
    std::vector<hdl_integer> &values,
    std::vector<int64_t> &value_sizes,
    const std::map<qualified_identifier, resolved_parameter> &context
) {
    if (packing || !has_return_unpacked_ascending) return;
    if (return_unpacked_ascending != container_unpacked_ascending) {
        std::reverse(values.begin(), values.end());
        std::reverse(value_sizes.begin(), value_sizes.end());
    }
}

std::optional<resolved_parameter> HDL_function_call::evaluate_system_task(const std::map<qualified_identifier, resolved_parameter> &context) {
    std::string task_name = function_name.substr(1, function_name.size()-1);
    std::vector<resolved_parameter> resolved_arguments;
    for (auto &arg:arguments) {
        auto resolved_val = arg->evaluate(context);
        if (!resolved_val.has_value()) return std::nullopt;
        resolved_arguments.push_back(resolved_val.value());
    }
    if (task_name == "rtoi") {
        if (resolved_arguments[0].is_real()) {
            return static_cast<hdl_integer>(std::round(resolved_arguments[0].get_real()));
        }
        if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer();
        }
        spdlog::warn("Encountered an invalid argument for a $rtoi call");
        return  0;
    }
    if (task_name == "itor") {
        if (resolved_arguments[0].is_real()) {
            return resolved_arguments[0].get_real();
        } else if (resolved_arguments[0].is_integer()) {
            return static_cast<double>(resolved_arguments[0].get_integer().get_value());
        }
        spdlog::warn("Encountered an invalid argument for a $itor call");
        return  0;
    }
    if (task_name == "ceil") {
        if (resolved_arguments[0].is_real()) {
            return std::ceil(resolved_arguments[0].get_real());
        } else if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer();
        }
        spdlog::warn("Encountered an invalid argument for a $ceil call");
    }
    if (task_name == "floor") {
        if (resolved_arguments[0].is_real()) {
            return std::floor(resolved_arguments[0].get_real());
        } else if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer();
        }
        spdlog::warn("Encountered an invalid argument for a $floor call");

    }
    if (task_name == "clog2") {
        if (resolved_arguments[0].is_real()) {
            return static_cast<hdl_integer>(std::ceil(std::log2(resolved_arguments[0].get_real())));
        } else if (resolved_arguments[0].is_integer()) {
            return static_cast<hdl_integer>(std::ceil(std::log2(resolved_arguments[0].get_integer().get_value())));
        }
        spdlog::warn("Encountered an invalid argument for a $floor call");
    }
    spdlog::warn("Unsupported system task {} encountered while parsing a parameter", function_name);
    return 0;
}

void HDL_function_call::set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context) {
    packing = s.unpacked_sizes.empty();
    container_unpacked_ascending = s.unpacked_ascending.empty() ? true : s.unpacked_ascending[0];
    if (s.return_unpacked_ascending.has_value()) {
        return_unpacked_ascending = s.return_unpacked_ascending.value();
        has_return_unpacked_ascending = true;
    }
    if (s.packed_sizes.empty() && s.unpacked_sizes.empty()) return;
    for (auto &stmt : body) {
        if (auto asgn = std::dynamic_pointer_cast<hdl_assignment_statement>(stmt)) {
            if (asgn->get_value()) asgn->get_value()->set_container_sizes(s, context);
        } else if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmt)) {
            for (auto &bs : loop->get_body()) {
                if (auto la = std::dynamic_pointer_cast<hdl_assignment_statement>(bs)) {
                    if (la->get_value()) la->get_value()->set_container_sizes(s, context);
                }
            }
        }
    }
}


std::string HDL_function_call::print() const {
    std::ostringstream result;
    if (!package_prefix.empty()) result<< package_prefix << "::";
    result << function_name << "(";
    for(int i = 0; i< arguments.size(); i++) {
        result << arguments[i]->print();
        if( arguments.size()>1 && i<arguments.size()-1) result << ", ";
    }
    result << ")";
    return result.str();
}

int64_t HDL_function_call::get_size() {
    return 0;
}


bool HDL_function_call::empty() const {
    return function_name.empty();
}

bool HDL_function_call::isEqual(const Expression_base &other) const {
    bool is_equal = true;


    const auto& rhs = static_cast<const HDL_function_call&>(other);
    is_equal &= function_name == rhs.function_name;
    if (arguments.size() != rhs.arguments.size()) return false;
    for (int i = 0; i< arguments.size(); i++) {
        is_equal &= *arguments[i] == *rhs.arguments[i];
    }
    is_equal &= body.size() == rhs.body.size();
    for (size_t i = 0; i < body.size(); i++)
        is_equal &= *body[i] == *rhs.body[i];
    is_equal &= package_prefix == rhs.package_prefix;
    return is_equal;
}
