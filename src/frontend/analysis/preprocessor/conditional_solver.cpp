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

#include "frontend/analysis/preprocessor/conditional_solver.hpp"


namespace preprocessor {
    void conditional_solver::close_loop() {
        if (loop_level == 0) {
            return;
        }
        loop_level--;
        if (loop_level == 0) {
            solver_active = false;
            in_taken_branch = false;
            already_taken = false;
        } else {
            already_taken = already_taken_stack.back();
            already_taken_stack.pop_back();
            if (!in_taken_branch_stack.empty()) {
                in_taken_branch = in_taken_branch_stack.back();
                in_taken_branch_stack.pop_back();
            }
        }
    }

    void conditional_solver::start_loop(bool if_taken) {
        if (loop_level > 0) {
            already_taken_stack.push_back(already_taken);
            in_taken_branch_stack.push_back(in_taken_branch);
        }

        loop_level++;

        bool parent_active = (loop_level == 1) ? true : in_taken_branch_stack.back();

        if (parent_active && if_taken) {
            in_taken_branch = true;
            already_taken = true;
        } else {
            in_taken_branch = false;
            already_taken = parent_active && if_taken;
        }
        solver_active = true;
    }

    void conditional_solver::advance_elseif(bool if_taken) {
        bool parent_active = (loop_level <= 1) ? true : in_taken_branch_stack.back();

        if (parent_active && !already_taken) {
            if (if_taken) {
                in_taken_branch = true;
                already_taken = true;
            } else {
                in_taken_branch = false;
            }
        } else {
            in_taken_branch = false;
        }
    }

    void conditional_solver::advance_else() {
        bool parent_active = (loop_level <= 1) ? true : in_taken_branch_stack.back();

        if (parent_active && !already_taken) {
            in_taken_branch = true;
            already_taken = true;
        } else {
            in_taken_branch = false;
        }
    }

    bool conditional_solver::is_active() const {
        if (loop_level == 0) return true;
        return in_taken_branch;
    }
}