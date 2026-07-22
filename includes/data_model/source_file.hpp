

//  Copyright  2026 University of Nottingham
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

#ifndef ANANKE_SOURCE_FILE_HPP
#define ANANKE_SOURCE_FILE_HPP

#include <variant>
#include "data_model/Script.hpp"
#include "data_model/HDL/HDL_Resource.hpp"
#include "data_model/DataFile.hpp"
#include "data_model/Constraints.hpp"


using source_content = std::variant<
    Script,
    DataFile,
    Constraints,
    std::vector<HDL_Resource>
>;


struct source_file {
    std::string path;
    std::string hash;
    source_content content;
    template<class Archive> void serialize(Archive & ar) {
        ar(path, hash, content);  // cereal supports variant natively
    }
};

#endif //ANANKE_SOURCE_FILE_HPP
