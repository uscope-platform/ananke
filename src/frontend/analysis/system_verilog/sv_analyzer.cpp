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



#include "frontend/analysis/system_verilog/sv_analyzer.hpp"
#include "data_model/HDL/statement/hdl_instance_statement.hpp"

#include "frontend/analysis/system_verilog/preprocessor/sv_preprocessor.hpp"


std::pair<std::string, std::vector<std::string>> sv_analyzer::preprocess(const std::string &path, const std::string_view &content) {

    preprocessor::sv_preprocessor preproc;
    preproc.set_path(path);
    preproc.set_include_directories(include_directories);
    auto processed_content = preproc.preprocess(content);
    auto documentation_comments = preproc.get_documentation_comments();
    return {processed_content, documentation_comments};
}


hdl_file sv_analyzer::analyze(const std::string &path, const std::string_view &file_content) {

    preprocessor::sv_preprocessor preproc;
    preproc.set_path(path);
    preproc.set_include_directories(include_directories);
    auto processed_content = preproc.preprocess(file_content);
    auto documentation_comments = preproc.get_documentation_comments();
    auto sources_map = preproc.get_source_map();

    auto result = process_hdl(path, processed_content);


    documentation_analyzer doc(documentation_comments);
    doc.set_source_path(path);

    Parameters_map parameters;
    doc.process_documentation(parameters);

    auto modules_doc = doc.get_modules_documentation();
    for(auto &e: result.get_content()){
        if (e->is<hdl_resource_statement>()) e->as<hdl_resource_statement>().set_documentation(modules_doc[e->as<hdl_resource_statement>().getName()]);
    }

    auto procs = doc.get_processors_documentation();
    for(auto &item: procs){
        for(auto &e:result.get_content()){
            if( e->is<hdl_resource_statement>() && e->as<hdl_resource_statement>().getName() == item.first){
                e->as<hdl_resource_statement>().add_processor_doc(item.second);
            }
        }
    }

    auto ch_groups =  doc.get_channel_groups();
    for(auto &item: ch_groups){
        std::string entity = item.first.substr(0, item.first.find("."));
        std::string scope_instance = item.first.substr(item.first.find(".")+1, item.first.size());
        for(auto &e:result.get_content()){
            if(e->is<hdl_resource_statement>() && e->as<hdl_resource_statement>().getName() == entity){
                for (auto &stmt : e->as<hdl_resource_statement>().get_statements()) {
                    auto inst = std::dynamic_pointer_cast<hdl_instance_statement>(stmt);
                    if (inst && inst->get_name() == scope_instance) {
                        inst->set_channel_groups(item.second);
                    }
                }
            }
        }
    }

    return result;
}


hdl_file sv_analyzer::process_hdl(const std::string &path, const std::string &preprocessed_content) {
    hdl_file result;
    std::istringstream istream(preprocessed_content);


    antlr4::ANTLRInputStream antlr_istream(istream);
    sv2017Lexer lexer(&antlr_istream);
    antlr4::CommonTokenStream tok_stream(&lexer);

    tok_stream.fill();

    SvParserErrorListener error_listener;
    error_listener.file_path = path;

    sv2017 parser(&tok_stream);

    parser.removeErrorListeners();
    parser.addErrorListener(&error_listener);

    //parser.getInterpreter<antlr4::atn::ParserATNSimulator>()->setPredictionMode(antlr4::atn::PredictionMode::SLL);
    antlr4::tree::ParseTree *Tree = parser.source_text();

    sv_visitor sv_modules_explorer;
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&sv_modules_explorer, Tree);
    result.set_content(sv_modules_explorer.get_entities());
    return result;
}


void SvParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                                        size_t charPositionInLine, const std::string &msg, std::exception_ptr e) {
    // 1. Standardized compiler-style error header (file:line:column: error: message)
    std::cerr << this->file_path << ":" << line << ":" << (charPositionInLine + 1) << ": error: " << msg << "\n";

    // 2. Fetch the underlying input stream to print the offending line
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
