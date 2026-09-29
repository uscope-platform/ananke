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

#ifndef ANANKE_SV_ANALYZER_HPP
#define ANANKE_SV_ANALYZER_HPP

#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <set>
#include <nlohmann/json.hpp>

#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/include_dependency.hpp"

#include "frontend/analysis/system_verilog/preprocessor/sv_preprocessor.hpp"
#include "frontend/repository_index.hpp"
#include "documentation_analyzer.hpp"

#include "mgp_sv/sv2017Lexer.h"
#include "mgp_sv/sv2017.h"
#include "sv_visitor.hpp"
#include "antlr4-runtime.h"
#include "data_model/hdl_file.hpp"

class sv_analyzer {
public:
    std::pair<std::string, std::vector<std::string>> preprocess(const std::string &path, const std::string_view &file_content);
    std::optional<hdl_file> analyze(const std::string &path, const std::string_view &file_content);
    void set_include_directories(const std::set<std::string> &i_d){include_directories = i_d;}
    void set_repository_index(const std::shared_ptr<repository_index> &idx){repo_idx = idx;}
    void set_defines(const std::set<std::string> &d){defines = d;}
    std::set<include_dependency> get_includes() {return includes;}
    [[nodiscard]] bool has_error() const {return last_error.has_value();}
    [[nodiscard]] const std::string& get_error() const {return last_error.value();}
private:

    hdl_file process_hdl(const std::string &path, const std::string &preprocessed_content);
    std::set<std::string> include_directories;
    std::set<std::string> defines;
    std::shared_ptr<repository_index> repo_idx;
    std::set<include_dependency> includes;
    std::optional<std::string> last_error;
};


class SvParserErrorListener : public antlr4::BaseErrorListener {
public:
    std::string file_path;
    void syntaxError(antlr4::Recognizer *recognizer, antlr4::Token * offendingSymbol, size_t line, size_t charPositionInLine,
                     const std::string &msg, std::exception_ptr e) override;
};
#endif //ANANKE_SV_ANALYZER_HPP
