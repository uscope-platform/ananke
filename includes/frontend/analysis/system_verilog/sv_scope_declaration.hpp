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

#ifndef ANANKE_SV_SCOPE_DECLARATION_HPP
#define ANANKE_SV_SCOPE_DECLARATION_HPP

#include <cstdint>
#include <optional>
#include <string>
#include <vector>

// One name declared directly in a module, task or function scope: either a
// non-ANSI port declaration (`input x;`) or a net/variable declaration
// (`wire x;`, `integer x;`). The visitor extracts these in source order.
struct scope_declaration {
    std::string name;
    unsigned int line = 0;
    bool is_port = false;
    // Port declared with a net type or data type (`output integer x`), as
    // opposed to a bare direction and optional packed range.
    bool explicit_type = false;
    // net/reg/logic/bit (or no type): the kind that takes a packed range.
    bool vector_type = true;
    // Packed range as written, whitespace removed; empty when none.
    std::string range;
};

// A block, task, function, module, class, package, ... with its optional
// `: label` after the closing keyword, extracted for the end-label check.
struct end_label_info {
    std::string kind;   // "begin", "fork", "module", "task", ... for the message
    std::string name;   // declared / begin-label name; empty for an unnamed block
    std::string end;    // label written after the closing keyword
    bool has_end = false;
    unsigned int line = 0;      // line of the end label
};

// Shape of a typedef as far as the declaration rules need it.
struct typedef_shape {
    enum class kind_t {
        other, forward, enum_type, struct_type, union_type, real_type, string_type, chandle_or_event,
        class_type, unpacked_array, dynamic_array, queue
    };
    std::string name;
    kind_t kind = kind_t::other;
    unsigned int line = 0;
    int packed_dims = 0;        // packed dimensions written on the type
    bool is_atom = false;       // int/byte/integer/...: implicitly a vector
    // Named types the typedef contains or aliases by value (for cycle detection).
    std::vector<std::string> depends;
    int member_count = -1;      // struct members (declarators), -1 when not a struct
};

// An enum declaration: its base type and members, for the value checks.
struct enum_member_info {
    std::string name;
    unsigned int line = 0;
    int count = 1;                          // `name[N]` expands to N members
    std::optional<std::string> expression;  // text after `=`
};
struct enum_info {
    unsigned int line = 0;
    std::string base_name;                  // typedef base, empty for built-in bases
    bool has_base_dimension = false;        // `enum T [1:0]`
    int width = 32;                         // 0 = unknown
    bool is_signed = true;
    bool two_state = true;
    std::vector<enum_member_info> members;
};

// A member of a packed struct or union.
struct packed_member_info {
    std::string name;
    unsigned int line = 0;
    bool has_unpacked_dimension = false;
    bool has_default = false;
    std::string type_name;                  // named type, empty otherwise
    std::string builtin_issue;              // non-empty when the built-in type itself is not allowed
};

// A use of an automatic variable that the LRM forbids.
struct automatic_misuse {
    std::string variable;
    std::string how;           // "written by a nonblocking assignment", ...
    unsigned int line = 0;
};

// A misuse of a random construct (constraint, randsequence) found by the tree scan.
struct random_finding {
    enum class kind_t { constraint, undeclared_name };
    kind_t kind = kind_t::constraint;
    unsigned int line = 0;
    std::string message;
};

#endif //ANANKE_SV_SCOPE_DECLARATION_HPP
