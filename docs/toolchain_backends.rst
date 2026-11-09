Toolchain Backends
==================

.. meta::
   :description: How Ananke generates projects for FPGA vendor toolchains.

.. note::

   This page is a placeholder. Content to be added.

Planned content:

* The backend abstraction: ``project_generator_base``, ``Toolchain_manager``
  and the ``project_data`` structure
  (:file:`includes/Backend/backend_types.hpp`).
* Xilinx Vivado support: project makefile, batch simulation flow, standalone
  synthesis flow, and board/target-part handling.
* Lattice Radiant support and its known limitations.
* Verilator simulation support: command-line simulation script generation
  from the simulation dependency closure
  (:file:`src/Backend/Verilator/verilator_project_generator.cpp`).
* Verilator SystemC testbenches: when the Depfile lists ``sim_libs`` (names of
  CMake library targets, e.g. ``sctb``) the simulation script builds the
  ``sim_harness`` through a generated CMake project that verilates the
  simulation closure with ``verilate(... SYSTEMC ...)`` (``--no-timing``,
  ``-std`` flags dropped, the C++ standard comes from the CMake targets) and
  links the libraries. Library targets are found by the repository walker in
  the ``add_library()`` calls of the repository's ``CMakeLists.txt`` files
  (build trees and ``.mkignore`` exclusions are skipped); each library is added
  with ``add_subdirectory``, so its ``CMakeLists.txt`` must work when it is not
  the top-level project, and the first configure may need network access if the
  library fetches dependencies. ``sim_tl`` must be a module with plain ports
  (Verilator rejects interface ports on the top). SystemC is located through
  ``SYSTEMC_HOME``, the ``systemc`` tool path of the settings, or Homebrew.
* Adding a new backend (e.g. Quartus or an open Yosys/nextpnr flow).
* Script and constraint resolution
  (:file:`src/Backend/Auxiliary_resolver.cpp`,
  :file:`src/Backend/python_script_runner.cpp`).
