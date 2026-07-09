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


#include "frontend/analysis/preprocessor/sv_preprocessor.hpp"




namespace preprocessor {


    std::string sv_preprocessor::preprocess(const std::string_view &file_content, unsigned int initial_output_line) {
        macro_processor macro_engine(definitions , line_number, path);

        auto flat_source= flatten_source(file_content);
        std::istringstream iss(flat_source);
        std::ostringstream out;
        output_line_n = initial_output_line;
        source_map.open_range(output_line_n, path);

        std::string line;
        line_number = 1;

        while (std::getline(iss, line)) {
            std::string_view trimmed_line = macro_processor::ltrim(line);

            bool skipped_directive = trimmed_line.starts_with("`resetall")
            || trimmed_line.starts_with("`timescale")
            || trimmed_line.starts_with("`default_nettype")
            || trimmed_line.starts_with("`nounconnected_drive")
            || trimmed_line.starts_with("`unconnected_drive")
            || trimmed_line.starts_with("`endcelldefine")
            || trimmed_line.starts_with("`celldefine")
            || trimmed_line.starts_with("`pragma")
            || trimmed_line.starts_with("`end_keywords")
            || trimmed_line.starts_with("`begin_keywords")
            || trimmed_line.starts_with("`line");
            std::string uncommented_line = line;
            if (line.contains("//")) {
                uncommented_line = line.substr(0, line.find("//"));
                auto comment = line.substr(line.find("//"));
                if (comment.contains("pragma translate_off")) disable_preprocessor = true;
                if (comment.contains("pragma translate_on")) disable_preprocessor = false;
            }
            if (disable_preprocessor) uncommented_line = "";

            if (trimmed_line.starts_with("`define") && c_solver.is_active()) {
                parse_definition(trimmed_line, 7);
            } else if (trimmed_line.starts_with("`include") && c_solver.is_active()) {

                auto included_file = parse_include_path(trimmed_line);
                if (included_file.has_value()) {
                    auto saved_path = path;
                    auto saved_line = line_number;
                    path = included_file.value();
                    source_map.close_range(output_line_n);

                    try {
                        mm_file file(path);
                        auto content = preprocess(file.view(), output_line_n);
                        out << content + '\n';
                    } catch (...) {
                        path = saved_path;
                        line_number = saved_line;
                        source_map.open_range(output_line_n, path);
                        throw;
                    }

                    line_number = saved_line;
                    path = saved_path;
                    source_map.open_range(output_line_n, path);
                }
            } else if (trimmed_line.starts_with("`ifdef")) {
                auto condition = parse_one_arg_directive(trimmed_line, 6);
                c_solver.start_loop(definitions.contains(std::string(condition)));
            } else if (trimmed_line.starts_with("`ifndef")) {
                auto condition = parse_one_arg_directive(trimmed_line, 7);
                c_solver.start_loop(!definitions.contains(std::string(condition)));
            } else if (trimmed_line.starts_with("`elsif")) {
                auto condition = parse_one_arg_directive(trimmed_line, 6);
                c_solver.advance_elseif(definitions.contains(std::string(condition)));
            } else if (trimmed_line.starts_with("`else")) {
                c_solver.advance_else();
            } else if (trimmed_line.starts_with("`endif")) {
                c_solver.close_loop();
            } else if (trimmed_line.starts_with("`undef") && c_solver.is_active()) {
                auto identifier = parse_one_arg_directive(trimmed_line, 6);
                definitions.erase(std::string(identifier));
            } else if (trimmed_line.starts_with("`undefineall") && c_solver.is_active()) {
                definitions.clear();
            } else if (!skipped_directive && c_solver.is_active()) {
                std::string output_line;
                if (uncommented_line.empty()) {
                    output_line = '\n';
                } else {
                    if (uncommented_line.contains('`')) {
                        uncommented_line = gather_multi_line_macro(uncommented_line, iss);
                    }

                    output_line = macro_engine.process_macro(uncommented_line) + '\n';
                }
                output_line_n += std::count(output_line.begin(), output_line.end(), '\n');
                out << output_line;
            }
            line_number++;
        }
        auto retval = out.str();
        if (!retval.empty()) retval.pop_back();
        source_map.close_range(output_line_n);
        return retval;
    }


