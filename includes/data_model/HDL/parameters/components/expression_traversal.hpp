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

#ifndef ANANKE_EXPRESSION_TRAVERSAL_HPP
#define ANANKE_EXPRESSION_TRAVERSAL_HPP

#include <memory>

#include "data_model/HDL/parameters/components/Expression_base.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"
#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include "data_model/HDL/parameters/components/Replication.hpp"
#include "data_model/HDL/parameters/components/Ternary.hpp"
#include "data_model/HDL/parameters/components/Cast.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/HDL_builtin_function.hpp"
#include "data_model/HDL/parameters/components/Streaming.hpp"

// Definition of Expression_base::visit_subexpressions (declared in
// Expression_base.hpp). Lives here rather than in the base header because it
// needs the complete composite types; the base header would otherwise include
// everything that includes it. Null children are skipped: every existing walk
// null-guards anyway (stack walks check before pushing, recursive walks guard
// on entry), so skipping is behavior-identical for all of them.
template<typename Fn>
void Expression_base::visit_subexpressions(Fn &&fn) {
    if (is<Expression_v2>()) {
        auto &e = as<Expression_v2>();
        if (e.get_lhs()) fn(e.get_lhs());
        if (e.get_rhs()) fn(e.get_rhs());
    } else if (is<Concatenation>()) {
        for (auto &comp : as<Concatenation>().get_components()) {
            if (comp) fn(comp);
        }
    } else if (is<Replication>()) {
        auto &r = as<Replication>();
        if (r.get_size()) fn(r.get_size());
        if (r.get_item()) fn(r.get_item());
    } else if (is<Ternary>()) {
        auto &t = as<Ternary>();
        if (t.get_condition()) fn(t.get_condition());
        if (t.get_true_value()) fn(t.get_true_value());
        if (t.get_false_value()) fn(t.get_false_value());
    } else if (is<Cast>()) {
        auto &c = as<Cast>();
        if (c.get_size_expr()) fn(c.get_size_expr());
        if (c.get_content()) fn(c.get_content());
    } else if (is<HDL_function_call>()) {
        for (auto &arg : as<HDL_function_call>().get_arguments()) {
            if (arg) fn(arg);
        }
    } else if (is<HDL_builtin_function>()) {
        for (auto &arg : as<HDL_builtin_function>().get_arguments()) {
            if (arg) fn(arg);
        }
    } else if (is<Streaming>()) {
        auto &st = as<Streaming>();
        if (st.get_slice_size()) fn(st.get_slice_size());
        for (auto &comp : st.get_components()) {
            if (comp) fn(comp);
        }
    }
}

#endif //ANANKE_EXPRESSION_TRAVERSAL_HPP
