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

#ifndef ANANKE_DATAFILE_HPP
#define ANANKE_DATAFILE_HPP

#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <cstdio>

class DataFile {
public:
    DataFile() = default;
    DataFile(std::string n, std::string p);

    std::string get_name()const;
    void set_path(std::string p);
    std::string get_path();

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, path);
    }

    friend bool operator==(const DataFile&lhs, const DataFile&rhs);
private:
    std::string name;
    std::string path;

};


#endif //ANANKE_DATAFILE_HPP
