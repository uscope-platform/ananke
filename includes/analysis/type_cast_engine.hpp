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


#ifndef ANANKE_TYPE_CAST_ENGINE_HPP
#define ANANKE_TYPE_CAST_ENGINE_HPP

#include <cstdint>

class type_cast_engine {
public:
    static int64_t to_unsigned(int64_t in, uint64_t container_size);
    static int64_t to_signed(int64_t in, uint64_t container_size);
    static int64_t to_int(int64_t in, uint64_t container_size);
private:

};

#endif //ANANKE_TYPE_CAST_ENGINE_HPP
