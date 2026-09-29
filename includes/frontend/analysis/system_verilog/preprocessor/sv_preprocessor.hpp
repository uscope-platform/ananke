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

#ifndef ANANKE_SV_PREPROCESSOR_HPP
#define ANANKE_SV_PREPROCESSOR_HPP

#include <ctre.hpp>
#include <filesystem>
#include <vector>
#include <fstream>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <iterator>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "frontend/analysis/system_verilog/preprocessor/conditional_solver.hpp"
#include "frontend/analysis/system_verilog/preprocessor/source_mapper.hpp"
#include "frontend/analysis/system_verilog/preprocessor/macro_processor.hpp"
#include "frontend/repository_index.hpp"
#include "data_model/include_dependency.hpp"
#include "data_model/mm_file.hpp"

namespace preprocessor {
    class sv_preprocessor {
    public:
        sv_preprocessor() = default;
        std::string preprocess(const std::string_view &file_content) {return  preprocess(file_content, 1);}
        std::string preprocess(const std::string_view &file_content, unsigned int initial_output_line);
        std::string flatten_source(const std::string_view &file_content);
        void set_include_directories(const std::set<std::string> &i_d){include_directories = i_d;}
        void set_repository_index(const std::shared_ptr<repository_index> &idx){repo_idx = idx;}
        void set_path(const std::string &s){path = s;}
        void set_defines(const std::set<std::string> &d) {global_defines = d;}
        std::vector<std::string> get_documentation_comments() {return documentation_comments;}
        std::set<include_dependency> get_includes() {return includes;}
        source_map_t get_source_map() const {return  source_map.get_map();}
        [[nodiscard]] bool has_error() const {return error.has_value();}
        [[nodiscard]] const std::string& get_error() const {return error.value();}
    private:
        void report_error(const std::string &msg);
        std::string post_process_macro_expansion(const std::string &text);
        std::string gather_multi_line_macro(const std::string &first_line, std::istringstream &iss);
        typedef std::unordered_map<std::string, std::variant<std::string, function_macro>> definitions_map;
        std::optional<include_dependency> parse_include_path(const std::string_view &v);
        std::optional<include_dependency> resolve_include(const std::string &name, bool quoted);
        std::string get_define_replacement(const std::string_view &v);
        void parse_definition(const std::string_view &sv, int prefix_length);
        void install_global_defines();
        static std::string_view parse_one_arg_directive(const std::string_view &sv, int prefix_length);
        uint64_t line_number = 1;
        definitions_map definitions;
        std::set<std::string> global_defines;
        std::vector<std::string> documentation_comments;
        std::string path;
        std::optional<std::string> error;
        std::set<std::string> active_includes;
        conditional_solver c_solver;
        std::set<std::string> include_directories;
        std::shared_ptr<repository_index> repo_idx;
        std::set<include_dependency> includes;
        source_mapper source_map;
        unsigned int output_line_n = 0;
        bool disable_preprocessor = false;
        static constexpr auto identifier_pattern = ctre::search<R"(`([a-zA-Z_][a-zA-Z0-9_]*)(\s*\()*)">;
    };
}



#endif //ANANKE_SV_PREPROCESSOR_HPP
