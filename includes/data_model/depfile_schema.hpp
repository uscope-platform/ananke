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


#ifndef ANANKE_DEPFILE_SCHEMA_HPP
#define ANANKE_DEPFILE_SCHEMA_HPP

#include <string>



constexpr std::string_view depfile_schema = R"~(
{
  "$schema": "https://json-schema.org/draft-07/schema",
  "type": "object",
  "properties": {
    "general": {
      "type": "object",
      "description": "Group of general project settings",
      "properties": {
        "project_name": {
          "type": "string",
          "description": "Name of the project to generate"
        },
        "target_part": {
          "type": "string",
          "description": "FPGA part to target"
        },
        "synth_tl": {
          "type": "string",
          "description": "Name of the top level module to use for synthesis"
        },
        "synth_modules": {
          "type": "array",
          "description": "Additional modules to be added to the synthesis dataset"
        },
        "sim_tl": {
          "type": "string",
          "description": "Name of the top level module to use for simulations"
        },
        "sim_modules": {
          "type": "array",
          "description": "Additional modules to be added to the simulation dataset"
        },
        "include_paths": {
          "type": "array",
          "description": "List of directories to be added to the include path"
        }
      },
      "required": [
        "project_name",
        "target_part",
        "synth_tl",
        "sim_tl"
      ]
    },
    "scripts": {
      "type": "array",
      "description": "List of scripts to run during build",
      "items": {
        "type": "object",
        "properties": {
          "name": {
            "type": "string",
            "description": "Name of the script file"
          },
          "type": {
            "type": "string",
            "description": "Type of the script to call"
          },
          "function_mode": {
            "type": "boolean",
            "description": "The selected TCL script defines a function that will be called during project construction with the specified arguments"
          },
          "arguments": {
            "type": "object",
            "description": "Arguments that should be passed to the script"
          }
        },
        "required": [
          "type",
          "name"
        ]
      }
    },
    "excluded_modules": {
      "type": "array",
      "description": "List of modules that should be excluded from the tool analysis"
    },
    "constraints": {
      "type": "array",
      "description": "List of constraint files to add to the project"
    },
    "bus": {
      "type": "object",
      "description": "Description of the control busses to track",
      "properties": {}
    }
  },
  "required": [
    "general"
  ]
}
)~";

#endif //ANANKE_DEPFILE_SCHEMA_HPP
