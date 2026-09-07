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

#ifndef ANANKE_HDL_CONDITIONAL_STATEMENT_HPP
#define ANANKE_HDL_CONDITIONAL_STATEMENT_HPP

#include <memory>
#include <vector>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

struct hdl_conditional_branch {
    std::shared_ptr<Expression_base> condition;
    std::vector<std::shared_ptr<hdl_statement_base>> body;

    template<class Archive>
    void serialize(Archive & ar) {
        ar(condition, body);
    }
};

class hdl_conditional_statement : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_def_ptr &def) override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void add_branch(const std::shared_ptr<Expression_base>& cond);
    void set_condition(const std::shared_ptr<Expression_base>& e);
    void add_to_branch(const std::shared_ptr<hdl_statement_base>& s);
    void add_to_else(const std::shared_ptr<hdl_statement_base>& s);
    void flatten();

    const std::vector<hdl_conditional_branch>& get_branches() const { return branches; }
    const std::vector<std::shared_ptr<hdl_statement_base>>& get_else_body() const { return else_body; }

    bool is_empty() const;

    friend void PrintTo(const hdl_conditional_statement& s, std::ostream* os);

    template<class Archive>
    void serialize(Archive & ar) {
        ar(branches, else_body);
    }

private:
    std::vector<hdl_conditional_branch> branches;
    std::vector<std::shared_ptr<hdl_statement_base>> else_body;
};

#endif //ANANKE_HDL_CONDITIONAL_STATEMENT_HPP
