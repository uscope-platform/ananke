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

#ifndef ANANKE_CRASH_CONTEXT_HPP
#define ANANKE_CRASH_CONTEXT_HPP

#include <string>
#include <csignal>
#include <unistd.h>

struct crash_context {
    void set(std::string ent, std::string f) {
        entity = std::move(ent);
        file = std::move(f);
    }

    std::string entity;
    std::string file;
};

inline thread_local crash_context crash_ctx;

inline void install_crash_handler() {
    struct sigaction sa = {};
    sa.sa_handler = [](int) {
        write(STDERR_FILENO, "\nCRASH while processing: ", 25);
        write(STDERR_FILENO, crash_ctx.entity.data(), crash_ctx.entity.size());
        write(STDERR_FILENO, " (", 2);
        write(STDERR_FILENO, crash_ctx.file.data(), crash_ctx.file.size());
        write(STDERR_FILENO, ")\n", 2);
        _exit(1);
    };
    sigaction(SIGSEGV, &sa, nullptr);
}

#endif
