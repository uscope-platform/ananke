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


#include "frontend/analysis/preprocessor/macro_processor.hpp"


namespace preprocessor {
    macro_processor::macro_processor(
    std::unordered_map<std::string, std::variant<std::string, function_macro>> &d,
        uint64_t &l_n,
        std::string &p) : definitions(d), line_number(l_n), path(p)
    {
    }

    std::string macro_processor::process_macro(const std::string_view &in) {
        if (!in.contains('`')) return std::string(in);
        bool  expansion_needed = true;
        std::string result;
        std::string remaining = std::string(in);
        int nesting_counter = 0;
        while (expansion_needed && nesting_counter <1000) {
            result.clear();
            while (auto match = identifier_pattern(remaining)) {
                result.append(remaining.begin(), match.begin());
                if (match.view().back() == '(') {
                    auto id_view = match.view().substr(1, match.size()-2);
                    auto id = std::string(trim(id_view));
                    size_t start_pos = (match.data() + match.size()) - remaining.data();
                    auto args_text = remaining.substr(start_pos);
                    auto [args, rest_of_line] = get_call_arguments(args_text);
                    if (!definitions.contains(id)) {
                        throw std::runtime_error(
                           fmt::format("Attempted to use undefined macro {} in file {}", id, path)
                       );
                    }
                    auto macro = definitions.at(id);
                    if (std::holds_alternative<std::string>(macro)) {
                        throw std::runtime_error(
                            fmt::format("Attempted to pass arguments to a macro {} that does not need them in file {}", id, path)
                        );
                    }
                    auto macro_text = replace_function_macro(args,std::get<function_macro>(macro));
                    if (!macro_text.has_value()) {
                        throw std::runtime_error(
                            fmt::format("Attempted to call a macro [{}] without enough parameters ", id)
                        );
                    }
                    result.append(macro_text.value());
                    remaining = rest_of_line;
                } else{
                    result.append(get_define_replacement(match));

                    remaining = std::string_view{match.end(), remaining.end()};
                }
            }

            result.append(remaining);
            expansion_needed = result.contains('`');
            remaining = result;
            nesting_counter++;
        }

        return result;
    }


    std::pair<std::vector<std::string_view>, std::string_view> macro_processor::get_call_arguments(const std::string_view &in) {
        std::vector<std::string_view> arguments;
        int nesting_level = 0;
        int args_last = 0;
        bool in_string_literal = false;
        for (; args_last< in.size(); args_last++) {
            if (in[args_last]=='"') {
                if (args_last == 0 || in[args_last-1] != '\\') {
                    in_string_literal = !in_string_literal;
                }

            }
            if (in[args_last] == '(' && !in_string_literal) nesting_level++;
            if (in[args_last] == ')'&& !in_string_literal) {
                if (nesting_level>0) nesting_level--;
                else break;
            }
        }

        if (args_last == in.size()) {
            return {};
        }
        in_string_literal = false;
        auto raw_arguments = in.substr(0, args_last);
        int current_arg_start = 0;
        for (int i = 0; i< raw_arguments.size(); i++) {
            const auto c = raw_arguments[i];
            if (c=='"') {
                if (i == 0 || raw_arguments[i-1] != '\\') {
                    in_string_literal = !in_string_literal;
                }
            }

            if ((c == '(' || c == '[' || c == '{') && !in_string_literal) nesting_level++;
            if ((c == ')' || c == ']' || c == '}') && !in_string_literal) nesting_level--;
            if (c==',' && nesting_level == 0||i == raw_arguments.size()-1) {
                if (!in_string_literal) {
                    auto string_length = i-current_arg_start+1;
                    if (c==',') string_length--;
                    auto arg_text = raw_arguments.substr(current_arg_start, string_length);
                    current_arg_start = i+1;
                    arguments.emplace_back(ltrim(arg_text));
                }
            }
        }

        if (!raw_arguments.empty() && raw_arguments.back() == ',') {
            arguments.emplace_back("");
        }

        auto value =  in.substr(args_last+1);
        return {arguments, value};
    }


    std::string_view macro_processor::ltrim(const std::string_view &in) {
        if (in.empty()) {
            return in;
        }
        const auto start = in.find_first_not_of(" \t");
        if (start == std::string_view::npos) {
            return in;
        }
        return in.substr(start);
    }

    std::string_view macro_processor::trim(const std::string_view &in) {
        if (in.empty())
            return in;
        const auto start = in.find_first_not_of(" \t");
        if (start == std::string_view::npos)
            return "";
        const auto end = in.find_last_not_of(" \t\r\n");
        return in.substr(start, end - start + 1);
    }


