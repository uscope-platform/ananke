// Copyright 2022 Filippo Savi
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ANANKE_CMAKE_PROJECT_HPP
#define ANANKE_CMAKE_PROJECT_HPP

#include <filesystem>
#include <string>
#include <utility>
#include <vector>

// A library target declared by an `add_library(<name> ...)` call in a CMakeLists.txt.
struct cmake_target {
    std::string name;
    // Directory holding the CMakeLists.txt that declares the target.
    std::filesystem::path dir;

    bool operator==(const cmake_target &) const = default;
};

// A CMakeLists.txt found in the repository, with the library targets it declares. Produced by the repository walker
// and cached like every other analyzed file.
class cmake_project {
public:
    cmake_project() = default;
    cmake_project(std::string p, std::vector<std::string> t) : path(std::move(p)), targets(std::move(t)) {}

    const std::string &get_path() const { return path; }
    const std::vector<std::string> &get_targets() const { return targets; }

    template<class Archive>
    void serialize(Archive &ar) {
        ar(path, targets);
    }

    friend bool operator==(const cmake_project &lhs, const cmake_project &rhs) = default;
private:
    std::string path;
    std::vector<std::string> targets;
};

#endif //ANANKE_CMAKE_PROJECT_HPP
