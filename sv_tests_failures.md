# sv-tests (non-ivtest) failures, Ananke runner

Source: `sv-tests/out/logs/Ananke` and `out/report/report.csv` from the run of 2026-10-15 ~18:30 (the build with the
constant-expression checks). ivtest is covered separately in `ivtest_failures.md`. Failure = (`should_fail: 0` and rc != 0)
or (`should_fail: 1` and rc == 0).

| Result | Count |
|---|---|
| Tests (excluding ivtest, sanity) | 2555 recorded in the report |
| Passing | 2493 |
| Failing in the report | 62 (48 false accepts, 14 false rejects) |
| Empty logs, no result recorded | 9 (4 chapter tests, 3 fusesoc, 2 veer-config) |

Everything else passes, including all of UVM (`uvm_classes_*`, `uvm-examples`), ariane, scr1, fx68k, rggen, rsd, tnoc,
projf-explore, utd-sv and 357 of 360 basejump tests.

**Regression found and fixed while writing this.** 2 of the 14 false rejects (`9.3.5--statement_labels_par/seq`) were caused
by the new end-label check: `name: begin ... end : name` names the block with a statement label, which the check did not
look at. Fixed in `sv_parsing_helpers.cpp` (with a unit test); both tests now return rc 0. The numbers above are the
run's, so they still count them; the lists below treat them as fixed.

The 9 empty logs have no result in the report (the log files are 0 bytes; they were being written when the run was
read, or the runner failed before logging). I ran the 4 chapter tests by hand (see group F) and approximated the 5
core tests (see group H).

---

## Part 1 - False rejects: valid code Ananke refuses (14 in the report, 2 fixed, 12 open, + 5 unrecorded)

### H1. Macro defined in an earlier `--parse` file is invisible to later files (3 basejump, likely 5 more)

`bsg_misc_..._bsg_decode_thermometer`, `bsg_dmc_..._bsg_dmc_sys_cfg_gen`, `bsg_noc_..._bsg_wormhole_broadcast`.
Error: `Undefined macro(s) BSG_ABSTRACT_MODULE` (or `BSG_INV_PARAM`). The macros come from `bsg_defines.sv`, listed before the
failing file on the command line, but the failing files do not `` `include`` it (357 other basejump files do). Reproduced
with `ananke --parse bsg_defines.sv bsg_decode_thermometer.sv` and with a 2-line file: each `--parse` target only sees macros
from its own includes. SystemVerilog compiles the files of one run as a single compilation unit, so a macro defined in
an earlier file stays defined for later ones (IEEE 1800-2017 22.5.1).
The same gap is the first error of the unrecorded cores: `veer-eh1-sim/synth` (`RV_BHT_GHR_RANGE`, ...) and
`veer-el2_sim/synth` (`ASSERT_STATIC_IN_PACKAGE`).
**Fix:** in multi-`--parse` mode feed the macros harvested from files earlier in the list into each later file (the
preprocessor already harvests `locally_defined` per file). Medium.

### H2. `ifdef` on a macro argument inside a macro body (6 black-parrot)

`bp_default`, `bp_unicore`, `bp_multicore_1`, `bp_multicore_4`, `bp_multicore_1_cce_ucode`, `bp_multicore_4_cce_ucode_cfg`.
Error: `Undefined macro(s) BP_ACACHE_ASSOC, BP_ACACHE_BLOCK_WIDTH, ...`. The cause is

```
`define bp_aviary_define_override(parameter_mp, define_mp, default_cfg_mp) \
  `ifdef define_mp  parameter_mp: `define_mp  `else  parameter_mp: default_cfg_mp.``parameter_mp``  `endif
```

After substituting the argument, the `` `ifdef`` must be evaluated before the `` `define_mp`` use in the taken branch is
expanded. Ananke expands both branches, so the undefined macro in the untaken branch is reported. Reproduced with a 9-line
file. **Fix:** after argument substitution run the conditional-directive pass on the expanded text, then expand the macros
that remain. Medium.

### H3. Type parameter of an interface class not in scope for the unit-scope name check (2 hdlconv_std2017)

`p198.sv` (`'T1' is not declared`) and `p200_2.sv` (`'TYPE' is not declared`):
`interface class IntfA #(type T1 = logic); typedef T1[1:0] T2; ...` The check for names used at compilation-unit scope
treats class, function and task bodies as nested scopes but not `interface class`, so its own `#(type ...)` parameters
look undeclared. **Fix:** treat `interface_class_declaration` like `class_declaration` in `collect_unit_nodes`. Small.

### H4. Specify-path polarity not in the grammar (1 yosys)

