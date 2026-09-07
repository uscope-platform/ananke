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
#include "data_model/HDL/types/resolved_type.hpp"
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
#include <limits>
#include <set>
#include <sstream>

int64_t HDL_function_call::declared_member_width(
    const std::shared_ptr<hdl_type> &member_type,
    const std::map<qualified_identifier, resolved_parameter> &context,
    int64_t fallback
) {
    if (!member_type) return fallback;
    auto resolved = member_type->evaluate_type(context);
    if (!resolved || resolved->packed_sizes.empty()) return fallback;
    uint64_t width = packed_width(*resolved);
    if (width == 0 || width > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) return fallback;
    return static_cast<int64_t>(width);
}

CEREAL_REGISTER_TYPE(HDL_function_call)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, HDL_function_call)

void HDL_function_call::add_argument(const std::shared_ptr<Expression_base> &p) {
    arguments.push_back(p);
}

parameter_deps_t HDL_function_call::get_dependencies() const {
    parameter_deps_t retval;
    for (auto &arg : arguments) {
        if (arg) retval.merge(arg->get_dependencies());
    }
    if (linked_) {
        // True dependencies only: the shared definition body's deps minus
        // the formal parameters (bound at evaluation, so not external deps).
        // This also drops the false CVA6Cfg-style cycle with no sorter
        // hacks. Nested function references are kept so the solver fixpoint
        // keeps discovering nested calls.
        const auto arg_names = linked_->get_arguments_names();
        const std::set<std::string> formals(arg_names.begin(), arg_names.end());
        const auto body_deps = linked_->get_dependencies();
        for (const auto &d : body_deps.data) {
            if (!d.get_package_prefix().empty()) {
                retval.data.insert(d);
                continue;
            }
            const auto inst = d.get_instance();
            const std::string &root = inst.empty() ? d.get_name() : inst.front();
            if (!formals.contains(root)) retval.data.insert(d);
        }
        retval.functions.insert(body_deps.functions.begin(), body_deps.functions.end());
        retval.types.insert(body_deps.types.begin(), body_deps.types.end());
    }
    retval.functions.insert(qualified_identifier(package_prefix, function_name));
    return retval;
}

void HDL_function_call::propagate_function(const hdl_function_def_ptr &def) {
    if (!def) return;
    // Forward into argument subtrees first (per-site nodes) so nested calls
    // visible from earlier rounds resolve. This writes nothing but
    // idempotent links (same shared_ptr value from every site).
    for (auto &arg : arguments) {
        if (arg) arg->propagate_function(def);
    }
    if (def->get_name() == function_name) {
        if (linked_ != def) {
            linked_ = def;
            // First link of this definition: cascade it into the shared
            // body so nested calls can link in later fixpoint rounds.
            // Nested nodes only record their own links — no values are
            // rewritten, so sharing stays safe by construction.
            for (const auto &stmt : linked_->get_body()) {
                if (stmt) stmt->propagate_function(def);
            }
        }
        return;
    }
    if (linked_) {
        // A different function: cascade into the already-linked body so
        // nested calls keep resolving as the fixpoint delivers new defs.
        // Same-definition deliveries short-circuit inside nested calls
        // (linked_ == def skips the cascade), so directly recursive
        // functions still terminate.
        for (const auto &stmt : linked_->get_body()) {
            if (stmt) stmt->propagate_function(def);
        }
    }
}

