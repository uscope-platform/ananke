//  Copyright 2023 Filippo Savi
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

#ifndef ANANKE_HDL_PARAMETER_HPP
#define ANANKE_HDL_PARAMETER_HPP

#include <string>
#include <regex>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include "components/Expression.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "common/dimension.hpp"

class HDL_function_def;

class HDL_parameter {
public:
    HDL_parameter() = default;
    HDL_parameter( const HDL_parameter &c );
    explicit HDL_parameter(const std::string &n) {name = n;}

    HDL_parameter(HDL_parameter &&other) noexcept
        : name(std::move(other.name)),
          type(other.type),
          raw_value(std::move(other.raw_value)),
          solved_value(std::move(other.solved_value)){
    }

    HDL_parameter & operator=(const HDL_parameter &other) {
        if (this == &other)
            return *this;
        name = other.name;
        type = other.type;
        raw_value = other.raw_value;
        solved_value = other.solved_value;
        return *this;
    }

    HDL_parameter & operator=(HDL_parameter &&other) noexcept {
        if (this == &other)
            return *this;
        name = std::move(other.name);
        type = std::move(other.type);
        raw_value = std::move(other.raw_value);
        solved_value = std::move(other.solved_value);
        return *this;
    }

    void set_name(const std::string &n) {name = n;}
    std::shared_ptr<HDL_parameter> clone() const;

    void set_value(const resolved_parameter &val){solved_value = val;}

    std::string get_string_value() const {
        if (!solved_value.has_value()) return "";
        return solved_value.value().get_string();
    }

    [[nodiscard]] std::optional<hdl_integer>  get_numeric_value() const {
        if (!solved_value.has_value()) return 0;
        return solved_value.value().get_integer();
    }

    [[nodiscard]] std::optional<mdarray<hdl_integer>> get_int_array_value() const{
        if (!solved_value.has_value()) return {};
        return solved_value.value().get_int_array();
    }

    void set_declared_type(const std::string & type);

    [[nodiscard]] std::optional<resolved_parameter> get_value() const {return solved_value;}

    void add_dimension(const dimension_t &d);

    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context);

    void propagate_function(const HDL_function_def &def);
    explicit operator std::string();

    bool is_array() const{return !type.is_scalar();}
    bool is_packed_array() const {return type.is_packed_array();}

    std::string get_name() const {return name;}
    qualified_identifier get_identifier(){return {"", "", name};}

    HDL_simple_type get_type()const {return type;}
    void set_type(const HDL_simple_type &t){type = t;}
    void set_packed_dimensions(const std::vector<dimension_t>  &d) {type.set_packed_dimensions(d);}
    void set_unpacked_dimensions(const std::vector<dimension_t>  &d) {type.set_unpacked_dimensions(d);}
    std::vector<dimension_t> get_packed_dimensions(){return type.get_packed_dimensions();}
    std::vector<dimension_t> get_unpacked_dimensions(){return type.get_unpacked_dimensions();}


    void add_component(const Expression_component &component);
    void set_scalar(const std::shared_ptr<Parameter_value_base>  &e);
    std::shared_ptr<Parameter_value_base> get_expression() {
        if (type.is_scalar()) return raw_value;
        throw std::runtime_error("A scalar parameter has been initialized with an array");
    }
    void clear_expression() {
        if (type.is_scalar()) raw_value = std::make_shared<Expression>();
    }

    void add_item(const std::shared_ptr<Parameter_value_base> &e);

    std::string to_string() const;

    friend bool operator <(const HDL_parameter& lhs, const HDL_parameter& rhs);
    friend bool operator==(const HDL_parameter&lhs, const HDL_parameter&rhs);

    friend void PrintTo(const HDL_parameter& point, std::ostream* os);

    std::set<qualified_identifier> get_dependencies();

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, raw_value, type);
    }

    nlohmann::json dump();



private:

    std::string name;

    HDL_simple_type type;

    std::shared_ptr<Parameter_value_base> raw_value;
    std::optional<resolved_parameter> solved_value;

};





#endif //ANANKE_HDL_PARAMETER_HPP
