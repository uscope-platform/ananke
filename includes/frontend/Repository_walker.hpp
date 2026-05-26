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

#ifndef ANANKE_REPOSITORY_WALKER_HPP
#define ANANKE_REPOSITORY_WALKER_HPP

#include <string>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <set>
#include <future>
#include <regex>
#include <utility>
#include <gtest/gtest.h>
#include <openssl/evp.h>
#include <utility>
#include <spdlog/spdlog.h>

#include "data_model/settings_store.hpp"
#include "data_model/Script.hpp"
#include "data_model/Constraints.hpp"
#include "data_model/data_store.hpp"
#include "data_model/DataFile.hpp"
#include "analysis/sv_analyzer.hpp"
#include "analysis/vhdl_analyzer.hpp"
#include "third_party/thread_pool.hpp"

template<typename  T>
struct file_analysis_context {
    std::string path;
    std::string hash;
    std::optional<std::vector<T>> resource;
};

static file_analysis_context<HDL_Resource> analyze_verilog(const std::filesystem::path &file, std::set<std::string> i_d, const std::string &old_hash);
static file_analysis_context<HDL_Resource> analyze_vhdl(const std::filesystem::path &file, std::set<std::string> i_d, const std::string &old_hash);
static file_analysis_context<Script>  analyze_script(const std::filesystem::path &file, std::set<std::string> i_d, const std::string &old_hash);
static file_analysis_context<DataFile>  analyze_data(const std::filesystem::path &file, std::set<std::string> i_d, const std::string &old_hash);
static file_analysis_context<Constraints>  analyze_constraint(const std::filesystem::path &file, std::set<std::string> i_d, const std::string &old_hash);
static std::string hash_file(const std::string_view &file_content);

const unsigned int max_threads = std::thread::hardware_concurrency()-1;

class Repository_walker {

public:
    Repository_walker(const std::shared_ptr<settings_store>& s, const std::shared_ptr<data_store>& d, bool ephimeral);
    Repository_walker(const std::shared_ptr<settings_store>& s, const std::shared_ptr<data_store>& d, bool ephimeral, std::set<std::string> ex);
    void analyze_dir();
private:
    void construct_walker(std::shared_ptr<settings_store> s, std::shared_ptr<data_store> d, std::set<std::string> ex);
    bool is_excluded_directory(const std::filesystem::path& dir);
    bool contains_excluding_file(const std::filesystem::path& dir);
    void read_ignore_file(const std::filesystem::path& file);
    void analyze_file(std::filesystem::path& dir);
    void collect_analysis_results();

    // File type discrimination methods
    // TODO: use these to make file associations dynamic a la vscode
    static bool file_is_verilog(const std::filesystem::path &file);
    static bool file_is_vhdl(const std::filesystem::path &file);
    static bool file_is_script(const std::filesystem::path &file);
    static bool file_is_constraint(const std::filesystem::path &file);
    static bool file_is_data(const std::filesystem::path &file);
    // TODO: Make excluded directories dynamic with a mechanism similar to .gitignore

    FRIEND_TEST(repository_walker , file_type_handling);

    std::set<std::string> excluded_directories;
    std::set<std::string> excluding_extensions = {".xpr"};
    std::string ignore_file_name = ".mkignore";

    std::string target_repository;
    std::shared_ptr<settings_store> s_store;
    std::shared_ptr<data_store> d_store;

    std::vector<std::future<file_analysis_context<HDL_Resource>>> hdl_futures;
    std::vector<std::future<file_analysis_context<Script>>> scripts_futures;
    std::vector<std::future<file_analysis_context<Constraints>>> constraints_futures;
    std::vector<std::future<file_analysis_context<DataFile>>> data_futures;

    thread_pool pool;
    int working_threads = 0;
    std::set<std::string> default_includes;
};


#endif //ANANKE_REPOSITORY_WALKER_HPP
