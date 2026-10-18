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

#ifndef ANANKE_HDL_SIGNAL_DECLARATION_HPP
#define ANANKE_HDL_SIGNAL_DECLARATION_HPP

#include <cstdint>
#include <memory>
#include <string>

#include "data_model/HDL/statement/hdl_statement_base.hpp"
#include "data_model/HDL/parameters/components/Expression_base.hpp"
#include "data_model/HDL/types/hdl_type.hpp"

// Storage semantics of a declared signal. SystemVerilog nets (wire, tri, ...)
// and variables (reg, logic, bit, ...) differ in how they may be driven;
// VHDL signals are kept apart so the language of origin is not lost.
enum class signal_storage_t : uint8_t {
    net,
    variable,
    vhdl_signal
};

// Net type keyword for storage == net; none for everything else.
enum class net_type_t : uint8_t {
    none,
    wire,
    tri,
    wand,
    wor,
    triand,
    trior,
    tri0,
    tri1,
    trireg,
    uwire,
    supply0,
    supply1
};

// A module-scope declaration of a single wire/reg/logic/signal. Multi-name
// declarations (`wire a, b;`) are stored as one statement per name.
// Width and dimensions live in the type (HDL_simple_type packed/unpacked
// dimensions), so widths resolve through the existing evaluate_type path.
class hdl_signal_declaration : public hdl_statement_base {
public:
    hdl_signal_declaration() = default;
    explicit hdl_signal_declaration(const std::string &n) : name(n) {}

    parameter_deps_t get_dependencies() const override;
    bool equals(const hdl_statement_base& other) const override;
    std::string print() const override;

    void set_name(const std::string &n) { name = n; }
    [[nodiscard]] const std::string &get_name() const { return name; }

    void set_storage(signal_storage_t s) { storage = s; }
    [[nodiscard]] signal_storage_t get_storage() const { return storage; }

    void set_net_type(net_type_t t) { net_type = t; }
    [[nodiscard]] net_type_t get_net_type() const { return net_type; }

    void set_type(const std::shared_ptr<hdl_type> &t) { type = t; }
    [[nodiscard]] std::shared_ptr<hdl_type> get_type() const { return type; }

    void set_initializer(const std::shared_ptr<Expression_base> &e) { initializer = e; }
    [[nodiscard]] std::shared_ptr<Expression_base> get_initializer() const { return initializer; }

    void set_line_n(int l) { line_n = l; }
    [[nodiscard]] int get_line_n() const { return line_n; }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, storage, net_type, type, initializer, line_n);
    }
private:
    std::string name;
    signal_storage_t storage = signal_storage_t::net;
    net_type_t net_type = net_type_t::none;
    std::shared_ptr<hdl_type> type;
    std::shared_ptr<Expression_base> initializer;
    int line_n = 0;
};

#endif //ANANKE_HDL_SIGNAL_DECLARATION_HPP
