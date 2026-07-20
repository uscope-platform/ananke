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

#ifndef ANANKE_VHDL_VISITOR_HPP
#define ANANKE_VHDL_VISITOR_HPP

#include <string>
#include <utility>
#include <vector>
#include <stack>
#include "data_model/HDL/HDL_Resource.hpp"
#include "data_model/HDL/statement/hdl_instance_statement.hpp"
#include "data_model/HDL/factories/HDL_modules_factory.hpp"

#include "mgp_vh/vhdlParserBaseListener.h"
#include "mgp_vh/vhdlParser.h"


class vhdl_visitor : public mgp_vh::vhdlParserBaseListener {

public:
    explicit vhdl_visitor(std::string p);
    void enterEntity_declaration(mgp_vh::vhdlParser::Entity_declarationContext *ctx) override;
    void exitEntity_declaration(mgp_vh::vhdlParser::Entity_declarationContext *ctx) override;
    void enterArchitecture_body(mgp_vh::vhdlParser::Architecture_bodyContext *ctx) override;
    void exitArchitecture_body(mgp_vh::vhdlParser::Architecture_bodyContext *ctx) override;
    void exitConcurrent_statement(mgp_vh::vhdlParser::Concurrent_statementContext *ctx) override;
    std::vector<HDL_Resource> get_entities();
private:
    std::string path;
    std::vector<HDL_Resource>  entities;
    std::unordered_map<std::string, std::vector<std::shared_ptr<hdl_statement_base>>> statement_map;
    std::string current_architecture;

    HDL_modules_factory modules_factory;
};


#endif //ANANKE_VHDL_VISITOR_HPP
