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

#include <cstdlib>

int main(int argc, char **argv) {
    // NOTE: the production crash handler (crash_context.hpp) calls _exit(1) on
    // SIGSEGV/SIGABRT, which would kill the whole suite on the first crashing
    // test. It is intentionally NOT installed here: gtest's per-test [ RUN ]
    // lines identify the culprit and the remaining tests still execute.
    spdlog::set_level(spdlog::level::trace);
    // Point bus spec resolution at the in-tree specs unless the runner
    // already provided some (e.g. an installed tree under test). Without
    // this, non-debug builds bake SPECS_FOLDER as the install location and
    // every spec-dependent test fails on a missing file.
    setenv("ANANKE_SPECS_DIR", TEST_SPECS_FOLDER, 0);
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
