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

#ifndef ANANKE_HDL_CLASS_STATEMENT_HPP
#define ANANKE_HDL_CLASS_STATEMENT_HPP

#include <string>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/HDL_definitions.hpp"
#include <cereal/types/string.hpp>

// Basic SystemVerilog class definition tracking. For now this is a leaf
// definition node only (name/line/language): class bodies, parameters,
// typedefs and methods are not elaborated yet, they are skipped by the
// visitor while `in_class` is set.
class hdl_class_statement : public hdl_statement_base {
public:
    hdl_class_statement() = default;
    explicit hdl_class_statement(const std::string &n) { name = n; }
    hdl_class_statement(const hdl_class_statement &c) = default;

    parameter_deps_t get_dependencies() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_name(const std::string &n) { name = n; }
    const std::string &getName() const { return name; }

    void set_language(hdl_language l) { language = l; }
    [[nodiscard]] hdl_language get_language() const { return language; }

    void set_line_n(unsigned int n) { line_n = n; }
    [[nodiscard]] unsigned int get_line_n() const { return line_n; }

    bool is_empty();

    friend bool operator<(const hdl_class_statement& lhs, const hdl_class_statement& rhs);
    friend bool operator==(const hdl_class_statement& lhs, const hdl_class_statement& rhs);
    friend void PrintTo(const hdl_class_statement& res, std::ostream* os);

    template<class Archive>
    void serialize(Archive & ar) {
        ar(name, line_n, language);
    }

private:
    std::string name;
    unsigned int line_n = 0;
    hdl_language language = hdl_language::unknown;
};


#endif //ANANKE_HDL_CLASS_STATEMENT_HPP
