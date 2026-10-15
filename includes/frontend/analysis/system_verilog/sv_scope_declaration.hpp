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

#include <string>

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

#endif //ANANKE_SV_SCOPE_DECLARATION_HPP
