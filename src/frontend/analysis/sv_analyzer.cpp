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



#include "frontend/analysis/sv_analyzer.hpp"

#include "frontend/analysis/preprocessor/sv_preprocessor.hpp"


std::pair<std::string, std::vector<std::string>> sv_analyzer::preprocess(const std::string &path, std::unique_ptr<std::istream> &input) {

    preprocessor::sv_preprocessor preproc;
    preproc.set_path(path);
    auto processed_content = preproc.preprocess(input);
    auto documentation_comments = preproc.get_documentation_comments();
    return {processed_content, documentation_comments};
}


std::vector<HDL_Resource> sv_analyzer::analyze(const std::string &path, std::unique_ptr<std::istream> &input) {
    auto [preprocessed_content, documentation_comments] = preprocess(path, input);
    auto entities = process_hdl(path, preprocessed_content);


    documentation_analyzer doc(documentation_comments);
    doc.set_source_path(path);

    Parameters_map parameters;
    doc.process_documentation(parameters);

    auto modules_doc = doc.get_modules_documentation();
    for(auto &e:entities){
        e.set_documentation(modules_doc[e.getName()]);
    }

    auto procs = doc.get_processors_documentation();
    for(auto &item: procs){
        for(auto &e:entities){
            if(e.getName() == item.first){
                e.add_processor_doc(item.second);
            }
        }
    }

    auto ch_groups =  doc.get_channel_groups();
    for(auto &item: ch_groups){
        std::string entity = item.first.substr(0, item.first.find("."));
        std::string scope_instance = item.first.substr(item.first.find(".")+1, item.first.size());
        for(auto &e:entities){
            if(e.getName() == entity){
                auto dependencies = e.get_dependencies();
                for(auto &dep:dependencies){
                    if(dep.get_name() == scope_instance){
                        dep.set_channel_groups(item.second);
                    }
                }
                e.set_dependencies(dependencies);
            }
        }
    }

    return  entities;
}


std::vector<HDL_Resource> sv_analyzer::process_hdl(const std::string &path, const std::string &preprocessed_content) {
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

    sv_visitor sv_modules_explorer(path);
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&sv_modules_explorer, Tree);
    return sv_modules_explorer.get_entities();
}


void SvParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                                        size_t charPositionInLine, const std::string &msg, std::exception_ptr e) {
    std::cerr << "Error in file: "<< this->file_path<< std::endl;
    std::cerr << "\t" << msg<< std::endl;
}
