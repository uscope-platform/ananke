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


#include "frontend/analysis/system_verilog/preprocessor/macro_processor.hpp"
#include "frontend/analysis/system_verilog/preprocessor/conditional_solver.hpp"
#include <spdlog/spdlog.h>
#include <mutex>
#include <unordered_set>

static constexpr size_t MAX_MACRO_EXPANSION_SIZE = 64 * 1024 * 1024;
static constexpr int MAX_NONCONVERGING_PASSES = 16;

namespace {
std::mutex stripped_warn_mutex;
bool uvm_warned = false;
bool ovm_warned = false;

// SystemVerilog keywords (from sv2017Lexer.g4). Fusing a keyword with an
// identifier via `` (e.g. `covergroup ``NAME``_cg` as found in riscv-dv/cva6)
// would destroy the keyword, which no correct expansion intends. Commercial
// simulators tolerate that idiom, so preserve a separating space instead of
// pasting when the left side is a keyword and the right side is an identifier.
bool is_sv_keyword(const std::string_view &word) {
    static const std::unordered_set<std::string_view> keywords = {
        "accept_on", "alias", "always", "always_comb", "always_ff", "always_latch",
        "and", "assert", "assign", "assume", "automatic", "before", "begin", "bind",
        "bins", "binsof", "bit", "break", "buf", "bufif0", "bufif1", "byte", "case",
        "casex", "casez", "cell", "chandle", "checker", "class", "clocking", "cmos",
        "config", "const", "constraint", "context", "continue", "cover", "covergroup",
        "coverpoint", "cross", "deassign", "default", "defparam", "design", "disable",
        "dist", "do", "edge", "else", "end", "endcase", "endchecker", "endclass",
        "endclocking", "endconfig", "endfunction", "endgenerate", "endgroup",
        "endinterface", "endmodule", "endpackage", "endprimitive", "endprogram",
        "endproperty", "endsequence", "endspecify", "endtable", "endtask", "enum",
        "event", "eventually", "expect", "export", "extends", "extern", "final",
        "first_match", "for", "force", "foreach", "forever", "fork", "forkjoin",
        "function", "generate", "genvar", "global", "highz0", "highz1", "if", "iff",
        "ifnone", "ignore_bins", "illegal_bins", "implements", "implies", "import",
        "incdir", "include", "initial", "inout", "input", "inside", "instance", "int",
        "integer", "interconnect", "interface", "intersect", "join", "join_any",
        "join_none", "large", "let", "liblist", "library", "local", "localparam",
        "logic", "longint", "macromodule", "matches", "medium", "modport", "module",
        "nand", "negedge", "nettype", "new", "nexttime", "nmos", "nor",
        "noshowcancelled", "not", "notif0", "notif1", "null", "option", "or",
        "output", "package", "packed", "parameter", "pmos", "posedge", "primitive",
        "priority", "program", "property", "protected", "pull0", "pull1", "pulldown",
        "pullup", "pulsestyle_ondetect", "pulsestyle_onevent", "pure", "rand",
        "randc", "randcase", "randomize", "randsequence", "rcmos", "real", "realtime",
        "ref", "reg", "reject_on", "release", "repeat", "restrict", "return", "rnmos",
        "rpmos", "rtran", "rtranif0", "rtranif1", "s_always", "s_eventually",
        "s_nexttime", "s_until", "s_until_with", "sample", "scalared", "sequence",
        "shortint", "shortreal", "showcancelled", "signed", "small", "soft", "solve",
        "specify", "specparam", "static", "std", "string", "strong", "strong0",
        "strong1", "struct", "super", "supply0", "supply1", "sync_accept_on",
        "sync_reject_on", "table", "tagged", "task", "this", "throughout", "time",
        "timeprecision", "timeunit", "tran", "tranif0", "tranif1", "tri", "tri0",
        "tri1", "triand", "trior", "trireg", "type", "type_option", "typedef",
        "union", "unique", "unique0", "unsigned", "until", "until_with", "untyped",
        "use", "uwire", "var", "vectored", "virtual", "void", "wait", "wait_order",
        "wand", "weak", "weak0", "weak1", "while", "wildcard", "wire", "with",
        "within", "wor", "xnor", "xor"
    };
    return keywords.contains(word);
}

bool is_id_char(char c) {
    return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '$';
}

// Depth backstop for paste-operand pre-expansion (see
// replace_function_macro): recursive paste shapes must degrade to a clean
// diagnostic, never unbounded C++ recursion. Thread-local so alternating
// engine instances share one budget, mirroring the preprocessor's own
// conditional-nesting guard.
constexpr int MAX_PASTE_OPERAND_DEPTH = 32;
thread_local int paste_operand_depth = 0;
struct paste_depth_guard {
    paste_depth_guard() { ++paste_operand_depth; }
    ~paste_depth_guard() { --paste_operand_depth; }
};
bool is_blank_token(const std::string_view &t) {
    return t.find_first_not_of(" \t\r\n") == std::string_view::npos;
}

// IEEE 1800-2017 22.5.1: `" becomes " and `` is deleted when a macro
// expands. Function-like bodies get this in replace_function_macro, but
// object-like bodies (e.g. UVM_VERSION_STRING's
// `"UVM_NAME``-```UVM_MAJOR_REV``.```UVM_MINOR_REV```"`) otherwise leak raw
// operators to the lexer, which has no backtick rule. String-aware: real
// "..." contents pass through verbatim so prose like "call `foo()?" keeps
// its backtick; only operators outside real strings are collapsed.
std::string strip_macro_operators(const std::string &in) {
    if (in.find('`') == std::string::npos) return in;
    std::string out;
    out.reserve(in.size());
    bool in_string = false;
    for (size_t i = 0; i < in.size();) {
        char c = in[i];
        if (in_string) {
            if (c == '\\' && i + 1 < in.size()) {
                out.push_back(c);
                out.push_back(in[i + 1]);
                i += 2;
                continue;
            }
            out.push_back(c);
            if (c == '"') {
                // A backtick-quote inside a real string is content
                // (e.g. "a `" b"), not the closing delimiter.
                const bool macro_quote = i > 0 && in[i - 1] == '`';
                if (!macro_quote) in_string = false;
            }
            ++i;
            continue;
        }
        if (c == '"') {
            in_string = true;
            out.push_back(c);
            ++i;
            continue;
        }
        if (c == '`' && i + 1 < in.size() && in[i + 1] == '"') {
            out.push_back('"');
            i += 2;
            continue;
        }
        if (c == '`' && i + 1 < in.size() && in[i + 1] == '`') {
            // Delete the paste, trimming surrounding blanks like
            // replace_function_macro does.
            while (!out.empty() && (out.back() == ' ' || out.back() == '\t')) out.pop_back();
            i += 2;
            while (i < in.size() && (in[i] == ' ' || in[i] == '\t')) ++i;
            continue;
        }
        out.push_back(c);
        ++i;
    }
    return out;
}
}

