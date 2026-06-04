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

#include "frontend/analysis/type_engine.hpp"

void Type_engine::start_type_declaration() {
    r_factory.start();
}

HDL_type Type_engine::stop_type_declaration(const std::string &name) {
    r_factory.stop();
    HDL_type t;
    auto [packed, unpacked] = r_factory.get_dimensions();
    t.set_packed_dimensions(packed);
    t.set_unpacked_dimensions(unpacked);
    r_factory.clear();
    type_registry[name] = t;
    return t;
}

void Type_engine::close_packed_dimensions() {
    r_factory.advance_stage();
}

void Type_engine::start_unpacked_dimension_declaration() {
    r_factory.advance_stage();
}

void Type_engine::start_range() {
    r_factory.start();
}

void Type_engine::stop_range() {
    r_factory.stop();
}

void Type_engine::open_range() {
    r_factory.open_range();
}

void Type_engine::close_range() {
    r_factory.close_range();
}

void Type_engine::advance_range() {
    r_factory.advance_range();
}

void Type_engine::start_expression() {
    expr_factory.start_expression();
}

void Type_engine::add_component(const Expression_component &c) {
    expr_factory.add_component(c);
}

void Type_engine::stop_expression() {
    expr_factory.stop_expression();
    if (expr_factory.get_level() == 0) {
        auto expr = expr_factory.get_expression();
        if (expr.has_value()) {
            r_factory.add_expression(expr.value());
        }
        expr_factory.clear_expression();
    }
}

bool Type_engine::active() const {
    return r_factory.active();
}

bool Type_engine::has_type(const std::string &name) const {
    return type_registry.contains(name);
}

HDL_type Type_engine::get_type(const std::string &name) const {
    return type_registry.at(name);
}
