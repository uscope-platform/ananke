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

#ifndef ANANKE_INDEXING_FACTORY_HPP
#define ANANKE_INDEXING_FACTORY_HPP

#include "data_model/HDL/parameters/components/Expression.hpp"
#include "data_model/HDL/parameters/common/dimension.hpp"

class indexing_factory {
public:
    void start_index(bool r);
    Expression get_index() {return index;}
    void stop_index();
    [[nodiscard]] bool is_active() const {return active;}
    [[nodiscard]] bool is_range() const {return active && range;}
    void add_expression(const Expression &e);
    void add_component(const Expression_component &c){index.push_back(c);}
    void set_packed(bool p);
    std::vector<dimension_t> get_dimensions();
    void set_quantifier(bool q);
private:
    bool quantifier = false;
    bool active = false;
    bool range = false;
    Expression index;
    bool first_bound = true;
    std::vector<dimension_t> dim{};
};


#endif //ANANKE_INDEXING_FACTORY_HPP