namespace preprocessor {
    static std::optional<std::string> arity_error(
        const std::vector<std::string_view> &args, const function_macro &macro, bool enforce_surplus);

    // UVM/OVM macros come from the external simulator library (uvm_macros.svh),
    // which is normally not part of the analyzed repository
    // The warning fires once per macro name per process: on UVM-heavy trees
    // every instance would otherwise format and emit a log line.
    static void warn_stripped_once(const std::string &id, const std::string &path) {
        const char first =
            static_cast<char>(std::tolower(static_cast<unsigned char>(id[0])));
        const char *family = (first == 'u') ? "UVM" : "OVM";
        std::lock_guard<std::mutex> lock(stripped_warn_mutex);
        bool &already_warned = (first == 'u') ? uvm_warned : ovm_warned;
        if (!already_warned) {
            already_warned = true;
            spdlog::warn("Stripping undefined {} macros ({} library not vendored; "
                         "further {} macros stripped silently, e.g. {} in file {})",
                         family, family, family, id, path);
        }
    }
    static bool is_external_methodology_macro(const std::string &id) {
        if (id.size() < 4) return false;
        char c0 = static_cast<char>(std::tolower(static_cast<unsigned char>(id[0])));
        char c1 = static_cast<char>(std::tolower(static_cast<unsigned char>(id[1])));
        char c2 = static_cast<char>(std::tolower(static_cast<unsigned char>(id[2])));
        if (c0 != 'u' && c0 != 'o') return false;
        if (c1 != 'v' || c2 != 'm') return false;
        return id[3] == '_';
    }

    macro_processor::macro_processor(
    macro_definitions_map &d,
        uint64_t &l_n,
        std::string &p,
        std::optional<std::string> &e,
        undefined_uses_map &u,
        bool &f,
        const parse_options &o, const line_remap &r) : definitions(d), line_number(l_n), path(p), error(e), undefined_macros(u),
                                  fatal_error(f), opts(o), remap(r)
    {
    }

    void macro_processor::report_error(const std::string &msg) {
        spdlog::error(msg);
        if (!error) error = msg;
        fatal_error = true;
    }

    void macro_processor::report_undefined(const std::string &id, std::optional<int> arg_count) {
        if (arg_count.has_value()) {
            undefined_macros[id].insert(arg_count.value());
        } else {
            undefined_macros.try_emplace(id);
        }
    }

