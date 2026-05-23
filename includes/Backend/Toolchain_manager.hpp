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

#ifndef ANANKE_TOOLCHAIN_MANAGER_HPP
#define ANANKE_TOOLCHAIN_MANAGER_HPP


#include <string>
#include <iostream>
#include <vector>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <filesystem>
#include <spawn.h>
#include <utility>
#include <gtest/gtest.h>

#include "data_model/settings_store.hpp"

class Toolchain_manager {
public:
    Toolchain_manager() = default;
    Toolchain_manager(std::shared_ptr<settings_store> s, bool del_mkfile, std::string name);
    virtual void create_project(const std::string& makefile, bool start_gui) {};

protected:
    FRIEND_TEST(Toolchain_manager, str_vect_to_char_p);
    FRIEND_TEST(Toolchain_manager, spawn_process);
    static std::vector<const char *> str_vect_to_char_p(const std::vector<std::string>& vect);
    static void spawn_process(const std::vector<std::string>&arg_v, bool daemonize, bool block);

    bool delete_makefile;
    std::string project_name;
    std::shared_ptr<settings_store> s_store;
};
#endif //ANANKE_TOOLCHAIN_MANAGER_HPP
