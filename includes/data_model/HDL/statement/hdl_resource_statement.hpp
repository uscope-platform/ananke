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

#ifndef ANANKE_HDL_RESOURCE_STATEMENT_HPP
#define ANANKE_HDL_RESOURCE_STATEMENT_HPP

#include <unordered_map>
#include <map>
#include <vector>
#include <memory>
#include <string>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/HDL_definitions.hpp"
#include "data_model/HDL/HDL_net.hpp"
#include "data_model/HDL/HDL_port.hpp"
#include "data_model/HDL/parameters/Parameters_map.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/types/hdl_type.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/statement/hdl_function_statement.hpp"
#include "data_model/documentation/module_documentation.hpp"
#include "data_model/documentation/processor_instance.hpp"
#include <cereal/types/unordered_map.hpp>

class hdl_resource_statement;
// TEMPORARY migration bridge, defined below (see comment there).
inline Parameters_map collect_parameter_declarations(const hdl_resource_statement &res);

class hdl_resource_statement : public hdl_statement_base {
public:
    hdl_resource_statement();
    explicit hdl_resource_statement(const std::string &n) { name = n; }
    hdl_resource_statement(const hdl_resource_statement &c);

    parameter_deps_t get_dependencies() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void add_typedef(const std::string &tname, const std::shared_ptr<hdl_type> &type) { typedefs.insert({tname, type}); }
    std::map<std::string, std::shared_ptr<hdl_type>> get_typedefs() { return typedefs; }

    void add_statement(const std::shared_ptr<hdl_statement_base> &s) { statements.push_back(s); }
    const std::vector<std::shared_ptr<hdl_statement_base>>& get_statements() const { return statements; }

    void set_name(const std::string &n) { name = n; }

    void set_architecture(const std::string &a) { architecture = a; }
    std::string get_architecture() const { return architecture; }
    const std::string &getName() const { return name; }

    void set_type(dependency_class t) { hdl_dependency_type = t; }
    dependency_class get_type() { return hdl_dependency_type; }

    void set_language(hdl_language l) { language = l; }
    [[nodiscard]] hdl_language get_language() const { return language; }

    void set_line_n(unsigned int n) { line_n = n; }
    [[nodiscard]] unsigned int get_line_n() const { return line_n; }

    bool is_interface();

    [[nodiscard]] std::unordered_map<std::string, HDL_port> get_port_specs() const { return port_specs; }
    void set_ports(std::unordered_map<std::string, HDL_port> p_s) { port_specs = std::move(p_s); }
    void add_ports(const std::string &p_n, HDL_port spec) { port_specs[p_n] = spec; }

    void add_processor_doc(processor_instance &p) { processor_docs.push_back(p); }
    std::vector<processor_instance> get_processor_doc() { return processor_docs; }
    bool has_processors() { return !processor_docs.empty(); }

    // Transitional: declarations are first-class statements now; the legacy
    // parameters_spec map is materialized from them beforehand (see
    // collect_parameter_declarations), so this only appends the statement.
    void add_parameter(const std::shared_ptr<HDL_parameter> &p) { statements.push_back(p); }
    void set_parameters(Parameters_map p);
    // Transitional: prefers the beforehand-materialized map, otherwise
    // collects from the statements (see collect_parameter_declarations).
    Parameters_map get_parameters() const {
        if (!parameters_spec.empty()) return parameters_spec;
        return collect_parameter_declarations(*this);
    }

    void add_function(const hdl_function_statement &f) {
        statements.push_back(std::make_shared<hdl_function_statement>(f));
    }
    std::unordered_map<std::string, hdl_function_statement> get_functions();
    std::optional<hdl_function_statement> get_function(const std::string &fname);
    // Stable shared handle to the stored definition. Unlike get_function
    // (which returns a copy), the pointed-to statement stays alive as long as
    // anyone holds the handle, so call sites can link it without cloning.
    std::shared_ptr<const hdl_function_statement> get_function_shared(const std::string &fname) const;

    void set_documentation(module_documentation &d) { doc = d; }
    module_documentation get_documentation() const { return doc; }

    bool is_empty();

    friend bool operator<(const hdl_resource_statement& lhs, const hdl_resource_statement& rhs);
    friend bool operator==(const hdl_resource_statement& lhs, const hdl_resource_statement& rhs);
    friend void PrintTo(const hdl_resource_statement& res, std::ostream* os);

    template<class Archive>
    void serialize(Archive & ar) {
        ar(name, hdl_dependency_type, parameters_spec, port_specs, doc, processor_docs,
            line_n, typedefs, statements, architecture, language);
    }

private:
    std::string name;
    std::string architecture;
    unsigned int line_n = 0;
    dependency_class hdl_dependency_type = module;
    hdl_language language = hdl_language::unknown;
    std::unordered_map<std::string, HDL_port> port_specs;
    // Legacy map, materialized from the statements beforehand (see
    // collect_parameter_declarations below). Do not write directly.
    Parameters_map parameters_spec;
    std::vector<processor_instance> processor_docs;
    std::map<std::string, std::shared_ptr<hdl_type>> typedefs;
    std::vector<std::shared_ptr<hdl_statement_base>> statements;
    module_documentation doc;
};

// TEMPORARY migration bridge: the parser stores parameter declarations as
// first-class statements and no longer fills parameters_spec. The map-based
// solver still needs a Parameters_map, so callers materialize it from the
// statements beforehand (see the AST builder). Delete together with
// Parameters_map once the solver reads statements directly.
inline Parameters_map collect_parameter_declarations(const hdl_resource_statement &res) {
    Parameters_map out;
    for (const auto &s : res.get_statements()) {
        if (auto p = std::dynamic_pointer_cast<HDL_parameter>(s)) out.insert(p);
    }
    return out;
}

#endif //ANANKE_HDL_RESOURCE_STATEMENT_HPP
