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

#ifndef ANANKE_SV_PARSING_HELPERS_HPP
#define ANANKE_SV_PARSING_HELPERS_HPP

#include <memory>
#include <string>

#include <vector>

#include "mgp_sv/sv2017.h"
#include "frontend/analysis/system_verilog/sv_scope_declaration.hpp"
#include "data_model/HDL/parameters/common/qualified_identifier.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/parameters/components/token/Real_token.hpp"
#include "data_model/HDL/parameters/components/token/Time_token.hpp"
#include "data_model/HDL/parameters/components/token/String_token.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"

namespace sv_parsing_helpers {
    qualified_identifier parse_qualified_identifier(mgp_sv::sv2017::Package_or_class_scoped_pathContext *ctx);
    std::shared_ptr<Expression_base> make_value(const std::string &s);

    // Port, net and variable declarations made directly in a module, task or
    // function scope, in source order. Nested scopes (blocks, generate
    // constructs, tasks, functions, classes, ...) are not entered.
    std::vector<scope_declaration> collect_scope_declarations(antlr4::ParserRuleContext *scope);

    // Name and closing label of a labelled construct (begin/fork blocks, generate blocks, modules, interfaces,
    // programs, packages, classes, checkers, primitives, tasks, functions); has_end is false when the context
    // is none of those or has no closing label.
    end_label_info end_labels(antlr4::ParserRuleContext *ctx);

    // Number of elements of a positional assignment pattern `'{a, b, c}`; -1 when the text is not one
    // (named, default or replication patterns, or not a pattern at all).
    int positional_pattern_elements(const std::string &text);
    // Number of elements of a literal `[a:b]` dimension, -1 when it is not literal.
    int first_literal_dimension(mgp_sv::sv2017::Variable_dimensionContext *dim);

    // Scope constructs around `node`, innermost first, ending with the unit root (the scope identity used by
    // the semantic checker's name tables).
    std::vector<const void *> enclosing_scopes(antlr4::tree::ParseTree *node);

    // Misuses of automatic variables in a `task automatic` / `function automatic`: nonblocking
    // assignments to them (or with them in the event control) and their use in procedural
    // continuous assignments. Empty for static tasks and functions.
    std::vector<automatic_misuse> find_automatic_misuse(antlr4::ParserRuleContext *task_or_function);

    // `typedef ...;` as the type-shape rules see it (kind, packed dimensions, named types it contains).
    typedef_shape describe_typedef(mgp_sv::sv2017::Type_declarationContext *ctx);
    // An enum `data_type` (KW_ENUM): base type and members.
    enum_info describe_enum(mgp_sv::sv2017::Data_typeContext *ctx);
    // Members of a `struct packed` / `union packed` data_type.
    std::vector<packed_member_info> describe_packed_members(mgp_sv::sv2017::Data_typeContext *ctx);
}

#endif
