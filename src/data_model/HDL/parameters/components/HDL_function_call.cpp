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
#include "data_model/HDL/parameters/HDL_parameter.hpp"
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
#include "data_model/HDL/statement/hdl_while_statement.hpp"
#include "data_model/HDL/statement/hdl_repeat_statement.hpp"
#include "data_model/HDL/statement/hdl_do_while_statement.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <algorithm>
#include <cctype>
#include <cstring>
#include <limits>
#include <set>
#include <sstream>

static constexpr int64_t MAX_SEQUENTIAL_ITERATIONS = 100'000;

namespace {
// Loop variables declared inside a function body (for-init names, including
// loops nested in other loops or conditionals): bound per evaluation in the
// call-local context, exactly like formals and locals.
void collect_body_loop_vars(
    const std::vector<std::shared_ptr<hdl_statement_base>> &stmts,
    std::set<std::string> &out) {
    for (const auto &s : stmts) {
        if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(s)) {
            if (loop->get_init() && !loop->get_init()->get_name().empty())
                out.insert(loop->get_init()->get_name());
            collect_body_loop_vars(loop->get_body(), out);
        } else if (auto cond = std::dynamic_pointer_cast<hdl_conditional_statement>(s)) {
            for (const auto &br : cond->get_branches()) collect_body_loop_vars(br.body, out);
            collect_body_loop_vars(cond->get_else_body(), out);
        }
    }
}
}

// Applies one assignment target: the function-name, struct-field and
// general branches of the former inline chain (early `continue`s become
// early `return`s). Shared by single-target statements and, per member, by
// concatenation assignments below.
void HDL_function_call::apply_member_assignment(
    const std::string &fcn_name,
    const qualified_identifier &target,
    const std::shared_ptr<Expression_base> &index,
    const std::shared_ptr<Expression_base> &value_expr,
    const std::expected<resolved_parameter, solver_errors> &val,
    std::map<qualified_identifier, resolved_parameter> &ctx,
    std::map<int64_t, hdl_integer> &value_map,
    std::map<int64_t, int64_t> &size_map,
    const std::shared_ptr<hdl_type> &rt,
    const std::optional<resolved_type> &expected_type
) {
    if (target.get_instance().empty() && target.get_name() == fcn_name) {
        if (!val.has_value()) {
            // return <struct-local>: the whole struct was never
            // entered as one value, but its per-field assigns live in
            // ctx under instance keys — pack the still-missing return
            // slots from them (direct funcname.field assigns win).
            if (!index && rt && rt->is<HDL_struct_type>() &&
                value_expr && value_expr->is<Identifier_token>()) {
                const auto &rid = value_expr->as<Identifier_token>().get_value();
                if (rid.is_bare() && rid.get_name() != fcn_name) {
                    const auto &members = rt->as<HDL_struct_type>().member;
                    for (size_t i = 0; i < members.size(); ++i) {
                        const int64_t midx = static_cast<int64_t>(i);
                        if (value_map.contains(midx)) continue;
                        qualified_identifier fkey(members[i].name);
                        fkey.set_instance_prefix({rid.get_name()});
                        auto fit = ctx.find(fkey);
                        if (fit == ctx.end()) continue;
                        if (fit->second.is_integer()) {
                            value_map[midx] = fit->second.get_integer();
                            size_map[midx] = HDL_function_call::declared_member_width(
                                members[i].type, ctx, fit->second.get_integer().get_size());
                        } else if (fit->second.is_int_array()) {
                            auto slice = fit->second.get_int_array().get_1d_slice({0, 0});
                            if (slice.empty()) continue;
                            value_map[midx] = slice[0];
                            size_map[midx] = 0;
                        }
                    }
                }
            }
            return;
        }
        int64_t idx = 0;
        if (index) {
            auto idx_res = index->evaluate(ctx, expected_type);
            if (!idx_res.has_value() || !idx_res.value().is_integer()) return;
            idx = idx_res.value().get_integer().get_value();
        }
        if (val.value().is_integer()) {
            value_map[idx] = val.value().get_integer();
            size_map[idx] = val.value().get_integer().get_size();
        } else if (val.value().is_int_array()) {
            auto slice = val.value().get_int_array().get_1d_slice({0, 0});
            if (slice.empty()) {
                spdlog::warn("Empty array value in function body assignment, defaulting to 0");
                return;
            }
            if (rt && rt->is<HDL_struct_type>()) {
                value_map[idx] = slice[0];
                size_map[idx] = 0;
            } else {
                hdl_integer packed = 0;
                int64_t shift = 0;
                for (const auto &e : slice) {
                    packed = packed | (e.truncate_to(e.get_size()) << hdl_integer(shift));
                    shift += e.get_size();
                }
                value_map[idx] = packed;
                size_map[idx] = packed.get_size();
            }
        }
    } else if (rt && rt->is<HDL_struct_type>() && target.get_instance().size() == 1 && target.get_instance().front() == fcn_name) {
        if (!val.has_value()) return;
        std::string field_name = target.get_name();
        const auto& members = rt->as<HDL_struct_type>().member;
        for (size_t i = 0; i < members.size(); ++i) {
            if (members[i].name == field_name) {
                int64_t idx = static_cast<int64_t>(i);
                if (index) {
                    auto idx_res = index->evaluate(ctx, expected_type);
                    if (!idx_res.has_value() || !idx_res.value().is_integer()) return;
                    idx = idx_res.value().get_integer().get_value();
                }
                if (val.value().is_integer()) {
                    value_map[idx] = val.value().get_integer();
                    // Pack at the declared member width so producer and
                    // consumer (extract_struct_fields) agree; indexed
                    // member assignments keep the previous behavior.
                    if (index) {
                        size_map[idx] = val.value().get_integer().get_size();
                    } else {
                        size_map[idx] = HDL_function_call::declared_member_width(
                            members[i].type, ctx, val.value().get_integer().get_size());
                    }
                } else if (val.value().is_int_array()) {
                    auto slice = val.value().get_int_array().get_1d_slice({0, 0});
                    if (slice.empty()) {
                        spdlog::warn("Empty array value in function body assignment, defaulting to 0");
                        return;
                    }
                    value_map[idx] = slice[0];
                    size_map[idx] = 0;
                }
                break;
            }
        }
    } else {
        if (!val.has_value()) return;
        if (index) {
            auto idx_res = index->evaluate(ctx, expected_type);
            if (!idx_res.has_value() || !idx_res.value().is_integer()) return;
            if (!val.value().is_integer()) return;
            std::vector<int64_t> idxv = {idx_res.value().get_integer().get_value()};
            while (idxv.size() < 3) idxv.insert(idxv.begin(), 0);
            mdarray<hdl_integer> arr;
            auto it = ctx.find(target);
            if (it != ctx.end() && it->second.is_int_array())
                arr = it->second.get_int_array();
            arr.set_value(idxv, val.value().get_integer());
            ctx[target] = resolved_parameter(arr);
        } else {
            ctx[target] = val.value();
        }
    }
}

