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


#ifndef ANANKE_HDL_EXTERNAL_TYPE_HPP
#define ANANKE_HDL_EXTERNAL_TYPE_HPP

#include "data_model/HDL/types/hdl_type.hpp"
#include "data_model/HDL/parameters/common/qualified_identifier.hpp"

class HDL_external_type : public hdl_type{
public:
   explicit HDL_external_type(const qualified_identifier &id) {
       value = id;
   }

    parameter_deps_t get_dependencies() override;
    [[nodiscard]] bool is_scalar()const override;
    std::optional<resolved_type> evaluate_type(const std::map<qualified_identifier, resolved_parameter> &context) override;
    [[nodiscard]] std::string to_print() const override;

   bool operator==(const HDL_external_type & hdl_external) const;

    [[nodiscard]] bool is_equal(const hdl_type &other) const override;

private:
    qualified_identifier value;

};


#endif //ANANKE_HDL_EXTERNAL_TYPE_HPP