    std::string macro_processor::process_macro(const std::string_view &in) {
        if (!in.contains('`')) return std::string(in);
        std::string result;
        std::string working = std::string(in);
        std::string prev_working;
        result.reserve(working.size());
        int nesting_counter = 0;
        size_t previous_size = 0;
        int nonconverging_passes = 0;
        bool expansion_needed = true;
        while (expansion_needed && nesting_counter < 1000) {
            result.clear();
            // Work on a view into `working` (a separate buffer) so that
            // advancing `remaining` can never alias the `result` buffer that
            // is being built in this pass.
            std::string_view remaining = working;
            // String-literal awareness for this pass: a `name inside "..." is
            // prose (e.g. "did you forget to call `set_sequencer()?"), not an
            // invocation. State is tracked over the pass *input* (`working`);
            // consumed macro-call text is quote-balanced, hence net-neutral,
            // so only the plain-text gaps need syncing.
            size_t quote_scanned = 0;
            bool in_string = false;
            auto sync_string_state = [&](size_t upto) {
                for (; quote_scanned < upto; ++quote_scanned) {
                    if (working[quote_scanned] != '"') continue;
                    const bool escaped = quote_scanned > 0 && working[quote_scanned - 1] == '\\';
                    // A backtick-quote (`") is a macro stringification
                    // operator, never a real string delimiter: it must not
                    // toggle string state in either direction. Otherwise an
                    // object-like body such as
                    // `"UVM_NAME``-```UVM_MAJOR_REV``..."` enters string
                    // mode at the opening `" and its inner `IDs are then
                    // mistaken for prose and never expanded (while the
                    // closing `" hits the old in_string-only exception and
                    // never toggles back). Real strings containing `" prose
                    // (e.g. "a `" b") still balance: the inner `" is
                    // ignored and the lone closing " toggles out.
                    const bool macro_quote = quote_scanned > 0 &&
                                               working[quote_scanned - 1] == '`';
                    if (!escaped && !macro_quote) in_string = !in_string;
                }
            };
            // Conditionals embedded in the text being expanded (a macro body
            // such as `ifdef ARG `ARG `else ... `endif). They are evaluated
            // later by the caller, but usages in an inactive branch must not
            // be expanded here or reported undefined.
            conditional_solver embedded_cond;
            auto embedded_name = [&](std::string_view after) {
                size_t i = 0;
                while (i < after.size() && (after[i] == ' ' || after[i] == '\t')) ++i;
                size_t j = i;
                while (j < after.size() && (std::isalnum(static_cast<unsigned char>(after[j])) ||
                                            after[j] == '_' || after[j] == '$')) ++j;
                return std::string(after.substr(i, j - i));
            };
            while (auto match = identifier_pattern(remaining)) {
                const size_t mstart = static_cast<size_t>(match.begin() - working.data());
                sync_string_state(mstart);
                if (!in_string) {
                    auto mid = match.view().substr(1);
                    if (!mid.empty() && mid.back() == '(') mid.remove_suffix(1);
                    mid = trim(mid);
                    const std::string_view after{match.end(), remaining.end()};
                    bool handled = true;
                    if (mid == "ifdef")       embedded_cond.start_loop(definitions.contains(embedded_name(after)));
                    else if (mid == "ifndef") embedded_cond.start_loop(!definitions.contains(embedded_name(after)));
                    else if (mid == "elsif")  embedded_cond.advance_elseif(definitions.contains(embedded_name(after)));
                    else if (mid == "else")   embedded_cond.advance_else();
                    else if (mid == "endif")  embedded_cond.close_loop();
                    else handled = false;
                    if (handled || !embedded_cond.is_active()) {
                        // Directive, or any usage in a skipped branch: keep verbatim.
                        result.append(remaining.begin(), match.end());
                        remaining = std::string_view{match.end(), remaining.end()};
                        quote_scanned = static_cast<size_t>(remaining.data() - working.data());
                        continue;
                    }
                }
                if (in_string) {
                    // Literal backtick text: preserve verbatim. A later pass
                    // re-examines it from the line start with fresh state.
                    result.append(remaining.begin(), match.end());
                    remaining = std::string_view{match.end(), remaining.end()};
                    quote_scanned = static_cast<size_t>(remaining.data() - working.data());
                    continue;
                }
                result.append(remaining.begin(), match.begin());
                if (match.view().back() == '(') {
                    auto id_view = match.view().substr(1, match.size()-2);
                    auto id = std::string(trim(id_view));
                    size_t start_pos = (match.data() + match.size()) - remaining.data();
                    auto args_text = remaining.substr(start_pos);
                    if (is_structural_directive(id)) {
                        result.append(match.view());
                        remaining = args_text;
                        quote_scanned = static_cast<size_t>(remaining.data() - working.data());
                        continue;
                    }
                    // Object-like macro followed by '(': not a call. The
                    // parens belong to the surrounding syntax — a module
                    // header after a cell-name define (`CELL_NAME (...)), or
                    // a call to whatever function the replacement names
                    // (`FOO (a, b) with `define FOO my_func). Expand the
                    // bare `ID and rewind to '('; the rescan handles any
                    // macro usages inside the parens. Skipping
                    // get_call_arguments here also avoids misreading e.g. a
                    // port list as macro actuals.
                    if (auto dit = definitions.find(id); dit != definitions.end() &&
                        std::holds_alternative<std::string>(dit->second)) {
                        result.append(get_define_replacement(std::string_view(match.begin(), id.size() + 1)));
                        // Rewind past the bare `ID only, keeping any whitespace
                        // before '(' in the stream for output fidelity.
                        remaining = std::string_view{match.begin() + id.size() + 1, remaining.end()};
                        quote_scanned = static_cast<size_t>(remaining.data() - working.data());
                        continue;
                    }
                    auto [args, rest_of_line] = get_call_arguments(args_text, !opts.strict);
                    if (!definitions.contains(id)) {
                        if (opts.strip_uvm_macros && is_external_methodology_macro(id)) {
                            warn_stripped_once(id, path);
                            // Stripped UVM/OVM calls typically stand in for a
                            // statement (e.g. `else `uvm_error(...)). Expanding
                            // to empty leaves a dangling `else` and collapses
                            // the newlines inside the multi-line call, shifting
                            // all downstream diagnostics. Emit a null statement
                            // and preserve the consumed newlines instead.
                            size_t newlines = 0;
                            if (rest_of_line.data() != nullptr) {
                                for (auto it = match.begin(); it != rest_of_line.begin(); ++it) {
                                    if (*it == '\n') ++newlines;
                                }
                            }
                            result.append(";");
                            result.append(newlines, '\n');
                            remaining = rest_of_line;
                            quote_scanned = static_cast<size_t>(remaining.data() - working.data());
                            continue;
                        }
                        // Record and strip: the file quarantines, but
                        // collecting every unknown (with its call-site arity)
                        // in one pass lets the fixpoint converge instead of
                        // learning one macro per round. Output is discarded
                        // with the file.
                        report_undefined(id, static_cast<int>(args.size()));
                        remaining = rest_of_line;
                        quote_scanned = static_cast<size_t>(remaining.data() - working.data());
                        continue;
                    }
                    auto macro = definitions.at(id);
                    if (std::holds_alternative<std::string>(macro)) {
                        report_error(fmt::format("Attempted to pass arguments to a macro {} that does not need them in file {}", id, path));
                        return "";
                    }
                    auto macro_text = replace_function_macro(args,std::get<function_macro>(macro));
                    if (!macro_text.has_value()) {
                        report_error(fmt::format("Macro [{}] {} in file {}", id,
                                                 arity_error(args, std::get<function_macro>(macro), opts.strict || opts.explicit_file_mode)
                                                     .value_or("could not be expanded"), path));
                        return "";
                    }
                    result.append(macro_text.value());
                    remaining = rest_of_line;
                } else{
                    result.append(get_define_replacement(match));

                    remaining = std::string_view{match.end(), remaining.end()};
                }
                // Consumed call text is quote-balanced (net-neutral for the
                // literal check), so the scan position simply follows.
                quote_scanned = static_cast<size_t>(remaining.data() - working.data());
            }

            result.append(remaining);
            if (result.size() > MAX_MACRO_EXPANSION_SIZE) {
                report_error(fmt::format("Macro expansion exceeded the maximum supported size of {} bytes in file {}", MAX_MACRO_EXPANSION_SIZE, path));
                return "";
            }
            expansion_needed = result.contains('`');
            // Fixed point or 2-cycle (e.g. kept undefined uses, A->B->A):
            // expansion is a deterministic function of text+definitions, so
            // further passes repeat forever. Stop at once instead of burning
            // all remaining passes and feeding non-converged text (with
            // surviving directives) downstream.
            // NOTE: the 2-cycle path must return the newest state (result),
            // while every other exit returns working (which the rotation
            // below keeps pointing at the latest text). Returning working
            // here would hand back the previous cycle state.
            // Operator cleanup applies on the way out: surviving `" / ``
            // come from object-like bodies (see strip_macro_operators) and
            // are removed only after nested `IDs had every pass to expand.
            if (result == prev_working) return strip_macro_operators(result);
            if (result == working) break;
            // A self-multiplying recursive macro keeps growing every pass and
            // never converges; a legitimate expansion (e.g. UVM field utils)
            // converges within a couple of passes. Detect sustained growth early
            // so the tool errors instead of exhausting memory.
            if (expansion_needed && result.size() > previous_size) {
                nonconverging_passes++;
            } else {
                nonconverging_passes = 0;
            }
            if (nonconverging_passes >= MAX_NONCONVERGING_PASSES) {
                report_error(fmt::format("Macro expansion did not converge, possible recursive macro in file {}", path));
                return "";
            }
            previous_size = result.size();
            // Rotate buffers instead of copying: working takes the new text,
            // prev_working takes last pass's input (for the fixed-point
            // check above), result keeps a reusable scratch buffer that
            // clear() at the loop top preserves the capacity of.
            prev_working.swap(working);
            working.swap(result);
            nesting_counter++;
        }

        return strip_macro_operators(working);
    }


