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

#ifndef ANANKE_HDL_PARAMETER_OVERRIDE_STATEMENT_HPP
#define ANANKE_HDL_PARAMETER_OVERRIDE_STATEMENT_HPP

#include <memory>
#include <vector>
#include <string>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

class hdl_parameter_override_statement : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_instance_path(const std::vector<std::string>& p) { instance_path = p; }
    const std::vector<std::string>& get_instance_path() const { return instance_path; }

    void set_parameter_name(const std::string& n) { parameter_name = n; }
    std::string get_parameter_name() const { return parameter_name; }

    void set_value(const std::shared_ptr<Expression_base>& v) { value = v; }
    std::shared_ptr<Expression_base> get_value() const { return value; }

    template<class Archive>
    void serialize(Archive & ar) {
        ar(instance_path, parameter_name, value);
    }

private:
    std::vector<std::string> instance_path;
    std::string parameter_name;
    std::shared_ptr<Expression_base> value;
};

#endif //ANANKE_HDL_PARAMETER_OVERRIDE_STATEMENT_HPP
