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

#include <map>
#include <vector>
#include <memory>
#include <string>
#include <optional>
#include <unordered_map>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/types/hdl_type.hpp"
#include "data_model/HDL/statement/hdl_function_statement.hpp"
#include <cereal/types/map.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>
#include <cereal/types/string.hpp>

class hdl_class_statement : public hdl_statement_base {
public:
    hdl_class_statement();
    explicit hdl_class_statement(const std::string &n) { name = n; }
    hdl_class_statement(const hdl_class_statement &c);

    parameter_deps_t get_dependencies() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_base_class(const std::string &b) { base_class = b; }
    const std::string &get_base_class() const { return base_class; }
    bool has_base_class() const { return !base_class.empty(); }

    void add_typedef(const std::string &tname, const std::shared_ptr<hdl_type> &type) { typedefs.insert({tname, type}); }
    std::map<std::string, std::shared_ptr<hdl_type>> get_typedefs() { return typedefs; }
    std::map<std::string, std::shared_ptr<hdl_type>> get_typedefs() const { return typedefs; }

    void add_property(const std::shared_ptr<HDL_parameter> &p) { properties.push_back(p); }
    const std::vector<std::shared_ptr<HDL_parameter>>& get_properties() const { return properties; }

    void add_method(const hdl_function_statement &f) {
        methods.push_back(std::make_shared<hdl_function_statement>(f));
    }
    const std::vector<std::shared_ptr<hdl_function_statement>>& get_methods() const { return methods; }

    // Classes declared inside this class body (`outer::inner` scope). A nested
    // class is filed here on exit instead of as a top-level entity, so the
    // declaration scope is preserved for qualified lookups.
    void add_nested_class(const std::shared_ptr<hdl_class_statement> &c) { nested_classes.push_back(c); }
    const std::vector<std::shared_ptr<hdl_class_statement>>& get_nested_classes() const { return nested_classes; }

    void set_name(const std::string &n) { name = n; }
    const std::string &getName() const { return name; }

    void set_line_n(unsigned int n) { line_n = n; }
    [[nodiscard]] unsigned int get_line_n() const { return line_n; }

    std::unordered_map<std::string, hdl_function_statement> get_functions();
    std::optional<hdl_function_statement> get_function(const std::string &fname);
    std::shared_ptr<const hdl_function_statement> get_function_shared(const std::string &fname) const;

    bool is_empty();

    friend bool operator<(const hdl_class_statement& lhs, const hdl_class_statement& rhs);
    friend bool operator==(const hdl_class_statement& lhs, const hdl_class_statement& rhs);
    friend void PrintTo(const hdl_class_statement& res, std::ostream* os);

    template<class Archive>
    void serialize(Archive & ar) {
        ar(name, line_n, base_class, typedefs, properties, methods,
           nested_classes);
    }

private:
    std::string name;
    unsigned int line_n = 0;
    std::string base_class;
    std::map<std::string, std::shared_ptr<hdl_type>> typedefs;
    std::vector<std::shared_ptr<HDL_parameter>> properties;
    std::vector<std::shared_ptr<hdl_function_statement>> methods;
    std::vector<std::shared_ptr<hdl_class_statement>> nested_classes;
};


#endif //ANANKE_HDL_CLASS_STATEMENT_HPP
