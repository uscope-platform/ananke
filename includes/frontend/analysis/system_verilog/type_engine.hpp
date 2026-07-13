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


#ifndef ANANKE_TYPE_ENGINE_HPP
#define ANANKE_TYPE_ENGINE_HPP

#include <string>
#include <map>

#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/factories/parameters/ranges_factory.hpp"
#include "data_model/HDL/factories/parameters/expressions_factory.hpp"

class Type_engine {
public:
    Type_engine() = default;

    enum type_kind{
        simple_type,
        struct_type,
        union_type,
        enum_type
    };

    void start_composite_type_declaration(type_kind kind);
    void open_composite_member();
    void close_composite_member(const std::string &name);
    std::shared_ptr<hdl_type> stop_composite_type_declaration(const std::string &name, bool anonymous);

    void start_simple_type_declaration();
    std::shared_ptr<hdl_type> stop_type_declaration(const std::string &name);

    void close_packed_dimensions();
    void start_unpacked_dimension_declaration();

    void start_range();
    void stop_range();
    void open_range();
    void close_range();
    void advance_range();

    void start_expression();
    void add_component(const std::shared_ptr<Expression_base> &c);
    void stop_expression();

    [[nodiscard]] bool active() const { return kind != simple_type; }
    [[nodiscard]] bool is_ranging() const { return r_factory.active(); }
    [[nodiscard]] bool has_type(const std::string &name) const;
    [[nodiscard]] std::shared_ptr<hdl_type> get_type(const std::string &name) const;
    [[nodiscard]] bool is_simple_type()const{ return kind == simple_type;}

    void set_base_type(const std::shared_ptr<hdl_type>  &t);
    std::shared_ptr<hdl_type> finalize_type();
    std::shared_ptr<hdl_type> finalize_dimensions();

    void set_type(const std::string & string);
    void set_packed();
    void set_member_signed(bool s);

    void set_operation(Expression_v2::expression_operator subtract);

    static std::shared_ptr<hdl_type> create_primitive_type(const std::string &type_name);
    std::shared_ptr<hdl_type> resolve_type(const std::string &type_name);

private:
    expressions_factory expr_factory;
    ranges_factory r_factory;
    std::map<std::string, std::shared_ptr<hdl_type>> type_registry;
    type_kind kind = simple_type;
    HDL_struct_type current_struct;
    std::shared_ptr<hdl_type> current_type;

};

#endif //ANANKE_TYPE_ENGINE_HPP
