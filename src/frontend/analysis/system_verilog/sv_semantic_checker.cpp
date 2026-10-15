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

#include "frontend/analysis/system_verilog/sv_semantic_checker.hpp"

#include <spdlog/spdlog.h>

bool sv_semantic_checker::check_class_method(
    const std::string &file, unsigned int line, const std::string &method_name) {
    if (opts_.disable_semantic_checks) return false;
    static const std::set<std::string> builtin_class_methods = {
        "randomize", "rand_mode", "constraint_mode"};
    if (builtin_class_methods.contains(method_name)) {
        semantic_violation violation{
            semantic_error_code::builtin_method_override,
            "method " + method_name + "() is built-in and cannot be overridden"};
        spdlog::error("{}:{}: error: {}", file, line, violation.message);
        violations_.push_back(std::move(violation));
        return true;
    }
    return false;
}

int sv_semantic_checker::exit_code_for(semantic_error_code code) {
    switch (code) {
    case semantic_error_code::builtin_method_override:
        return 56;
    }
    return 56;
}