    std::pair<std::vector<std::string_view>, std::string_view> macro_processor::get_call_arguments(const std::string_view &in, bool empty_is_one_empty) {
        std::vector<std::string_view> arguments;
        int nesting_level = 0;
        int args_last = 0;
        bool in_string_literal = false;
        for (; args_last< in.size(); args_last++) {
            if (in[args_last]=='"') {
                if (args_last == 0 || in[args_last-1] != '\\') {
                    in_string_literal = !in_string_literal;
                }

            }
            if (in[args_last] == '(' && !in_string_literal) nesting_level++;
            if (in[args_last] == ')'&& !in_string_literal) {
                if (nesting_level>0) nesting_level--;
                else break;
            }
        }

        if (args_last == in.size()) {
            return {};
        }
        in_string_literal = false;
        auto raw_arguments = in.substr(0, args_last);
        // `f()` supplies one empty actual (commercial tools agree; the
        // empty formal then expands to nothing, and a trailing comma in
        // the resulting call means "use default"). Without this, empty
        // parens read as zero actuals and error out below. Gated on
        // lenient mode: strict rejects it via the required-count check in
        // replace_function_macro.
        if (empty_is_one_empty && trim(raw_arguments).empty()) {
            auto value = in.substr(args_last + 1);
            return {{{}}, value};
        }
        int current_arg_start = 0;
        for (int i = 0; i< raw_arguments.size(); i++) {
            const auto c = raw_arguments[i];
            if (c=='"') {
                if (i == 0 || raw_arguments[i-1] != '\\') {
                    in_string_literal = !in_string_literal;
                }
            }

            if ((c == '(' || c == '[' || c == '{') && !in_string_literal) nesting_level++;
            if ((c == ')' || c == ']' || c == '}') && !in_string_literal) nesting_level--;
            if (c==',' && nesting_level == 0||i == raw_arguments.size()-1) {
                if (!in_string_literal) {
                    auto string_length = i-current_arg_start+1;
                    if (c==',') string_length--;
                    auto arg_text = raw_arguments.substr(current_arg_start, string_length);
                    current_arg_start = i+1;
                    // Keep the actual verbatim (leading indentation of
                    // continuation-line actuals is significant to callers
                    // that splice values into output text); blank detection
                    // for defaults happens at substitution time.
                    arguments.emplace_back(ltrim(arg_text));
                }
            }
        }

        if (!raw_arguments.empty() && raw_arguments.back() == ',') {
            arguments.emplace_back("");
        }

        auto value =  in.substr(args_last+1);
        return {arguments, value};
    }


