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

void hdl_integer::set_size(const int64_t v) {
    value = v;
    wide = false;
}

void hdl_integer::set_value(const uint64_t v) {
    value = v;
    wide = false;
}

void hdl_integer::set_value(const int1024_t v) {
    wide_value = v;
    wide = true;
}

uint64_t hdl_integer::get_size() {
    if(value == 0) return 1;
    auto n_bits = std::log2(value);
    if(std::isinf(n_bits)) {
        return 1;
    }
    if(n_bits == 0) {
        return 1;
    }
    return static_cast<uint64_t>(std::ceil(n_bits));
}

hdl_integer hdl_integer::operator+(const hdl_integer &o) const {
    hdl_integer res;
    if (o.wide && wide) {
        auto res_val = o.wide_value + wide_value;
        res.set_value(res_val);
    }else if (o.wide && !wide) {
        auto res_val = o.wide_value + int1024_t(value);
        res.set_value(res_val);
    }else if (!o.wide && wide) {
        auto res_val = int1024_t(o.value) + wide_value;
        res.set_value(res_val);
    }else if (!o.wide && !wide) {
        return o.value + value;
    }
    return res;
}

hdl_integer hdl_integer::operator-(const hdl_integer &o) const {
    return value - o.value;
}

hdl_integer hdl_integer::operator*(const hdl_integer &o) const {
    return value * o.value;
}

hdl_integer hdl_integer::operator/(const hdl_integer &o) const {
    if(o.value == 0) return 0;
    return value / o.value;
}

hdl_integer hdl_integer::operator%(const hdl_integer &o) const {
    if(o.value == 0) return 0;
    return value % o.value;
}

hdl_integer hdl_integer::operator&&(const hdl_integer &o) const {
    return value && o.value;
}

hdl_integer hdl_integer::operator||(const hdl_integer &o) const {
    return value || o.value;
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
    if(o.value >= 64) return 0;
    return value << o.value;
}

hdl_integer hdl_integer::operator>>(const hdl_integer &o) const {
    if(o.value >= 64) return 0;
    return value >> o.value;
}
