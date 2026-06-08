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


#ifndef ANANKE_HDL_TYPE_BASE_HPP
#define ANANKE_HDL_TYPE_BASE_HPP

#include <optional>
#include <map>

#include "data_model/HDL/types/resolved_type.hpp"
#include "data_model/HDL/parameters/common/resolved_parameter.hpp"
#include "data_model/HDL/parameters/common/qualified_identifier.hpp"

class hdl_type {
public:
    virtual ~hdl_type() = default;
    template<typename T>
      T& as() { return static_cast<T&>(*this); }

    [[nodiscard]] virtual bool is_scalar()const = 0;
     virtual std::optional<resolved_type> evaluate_type(const std::map<qualified_identifier, resolved_parameter> &context) = 0;
};

#endif //ANANKE_HDL_TYPE_BASE_HPP
