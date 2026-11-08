# sv-tests: remaining Ananke failures, categories and fix plans

Source: `sv-tests/out/report/report.csv` and `out/logs/Ananke` from the run of 2026-10-17 17:03 (release build), plus
re-runs of single failing commands with the current debug build. Test sources were read from `tests/` and
`third_party/tools/icarus/ivtest/ivltests`. The older `sv_tests_failures.md` (2026-10-15) is superseded by this file.

Tests that have since been fixed are no longer described here; the summary says how many there are. Everything below is
still open.

Impact = estimated blast radius in this codebase if the fix is done the obvious way (my judgment, not measured):
**Low** = one local check or a few lines in one file, **Medium** = a new check family or data structure inside one
subsystem, **High** = new analysis pass or changes that touch the data model / cache schema / many visitors.

## Summary

| | Count |
|---|---|
| Tests in the 17:03 report | 5104 |
| Failing in that report | 225 (56 non-ivtest, 169 ivtest of 2546) |
| Fixed since (checked by re-running the logged commands with the current debug build) | 32 |
| **Open** | **193** (39 non-ivtest, 154 ivtest) |
| of which: tests that must be rejected and are accepted | 188 |
| of which: valid code rejected by Ananke | 3 (`simple_specify`, `hdlconvertor_std2017_p198`, `p200_2`) |
| of which: harness config, not Ananke | 2 (veer synth tests, bad top-module name) |

Fixed since the run, no longer listed: the three BaseJump files that needed macros from an earlier `--parse` file, ivtest
`br979` (a regression of mine in macro paste spacing), the four preprocessor-directive tests (22.3, 22.9), the nine
constraint and randsequence tests (18.5.x, 18.17.x), 14 constant-expression, genvar and generate-name tests (including `$bits` of a real, rejected as the LRM says), and
`vlg_pr1704726c`, which the generate-name check also catches.

BlackParrot, VeeR EH1 sim, UVM, ariane, scr1 and the other big designs pass. What is left is dominated by negative
tests: programs that must be rejected and that Ananke accepts. Ananke checks declarations (`sv_semantic_checker`,
`sv_constant_checks`) but has no pass that type-checks expressions and statements, no elaboration pass that binds
instances to modules, and no class member table with access rules. Those three gaps explain about three quarters of the open items.

---

## Ranked by value for effort

| # | Category | Tests | Impact | Notes |
|---|---|---|---|---|
| N | Harness top-module names | 2 | Low (runner) | not an Ananke bug |
| M | Parser gaps (false rejects) | 3 | Low-Medium | the only wrong-rejection items |
| G | Call arity, return, void legality | 17 | Medium | needs a callable table |
| J | Ports, gates, UDPs | 16 | Medium | |
| L | Parser leniency | 5 | Medium | grammar tightening in strict only |
| C | Undeclared names | 18 | Medium | per-context name resolution |
| D | Package import / export | 16 | Medium | |
| E | Class semantics | 16 | Medium-High | |
| B | Parameter / defparam checks | 21 | Medium-High | needs instance binding |
| H | Assignment legality, drivers | 20 | High | needs driver analysis |
| A | Assignment type compatibility | 55 (A1+A2) | High | needs an expression type checker |

---

## N. Harness top-module names (2 tests, Low, not Ananke)

`veer-eh1` (fusesoc synth) and `veer-el2` (veer-config synth). Every file parses now; the only failure is
`Top module: veer-eh1_wrapper not found in parsed targets` (exit 55). The sv-tests config gives `:top_module:` as
`veer-el2_wrapper`, but the module is `el2_veer_wrapper` (EL2) or `veer_wrapper` (EH1). Verilator's runner would hit
the same bad name. **Plan:** fix the generator/config in sv-tests, or make `runners/Ananke.py` drop `--parse-top` when
the name contains `-` (not a valid identifier). **Impact:** none on Ananke.

## M. Parser gaps: valid code rejected (3 open, Low-Medium)

