

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

#include "HDL/HDL_Resource.hpp"

class hdl_file {
public:
    void set_content(const std::vector<std::shared_ptr<hdl_statement_base>> &c) {content = c;}
    std::vector<std::shared_ptr<hdl_statement_base>> get_content() {return content;}

    void set_path(const std::string &p) { path = p; }
    std::string get_path() { return path; }

    template<class Archive> void serialize(Archive & ar) {
        ar(content, path);  // cereal supports variant natively
    }


private:
    std::string path;
    std::vector<std::shared_ptr<hdl_statement_base>> content;
};


#endif //ANANKE_HDL_FILE_HPP
