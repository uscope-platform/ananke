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
#include "frontend/analysis/preprocessor/conditional_solver.hpp"
#include "frontend/analysis/preprocessor/macro_processor.hpp"
#include "data_model/mm_file.hpp"

namespace preprocessor {
    class sv_preprocessor {
    public:
        sv_preprocessor() = default;
        std::string preprocess(const std::string_view &file_content);
        std::string flatten_source(const std::string_view &file_content);
        void set_include_directories(const std::vector<std::string> &i_d){include_directories = i_d;}
        void set_path(const std::string &s){path = s;}
        std::vector<std::string> get_documentation_comments() {return documentation_comments;}
    private:
        typedef std::unordered_map<std::string, std::variant<std::string, function_macro>> definitions_map;
        std::optional<std::string> parse_include_path(const std::string_view &v);
        std::string get_define_replacement(const std::string_view &v);
        void parse_definition(const std::string_view &sv, int prefix_length);
        static std::string_view parse_one_arg_directive(const std::string_view &sv, int prefix_length);
        uint64_t line_number;
        definitions_map definitions;
        std::vector<std::string> documentation_comments;
        std::string path;
        conditional_solver c_solver;
        std::vector<std::string> include_directories;

        static constexpr auto identifier_pattern = ctre::search<R"(`([a-zA-Z_][a-zA-Z0-9_]*)(\s*\()*)">;
    };
}



#endif //ANANKE_SV_PREPROCESSOR_HPP
