// Copyright 2021 University of Nottingham Ningbo China
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include <gtest/gtest.h>
#include <spdlog/spdlog.h>

#include <signal.h>
#include <execinfo.h>
#include <unistd.h>
#include <sys/wait.h>

void sigsegv_handler(int sig) {
    void* bt[32];
    int n = backtrace(bt, 32);

    char exe[1024] = {0};
    ssize_t exe_len = readlink("/proc/self/exe", exe, sizeof(exe) - 1);
    if (exe_len <= 0) exe[0] = '\0';
    else exe[exe_len] = '\0';

    dprintf(STDERR_FILENO, "\nSEGFAULT - Backtrace:\n");

    for (int i = 0; i < n; i++) {
        dprintf(STDERR_FILENO, "  #%d ", i);
        if (exe[0]) {
            pid_t pid = fork();
            if (pid == 0) {
                char addr[32];
                snprintf(addr, sizeof(addr), "%p", bt[i]);
                execlp("addr2line", "addr2line", "-e", exe, "-f", "-p", addr, nullptr);
                _exit(1);
            } else if (pid > 0) {
                waitpid(pid, nullptr, 0);
            } else {
                dprintf(STDERR_FILENO, "%p\n", bt[i]);
            }
        } else {
            dprintf(STDERR_FILENO, "%p\n", bt[i]);
        }
    }
    _exit(1);
}

int main(int argc, char **argv) {
    signal(SIGSEGV, sigsegv_handler);
    spdlog::set_level(spdlog::level::trace);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
