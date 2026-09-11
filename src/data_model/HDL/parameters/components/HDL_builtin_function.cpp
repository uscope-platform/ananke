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

#include "data_model/HDL/parameters/components/HDL_builtin_function.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"
#include "data_model/HDL/types/HDL_union_type.hpp"
#include "analysis/type_cast_engine.hpp"

#include <spdlog/spdlog.h>
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

#include <algorithm>
#include <cctype>
#include <cmath>
#include <iomanip>
#include <map>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

CEREAL_REGISTER_TYPE(HDL_builtin_function)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, HDL_builtin_function)

std::optional<HDL_builtin_function::function> HDL_builtin_function::from_sv(std::string_view name) {
    if (!name.empty() && name.front() == '$') name.remove_prefix(1);
    static const std::map<std::string_view, function> table = {
        {"bits", function::bits}, {"size", function::size},
        {"left", function::left}, {"right", function::right},
        {"high", function::high}, {"low", function::low},
        {"dimensions", function::dimensions}, {"unpacked_dimensions", function::unpacked_dimensions},
        {"typename", function::typename_fn}, {"signed", function::signed_fn}, {"unsigned", function::unsigned_fn},
        {"rtoi", function::rtoi}, {"itor", function::itor},
        {"ceil", function::ceil}, {"floor", function::floor},
        {"round", function::round}, {"truncate", function::truncate},
        {"ln", function::ln}, {"log10", function::log10}, {"log2", function::log2}, {"clog2", function::clog2},
        {"sqrt", function::sqrt}, {"pow", function::pow}, {"exp", function::exp}, {"hypot", function::hypot},
        {"min", function::min}, {"max", function::max},
        {"countones", function::countones}, {"countbits", function::countbits},
        {"sin", function::sin}, {"cos", function::cos}, {"tan", function::tan},
        {"asin", function::asin}, {"acos", function::acos}, {"atan", function::atan}, {"atan2", function::atan2},
        {"sinh", function::sinh}, {"cosh", function::cosh}, {"tanh", function::tanh},
        {"asinh", function::asinh}, {"acosh", function::acosh}, {"atanh", function::atanh},
        {"onehot", function::onehot}, {"onehot0", function::onehot0},
        {"isunknown", function::isunknown}, {"isunbounded", function::isunbounded},
        {"realtobits", function::realtobits}, {"bitstoreal", function::bitstoreal},
        {"shortrealtobits", function::shortrealtobits}, {"bitstoshortreal", function::bitstoshortreal},
        {"len", function::len}, {"substr", function::substr},
        {"to_lower", function::to_lower}, {"to_upper", function::to_upper},
        {"atoi", function::atoi}, {"compare", function::compare}, {"sformatf", function::sformatf},
        {"random", function::random}, {"urandom", function::urandom}, {"urandom_range", function::urandom_range},
        {"dist_uniform", function::dist_uniform}, {"dist_normal", function::dist_normal},
        {"dist_exponential", function::dist_exponential}, {"dist_poisson", function::dist_poisson},
        {"dist_chi_square", function::dist_chi_square}, {"dist_t", function::dist_t},
        {"dist_erlang", function::dist_erlang}
    };
    auto it = table.find(name);
    if (it == table.end()) return std::nullopt;
    return it->second;
}

std::optional<HDL_builtin_function::function> HDL_builtin_function::from_vhdl(std::string_view name) {
    static const std::map<std::string_view, function> table = {
        {"ceil", function::ceil}, {"floor", function::floor}, {"round", function::round},
        {"trunc", function::truncate}, {"sqrt", function::sqrt},
        {"log", function::ln}, {"log10", function::log10}, {"log2", function::log2},
        {"exp", function::exp}, {"pow", function::pow},
        {"minimum", function::min}, {"maximum", function::max},
        {"sin", function::sin}, {"cos", function::cos}, {"tan", function::tan},
        {"asin", function::asin}, {"acos", function::acos}, {"atan", function::atan},
        {"atan2", function::atan2}, {"sinh", function::sinh}, {"cosh", function::cosh},
        {"tanh", function::tanh}, {"hypot", function::hypot}, {"countones", function::countones}
    };
    auto it = table.find(name);
    if (it == table.end()) return std::nullopt;
    return it->second;
}

std::optional<HDL_builtin_function::function> HDL_builtin_function::parse(std::string_view name) {
    if (auto f = from_sv(name)) return f;
    return from_vhdl(name);
}

