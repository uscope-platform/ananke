

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

#ifndef ANANKE_HDL_FILE_HPP
#define ANANKE_HDL_FILE_HPP

#include "data_model/HDL/statement/hdl_statements.hpp"
#include <cereal/types/vector.hpp>
#include <cereal/types/set.hpp>
#include <cereal/types/string.hpp>
#include <set>
#include <string>

// A file-level name the file itself could not resolve (see
// parse_options::defer_unit_name_errors). `forward` marks a `typedef name;`
// that was never defined in the file.
struct unresolved_unit_name {
    std::string name;
    unsigned int line = 0;
    bool forward = false;
    template<class Archive> void serialize(Archive & ar) { ar(name, line, forward); }
};

class hdl_file {
public:
    void set_content(const std::vector<std::shared_ptr<hdl_statement_base>> &c) {content = c;}
    const std::vector<std::shared_ptr<hdl_statement_base>>& get_content() const {return content;}

    // Names declared anywhere in the file (typedefs, classes, enum members,
    // parameters) and the file-level names it could not resolve itself; the
    // repository walker joins them across files.
    void set_unit_names(std::set<std::string> declared, std::vector<unresolved_unit_name> unresolved) {
        declared_names = std::move(declared);
        unresolved_names = std::move(unresolved);
    }
    const std::set<std::string>& get_declared_names() const {return declared_names;}
    const std::vector<unresolved_unit_name>& get_unresolved_names() const {return unresolved_names;}

    template<class Archive> void serialize(Archive & ar) {
        ar(content, declared_names, unresolved_names);
    }

private:
    std::set<std::string> declared_names;
    std::vector<unresolved_unit_name> unresolved_names;
    std::vector<std::shared_ptr<hdl_statement_base>> content;
};


#endif //ANANKE_HDL_FILE_HPP
