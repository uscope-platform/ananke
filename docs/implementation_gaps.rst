Implementation Gaps and Development Roadmap
============================================

.. contents:: Table of Contents
   :depth: 2

This document evaluates the current state of the Ananke FPGA build system,
identifies gaps in the implementation, and categorizes possible future
development into three buckets: **cheap wins**, **mid-sized projects**, and
**heavy refactors**.

All file references use the form ``path:line``. Where a feature spans several
locations the most representative reference is given.


Current State of the Implementation
-----------------------------------

Ananke is a C++ FPGA HDL build system (roughly 14K lines of application code on
top of ANTLR-generated parsers). The overall pipeline is::

    Repository_walker
        -> sv_analyzer / vhdl_analyzer        (preprocess + parse + visit)
        -> data_store                          (hash-based per-file cache)
        -> HDL_ast_builder_v2                  (parameter resolution, elaboration)
        -> analysis passes                     (port_solution, processor_detection)
        -> control_bus_analysis                (AXI-Lite address map extraction)
        -> proxy_bus_analysis                  (proxied subsystem ASTs)
        -> data_acquisition_analysis           (AXI-Stream sink->source backtrace)
        -> Backend                             (Vivado / Radiant / uplatform JSON)

Mature areas
~~~~~~~~~~~~

* **SystemVerilog parameter/expression evaluation engine** -- the deepest and
  best-tested subsystem: wide integers, streaming operators, structs/enums/
  unions, type parameters, system tasks (``$clog2``, ``$bits``, ``$typename``,
  math tasks, ...), ``defparam``. This is also where the most recent
  development effort has been concentrated (see ``git log``).
* **SystemVerilog preprocessor** -- macros (incl. function-like with defaults),
  ``ifdef/ifndef/elsif/else/endif``, includes with source mapping,
  stringification and token pasting.
* **AST builder and parameter solver** -- generate loops/conditionals,
  hierarchical instance elaboration, parameter override propagation.
* **Address map extraction (AXI-Lite control bus)** and the uplatform JSON
  generators (peripheral and application definitions).
* **Xilinx Vivado backend** -- the complete, exercised path (project makefile,
  batch simulation flow, standalone synthesis flow).

Immature areas
~~~~~~~~~~~~~~

* **VHDL frontend** -- barely extracts anything (see :ref:`vhdl-frontend`).
* **Procedural and net-level SystemVerilog** -- ``always``/``assign``/
  ``initial``/``case`` and module-level signal declarations are parsed by the
  grammar but silently discarded by the walker.
* **Soft-core ROM/program handling** -- the application definition leaves the
  ``programs`` and ``default_program`` fields permanently empty.


Cheap Wins
----------

Small, well-bounded fixes. Each item is hours to a few days of work and most
are latent bugs or dead code.

Unimplemented design-synthesis flow
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``--S`` / ``opts.synth_design`` is declared and wired to the CLI
(:file:`src/main.cpp:39`, :file:`includes/ananke.hpp:59`) but the feature it
announces -- running synthesis on the design -- is not implemented yet: the
flag is never referenced in :file:`src/ananke.cpp`. Most of the building blocks
already exist: the Xilinx backend can emit a standalone synthesis script
(:file:`src/Backend/Xilinx/xilinx_project_generator.cpp:208-295`), and the
Lattice backend has ``generate_synth_script`` as well
(:file:`src/Backend/Lattice/lattice_project_generator.cpp:52-60`), but neither
is reachable through the main flow. Implementing ``--S`` is wiring the flag to
the existing synth-script generators and driving the corresponding
``Toolchain_manager``.

Unary and reduction operators are dropped
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The visitor only handles ``+`` / ``-`` unary operators
(:file:`src/frontend/analysis/system_verilog/sv_visitor.cpp:806-811`) while
the grammar and the evaluation engine already support ``!``, ``~``, ``&``,
``~&``, ``|``, ``~|``, ``^``, ``~^`` (:file:`grammars/sv2017.g4:94-110`).
Consequently ``parameter P = ~X`` loses the negation and vector reduction
operators produce wrong results. Cheap, high-impact correctness fix.

Test-suite hygiene
~~~~~~~~~~~~~~~~~~

