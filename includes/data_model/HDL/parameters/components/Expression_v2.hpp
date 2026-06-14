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


#ifndef ANANKE_EXPRESSION_V2_HPP
#define ANANKE_EXPRESSION_V2_HPP

#include "Parameter_value_base.hpp"


class Expression_v2 : public Parameter_value_base {
public:
private:

    bool isEqual(const Parameter_value_base& other) const override {
        const auto& rhs = static_cast<const Expression_v2&>(other);
        bool ret = true;

        return ret;
    }

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;


};



#endif //ANANKE_EXPRESSION_V2_HPP
