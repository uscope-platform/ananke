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

#include "frontend/analysis/system_verilog/sv_parsing_helpers.hpp"

namespace sv_parsing_helpers {

    qualified_identifier parse_qualified_identifier(mgp_sv::sv2017::Package_or_class_scoped_pathContext *ctx) {
        auto items = ctx->package_or_class_scoped_path_item();
        auto d_colon = ctx->DOUBLE_COLON();

        if (d_colon.empty()) {
            return qualified_identifier(ctx->getText());
        }

        std::vector<std::string> prefix;
        std::string name;

        unsigned int first_item = 0;

        if (ctx->KW_DOLAR_UNIT()) {
            prefix.push_back("$unit");
            first_item = 0;
        } else if (ctx->KW_DOLAR_ROOT()) {
            prefix.push_back("$root");
            first_item = 0;
        } else if (ctx->implicit_class_handle()) {
            prefix.push_back(ctx->implicit_class_handle()->getText());
            first_item = 0;
        }

        unsigned int n_segments = items.size();
        for (unsigned int i = first_item; i < n_segments - 1; ++i) {
            prefix.push_back(items[i]->identifier()->getText());
        }
        name = items[n_segments - 1]->identifier()->getText();

        qualified_identifier qi(name);
        if (!prefix.empty()) {
            qi.set_package_prefix(prefix);
        }
        return qi;
    }

}
