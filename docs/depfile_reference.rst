Depfile Reference
=================

.. meta::
   :description: Reference for the Depfile JSON project description format.

.. note::

   This page is a placeholder. Content to be added.

Planned content:

* The ``general`` section (``project_name``, ``synth_tl``, ``sim_tl``,
  ``target_part``, ``board``, additional modules, include paths,
  ``sim_harness`` C++ harness sources and the ``sim_defines`` leading-units
  override for the Verilator backend).
* The ``verilator`` tool section (``cflags``, ``waivers``, ``make_args``,
  ``autoflush``, ``extra_args``) and its defaults.
* The ``scripts`` section (``name``, ``type``, ``arguments``,
  ``include_products``, ``products_type``) and the ``Script`` data model.
* ``excluded_modules`` and ``constraints``.
* The ``bus`` section (``type``, ``starting_module``, ``bus_interface``) and
  how it drives bus analysis.

The current schema lives in
:file:`includes/data_model/Depfile/depfile_schema.hpp`; validation is in
:file:`includes/data_model/Depfile/depfile_validator.hpp`.
