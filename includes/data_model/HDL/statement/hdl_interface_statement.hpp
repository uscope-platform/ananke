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

#ifndef ANANKE_HDL_INTERFACE_STATEMENT_HPP
#define ANANKE_HDL_INTERFACE_STATEMENT_HPP

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <optional>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/HDL_definitions.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/types/hdl_type.hpp"
#include "data_model/HDL/statement/hdl_function_statement.hpp"
#include <cereal/types/map.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>
#include <unordered_map>

// Interface data model: sibling of hdl_resource_statement (modules) and
// hdl_package_statement, not a subclass. Interfaces carry parameters,
// typedefs, functions and structural statements only: in this data model
// interfaces have no port_specs (interface signals are filed as statements,
// and `interface_port` marks module ports of interface type), no
// architectures and no module documentation. Splitting them out leaves
// hdl_resource_statement as modules-only.
class hdl_interface_statement : public hdl_statement_base {
public:
    hdl_interface_statement();
    explicit hdl_interface_statement(const std::string &n) { name = n; }
    hdl_interface_statement(const hdl_interface_statement &c);

    parameter_deps_t get_dependencies() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void add_typedef(const std::string &tname, const std::shared_ptr<hdl_type> &type) { typedefs.insert({tname, type}); }
    std::map<std::string, std::shared_ptr<hdl_type>> get_typedefs() { return typedefs; }
    std::map<std::string, std::shared_ptr<hdl_type>> get_typedefs() const { return typedefs; }

    void add_statement(const std::shared_ptr<hdl_statement_base> &s) { statements.push_back(s); }
    const std::vector<std::shared_ptr<hdl_statement_base>>& get_statements() const { return statements; }

    // Parameter declarations in declaration order. Source of truth for
    // interface parameters (mirrors hdl_resource_statement).
    std::vector<std::shared_ptr<HDL_parameter>> get_parameter_statements() const {
        std::vector<std::shared_ptr<HDL_parameter>> out;
        for (const auto &s : statements) {
            if (auto p = std::dynamic_pointer_cast<HDL_parameter>(s)) out.push_back(p);
        }
        return out;
    }

    void set_name(const std::string &n) { name = n; }
    const std::string &getName() const { return name; }

    void set_language(hdl_language l) { language = l; }
    [[nodiscard]] hdl_language get_language() const { return language; }

    void set_line_n(unsigned int n) { line_n = n; }
    [[nodiscard]] unsigned int get_line_n() const { return line_n; }

    void add_function(const hdl_function_statement &f) {
        statements.push_back(std::make_shared<hdl_function_statement>(f));
    }
    std::unordered_map<std::string, hdl_function_statement> get_functions();
    std::optional<hdl_function_statement> get_function(const std::string &fname);
    std::shared_ptr<const hdl_function_statement> get_function_shared(const std::string &fname) const;

    bool is_empty();

    friend bool operator<(const hdl_interface_statement& lhs, const hdl_interface_statement& rhs);
    friend bool operator==(const hdl_interface_statement& lhs, const hdl_interface_statement& rhs);
    friend void PrintTo(const hdl_interface_statement& res, std::ostream* os);

    template<class Archive>
    void serialize(Archive & ar) {
        ar(name, line_n, typedefs, statements, language);
    }

private:
    std::string name;
    unsigned int line_n = 0;
    hdl_language language = hdl_language::unknown;
    std::map<std::string, std::shared_ptr<hdl_type>> typedefs;
    std::vector<std::shared_ptr<hdl_statement_base>> statements;
};

#endif //ANANKE_HDL_INTERFACE_STATEMENT_HPP
