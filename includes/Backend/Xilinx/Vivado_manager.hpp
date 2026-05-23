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

#ifndef ANANKE_VIVADO_MANAGER_HPP
#define ANANKE_VIVADO_MANAGER_HPP

#include <string>
#include <spdlog/spdlog.h>
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
#include "Backend/Toolchain_manager.hpp"

class Vivado_manager :public Toolchain_manager {
public:
    Vivado_manager(std::shared_ptr<settings_store> s, bool del_mkfile, std::string name);
    void create_project(const std::string& makefile, bool start_gui) override;

protected:

    FRIEND_TEST(Toolchain_manager, vivado_manager);
    std::vector<std::string> prepare_call(const std::string& makefile);

    std::string vivado_path;
};


#endif //ANANKE_VIVADO_MANAGER_HPP