| Test | Error | Cause |
|---|---|---|
| `yosys simple_specify` | `no viable alternative at input '(A- =>'` | specify path with polarity: `(A- => B) = (1,2)`, `(A+ => B)`. The grammar's `path_declaration` lacks the optional `polarity_operator` (`+`/`-`) before `=>`/`*>`. Verilog-2005 12.3. |
| `hdlconvertor_std2017_p198` | `'T1' is not declared` (exit 56) | `interface class IntfB #(type T) extends IntfA #(T);` then `funcB` returns `T2`, a typedef inherited from `IntfA`. |
| `hdlconvertor_std2017_p200_2` | `'TYPE' is not declared` (exit 56) | `interface class PutGetIntf #(type TYPE = logic) extends ...; typedef TYPE T;`. |

**Plan:**
- Specify polarity: add `( PLUS | MINUS )?` before the path arrow in `sv2017.g4`, rebuild in Debug, commit the
  regenerated parser. Low (grammar + regenerated files; no visitor change, specify is ignored).
- The two interface-class cases: the unit-scope name check does not put an interface class's `#(type ...)` parameters or
  inherited typedefs into scope for members. Look at `check_constant_use`/`undefined_identifier` scope chain
  building in `sv_visitor.cpp` (the `scope_chain`) and add the class parameter list plus the typedefs of every
  `extends` base. Medium: touches the scope chain used by every check, so add the two files as tests first.

## G. Call arity, return and void legality (17 tests, Medium)

- Wrong argument count: `vlg_br982`, `br982a`, `br982b` (task/function declared with 2 args, called with 1),
  `sv_func_empty_arg_fail1-4` (empty actuals `f(,,,)`, `f(4,5,6,)`, `f(,)` for no-arg functions, empty actual with no
  default).
- Void and statement forms in expressions: `sv_func_void_in_expr_fail`, `vlg_task_in_expr_fail`, `sv_sv_void_cast_fail3`
  (`void'(1+2)`), `sv_function11` and `function_void_return` (void function returns a value).
- Process constraints: `fork_return` (return from fork), `function_fork_invalid` (non-`join_none` fork in a function).
- Others: `sv_sv_port_default13` (constant function with default argument using non-constants), `vlg_pr2848986` (event
  passed to a function input), `vlg_pr3270320`.

**Plan:** the function/task factories know the declaration. Build a per-scope table of declared tasks and functions
(name -> formal list with defaults, void-ness) when the declaration is visited, and check each call against it at the
end of the enclosing module/package (functions can be called before their declaration). The return/fork checks are
statement-context checks: add a context stack (in function, in fork, in void function) to `sv_visitor`. **Impact:**
Medium: new table + call-site hook in the function call path in the factories; calls to functions defined in other
files/packages must be skipped (unknown callee is not an error here), so the table only covers same-scope
callees. The `void` and `fork` checks alone are Low and can ship first (8 tests).

## J. Ports, gates and UDPs (16 tests, Medium)

`vlg_br_gh26` (named connection to a port that does not exist), `vlg_br_gh1173` (trailing comma in port list),
`vlg_br_gh1178a` (duplicate port name), `vlg_porttest3` (`reg` redeclaration of a port that is `inout`),
`vlg_pr3190941` (constant `4'hx` connected to an output), `vlg_module_inout_port_list_def` (default on an `inout`),
`vlg_gate_connect2`, `fsv_br_gh956b`/`vlg_br_gh956b` (`tranif1(y, ctrl, x)` with undeclared `x`), `vlg_br_gh1268a`
(`not` driving a variable), `sv_br_gh1268c` (`tranif0` between variables), `sv_br_gh1222` (strength on a variable continuous
assign, gate output to a real), `vlg_udp_delay_fail` (4 delays on a UDP), `vlg_br_gh1175a/b` (UDP table row with the wrong
number of columns), `sv_program5b` (a module that has a `final` and is instantiated in a program, expected rejection).

**Plan:** three independent groups: (1) declaration-only checks inside a module header and UDP (trailing comma, duplicate
port, inout default, UDP table width and delay count): Low each, all in `sv_semantic_checker`; (2) gate rules (output
must be a net unless SystemVerilog, terminal count, undeclared terminals): Medium, needs the net/variable kind of the
terminal; (3) connection checks against another module (`vlg_br_gh26`, `vlg_pr3190941`, `vlg_porttest3`): needs
instance binding, see category B. **Impact:** Medium overall; group 1 is Low and gives about 6 tests.

## L. Parser leniency: syntax Ananke accepts (5 tests, Medium)

