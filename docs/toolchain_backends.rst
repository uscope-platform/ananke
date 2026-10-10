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
* Adding a new backend (e.g. Quartus or an open Yosys/nextpnr flow).
* Script and constraint resolution
  (:file:`src/Backend/Auxiliary_resolver.cpp`,
  :file:`src/Backend/python_script_runner.cpp`).
