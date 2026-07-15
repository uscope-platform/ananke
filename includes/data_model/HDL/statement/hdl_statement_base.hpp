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

#ifndef ANANKE_HDL_STATEMENT_BASE_HPP
#define ANANKE_HDL_STATEMENT_BASE_HPP

#include "data_model/HDL/parameters/common/qualified_identifier.hpp"


class hdl_statement_base {
public:
    virtual ~hdl_statement_base() = default;
    virtual parameter_deps_t get_dependencies() const = 0;
    virtual std::unique_ptr<hdl_statement_base> clone() const = 0;
    virtual bool equals(const hdl_statement_base& other) const = 0;
    virtual std::string print() const = 0;

    bool operator==(const hdl_statement_base& rhs) const {
        return typeid(*this) == typeid(rhs) && equals(rhs);
    }
};


#endif //ANANKE_HDL_STATEMENT_BASE_HPP
