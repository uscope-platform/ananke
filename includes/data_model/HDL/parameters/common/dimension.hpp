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


#ifndef ANANKE_DIMENSION_HPP
#define ANANKE_DIMENSION_HPP

#include "data_model/HDL/parameters/components/Parameter_value_base.hpp"

typedef struct dims_struct{
    std::shared_ptr<Parameter_value_base> first_bound;
    std::shared_ptr<Parameter_value_base> second_bound;
    bool packed;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(first_bound, second_bound, packed);
    }
    bool operator==(const dims_struct& other) const {
        bool ret = true;
        ret &= *first_bound == *other.first_bound;
        ret &= *second_bound == *other.second_bound;
        ret &= packed == other.packed;
        return ret;
    };

    friend ::std::ostream& operator<<(::std::ostream& os, const dims_struct& dim) {
        return os << "dimension_t { "
                  << "first_bound: " << dim.first_bound->print() << ", "
                  << "second_bound: " << dim.second_bound->print() << ", "
                  << "packed: " << (dim.packed ? "true" : "false")
                  << " }";
    }

} dimension_t;

#endif //ANANKE_DIMENSION_HPP