* :file:`Testing/data_model/settings_store_test.cpp` is an empty stub
  (a single ``// TODO: TEST SETTINGS STORE??`` comment).
* Four stale test files remain in :file:`Testing/analysis/`
  (``parameter_extraction.cpp``, ``parameter_processing.cpp``,
  ``system_task_tests.cpp``, ``type_parameter_extraction.cpp``) that are no
  longer registered in ``ANANKE_TESTS`` and can confuse readers.
* There is no CTest registration (no ``enable_testing()`` / ``add_test()``);
  tests run as a single executable. Registering them is a trivial win.
* The documentation analyzer aborts the whole process with ``exit(2)`` on
  malformed doc JSON (:file:`src/frontend/analysis/system_verilog/documentation_analyzer.cpp:37-40`).

Documentation drift
~~~~~~~~~~~~~~~~~~~

* The README depfile sample is out of sync with the real schema: it uses
  ``product_include`` / ``product_type`` while the code uses
  ``include_products`` / ``products_type``
  (:file:`includes/data_model/Depfile/depfile_schema.hpp:90-97`), contains a
  ``sym_modules`` typo, predates the ``board`` and ``bus`` sections, and still
  marks ``target_part`` as "reserved for future development" although the code
  consumes it.

Preprocessor hygiene
~~~~~~~~~~~~~~~~~~~~

* No include-cycle detection and no diagnostic for an unbalanced ``endif``
  (:file:`src/frontend/analysis/system_verilog/preprocessor/conditional_solver.cpp:20-24`).
* Macro recursion is silently capped at 1000 iterations instead of reporting
  genuine infinite recursion (:file:`src/frontend/analysis/system_verilog/preprocessor/macro_processor.cpp:34`).
* ``"..."`` include-not-found is a warning while ``<...>`` include-not-found
  throws (:file:`src/frontend/analysis/system_verilog/preprocessor/sv_preprocessor.cpp:147-158`).


Mid-Sized Projects
------------------

Meaningful feature work. Roughly one to four weeks each.

.. _vhdl-frontend:

VHDL frontend completion
~~~~~~~~~~~~~~~~~~~~~~~~

The VHDL analyzer is wired into the main flow (walker dispatch, data store, AST
builder all accept VHDL entities) but extracts almost nothing: only entity
names and component instantiations
(:file:`src/frontend/analysis/vhdl/vhdl_visitor.cpp:26-63`). Missing entirely:

* Ports (``port_clause`` is never visited).
* Generics / parameters (``generic_clause`` is never visited).
* Types and typedefs.
* Documentation comments (no VHDL doc analyzer exists).
* Generate statements (``for``/``if``/``case`` generate).
* Packages and configurations.

The intended strategy is *not* to build a parallel VHDL frontend, but to fit
VHDL into the existing SystemVerilog machinery. This is enabled by a
deliberate design decision in the architecture: the factories
(``expressions_factory``, ``HDL_functions_factory``, ``HDL_modules_factory``,
the net factories, ...) are **event-driven state machines that are agnostic of
the ANTLR parse tree**. They are driven through plain calls
(``start_expression()``, ``set_operation()``, ``add_component()``,
``start_concat()``, ...) rather than through ``sv2017`` parse contexts. The
``sv_visitor`` is therefore only a thin translator from parse events to
factory calls, and a VHDL visitor can drive the same factories through the
same interface.

What reuses as-is:

* The value model (``Expression_v2``, ``resolved_parameter``, the tokens).
* The expression building blocks: concatenation (VHDL ``&``, including
  element/array mixes and ``default`` initialization), replication
  (``others =>``), ternary (``when ... else``), and casts (qualified
  expressions / type conversions).
* The statement model (``hdl_instance_statement``, ``hdl_loop_statement``,
  ``hdl_conditional_statement``, ``hdl_assignment_statement``); a VHDL
  ``generic map`` at instantiation maps onto the existing instance
  parameter-override path.
* Everything downstream (``data_store``, ``HDL_ast_builder_v2``,
  ``parameter_solver``, bus analyses, backends), which only consumes the
  language-agnostic ``hdl_file`` / AST.

What needs genuinely new, VHDL-specific semantics:

* **The type system** -- the largest extension area: VHDL scalars with
  subtypes and ranges, physical types (``time``), 9-valued
  ``std_logic``/``std_logic_vector``, records and unconstrained arrays, and
  strong typing (no implicit conversions, which is a semantic rule and not
  just a syntax one).
* **The operator set** -- VHDL adds ``rem``, ``mod``, ``abs``, ``not`` and
  ``sll/srl/sla/sra/rol/ror``, plus different precedence rules for
  ``and/or/xor``; ``Expression_v2::expression_operator`` and its evaluation
  must be extended.
* **A VHDL visitor** -- the visitor is necessarily grammar-specific
  (aggregates, ``when ... else``, ``select``, qualified expressions), but it
  is a thin translator producing the same node types.
* **Doc comments** -- ``--`` extraction differs from ``/** */``, but the JSON
  documentation format is reused as-is.

Deliberately left separate (no VHDL analogue to extend): the preprocessor,
the SV streaming operators, and ``defparam``.

Because the downstream plumbing already works, filling in ports/generics/types
in the visitor would immediately enable bus analysis, documentation, and
definition generation on VHDL designs.

Module-level net/signal declarations
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``sv_visitor`` drops module-level declarations such as ``logic [7:0] foo;`` and
``reg [31:0] mem [5:0];`` (only type/struct/enum declarations are handled,
:file:`src/frontend/analysis/system_verilog/sv_visitor.cpp:207-290`).
Consequences:

* No internal-net width information; ``find_datapoint_width`` works around
  this by probing instance ``DATA_WIDTH`` parameters
  (:file:`src/analysis/data_acquisition_analysis.cpp:327-342`).
* Data-flow analysis cannot trace through internal signals.

Procedural block support
~~~~~~~~~~~~~~~~~~~~~~~~

``always``, ``always_ff``, ``always_comb``, ``always_latch``, ``assign``,
``initial``, ``case``, tasks, delays and event controls are all parsed by the
grammar but silently discarded. ``$readmemh``/``$readmemb`` are only captured
as a global side effect of the task-call handler
(:file:`src/frontend/analysis/system_verilog/sv_visitor.cpp:482-501`).
Supporting procedural blocks would enable register-offset derivation from RTL,
proper memory-initialization handling, and real signal-level data-flow.

Generalize bus and scope analysis
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

* The depfile ``bus`` section only consumes the ``"control"`` key; ``type``
  and ``starting_module`` are parsed but unused
  (:file:`src/data_model/Depfile.cpp:79-86`).
* Bus spec files are hardcoded: ``control_bus_analysis`` always loads
  ``axi_lite`` and ``data_acquisition_analysis`` always loads ``axi_stream``
  (:file:`src/analysis/control_bus_analysis.cpp:20`,
  :file:`src/analysis/data_acquisition_analysis.cpp:20`).
* ``data_acquisition_analysis`` hard-errors on multiple sinks
  (:file:`src/analysis/data_acquisition_analysis.cpp:28-31`).
* The address-map extraction supports a single address-space topology with no
  windows, regions, or remap segments beyond the interconnect ``SLAVE_ADDR``
  pattern (:file:`src/analysis/control_bus_analysis.cpp:69-124`).

Making bus/stream declaration data-driven (via the depfile ``bus`` section),
supporting multiple sinks and n-to-m interconnects, and adding address-space
regions are the natural generalizations.

C header / language binding generation
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The tool already produces address maps and bit-field layouts as JSON
(:file:`src/Backend/uplatform/peripheral_definition_generator.cpp`,
:file:`src/Backend/uplatform/application_definition_generator.cpp`). A
generator emitting C headers (``#define`` address/offset/mask constants) or
Rust/Python bindings would complete the "produce headers or otherwise data
structures for software" feature and is cleanly separated from the JSON core.

Soft-core ROM / program handling
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``processor_detection`` resolves a soft core's control address
(:file:`src/analysis/passes/processor_detection.cpp:24-59`), but the
application definition leaves ``programs`` empty and ``default_program`` as
``""`` (:file:`src/Backend/uplatform/application_definition_generator.cpp:47,162`).
Wiring up program image paths and boot/ROM addresses is missing.

Additional toolchain backends
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``project_generator_base`` / ``Toolchain_manager`` abstraction
(:file:`includes/Backend/project_generator_base.hpp`,
:file:`src/Backend/Toolchain_manager.cpp`) makes a Quartus or open
Yosys/nextpnr backend tractable. The Xilinx and Lattice backends both cover
project generation; a new backend would follow the same abstraction.

Generate-statement gaps
~~~~~~~~~~~~~~~~~~~~~~~

* Standalone ``generate ... endgenerate`` regions, ``case``-generate, and
  standalone ``genvar`` declarations are dropped.
* ``while``/``foreach``/``repeat``/``do-while`` are mis-treated as ``for``
  loops in function evaluation (:file:`src/frontend/analysis/system_verilog/sv_visitor.cpp:1397`).
* ``modport`` declarations *inside* interfaces are dropped (only interface
  modport references in module ports are parsed).
* ``defparam`` only consumes the first assignment of a comma-separated group
  (:file:`src/frontend/analysis/system_verilog/sv_visitor.cpp:599-602`) and
  ``$root.`` scopes are skipped with a warning (``$unit.`` hierarchical
  prefixes are invalid SystemVerilog and rejected during parsing).

.. _factory-consolidation:

Factory consolidation residuals
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The ``unified_function_params_factories`` branch has essentially landed: its
commits are in ``main``, and ``main`` has continued the migration (the
functions factory moved off the legacy expression engine, the old expression
was removed from the main expressions factory). What remains is not a branch to
merge but residual dual-path complexity in the visitor, where the functions
factory and the parameters factory are two parallel instances selected by
``f_factory.is_active()`` branching
(:file:`src/frontend/analysis/system_verilog/sv_visitor.cpp:1198-1224`).

The concrete symptom: streaming concatenations inside function bodies are
skipped (:file:`src/frontend/analysis/system_verilog/sv_visitor.cpp:1207`)
while the standalone ``Streaming`` factory implements them for module
parameters. Closing that gap, and over time collapsing the two visitor
factories into one, would remove the remaining duplication.


Heavy Refactors
---------------

Architectural restructuring, multi-week effort each.

A real data-flow / elaboration engine
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The current AXI-Stream backtrace only works through JSON-declared bus
components (sinks, interconnects, sources); it cannot trace through arbitrary
combinational logic or internal nets. Replacing topology-based backtracing
with signal-flow analysis over the AST (nets plus ``always``/``assign``
blocks) is the prerequisite for general, correct "set the runtime selectors"
behavior. This overlaps with the procedural-block work in the mid-sized
section.

Unified type / value model
~~~~~~~~~~~~~~~~~~~~~~~~~~

``type_engine`` tracks SystemVerilog types, but evaluation still juggles the
``resolved_parameter`` variant family (separate wide-integer / string / array
kinds). The ``string`` data type is unresolved and parametric types are
partial. A unified context-based value/type model is the substrate most other
features depend on and builds on the context-based parameter machinery already
in place (``work_order`` parent contexts and ``parameter_solver`` resolution).

Shared frontend semantics (SystemVerilog + VHDL)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

As described under :ref:`vhdl-frontend`, the factories are deliberately
parse-tree-agnostic event-driven state machines, so both languages already
feed a common IR. What is *not* yet unified is the **semantics** layered on top
of that IR:

* The type model is SystemVerilog-shaped (``type_engine``,
  ``HDL_simple_type`` and friends); a single type/value model must accommodate
  both SV packed structs/enums/unions and VHDL records, subtypes, physical
  types, and 9-valued logic.
* ``Expression_v2::expression_operator`` and its evaluation carry
  SystemVerilog operator semantics; VHDL operators and precedence must be
  folded into the same evaluator rather than handled on a parallel expression
  path.
* The factory hierarchy still carries residual internal duplication between
  the visitor and the standalone factories (see
  :ref:`factory-consolidation`).

The refactor is therefore about unifying these semantics and retiring the
duplication -- not about inventing a new abstraction or a third pipeline. VHDL
completion should land as semantic fill-ins on the shared model, not as a
second frontend (see :ref:`vhdl-frontend`).

Test infrastructure overhaul
~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The test suite configures itself by ``FetchContent``-fetching an external HDL
repository (``uscope_hdl``) at configure time, builds a single test
executable, and the end-to-end tests assert exact generated-script text
(:file:`Testing/end_to_end/full_application.cpp`). This makes the suite
fragile and non-hermetic. Moving to proper CTest registration with checked-in
fixtures is the end goal.


Recommended Sequencing
----------------------

1. **Cheap wins first.** The unary operator gap and the unimplemented synthesis
   flow are contained, well-bounded items.
2. **Then the highest-value mid projects:** VHDL frontend completion followed
   by generalization of the bus/scope analysis.
3. **Finally the heavy refactors**, starting with the data-flow engine once
   procedural-block support lands, since the two are mutually reinforcing.
