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

#include <BailErrorStrategy.h>
#include <spdlog/spdlog.h>

#include <filesystem>

namespace {
// A `.svh`/`.vh` header that cannot parse as a standalone compilation unit
// is assumed to be a module-body include fragment (bare `logic` decls and
// `assign` statements meant to be `` `include ``d inside a module, e.g. the
// VeeR `el2_dec_csr_equ_m.svh` shape). Only `.svh`/`.vh` qualify: `.sv`/`.v`
// must stay compilation units so genuine errors there keep failing.
bool is_fragment_header(const std::string &path) {
    if (path.empty()) return false;
    const std::string ext = std::filesystem::path(path).extension().string();
    return ext == ".svh" || ext == ".vh";
}
} // namespace


void sv_analyzer::capture_preprocessor_state(preprocessor::sv_preprocessor &preproc) {
    includes = preproc.get_includes();
    fatal_error = preproc.has_fatal_error();
    undefined_macros = preproc.get_undefined_macros();
    unknown_conditionals = preproc.get_unknown_conditionals();
    harvested = preproc.get_harvested_definitions();
}

std::pair<std::string, std::vector<std::string>> sv_analyzer::preprocess(const std::string &path, const std::string_view &content) {
    last_error.reset();
    includes.clear();
    fatal_error = false;
    syntax_errors = false;
    is_include_fragment_ = false;
    undefined_macros.clear();
    unknown_conditionals.clear();
    harvested.clear();
    preprocessor::sv_preprocessor preproc;
    preproc.set_path(path);
    preproc.set_options(opts_);
    preproc.set_base_definitions(injected);
    if (repo_idx) preproc.set_repository_index(repo_idx);
    auto processed_content = preproc.preprocess(content);
    if (preproc.has_error()) last_error = preproc.get_error();
    capture_preprocessor_state(preproc);
    auto documentation_comments = preproc.get_documentation_comments();
    return {processed_content, documentation_comments};
}