`keyword_ref` (`bit ref;`), yosys `syntax_err01` (`integer [31:0] w;`), `err02` (same in a task port), `err05`
(`input x[2:0];` in a module with no port list), `err10` (`parameter integer [2:0] x=0;`). All pass even with `--strict`.

**Plan:** these are grammar-permissiveness issues. Packed dimensions on `integer`/`int`/`byte`, etc. are not legal
(6.11 only allows them on `bit`, `logic`, `reg` and on packed arrays/structs); `ref` is reserved. Add a strict-only check in
`sv_semantic_checker` for "packed dimension on an integer-type keyword" (the data type context is available in
`RuleData_type`), and make `ref` a plain keyword in the lexer for identifier positions (the grammar likely has `KW_REF`
accepted as an identifier for some legacy reason; find it with `grep -n "KW_REF" grammars/sv2017.g4`). `err05` needs a
port-declaration-in-body check when the header has no ports. **Impact:** Medium: the `ref`-as-identifier change touches the
shared identifier rule, which is the most dangerous one in the grammar (real code uses odd identifiers); keep it
lenient-only, and keep the others strict-only to match the existing lenient/strict split.

## C. Undeclared names (18 tests, Medium)

Missing declarations that Ananke does not report: `vlg_br_gh142` (`b` in a parameter initialiser), `br_ml20150321`
(`undeclared` in `always @*`), `br924` (`func1`), `br961a` (`w` in a function range), `pr2051694`, `pr2528915`,
`pr2039632`, `sv_br_gh699` (undefined function called in a constant function), `sv_br_gh1112a` (`$bits(value)`),
`vlg_hier_ref_error` (`my_task.missing`), `fsv/vlg_sys_func_task_error` (unknown `$this_icarus_call_should_not_exist`,
and `val = $display;`), `vlg_br_gh163` (unknown type name), `vlg_contrib83` (unknown module `MUX_REG_8x8`),
`vlg_pr1833754` (unknown module in a generate), `vlg_pr2823414`, `sv_br_gh823a/b` (`s.c` is not a member of the struct).

**Plan:** the randsequence check (`sv_parsing_helpers::find_random_misuse`) already uses a conservative technique that
fits here: a simple identifier whose every occurrence in the file sits in the checked expressions, never in a declaration,
is undeclared; skip it when the file has a wildcard import or the scope sits in a class with a base class. Reuse that
shape per context. `undefined_identifier` (code 7) exists for some contexts (see `check_constant_use`). Extend it to: procedural
bodies and continuous assignments (declared names in the enclosing scope chain), function return/formal ranges, struct
member selects (the struct shape is in `type_engine`), and unknown system tasks (a fixed list of IEEE 21.x/20.x system
tasks plus tool extensions; unknown ones are rejected only in strict mode, since real code uses vendor tasks).
Unknown module names need the repository's module set, so they only make sense in repository mode, not in
`--parse` of one file (do not do those). **Impact:** Medium: the main risk is false rejects, because identifiers can come
from places Ananke does not model (hierarchical references, bind, `$unit`, `import ...::*` of packages from other
files, macros that expand to declarations). Roll out context by context, with the sv-tests/UVM/ariane runs as the guard.

## D. Package import, export and scope resolution (16 tests, Medium)

`sv_export_fail2-6`, `sv_import_hier_fail1-3` (imported names are not visible hierarchically, `m.x`, `inner.x`),
`sv_ps_function_fail1-3` (`P::f` missing, not a function, a task), `sv_ps_hier_fail1/2` (`P::x` where `x` is not in
`P`), `wildcard_import4/5`, `parametrized_class_invalid_scope_resolution`.

**Plan:** `package_solver` already resolves package contents. Add a per-package visibility model: declared names,
explicitly imported names, wildcard-imported names only after first use (26.3), and `export` entries. Then check
`P::name` references against it and reject `export P::y` where `y` was not imported, reject hierarchical references to
imported names. **Impact:** Medium: self-contained in the package analysis path, but `import`/`export` handling is
used by UVM and every package-heavy design, so keep the new checks "report only references that provably resolve
to nothing" and test against the UVM runs first.

## E. Class semantics (16 tests, Medium-High)

