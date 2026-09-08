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
    size = v;
}

void hdl_integer::set_value(const uint64_t v) {
    content = static_cast<int64_t>(v);
}

void hdl_integer::set_value(const wide_integer v) {
    if (v >= wide_integer(INT64_MIN) && v <= wide_integer(INT64_MAX)) {
        content = static_cast<int64_t>(v);
    } else {
        content = v;
    }
}

uint64_t hdl_integer::get_size() const {
    if (size > 0) return size;
    wide_integer tmp;
    if (is_wide()) {
        tmp = std::get<wide_integer>(content);
    } else {
        tmp = get_value();
    }
    if (tmp < 0) tmp = -tmp;
    int bits = 0;
    while (tmp != 0) {
        tmp >>= 1;
        bits++;
    }
    if (bits == 0) bits = 1;
    return static_cast<uint64_t>(bits);
}

hdl_integer hdl_integer::operator+(const hdl_integer &o) const {
    hdl_integer res;
    if (o.is_wide() || is_wide()) {
        res.set_value(to_wide() + o.to_wide());
    } else {
        res.set_value(static_cast<uint64_t>(get_value() + o.get_value()));
    }
    return res;
}

hdl_integer hdl_integer::operator-(const hdl_integer &o) const {
    hdl_integer res;
    if (o.is_wide() || is_wide()) {
        res.set_value(to_wide() - o.to_wide());
    } else {
        res.set_value(static_cast<uint64_t>(get_value() - o.get_value()));
    }
    return res;
}

hdl_integer hdl_integer::operator*(const hdl_integer &o) const {
    hdl_integer res;
    if (o.is_wide() || is_wide()) {
        res.set_value(to_wide() * o.to_wide());
    } else {
        res.set_value(static_cast<uint64_t>(get_value() * o.get_value()));
    }
    return res;
}

hdl_integer hdl_integer::operator/(const hdl_integer &o) const {
    if (o.get_value() == 0) return 0;
    hdl_integer res;
    if (o.is_wide() || is_wide()) {
        auto divisor = o.to_wide();
        if (divisor == 0) return 0;
        res.set_value(to_wide() / divisor);
    } else {
        res.set_value(static_cast<uint64_t>(get_value() / o.get_value()));
    }
    return res;
}

hdl_integer hdl_integer::operator%(const hdl_integer &o) const {
    if (o.get_value() == 0) return 0;
    hdl_integer res;
    if (o.is_wide() || is_wide()) {
        auto divisor = o.to_wide();
        if (divisor == 0) return 0;
        res.set_value(to_wide() % divisor);
    } else {
        res.set_value(static_cast<uint64_t>(get_value() % o.get_value()));
    }
    return res;
}

hdl_integer hdl_integer::operator&&(const hdl_integer &o) const {
    hdl_integer res;
    res.set_value(static_cast<uint64_t>(to_wide() != 0 && o.to_wide() != 0));
    return res;
}

hdl_integer hdl_integer::operator||(const hdl_integer &o) const {
    hdl_integer res;
    res.set_value(static_cast<uint64_t>(to_wide() != 0 || o.to_wide() != 0));
    return res;
}

hdl_integer hdl_integer::operator&(const hdl_integer &o) const {
    hdl_integer res;
    if (o.is_wide() || is_wide()) {
        res.set_value(to_wide() & o.to_wide());
    } else {
        res.set_value(static_cast<uint64_t>(get_value() & o.get_value()));
    }
    return res;
}

hdl_integer hdl_integer::operator|(const hdl_integer &o) const {
    hdl_integer res;
    if (o.is_wide() || is_wide()) {
        res.set_value(to_wide() | o.to_wide());
    } else {
        res.set_value(static_cast<uint64_t>(get_value() | o.get_value()));
    }
    return res;
}

hdl_integer hdl_integer::operator^(const hdl_integer &o) const {
    hdl_integer res;
    if (o.is_wide() || is_wide()) {
        res.set_value(to_wide() ^ o.to_wide());
    } else {
        res.set_value(static_cast<uint64_t>(get_value() ^ o.get_value()));
    }
    return res;
}

hdl_integer hdl_integer::operator~() const {
    hdl_integer res;
    if (is_wide()) {
        res.set_value(-std::get<wide_integer>(content) - 1);
    } else {
        res.set_value(static_cast<uint64_t>(~get_value()));
    }
    return res;
}

hdl_integer hdl_integer::operator!() const {
    hdl_integer res;
    res.set_value(static_cast<uint64_t>(to_wide() == 0));
    return res;
}

hdl_integer hdl_integer::operator<<(const hdl_integer &o) const {
    int64_t shift = o.get_value();
    if (shift < 0 || shift >= MAX_BIT_WIDTH) return hdl_integer(0);
    hdl_integer res;
    if (is_wide() || shift >= 64) {
        res.set_value(to_wide() << static_cast<int>(shift));
    } else {
        res.set_value(static_cast<uint64_t>(get_value() << shift));
    }
    return res;
}

hdl_integer hdl_integer::operator>>(const hdl_integer &o) const {
    int64_t shift = o.get_value();
    if (shift < 0 || shift >= MAX_BIT_WIDTH) return hdl_integer(0);
    hdl_integer res;
    if (is_wide() || shift >= 64) {
        res.set_value(to_wide() >> static_cast<int>(shift));
    } else {
        res.set_value(static_cast<uint64_t>(get_value() >> shift));
    }
    return res;
}