    std::string_view macro_processor::ltrim(const std::string_view &in) {
        if (in.empty()) {
            return in;
        }
        const auto start = in.find_first_not_of(" \t");
        if (start == std::string_view::npos) {
            return in;
        }
        return in.substr(start);
    }

    std::string_view macro_processor::trim(const std::string_view &in) {
        if (in.empty())
            return in;
        // Leading set must cover newlines as well: multi-line invocations
        // leave `\n<indent>` at the head of continuation-line actuals, and a
        // residue there defeats default substitution and `` paste fusion
        // (phantom `dv_ from `dv_``SEV_ with blank-looking SEV_).
        const auto start = in.find_first_not_of(" \t\r\n");
        if (start == std::string_view::npos)
            return "";
        const auto end = in.find_last_not_of(" \t\r\n");
        return in.substr(start, end - start + 1);
    }


    // IEEE 1800-2017 22.5.1: actuals bind positionally, and every formal past
    // the last actual needs a default. `f()` supplies one (empty) actual, so
    // a single-formal macro expands while `ADD()` for two formals without
    // defaults still errors, matching commercial tools. More actuals than
    // formals is an error too, except the lone empty actual of `m()` against
    // a formal-less macro. Repository runs in lenient mode tolerate it
    // (trailing blank actuals, e.g. dv_macros.svh's `WRAP(msg, , )`), so that
    // part is enforced only when `enforce_surplus` (strict or explicit
    // `--parse` mode).
    static std::optional<std::string> arity_error(
        const std::vector<std::string_view> &args, const function_macro &macro, bool enforce_surplus) {
        for (size_t i = args.size(); i < macro.arguments.size(); ++i) {
            if (!macro.arguments[i].has_default) {
                return fmt::format("was called without an actual for formal '{}', which has no default",
                                   macro.arguments[i].name);
            }
        }
        if (enforce_surplus && args.size() > macro.arguments.size() &&
            !(macro.arguments.empty() && args.size() == 1 && macro_processor::trim(args[0]).empty())) {
            return fmt::format("was called with {} actual argument(s) but has only {} formal(s)",
                               args.size(), macro.arguments.size());
        }
        return std::nullopt;
    }

