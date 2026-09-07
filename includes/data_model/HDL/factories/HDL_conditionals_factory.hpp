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

#ifndef ANANKE_HDL_CONDITIONALS_FACTORY_HPP
#define ANANKE_HDL_CONDITIONALS_FACTORY_HPP

#include <stack>

#include "data_model/HDL/statement/hdl_conditional_statement.hpp"


class HDL_conditionals_factory {
public:
    void new_conditional();
    void add_branch();
    void push_nested();
    void set_condition(const std::shared_ptr<Expression_base> &cond);
    void enter_body_item();
    void exit_body_item();
    void add_statement(const std::shared_ptr<hdl_statement_base> &stmt);
    hdl_conditional_statement get_conditional();
    bool is_active() const { return active; }
    bool in_else_branch() const { return in_else; }
    bool has_condition() const;

private:
    struct if_frame {
        int base_depth = 0;
        bool then_done = false;
    };

    struct nested_state {
        hdl_conditional_statement statement;
        std::stack<if_frame> if_stack;
        int body_item_depth = 0;
        bool in_else = false;
    };

    hdl_conditional_statement _statement;
    std::stack<nested_state> _statement_stack;
    std::stack<if_frame> _if_stack;
    int _body_item_depth = 0;
    int _nesting = 0;
    bool active = false;
    bool in_else = false;
};


#endif //ANANKE_HDL_CONDITIONALS_FACTORY_HPP
