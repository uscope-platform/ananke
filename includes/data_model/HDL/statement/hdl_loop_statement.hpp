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

#ifndef ANANKE_HDL_LOOP_STATEMENT_HPP
#define ANANKE_HDL_LOOP_STATEMENT_HPP

#include <memory>
#include <vector>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"

class hdl_loop_statement : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    std::unique_ptr<hdl_statement_base> clone() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_init(const std::shared_ptr<HDL_parameter>& p) { init = p; }
    void set_end_condition(const std::shared_ptr<Expression_base>& e) { end_condition = e; }
    void set_iteration(const std::shared_ptr<Expression_base>& i) { iteration = i; }
    void add_body_stmt(const std::shared_ptr<hdl_statement_base>& s) { loop_body.push_back(s); }

    std::shared_ptr<HDL_parameter> get_init() const { return init; }
    std::shared_ptr<Expression_base> get_end_condition() const { return end_condition; }
    std::shared_ptr<Expression_base> get_iteration() const { return iteration; }
    const std::vector<std::shared_ptr<hdl_statement_base>>& get_body() const { return loop_body; }

private:
    std::shared_ptr<HDL_parameter> init;
    std::shared_ptr<Expression_base> end_condition;
    std::shared_ptr<Expression_base> iteration;
    std::vector<std::shared_ptr<hdl_statement_base>> loop_body;
};


#endif //ANANKE_HDL_LOOP_STATEMENT_HPP
