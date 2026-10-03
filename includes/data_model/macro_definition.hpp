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

#ifndef ANANKE_MACRO_DEFINITION_HPP
#define ANANKE_MACRO_DEFINITION_HPP

#include <string>
#include <vector>

// Serializable snapshot of a single preprocessor macro definition, as
// harvested from one analyzed file. This is the persisted form of the
// live preprocessor definitions map (see macro_processor.hpp): simple
// macros carry their replacement text in `value`, function-like macros
// carry the macro body plus the ordered argument list.
struct macro_argument_def {
    std::string name;
    std::string default_value;
    bool has_default = false;

    template<class Archive> void serialize(Archive & ar) {
        ar(name, default_value, has_default);
    }

    bool operator==(const macro_argument_def &other) const = default;
};

struct stored_macro_def {
    std::string name;
    bool is_function = false;
    std::string value;
    std::vector<macro_argument_def> args;

    template<class Archive> void serialize(Archive & ar) {
        ar(name, is_function, value, args);
    }

    bool operator==(const stored_macro_def &other) const = default;
};

#endif //ANANKE_MACRO_DEFINITION_HPP