std::string_view HDL_builtin_function::sv_name(function f) {
    static const std::map<function, std::string_view> names = {
        {function::bits, "$bits"}, {function::size, "$size"},
        {function::left, "$left"}, {function::right, "$right"},
        {function::high, "$high"}, {function::low, "$low"},
        {function::dimensions, "$dimensions"}, {function::unpacked_dimensions, "$unpacked_dimensions"},
        {function::typename_fn, "$typename"}, {function::signed_fn, "$signed"}, {function::unsigned_fn, "$unsigned"},
        {function::rtoi, "$rtoi"}, {function::itor, "$itor"},
        {function::ceil, "$ceil"}, {function::floor, "$floor"},
        {function::round, "$round"}, {function::truncate, "$truncate"},
        {function::ln, "$ln"}, {function::log10, "$log10"}, {function::log2, "$log2"}, {function::clog2, "$clog2"},
        {function::sqrt, "$sqrt"}, {function::pow, "$pow"}, {function::exp, "$exp"}, {function::hypot, "$hypot"},
        {function::min, "$min"}, {function::max, "$max"},
        {function::countones, "$countones"}, {function::countbits, "$countbits"},
        {function::sin, "$sin"}, {function::cos, "$cos"}, {function::tan, "$tan"},
        {function::asin, "$asin"}, {function::acos, "$acos"}, {function::atan, "$atan"}, {function::atan2, "$atan2"},
        {function::sinh, "$sinh"}, {function::cosh, "$cosh"}, {function::tanh, "$tanh"},
        {function::asinh, "$asinh"}, {function::acosh, "$acosh"}, {function::atanh, "$atanh"},
        {function::onehot, "$onehot"}, {function::onehot0, "$onehot0"},
        {function::isunknown, "$isunknown"}, {function::isunbounded, "$isunbounded"},
        {function::realtobits, "$realtobits"}, {function::bitstoreal, "$bitstoreal"},
        {function::shortrealtobits, "$shortrealtobits"}, {function::bitstoshortreal, "$bitstoshortreal"},
        {function::len, "$len"}, {function::substr, "$substr"},
        {function::to_lower, "$to_lower"}, {function::to_upper, "$to_upper"},
        {function::atoi, "$atoi"}, {function::compare, "$compare"}, {function::sformatf, "$sformatf"},
        {function::random, "$random"}, {function::urandom, "$urandom"}, {function::urandom_range, "$urandom_range"},
        {function::dist_uniform, "$dist_uniform"}, {function::dist_normal, "$dist_normal"},
        {function::dist_exponential, "$dist_exponential"}, {function::dist_poisson, "$dist_poisson"},
        {function::dist_chi_square, "$dist_chi_square"}, {function::dist_t, "$dist_t"},
        {function::dist_erlang, "$dist_erlang"}
    };
    auto it = names.find(f);
    if (it == names.end()) return "";
    return it->second;
}

bool HDL_builtin_function::is_known(std::string_view name) {
    return parse(name).has_value();
}

namespace {

std::string unquote(const std::string &s) {
    if (s.size() >= 2 && s.front() == '"' && s.back() == '"')
        return s.substr(1, s.size() - 2);
    return s;
}

std::string to_binary_string(wide_integer v, int width) {
    if (v == 0) return width > 0 ? std::string(width, '0') : "0";
    std::string s;
    while (v != 0) {
        s.push_back((v & 1) ? '1' : '0');
        v >>= 1;
    }
    std::reverse(s.begin(), s.end());
    if (width > 0 && static_cast<int>(s.size()) < width) s = std::string(width - s.size(), '0') + s;
    return s;
}

std::string format_value(const resolved_parameter &arg, char spec, int width, bool zero_pad) {
    std::ostringstream piece;
    switch (spec) {
        case 'd': case 'i':
            if (arg.is_real()) piece << static_cast<int64_t>(arg.get_real());
            else if (arg.is_integer()) piece << arg.get_integer().to_wide();
            else if (arg.is_string()) piece << arg.get_string();
            else piece << 0;
            break;
        case 'x': case 'h':
            piece << std::hex;
            if (zero_pad && width > 0) piece << std::setw(width) << std::setfill('0');
            if (arg.is_integer()) piece << arg.get_integer().to_wide();
            else if (arg.is_real()) piece << static_cast<int64_t>(arg.get_real());
            else piece << 0;
            break;
        case 'X': case 'H':
            piece << std::hex << std::uppercase;
            if (zero_pad && width > 0) piece << std::setw(width) << std::setfill('0');
            if (arg.is_integer()) piece << arg.get_integer().to_wide();
            else if (arg.is_real()) piece << static_cast<int64_t>(arg.get_real());
            else piece << 0;
            break;
        case 'o':
            piece << std::oct;
            if (zero_pad && width > 0) piece << std::setw(width) << std::setfill('0');
            if (arg.is_integer()) piece << arg.get_integer().to_wide();
            else if (arg.is_real()) piece << static_cast<int64_t>(arg.get_real());
            else piece << 0;
            break;
        case 'b':
            if (arg.is_integer()) piece << to_binary_string(arg.get_integer().to_wide(), zero_pad ? width : 0);
            else if (arg.is_real()) piece << to_binary_string(static_cast<int64_t>(arg.get_real()), zero_pad ? width : 0);
            else piece << 0;
            break;
        case 's':
            if (arg.is_string()) piece << unquote(arg.get_string());
            else if (arg.is_integer()) piece << arg.get_integer().to_string();
            else if (arg.is_real()) piece << arg.get_real();
            break;
        case 'f': case 'F':
            if (arg.is_real()) piece << std::fixed << arg.get_real();
            else if (arg.is_integer()) piece << std::fixed << arg.get_integer().to_double();
            else piece << std::fixed << 0.0;
            break;
        case 'e': case 'E':
            if (arg.is_real()) piece << std::scientific << arg.get_real();
            else if (arg.is_integer()) piece << std::scientific << arg.get_integer().to_double();
            else piece << std::scientific << 0.0;
            break;
        case 'g': case 'G':
            if (arg.is_real()) piece << arg.get_real();
            else if (arg.is_integer()) piece << arg.get_integer().to_double();
            else piece << 0.0;
            break;
        default:
            piece << "%" << spec;
            break;
    }
    return piece.str();
}

std::string format_string(const std::string &fmt, const std::vector<resolved_parameter> &args) {
    std::ostringstream out;
    size_t arg_idx = 0;
    for (size_t i = 0; i < fmt.size(); i++) {
        if (fmt[i] != '%') { out << fmt[i]; continue; }
        i++;
        if (i >= fmt.size()) { out << "%"; break; }
        if (fmt[i] == '%') { out << "%"; continue; }
        bool zero_pad = false;
        if (fmt[i] == '0') { zero_pad = true; i++; }
        int width = 0;
        while (i < fmt.size() && std::isdigit(static_cast<unsigned char>(fmt[i]))) {
            width = width * 10 + (fmt[i] - '0');
            i++;
        }
        if (i >= fmt.size()) break;
        char spec = fmt[i];
        if (arg_idx >= args.size()) { out << "%" << spec; continue; }
        out << format_value(args[arg_idx++], spec, width, zero_pad);
    }
    return out.str();
}

} // namespace

