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

#ifndef ANANKE_CALL_ARGUMENT_FRAME_HPP
#define ANANKE_CALL_ARGUMENT_FRAME_HPP

#include "data_model/HDL/factories/parameters/factory_base.hpp"
#include "data_model/HDL/factories/parameters/expressions_factory.hpp"

// Marker pushed while a single call argument (a direct `expression` child of
// a `list_of_arguments`) is parsed. Completions arriving with this frame on
// top are interior operands of the argument expression, never separate call
// arguments, so they accumulate into the argument expression instead of
// being claimed by the call. The argument's own exit finalizes exactly one
// value into the call (see start/stop_call_argument); without this, e.g.
// `$clog2({a,b} ^ {c,d})` split into one argument per concatenation.
class call_argument_frame : public factory_base {
public:
    explicit call_argument_frame(expressions_factory &ef) : expr(ef) {}
    void consume(const std::shared_ptr<Expression_base> &v) override {
        if (v) expr.consume(v);
    }
    [[nodiscard]] bool active() const override { return true; }
    std::shared_ptr<Expression_base> result() override { return nullptr; }

private:
    expressions_factory &expr;
};

#endif //ANANKE_CALL_ARGUMENT_FRAME_HPP
