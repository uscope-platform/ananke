# CLAUDE.md

## Project

**Ananke** (repo dir `makefile_gen`) is a C++23 build system for FPGA HDL repositories. It scans a repository of
SystemVerilog/VHDL, resolves module dependencies, parameters, and bus/register structure, and generates vendor
projects (Xilinx Vivado, Lattice Radiant) plus Verilator scripts, uPlatform application/peripheral definitions, and
documentation. Project behavior is driven by a JSON `Depfile` and a per-profile settings file. See `Readme.md` for
user-facing docs and `docs/` (Sphinx, `.rst`) for reference docs (`architecture.rst` is still a placeholder).

## Build & test

Dependencies come from Conan (`conanfile.txt`) via `conan_provider.cmake`; do not vendor or hand-install them.

```shell
mkdir build && cd build
cmake .. -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake -DCMAKE_BUILD_TYPE=Debug
cmake --build . --parallel 8
ctest --test-dir . --output-on-failure        # or run ./Testing/tests directly
./Testing/tests --gtest_filter='Suite.Name*'  # single test / suite
```

- Existing build dirs: `build/`, `cmake-build-{debug,release,relwithdebinfo,asan,ubsan}` (CLion-style, driven by
  `CMakeUserPresets.json`). Don't commit them (all gitignored).
- Optional CMake flags: `ENABLE_ASAN`, `ENABLE_TSAN`, `ENABLE_COVERAGE`, `ENABLE_PROFILING`, `ENABLE_PEDANTIC`
  (`-Wall -Wextra`), `ENABLE_PARANOID` (adds `-Werror`).
- **Debug builds regenerate the ANTLR parsers** from `grammars/*.g4` (needs a JDK and network to fetch the ANTLR jar).
  Non-Debug builds use the committed generated sources in `includes/mgp_sv` and `includes/mgp_vh`. After editing a
  grammar, build in Debug and commit the regenerated files; never hand-edit generated parser files.
- `SPECS_FOLDER` (bus spec JSONs): `bus_specs/` in-tree for Debug/RelWithDebInfo, `/usr/local/etc/ananke/specs`
  otherwise. Tests override this via `ANANKE_SPECS_DIR` (set in `Testing/test_main.cpp`).
- Tests fetch the `uscope_hdl` repo at a pinned commit through `FetchContent` into the test build dir at configure
  time (network needed on first configure). Test fixtures in `Testing/check_files` and `Testing/files` are copied to the
  build dir with `configure_file`; **a new fixture or a new test `.cpp` must be registered in `Testing/CMakeLists.txt`**.
- Only Linux is officially supported (developed on Debian-based distros); it also builds on macOS.

## Layout

```
src/ , includes/        mirrored trees: headers in includes/<area>/, sources in src/<area>/
  main.cpp, ananke.cpp  CLI11 entry point (flags listed in main.cpp) and top-level orchestration
  frontend/             Repository_walker, ignore_matcher (.mkignore), macro_table, repository_index
    analysis/system_verilog/   preprocessor/, sv_visitor, sv_analyzer, sv_semantic_checker, type_engine, documentation_analyzer
    analysis/vhdl/             vhdl_analyzer, vhdl_visitor, vhdl_type_engine
  data_model/           hdl_file, HDL/ (resources, statements, params, types), Depfile, data_store (cache), settings_store, bus_mapping, documentation
  analysis/             HDL_ast_builder_v2, parameter_solver, package_solver, loop_solver, type_cast_engine,
                        topological_sorter, control_bus/data_acquisition/proxy_bus analyses, passes/ (pass_manager, port_solution, processor_detection)
  Backend/              Dependency_resolver, project_generator_base, Xilinx/, Lattice/, Verilator/, uplatform/, new_app_generator, python_script_runner
  mgp_sv/, mgp_vh/      GENERATED ANTLR parsers (includes only; don't edit)
  third_party/          boost (standalone multiprecision), thread_pool.hpp
grammars/               sv2017*.g4, vhdl*.g4 (source of truth for the parsers)
bus_specs/              bus protocol definitions (axi_lite, axi_stream)
Testing/                single `tests` GoogleTest binary; subdirs mirror src (frontend, parameters, analysis, data_model, Backend, end_to_end)
```

Pipeline: repository walk (multithreaded, honors `.mkignore`, skips IDE-project dirs and `.git`) → per-file
SV/VHDL parse (ANTLR) via visitors → `hdl_file` data model, cached on disk → AST build + parameter/package/loop solving
→ analysis passes (ports, processors, buses) → dependency resolution from the Depfile top levels → backend generation.

Design seam: the expression/function/statement/net factories are event-driven state machines independent of the ANTLR
parse tree; language visitors are thin translators into them. Keep new language-specific logic in the visitors and
shared semantics in the factories.

## Things that bite

- **Cache schema hash**: `CMakeLists.txt` hashes `data_model`, `frontend/analysis`, `macro_table`, `Repository_walker`,
  `repository_index`, and `mgp_sv` sources into `generated/cache_schema_hash.hpp` to auto-invalidate the on-disk cache.
  Changes to these files legitimately trigger a full re-parse for users; no manual version bump is needed. Cache is
  per-profile (`--cache_dir`, `--no-cache`, `--refresh-cache`, `--clear-cache`).
- Changing Depfile `defines`/`includes` or profile `defines` forces a full re-parse; keep that in mind when touching
  settings/Depfile loading.
- `--strict` rejects non-compliant syntax; default mode applies lenient workarounds. `--no-semantic-checks` skips the
  LRM semantic checker. SV compliance work is validated against sv-tests using `--parse`, `--parse-define`,
  `--parse-incdir`.
- Useful debug flags: `--trace`, `--dump_ast` (writes `/tmp/ast.json`), `--measure-runtime`, `--wait_profiler`.
  `crash_context.hpp` installs a crash handler; `profiler.hpp` provides `LOG_TIMEPOINT`.
- Libraries are statically linked (OpenSSL static); only pthread is a runtime dependency. Use the existing deps
  (spdlog, nlohmann_json, valijson, cereal, ctre, CLI11) rather than adding new ones.

## Conventions

- C++23. Files carry the Apache-2.0 header (University of Nottingham, Filippo Savi); copy it into new
  source files.
- Match existing style: header/source pairs under mirrored `includes/` and `src/` paths; new sources must be added to
  the `target_sources` list in `src/CMakeLists.txt` (and headers in `includes/CMakeLists.txt` if listed there).
- Bug fixes come with a regression test in the matching `Testing/` subdirectory (recent history is mostly small
  "fix X" + "add test for X" commits). Commit messages are short, lowercase, imperative.
- Untracked scratch files at the repo root (`profile_points.csv`, `sim.tcl`) are run artifacts; don't commit them.
