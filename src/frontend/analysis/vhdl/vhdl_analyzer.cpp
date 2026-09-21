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

#include "frontend/analysis/vhdl/vhdl_analyzer.hpp"

#include "frontend/analysis/system_verilog/documentation_analyzer.hpp"
#include "data_model/HDL/statement/hdl_instance_statement.hpp"

#include <sstream>

namespace {

// Collect consecutive `--` comment lines, stripped of the `--` marker, so a
// multi-line VHDL documentation comment joins into a single string.
std::vector<std::string> extract_vhdl_doc_comments(const std::string &content) {
    std::vector<std::string> result;
    std::istringstream ss(content);
    std::string line;
    std::string block;
    auto flush = [&]() {
        if (!block.empty()) result.push_back(block);
        block.clear();
    };
    while (std::getline(ss, line)) {
        auto pos = line.find_first_not_of(" \t");
        if (pos != std::string::npos && line.compare(pos, 2, "--") == 0) {
            std::string comment = line.substr(pos + 2);
            auto cpos = comment.find_first_not_of(" \t");
            if (cpos != std::string::npos) comment = comment.substr(cpos);
            if (!block.empty()) block += "\n";
            block += comment;
        } else {
            flush();
        }
    }
    flush();
    return result;
}

} // namespace

vhdl_analyzer::vhdl_analyzer(const std::string &file_path): vhdl_modules_explorer(file_path) {
    path = file_path;

}

void vhdl_analyzer::cleanup_content(const std::string &regex) {
    std::ifstream t(path);


    t.seekg(0, std::ios::end);
    processed_content.reserve(t.tellg());
    t.seekg(0, std::ios::beg);

    processed_content.assign((std::istreambuf_iterator<char>(t)),
                       std::istreambuf_iterator<char>());


}

hdl_file vhdl_analyzer::analyze() {

    std::istringstream istream(processed_content);


    antlr4::ANTLRInputStream antlr_istream(istream);
    mgp_vh::vhdlLexer lexer(&antlr_istream);
    antlr4::CommonTokenStream tok_stream(&lexer);

    tok_stream.fill();

    VhParserErrorListener error_listener;
    error_listener.file_path = path;
    mgp_vh::vhdlParser parser(&tok_stream);
    parser.removeErrorListeners();
    parser.addErrorListener(&error_listener);

    antlr4::tree::ParseTree *Tree = parser.design_file();
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&vhdl_modules_explorer, Tree);
    hdl_file result;
    auto content = vhdl_modules_explorer.get_entities();
    auto imports = vhdl_modules_explorer.get_imports();
    content.insert(content.end(), imports.begin(), imports.end());
    result.set_content(content);

    attach_documentation(result);

    return  result;
}

hdl_file vhdl_analyzer::analyze_content(const std::string &content, const std::string &src_path) {
    path = src_path;
    processed_content = content;
    return analyze();
}

void vhdl_analyzer::attach_documentation(hdl_file &result) {
    auto comments = extract_vhdl_doc_comments(processed_content);
    documentation_analyzer doc(comments);
    doc.set_source_path(path);
    doc.process_documentation();

    // VHDL identifiers are case-insensitive: the resource name is canonicalized
    // lowercase, so match the documentation name case-insensitively.
    auto lower = [](std::string s) {
        std::transform(s.begin(), s.end(), s.begin(), ::tolower);
        return s;
    };

    std::unordered_map<std::string, module_documentation> modules_doc;
    for (auto &[n, md] : doc.get_modules_documentation())
        modules_doc[lower(n)] = md;
    for (auto &e : result.get_content()) {
        if (!e->is<hdl_resource_statement>()) continue;
        auto &res = e->as<hdl_resource_statement>();
        auto it = modules_doc.find(lower(res.getName()));
        if (it != modules_doc.end())
            res.set_documentation(it->second);
    }

    auto procs = doc.get_processors_documentation();
    for (auto &item : procs) {
        for (auto &e : result.get_content()) {
            if (e->is<hdl_resource_statement>() && lower(e->as<hdl_resource_statement>().getName()) == lower(item.first))
                e->as<hdl_resource_statement>().add_processor_doc(item.second);
        }
    }

    auto ch_groups = doc.get_channel_groups();
    for (auto &item : ch_groups) {
        std::string entity = item.first.substr(0, item.first.find("."));
        std::string scope_instance = item.first.substr(item.first.find(".") + 1, item.first.size());
        for (auto &e : result.get_content()) {
            if (e->is<hdl_resource_statement>() && lower(e->as<hdl_resource_statement>().getName()) == lower(entity)) {
                for (auto &stmt : e->as<hdl_resource_statement>().get_statements()) {
                    auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(stmt);
                    if (inst && inst->get_name() == lower(scope_instance))
                        inst->set_channel_groups(item.second);
                }
            }
        }
    }
}

void VhParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                                      size_t charPositionInLine, const std::string &msg, std::exception_ptr e) {
    // Standardized compiler-style error header (file:line:column: error: message)
    std::cerr << this->file_path << ":" << line << ":" << (charPositionInLine + 1) << ": error: " << msg << "\n";

    // Fetch the underlying input stream to print the offending line
    if (recognizer != nullptr) {
        auto tokens = dynamic_cast<antlr4::TokenStream*>(recognizer->getInputStream());
        if (tokens != nullptr) {
            // Get the text of the entire source line where the error occurred
            std::string input = tokens->getTokenSource()->getInputStream()->toString();
            std::stringstream ss(input);
            std::string lineText;
            // Move to the correct line in the source string
            for (size_t i = 0; i < line; ++i) {
                std::getline(ss, lineText);
            }
            // Print the offending line of code
            std::cerr << " " << line << " | " << lineText << "\n";

            // Print the visual caret (^) pointer underneath the offending character
            std::cerr << std::string(std::to_string(line).length() + 1, ' ') << " | "; // Aligns with the '|'
            std::cerr << std::string(charPositionInLine, ' ') << "^\n";
        }
    }
    std::cerr << std::endl;
}
