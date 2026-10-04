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


#include "frontend/analysis/system_verilog/preprocessor/macro_processor.hpp"
#include <spdlog/spdlog.h>
#include <mutex>

static constexpr size_t MAX_MACRO_EXPANSION_SIZE = 64 * 1024 * 1024;
static constexpr int MAX_NONCONVERGING_PASSES = 16;

namespace {
std::mutex stripped_warn_mutex;
bool uvm_warned = false;
bool ovm_warned = false;
}

namespace preprocessor {
    // UVM/OVM macros come from the external simulator library (uvm_macros.svh),
    // which is normally not part of the analyzed repository
    // The warning fires once per macro name per process: on UVM-heavy trees
    // every instance would otherwise format and emit a log line.
    static void warn_stripped_once(const std::string &id, const std::string &path) {
        const char first =
            static_cast<char>(std::tolower(static_cast<unsigned char>(id[0])));
        const char *family = (first == 'u') ? "UVM" : "OVM";
        std::lock_guard<std::mutex> lock(stripped_warn_mutex);
        bool &already_warned = (first == 'u') ? uvm_warned : ovm_warned;
        if (!already_warned) {
            already_warned = true;
            spdlog::warn("Stripping undefined {} macros ({} library not vendored; "
                         "further {} macros stripped silently, e.g. {} in file {})",
                         family, family, family, id, path);
        }
    }
    static bool is_external_methodology_macro(const std::string &id) {
        if (id.size() < 4) return false;
        char c0 = static_cast<char>(std::tolower(static_cast<unsigned char>(id[0])));
        char c1 = static_cast<char>(std::tolower(static_cast<unsigned char>(id[1])));
        char c2 = static_cast<char>(std::tolower(static_cast<unsigned char>(id[2])));
        if (c0 != 'u' && c0 != 'o') return false;
        if (c1 != 'v' || c2 != 'm') return false;
        return id[3] == '_';
    }

    macro_processor::macro_processor(
    macro_definitions_map &d,
        uint64_t &l_n,
        std::string &p,
        std::optional<std::string> &e,
        undefined_uses_map &u,
        bool &f) : definitions(d), line_number(l_n), path(p), error(e), undefined_macros(u), fatal_error(f)
    {
    }

    void macro_processor::report_error(const std::string &msg) {
        spdlog::error(msg);
        if (!error) error = msg;
        fatal_error = true;
    }

    void macro_processor::report_undefined(const std::string &id, std::optional<int> arg_count) {
        if (arg_count.has_value()) {
            undefined_macros[id].insert(arg_count.value());
        } else {
            undefined_macros.try_emplace(id);
        }
    }

