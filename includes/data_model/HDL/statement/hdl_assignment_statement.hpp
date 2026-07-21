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


#ifndef ANANKE_HDL_ASSIGNMENT_STATEMENT_HPP
#define ANANKE_HDL_ASSIGNMENT_STATEMENT_HPP

#include <memory>
#include <string>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

class hdl_assignment_statement : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    std::unique_ptr<hdl_statement_base> clone() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_target(const std::string& n) { target = n; }
    std::string get_target() const { return target; }

    void set_index(const std::shared_ptr<Expression_base>& idx) { index = idx; }
    std::shared_ptr<Expression_base> get_index() const { return index; }

    void set_value(const std::shared_ptr<Expression_base>& v) { value = v; }
    std::shared_ptr<Expression_base> get_value() const { return value; }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(target, index, value);
    }
private:
    std::string target;
    std::shared_ptr<Expression_base> index;
    std::shared_ptr<Expression_base> value;
};


#endif //ANANKE_HDL_ASSIGNMENT_STATEMENT_HPP