- Access: `prot_from_outside`, `local_from_outside`, `inherited_local_from_inside/outside`, `inherited_prot_from_outside`
  (8.18: `local`/`protected` members accessed from outside, or `local` members of a base class from a subclass).
- `abstract_class_inst` (8.21), `sv_sv_class_method_lt_static1/2` (`task static`/`function static` in a class),
  `sv_enum_in_class_name_coll` (enum literal and typedef with the same name in a class).
- Interface classes (8.26): `illegal_forward_def_implements`, `type_access_implements_invalid`, `name_conflict_unresolved`,
  `parameter_type_conflict_unresolved`, `diamond_relationship_parametrized`, `illegal_implements_parameter`,
  `interface_instantiation`.

**Plan:** `sv_parsing_helpers::find_random_misuse` already builds a per-file class table (name, base, virtual, constraint
names); extend it into a class member table (name, kind, qualifier `local`/`protected`/`static`, owning class, base) next to
`class_tracking`, and resolve member selects `obj.member` through the static type of `obj` for the access checks
(needs a variable-to-type map, which `type_engine` has for class handles). The interface-class rules are
declaration-level: check the `implements` list members (must be interface classes, no forward typedefs, no parameters
resolving to interface classes) and name conflicts between inherited members. **Impact:** Medium-High for the
access checks (needs member lookup through types, affects the visitor that handles `.` selects) but Medium and
independent for the 7 interface-class tests, which are only declaration checks. Do those first.

## B. Parameters, defparam and overrides (21 tests, Medium-High)

- Overrides that name something wrong: `vlg_parameter_override_invalid1-6`, `sv_parameter_override_invalid7/8` (override
  of a `localparam`, of a non-existent parameter, `defparam` to a `localparam` or into a parameter-port-list module),
  `vlg_par_mismatch`, `vlg_pr2877564` (`#(ASDF)` with `ASDF` undeclared).
- Parameters without a value: `sv_parameter_no_default_fail1/2` (no default and not overridden).
- Self reference: `vlg_pr3061015a/b/c` (`parameter PARAMB = PARAMB + 6;`, cycles).
- Others: `vlg_pr498b` (`defparam foo` at the wrong level), `sv_localparam_implicit2/3` (`localparam` in a `#(...)` list
  followed by `parameter` that inherits the `localparam` kind, overridden by instance), `sv_parameter_in_generate2`
  (defparam to a parameter inside a generate block), `sv_sv_type_param_fail1` (type parameter overridden by an
  expression), `specparam_inv` (specparam assigned from a parameter).