    std::string macro_processor::process_macro(const std::string_view &in) {
        if (!in.contains('`')) return std::string(in);
        std::string result;
        std::string working = std::string(in);
        int nesting_counter = 0;
        size_t previous_size = 0;
        int nonconverging_passes = 0;
        bool expansion_needed = true;
        while (expansion_needed && nesting_counter < 1000) {
            result.clear();
            // Work on a view into `working` (a separate buffer) so that
            // advancing `remaining` can never alias the `result` buffer that
            // is being built in this pass.
            std::string_view remaining = working;
            while (auto match = identifier_pattern(remaining)) {
                result.append(remaining.begin(), match.begin());
                if (match.view().back() == '(') {
                    auto id_view = match.view().substr(1, match.size()-2);
                    auto id = std::string(trim(id_view));
                    size_t start_pos = (match.data() + match.size()) - remaining.data();
                    auto args_text = remaining.substr(start_pos);
                    if (is_structural_directive(id)) {
                        result.append(match.view());
                        remaining = args_text;
                        continue;
                    }
                    auto [args, rest_of_line] = get_call_arguments(args_text);
                    if (!definitions.contains(id)) {
                        if (is_external_methodology_macro(id)) {
                            warn_stripped_once(id, path);
                            // Stripped UVM/OVM calls typically stand in for a
                            // statement (e.g. `else `uvm_error(...)). Expanding
                            // to empty leaves a dangling `else` and collapses
                            // the newlines inside the multi-line call, shifting
                            // all downstream diagnostics. Emit a null statement
                            // and preserve the consumed newlines instead.
                            size_t newlines = 0;
                            if (rest_of_line.data() != nullptr) {
                                for (auto it = match.begin(); it != rest_of_line.begin(); ++it) {
                                    if (*it == '\n') ++newlines;
                                }
                            }
                            result.append(";");
                            result.append(newlines, '\n');
                            remaining = rest_of_line;
                            continue;
                        }
                        // Record and strip: the file quarantines, but
                        // collecting every unknown (with its call-site arity)
                        // in one pass lets the fixpoint converge instead of
                        // learning one macro per round. Output is discarded
                        // with the file.
                        report_undefined(id, static_cast<int>(args.size()));
                        remaining = rest_of_line;
                        continue;
                    }
                    auto macro = definitions.at(id);
                    if (std::holds_alternative<std::string>(macro)) {
                        report_error(fmt::format("Attempted to pass arguments to a macro {} that does not need them in file {}", id, path));
                        return "";
                    }
                    auto macro_text = replace_function_macro(args,std::get<function_macro>(macro));
                    if (!macro_text.has_value()) {
                        report_error(fmt::format("Attempted to call a macro [{}] without enough parameters ", id));
                        return "";
                    }
                    result.append(macro_text.value());
                    remaining = rest_of_line;
                } else{
                    result.append(get_define_replacement(match));

                    remaining = std::string_view{match.end(), remaining.end()};
                }
            }

            result.append(remaining);
            if (result.size() > MAX_MACRO_EXPANSION_SIZE) {
                report_error(fmt::format("Macro expansion exceeded the maximum supported size of {} bytes in file {}", MAX_MACRO_EXPANSION_SIZE, path));
                return "";
            }
            expansion_needed = result.contains('`');
            // A self-multiplying recursive macro keeps growing every pass and
            // never converges; a legitimate expansion (e.g. UVM field utils)
            // converges within a couple of passes. Detect sustained growth early
            // so the tool errors instead of exhausting memory.
            if (expansion_needed && result.size() > previous_size) {
                nonconverging_passes++;
            } else {
                nonconverging_passes = 0;
            }
            if (nonconverging_passes >= MAX_NONCONVERGING_PASSES) {
                report_error(fmt::format("Macro expansion did not converge, possible recursive macro in file {}", path));
                return "";
            }
            previous_size = result.size();
            working = result;
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

            if (body[i] == '`' && i + 1 < body.size() && body[i + 1] == '`') {
                if (i > current_token_start) {
                    tokens.push_back(body.substr(current_token_start, i - current_token_start));
                }
                tokens.push_back(body.substr(i, 2));
                i++; // Advance loop pointer past the second backtick
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
            // Evaluates the isolated stringification token sequence back into standard quotes
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

                std::string token_to_append = expanded_tokens[i];

                size_t quote_count = std::count(result.begin(), result.end(), '"');
                bool currently_in_string = (quote_count % 2 != 0);

                if (currently_in_string) {
                    for (char &c : token_to_append) {
                        if (c == '\n' || c == '\r') {
                            c = ' '; // Flatten the newline into a harmless space inside the string literal
                        }
                    }
                }

                result.append(token_to_append);
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
            if (is_structural_directive(purged_identifier)) {
                return std::string(identifier);
            }
            auto id = std::string(purged_identifier);
            if (!definitions.contains(id)) {
                if (is_external_methodology_macro(id)) {
                    warn_stripped_once(id, path);
                    return "";
                }
                // Bare use (no parentheses): no arity information to record.
                report_undefined(id, std::nullopt);
                return std::string(identifier);
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