    std::optional<std::string> macro_processor::replace_function_macro(const std::vector<std::string_view> &args, const function_macro &macro) {
        if (arity_error(args, macro, opts.strict || opts.explicit_file_mode).has_value()) return std::nullopt;

        std::string_view body = macro.value;
        if (body.empty()) return "";
        while (!body.empty() && (body.back() == ' ' || body.back() == '\t' || body.back() == '\r' || body.back() == '\n' || body.back() == '\\')) {
            body.remove_suffix(1);
        }
        std::vector<std::string_view> tokens;

        auto is_valid_id_char = [](char c) {
            return std::isalnum(static_cast<unsigned char>(c)) || c == '_' || c == '$';
        };

        bool in_token = is_valid_id_char(body[0]);
        int current_token_start = 0;
        for (int i = 0; i<body.size(); i++){
            if (body[i] == '`' && i + 1 < body.size() && body[i + 1] == '"') {
                if (i > current_token_start) {
                    tokens.push_back(body.substr(current_token_start, i - current_token_start));
                }
                tokens.push_back(body.substr(i, 2));
                i++; // Advance loop pointer past the double quote
                current_token_start = i + 1;
                if (current_token_start < body.size()) in_token = is_valid_id_char(body[current_token_start]);
                continue;
            }

            if (body[i] == '`' && i + 1 < body.size() && body[i + 1] == '`') {
                if (i > current_token_start) {
                    tokens.push_back(body.substr(current_token_start, i - current_token_start));
                }
                tokens.push_back(body.substr(i, 2));
                i++; // Advance loop pointer past the second backtick
                current_token_start = i + 1;
                if (current_token_start < body.size()) in_token = is_valid_id_char(body[current_token_start]);
                continue;
            }

            bool current_char_valid = is_valid_id_char(body[i]);


            if (in_token & !current_char_valid) {
                in_token = false;
                tokens.push_back(body.substr(current_token_start, i-current_token_start));
                current_token_start = i;
            } else if (!in_token & current_char_valid) {
                in_token = true;
                tokens.push_back(body.substr(current_token_start, i-current_token_start));
                current_token_start = i;
            }
        }

        if (current_token_start < body.size()) {
            tokens.push_back(body.substr(current_token_start));
        }

        // Formals adjacent to a `` paste operator (nearest non-blank token
        // on either side). Their actuals are expanded before substitution
        // (see below); every other formal substitutes verbatim as before.
        std::unordered_set<std::string_view> paste_formals;
        for (size_t k = 0; k < tokens.size(); ++k) {
            if (tokens[k] != "``") continue;
            size_t l = k;
            while (l > 0 && is_blank_token(tokens[l - 1])) --l;
            if (l > 0) {
                for (const auto &formal : macro.arguments) {
                    if (tokens[l - 1] == formal.name) paste_formals.insert(tokens[l - 1]);
                }
            }
            size_t r = k + 1;
            while (r < tokens.size() && is_blank_token(tokens[r])) ++r;
            if (r < tokens.size()) {
                for (const auto &formal : macro.arguments) {
                    if (tokens[r] == formal.name) paste_formals.insert(tokens[r]);
                }
            }
        }

        std::unordered_map<std::string_view, std::string> arguments_map;
        for (int i = 0; i<macro.arguments.size(); i++) {
            // A whitespace-only actual (`, ,`, or a continuation line holding
            // only indentation) counts as empty: the formal default applies
            // (LRM 22.5.1). The stored actual itself stays verbatim so plain
            // substitution preserves the call-site text — except for paste
            // operands below.
            std::string actual;
            if (i >= args.size() || trim(args[i]).empty()) {
                actual = std::string(macro.arguments[i].default_value);
            } else {
                actual = std::string(args[i]);
            }
            // A paste operand that is itself a macro usage must expand
            // before fusing, matching commercial simulators: verbatim
            // substitution would fuse the backtick-led name into a longer,
            // bogus macro usage (caliptra's `PREFIX``_``and2 fusing to
            // `PREFIX_and2 instead of `generic_and2`). Only paste-adjacent
            // formals are pre-expanded: an unused actual's macros must never
            // report (the rescan pass expands everything else in due course).
            if (!actual.empty() && actual.find('`') != std::string::npos &&
                paste_formals.contains(std::string_view(macro.arguments[i].name))) {
                if (paste_operand_depth >= MAX_PASTE_OPERAND_DEPTH) {
                    report_error(fmt::format("Macro paste-operand expansion exceeded maximum depth ({}) in file {}",
                                             MAX_PASTE_OPERAND_DEPTH, path));
                    return "";
                }
                paste_depth_guard guard;
                std::string expanded = process_macro(actual);
                if (fatal_error) return "";
                actual = std::move(expanded);
            }
            arguments_map[macro.arguments[i].name] = std::move(actual);
        }

        // Views only: arg values live in arguments_map (no inserts after
        // this point, so references are stable), other tokens view the macro
        // body. Avoids one std::string copy per token per macro call.
        std::vector<std::string_view> expanded_tokens;
        std::vector<char> expanded_from_arg;
        expanded_tokens.reserve(tokens.size());
        expanded_from_arg.reserve(tokens.size());
        for (const auto &t: tokens) {
            auto arg_it = arguments_map.find(t);
            if (arg_it != arguments_map.end()) {
                expanded_tokens.emplace_back(arg_it->second);
                expanded_from_arg.push_back(1);
            } else {
                expanded_tokens.emplace_back(t);
                expanded_from_arg.push_back(0);
            }
        }

        std::string result;
        result.reserve(body.size() + 32);
        // Running quote parity of `result`. The old code ran
        // std::count(result.begin(), result.end(), '"') per token (O(n^2)
        // on long expansions); counting '"' bytes of each appended piece
        // yields the same total since appends never remove quotes (the ``
        // branch only trims spaces/tabs).
        size_t result_quote_count = 0;
        auto append_counted = [&](std::string_view s) {
            for (char c : s) if (c == '"') ++result_quote_count;
            result.append(s);
        };
        for (size_t i = 0; i < expanded_tokens.size(); ++i) {
            // Evaluates the isolated stringification token sequence back into standard quotes
            if (expanded_tokens[i] == "`\"") {
                result.push_back('"');
                ++result_quote_count;
                continue;
            }

            if (expanded_tokens[i] == "``") {
                // Peek at the identifier tail on the left and the head on the
                // right. Pasting is meant to build one identifier from
                // fragments, but the `covergroup ``NAME``_cg` idiom puts a
                // paste between a keyword and a macro argument
                // (`covergroup ``fmul_d`), which would destroy the keyword.
                // Commercial simulators tolerate that idiom, so keep a
                // separating space only there: pastes onto a literal suffix
                // (`xor``_cg`) still fuse, even when the argument is
                // lexically a keyword, since that is the intended name.
                std::string_view left_word;
                {
                    size_t end = result.size();
                    while (end > 0 && (result[end - 1] == ' ' || result[end - 1] == '\t')) --end;
                    size_t start = end;
                    while (start > 0 && is_id_char(result[start - 1])) --start;
                    left_word = std::string_view(result.data() + start, end - start);
                }
                size_t next_idx = i + 1;
                while (next_idx < expanded_tokens.size() &&
                        expanded_tokens[next_idx].find_first_not_of(" \t") == std::string_view::npos) {
                    ++next_idx;
                }
                // A substituted actual from a continuation line starts with
                // `\n<indent>` (kept verbatim for plain substitution). For a
                // paste the leading whitespace is not part of the fused
                // identifier: drop it inside the token so `frag`` + `\n  id`
                // fuses to `fragid` instead of leaving a phantom `frag` use.
                // Literal body text is left untouched.
                if (next_idx < expanded_tokens.size() && expanded_from_arg[next_idx]) {
                    const auto lead = expanded_tokens[next_idx].find_first_not_of(" \t\r\n");
                    if (lead != std::string_view::npos && lead > 0) {
                        expanded_tokens[next_idx].remove_prefix(lead);
                    }
                }
                std::string_view right_tok;
                if (next_idx < expanded_tokens.size()) right_tok = expanded_tokens[next_idx];
                size_t right_pos = right_tok.find_first_not_of(" \t\r\n");
                bool keyword_paste = !left_word.empty() && right_pos != std::string_view::npos &&
                    is_sv_keyword(left_word) && is_id_char(right_tok[right_pos]) &&
                    expanded_from_arg[next_idx];

                // Trim trailing whitespace from the result accumulated so far
                // Only whitespace spelled in the macro body counts: trailing
                // blanks inside a substituted actual (`m( 4 , 5 )`) are not a
                // separator and must still fuse (`i``a``b``j`).
                const bool ws_before_paste = !result.empty() &&
                    (result.back() == ' ' || result.back() == '\t') &&
                    i > 0 && !expanded_from_arg[i - 1] &&
                    expanded_tokens[i - 1].find_first_not_of(" \t") == std::string_view::npos;
                while (!result.empty() && (result.back() == ' ' || result.back() == '\t')) {
                    result.pop_back();
                }
                // IEEE 1800-2017 22.5.1: `` joins the two tokens adjacent to
                // it. Whitespace written before it in the body separates the
                // left token from the pasted name (`struct_t ``port``_i`
                // declares a variable of type struct_t), so keep one space.
                // A paste after a keyword/identifier tail that is itself
                // spelled `word ``frag` therefore stays two tokens.
                const bool separating_paste = ws_before_paste && !left_word.empty() &&
                    !keyword_paste && right_pos != std::string_view::npos &&
                    expanded_from_arg[next_idx];

                // `localparam `` i``a``j`: a paste written with a space after it
                // in the body, directly after a keyword, separates the keyword
                // from the fused name instead of destroying it.
                const bool spaced_keyword_paste = !keyword_paste && !left_word.empty() &&
                    right_pos != std::string_view::npos && is_sv_keyword(left_word) &&
                    is_id_char(right_tok[right_pos]) && i + 1 < expanded_tokens.size() &&
                    next_idx > i + 1 && !expanded_from_arg[next_idx];

                // Skip the `` operator and any immediate whitespace following it
                while (i + 1 < expanded_tokens.size()) {
                    const auto& next_tok = expanded_tokens[i + 1];
                    if (next_tok.find_first_not_of(" \t") == std::string::npos) {
                        i++; // consume whitespace token
                    } else {
                        break;
                    }
                }

                if (keyword_paste || spaced_keyword_paste || separating_paste) result.push_back(' ');
            } else {

                const std::string_view token_to_append = expanded_tokens[i];

                bool currently_in_string = (result_quote_count % 2 != 0);

                if (!currently_in_string ||
                    (token_to_append.find('\n') == std::string_view::npos &&
                     token_to_append.find('\r') == std::string_view::npos)) {
                    append_counted(token_to_append);
                } else {
                    // Flatten newlines into spaces inside string literals.
                    // Only materialize a copy when the token actually
                    // contains a newline (rare); the common case appends
                    // the view directly.
                    std::string flattened(token_to_append);
                    for (char &c : flattened) {
                        if (c == '\n' || c == '\r') {
                            c = ' '; // Flatten the newline into a harmless space inside the string literal
                        }
                    }
                    append_counted(flattened);
                }
            }
        }
        return result;
    }



