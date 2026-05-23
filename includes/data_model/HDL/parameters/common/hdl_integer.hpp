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


#ifndef ANANKE_HDL_INTEGER_HPP
#define ANANKE_HDL_INTEGER_HPP

#include <cstdint>

class hdl_integer {

public:
    hdl_integer() = default;
    hdl_integer(int64_t val){value = val;}
    void set_size(const int64_t v) {value = v;}
    void set_value(const uint64_t s) {size = s;}
    void set_signed(const bool s) {signedness = s;}

private:
    int64_t value = 0;
    uint64_t size = 0;
    bool signedness = false;
};



#endif //ANANKE_HDL_INTEGER_HPP
