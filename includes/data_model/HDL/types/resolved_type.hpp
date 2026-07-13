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


#ifndef ANANKE_RESOLVED_TYPE_HPP
#define ANANKE_RESOLVED_TYPE_HPP

#include <vector>
#include <cstdint>
#include <optional>

struct struct_member_resolved_type {
    std::vector<uint64_t> unpacked_sizes;
    std::vector<uint64_t> packed_sizes;
    std::vector<struct_member_resolved_type> members;
};

struct resolved_type {
    std::vector<uint64_t> unpacked_sizes;
    std::vector<bool> unpacked_ascending;
    std::vector<uint64_t> packed_sizes;
    std::vector<bool> packed_ascending;
    std::vector<struct_member_resolved_type> struct_sizes;
    bool packed_struct = false;
    std::optional<bool> return_unpacked_ascending;
};

#endif //ANANKE_RESOLVED_TYPE_HPP
