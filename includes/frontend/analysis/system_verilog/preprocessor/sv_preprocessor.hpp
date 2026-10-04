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

#ifndef ANANKE_SV_PREPROCESSOR_HPP
#define ANANKE_SV_PREPROCESSOR_HPP

#include <ctre.hpp>
#include <filesystem>
#include <vector>
#include <fstream>
#include <set>
#include <string>
#include <unordered_map>
#include <variant>
#include <iterator>
#include <fmt/format.h>
#include <spdlog/spdlog.h>
#include "frontend/analysis/system_verilog/preprocessor/conditional_solver.hpp"
#include "frontend/analysis/system_verilog/preprocessor/source_mapper.hpp"
#include "frontend/analysis/system_verilog/preprocessor/macro_processor.hpp"
#include "frontend/repository_index.hpp"
#include "data_model/include_dependency.hpp"
#include "data_model/mm_file.hpp"
#include "data_model/parse_options.hpp"

namespace preprocessor {
    class sv_preprocessor {
    public:
        sv_preprocessor() = default;
        std::string preprocess(const std::string_view &file_content) {return  preprocess(file_content, 1);}
        std::string preprocess(const std::string_view &file_content, unsigned int initial_output_line);
        std::string flatten_source(const std::string_view &file_content);
        void set_options(const parse_options &o) {opts_ = o;}
        void set_repository_index(const std::shared_ptr<repository_index> &idx){repo_idx = idx;}
        void set_path(const std::string &s){path = s;}
        // Base definitions learned from the rest of the repository
        // (compilation-order macros). Installed underneath global defines
        // and file-local `defines: file-local > global > base.
        void set_base_definitions(const macro_definitions_map &b) {base_definitions = b;}
        std::vector<std::string> get_documentation_comments() {return documentation_comments;}
        std::set<include_dependency> get_includes() {return includes;}
        source_map_t get_source_map() const {return  source_map.get_map();}
        [[nodiscard]] bool has_error() const {return error.has_value() || !undefined_macros.empty();}
        // Reconstructed on read from recorded ids when no stored message
        // exists: nothing is formatted or kept during the pass itself.
        [[nodiscard]] const std::string& get_error() const;
        // Order-dependent unknowns: referenced while undefined, so the file
        // may still parse once repository macros are learned (quarantine).
        // has_fatal_error() distinguishes terminal failures (syntax-adjacent
        // preprocessor errors) from deferrable unknown-macro failures.
        [[nodiscard]] bool has_fatal_error() const {return fatal_error;}
        [[nodiscard]] const undefined_uses_map& get_undefined_macros() const {return undefined_macros;}
        [[nodiscard]] const std::set<std::string>& get_unknown_conditionals() const {return unknown_conditionals;}
        // Definitions this file (transitively through its includes) explicitly
        // `define'd. Seeded base/global entries are excluded unless locally
        // redefined: otherwise every consumer would re-attribute repository
        // macros to itself, defeating self-exclusion and fabricating
        // definers. Partial if preprocessing aborted early.
        [[nodiscard]] macro_definitions_map get_harvested_definitions() const;
    private:
        // Evaluates line-leading directives (`define/`undef/`include) left
        // in expanded macro text (e.g. `include via an `include_file body, or
        // a `define nested in another macro body). Directives wrapped in
        // embedded conditionals were already consumed by the ifdef pass
        // above; only naked ones reach here. Mirrors line-level semantics.
        std::string evaluate_embedded_directives(const std::string &text);
        void report_error(const std::string &msg);
        // Records a conditional query on a currently-undefined macro.
        // The taken branch may be wrong once repository macros are known,
        // so the file becomes a quarantine candidate.
        void record_unknown_conditional(const std::string_view &name);
        std::string preprocess_body(const std::string_view &file_content, unsigned int initial_output_line);
        std::string post_process_macro_expansion(const std::string &text);
        std::string gather_multi_line_macro(const std::string &first_line, std::istringstream &iss);
        using definitions_map = macro_definitions_map;
        std::optional<include_dependency> parse_include_path(const std::string_view &v);
        std::optional<include_dependency> resolve_include(const std::string &name, bool quoted);
        std::string get_define_replacement(const std::string_view &v);
        void parse_definition(const std::string_view &sv, int prefix_length);
        void install_global_defines();
        static std::string_view parse_one_arg_directive(const std::string_view &sv, int prefix_length);
        uint64_t line_number = 1;
        definitions_map definitions;
        definitions_map base_definitions;
        std::set<std::string> locally_defined;
        undefined_uses_map undefined_macros;
        std::set<std::string> unknown_conditionals;
        bool fatal_error = false;
        parse_options opts_;
        std::vector<std::string> documentation_comments;
        std::string path;
        std::optional<std::string> error;
        // Lazily reconstructed diagnostic for recorded unknowns (see
        // get_error): the pass itself keeps ids only, never message text.
        mutable std::string deferred_error;
        std::set<std::string> active_includes;
        conditional_solver c_solver;
        std::set<std::string> include_directories;
        std::shared_ptr<repository_index> repo_idx;
        std::set<include_dependency> includes;
        source_mapper source_map;
        unsigned int output_line_n = 0;
        bool disable_preprocessor = false;
        static constexpr auto identifier_pattern = ctre::search<R"(`([a-zA-Z_][a-zA-Z0-9_]*)(\s*\()*)">;
    };
}



#endif //ANANKE_SV_PREPROCESSOR_HPP