**Plan:** two levels. Level 1 needs no instance binding and is Low: self-reference and cycles between parameters
within a module (the parameter solver already detects non-convergence; turn it into an error), missing default with no
override inside the same file, specparam initialised from a parameter. Level 2 needs binding an instance to its module
declaration: when the module is in the same compilation (same `--parse` run or repository index), check named overrides
against its parameter list and `localparam` set, `defparam` targets, and type-parameter vs expression overrides.
**Impact:** Level 1 Low (parameter solver). Level 2 is High on its own, because elaboration mode currently only checks
the top module's presence; a real instance binder touches `Dependency_resolver`, the hierarchy build and the cache
(files would then depend on other files' parameter lists, so the cache schema needs a cross-file dependency).
Stage it: do Level 1, then Level 2 only for modules in the same file, which needs no cache change.

**Careful with defaults:** a parameter declared in a `#(...)` header may carry an invalid default on purpose (BaseJump:
`-1`, `"inv"`, x) to mean "must be overridden", and `sv_semantic_checks.module_parameter_defaults_are_not_final_values`
pins that. `sv_parameter_no_default_fail1/2` are the opposite case (a parameter with no default at all, left
unset). Judge those only from a parameter that has neither a default nor any override in the same compilation, and never
reuse the replication/width rule that trusts body-declared defaults.

## H. Assignment legality, drivers, real operands (20 tests, High)

- Driver rules (6.5, 10.3, 14.3): `variable_multiple_assignments`, `variable_mixed_assignments`, `proc_assignment__bad`
  (procedural assignment to a `wire`), `clocking_block_signals_fail`, `vlg_br_gh1117` (continuous assigns in a generate loop to
  the same net), `vlg_pr2597278b`, `vlg_br_gh1174b`.
- Lvalue forms: `vlg_array_lval_select3b/4b`, `vlg_undef_lval_select3b/3c/4b/4c` (`assign`/`force` on an array element,
  or an x index), `structure-arrays-illegal`, `vlg_pr1735724`, `vlg_br_gh62`.
- Real operands: `real_idx`, `real_bit_select`, `real_edge`, `sv_real_edges`.

**Plan:** the six real-operand and three `assign`/`force` tests are Low-Medium: they need the declared type of the
operand, which exists in the type engine for simple names (`real r; r[1]`, `@(posedge r)`). The driver rules are High:
they require collecting drivers per net/variable across `assign`, `always*`, port connections and generate
expansion, then applying the single-driver rules per kind. That is a new analysis pass over the AST, and it has
to respect generate loops (`gh1117`) and procedural continuous assignment. **Impact:** High for drivers. Take the
operand-type group first (about 10 tests), and do the driver pass only if the project wants lint-level checking.

## A. Assignment type compatibility (55 tests, High)

- `A1` (23): `sv_array_cassign_fail1-11`, `sv_darray_assign_fail1-6`, `sv_queue_assign_fail1-6`. Assigning arrays or queues
  with a different element type or size (`bit [31:0]` to `logic [31:0]`, `int` to `shortint`, `int` to `real`).
- `A2` (32): `sv_enum_compatibility_fail1-8` (implicit int-to-enum assignment, arguments, returns, struct and class
  members), `br_gh130a/gh386c` (`enum_value = 1`), `br_gh265` (`array = 8'd1 << 4`), `class_compat_fail1-3`,
  `class_new_typed_fail1-4`, `class_new_fail1/2`, `class_virt_new_fail`, `queue_vec/string/real/parray_fail` (bound and
  `size()` arguments), `wild_cmp_err/2` (`==?` with real), `unpack_stream_inv`, `enum_numerical_expr_no_cast`,
  `enum_xx_inv_order`, `br_gh716`, `br_gh390a`.

**Plan:** an expression type checker pass. The type engine already computes types for declarations and parameter
values, but nothing computes the type of an arbitrary expression or checks an assignment-like context (assignment,
call argument, return, port connection). Build `type_of(expression)` for the common cases (simple names, selects,
literals, casts, function calls, `new`), then implement the LRM 6.22 rules: assignment compatibility, enum rules (6.19.4:
only the same enum or an explicit cast), class handle rules (8.x: upcast only), array and queue equivalence (7.x).
The `class_new`/`virt_new` and `enum_xx_inv_order` subsets are declaration or single-statement checks and are Low
each, so do them first (about 8 tests). **Impact:** High: it is a new pass across the statement visitors, and real code
leans on lenient conversions (`logic` to enum, array literals), so every rule needs a strict-only or warning
variant first and a run over BlackParrot, VeeR, UVM and ariane before turning it on by default. This category is
also the largest block of failures (55 of 193), so it is worth building the type-of-expression foundation once and
getting H (operand kinds) and parts of G (argument types) for free.

---

## Suggested order

1. Specify polarity and the veer top-module name (N, M): about 3 tests, Low risk.
2. The `void`/`fork` half of G, and the declaration-only half of J and E (interface classes): about 25 tests, Low-Medium.
3. The Level 1 half of B (self-referencing parameters): about 5 tests, confined to the parameter solver.
4. L, C and D, each one context at a time, guarded by the large-design runs.
5. Build the expression-type foundation, then A, the operand half of H, the argument half of G and the scalar-select
   case in I (about 70 tests).
6. Instance binding for B Level 2, the recursion case in I and J group 3, and the driver pass in H, last: they are the
   only High items that need cache or hierarchy changes.

## Caveats

- Counts come from `report.csv`; the category assignment is mine and was made from reading the test sources, not from
  running each test against a fix. A few tests (`pr3270320`, `pr2848986`, `pr1833024`, `pr1988302b`) were placed by best
  guess and may move.
- The "fixed" count comes from re-running the 225 originally failing commands with the current debug build, not from a
  new full sv-tests run. The no-regression check on tests that passed at 17:03 is the user's own run; this file
  does not repeat it.
- `ivtest_failures.md` from the previous session is no longer in the working tree; this file covers the ivtest failures too.
