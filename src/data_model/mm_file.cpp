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


#include "data_model/mm_file.hpp"


mm_file::mm_file(const std::string &file) {
    fd = open(file.c_str(), O_RDONLY);
    if (fd == -1) throw std::runtime_error("Match error: open");

    struct stat sb;
    if (fstat(fd, &sb) == -1) {
        close(fd);
        throw std::runtime_error("Match error: fstat");
    }
    file_size = sb.st_size;

    addr = mmap(nullptr, file_size, PROT_READ, MAP_PRIVATE, fd, 0);
    if (addr == MAP_FAILED) {
        close(fd);
        throw std::runtime_error("Match error: mmap");
    }
}

mm_file::~mm_file() {
    if (addr != MAP_FAILED) munmap(addr, file_size);
    if (fd != -1) close(fd);
}

std::string_view mm_file::view() {
    return {static_cast<const char*>(addr), file_size};
}