    std::optional<std::string> macro_processor::replace_function_macro(const std::vector<std::string_view> &args, const function_macro &macro) {
        std::unordered_map<std::string_view, std::string> arguments_map;
        bool full_default = true;
        for (int i = 0; i<macro.arguments.size(); i++) {
            full_default &= macro.arguments[i].has_default;
            if (i >= args.size() || args[i].empty()) {
                arguments_map[macro.arguments[i].name] = macro.arguments[i].default_value;
            } else {
                arguments_map[macro.arguments[i].name] = std::string(args[i]);
            }
        }
        if (args.empty() && !full_default) {
            return std::nullopt;
        }

        std::string_view body = macro.value;
        if (body.empty()) return "";
        while (!body.empty() && (body.back() == ' ' || body.back() == '\t' || body.back() == '\r' || body.back() == '\n' || body.back() == '\\')) {
            body.remove_suffix(1);
        }
        std::vector<std::string_view> tokens;

        auto is_valid_id_char = [](char c) {
            return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '$';
        };

        bool in_token = is_valid_id_char(body[0]);
        int current_token_start = 0;
        for (int i = 0; i<body.size(); i++){
            if (body[i] == '`' && i + 1 < body.size() && body[i + 1] == '"') {
                if (i > current_token_start) {
                    tokens.push_back(body.substr(current_token_start, i - current_token_start));
                }
                tokens.push_back(body.substr(i, 2));
                i++; // Advance loop pointer past the double quote
                current_token_start = i + 1;
                if (current_token_start < body.size()) in_token = is_valid_id_char(body[current_token_start]);
                continue;
            }

            bool current_char_valid = is_valid_id_char(body[i]);


            if (in_token & !current_char_valid) {
                in_token = false;
                tokens.push_back(body.substr(current_token_start, i-current_token_start));
                current_token_start = i;
            } else if (!in_token & current_char_valid) {
                in_token = true;
                tokens.push_back(body.substr(current_token_start, i-current_token_start));
                current_token_start = i;
            }
        }

        if (current_token_start < body.size()) {
            tokens.push_back(body.substr(current_token_start));
        }

        std::vector<std::string> expanded_tokens;
        expanded_tokens.reserve(tokens.size());
        for (const auto &t: tokens) {
            if (arguments_map.contains(t)) {
                expanded_tokens.push_back(arguments_map[t]);
            } else {
                expanded_tokens.emplace_back(t);
            }
        }

        std::string result;
        for (size_t i = 0; i < expanded_tokens.size(); ++i) {
            // Evaluates the cleanly isolated stringification token sequence back into standard quotes
            if (expanded_tokens[i] == "`\"") {
                result.push_back('"');
                continue;
            }

            if (expanded_tokens[i] == "``") {
                // Trim trailing whitespace from the result accumulated so far
                while (!result.empty() && (result.back() == ' ' || result.back() == '\t')) {
                    result.pop_back();
                }

                // Skip the `` operator and any immediate whitespace following it
                while (i + 1 < expanded_tokens.size()) {
                    const auto& next_tok = expanded_tokens[i + 1];
                    if (next_tok.find_first_not_of(" \t") == std::string::npos) {
                        i++; // consume whitespace token
                    } else {
                        break;
                    }
                }
            } else {
                result.append(expanded_tokens[i]);
            }
        }
        return result;
    }



    std::string macro_processor::get_define_replacement(const std::string_view &identifier) {
        std::string_view purged_identifier = {identifier.begin()+1, identifier.end()};
        std::string replacement;
        if (purged_identifier == "__FILE__") {
            replacement = "\"" + path + "\"";

        } else if (purged_identifier == "__LINE__"){
            replacement = std::to_string(line_number);
        } else {
            // Prevent embedded structural directives from being evaluated as substitution macros
            if (purged_identifier == "ifdef" || purged_identifier == "ifndef" ||
                purged_identifier == "else"  || purged_identifier == "elsif"  || purged_identifier == "endif") {
                return std::string(identifier);
            }
            auto id = std::string(purged_identifier);
            if (!definitions.contains(id)) {
                throw std::runtime_error(fmt::format("{}:{} MACRO {} is not defined", path, line_number, id));
            }
            auto def = definitions.at(id);
            if (std::holds_alternative<std::string>(def)) {
                replacement = std::get<std::string>(def);
            }
        }
        return replacement;
    }

    std::optional<function_macro> macro_processor::parse_function_macro(const std::string_view &in) {
        function_macro macro;
        auto [arguments, body] = get_call_arguments(in);

        for (auto &m:arguments) {
            function_macro_argument arg;
            if (m.contains('=')) {
                arg.has_default = true;
                arg.default_value = trim(m.substr(m.find_first_of('=')+1));
                arg.name = trim(m.substr(0, m.find_first_of('=')));
            } else {
                arg.name = trim(m);
                arg.has_default = false;
            }
            macro.arguments.emplace_back(arg);
        }
        macro.value = ltrim(body);
        return macro;
    }
}