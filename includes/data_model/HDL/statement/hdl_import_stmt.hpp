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

#ifndef ANANKE_HDL_IMPORT_STMT_HPP
#define ANANKE_HDL_IMPORT_STMT_HPP

#include <string>

#include "data_model/HDL/statement/hdl_statement_base.hpp"

class hdl_import_stmt : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    bool equals(const hdl_statement_base &other) const override;
    std::string print() const override;

    void set_library(const std::string &l) { library = l; }
    std::string get_library() const { return library; }
    void set_package(const std::string &p) { package = p; }
    std::string get_package() const { return package; }
    void set_item(const std::string &i) { item = i; }
    std::string get_item() const { return item; }
    void set_wildcard(bool w) { wildcard = w; }
    bool is_wildcard() const { return wildcard; }

    friend void PrintTo(const hdl_import_stmt &s, std::ostream *os);

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(library, package, item, wildcard);
    }

private:
    std::string library;
    std::string package;
    std::string item;
    bool wildcard = false;
};

#endif //ANANKE_HDL_IMPORT_STMT_HPP