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

#ifndef ANANKE_CMAKE_TARGET_SCANNER_HPP
#define ANANKE_CMAKE_TARGET_SCANNER_HPP

#include <expected>
#include <filesystem>
#include <string>
#include <vector>

#include "data_model/cmake_project.hpp"

// Reads library targets out of CMakeLists.txt files, so that a Depfile can refer to a library by target name only
// (`sim_libs`). Finding the files is the job of the repository walker, which hands each one to `parse` and caches the
// result in the data store.
//
// This is a lexical read, not a CMake evaluation: only calls whose target name is a literal are reported. Skipped on
// purpose:
//   - `IMPORTED` targets (they are not built from this tree) and `ALIAS` targets (the aliased target is reported)
//   - names containing a variable or generator expression (`${PROJECT_NAME}`), which cannot be known without
//     evaluating the project
// Calls under `if()` are reported regardless of the condition.
class cmake_target_scanner {
public:
    // Targets declared in the text of one CMakeLists.txt located in `dir`. Pure function.
    static std::vector<cmake_target> parse(const std::string &content, const std::filesystem::path &dir);

    // The single target called `name`. An error message (unknown, or defined in several directories) lists the
    // alternatives so the Depfile can be fixed.
    static std::expected<cmake_target, std::string> resolve(const std::vector<cmake_target> &targets,
                                                            const std::string &name);
};

#endif //ANANKE_CMAKE_TARGET_SCANNER_HPP
