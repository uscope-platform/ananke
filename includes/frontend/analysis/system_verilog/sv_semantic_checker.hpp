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

#include <map>
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
    end_label_mismatch = 10,
    invalid_type_use = 11,
    invalid_constant_expression = 12,
    timing_check_missing_argument = 13,
    bare_generate_block = 14,
    invalid_constraint = 15,
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
        typedefs_.clear();
        class_names_.clear();
        array_dims_.clear();
        names_.clear();
        generate_labels_.clear();
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

    // IEEE 1800-2017 §9.3.4 / §23.2 / §13.3: a closing label (`end : name`,
    // `endmodule : name`, ...) shall match the name of the block or
    // declaration it closes, and an unnamed block cannot carry one. Logs
    // `file:line: error: ...`, records the violation and returns true on a
    // mismatch. Constructs without a closing label are legal.
    [[nodiscard]] bool check_end_label(const std::string &file, const end_label_info &info);

    // IEEE 1800-2017 §31.9.1: `$width` takes a threshold after the limit.
    // Verilog-2001 (and simulators) accept it without one, so this is an
    // error only with --strict. Returns true when it records a violation.
    // IEEE 1800-2017 §27: a `begin ... end` block at module scope is only
    // legal inside a generate construct. Simulators/Verilator accept it bare
    // (BlackParrot's declare_csr macro), so this is an error only with
    // --strict. Returns true when it records a violation.
    [[nodiscard]] bool check_bare_generate_block(const std::string &file, unsigned int line);

    // IEEE 1800-2017 18.5 / 18.17: findings of the constraint and randsequence scan of a source
    // text. Logs each as `file:line: error: ...`, records the violation and returns true when any.
    [[nodiscard]] bool check_random_constructs(const std::string &file, const std::vector<random_finding> &findings);

    [[nodiscard]] bool check_width_threshold(const std::string &file, unsigned int line, bool has_threshold);

    // Declared types, for the type-shape rules below. Names are tracked
    // scope-insensitively like the enum tracking above.
    void note_class_name(const std::string &name) { class_names_.insert(name); }
    // IEEE 1800-2017 §6.18: a type may not (transitively) contain or alias
    // itself by value. Records the typedef and returns true on a cycle.
    [[nodiscard]] bool check_typedef(const std::string &file, const typedef_shape &shape);
    // IEEE 1800-2017 §6.19: an enum base is an integral type: not an array,
    // queue, enum, struct, real or string, not a typedef of itself, and a
    // typedef with packed dimensions or an atom type takes no further
    // dimension. Member values must be constant, in range for the base and
    // unique. Returns true if any violation fired.
    [[nodiscard]] bool check_enum(const std::string &file, const enum_info &info);
    // IEEE 1800-2017 §7.2.1: members of a packed struct or union are packed
    // integral types: no unpacked/dynamic/queue dimensions, default values,
    // real, string, chandle, event or unpacked aggregate types.
    [[nodiscard]] bool check_packed_members(
        const std::string &file, const std::string &aggregate,
        const std::vector<packed_member_info> &members);
    // IEEE 1800-2017 §6.7: a net has a 4-state integral data type: not
    // string, class, queue or dynamic array. `type_name` is a named type.
    [[nodiscard]] bool check_net_type(
        const std::string &file, unsigned int line, const std::string &net_name,
        bool is_string, const std::string &type_name, bool has_queue_dimension);

    // IEEE 1800-2017 §10.9.1: a positional assignment pattern has exactly one
    // element per array element / struct member, and cannot target a
    // scalar. `expected` is -1 when unknown (not checked); `scalar` marks a
    // declared non-aggregate. Returns true on a violation.
    [[nodiscard]] bool check_assignment_pattern(
        const std::string &file, unsigned int line, const std::string &name, int elements, int expected,
        bool scalar);
    // IEEE 1800-2017 §12.7.3: a foreach loop has at most one index per array
    // dimension. The visitor reports declared dimension counts; a name that
    // is not tracked (typedef'd, redeclared in another form) is not checked.
    // Declared in `scope` (an opaque id for the enclosing scope construct).
    void note_array_dims(const void *scope, const std::string &name, int dims) {
        array_dims_[name].emplace_back(scope, dims);
    }
    void forget_array(const std::string &name) { array_dims_.erase(name); }
    // `scopes` are the scope constructs enclosing the loop, innermost first;
    // only a declaration found in one of them counts.
    [[nodiscard]] bool check_foreach(
        const std::string &file, unsigned int line, const std::string &name, int loop_variables,
        const std::vector<const void *> &scopes);

    // IEEE 1800-2017 §6.21: automatic variables cannot be written by
    // nonblocking assignments or used in procedural continuous assignments.
    // Logs and records one violation per misuse; returns true if any.
    [[nodiscard]] bool check_automatic_use(const std::string &file, const std::vector<automatic_misuse> &misuses);

    // ---- Constant expressions (IEEE 1800-2017 §11.2.1, §11.4.12, §6.9) --------------------------------
    // Names are recorded per scope construct (an opaque id) as the visitor meets their declarations, so a
    // lookup walks `chain` (innermost scope first) and a name that was never recorded is simply unknown.
    using scope_chain = std::vector<const void *>;
    // parameter / localparam / genvar / enum member. `value_text` may be empty; an evaluable value is kept
    // so later dimensions, replication counts and casts can use it.
    // `overridable` marks a module/interface/program parameter: its default is not necessarily its value (an
    // instance can override it). Checks that opt in (x/z and negative counts, zero part-select widths) still
    // read the default of a parameter declared in the module body, which legacy top-level modules rely on,
    // but never of one in the `#(...)` header (`header`), where an invalid default means "must be overridden".
    void declare_constant_name(
        const scope_chain &chain, const std::string &name, const std::string &value_text, bool is_real,
        bool overridable = false, bool header = false);
    // Variable, net or port. `plain_vector` is true for a fixed-size integral object (the only kind whose
    // part-selects need constant bounds; queues and dynamic arrays slice with variables).
    void declare_variable_name(const scope_chain &chain, const std::string &name, bool plain_vector);
    // `text` must be a constant expression: no identifier that resolves to a variable, net or port.
    [[nodiscard]] bool check_constant_use(
        const std::string &file, unsigned int line, const std::string &text, const scope_chain &chain,
        const std::string &what);
    // A declared dimension `[inner]`. Packed ones must be `[msb:lsb]`; bounds must be constant and not x/z.
    // `judge_values` is false inside conditional generate branches: a width that is zero or negative there may
    // belong to a branch that is never elaborated, so only structural problems are reported.
    [[nodiscard]] bool check_dimension(
        const std::string &file, unsigned int line, const std::string &inner, bool packed, const scope_chain &chain,
        bool judge_values = true);
    // A part-select `object[a:b]` / `object[a+:w]` / `object[a-:w]`.
    [[nodiscard]] bool check_part_select(
        const std::string &file, unsigned int line, const std::string &object, const std::string &separator,
        const std::string &first, const std::string &second, const scope_chain &chain, bool judge_values = true);
    // `{count{...}}`: count may not be x/z or negative; zero only inside a concatenation with other operands.
    [[nodiscard]] bool check_replication(
        const std::string &file, unsigned int line, const std::string &count, bool zero_allowed,
        const scope_chain &chain, bool judge_values = true);
    // `N'(expr)`: N must be a positive, defined constant.
    [[nodiscard]] bool check_size_cast(
        const std::string &file, unsigned int line, const std::string &width, const scope_chain &chain,
        bool judge_values = true);
    // Real operands cannot be concatenated.
    [[nodiscard]] bool check_concatenation_operands(
        const std::string &file, unsigned int line, const std::vector<std::string> &operands,
        const scope_chain &chain);
    // IEEE 1800-2017 §20.6.2 / §20.5: `$bits` takes a data type or expression (not a real value, not a
    // module name) and `$signed`/`$unsigned` take an integral expression. `args` are the argument texts;
    // `enclosing_unit` is the module/interface/program being declared.
    [[nodiscard]] bool check_system_call(
        const std::string &file, unsigned int line, const std::string &name, const std::vector<std::string> &args,
        const std::string &enclosing_unit, const scope_chain &chain);
    // IEEE 1800-2017 §27.4: `for (genvar i = init; cond; i = step)`. All three expressions are constant and
    // defined, the iteration assigns the initialised genvar, the initial value does not read it, the
    // variable really is a genvar (not an `integer`) and an enclosing loop does not use the same one.
    struct genvar_loop {
        std::string init_var, init_expr, condition, step_var, step_expr;
        bool declares_genvar = false;     // `for (genvar i = ...`
        bool nested_reuse = false;        // an enclosing generate loop uses the same genvar
    };
    [[nodiscard]] bool check_genvar_loop(
        const std::string &file, unsigned int line, const genvar_loop &loop, const scope_chain &chain);
    // IEEE 1800-2017 §27.6: a generate block name is unique in its scope and cannot reuse a variable's name.
    // `conditional` is true inside an if/case generate, where sibling branches may share a name.
    [[nodiscard]] bool check_generate_block_name(
        const std::string &file, unsigned int line, const std::string &label, const scope_chain &chain,
        bool conditional);
    // Continuous assignment targets: bit/array indices must be constant.
    [[nodiscard]] bool check_net_lvalue(
        const std::string &file, unsigned int line, const std::string &lvalue, const scope_chain &chain);

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
    std::map<std::string, typedef_shape> typedefs_;
    std::set<std::string> class_names_;
    std::map<std::string, std::vector<std::pair<const void *, int>>> array_dims_;
    struct constant_value {
        bool known = false;
        bool xz = false;
        bool real = false;
        long long value = 0;
    };
    struct name_entry {
        bool variable = false;
        bool plain_vector = false;
        bool overridable = false;
        bool header = false;
        constant_value value;
    };
    std::set<std::pair<const void *, std::string>> generate_labels_;
    std::map<std::pair<const void *, std::string>, name_entry> names_;
    [[nodiscard]] const name_entry *lookup_name(const scope_chain &chain, const std::string &name) const;
    // With `trust_defaults` a parameter that an instance may override still resolves to its default.
    [[nodiscard]] constant_value evaluate_constant(
        const std::string &text, const scope_chain &chain, bool trust_defaults = false) const;
    std::set<std::string> unit_declared_;
    std::set<std::string> unit_resolved_;
    std::vector<std::pair<std::string, unsigned int>> unit_forwards_;
    bool unit_wildcard_import_ = false;
    std::vector<unresolved_unit_name> pending_unit_names_;
};

#endif //ANANKE_SV_SEMANTIC_CHECKER_HPP
