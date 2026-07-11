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

#ifndef ANANKE_CONDITIONAL_SOLVER_HPP
#define ANANKE_CONDITIONAL_SOLVER_HPP


#include <vector>
#include <string>
#include <set>
#include <cstdint>

namespace preprocessor {
    class conditional_solver {
    public:
        void start_loop(bool if_taken);
        void close_loop();
        void advance_elseif(bool if_taken);
        void advance_else();
        [[nodiscard]] bool is_active() const;
    private:
        bool solver_active = false;
        bool already_taken = false;
        bool in_taken_branch = false;
        unsigned int loop_level = 0;
        std::vector<bool> already_taken_stack;
        std::vector<bool> in_taken_branch_stack;
    };
}


#endif //ANANKE_CONDITIONAL_SOLVER_HPP
