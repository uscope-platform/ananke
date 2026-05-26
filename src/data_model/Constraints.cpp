// Copyright 2021 University of Nottingham Ningbo China
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

#include "data_model/Constraints.hpp"

#include <utility>


Constraints::Constraints(const Constraints &C) {
    name = C.name;
    path = C.path;
}


Constraints::Constraints(std::string n) {
    name = std::move(n);
}


std::string Constraints::get_name() const {
    return name;
}

void Constraints::set_path(std::string p) {
    path = std::move(p);
}

std::string Constraints::get_path() {
    return path;
}

bool operator==(const Constraints &lhs, const Constraints &rhs) {
    return lhs.name == rhs.name && lhs.path == rhs.path;
}