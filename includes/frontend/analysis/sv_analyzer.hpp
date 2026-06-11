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

#ifndef ANANKE_SV_ANALYZER_HPP
#define ANANKE_SV_ANALYZER_HPP

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

#include "data_model/HDL/HDL_Resource.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"

#include "frontend/analysis/preprocessor/sv_preprocessor.hpp"
#include "frontend/analysis/documentation_analyzer.hpp"

#include "mgp_sv/sv2017Lexer.h"
#include "mgp_sv/sv2017.h"
#include "frontend/analysis/sv_visitor.hpp"
#include "antlr4-runtime.h"

class sv_analyzer {
public:
    std::pair<std::string, std::vector<std::string>> preprocess(const std::string &path, const std::string_view &file_content);
    std::vector<HDL_Resource> analyze(const std::string &path, const std::string_view &file_content);
    void set_include_directories(const std::set<std::string> &i_d){include_directories = i_d;}
private:

    std::vector<HDL_Resource> process_hdl(const std::string &path, const std::string &preprocessed_content);
    std::set<std::string> include_directories;
};


class SvParserErrorListener : public antlr4::BaseErrorListener {
public:
    std::string file_path;
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token * offendingSymbol, size_t line, size_t charPositionInLine,
                     const std::string &msg, std::exception_ptr e) override;
};
#endif //ANANKE_SV_ANALYZER_HPP
