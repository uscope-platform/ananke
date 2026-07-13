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


#include "data_model/HDL/factories/parameters/cast_factory.hpp"
#include "data_model/HDL/parameters/components/Expression_v2.hpp"

void cast_factory::start() {
    new_cast = Cast();
    state = build_phase::size;
}

void cast_factory::start(bool is_expr_size) {
    new_cast = Cast();
    state = build_phase::size;
    expression_size = is_expr_size;
}


void cast_factory::set_type(const std::string &t) {
    new_cast.set_type_cast();
    new_cast.set_target_type(t);
    state = build_phase::content;
}


void cast_factory::consume(const std::shared_ptr<Expression_base> &c) {
    if (state == build_phase::size) {
        auto size_val = c;
        if (c->is<Expression_v2>()) {
            auto &expr = c->as<Expression_v2>();
            if (expr.get_operation() == Expression_v2::none) {
                if (auto lhs = expr.get_lhs(); lhs && !lhs->is<Expression_v2>()) {
                    new_cast.set_size(lhs);
                }
            } else {
                new_cast.set_size(size_val);
            }
        } else {
            new_cast.set_size(c);
        }
    } else {
        new_cast.set_content(c);
    }
}

bool cast_factory::active() const {
    return state != build_phase::inactive;
}

std::shared_ptr<Expression_base> cast_factory::result() {
    auto cast = new_cast;
    state = build_phase::inactive;
    return std::make_shared<Cast>(cast);
}