`simple_specify`: `(A- => B) = (1,2);` and `(A+ => B) = ...` fail with `no viable alternative at input '(A- =>'`.
`specify_input_terminal_descriptor` has no optional `+`/`-` polarity before `=>`/`*>`. Needs a grammar change (build in
Debug and commit the regenerated parser). Small to medium.

### H5. `verilator_config` directive in `.vlt` files (ibex-sim)

Approximation, since the log is empty: the file list contains Verilator `.vlt` files that start with
`` `verilator_config``, reported as `Undefined macro(s) verilator_config`. Either skip `.vlt` files or accept the directive.
Small.

### Fixed in this session
`9.3.5--statement_labels_par`, `9.3.5--statement_labels_seq` (see above).

---

## Part 2 - False accepts: invalid code Ananke accepts (48 in the report, + 4 unrecorded chapter tests)

Each is a missing semantic check. Sizes: S small, M medium, L large.

### A. Illegal assignments to nets and variables (4, M)
- `10.3--proc-assignment--bad`: `wire w; initial w = #10 a & b;` procedural assignment to a net.
- `6.5--variable_mixed_assignments`: `assign v = 12;` plus `always ... v <= ~v;` on the same variable.
- `6.5--variable_multiple_assignments`: two continuous assignments to one variable.
- `14.3--clocking-block-signals-error`: `output b` (a net) assigned with `<=` in `always_ff`.
**Check:** classify each target as net or variable and count drivers per variable. The declaration tables added for the
constant checks (name -> kind, per scope) are the starting point.

### B. Real-typed operands (3, M)
`6.12--real_bit_select` (`a[2]` on a real), `6.12--real_bit_select_idx` (`b[a]` with real `a`), `6.12--real_edge`
(`posedge a` on a real). The same family is in ivtest (`bitsel_real_idx`, `real_edges`, ...), which are now fixed only for
the cases that are `ivtest` part-selects. Needs the declared type of the operand.

### C. Class and interface-class rules (9, M to L)
`8.21--abstract_class_inst` (`new` on a virtual class), `8.25.1--parametrized_class_invalid_scope_resolution`
(`par_cls::b` on a parameterized class without specialization), `8.26.3--type_access_implements_invalid` (typedefs are not
inherited by `implements`), `8.26.4--illegal_forward_def_implements`, `8.26.4--illegal_implements_parameter`,
`8.26.5--invalid_interface_instantiation` (`new` on an interface class), `8.26.6.1--name_conflict_unresolved`,
`8.26.6.2--parameter_type_conflict_unresolved`, `8.26.6.3--diamond_relationship_parametrized`.
Needs a class hierarchy table (kind: virtual/interface, bases, members, typedefs). The class tracking
(`sv_class_tracking`) is the base. The ivtest `sv_class_*` group needs the same table.

### D. Encapsulation: `local` and `protected` access (5, M)
`prot_from_outside`, `local_from_outside`, `inherited_local_from_outside`, `inherited_prot_from_outside`,
`inherited_local_from_inside` (suite `encapsulation`, 8.18). Member access `obj.m` must resolve `obj`'s class and check the
qualifier; `local` is also invisible in subclasses. Needs variable-to-class resolution plus the class table from group C.

### E. Randomization and constraint rules (5, S to M)
`18.5.1--explicit-external-constraint_1` (`extern constraint c;` never defined), `18.5.2--pure-constraint_2` (pure
constraint in a virtual class not implemented by a concrete subclass), `18.5.10--variable-ordering_1` (`solve ... before`
with a `randc` variable), `18.5.14--soft-constraints_2` (`soft` on `randc`), `18.5.4--distribution_2` (`dist` on `randc`).
The `randc` ones are local: look up the declared qualifier of the variable named in the constraint. Small.

### F. Failures only visible at run time or in assertion evaluation (4, out of scope for a static tool)
Unrecorded in the report (empty logs). I ran them locally: all four return rc 0.
- `11.9--tagged_union_member_access_inv`: "accessing wrong member should result in run-time error".
- `16.10--property-local-var-fail`, `16.10--sequence-local-var-fail`: "pipeline increments by 4 but the property expects 3".
- `16.15--property-disable-iff-fail`: "disable iff uses wrong reset polarity".
These expect a failing simulation or assertion, so they cannot be detected by static checks.

### G. `randsequence` names (4, M)
`18.17.2--if-else-production-statements_0_fail` / `_2_fail`, `18.17.3--case-production-statements_0_fail` (undeclared
variable `switch` in the condition), `18.17.6--aborting-productions-break-and-return_2_fail` (production name typo
`secondi`). Needs name resolution inside `randsequence` bodies (productions and function-scope variables). It is part of
the "undeclared identifiers" family in `ivtest_failures.md`.

