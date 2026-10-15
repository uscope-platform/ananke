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
#include <cstdint>
#include <set>
#include <string_view>
#include <string>
#include <utility>
#include <vector>

#include "data_model/hdl_file.hpp"
#include "frontend/analysis/system_verilog/sv_scope_declaration.hpp"
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
    enum_assignment_type_mismatch = 5,
    timescale_precision_exceeds_unit = 6,
    undefined_identifier = 7,
    unresolved_forward_typedef = 8,
    conflicting_declaration = 9,
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
        enum_typedefs_.clear();
        enum_variables_.clear();
        begin_unit_scope();
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

    // IEEE 1800-2017 §6.19.3: enum types are strongly typed; a variable of
    // enum type may only be assigned a member of the same enum or an
    // expression cast to it, never a plain integral literal. The checker
    // owns the name tracking: the visitor reports typedefs, variable
    // declarations and assignments as written, and this class decides.
    // Names are tracked scope-insensitively and dropped when redeclared
    // as non-enum, so a miss is possible but a false positive is not.
    void note_typedef(const std::string &name, bool is_enum);
    // `type_name` is the spelled type of a named-type declaration (empty
    // for built-in/anonymous types); `is_anonymous_enum` marks `enum {..} v`.
    void note_variable(
        const std::string &name, const std::string &type_name, bool is_anonymous_enum,
        bool has_dimensions);
    // Plain `lhs = rhs` blocking assignment. Logs `file:line: error: ...`,
    // records the violation and returns true when `lhs` is an enum variable
    // and `rhs_text` is a bare numeric literal.
    [[nodiscard]] bool check_assignment(
        const std::string &file, unsigned int line, const std::string &lhs,
        const std::string &rhs_text);

    // IEEE 1800-2017 §22.7: the `timescale time_precision shall be at least
    // as precise as the time_unit, i.e. its period may not exceed the
    // unit's (equal is legal). Both values are in femtoseconds; `directive`
    // is the spelled directive for the message. Logs `file:line: error:
    // ...`, records the violation and returns true when precision > unit.
    [[nodiscard]] bool check_timescale(
        const std::string &file, unsigned int line, uint64_t unit_fs, uint64_t precision_fs,
        std::string_view directive);

    // IEEE 1800-2017 §6.18 / §3.12: names used by compilation-unit scope
    // declarations (typedefs, packed dimensions) must be declared earlier in
    // that scope, and a forward typedef must resolve to a data type in the
    // same scope. The visitor reports the unit-scope declarations in source
    // order. A wildcard import disables the use check, since the name may
    // come from the package.
    void begin_unit_scope();
    void declare_unit_name(const std::string &name);
    // `typedef name;`: usable immediately, but still owed a real declaration.
    void declare_unit_forward(const std::string &name, unsigned int line);
    void note_unit_wildcard_import() { unit_wildcard_import_ = true; }
    // Logs `file:line: error: ...`, records the violation and returns true
    // when `name` was never declared in the unit scope.
    [[nodiscard]] bool check_unit_reference(
        const std::string &file, unsigned int line, const std::string &name);
    // End of the source text: returns true if any forward typedef was left
    // unresolved (one violation per name).
    [[nodiscard]] bool finish_unit_scope(const std::string &file);
    // With parse_options::defer_unit_name_errors the two checks above record
    // here instead of failing, for the repository-wide judgement.
    [[nodiscard]] const std::vector<unresolved_unit_name> &get_pending_unit_names() const {
        return pending_unit_names_;
    }

    // IEEE 1800-2017 §23.2.2.1 / §13.3: in one module, task or function
    // scope a name may be declared once as a port and once as a net or
    // variable. A second port declaration, a second net/variable declaration,
    // a net/variable declaration for a port that already has a type, a
    // non-vector type or a different packed range for a port declared with
    // a range all conflict. Logs `file:line: error: ...` per conflicting
    // name, records the violations and returns true if any fired. Ranges are
    // only compared when both are plain literal ranges (`[7:0]`): anything
    // else needs elaboration, so it is given the benefit of the doubt.
    // In a task or function (`is_tf_scope`) a port without a range may take
    // its range from the later declaration (`input n; reg [7:0] n;`); in a
    // module that is a mismatch.
    [[nodiscard]] bool check_scope_declarations(
        const std::string &file, const std::vector<scope_declaration> &declarations, bool is_tf_scope = false);

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
    std::set<std::string> enum_typedefs_;
    std::set<std::string> enum_variables_;
    std::set<std::string> unit_declared_;
    std::set<std::string> unit_resolved_;
    std::vector<std::pair<std::string, unsigned int>> unit_forwards_;
    bool unit_wildcard_import_ = false;
    std::vector<unresolved_unit_name> pending_unit_names_;
};

#endif //ANANKE_SV_SEMANTIC_CHECKER_HPP
