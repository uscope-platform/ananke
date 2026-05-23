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

#ifndef ANANKE_REPLICATION_FACTORY_HPP
#define ANANKE_REPLICATION_FACTORY_HPP

#include "../../parameters/components/Replication.hpp"
#include <stack>

class replication_factory {
public:

    void start_replication(bool is_ass = false);
    void add_expression(const Expression& expr);
    std::shared_ptr<Replication> finish();

    [[nodiscard]] bool is_assignment_context() const;
    [[nodiscard]] bool in_replication() const {return state != build_phase::inactive;};

private:
    std::stack<std::pair<Replication, bool>> replication_stack;
    Replication new_replication;
    bool is_assignment = false;
    enum class build_phase {
        inactive,
        size,
        item
    };

    build_phase state = build_phase::inactive;
};


#endif //ANANKE_REPLICATION_FACTORY_HPP