// Concatenation assignment (`{a[1:0], b} = X`): every member is written.
// Members with literal `[msb:lsb]` bounds are unpacked MSB-first out of the
// shared RHS with a read-modify-write merge into their current value
// (absent reads as 0, per the 2-state rule); everything else behaves exactly
// like the equivalent single assignment (whole-var, selects as today).
void HDL_function_call::apply_concat_assignment(
    const std::string &fcn_name,
    const std::vector<qualified_identifier> &targets,
    const std::vector<std::shared_ptr<Expression_base>> &indices,
    const std::vector<concat_member_select> &selects,
    const std::shared_ptr<Expression_base> &value_expr,
    const std::expected<resolved_parameter, solver_errors> &val,
    std::map<qualified_identifier, resolved_parameter> &ctx,
    std::map<int64_t, hdl_integer> &value_map,
    std::map<int64_t, int64_t> &size_map,
    const std::shared_ptr<hdl_type> &rt,
    const std::optional<resolved_type> &expected_type,
    const std::map<std::string, int64_t> &local_widths
) {
    auto record_whole = [&](size_t i) {
        std::shared_ptr<Expression_base> index = i < indices.size() ? indices[i] : nullptr;
        apply_member_assignment(fcn_name, targets[i], index, value_expr, val, ctx,
                                value_map, size_map, rt, expected_type);
    };
    bool warned = false;
    auto warn_once = [&](const std::string &what) {
        if (!warned) {
            warned = true;
            spdlog::warn("Concatenation assignment {} in function '{}', proceeding", what, fcn_name);
        }
    };
    if (!val.has_value()) return;
    // 1. Shared RHS as bits, masked to its native width and zero-filled
    // above it (Xcelium parity: `{a, b} = -1` gives a = 0, b = -1, never
    // sign-extended). Native width is the explicit size when carried, else
    // the declared width of a bare identifier RHS, else the 32-bit unsized
    // default. Non-integral values have no bits to split: record whole.
    wide_integer rhs_bits = 0;
    int64_t rhs_width = 0;
    if (val.value().is_integer()) {
        const auto &iv = val.value().get_integer();
        int64_t native = -1;
        if (iv.has_explicit_size() && iv.get_size() > 0) {
            native = static_cast<int64_t>(iv.get_size());
        } else if (value_expr && value_expr->is<Identifier_token>()) {
            const auto &rid = value_expr->as<Identifier_token>().get_value();
            if (rid.is_bare()) {
                auto hit = local_widths.find(rid.get_name());
                if (hit != local_widths.end()) native = hit->second;
            }
        }
        if (native < 0) {
            // No declared width anywhere: computed values keep their minimal
            // width, everything else defaults to 32-bit unsized (house rule).
            uint64_t minimal = iv.get_size();
            if (minimal < 32) minimal = 32;
            native = minimal > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())
                         ? 32
                         : static_cast<int64_t>(minimal);
        }
        rhs_bits = iv.to_wide() & hdl_integer::width_mask(native).to_wide();
        rhs_width = native;
    } else if (val.value().is_int_array()) {
        auto packed = val.value().get_int_array().get_1d_slice({0, 0});
        if (packed.empty()) return;
        int64_t shift = 0;
        for (const auto &e : packed) {
            rhs_bits = rhs_bits | (wide_integer(e.truncate_to(e.get_size()).to_wide()) << shift);
            shift += static_cast<int64_t>(e.get_size());
            rhs_width = shift;
        }
    } else {
        warn_once("has non-integral RHS value");
        for (size_t i = 0; i < targets.size(); ++i) record_whole(i);
        return;
    }
    // 2. Per-member measurability. Unpacking needs each member's width;
    // slicing additionally needs every previous width (offsets). Function
    // and struct members always take the whole value through their dedicated
    // paths, but a known width still advances the cursor for followers.
    struct measure {
        int64_t width = -1;
        int64_t lo = -1;
        bool slice = false;
        bool bare = false;
        bool routed_whole = false;
    };
    auto struct_field_width = [&](const qualified_identifier &target) -> int64_t {
        if (!rt || !rt->is<HDL_struct_type>() || target.get_instance().size() != 1 ||
            target.get_instance().front() != fcn_name)
            return -1;
        for (const auto &member : rt->as<HDL_struct_type>().member) {
            if (member.name == target.get_name())
                return declared_member_width(member.type, ctx, -1);
        }
        return -1;
    };
    std::vector<measure> measures(targets.size());
    for (size_t i = 0; i < targets.size(); ++i) {
        const auto &target = targets[i];
        const bool is_fcn = target.get_instance().empty() && target.get_name() == fcn_name;
        concat_member_select sel = i < selects.size() ? selects[i] : concat_member_select{};
        if (is_fcn) {
            measures[i].routed_whole = true;
            continue;
        }
        if (sel.dynamic_select) {
            // Present-but-non-literal select (part-selects, parametric
            // bounds): width is unknowable, and declaration lookup would give
            // a bogus whole-var width for a slice. Whole value + cursor stop.
            continue;
        }
        if (sel.has_bounds()) {
            measures[i].width = (sel.hi > sel.lo ? sel.hi - sel.lo : sel.lo - sel.hi) + 1;
            measures[i].lo = sel.hi > sel.lo ? sel.lo : sel.hi;
            // Struct fields keep their dedicated whole-value path; every
            // other bounded member (plain or hierarchical) slices.
            if (struct_field_width(target) >= 0 && !target.get_instance().empty()) {
                measures[i].routed_whole = true;
            } else {
                measures[i].slice = true;
            }
            continue;
        }
        std::shared_ptr<Expression_base> index = i < indices.size() ? indices[i] : nullptr;
        if (index) {
            // Single-bit select: width 1 by language rule, recorded through
            // the existing index path exactly like a single assignment.
            measures[i].width = 1;
            continue;
        }
        if (!target.get_instance().empty()) {
            int64_t struct_width = struct_field_width(target);
            if (struct_width > 0) {
                measures[i].width = struct_width;
            } else {
                auto found = ctx.find(target);
                if (found != ctx.end() && found->second.is_integer() &&
                    found->second.get_integer().get_size() > 0)
                    measures[i].width = static_cast<int64_t>(found->second.get_integer().get_size());
            }
            continue;
        }
        auto hit = local_widths.find(target.get_name());
        if (hit != local_widths.end()) {
            measures[i].width = hit->second;
            measures[i].bare = true;
            continue;
        }
        auto found = ctx.find(target);
        if (found != ctx.end() && found->second.is_integer() && found->second.get_integer().get_size() > 0) {
            measures[i].width = static_cast<int64_t>(found->second.get_integer().get_size());
            measures[i].bare = true;
        }
    }
    for (auto &m : measures) {
        if (m.width <= 0) m.width = -1;
    }
    // 3. Normalize once when every width is known (standard assignment
    // conversion: truncate excess top bits with a warning, zero-extend).
    bool all_known = true;
    int64_t total = 0;
    for (const auto &m : measures) {
        if (m.width < 0) { all_known = false; break; }
        total += m.width;
    }
    if (all_known && total != rhs_width) {
        if (total < rhs_width) {
            warn_once("has " + std::to_string(rhs_width) + "-bit RHS into " +
                      std::to_string(total) + " bits, truncating");
            rhs_bits = rhs_bits & hdl_integer::width_mask(total).to_wide();
        }
        rhs_width = total;
    }
    // 4. MSB-first walk. The first unmeasurable member (and everything after
    // it, whose offsets are unknowable) takes the whole value. Bare members
    // slice like everyone else and replace (whole variable, no merge).
    int64_t cursor = rhs_width;
    bool cursor_known = true;
    for (size_t i = 0; i < targets.size(); ++i) {
        const auto &target = targets[i];
        if (!cursor_known || measures[i].width < 0) {
            warn_once("has unmeasurable member '" + target.get_name() + "'");
            record_whole(i);
            cursor_known = false;
            continue;
        }
        int64_t take = std::min(measures[i].width, cursor);
        if (take <= 0) {
            warn_once("has " + std::to_string(rhs_width) + "-bit RHS exhausted, zero-padding");
            record_whole(i);
            cursor_known = false;
            continue;
        }
        if (take < measures[i].width) {
            warn_once("has " + std::to_string(rhs_width) + "-bit RHS exhausted, zero-padding");
            cursor_known = false;
        }
        if (!measures[i].slice) {
            if (measures[i].bare) {
                wide_integer part = (rhs_bits >> (cursor - take)) & hdl_integer::width_mask(take).to_wide();
                cursor -= take;
                hdl_integer result;
                result.set_value(part);
                result.set_size(take);
                ctx[target] = resolved_parameter(result);
                continue;
            }
            record_whole(i);
            cursor -= take;
            continue;
        }
        int64_t lo = measures[i].lo;
        wide_integer field_mask = hdl_integer::width_mask(take).to_wide() << lo;
        wide_integer slice = ((rhs_bits >> (cursor - take)) & hdl_integer::width_mask(take).to_wide()) << lo;
        cursor -= take;
        wide_integer current = 0;
        uint64_t current_size = 0;
        if (auto found = ctx.find(target); found != ctx.end() && found->second.is_integer()) {
            current = found->second.get_integer().to_wide();
            current_size = found->second.get_integer().get_size();
        }
        wide_integer merged = (current & ~field_mask) | slice;
        hdl_integer result;
        result.set_value(merged);
        uint64_t want_size = static_cast<uint64_t>(lo + take);
        if (current_size > want_size) want_size = current_size;
        result.set_size(static_cast<int64_t>(want_size));
        ctx[target] = resolved_parameter(result);
    }
}

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

        const auto arg_names = linked_->get_arguments_names();
        std::set<std::string> intra_roots(arg_names.begin(), arg_names.end());

        for (const auto &local : linked_->get_local_variables()) {
            if (local) intra_roots.insert(local->get_name());
        }
        collect_body_loop_vars(linked_->get_body(), intra_roots);
        const auto body_deps = linked_->get_dependencies();
        for (const auto &d : body_deps.data) {
            if (!d.get_package_prefix().empty()) {
                retval.data.insert(d);
                continue;
            }
            const auto inst = d.get_instance();
            const std::string &root = inst.empty() ? d.get_name() : inst.front();
            if (!intra_roots.contains(root)) retval.data.insert(d);
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
    std::map<qualified_identifier, resolved_parameter> &ctx,
    std::map<int64_t, hdl_integer> &value_map,
    std::map<int64_t, int64_t> &size_map,
    const std::shared_ptr<hdl_type> &rt,
    const std::optional<resolved_type> &expected_type,
    const std::map<std::string, int64_t> &local_widths
) {
    for (const auto &stmt : stmts) {
        if (auto asgn = std::dynamic_pointer_cast<hdl_assignment_statement>(stmt)) {
            if (!asgn->get_value()) continue;
            auto val = asgn->get_value()->evaluate(ctx, expected_type);

            // Single-target fast path (all existing producers): identical
            // behavior to the former scalar fields.
            static const qualified_identifier no_target;
            static const std::shared_ptr<Expression_base> no_index;
            const auto &targets = asgn->get_targets();
            const auto &indices = asgn->get_indices();
            bool has_bounds = false;
            for (const auto &sel : asgn->get_member_selects()) {
                if (sel.has_bounds()) { has_bounds = true; break; }
            }
            if (targets.size() > 1 || has_bounds) {
                apply_concat_assignment(fcn_name, targets, indices, asgn->get_member_selects(), asgn->get_value(), val, ctx,
                                        value_map, size_map, rt, expected_type, local_widths);
                continue;
            }
            const qualified_identifier &target = targets.empty() ? no_target : targets.front();
            const std::shared_ptr<Expression_base> &index = indices.empty() ? no_index : indices.front();
            apply_member_assignment(fcn_name, target, index, asgn->get_value(), val, ctx,
                                    value_map, size_map, rt, expected_type);
        } else if (auto loop = std::dynamic_pointer_cast<hdl_loop_statement>(stmt)) {
            if (!loop->get_init()) {
                spdlog::warn("loop construct cannot be evaluated in constant function, it will be skipped");
                continue;
            }
            auto loop_var = loop->get_init()->get_identifier();
            auto indices = loop_solver::solve_loop(*loop, ctx);
            for (auto &idx : indices) {
                auto loop_ctx = ctx;
                loop_ctx[loop_var] = resolved_parameter(idx);
                walk_body(fcn_name, loop->get_body(), loop_ctx, value_map, size_map, rt, expected_type, local_widths);
                // Scalar updates (locals, accumulators) must survive into
                // later iterations: loop_ctx is per-iteration scratch, while
                // return-target writes already persist via value_map. Only
                // the loop variable itself is iteration-scoped.
                for (const auto &[key, val] : loop_ctx) {
                    if (key == loop_var) continue;
                    ctx[key] = val;
                }
            }
        } else if (auto while_loop = std::dynamic_pointer_cast<hdl_while_statement>(stmt)) {
            if (!while_loop->get_end_condition()) continue;
            int64_t iteration_count = 0;
            while (true) {
                auto cond = while_loop->get_end_condition()->evaluate(ctx, expected_type);
                if (!cond.has_value() || !cond.value().is_integer() ||
                    cond.value().get_integer() == 0) break;
                if (iteration_count >= MAX_SEQUENTIAL_ITERATIONS) {
                    spdlog::warn("while loop exceeded the maximum number of iterations ({})",
                                 MAX_SEQUENTIAL_ITERATIONS);
                    break;
                }
                walk_body(fcn_name, while_loop->get_body(), ctx, value_map, size_map, rt, expected_type, local_widths);
                iteration_count++;
            }
        } else if (auto repeat_loop = std::dynamic_pointer_cast<hdl_repeat_statement>(stmt)) {
            if (!repeat_loop->get_count()) continue;
            auto reps_val = repeat_loop->get_count()->evaluate(ctx, expected_type);
            if (!reps_val.has_value() || !reps_val.value().is_integer()) continue;
            int64_t reps = reps_val.value().get_integer().get_value();
            if (reps > MAX_SEQUENTIAL_ITERATIONS) {
                spdlog::warn("repeat count {} exceeds the maximum number of iterations ({})",
                             reps, MAX_SEQUENTIAL_ITERATIONS);
                reps = MAX_SEQUENTIAL_ITERATIONS;
            }
            for (int64_t i = 0; i < reps; i++) {
                walk_body(fcn_name, repeat_loop->get_body(), ctx, value_map, size_map, rt, expected_type, local_widths);
            }
        } else if (auto do_loop = std::dynamic_pointer_cast<hdl_do_while_statement>(stmt)) {\
            int64_t iteration_count = 0;
            while (true) {
                if (iteration_count >= MAX_SEQUENTIAL_ITERATIONS) {
                    spdlog::warn("do-while loop exceeded the maximum number of iterations ({})",
                                 MAX_SEQUENTIAL_ITERATIONS);
                    break;
                }
                walk_body(fcn_name, do_loop->get_body(), ctx, value_map, size_map, rt, expected_type, local_widths);
                iteration_count++;
                if (do_loop->get_end_condition()) {
                    auto cond = do_loop->get_end_condition()->evaluate(ctx, expected_type);
                    if (!cond.has_value() || !cond.value().is_integer() ||
                        cond.value().get_integer() == 0) break;
                } else {
                    break;
                }
            }
        } else if (auto cond = std::dynamic_pointer_cast<hdl_conditional_statement>(stmt)) {
            bool matched = false;
            for (auto &branch : cond->get_branches()) {
                if (branch.condition) {
                    auto result = branch.condition->evaluate(ctx, expected_type);
                    if (result.has_value() && result.value().is_integer() && result.value().get_integer() != 0) {
                        matched = true;
                        walk_body(fcn_name, branch.body, ctx, value_map, size_map, rt, expected_type, local_widths);
                        break;
                    }
                }
            }
            if (!matched)
                walk_body(fcn_name, cond->get_else_body(), ctx, value_map, size_map, rt, expected_type, local_widths);
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
                // NOTE: the package prefix is deliberately dropped — the
                // formal is a call-local binding and the body reads it
                // package-less (CVA6Cfg.XLEN, not pkg::CVA6Cfg.XLEN).
                qualified_identifier rekeyed(key.get_name());
                rekeyed.set_instance_prefix(new_inst);
                call_ctx[rekeyed] = val;
            }

        }
    }

    // With an incoming container type the locals hold the sized member values;
    // otherwise the members are used unchanged.
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

    // 4. Declared widths of function locals, for unpacking bare concatenation
    // members (`{a, b} = v` needs `a`/`b` widths; no types are visible
    // inside the walk itself). Unresolvable types stay absent = unmeasurable.
    std::map<std::string, int64_t> local_widths;
    for (const auto &local : linked_->get_local_variables()) {
        if (!local || !local->get_type()) continue;
        auto resolved = local->get_type()->evaluate_type(call_ctx);
        if (!resolved || resolved->packed_sizes.empty()) continue;
        uint64_t width = packed_width(*resolved);
        if (width == 0 || width > static_cast<uint64_t>(std::numeric_limits<int64_t>::max())) continue;
        local_widths[local->get_name()] = static_cast<int64_t>(width);
    }

    // 5. Walk the SHARED definition body read-only. Nothing here writes
    // through linked_: per-site state lives in call_ctx and the maps below.
    std::map<int64_t, hdl_integer> value_map;
    std::map<int64_t, int64_t> size_map;
    walk_body(function_name, linked_->get_body(), call_ctx, value_map, size_map, linked_->get_return_type(), expected_type, local_widths);

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
        // Producer/consumer agreement (and SV packed layout: the first
        // declared member is most significant). walk_body keys slots by
        // member index, so reverse into MSB-first order before packing —
        // matching what struct literals produce and what
        // extract_struct_fields splits. Concatenation is unaffected (it
        // pre-reverses into pack_values itself); non-struct packed returns
        // keep their existing order.
        if (linked_ && linked_->get_return_type() && linked_->get_return_type()->is<HDL_struct_type>()) {
            std::reverse(values.begin(), values.end());
            std::reverse(sizes.begin(), sizes.end());
        }
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