    std::optional<std::string> sv_preprocessor::parse_include_path(const std::string_view &line) {
        auto start_identifier = line.find_first_of("\"<");
        std::string file_path;
        if (start_identifier == std::string_view::npos) {
            throw std::runtime_error(fmt::format("Malformed include [{}] at line {} in file: {}", line,line_number, path));
        }else if (line[start_identifier] == '\"') {
            auto end_identifier = line.substr(start_identifier+1).find_first_of('"');
            auto name = line.substr(start_identifier+1, end_identifier);
            if (name.starts_with('/')) return std::string(name);
            std::string full_path;
            auto rel_path = std::string(std::filesystem::path(path).parent_path()/name);
            if (std::filesystem::exists(rel_path)) {
                full_path = rel_path;
            } else {
                for (std::filesystem::path dir: include_directories) {
                    auto tmp_path = std::string(dir/name);
                    if (std::filesystem::exists(tmp_path)) full_path = tmp_path;
                }
            }
            if (full_path.empty()) {
                spdlog::warn("include file not found: {}", std::string(name));
                return {};
            }
            return full_path;
        } else {
            auto filename = std::string(line.substr(start_identifier+1, line.find_first_of('>')- start_identifier-1));
            for (std::filesystem::path dir: include_directories) {
                auto full_path = dir/ filename;
                if (std::filesystem::exists(full_path)) return full_path;
            }
            throw std::runtime_error(fmt::format("included file not found: {}", filename));
        }
    }