std::optional<hdl_file> sv_analyzer::analyze(const std::string &path, const std::string_view &file_content) {
    last_error.reset();
    syntax_errors = false;
    is_include_fragment_ = false;
    semantic_violations.clear();
    includes.clear();
    fatal_error = false;
    undefined_macros.clear();
    unknown_conditionals.clear();
    harvested.clear();
    preprocessor::sv_preprocessor preproc;
    preproc.set_path(path);
    preproc.set_options(opts_);
    preproc.set_base_definitions(injected);
    if (repo_idx) preproc.set_repository_index(repo_idx);
    auto processed_content = preproc.preprocess(file_content);
    if (preproc.has_error()) {
        last_error = preproc.get_error();
        semantic_violations = preproc.get_semantic_violations();
        capture_preprocessor_state(preproc);
        return std::nullopt;
    }
    capture_preprocessor_state(preproc);
    auto documentation_comments = preproc.get_documentation_comments();
    auto sources_map = preproc.get_source_map();

    auto result = process_hdl(path, processed_content);
    if (last_error.has_value()) return std::nullopt;


    documentation_analyzer doc(documentation_comments);
    doc.set_source_path(path);

    doc.process_documentation();

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


namespace {

// Words that 1364-2005 reserves; older editions drop some of them.
const std::set<std::string> &verilog_2005_keywords() {
    static const std::set<std::string> kw = {
        "always", "and", "assign", "automatic", "begin", "buf", "bufif0", "bufif1", "case", "casex", "casez", "cell",
        "cmos", "config", "deassign", "default", "defparam", "design", "disable", "edge", "else", "end", "endcase",
        "endconfig", "endfunction", "endgenerate", "endmodule", "endprimitive", "endspecify", "endtable", "endtask",
        "event", "for", "force", "forever", "fork", "function", "generate", "genvar", "highz0", "highz1", "if",
        "ifnone", "incdir", "include", "initial", "inout", "input", "instance", "integer", "join", "large", "liblist",
        "library", "localparam", "macromodule", "medium", "module", "nand", "negedge", "nmos", "nor",
        "noshowcancelled", "not", "notif0", "notif1", "or", "output", "parameter", "pmos", "posedge", "primitive",
        "pull0", "pull1", "pulldown", "pullup", "pulsestyle_onevent", "pulsestyle_ondetect", "rcmos", "real",
        "realtime", "reg", "release", "repeat", "rnmos", "rpmos", "rtran", "rtranif0", "rtranif1", "scalared",
        "showcancelled", "signed", "small", "specify", "specparam", "strong0", "strong1", "supply0", "supply1",
        "table", "task", "time", "tran", "tranif0", "tranif1", "tri", "tri0", "tri1", "triand", "trior", "trireg",
        "unsigned", "use", "uwire", "vectored", "wait", "wand", "weak0", "weak1", "while", "wire", "wor", "xnor",
        "xor"};
    return kw;
}

// Returns nullptr when the version leaves every SystemVerilog keyword active.
std::shared_ptr<const std::set<std::string>> keyword_set_for(const std::string &version) {
    static const std::set<std::string> config_words = {"cell", "config", "design", "endconfig", "incdir", "include",
                                                       "instance", "liblist", "library", "use"};
    static const std::set<std::string> v2001_words = {"automatic", "endgenerate", "generate", "genvar", "localparam",
                                                      "noshowcancelled", "pulsestyle_onevent", "pulsestyle_ondetect",
                                                      "showcancelled", "signed", "unsigned"};
    if (version == "1364-2005") return std::make_shared<std::set<std::string>>(verilog_2005_keywords());
    if (version != "1364-1995" && version != "1364-2001" && version != "1364-2001-noconfig") return nullptr;

    auto kw = verilog_2005_keywords();
    kw.erase("uwire");
    if (version == "1364-1995") {
        for (const auto &w : v2001_words) kw.erase(w);
    }
    if (version == "1364-1995" || version == "1364-2001-noconfig") {
        for (const auto &w : config_words) kw.erase(w);
    }
    return std::make_shared<std::set<std::string>>(std::move(kw));
}

// Honours `begin_keywords / `end_keywords: inside a Verilog keyword region, SV-only
// keywords are plain identifiers, so their tokens are retyped before parsing.
// The preprocessor leaves a KEYWORD_MARKER line for each directive.
void apply_keyword_regions(const std::string &text, antlr4::CommonTokenStream &tok_stream, sv2017Lexer &lexer) {
    using region_set = std::shared_ptr<const std::set<std::string>>;
    if (!text.contains(preprocessor::KEYWORD_MARKER)) return;

    struct event { size_t line; bool begin; region_set kw; };
    std::vector<event> events;
    {
        std::istringstream in(text);
        std::string line;
        size_t n = 1;
        while (std::getline(in, line)) {
            if (line.starts_with(preprocessor::KEYWORD_MARKER)) {
                std::string_view rest = std::string_view(line).substr(preprocessor::KEYWORD_MARKER.size());
                rest.remove_prefix(std::min(rest.find_first_not_of(' '), rest.size()));
                if (rest.starts_with("begin")) {
                    rest.remove_prefix(5);
                    rest.remove_prefix(std::min(rest.find_first_not_of(' '), rest.size()));
                    events.push_back({n, true, keyword_set_for(std::string(rest))});
                } else {
                    events.push_back({n, false, nullptr});
                }
            }
            n++;
        }
    }

    std::vector<bool> is_keyword_type(lexer.getVocabulary().getMaxTokenType() + 1, false);
    for (size_t i = 0; i < is_keyword_type.size(); i++) {
        is_keyword_type[i] = lexer.getVocabulary().getSymbolicName(i).starts_with("KW_");
    }

    std::vector<region_set> stack;
    region_set active;
    size_t next_event = 0;
    for (auto *tok : tok_stream.getTokens()) {
        while (next_event < events.size() && events[next_event].line <= tok->getLine()) {
            if (events[next_event].begin) stack.push_back(events[next_event].kw);
            else if (!stack.empty()) stack.pop_back();
            active = stack.empty() ? nullptr : stack.back();
            next_event++;
        }
        if (active && tok->getType() < is_keyword_type.size() && is_keyword_type[tok->getType()]
            && !active->contains(tok->getText())) {
            static_cast<antlr4::CommonToken *>(tok)->setType(sv2017Lexer::SIMPLE_IDENTIFIER);
        }
    }
}

}  // namespace

hdl_file sv_analyzer::process_hdl(const std::string &path, const std::string &preprocessed_content) {
    hdl_file result;
    std::istringstream istream(preprocessed_content);


    antlr4::ANTLRInputStream antlr_istream(istream);
    sv2017Lexer lexer(&antlr_istream);
    antlr4::CommonTokenStream tok_stream(&lexer);

    SvParserErrorListener error_listener;
    error_listener.file_path = path;
    lexer.addErrorListener(&error_listener);

    tok_stream.fill();
    apply_keyword_regions(preprocessed_content, tok_stream, lexer);

    // A header that declares a design unit is a real compilation unit, even
    // when broken: never mistake it for a module-body include fragment.
    // Token types (not text) are scanned, so a comment or string mentioning
    // e.g. "module" cannot false-positive. Covers every `description`
    // alternative that declares a unit: module/macromodule, interface,
    // package, program, primitive (udp) and config.
    bool declares_unit = false;
    for (auto *tok : tok_stream.getTokens()) {
        const auto type = tok->getType();
        if (type == sv2017Lexer::KW_MODULE || type == sv2017Lexer::KW_MACROMODULE ||
            type == sv2017Lexer::KW_INTERFACE || type == sv2017Lexer::KW_PACKAGE ||
            type == sv2017Lexer::KW_PROGRAM || type == sv2017Lexer::KW_PRIMITIVE ||
            type == sv2017Lexer::KW_CONFIG) {
            declares_unit = true;
            break;
        }
    }

    sv2017 parser(&tok_stream);

    // Two-stage parsing: fast SLL trial with Bail (fail-fast, never silently
    // wrong), fallback to full LL on ParseCancellationException. Final tree
    // is identical to pure LL.
    parser.removeErrorListeners();
    parser.getInterpreter<antlr4::atn::ParserATNSimulator>()->setPredictionMode(
        antlr4::atn::PredictionMode::SLL);
    parser.setErrorHandler(std::make_shared<antlr4::BailErrorStrategy>());

    antlr4::tree::ParseTree *Tree = nullptr;
    bool sll_failed = false;
    try {
        Tree = parser.source_text();
    } catch (antlr4::ParseCancellationException &) {
        sll_failed = true;
    }

    if (sll_failed || Tree == nullptr) {
        parser.reset();
        parser.removeErrorListeners();
        parser.addErrorListener(&error_listener);
        parser.setErrorHandler(std::make_shared<antlr4::DefaultErrorStrategy>());
        parser.getInterpreter<antlr4::atn::ParserATNSimulator>()->setPredictionMode(
            antlr4::atn::PredictionMode::LL);
        Tree = parser.source_text();
    }
    syntax_errors = error_listener.has_errors;
    // Lenient-only workaround (strict rejects): a `.svh`/`.vh` header that
    // declares no design unit and fails as a standalone compilation unit is
    // a module-body include fragment, not a broken file. Its content still
    // reaches the build via `include inlining at each includer, so store an
    // empty resource here instead of failing. A header that does declare a
    // unit (module/interface/package/...) but fails to parse is genuinely
    // broken and keeps failing, even in lenient mode.
    const bool fragment =
        syntax_errors && !opts_.strict && !declares_unit && is_fragment_header(path);
    if (fragment) {
        is_include_fragment_ = true;
        spdlog::info("{} is a module-body include fragment, skipping standalone parse "
                      "(lenient mode; --strict rejects it)", path);
        for (const auto &message : error_listener.messages) spdlog::debug("{}", message);
        return result;
    }
    for (const auto &message : error_listener.messages) spdlog::error("{}", message);
    if (syntax_errors) {
        // The error-recovered tree is not a faithful model of the file:
        // fail loudly instead of storing partial entities silently. The
        // walker quarantines the file (retry across fixpoint passes when new
        // macros arrive) and reports it; single-shot flows surface
        // last_error directly. This also skips the visitor walk, whose
        // diagnostics on a broken tree would only be noise.
        // Message stays file-independent: every surfacing path prefixes the
        // path already, and identical messages group in the quarantine report.
        last_error = "Syntax error(s) during parsing (see log for details)";
        return result;
    }

    sv_visitor sv_modules_explorer;
    sv_modules_explorer.set_file(path);
    sv_modules_explorer.set_options(opts_);
    antlr4::tree::ParseTreeWalker::DEFAULT.walk(&sv_modules_explorer, Tree);
    semantic_violations = sv_modules_explorer.get_semantic_violations();
    if (sv_modules_explorer.is_error()) {
        // Prefer the checker's message when a rule fired; the generic text
        // stays for failures no rule describes.
        last_error = semantic_violations.empty()
            ? "Unsupported construct while parsing " + path
            : semantic_violations.front().message;
        return result;
    }
    auto content = sv_modules_explorer.get_entities();
    auto imports = sv_modules_explorer.get_imports();
    content.insert(content.end(), imports.begin(), imports.end());
    result.set_content(content);
    result.set_unit_names(
        sv_modules_explorer.get_declared_names(), sv_modules_explorer.get_pending_unit_names());
    return result;
}


void SvParserErrorListener::syntaxError(antlr4::Recognizer *recognizer, antlr4::Token *offendingSymbol, size_t line,
                                        size_t charPositionInLine, const std::string &msg, std::exception_ptr e) {
    (void)offendingSymbol;
    (void)e;
    has_errors = true;
    // Standardized compiler-style error header (file:line:column: error:
    // message), offending source line and caret. Assembled here and emitted
    // by the caller in one logger call so concurrent parses cannot
    // interleave fragments (the old code wrote to stderr piece by piece).
    std::ostringstream oss;
    oss << this->file_path << ":" << line << ":" << (charPositionInLine + 1) << ": error: " << msg << "\n";

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
                if (!std::getline(ss, lineText)) {
                    lineText.clear();
                    break;
                }
            }

            // Print the offending line of code
            oss << " " << line << " | " << lineText << "\n";

            // Print the visual caret (^) pointer underneath the offending character
            oss << std::string(std::to_string(line).length() + 1, ' ') << " | "; // Aligns with the '|'
            oss << std::string(charPositionInLine, ' ') << "^";
        }
    }
    messages.push_back(oss.str());
}