std::expected<resolved_parameter, solver_errors> HDL_builtin_function::evaluate(const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) {
    switch (function_kind) {
        case function::bits: case function::size:
        case function::left: case function::right:
        case function::high: case function::low:
        case function::dimensions: case function::unpacked_dimensions:
            return evaluate_type_query(context);
        case function::typename_fn:
            return evaluate_typename(context);
        case function::signed_fn: case function::unsigned_fn:
            return evaluate_signedness(context);
        default:
            break;
    }

    std::vector<resolved_parameter> resolved_arguments;
    for (auto &arg:arguments) {
        auto resolved_val = arg->evaluate(context);
        if (!resolved_val.has_value()) return std::unexpected{missing_value};
        resolved_arguments.push_back(resolved_val.value());
    }
    if (resolved_arguments.empty()) {
        spdlog::warn("System task {} requires at least one argument", sv_name(function_kind));
        return std::unexpected{missing_arguments};
    }
    if (function_kind == function::rtoi) {
        if (resolved_arguments[0].is_real()) {
            return static_cast<hdl_integer>(std::trunc(resolved_arguments[0].get_real()));
        }
        if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer();
        }
        spdlog::warn("Encountered an invalid argument for a $rtoi call");
        return  0;
    }
    if (function_kind == function::itor) {
        if (resolved_arguments[0].is_real()) {
            return resolved_arguments[0].get_real();
        } else if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer().to_double();
        }
        spdlog::warn("Encountered an invalid argument for a $itor call");
        return  0;
    }
    if (function_kind == function::ceil) {
        if (resolved_arguments[0].is_real()) {
            return std::ceil(resolved_arguments[0].get_real());
        } else if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer();
        }
        spdlog::warn("Encountered an invalid argument for a $ceil call");
    }
    if (function_kind == function::floor) {
        if (resolved_arguments[0].is_real()) {
            return std::floor(resolved_arguments[0].get_real());
        } else if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer();
        }
        spdlog::warn("Encountered an invalid argument for a $floor call");

    }
    if (function_kind == function::ln) {
        double arg;
        if (resolved_arguments[0].is_real()) arg = resolved_arguments[0].get_real();
        else if (resolved_arguments[0].is_integer()) arg = resolved_arguments[0].get_integer().to_double();
        else { spdlog::warn("Encountered an invalid argument for a $ln call"); return 0; }
        if (arg <= 0) { spdlog::warn("$ln argument must be positive"); return 0; }
        return std::log(arg);
    }
    if (function_kind == function::log10) {
        double arg;
        if (resolved_arguments[0].is_real()) arg = resolved_arguments[0].get_real();
        else if (resolved_arguments[0].is_integer()) arg = resolved_arguments[0].get_integer().to_double();
        else { spdlog::warn("Encountered an invalid argument for a $log10 call"); return 0; }
        if (arg <= 0) { spdlog::warn("$log10 argument must be positive"); return 0; }
        return std::log10(arg);
    }
    if (function_kind == function::sqrt) {
        double arg;
        if (resolved_arguments[0].is_real()) arg = resolved_arguments[0].get_real();
        else if (resolved_arguments[0].is_integer()) arg = resolved_arguments[0].get_integer().to_double();
        else { spdlog::warn("Encountered an invalid argument for a $sqrt call"); return 0; }
        if (arg < 0) { spdlog::warn("$sqrt argument must be non-negative"); return 0; }
        return std::sqrt(arg);
    }
    if (function_kind == function::pow) {
        if (resolved_arguments.size() < 2) {
            spdlog::warn("$pow requires exactly 2 arguments");
            return 0;
        }
        if (!(resolved_arguments[0].is_integer() || resolved_arguments[0].is_real()) ||
            !(resolved_arguments[1].is_integer() || resolved_arguments[1].is_real())) {
            spdlog::warn("Encountered an invalid argument for a $pow call");
            return 0;
        }
        double base = resolved_arguments[0].is_real()
                          ? resolved_arguments[0].get_real()
                          : resolved_arguments[0].get_integer().to_double();
        double exponent = resolved_arguments[1].is_real()
                              ? resolved_arguments[1].get_real()
                              : resolved_arguments[1].get_integer().to_double();
        return std::pow(base, exponent);
    }
    if (function_kind == function::min) {
        if (resolved_arguments.empty()) {
            spdlog::warn("$min requires at least 1 argument");
            return 0;
        }
        if (resolved_arguments.size() == 1) return resolved_arguments[0];
        bool all_int = std::ranges::all_of(resolved_arguments, [](const auto &a) { return a.is_integer(); });
        if (all_int) {
            auto result = resolved_arguments[0].get_integer();
            for (size_t i = 1; i < resolved_arguments.size(); i++) {
                if (resolved_arguments[i].get_integer() < result) result = resolved_arguments[i].get_integer();
            }
            return result;
        } else {
            double result = 0.0;
            bool result_set = false;
            for (const auto &a : resolved_arguments) {
                if (!(a.is_integer() || a.is_real())) {
                    spdlog::warn("Encountered an invalid argument for a $min call");
                    return 0;
                }
                double v = a.is_real() ? a.get_real() : a.get_integer().to_double();
                if (!result_set) { result = v; result_set = true; }
                else if (v < result) result = v;
            }
            return result;
        }
    }
    if (function_kind == function::max) {
        if (resolved_arguments.empty()) {
            spdlog::warn("$max requires at least 1 argument");
            return 0;
        }
        if (resolved_arguments.size() == 1) return resolved_arguments[0];
        bool all_int = std::ranges::all_of(resolved_arguments, [](const auto &a) { return a.is_integer(); });
        if (all_int) {
            auto result = resolved_arguments[0].get_integer();
            for (size_t i = 1; i < resolved_arguments.size(); i++) {
                if (result < resolved_arguments[i].get_integer()) result = resolved_arguments[i].get_integer();
            }
            return result;
        } else {
            double result = 0.0;
            bool result_set = false;
            for (const auto &a : resolved_arguments) {
                if (!(a.is_integer() || a.is_real())) {
                    spdlog::warn("Encountered an invalid argument for a $max call");
                    return 0;
                }
                double v = a.is_real() ? a.get_real() : a.get_integer().to_double();
                if (!result_set) { result = v; result_set = true; }
                else if (result < v) result = v;
            }
            return result;
        }
    }
    if (function_kind == function::countones) {
        if (resolved_arguments[0].is_integer()) {
            wide_integer val = resolved_arguments[0].get_integer().to_wide();
            int64_t count = 0;
            while (val != 0) { count++; val &= (val - 1); }
            return static_cast<hdl_integer>(count);
        }
        spdlog::warn("Encountered an invalid argument for a $countones call");
        return 0;
    }
    if (function_kind == function::sin) {
        if (resolved_arguments[0].is_real()) return std::sin(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::sin(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $sin call");
        return 0;
    }
    if (function_kind == function::cos) {
        if (resolved_arguments[0].is_real()) return std::cos(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::cos(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $cos call");
        return 0;
    }
    if (function_kind == function::tan) {
        if (resolved_arguments[0].is_real()) return std::tan(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::tan(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $tan call");
        return 0;
    }
    if (function_kind == function::sinh) {
        if (resolved_arguments[0].is_real()) return std::sinh(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::sinh(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $sinh call");
        return 0;
    }
    if (function_kind == function::cosh) {
        if (resolved_arguments[0].is_real()) return std::cosh(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::cosh(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $cosh call");
        return 0;
    }
    if (function_kind == function::tanh) {
        if (resolved_arguments[0].is_real()) return std::tanh(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::tanh(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $tanh call");
        return 0;
    }
    if (function_kind == function::asinh) {
        if (resolved_arguments[0].is_real()) return std::asinh(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::asinh(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $asinh call");
        return 0;
    }
    if (function_kind == function::acosh) {
        if (resolved_arguments[0].is_real()) return std::acosh(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::acosh(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $acosh call");
        return 0;
    }
    if (function_kind == function::atanh) {
        if (resolved_arguments[0].is_real()) return std::atanh(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::atanh(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $atanh call");
        return 0;
    }
    if (function_kind == function::exp) {
        if (resolved_arguments[0].is_real()) return std::exp(resolved_arguments[0].get_real());
        if (resolved_arguments[0].is_integer()) return std::exp(resolved_arguments[0].get_integer().to_double());
        spdlog::warn("Encountered an invalid argument for a $exp call");
        return 0;
    }
    if (function_kind == function::hypot) {
        if (resolved_arguments.size() < 2) { spdlog::warn("$hypot requires exactly 2 arguments"); return 0; }
        if (!(resolved_arguments[0].is_integer() || resolved_arguments[0].is_real()) ||
            !(resolved_arguments[1].is_integer() || resolved_arguments[1].is_real())) {
            spdlog::warn("Encountered an invalid argument for a $hypot call");
            return 0;
        }
        double a = resolved_arguments[0].is_real() ? resolved_arguments[0].get_real()
                    : resolved_arguments[0].get_integer().to_double();
        double b = resolved_arguments[1].is_real() ? resolved_arguments[1].get_real()
                    : resolved_arguments[1].get_integer().to_double();
        return std::hypot(a, b);
    }
    if (function_kind == function::onehot) {
        if (resolved_arguments[0].is_integer()) {
            wide_integer v = resolved_arguments[0].get_integer().to_wide();
            return static_cast<hdl_integer>(v != 0 && (v & (v - 1)) == 0);
        }
        spdlog::warn("Encountered an invalid argument for a $onehot call");
        return 0;
    }
    if (function_kind == function::onehot0) {
        if (resolved_arguments[0].is_integer()) {
            wide_integer v = resolved_arguments[0].get_integer().to_wide();
            return static_cast<hdl_integer>((v & (v - 1)) == 0);
        }
        spdlog::warn("Encountered an invalid argument for a $onehot0 call");
        return 0;
    }
    if (function_kind == function::isunknown) {
        return 0;
    }
    if (function_kind == function::isunbounded) {
        // No unbounded types are representable in this tool, so this always
        // resolves to false in constant evaluation.
        return 0;
    }
    if (function_kind == function::clog2) {
        if (resolved_arguments[0].is_real()) {
            return static_cast<hdl_integer>(std::ceil(std::log2(resolved_arguments[0].get_real())));
        } else if (resolved_arguments[0].is_integer()) {
            auto arg = resolved_arguments[0].get_integer().to_wide();
            if (arg <= 1) return static_cast<hdl_integer>(0);
            int bits = 0;
            auto tmp = arg - 1;
            while (tmp != 0) { tmp >>= 1; bits++; }
            return static_cast<hdl_integer>(bits);
        }
        spdlog::warn("Encountered an invalid argument for a $clog2 call");
    }

    auto to_double = [](const resolved_parameter &p) -> std::optional<double> {
        if (p.is_real()) return p.get_real();
        if (p.is_integer()) return p.get_integer().to_double();
        return std::nullopt;
    };

    if (function_kind == function::log2) {
        auto arg = to_double(resolved_arguments[0]);
        if (!arg) { spdlog::warn("Encountered an invalid argument for a $log2 call"); return 0; }
        if (*arg <= 0) { spdlog::warn("$log2 argument must be positive"); return 0; }
        return std::log2(*arg);
    }
    if (function_kind == function::asin) {
        auto arg = to_double(resolved_arguments[0]);
        if (!arg) { spdlog::warn("Encountered an invalid argument for a $asin call"); return 0; }
        if (*arg < -1.0 || *arg > 1.0) { spdlog::warn("$asin argument must be in the range [-1, 1]"); return 0; }
        return std::asin(*arg);
    }
    if (function_kind == function::acos) {
        auto arg = to_double(resolved_arguments[0]);
        if (!arg) { spdlog::warn("Encountered an invalid argument for a $acos call"); return 0; }
        if (*arg < -1.0 || *arg > 1.0) { spdlog::warn("$acos argument must be in the range [-1, 1]"); return 0; }
        return std::acos(*arg);
    }
    if (function_kind == function::atan) {
        auto arg = to_double(resolved_arguments[0]);
        if (!arg) { spdlog::warn("Encountered an invalid argument for a $atan call"); return 0; }
        return std::atan(*arg);
    }
    if (function_kind == function::atan2) {
        if (resolved_arguments.size() < 2) { spdlog::warn("$atan2 requires exactly 2 arguments"); return 0; }
        auto y = to_double(resolved_arguments[0]);
        auto x = to_double(resolved_arguments[1]);
        if (!y || !x) { spdlog::warn("Encountered an invalid argument for a $atan2 call"); return 0; }
        return std::atan2(*y, *x);
    }
    if (function_kind == function::round) {
        auto arg = to_double(resolved_arguments[0]);
        if (!arg) { spdlog::warn("Encountered an invalid argument for a $round call"); return 0; }
        return std::round(*arg);
    }
    if (function_kind == function::truncate) {
        auto arg = to_double(resolved_arguments[0]);
        if (!arg) { spdlog::warn("Encountered an invalid argument for a $truncate call"); return 0; }
        return std::trunc(*arg);
    }
    if (function_kind == function::countbits) {
        if (resolved_arguments.size() < 2) { spdlog::warn("$countbits requires exactly 2 arguments"); return 0; }
        if (!resolved_arguments[0].is_integer() || !resolved_arguments[1].is_integer()) {
            spdlog::warn("Encountered an invalid argument for a $countbits call");
            return 0;
        }
        wide_integer ctrl = resolved_arguments[1].get_integer().to_wide();
        int ctrl_width = 1;
        auto ctrl_type = arguments[1]->resolve_expression_type(context);
        if (ctrl_type) ctrl_width = std::max(1, static_cast<int>(packed_width(*ctrl_type)));
        else ctrl_width = std::max(1, static_cast<int>(resolved_arguments[1].get_integer().get_size()));
        bool count_ones = false, count_zeros = false;
        for (int i = 0; i < ctrl_width; i++) {
            if ((ctrl >> i) & 1) count_ones = true; else count_zeros = true;
        }
        wide_integer value = resolved_arguments[0].get_integer().to_wide();
        int expr_width = 64;
        auto expr_type = arguments[0]->resolve_expression_type(context);
        if (expr_type) expr_width = static_cast<int>(packed_width(*expr_type));
        else expr_width = std::max(1, static_cast<int>(resolved_arguments[0].get_integer().get_size()));
        int64_t count = 0;
        for (int64_t i = 0; i < expr_width && i < hdl_integer::MAX_BIT_WIDTH; i++) {
            bool bit = ((value >> i) & 1) != 0;
            if (bit && count_ones) count++;
            if (!bit && count_zeros) count++;
        }
        return static_cast<hdl_integer>(count);
    }
    if (function_kind == function::realtobits) {
        if (resolved_arguments[0].is_real()) {
            double d = resolved_arguments[0].get_real();
            uint64_t bits;
            std::memcpy(&bits, &d, sizeof(bits));
            return static_cast<hdl_integer>(bits);
        }
        spdlog::warn("Encountered an invalid argument for a $realtobits call");
        return 0;
    }
    if (function_kind == function::bitstoreal) {
        if (resolved_arguments[0].is_integer()) {
            uint64_t bits = static_cast<uint64_t>(resolved_arguments[0].get_integer().to_wide());
            double d;
            std::memcpy(&d, &bits, sizeof(d));
            return d;
        }
        spdlog::warn("Encountered an invalid argument for a $bitstoreal call");
        return 0;
    }
    if (function_kind == function::shortrealtobits) {
        if (resolved_arguments[0].is_real()) {
            float f = static_cast<float>(resolved_arguments[0].get_real());
            uint32_t bits;
            std::memcpy(&bits, &f, sizeof(bits));
            return static_cast<hdl_integer>(bits);
        }
        spdlog::warn("Encountered an invalid argument for a $shortrealtobits call");
        return 0;
    }
    if (function_kind == function::bitstoshortreal) {
        if (resolved_arguments[0].is_integer()) {
            uint32_t bits = static_cast<uint32_t>(resolved_arguments[0].get_integer().to_wide());
            float f;
            std::memcpy(&f, &bits, sizeof(f));
            return static_cast<double>(f);
        }
        spdlog::warn("Encountered an invalid argument for a $bitstoshortreal call");
        return 0;
    }
    if (function_kind == function::random || function_kind == function::urandom || function_kind == function::urandom_range ||
        function_kind == function::dist_uniform || function_kind == function::dist_normal ||
        function_kind == function::dist_exponential || function_kind == function::dist_poisson ||
        function_kind == function::dist_chi_square || function_kind == function::dist_t ||
        function_kind == function::dist_erlang) {
        spdlog::warn("{} is not supported in constant expressions, defaulting to 0", sv_name(function_kind));
        return 0;
    }
    if (function_kind == function::len) {
        if (resolved_arguments[0].is_string()) {
            return static_cast<hdl_integer>(unquote(resolved_arguments[0].get_string()).size());
        }
        spdlog::warn("Encountered an invalid argument for a $len call");
        return 0;
    }
    if (function_kind == function::to_lower) {
        if (resolved_arguments[0].is_string()) {
            auto s = unquote(resolved_arguments[0].get_string());
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::tolower(c); });
            return s;
        }
        spdlog::warn("Encountered an invalid argument for a $to_lower call");
        return std::string("");
    }
    if (function_kind == function::to_upper) {
        if (resolved_arguments[0].is_string()) {
            auto s = unquote(resolved_arguments[0].get_string());
            std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c){ return std::toupper(c); });
            return s;
        }
        spdlog::warn("Encountered an invalid argument for a $to_upper call");
        return std::string("");
    }
    if (function_kind == function::atoi) {
        if (resolved_arguments[0].is_string()) {
            const auto s = unquote(resolved_arguments[0].get_string());
            size_t i = 0;
            bool neg = false;
            if (i < s.size() && (s[i] == '+' || s[i] == '-')) { neg = (s[i] == '-'); i++; }
            int64_t val = 0;
            bool any = false;
            while (i < s.size() && std::isdigit(static_cast<unsigned char>(s[i]))) {
                val = val * 10 + (s[i] - '0');
                any = true;
                i++;
            }
            if (!any) return static_cast<hdl_integer>(0);
            return static_cast<hdl_integer>(neg ? -val : val);
        }
        spdlog::warn("Encountered an invalid argument for a $atoi call");
        return 0;
    }
    if (function_kind == function::compare) {
        if (resolved_arguments.size() >= 2 && resolved_arguments[0].is_string() && resolved_arguments[1].is_string()) {
            int cmp = unquote(resolved_arguments[0].get_string()).compare(unquote(resolved_arguments[1].get_string()));
            return static_cast<hdl_integer>(cmp < 0 ? -1 : (cmp > 0 ? 1 : 0));
        }
        spdlog::warn("Encountered an invalid argument for a $compare call");
        return 0;
    }
    if (function_kind == function::substr) {
        if (resolved_arguments.size() < 3) { spdlog::warn("$substr requires exactly 3 arguments"); return std::string(""); }
        if (!resolved_arguments[0].is_string() || !resolved_arguments[1].is_integer() || !resolved_arguments[2].is_integer()) {
            spdlog::warn("Encountered an invalid argument for a $substr call");
            return std::string("");
        }
        const auto s = unquote(resolved_arguments[0].get_string());
        int64_t start = resolved_arguments[1].get_integer().get_value();
        int64_t end = resolved_arguments[2].get_integer().get_value();
        if (start < 0) start = 0;
        if (end >= static_cast<int64_t>(s.size())) end = static_cast<int64_t>(s.size()) - 1;
        if (start > end) return std::string("");
        return s.substr(static_cast<size_t>(start), static_cast<size_t>(end - start + 1));
    }
    if (function_kind == function::sformatf) {
        if (resolved_arguments[0].is_string()) {
            std::vector<resolved_parameter> fmt_args(resolved_arguments.begin() + 1, resolved_arguments.end());
            return format_string(unquote(resolved_arguments[0].get_string()), fmt_args);
        }
        spdlog::warn("Encountered an invalid argument for a $sformatf call");
        return std::string("");
    }
    spdlog::warn("Unsupported system task {} encountered while parsing a parameter", sv_name(function_kind));
    return 0;
}

namespace {

struct type_dimension {
    uint64_t size;
    int64_t left;
    int64_t right;
};

std::optional<resolved_type> resolve_argument_type(
    const std::shared_ptr<Expression_base> &arg,
    const std::map<qualified_identifier, resolved_parameter> &context
) {
    if (!arg) return std::nullopt;
    auto type = arg->resolve_expression_type(context);
    if (type) return type;

    auto val = arg->evaluate(context);
    if (!val) return std::nullopt;
    if (val->is_real()) {
        resolved_type result;
        result.is_real = true;
        return result;
    }
    if (val->is_integer()) {
        resolved_type result;
        auto width = val->get_integer().get_size();
        if (width == 0) width = 32;
        result.packed_sizes.push_back(width);
        result.packed_ascending.push_back(false);
        result.packed_left.push_back(static_cast<int64_t>(width) - 1);
        result.packed_right.push_back(0);
        return result;
    }
    return std::nullopt;
}

std::vector<type_dimension> collect_dimensions(const resolved_type &t) {
    std::vector<type_dimension> dims;
    for (size_t i = 0; i < t.unpacked_sizes.size(); i++) {
        bool asc = i < t.unpacked_ascending.size() ? t.unpacked_ascending[i] : false;
        int64_t left = i < t.unpacked_left.size()
            ? t.unpacked_left[i]
            : (asc ? 0 : static_cast<int64_t>(t.unpacked_sizes[i]) - 1);
        int64_t right = i < t.unpacked_right.size()
            ? t.unpacked_right[i]
            : (asc ? static_cast<int64_t>(t.unpacked_sizes[i]) - 1 : 0);
        dims.push_back({t.unpacked_sizes[i], left, right});
    }
    for (size_t i = 0; i < t.packed_sizes.size(); i++) {
        bool asc = i < t.packed_ascending.size() ? t.packed_ascending[i] : false;
        int64_t left = i < t.packed_left.size()
            ? t.packed_left[i]
            : (asc ? 0 : static_cast<int64_t>(t.packed_sizes[i]) - 1);
        int64_t right = i < t.packed_right.size()
            ? t.packed_right[i]
            : (asc ? static_cast<int64_t>(t.packed_sizes[i]) - 1 : 0);
        dims.push_back({t.packed_sizes[i], left, right});
    }
    return dims;
}

} // namespace

std::expected<resolved_parameter, solver_errors> HDL_builtin_function::evaluate_type_query(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    std::string task_name = std::string(sv_name(function_kind));
    if (arguments.empty()) {
        spdlog::warn("{} requires at least one argument", task_name);
        return std::unexpected{missing_arguments};
    }

    int dim = 1;
    if (arguments.size() >= 2) {
        auto dim_val = arguments[1]->evaluate(context);
        if (dim_val && dim_val->is_integer()) dim = dim_val->get_integer().get_value();
    }

    auto type = resolve_argument_type(arguments[0], context);

    if (function_kind == function::bits) {
        if (!type) {
            spdlog::warn("$bits argument type could not be resolved, defaulting to 0");
            return 0;
        }
        if (type->is_real) return static_cast<hdl_integer>(64);
        uint64_t bits = 1;
        bool any = false;
        for (auto s : type->packed_sizes) { bits *= s; any = true; }
        for (auto s : type->unpacked_sizes) { bits *= s; any = true; }
        if (!any) {
            spdlog::warn("$bits argument is of unsupported type, defaulting to 0");
            return 0;
        }
        return static_cast<hdl_integer>(bits);
    }

    if (function_kind == function::dimensions || function_kind == function::unpacked_dimensions) {
        if (!type) {
            spdlog::warn("{} argument type could not be resolved, defaulting to 0", task_name);
            return 0;
        }
        if (type->is_real) return static_cast<hdl_integer>(0);
        if (function_kind == function::dimensions)
            return static_cast<hdl_integer>(type->packed_sizes.size() + type->unpacked_sizes.size());
        return static_cast<hdl_integer>(type->unpacked_sizes.size());
    }

    if (!type) {
        spdlog::warn("{} argument type could not be resolved, defaulting to 0", task_name);
        return 0;
    }
    if (type->is_real) {
        spdlog::warn("{} is not defined for real arguments, defaulting to 0", task_name);
        return 0;
    }
    auto dims = collect_dimensions(*type);
    if (dim < 1 || static_cast<size_t>(dim) > dims.size()) {
        spdlog::warn("{} dimension {} is out of range, defaulting to 0", task_name, dim);
        return 0;

    }
    const auto &d = dims[dim - 1];
    if (function_kind == function::size) return static_cast<hdl_integer>(d.size);
    if (function_kind == function::left) return static_cast<hdl_integer>(d.left);
    if (function_kind == function::right) return static_cast<hdl_integer>(d.right);
    if (function_kind == function::high) return static_cast<hdl_integer>(std::max(d.left, d.right));
    return static_cast<hdl_integer>(std::min(d.left, d.right));
}

std::expected<resolved_parameter, solver_errors> HDL_builtin_function::evaluate_typename(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    if (arguments.empty()) {
        spdlog::warn("$typename requires at least one argument");
        return resolved_parameter("");
    }
    auto &arg = arguments[0];
    if (arg->is<Identifier_token>()) {
        auto t = arg->as<Identifier_token>().get_expression_type();
        if (t) {
            if (t->is<HDL_simple_type>())
                return resolved_parameter(t->as<HDL_simple_type>().get_type_name());
            if (t->is<HDL_struct_type>()) return resolved_parameter("struct");
            if (t->is<HDL_union_type>()) return resolved_parameter("union");
            if (t->is<HDL_enum_type>()) return resolved_parameter("enum");
            if (t->is<HDL_external_type>()) return resolved_parameter(t->as<HDL_external_type>().to_print());
        }
    }
    auto type = resolve_argument_type(arg, context);
    if (type) {
        if (type->is_real) return resolved_parameter("real");
        if (!type->packed_sizes.empty() || !type->unpacked_sizes.empty()) return resolved_parameter("logic");
    }
    auto val = arg->evaluate(context);
    if (val) {
        if (val->is_real()) return resolved_parameter("real");
        if (val->is_string()) return resolved_parameter("string");
        if (val->is_integer()) return resolved_parameter("integer");
    }
    spdlog::warn("$typename argument type could not be determined, defaulting to empty");
    return resolved_parameter("");
}

std::expected<resolved_parameter, solver_errors> HDL_builtin_function::evaluate_signedness(
    const std::map<qualified_identifier, resolved_parameter> &context) {
    std::string task_name = std::string(sv_name(function_kind));
    if (arguments.empty()) {
        spdlog::warn("{} requires at least one argument", task_name);
        return 0;
    }
    auto &arg = arguments[0];
    auto val = arg->evaluate(context);
    if (!val || !val->is_integer()) {
        spdlog::warn("Encountered an invalid argument for a {} call", task_name);
        return 0;
    }

    uint64_t container = 32;
    auto type = arg->resolve_expression_type(context);
    if (type) {
        uint64_t width = packed_width(*type);
        if (width > 0) container = width;
    } else if (arg->is<Numeric_token>()) {
        auto &num = arg->as<Numeric_token>();
        if (num.is_sized_explicit() && num.get_size() > 0) container = static_cast<uint64_t>(num.get_size());
    }

    if (function_kind == function::signed_fn) return type_cast_engine::to_signed(val->get_integer(), container);
    return type_cast_engine::to_unsigned(val->get_integer(), container);
}
std::optional<resolved_type> HDL_builtin_function::resolve_expression_type(
    const std::map<qualified_identifier, resolved_parameter> &context, [[maybe_unused]] const std::optional<resolved_type> &expected_type) const {
    if (function_kind == function::signed_fn || function_kind == function::unsigned_fn) {
        if (!arguments.empty()) return arguments[0]->resolve_expression_type(context);
    }
    if (function_kind == function::bits && !arguments.empty()) {
        return resolve_argument_type(arguments[0], context);
    }
    return std::nullopt;
}

std::string HDL_builtin_function::print() const {
    return std::string(sv_name(function_kind));
}

parameter_deps_t HDL_builtin_function::get_dependencies() const {
    parameter_deps_t retval;
    for (const auto &arg : arguments) {
        retval.merge(arg->get_dependencies());
    }
    return retval;
}

bool HDL_builtin_function::isEqual(const Expression_base &other) const {
    if (auto *o = dynamic_cast<const HDL_builtin_function*>(&other)) {
        if (function_kind != o->function_kind) return false;
        if (arguments.size() != o->arguments.size()) return false;
        for (size_t i = 0; i < arguments.size(); i++) {
            if (!(*arguments[i] == *o->arguments[i])) return false;
        }
        return true;
    }
    return false;
}
