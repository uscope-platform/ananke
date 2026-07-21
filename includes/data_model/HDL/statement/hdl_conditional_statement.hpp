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

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

struct conditional_block {
    std::shared_ptr<Expression_base> condition;
    std::vector<std::shared_ptr<hdl_statement_base>> content;
};

class hdl_conditional_statement : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    std::unique_ptr<hdl_statement_base> clone() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_else_content(const  std::vector<std::shared_ptr<hdl_statement_base>> &block){else_branch = block;}
    void add_if_block(const conditional_block &block) {if_branches.push_back(block);}

private:
    std::vector<conditional_block> if_branches;
    std::vector<std::shared_ptr<hdl_statement_base>> else_branch;
};


#endif //ANANKE_HDL_CONDITIONAL_STATEMENT_HPP
