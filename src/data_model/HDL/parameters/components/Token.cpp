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

#include "data_model/HDL/parameters/components/Token.hpp"

bool operator==(const Token &lhs, const Token &rhs) {
    bool ret_val = true;
    ret_val &= lhs.value == rhs.value;
    ret_val &= lhs.array_index == rhs.array_index;
    ret_val &= lhs.package_prefix == rhs.package_prefix;
    ret_val &= lhs.instance_prefix == rhs.instance_prefix;
    ret_val &= lhs.binary_size == rhs.binary_size;
    return ret_val;
}
