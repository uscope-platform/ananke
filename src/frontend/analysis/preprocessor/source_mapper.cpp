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


#include "frontend/analysis/preprocessor/source_mapper.hpp"


std::optional<std::string> source_mapper::get_path(unsigned int line_n, std::vector<source_range>map) {
    for (auto &[start, stop, path]: map) {
        if (line_n>=start && line_n <= stop) return path;
    }
    return std::nullopt;
}

void source_mapper::add_range(unsigned int start, unsigned int stop, const std::string &path) {
    map.emplace_back(start, stop, path);
}

void source_mapper::open_range(unsigned int line_n, const std::string &path) {
    current_range = {line_n, line_n, path};
}

void source_mapper::close_range(unsigned int line_n) {
    current_range.end_line = line_n-1;
    map.push_back(current_range);
    current_range = {};
}