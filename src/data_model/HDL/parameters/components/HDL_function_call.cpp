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
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/parameters/components/Replication.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"
#include "data_model/HDL/types/HDL_union_type.hpp"
#include "analysis/type_cast_engine.hpp"

#include "analysis/loop_solver.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <sstream>


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
    // Forward first so nested calls visible from earlier rounds resolve.
    // Bodies filled below are deliberately not traversed in this pass; the
    // solver fixpoint picks them up in later rounds. This ordering also keeps
    // directly recursive functions terminating.
    for (auto &arg : arguments) {
        if (arg) arg->propagate_function(def);
    }
    for (auto &stmt : body) {
        if (stmt) stmt->propagate_function(def);
    }
    if(def.get_name() == function_name) {
        body.clear();
        for (const auto &stmt : def.get_body())
            body.push_back(stmt->clone());
        return_type = def.get_return_type();
        auto arg_names = def.get_arguments_names();
        for (int i =0;i<arg_names.size(); i++) {
            if (i >= static_cast<int>(arguments.size())) break;
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

void HDL_function_call::propagate_expression(const qualified_identifier &constant_id,
                                             const std::shared_ptr<Expression_base> &value) {
    for (auto &arg : arguments) {
        if (arg && arg->is<Identifier_token>() && arg->as<Identifier_token>().get_value() == constant_id) {
            arg = value;
        } else if (arg) {
            arg->propagate_expression(constant_id, value);
        }
    }
}

void HDL_function_call::walk_body(
    const std::string &fcn_name,
    const std::vector<std::shared_ptr<hdl_statement_base>> &stmts,
    std::map<qualified_identifier, resolved_parameter> ctx,
    std::map<int64_t, hdl_integer> &value_map,
    std::map<int64_t, int64_t> &size_map,
    const std::shared_ptr<hdl_type> &rt
) {
    for (const auto &stmt : stmts) {
        if (auto asgn = std::dynamic_pointer_cast<hdl_assignment_statement>(stmt)) {
            if (!asgn->get_value()) continue;
            auto val = asgn->get_value()->evaluate(ctx);
            if (!val.has_value()) continue;

            const auto &target = asgn->get_target();
            if (target == fcn_name) {
                int64_t idx = 0;
                if (asgn->get_index()) {
                    auto idx_res = asgn->get_index()->evaluate(ctx);
                    if (!idx_res.has_value() || !idx_res.value().is_integer()) continue;
                    idx = idx_res.value().get_integer().get_value();
                }
                if (val.value().is_integer()) {
                    value_map[idx] = val.value().get_integer();
                    size_map[idx] = val.value().get_integer().get_size();
                } else if (val.value().is_int_array()) {
                    auto slice = val.value().get_int_array().get_1d_slice({0, 0});
                    if (slice.empty()) {
                        spdlog::warn("Empty array value in function body assignment, defaulting to 0");
                        continue;
                    }
                    value_map[idx] = slice[0];
                    size_map[idx] = 0;
                }
            } else if (rt && rt->is<HDL_struct_type>() && target.starts_with(fcn_name + ".")) {
                std::string field_name = target.substr(fcn_name.size() + 1);
                const auto& members = rt->as<HDL_struct_type>().member;
                for (size_t i = 0; i < members.size(); ++i) {
                    if (members[i].name == field_name) {
                        int64_t idx = static_cast<int64_t>(i);
                        if (asgn->get_index()) {
                            auto idx_res = asgn->get_index()->evaluate(ctx);
                            if (!idx_res.has_value() || !idx_res.value().is_integer()) continue;
                            idx = idx_res.value().get_integer().get_value();
                        }
                        if (val.value().is_integer()) {
                            value_map[idx] = val.value().get_integer();
                            size_map[idx] = val.value().get_integer().get_size();
                        } else if (val.value().is_int_array()) {
                            auto slice = val.value().get_int_array().get_1d_slice({0, 0});
                            if (slice.empty()) {
                                spdlog::warn("Empty array value in function body assignment, defaulting to 0");
                                continue;
                            }
                            value_map[idx] = slice[0];
                            size_map[idx] = 0;
                        }
                        break;
                    }
                }
            } else {
                ctx[qualified_identifier(target)] = val.value();
            }
        } else if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmt)) {
            auto indices = loop_solver::solve_loop(*loop, ctx);
            auto loop_var = loop->get_init()->get_identifier();
            for (auto &idx : indices) {
                auto loop_ctx = ctx;
                loop_ctx[loop_var] = resolved_parameter(idx);
                walk_body(fcn_name, loop->get_body(), loop_ctx, value_map, size_map, rt);
            }
        } else if (auto cond = std::dynamic_pointer_cast<hdl_conditional_statement>(stmt)) {
            bool matched = false;
            for (auto &branch : cond->get_branches()) {
                if (branch.condition) {
                    auto result = branch.condition->evaluate(ctx);
                    if (result.has_value() && result.value().is_integer() && result.value().get_integer() != 0) {
                        matched = true;
                        walk_body(fcn_name, branch.body, ctx, value_map, size_map, rt);
                        break;
                    }
                }
            }
            if (!matched)
                walk_body(fcn_name, cond->get_else_body(), ctx, value_map, size_map, rt);
        }
    }
}

std::expected<resolved_parameter, solver_errors> HDL_function_call::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    if (body.empty()) return std::unexpected{empty_body};

    std::map<int64_t, hdl_integer> value_map;
    std::map<int64_t, int64_t> size_map;
    walk_body(function_name, body, context, value_map, size_map, return_type);

    if (value_map.empty()) return std::unexpected{missing_value};

    constexpr int64_t MAX_FUNCTION_RETURN_INDEX = 1'000'000;
    int64_t max_key = value_map.rbegin()->first;
    if (max_key < 0) max_key = 0;
    if (max_key >= MAX_FUNCTION_RETURN_INDEX) {
        spdlog::warn("Function return index {} exceeds the maximum supported size of {}, clamping", value_map.rbegin()->first, MAX_FUNCTION_RETURN_INDEX);
        max_key = MAX_FUNCTION_RETURN_INDEX - 1;
    }
    size_t max_idx = static_cast<size_t>(max_key + 1);
    std::vector<hdl_integer> values(max_idx);
    std::vector<int64_t> sizes(max_idx);
    for (auto &[idx, val] : value_map) {
        if (idx >= 0 && static_cast<size_t>(idx) < max_idx) {
            values[idx] = val;
            sizes[idx] = size_map[idx];
        }
    }

    if (values.size() == 1) {
        return resolved_parameter(values[0]);
    }

    apply_return_order_reversal(values, sizes, context);

    if (packing) {
        return resolved_parameter(pack_values(values, sizes));
    }

    mdarray<hdl_integer> result;
    result.set_1d_slice({0, 0}, values);
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


std::optional<resolved_type> HDL_function_call::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context) const {
    if (return_type) {
        return return_type->evaluate_type(context);
    }
    return std::nullopt;
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
