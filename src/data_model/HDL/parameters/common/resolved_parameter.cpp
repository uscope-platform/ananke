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


#include "../../../../../includes/data_model/HDL/parameters/common/resolved_parameter.hpp"

int64_t resolved_parameter::get_integer() const {
    return std::get<int64_t>(content);
}

std::string resolved_parameter::get_string() const {
    return std::get<std::string>(content);
}

mdarray<int64_t> resolved_parameter::get_int_array() const {
    return std::get<mdarray<int64_t>>(content);
}

mdarray<std::string> resolved_parameter::get_string_array() const {
    return std::get<mdarray<std::string>>(content);
}

double resolved_parameter::get_real() const {
    return std::get<double>(content);
}
