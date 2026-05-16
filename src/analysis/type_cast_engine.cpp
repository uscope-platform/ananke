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


#include "analysis/type_cast_engine.hpp"

int64_t type_cast_engine::to_unsigned(int64_t in, uint64_t container_size) {
    uint64_t mask = (1ULL << container_size) - 1;
    return static_cast<int64_t>(static_cast<uint64_t>(in) & mask);
}

int64_t type_cast_engine::to_signed(int64_t in, uint64_t container_size) {

    uint64_t shift_amount = 64 - container_size;
    return (in << shift_amount) >> shift_amount;
}

int64_t type_cast_engine::to_int(int64_t in, uint64_t container_size) {
    return 0;
}