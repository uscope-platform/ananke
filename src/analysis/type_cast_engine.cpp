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

#include "data_model/HDL/parameters/common/hdl_integer.hpp"

hdl_integer type_cast_engine::to_unsigned(hdl_integer in, uint64_t container_size) {
    uint64_t mask;
    if(container_size >= 64) {
        mask = ~0ULL;
    } else {
        mask = (1ULL << container_size) - 1;
    }
    return static_cast<hdl_integer>(static_cast<hdl_integer>(in) & mask);
}

hdl_integer type_cast_engine::to_signed(hdl_integer in, uint64_t container_size) {
    if(container_size == 0) return 0;
    if(container_size >= 64) return in;

    uint64_t shift_amount = 64 - container_size;
    return (in << shift_amount) >> shift_amount;
}

hdl_integer type_cast_engine::to_int(hdl_integer in, uint64_t container_size) {
    return 0;
}

hdl_integer type_cast_engine::to_int(double in, uint64_t container_size) {
    if (std::isnan(in) || std::isinf(in)) return 0;

    double rounded = std::round(in);

    int64_t intermediate;
    if (rounded >= static_cast<double>(std::numeric_limits<int64_t>::max())) {
        intermediate = std::numeric_limits<int64_t>::max();
    } else if (rounded <= static_cast<double>(std::numeric_limits<int64_t>::min())) {
        intermediate = std::numeric_limits<int64_t>::min();
    } else {
        intermediate = static_cast<int64_t>(rounded);
    }

    if (container_size >= 64 || container_size == 0) {
        return intermediate;
    }

    uint64_t shift_amount = 64 - container_size;
    return (intermediate << shift_amount) >> shift_amount;
}