    std::string macro_processor::get_define_replacement(const std::string_view &identifier) {
        std::string_view purged_identifier = {identifier.begin()+1, identifier.end()};
        std::string replacement;
        if (purged_identifier == "__FILE__") {
            replacement = "\"" + remap.file.value_or(path) + "\"";

        } else if (purged_identifier == "__LINE__"){
            replacement = std::to_string(static_cast<int64_t>(line_number) + remap.offset);
        } else {
            // Prevent embedded structural directives from being evaluated as substitution macros
            if (is_structural_directive(purged_identifier)) {
                return std::string(identifier);
            }
            auto id = std::string(purged_identifier);
            if (!definitions.contains(id)) {
                if (opts.strip_uvm_macros && is_external_methodology_macro(id)) {
                    warn_stripped_once(id, path);
                    return "";
                }
                // Bare use (no parentheses): no arity information to record.
                report_undefined(id, std::nullopt);
                return std::string(identifier);
            }
            auto def = definitions.at(id);
            if (std::holds_alternative<std::string>(def)) {
                replacement = std::get<std::string>(def);
            } else {
                // IEEE 1800-2017 22.5.1: the parentheses are always required
                // for a macro with formal arguments, even if all have defaults.
                report_error(fmt::format("Macro [{}] takes arguments and must be called with parentheses in file {}",
                                         id, path));
                return std::string(identifier);
            }
        }
        return replacement;
    }

    std::optional<function_macro> macro_processor::parse_function_macro(const std::string_view &in) {
        function_macro macro;
        auto [arguments, body] = get_call_arguments(in);

        for (auto &m:arguments) {
            function_macro_argument arg;
            if (m.contains('=')) {
                arg.has_default = true;
                arg.default_value = trim(m.substr(m.find_first_of('=')+1));
                arg.name = trim(m.substr(0, m.find_first_of('=')));
            } else {
                arg.name = trim(m);
                arg.has_default = false;
            }
            macro.arguments.emplace_back(arg);
        }
        macro.value = ltrim(body);
        return macro;
    }
}
