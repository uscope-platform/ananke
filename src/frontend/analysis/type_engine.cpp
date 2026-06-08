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

void Type_engine::start_composite_type_declaration(type_kind k) {
    kind = k;
    if (k==struct_type) {
        current_struct = {};
    }
}

void Type_engine::open_composite_member() {
    r_factory.start();
    current_struct.member.emplace_back();
}

void Type_engine::close_composite_member(const std::string &name) {
    r_factory.stop();
    auto  [packed, unpacked] = r_factory.get_dimensions();
    r_factory.clear();
    current_struct.member.back().name = name;
    current_struct.member.back().type.set_packed_dimensions(packed);
    current_struct.member.back().type.set_unpacked_dimensions(unpacked);
}

std::shared_ptr<hdl_type> Type_engine::stop_composite_type_declaration() {
    kind = simple_type;
    return std::make_shared<HDL_struct>(current_struct);
}

void Type_engine::set_type(const std::string &type) {
    if (kind != simple_type) {
        current_struct.member.back().type = create_primitive_type(type);
    }
}

void Type_engine::set_packed() {
    if (kind != simple_type) {
        current_struct.packed = true;
    }
}

void Type_engine::start_simple_type_declaration() {
    r_factory.start();
}

std::shared_ptr<hdl_type> Type_engine::stop_type_declaration(const std::string &name) {
    r_factory.stop();
    HDL_simple_type t;
    auto  [packed, unpacked] = r_factory.get_dimensions();
    t.set_packed_dimensions(packed);
    t.set_unpacked_dimensions(unpacked);
    r_factory.clear();
    type_registry[name] = t;
    return std::make_shared<HDL_simple_type>(t);
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
    if (kind == simple_type) {
        return r_factory.active();
    }
    else return true;
}

bool Type_engine::has_type(const std::string &name) const {
    return type_registry.contains(name);
}

HDL_simple_type Type_engine::get_type(const std::string &name) const {
    return type_registry.at(name);
}

HDL_simple_type Type_engine::create_primitive_type(const std::string &type_name) {
    HDL_simple_type t;
    if (type_name == "implicit") {
        t.set_implicit(true);
        t.set_packed_dimensions({
            {
                Expression(Expression_component("31", Expression_component::number)),
                Expression(Expression_component("0", Expression_component::number)),
                true
            }
        });
    }
    if (type_name == "shortint") {
        t.set_signed(true);
        t.set_packed_dimensions({
            {
                Expression(Expression_component("15", Expression_component::number)),
                Expression(Expression_component("0", Expression_component::number)),
                true
            }
        });
    }
    if (type_name == "int" || type_name == "integer") {
        t.set_signed(true);
        t.set_packed_dimensions({
            {
                Expression(Expression_component("31", Expression_component::number)),
                Expression(Expression_component("0", Expression_component::number)),
                true
            }
        });
    }
    if (type_name == "longint") {
        t.set_signed(true);
        t.set_packed_dimensions({
            {
                Expression({Expression_component("63", Expression_component::number)}),
                Expression(Expression_component("0", Expression_component::number)),
                true
            }
        });
    }
    if (type_name == "time") {
        t.set_signed(false);
        t.set_packed_dimensions({
            {
                Expression(Expression_component("63", Expression_component::number)),
                Expression(Expression_component("0", Expression_component::number)),
                true
            }
        });
    }
    if (type_name == "real" || type_name == "realtime") {
        t.set_real(true);
        t.set_packed_dimensions({
            {
                Expression(Expression_component("63", Expression_component::number)),
                Expression(Expression_component("0", Expression_component::number)),
                true
            }
        });
    }
    if (type_name == "shortreal") {
        t.set_real(true);
        t.set_packed_dimensions({
            {
                Expression(Expression_component("31", Expression_component::number)),
                Expression(Expression_component("0", Expression_component::number)),
                true
            }
        });
    }
    return t;
}

HDL_simple_type Type_engine::resolve_type(const std::string &type_name) {
    if (has_type(type_name)) {
        return get_type(type_name);
    }
    return create_primitive_type(type_name);
}