void HDL_function_call::walk_body(
    const std::string &fcn_name,
    const std::vector<std::shared_ptr<hdl_statement_base>> &stmts,
    std::map<qualified_identifier, resolved_parameter> ctx,
    std::map<int64_t, hdl_integer> &value_map,
    std::map<int64_t, int64_t> &size_map,
    const std::shared_ptr<hdl_type> &rt,
    const std::optional<resolved_type> &expected_type
) {
    for (const auto &stmt : stmts) {
        if (auto asgn = std::dynamic_pointer_cast<hdl_assignment_statement>(stmt)) {
            if (!asgn->get_value()) continue;
            auto val = asgn->get_value()->evaluate(ctx, expected_type);
            if (!val.has_value()) continue;

            const auto &target = asgn->get_target();
            if (target == fcn_name) {
                int64_t idx = 0;
                if (asgn->get_index()) {
                    auto idx_res = asgn->get_index()->evaluate(ctx, expected_type);
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
                            auto idx_res = asgn->get_index()->evaluate(ctx, expected_type);
                            if (!idx_res.has_value() || !idx_res.value().is_integer()) continue;
                            idx = idx_res.value().get_integer().get_value();
                        }
                        if (val.value().is_integer()) {
                            value_map[idx] = val.value().get_integer();
                            // Pack at the declared member width so producer and
                            // consumer (extract_struct_fields) agree; indexed
                            // member assignments keep the previous behavior.
                            if (asgn->get_index()) {
                                size_map[idx] = val.value().get_integer().get_size();
                            } else {
                                size_map[idx] = declared_member_width(
                                    members[i].type, ctx, val.value().get_integer().get_size());
                            }
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
                walk_body(fcn_name, loop->get_body(), loop_ctx, value_map, size_map, rt, expected_type);
            }
        } else if (auto cond = std::dynamic_pointer_cast<hdl_conditional_statement>(stmt)) {
            bool matched = false;
            for (auto &branch : cond->get_branches()) {
                if (branch.condition) {
                    auto result = branch.condition->evaluate(ctx, expected_type);
                    if (result.has_value() && result.value().is_integer() && result.value().get_integer() != 0) {
                        matched = true;
                        walk_body(fcn_name, branch.body, ctx, value_map, size_map, rt, expected_type);
                        break;
                    }
                }
            }
            if (!matched)
                walk_body(fcn_name, cond->get_else_body(), ctx, value_map, size_map, rt, expected_type);
        }
    }
}

std::expected<resolved_parameter, solver_errors> HDL_function_call::evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type) {
    if (!linked_) return std::unexpected{empty_body};

    const auto arg_names = linked_->get_arguments_names();

    // 1. Each actual is evaluated once in the caller context (the old
    // substitution re-evaluated a formal at every use). The call's own
    // expected type is threaded so actuals size exactly as the grafted
    // expressions did under the old per-site body sizing.
    std::vector<std::optional<resolved_parameter>> actual_vals;
    actual_vals.reserve(arg_names.size());
    for (size_t i = 0; i < arg_names.size(); ++i) {
        if (i < arguments.size() && arguments[i]) {
            auto v = arguments[i]->evaluate(context, expected_type);
            if (v.has_value() && v.value().is_integer()) {
                // Width lift: the old substitution grafted the actual NODE,
                // so body truncation saw the node's resolved width (unsized
                // literals resolve to at least 32). A bound VALUE read back
                // through an Identifier would otherwise fall back to the
                // parse-minimal size (e.g. 5+7 truncated to 3 bits = 4).
                // Widen-only: explicit small sizes are preserved.
                auto iv = v.value().get_integer();
                if (auto t = arguments[i]->resolve_expression_type(context, expected_type);
                    t && !t->is_real && !t->packed_sizes.empty()) {
                    const auto w = packed_width(*t);
                    if (w > iv.get_size()) {
                        iv.set_size(static_cast<int64_t>(w));
                        v.value() = resolved_parameter(iv);
                    }
                }
                actual_vals.emplace_back(v.value());
            }
            else if (v.has_value()) actual_vals.emplace_back(v.value());
            else actual_vals.emplace_back(std::nullopt);
        } else {
            // Fewer actuals than formals: leave the formal unbound, exactly
            // like the old substitution (which simply skipped it), so reads
            // miss in the context and the assignment is skipped.
            actual_vals.emplace_back(std::nullopt);
        }
    }

    std::map<qualified_identifier, resolved_parameter> call_ctx = context;

    // 2. Enum members declared in the function scope are constants.
    // Bound before formals so formals win on name clashes (as before).
    for (const auto &local : linked_->get_local_variables()) {
        if (!local || !local->get_type() || !local->get_type()->is<HDL_enum_type>()) continue;
        for (const auto &member : local->get_type()->as<HDL_enum_type>().members) {
            if (!member.value.has_value()) continue;
            call_ctx[qualified_identifier(member.name)] =
                resolved_parameter(hdl_integer(static_cast<int64_t>(member.value.value())));
        }
    }

    // 3. Bind formals. A struct actual passed by identifier re-keys the
    // caller's already-split field entries (extract_struct_fields output)
    // under the formal name, so formal.field reads resolve with no type
    // info and no tree rewriting. Only the local call_ctx is written.
    for (size_t i = 0; i < arg_names.size(); ++i) {
        if (!actual_vals[i].has_value()) continue;
        call_ctx[qualified_identifier(arg_names[i])] = actual_vals[i].value();
        if (i < arguments.size() && arguments[i] && arguments[i]->is<Identifier_token>()) {
            const auto &actual_id = arguments[i]->as<Identifier_token>().get_value();
            std::vector<std::string> actual_path = actual_id.get_instance();
            actual_path.push_back(actual_id.get_name());
            for (const auto &[key, val] : context) {
                const auto key_inst = key.get_instance();
                if (key_inst.size() < actual_path.size()) continue;
                if (!std::equal(actual_path.begin(), actual_path.end(), key_inst.begin())) continue;
                std::vector<std::string> new_inst;
                new_inst.push_back(arg_names[i]);
                new_inst.insert(new_inst.end(), key_inst.begin() + actual_path.size(), key_inst.end());
                qualified_identifier rekeyed(key.get_name());
                if (!key.get_package_prefix().empty()) rekeyed.set_package_prefix(key.get_package_prefix());
                rekeyed.set_instance_prefix(new_inst);
                call_ctx[rekeyed] = val;
            }
        }
    }

    // Locals mirror the set_container_sizes flag derivation: with an incoming
    // container type they hold what the members would have held after sizing;
    // otherwise the members are used unchanged, exactly as before.
    const bool packing_l = expected_type ? expected_type->unpacked_sizes.empty() : false;
    bool container_unpacked_ascending_l = false;
    bool has_return_unpacked_ascending_l = false;
    bool return_unpacked_ascending_l = false;
    if (expected_type) {
        const auto &s = *expected_type;
        container_unpacked_ascending_l = s.unpacked_ascending.empty() ? true : s.unpacked_ascending[0];
        if (s.return_unpacked_ascending.has_value()) {
            return_unpacked_ascending_l = s.return_unpacked_ascending.value();
            has_return_unpacked_ascending_l = true;
        }
    }

    // 4. Walk the SHARED definition body read-only. Nothing here writes
    // through linked_: per-site state lives in call_ctx and the maps below.
    std::map<int64_t, hdl_integer> value_map;
    std::map<int64_t, int64_t> size_map;
    walk_body(function_name, linked_->get_body(), call_ctx, value_map, size_map, linked_->get_return_type(), expected_type);

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

    apply_return_order_reversal(values, sizes, context, packing_l,
        has_return_unpacked_ascending_l, return_unpacked_ascending_l,
        container_unpacked_ascending_l);

    if (packing_l) {
        return resolved_parameter(pack_values(values, sizes));
    }

    mdarray<hdl_integer> result;
    result.set_1d_slice({0, 0}, values);
    return result;
}

void HDL_function_call::apply_return_order_reversal(
    std::vector<hdl_integer> &values,
    std::vector<int64_t> &value_sizes,
    const std::map<qualified_identifier, resolved_parameter> &context,
    bool packing,
    bool has_return_unpacked_ascending,
    bool return_unpacked_ascending,
    bool container_unpacked_ascending
) {
    (void)context;
    if (packing || !has_return_unpacked_ascending) return;
    if (return_unpacked_ascending != container_unpacked_ascending) {
        std::reverse(values.begin(), values.end());
        std::reverse(value_sizes.begin(), value_sizes.end());
    }
}

std::optional<resolved_type> HDL_function_call::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) const {
    if (linked_ && linked_->get_return_type()) {
        return linked_->get_return_type()->evaluate_type(context);
    }
    return std::nullopt;
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
    // Deliberately link-insensitive: equality is structural on the call, so
    // it is stable across propagation (the old body compare flipped from
    // false to true as bodies filled in).
    is_equal &= package_prefix == rhs.package_prefix;
    return is_equal;
}
