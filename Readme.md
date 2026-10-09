# Ananke

Ananke is a build system for FPGA HDL development, it allows the developer to structure their
IP repository in the most logical way, without having to conform to the one required by the toolchain vendor.

The tool analyzes all the HDL files in the repository, figuring out the dependencies between modules, and can automatically generate 
a working project in the vendor IDE of choice through a templated tcl-script.

Upon the first run the software will prompt the user for the HDL repository base directory path, and it will start analyzing its content.
It should be noted that while the step is multi-threaded, it is computationally intensive, and could take up to few minutes for very large repositories.
Any subsequent run will be nearly instant as only the modified files will be scanned again.

Several mechanisms are available to exclude paths from the analysis:

- **.mkignore**: `.mkignore` files hold gitignore-style patterns (one per line,
  relative to the marker's directory) selecting files and sub-directories to
  skip. Supported syntax: `*`, `?`, `[...]` and `**` wildcards, trailing `/`
  for directories only, leading `/` to anchor to the marker's directory,
  `#` comments, `!` negation, last match wins. Markers apply to their subtree
  and nested markers extend the rule set. An empty `.mkignore` keeps the legacy
  behavior of skipping its directory. Excluded files are neither parsed nor
  indexed for `` `include `` auto-discovery.
- **IDE projects**: Directories containing an IDE project file (like .xpr for Xilinx Vivado) will be ignored
- **.git**: git source control hidden directories will be ignored

## Preprocessor defines

Designs frequently gate code on macros supplied by the build system (`+define+`,
Vivado `` `ifdef `` switches, etc.). These can be injected globally for every
analyzed file via the `defines` key of the profile in the settings file
(`<cache_dir>/settings`):

```json
{
    "profiles": {
        "my_profile": {
            "defines": ["KINTEX7", "BOARD_ID=42"]
        }
    }
}
```

A bare name defines the macro to the value `1`; `NAME=value` defines it to the
given value. This makes `` `ifdef NAME `` branches and `` `NAME `` substitutions
resolve as they would in the real build.

## Toolchain support

The following toolchains are supported:

- Xilinx Vivado
- Lattice Radiant

### Limitations

- Include directories need to be set up manually in Lattice Radiant as the feature is not exposed through tcl scripting

## Supported platform

- Linux (developed and tested on debian based distro)

## Depfile

A Depfile is a JSON file used to define how a specific project should be set up by the tool, the following fields are
required by the tool

```json
{
    "general":{
        "project_name":"Test",
        "target_part":"xc7z020clg400",
	    "synth_modules":[
          "first_additional_synth_module", 
          "second_additional_synth_module"
        ],
        "synth_tl": "synth_tl_module",
        "sym_modules":[
          "first_additional_sim_module",
          "second_additional_sim_module"
        ],
        "sim_tl":"sim_tl_module",
        "include_paths":[
            "first_include_dir",
            "second_include_dir"
        ]
    },
    "scripts":[
        {
            "name":"set_properties.tcl",
            "type": "tcl",
            "arguments":[]
        },
        {
            "name":"zynqAxiStreamBd.tcl",
            "type": "py",
            "arguments":[
              "first_arg",
              "second_arg"
            ],
            "product_include":true,
            "product_type":"tcl"
        }
    ],
    "excluded_modules":[
       "excluded_module_1",
       "excluded_module_2"
    ],
    "constraints": [
        "constraints.xdc"
    ]
}
```

### General section
This section of the file contains general information needed to create the output project

- **project name**: Field specifying the name of the output project -- MANDATORY
- **target_part**: String identifying the target part/board -- RESERVED FOR FUTURE DEVELOPMENT
- **synth_tl**: Name of the synthesis top level module. -- MANDATORY
- **synth_modules**: List of additional HDL module names to add to the project, for synthesis, that can not be automatically discovered starting from the top level -- OPTIONAL
- **sim_tl**: Name of the simulation top level module. -- MANDATORY
- **sim_modules**: List of additional HDL module names to add to the project,  for simulation, that can not be automatically discovered starting from the top level -- OPTIONAL
- **include_paths**: List of paths (relative to the repository base), that contain Verilog and System Verilog include files -- OPTIONAL

### Scripts section
This section contains a list of script objects that need to be invoked prior or during the project generation step, with each script containing the following fields

- **name**: Name of the script to invoke (comprising of eventual file extension)
- **type**: Type of the script (supported types TCL and python)
- **arguments**: List of arguments to pass to the script (Leave empty if no arguments are needed) 
- **product_include**: Flag that indicated whether the file generated by the script should be added (if not present it is assumed to be false)
- **product_type**: Type of the file generated by the script (tcl, hdl or xdc)

### Other sections

- **excluded_modules**: List of HDL module names that do not need to be added to the project even if they are dependencies (ie. Xilinx IPI block diagrams)
- **constraints**: List of constraint files that need to be added to the project


# Development

## Dependencies

The only runtime dependency needed by the ananke tool is the pthread library, which should be already available in any modern Linux distribution.
All other libraries are statically linked during compilation to ease deployment.


## Building from source

When building from source the following additional dependencies are necessary

- Cmake
- OpenSSL
- Gtest library

To install on Ubuntu (22.04) use
```shell
sudo apt install libssl-dev libgtest-dev build-essential pkg-config uuid-dev libgmock-dev
```

To build from source the following command can be used:

```shell
cd ananke
mkdir build && cd build
cmake .. -DCMAKE_PROJECT_TOP_LEVEL_INCLUDES=conan_provider.cmake -DCMAKE_BUILD_TYPE=Release
cmake --build . --parallel 8
sudo cmake --install .
```
To generate a distributable DEB package use the following command:

```shell
cpack -G DEB
```

# Known limitations

The SystemVerilog constant-expression evaluator (used for parameter/localparam resolution) has a few deliberate gaps. A detailed, tiered audit lives in `PARAMETER_EVALUATION_ISSUES.md`.

## X/Z 4-state values

The value model is 2-state (0/1). There is no X/Z representation, so literals containing `x`, `z` or `?` (`8'hx`, `'z`, `4'b101z`, unbased `'0`/`'1`/`'x`/`'z`) are not modeled and resolve to `0`. `$isunknown` and `$isunbounded` always return `0`. This only matters for testbench/don't-care code — it does not affect the synthesizable parameters used to generate projects.

## `inside` operator

The set-membership operator (`x inside {1, 4, [7:9]}`) is parsed by the grammar but not evaluated. It is valid in constant expressions per the LRM, but does not appear in the parameter sets this tool processes, so it is not implemented.

## `$root` / `$unit` defparam prefixes

Package-scoped defparam paths using `$root` or `$unit` are not supported; the tool warns and skips them (no crash).

