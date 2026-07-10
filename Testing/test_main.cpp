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

void test_sigsegv_handler(int sig) {
    std::cerr << "\nSEGFAULT - Backtrace:" << std::endl;
    void* bt[32];
    int n = backtrace(bt, 32);
    backtrace_symbols_fd(bt, n, STDERR_FILENO);
    std::cerr << std::endl;
    _exit(1);
}

int main(int argc, char **argv) {
    signal(SIGSEGV, test_sigsegv_handler);
    spdlog::set_level(spdlog::level::trace);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