    std::string sv_preprocessor::get_define_replacement(const std::string_view &identifier) {
        std::string_view purged_identifier = {identifier.begin()+1, identifier.end()};
        std::string replacement;
        if (purged_identifier == "__FILE__") {
            replacement = "\"" + path + "\"";

        } else if (purged_identifier == "__LINE__"){
            replacement = std::to_string(line_number);
        } else {
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

    void sv_preprocessor::parse_definition(const std::string_view &sv, int prefix_length) {
        auto trimmed_view = sv.substr(prefix_length);
        trimmed_view = trimmed_view.substr(trimmed_view.find_first_not_of("\t "));
        auto id_last = trimmed_view.find_first_of("\t (");
        auto identifier = trimmed_view.substr(0, id_last);
        if (id_last == std::string_view::npos) {
            definitions[std::string(identifier)] = "";
            return;
        }
        auto remaining_view = trimmed_view.substr(id_last+1);
        if (trimmed_view[id_last] == '(') {
            auto macro = macro_processor::parse_function_macro(trimmed_view.substr(id_last+1));
            if (!macro.has_value()) {
                throw std::runtime_error(fmt::format("The arguments list in macro {} is never closed [{}]", identifier, path));
            }
            definitions[std::string(identifier)] = macro.value();
        } else {
            auto value = remaining_view.substr(remaining_view.find_first_not_of("\t "));
            definitions[std::string(identifier)] =  std::string{value};
        }

    }

    std::string_view sv_preprocessor::parse_one_arg_directive(const std::string_view &sv, int prefix_length) {
        auto trimmed_line = sv.substr(prefix_length);
        return trimmed_line.substr( trimmed_line.find_first_not_of(' '));
    }


    std::string sv_preprocessor::flatten_source(const std::string_view &content) {

        std::string result;
        result.reserve(content.size());
        size_t cursor = 0;
        bool in_string = false;
        bool in_macro = false;

        // Source flattening uses a search and append approach to be cache and SIMD friendly
        // The main loop keeps searching for potentially problematic characters and appends as needed
        while (cursor < content.size()) {

            size_t next_identifier = content.find_first_of("/\"\\`\n\r", cursor);

            if (next_identifier == std::string_view::npos) {
                result.append(content.substr(cursor));
                break;
            }

            if (next_identifier > cursor) {
                result.append(content.substr(cursor, next_identifier - cursor));
            }
            cursor = next_identifier;
            char trigger = content[cursor];

            if (trigger == '\n' || trigger == '\r') {
                in_macro = false;
                result.push_back(trigger);
                cursor++;
            } else if (trigger == '`') {
                auto directive = content.substr(cursor);
                if (directive.starts_with("`define")) {
                    in_macro = true;
                }
                result.push_back('`');
                cursor++;
            } else if (trigger == '/') {
                if (cursor != content.size()-1) {
                    if (content[cursor + 1] == '/') {
                        result.append("//");
                        cursor += 2;
                        while (cursor < content.size() && content[cursor] != '\n' && content[cursor] != '\r') {
                            if (content[cursor] == '\\' && (content[cursor+1] == '\n' || content[cursor+1] == '\r')) {
                                size_t next = content.find_first_of("\n\r", cursor + 1);
                                if (next != std::string_view::npos) {
                                    cursor = next + (content[next] == '\r' && content[next+1] == '\n' ? 2 : 1);
                                    continue; // Skip backslash and newline, stay in comment
                                }
                            }
                            result.push_back(content[cursor++]);
                        }
                    } else if (content[cursor + 1] == '*') {

                        size_t end = content.find("*/", cursor + 2);
                        if (content[cursor + 2] == '*') {
                            documentation_comments.emplace_back(content.substr(cursor+3, end - cursor-4));
                        }
                        if (end == std::string_view::npos) end = content.size() - 2;
                        cursor = end + 2;
                    }
                    else {
                        result.push_back('/');
                        cursor++;
                    }
                } else {
                    result.push_back('/');
                    cursor++;
                }
            } else if (trigger == '"') {
                if (in_string) {
                    in_string = false;
                } else {
                    in_string = true;
                }
                result.push_back('\"');
                cursor++;
            } else if (trigger == '\\') {
                if (cursor + 1 < content.size()) {
                    char next = content[cursor + 1];

                    // skip backslash and newline when necessary
                    if (next == '\n' || next == '\r') {
                        if (in_macro || in_string) {
                            size_t skip_len = (next == '\r' && content[cursor+2] == '\n') ? 3 : 2;

                            if (cursor + skip_len < content.size()) {
                                char next_char = content[cursor + skip_len];

                                // Check if BOTH flanking sides are valid identifier characters
                                bool left_is_word = !result.empty() && (std::isalnum(static_cast<unsigned char>(result.back())) || result.back() == '_');
                                bool right_is_word = std::isalnum(static_cast<unsigned char>(next_char)) || next_char == '_';

                                // Check that the character before the backslash isn't already whitespace
                                bool already_has_space = !result.empty() && (result.back() == ' ' || result.back() == '\t');

                                if (left_is_word && right_is_word && !already_has_space) {
                                    result.push_back(' ');
                                }
                            }

                            cursor += skip_len;
                            continue;
                        }
                    }

                    //keep escaped quotes as is
                    if (in_string && next == '"') {
                        result.push_back('\\');
                        result.push_back('\"');
                        cursor += 2;
                        continue;
                    }
                }
                // pass through normal backslashes
                result.push_back('\\');
                cursor++;
            }
        }

        return result;
    }

    std::string sv_preprocessor::gather_multi_line_macro(const std::string &first_line, std::istringstream &iss) {
        std::string accumulated = first_line;

        // Lambda to evaluate unescaped structural parenthesis balance
        auto count_paren_balance = [](const std::string &str) {
            int balance = 0;
            bool in_string = false;
            for (size_t i = 0; i < str.size(); ++i) {
                if (str[i] == '"' && (i == 0 || str[i-1] != '\\')) {
                    in_string = !in_string;
                }
                if (!in_string) {
                    if (str[i] == '(') balance++;
                    if (str[i] == ')') balance--;
                }
            }
            return balance;
        };

        std::string next_line;
        // Continue pulling raw lines from the stream as long as parentheses are unbalanced
        while (count_paren_balance(accumulated) > 0 && std::getline(iss, next_line)) {
            line_number++; // Crucial: Keeps the preprocessor line tracking 1:1 with the file

            // Strip single-line comments from the lookahead segment before combining
            std::string uncommented_next = next_line;
            if (next_line.contains("//")) {
                uncommented_next = next_line.substr(0, next_line.find("//"));
            }

            accumulated += "\n" + uncommented_next;
        }

        return accumulated;
    }

}
