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

#include "frontend/cmake_target_scanner.hpp"

#include <algorithm>
#include <cctype>
#include <set>
#include <sstream>

namespace {

// Replaces every comment (`# ...` and `#[[ ... ]]`) with whitespace; quoted arguments are left alone, so a `#`
// inside a string does not start a comment. Newlines are preserved.
// `quoted` is set for every character inside a quoted argument (the quotes included), so a command-looking text
// inside a string is not mistaken for a call.
std::string strip_comments(const std::string &in, std::vector<bool> &quoted) {
    std::string out;
    out.reserve(in.size());
    auto emit = [&](char ch, bool is_quoted) { out += ch; quoted.push_back(is_quoted); };
    size_t i = 0;
    while (i < in.size()) {
        const char c = in[i];
        if (c == '"') {
            emit(c, true);
            ++i;
            while (i < in.size() && in[i] != '"') {
                if (in[i] == '\\' && i + 1 < in.size()) emit(in[i++], true);
                emit(in[i++], true);
            }
            if (i < in.size()) emit(in[i++], true);
        } else if (c == '#') {
            // bracket comment: #[==[ ... ]==]
            size_t j = i + 1;
            if (j < in.size() && in[j] == '[') {
                size_t eq = 0;
                ++j;
                while (j < in.size() && in[j] == '=') { ++eq; ++j; }
                if (j < in.size() && in[j] == '[') {
                    const std::string close = "]" + std::string(eq, '=') + "]";
                    const size_t end = in.find(close, j + 1);
                    const size_t stop = end == std::string::npos ? in.size() : end + close.size();
                    for (size_t k = i; k < stop; ++k) emit(in[k] == '\n' ? '\n' : ' ', false);
                    i = stop;
                    continue;
                }
            }
            while (i < in.size() && in[i] != '\n') { emit(' ', false); ++i; }
        } else {
            emit(c, false);
            ++i;
        }
    }
    return out;
}

bool is_ident_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_';
}

// Splits the text between the parentheses of a command into arguments (quotes removed).
std::vector<std::string> split_arguments(const std::string &text) {
    std::vector<std::string> args;
    size_t i = 0;
    while (i < text.size()) {
        while (i < text.size() && std::isspace(static_cast<unsigned char>(text[i]))) ++i;
        if (i >= text.size()) break;
        std::string arg;
        if (text[i] == '"') {
            ++i;
            while (i < text.size() && text[i] != '"') {
                if (text[i] == '\\' && i + 1 < text.size()) ++i;
                arg += text[i++];
            }
            if (i < text.size()) ++i;
        } else {
            while (i < text.size() && !std::isspace(static_cast<unsigned char>(text[i]))) arg += text[i++];
        }
        args.push_back(std::move(arg));
    }
    return args;
}

} // namespace

std::vector<cmake_target> cmake_target_scanner::parse(const std::string &content, const std::filesystem::path &dir) {
    std::vector<bool> quoted;
    const std::string text = strip_comments(content, quoted);
    const std::string command = "add_library";
    std::vector<cmake_target> targets;
    std::set<std::string> seen;

    size_t pos = 0;
    while (pos < text.size()) {
        // command names are case insensitive
        size_t found = std::string::npos;
        for (size_t i = pos; i + command.size() <= text.size(); ++i) {
            if ((i > 0 && is_ident_char(text[i - 1])) || quoted[i]) continue;
            bool match = true;
            for (size_t k = 0; k < command.size(); ++k) {
                if (std::tolower(static_cast<unsigned char>(text[i + k])) != command[k]) { match = false; break; }
            }
            if (match) { found = i; break; }
        }
        if (found == std::string::npos) break;

        size_t i = found + command.size();
        pos = i;
        while (i < text.size() && std::isspace(static_cast<unsigned char>(text[i]))) ++i;
        if (i >= text.size() || text[i] != '(') continue; // `add_library_foo`, or a mention without a call
        ++i;

        // up to the matching parenthesis, quotes respected
        const size_t start = i;
        int depth = 1;
        bool quoted = false;
        for (; i < text.size() && depth > 0; ++i) {
            const char c = text[i];
            if (quoted) {
                if (c == '\\') ++i;
                else if (c == '"') quoted = false;
            } else if (c == '"') quoted = true;
            else if (c == '(') ++depth;
            else if (c == ')') --depth;
        }
        if (depth != 0) break; // unterminated call: nothing more can be trusted
        pos = i;

        const auto args = split_arguments(text.substr(start, i - 1 - start));
        if (args.empty()) continue;
        const auto &name = args[0];
        if (name.empty() || name.find('$') != std::string::npos) continue; // not known without evaluating CMake
        // `<name> ALIAS <target>` and `<name> [<type>] IMPORTED`
        bool skip = false;
        for (size_t k = 1; k < args.size() && k <= 2; ++k) {
            if (args[k] == "ALIAS" || args[k] == "IMPORTED") skip = true;
        }
        if (skip) continue;
        if (seen.insert(name).second) targets.push_back({name, dir});
    }
    return targets;
}

std::expected<cmake_target, std::string> cmake_target_scanner::resolve(const std::vector<cmake_target> &targets,
                                                                       const std::string &name) {
    std::vector<cmake_target> matches;
    for (const auto &t : targets) {
        if (t.name == name) matches.push_back(t);
    }
    if (matches.size() == 1) return matches.front();

    std::ostringstream msg;
    if (matches.empty()) {
        msg << "CMake library target '" << name << "' not found in the repository";
        if (targets.empty()) {
            msg << " (no add_library() declarations were found)";
        } else {
            msg << "; known targets:";
            for (const auto &t : targets) msg << " " << t.name;
        }
    } else {
        msg << "CMake library target '" << name << "' is declared in more than one place:";
        for (const auto &t : matches) msg << " " << t.dir.string();
    }
    return std::unexpected(msg.str());
}
