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

std::vector<HDL_Resource> vhdl_analyzer::analyze() {

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

    return  vhdl_modules_explorer.get_entities();
}

void VhParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                                      size_t charPositionInLine, const std::string &msg, std::exception_ptr e) {
    std::cerr << "Error in file: "<< this->file_path << " At line: " << line << " Column: "<< charPositionInLine << std::endl;
    std::cerr << "\t" << msg<< std::endl;
}