### I. Task/function and fork rules (3, S)
`13.4.1--function-void-return` (`return a + b;` in a `void` function; same family as ivtest `function11`),
`13.4.4--fork-invalid` (`join_any` in a function; only `join_none` is allowed), `9.3.3--fork_return` (`return` inside a
`fork`). All three are structural checks on the enclosing function and block kinds.

### J. Compiler directives (4, S)
`22.3--resetall_illegal` (`` `resetall`` inside a design element), `22.9--unconnected_drive-invalid-1` (no argument),
`-invalid-2` (`pull2`), `-invalid-3` (`` `nounconnected_drive pull0`` takes no argument). The preprocessor skips these
directives wholesale (`skipped_directive` list); give them argument and placement checks. Small.

### K. Enum, array, stream and width rules (6, S to M)
- `5.10-structure-arrays-illegal`: `ms_t ms[1:0] = '{0, 0, 1, 1};` (an array of structs needs one pattern per element).
- `6.19--enum_xx_inv_order`: an unassigned enum name after one assigned `x`/`z` (`{b = {32{1'bx}}, c}`).
- `6.19.4--enum_numerical_expr_no_cast`: `val += 1;` on an enum variable (the existing enum assignment check only
  handles a bare literal on `=`).
- `6.20.5--specparam_inv`: a `parameter` initialised from a `specparam`.
- `7/arrays/packed/variable-slice-zero`: `arr_b[4+:c]` with `parameter c = 0`. Left failing on purpose: a module
  parameter's default is not its final value (see the constant-expression notes).
- `11.4.14.3--unpack_stream_inv`: stream wider than the target (`int d = {<<{a, b, c}}`). The runner passes
  `-Werror-WIDTH`, so this is a width lint, not an LRM error.

### L. `ref` accepted as an identifier (1, S)
`5.6.2--keyword_ref`: `bit ref;`. The grammar lists `KW_REF` in the `identifier` rule so that code which uses `ref` as a
name still parses. A strict mode (`--strict`) could stop doing that. Small, but it changes lenient behaviour.

### M. Declaration syntax rules (4 yosys, S)
- `errors_syntax_err01`: `integer [31:0] w;`, `errors_syntax_err02`: `input integer [3:0] x`,
  `errors_syntax_err10`: `parameter integer [2:0] x = 0;`: packed dimensions are not allowed on `integer` (or the other
  atom types `int`, `byte`, `shortint`, `longint`, `time`). One check on the data type.
- `errors_syntax_err05`: `input x[2:0];` in a module body: a non-ANSI port declaration cannot have unpacked dimensions.
Same area as the non-ANSI port checks already implemented.

---

## Summary by effort

| Group | Tests | Effort |
|---|---|---|
| H1 macro visibility across `--parse` files | 3 (+5 unrecorded) | Medium |
| H2 `ifdef` inside macro bodies | 6 | Medium |
| H3 interface-class type parameters | 2 | Small |
| H4 specify polarity (grammar) | 1 | Small-Medium |
| H5 `.vlt` files / `verilator_config` | 1 (unrecorded) | Small |
| M declaration syntax (atom types, port arrays) | 4 | Small |
| J directive argument checks | 4 | Small |
| I function/fork rules | 3 | Small |
| E `randc` constraint rules | 5 | Small-Medium |
| L `ref` as identifier | 1 | Small |
| K enum/array/stream/width | 6 | Small-Medium |
| A net/variable assignment legality | 4 | Medium |
| B real operands | 3 | Medium |
| G `randsequence` names | 4 | Medium |
| C class / interface-class rules | 9 | Medium-Large |
| D encapsulation | 5 | Medium |
| F run-time / assertion expectations | 4 | Not static |

**Suggested order.** First the false rejects, because they fail valid designs: H1, H2, H3 (11 tests, 3 small fixes
and 2 medium ones). Then M, J, I (11 small checks), then E, K, L. A, B, G, C, D need type and name resolution and are
better done together with the ivtest groups that need the same machinery.

## Method and caveats
- Counts come from each log's `rc` and `should_fail`, cross-checked against `report.csv` (62 failures match exactly).
- Sources of the failing tests were read to classify them; effort sizes are estimates.
- Group H5 and the veer/fusesoc remarks are from my own approximation of the runner command for those tests (their logs
  are empty), not from a recorded result.
- I did not re-run the full suite after the statement-label fix; only the two affected tests were run (rc 0).
