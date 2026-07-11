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


#ifndef ANANKE_MACRO_PROCESSOR_HPP
#define ANANKE_MACRO_PROCESSOR_HPP

#include <string>
#include <optional>
#include <vector>
#include <unordered_map>
#include <variant>
#include <algorithm>
#include <ctre.hpp>
#include <utility>
#include <memory>
#include <fmt/format.h>

namespace preprocessor {
    struct function_macro_argument {
        std::string name;
        std::string default_value;
        bool has_default;

        friend bool operator==(const function_macro_argument &lhs, const function_macro_argument &rhs) {
            return lhs.name == rhs.name
                   && lhs.default_value == rhs.default_value
                   && lhs.has_default == rhs.has_default;
        }


    };

    struct function_macro {

        std::vector<function_macro_argument> arguments;
        std::string value;

        friend bool operator==(const function_macro &lhs, const function_macro &rhs) {
            return lhs.arguments == rhs.arguments && lhs.value == rhs.value;
        }

    };

    static constexpr auto identifier_pattern = ctre::search<R"(`([a-zA-Z_][a-zA-Z0-9_]*)(\s*\()?)">;

    class macro_processor {
    public:
        macro_processor(
            std::unordered_map<std::string, std::variant<std::string, function_macro>> &d,
            uint64_t &l_n,
            std::string &path
        );
        std::string process_macro(const std::string_view &in);
        static std::pair<std::vector<std::string_view>, std::string_view> get_call_arguments(const std::string_view &in);
        std::optional<std::string> replace_function_macro(
            const std::vector<std::string_view> &args,
            const function_macro &macro
        );
        std::string get_define_replacement(const std::string_view &identifier);
        static std::string_view ltrim(const std::string_view &in);
        static std::string_view trim(const std::string_view &in);
        void set_path(const std::string &p) {path = p;}
        void set_line_number(const unsigned int &n) {line_number = n;}


        static std::optional<function_macro> parse_function_macro(const std::string_view &in);

    private:
        std::string &path;
        uint64_t &line_number;
        std::unordered_map<std::string, std::variant<std::string, function_macro>> &definitions;
    };
}
#endif //ANANKE_MACRO_PROCESSOR_HPP
