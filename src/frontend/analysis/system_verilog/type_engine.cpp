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

#include "frontend/analysis/system_verilog/type_engine.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"

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
    if (current_struct.member.back().type) {
        auto &old = current_struct.member.back().type->as<HDL_simple_type>();
        HDL_simple_type t;
        t.set_signed(old.get_signed());
        t.set_implicit(old.get_implicit());
        t.set_real(old.get_real());
        t.set_packed_dimensions(packed);
        t.set_unpacked_dimensions(unpacked);
        if (packed.empty()) {
            t.set_packed_dimensions(old.get_packed_dimensions());
        }
        if (unpacked.empty()) {
            t.set_unpacked_dimensions(old.get_unpacked_dimensions());
        }
        current_struct.member.back().type = std::make_shared<HDL_simple_type>(t);
    } else {
        HDL_simple_type t;
        t.set_packed_dimensions(packed);
        t.set_unpacked_dimensions(unpacked);
        current_struct.member.back().type = std::make_shared<HDL_simple_type>(t);
    }
}

void Type_engine::set_member_signed(bool s) {
    if (kind != simple_type && !current_struct.member.empty() && current_struct.member.back().type) {
        auto &t = current_struct.member.back().type->as<HDL_simple_type>();
        t.set_signed(s);
    }
}

void Type_engine::set_operation(Expression_v2::expression_operator op) {
    expr_factory.set_operation(op);
}

std::shared_ptr<hdl_type> Type_engine::stop_composite_type_declaration(const std::string &name, bool anonymous) {
    kind = simple_type;
    auto result = std::make_shared<HDL_struct_type>(current_struct);
    if (!anonymous) type_registry[name] = result;
    return result;
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
    type_registry[name] = std::make_shared<HDL_simple_type>(t);
    return type_registry.at(name);
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
    expr_factory.start_expression(false);
}

void Type_engine::add_component(const std::shared_ptr<Parameter_value_base> &c) {
    expr_factory.add_component(c);
}

void Type_engine::stop_expression() {
    expr_factory.stop_expression(false);
    if (expr_factory.get_level() == 0) {
        auto expr = expr_factory.get_expression_v2();
        if (expr.has_value()) {
            if (expr->get_operation() != Expression_v2::none) {
                r_factory.add_expression(Expression_v2::unwrap(expr.value()));
            } else if (auto lhs = expr->get_lhs()) {
                r_factory.add_expression(lhs);
            } else {
                r_factory.add_expression(Expression_v2::unwrap(expr.value()));
            }
        }
        expr_factory.clear_expression();
    }
}

void Type_engine::set_base_type(const std::shared_ptr<hdl_type>  &t) {
    current_type = t;
}

std::shared_ptr<hdl_type> Type_engine::finalize_type() {
    if (!current_type) return nullptr;
    if (current_type->is<HDL_simple_type>()) {
        auto result = current_type->as<HDL_simple_type>();
        auto [packed, unpacked] = r_factory.get_dimensions();
        result.set_packed_dimensions(packed);
        result.set_unpacked_dimensions(unpacked);
        r_factory.clear();
        return std::make_shared<HDL_simple_type>(result);
    }
    if (current_type->is<HDL_external_type>()) {
        auto &result = current_type->as<HDL_external_type>();
        r_factory.clear();
        return std::make_shared<HDL_external_type>(result);
    }
    r_factory.clear();
    return current_type;

}

std::shared_ptr<hdl_type> Type_engine::finalize_dimensions() {
    auto [packed, unpacked] = r_factory.get_dimensions();
    HDL_simple_type result;
    result.set_packed_dimensions(packed);
    result.set_unpacked_dimensions(unpacked);
    r_factory.clear();
    return std::make_shared<HDL_simple_type>(result);
}

bool Type_engine::has_type(const std::string &name) const {
    return type_registry.contains(name);
}

std::shared_ptr<hdl_type> Type_engine::get_type(const std::string &name) const {
    return type_registry.at(name);
}

std::shared_ptr<hdl_type> Type_engine::create_primitive_type(const std::string &type_name) {
    HDL_simple_type t;
    if (type_name == "implicit") {
        t.set_implicit(true);
        t.set_packed_dimensions({
            {
                std::make_shared<Numeric_token>("31"),
                std::make_shared<Numeric_token>("0"),
                true
            }
        });
    }
    if (type_name == "shortint") {
        t.set_signed(true);
        t.set_packed_dimensions({
            {
                std::make_shared<Numeric_token>("15"),
                std::make_shared<Numeric_token>("0"),
                true
            }
        });
    }
    if (type_name == "int" || type_name == "integer") {
        t.set_signed(true);
        t.set_packed_dimensions({
            {
                std::make_shared<Numeric_token>("31"),
                std::make_shared<Numeric_token>("0"),
                true
            }
        });
    }
    if (type_name == "longint") {
        t.set_signed(true);
        t.set_packed_dimensions({
            {
                std::make_shared<Numeric_token>("63"),
                std::make_shared<Numeric_token>("0"),
                true
            }
        });
    }
    if (type_name == "time") {
        t.set_signed(false);
        t.set_packed_dimensions({
            {
                std::make_shared<Numeric_token>("63"),
                std::make_shared<Numeric_token>("0"),
                true
            }
        });
    }
    if (type_name == "real" || type_name == "realtime") {
        t.set_real(true);
        t.set_packed_dimensions({
            {
                std::make_shared<Numeric_token>("63"),
                std::make_shared<Numeric_token>("0"),
                true
            }
        });
    }
    if (type_name == "shortreal") {
        t.set_real(true);
        t.set_packed_dimensions({
            {
                std::make_shared<Numeric_token>("31"),
                std::make_shared<Numeric_token>("0"),
                true
            }
        });
    }
    return std::make_shared<HDL_simple_type>(t);
}

std::shared_ptr<hdl_type> Type_engine::resolve_type(const std::string &type_name) {
    if (has_type(type_name)) {
        return get_type(type_name);
    }
    return create_primitive_type(type_name);
}
