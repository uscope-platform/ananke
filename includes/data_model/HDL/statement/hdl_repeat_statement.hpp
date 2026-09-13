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

#ifndef ANANKE_HDL_REPEAT_STATEMENT_HPP
#define ANANKE_HDL_REPEAT_STATEMENT_HPP

#include <memory>
#include <vector>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"


class hdl_repeat_statement : public hdl_statement_base {
public:
    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_def_ptr &def) override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_count(const std::shared_ptr<Expression_base>& e) { count = e; }
    std::shared_ptr<Expression_base> get_count() const { return count; }
    void add_body_stmt(const std::shared_ptr<hdl_statement_base>& s) { loop_body.push_back(s); }
    const std::vector<std::shared_ptr<hdl_statement_base>>& get_body() const { return loop_body; }

    friend void PrintTo(const hdl_repeat_statement& s, std::ostream* os);

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(count, loop_body);
    }

private:
    std::shared_ptr<Expression_base> count;
    std::vector<std::shared_ptr<hdl_statement_base>> loop_body;
};

#endif //ANANKE_HDL_REPEAT_STATEMENT_HPP
