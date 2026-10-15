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

#ifndef ANANKE_SV_SEMANTIC_CHECKER_HPP
#define ANANKE_SV_SEMANTIC_CHECKER_HPP

#include <memory>
#include <optional>
#include <set>
#include <string>
#include <vector>

#include "data_model/parse_options.hpp"

class hdl_type;

// Stable rule identifiers. The checker owns these, the human-readable text,
// the recorded violations and the process exit code mapping below, so adding
// a rule never touches the visitor, the analyzer or the CLI: they just
// forward what was recorded.
enum class semantic_error_code {
    builtin_method_override = 1,
    enum_value_size_mismatch = 2,
    enum_unknown_in_two_state = 3,
    pure_virtual_in_non_virtual_class = 4,
    // Future rules append here.
};

struct semantic_violation {
    semantic_error_code code;
    std::string message;
};

// LRM semantic rule checks for SystemVerilog declarations.
//
// The visitor stays a pure model builder: it reports what was declared
// (with source location) and this class decides whether it is legal,
// logging the diagnostic and recording the violation internally. No
// exceptions, so rules get unit tests here instead of golden log files.
// All checks honor parse_options::disable_semantic_checks for hot paths
// that only need dependency information.
class sv_semantic_checker {
public:
    void set_options(const parse_options &o) {
        opts_ = o;
        violations_.clear();
    }

    // IEEE 1800-2017 §18: randomize(), rand_mode() and constraint_mode()
    // are built-in methods of every class and shall not be overridden.
    // (pre_randomize()/post_randomize() are the sanctioned user hooks and
    // stay legal.) Logs `file:line: error: ...`, records the violation and
    // returns true when `method_name` may not be declared as a class
    // method; returns false when legal.
    [[nodiscard]] bool check_class_method(
        const std::string &file, unsigned int line, const std::string &method_name);

    // IEEE 1800-2017 §8.21: a pure virtual method prototype may only be
    // declared in an abstract (`virtual class`) class. Logs `file:line:
    // error: ...`, records the violation and returns true when
    // `class_is_virtual` is false.
    [[nodiscard]] bool check_pure_virtual_method(
        const std::string &file, unsigned int line, const std::string &class_name,
        bool class_is_virtual);

    // IEEE 1800-2017 §6.19: when an enum member value is a sized literal
    // constant, its size shall match the enum base type width, even if the
    // value fits; and an x/z value in an enum with no explicit data type or
    // an explicit 2-state base is a syntax error. Logs `file:line: error:
    // ...`, records the violation(s) and returns true on any mismatch.
    // `base_type` is the declared base (nullptr selects the default int
    // base); unresolvable widths and unsized literals are legal and return
    // false. `has_unknown_digits` reports x/z/? in the literal digits
    // (independent of the token value, where X/Z resolve to 0).
    [[nodiscard]] bool check_enum_value(
        const std::string &file, unsigned int line, const std::string &member_name,
        int64_t literal_size, bool sized_explicit, bool has_unknown_digits,
        const std::shared_ptr<hdl_type> &base_type);

    [[nodiscard]] bool has_violations() const { return !violations_.empty(); }
    [[nodiscard]] const std::vector<semantic_violation> &get_violations() const {
        return violations_;
    }
    // Latest recorded violation; only valid after a check returned true.
    [[nodiscard]] const semantic_violation &last_violation() const { return violations_.back(); }
    void clear() { violations_.clear(); }

    // Process exit code for a violation (< 126, so sv-tests counts it as a
    // tool-detected failure rather than a crash). One mapping for all
    // rules; extend the switch if a rule ever needs its own code.
    static int exit_code_for(semantic_error_code code);

private:
    parse_options opts_;
    std::vector<semantic_violation> violations_;
};

#endif //ANANKE_SV_SEMANTIC_CHECKER_HPP
