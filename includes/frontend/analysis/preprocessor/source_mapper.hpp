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


#ifndef ANANKE_SOURCE_MAPPER_HPP
#define ANANKE_SOURCE_MAPPER_HPP

#include <string>
#include <vector>
#include <optional>

class source_mapper {
public:
    struct source_range {
        unsigned int start_line;
        unsigned int end_line;
        std::string source_path;
    };
    std::optional<std::string> get_path(unsigned int line_n);
    void add_range(unsigned int start, unsigned int stop, const std::string &path);
    void open_range(unsigned int line_n,const std::string &path);
    void close_range(unsigned int line_n);
    [[nodiscard]] std::vector<source_range> get_map() const {return map;}
private:
    std::vector<source_range> map;
    std::vector<source_range> ranges_stack;
    source_range current_range{};
    bool constructing_range = false;

};


#endif //ANANKE_SOURCE_MAPPER_HPP
