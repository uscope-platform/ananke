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


#ifndef ANANKE_HDL_ASSIGNMENT_STATEMENT_HPP
#define ANANKE_HDL_ASSIGNMENT_STATEMENT_HPP

#include "data_model/HDL/statement/hdl_statement_base.hpp"

class hdl_assignment_statement : public hdl_statement_base{
     parameter_deps_t get_dependencies() const override;
     std::unique_ptr<hdl_statement_base> clone() const override;
     bool equals(const hdl_statement_base& other) const override;
     std::string print() const override;
private:


};


#endif //ANANKE_HDL_ASSIGNMENT_STATEMENT_HPP
