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

#include "data_model/HDL/parameters/Expression.hpp"
#include "data_model/HDL/parameters/Initialization_list.hpp"

#include <cereal/types/vector.hpp>
#include <cereal/types/variant.hpp>
#include "cereal/types/utility.hpp"

class HDL_function_def;

class HDL_parameter {
public:
    HDL_parameter( const HDL_parameter &c );
    HDL_parameter();

    void set_name(const std::string &n) {
        name  = n;
    };
    std::shared_ptr<HDL_parameter> clone() const;

    void set_value(const resolved_parameter &val);

    std::string get_string_value() const {
        if (std::holds_alternative<std::vector<std::string>>(i_l.get_solved_value()))
            return std::get<std::vector<std::string>>(i_l.get_solved_value())[0];
        else
            return "";
    }
    [[nodiscard]] std::optional<int64_t>  get_numeric_value() const {
        return std::get<mdarray<int64_t>>(i_l.get_solved_value()).get_scalar();
    }
    [[nodiscard]] std::optional<mdarray<int64_t>> get_int_array_value() const{
        if(std::holds_alternative<mdarray<int64_t>>(i_l.get_solved_value()))
            return std::get<mdarray<int64_t>>(i_l.get_solved_value());
        return std::nullopt;
    };
    [[nodiscard]] std::optional<resolved_parameter> get_value() const {
        if(is_array()) {
            if(std::holds_alternative<mdarray<int64_t>>(i_l.get_solved_value())) {
                return get_int_array_value();
            } else {
                mdarray<std::string> ret_val;
                ret_val.set_1d_slice({0,0}, std::get<std::vector<std::string>>(i_l.get_solved_value()));
                return ret_val;
            }
        } else {
            if(std::holds_alternative<std::vector<std::string>>(i_l.get_solved_value())){
                return get_string_value();
            } else {
                return get_numeric_value();
            }
        }
    }

    bool propagate_constant(const qualified_identifier &constant_name, const resolved_parameter &constant_value);
    void propagate_function(const HDL_function_def &def);
    explicit operator std::string();

    bool is_function() const {return type == function_parameter;}
    bool is_array() const {return i_l.is_array();}
    bool is_packed_array() {return i_l.is_packed();}

    std::string get_name() const {return name;};
    qualified_identifier get_identifier(){return {"", "", name};}

    enum parameter_type {
        string_parameter=0,
        numeric_parameter=1,
        array_parameter = 2,
        expression_parameter = 4,
        function_parameter = 5
    };

    parameter_type get_type() const {return type;}
    void set_type(parameter_type t) {
        type = t;
    }

    bool is_empty();

    void add_component(const Expression_component &component);
    void set_expression(const std::shared_ptr<Parameter_value_base>  &e) {
        i_l.set_scalar(e);
    };
    std::shared_ptr<Parameter_value_base> get_expression() {
        auto exp =  i_l.get_scalar();
        if (!exp.has_value()) throw std::runtime_error("A scalar parameter has been initialized with an array");
        return exp.value();
    }
    void clear_expression() {
        i_l.clear_scalar();
    }

    std::string value_as_string() const;
    std::string to_string() const;

    friend bool operator <(const HDL_parameter& lhs, const HDL_parameter& rhs);
    friend bool operator==(const HDL_parameter&lhs, const HDL_parameter&rhs);

    friend void PrintTo(const HDL_parameter& point, std::ostream* os);

    std::set<qualified_identifier> get_dependencies();

    void add_initialization_list(const Initialization_list &i){
        i_l = i;
    }
    Initialization_list get_i_l() {return i_l;}

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name ,type, i_l);
    }

    nlohmann::json dump();



private:

    std::string name;
    parameter_type type;

    Initialization_list i_l;
};

constexpr std::string parameter_type_to_string(HDL_parameter::parameter_type in){
    switch(in){
        case HDL_parameter::string_parameter: return "string_parameter";
        case HDL_parameter::numeric_parameter: return "numeric_parameter";
        case HDL_parameter::array_parameter: return "array_parameter";
        case HDL_parameter::expression_parameter: return "expression_parameter";
        case HDL_parameter::function_parameter: return "function_parameter";
        default: return "unknown parameter type";
    }
}

#endif //ANANKE_HDL_PARAMETER_HPP
