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


#include "data_model/HDL/parameters/common/hdl_integer.hpp"

uint64_t hdl_integer::get_size() {
    auto n_bits = std::log2(value);
    if(std::isinf(n_bits) || n_bits == 0) {
        return 1;
    }else{
        return  std::ceil(n_bits);
    }
}

hdl_integer hdl_integer::operator+(const hdl_integer &o) const {
    return o.value + value;
}

hdl_integer hdl_integer::operator-(const hdl_integer &o) const {
    return value - o.value;
}

hdl_integer hdl_integer::operator*(const hdl_integer &o) const {
    return value * o.value;
}

hdl_integer hdl_integer::operator/(const hdl_integer &o) const {
    return value / o.value;
}

hdl_integer hdl_integer::operator%(const hdl_integer &o) const {
    return value % o.value;
}

hdl_integer hdl_integer::operator&(const hdl_integer &o) const {
    return value & o.value;
}

hdl_integer hdl_integer::operator|(const hdl_integer &o) const {
    return value | o.value;
}

hdl_integer hdl_integer::operator^(const hdl_integer &o) const {
    return value ^ o.value;
}

hdl_integer hdl_integer::operator~() const {
    return ~value;
}

hdl_integer hdl_integer::operator!() const {
    return !value;
}

hdl_integer hdl_integer::operator<<(const hdl_integer &o) const {
    return  value<<o.value;
}

hdl_integer hdl_integer::operator>>(const hdl_integer &o) const {
    return value >> o.value;
}
