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

#ifndef ANANKE_EXPRESSION_COMPONENT_HPP
#define ANANKE_EXPRESSION_COMPONENT_HPP

#include <string>
#include <utility>
#include <ctre.hpp>
#include <set>
#include <nlohmann/json.hpp>
#include <map>
#include <unordered_map>
#include <variant>
#include <cmath>

#include "data_model/HDL/parameters/common/qualified_identifier.hpp"

#include <cereal/types/vector.hpp>
#include <cereal/types/variant.hpp>

#include "data_model/HDL/parameters/HDL_function_def.hpp"
#include "Parameter_value_base.hpp"

class Expression;
class HDL_function_call;

class Expression_component {
public:

    Expression_component();
    Expression_component( const Expression_component &c );

    enum component_type {
        number,
        string,
        identifier,
        function,
        operation,
        parenthesis
    };


    explicit Expression_component(const std::string &s,const component_type &t);
    explicit Expression_component(std::variant<hdl_integer, double> n, int64_t b_s);
    explicit Expression_component(const std::shared_ptr<Parameter_value_base> &param);
    std::set<qualified_identifier> get_dependencies()const;
    void propagate_function(HDL_function_def def);
    bool is_subscripted() const {return !array_index.empty();}
    bool is_string() const {return type == string;}
    bool is_identifier() const {return type == identifier;}
    bool is_array() const {return value.is_int_array();}

    bool is_function() const {
        return type == function;
    }
    bool is_operator() const {
        return type == operation;
    }

    bool is_numeric() const {
        return type == number;
    }

    std::optional<resolved_parameter> get_value()const;
    std::optional<resolved_parameter> get_value(const std::map<qualified_identifier, resolved_parameter> &context) const;

    void set_value(const resolved_parameter &v) {
        value = v;
    }

    void set_instance_prefix(const std::string &p){instance_prefix = p;}
    std::string get_instance_prefix() {return instance_prefix;};

    void set_package_prefix(const std::string &s) {package_prefix = s;}
    std::string get_package_prefix() const {return package_prefix;};

    bool is_right_associative();
    int64_t get_operator_precedence();
    std::string print_value() const;

    typedef enum{
        unary_operator = 0,
        binary_operator = 1
    } operator_type_t;

    operator_type_t get_operator_type();

    friend bool operator==(const Expression_component&lhs, const Expression_component&rhs);

    void set_array_index(const std::vector<Expression> &v);
    void add_array_index(const Expression &c);
    std::vector<Expression> get_array_index();

    std::string print_index(const std::vector<Expression> &index)const;

    int64_t get_binary_size() const{return binary_size;}
    void set_binary_size(int64_t s) {binary_size = s;}

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(value,type, array_index, instance_prefix, package_prefix, binary_size);
    }

    static component_type get_type(const std::string &s);

private:
    static  std::pair<resolved_parameter, int64_t>  process_number(const std::string &s);

    component_type type = number;

    resolved_parameter value;

    std::shared_ptr<HDL_function_call> call;

    std::string package_prefix;
    std::string instance_prefix;

    int64_t binary_size = 0;

    std::vector<Expression> array_index;

    static constexpr bool is_string_operator(std::string_view op) {
        constexpr std::string_view operators[] = {
            "!", "~", "*", "/", "%", "+", "-", "<<", ">>", "<<<", ">>>",
            ">", ">=", "<", "<=", "==", "!=", "&", "|", "^", "~^", "^~", "**",
            "&&", "||"
        };

        return std::ranges::any_of(operators, [op](std::string_view valid_op) {
            return op == valid_op;
        });
    }

    static constexpr bool is_string_parenthesis(std::string_view op) {
        constexpr std::string_view operators[] = {
            "(", ")", "[", "]", "{", "}"
        };

        return std::ranges::any_of(operators, [op](std::string_view valid_op) {
            return op == valid_op;
        });
    }

    static constexpr bool is_string_function(std::string_view op) {
        constexpr std::string_view operators[] = {
            "$clog2","$ceil", "$floor","$pow", "$rtoi", "$itor",
        };

        return std::ranges::any_of(operators, [op](std::string_view valid_op) {
            return op == valid_op;
        });
    }


    std::unordered_map<std::string, operator_type_t> operators_types = {
            {"$clog2", unary_operator},
            {"$ceil", unary_operator},
            {"$floor", unary_operator},
            {"$rtoi", unary_operator},
            {"$itor", unary_operator},
            {"$pow", binary_operator},
            {"!", unary_operator},
            {"~", unary_operator},
            {"**", binary_operator},
            {"*", binary_operator},
            {"/", binary_operator},
            {"&", binary_operator},
            {"|", binary_operator},
            {"^", binary_operator},
            {"^~", binary_operator},
            {"~^", binary_operator},
            {"%", binary_operator},
            {"+", binary_operator},
            {"-", binary_operator},
            {"<<", binary_operator},
            {">>", binary_operator},
            {"<<<", binary_operator},
            {">>>", binary_operator},
            {">", binary_operator},
            {">=", binary_operator},
            {"<", binary_operator},
            {"<=", binary_operator},
            {"==",  binary_operator},
            {"!=", binary_operator},
            {"&&", binary_operator},
            {"||", binary_operator}
    };


    std::unordered_map<std::string, int64_t> operators_precedence = {
            {"$clog2", 0},
            {"$ceil",  0},
            {"$floor", 0},
            {"$rtoi",  0},
            {"$itor",  0},
            {"$pow",   0},
            {"!",      1},
            {"~",      1},
            {"**",     2},
            {"*",      3},
            {"/",      3},
            {"%",      3},
            {"+",      4},
            {"-",      4},
            {"<<",     5},
            {">>",     5},
            {"<<<",     5},
            {">>>",     5},
            {">",      6},
            {">=",     6},
            {"<",      6},
            {"<=",     6},
            {"==",     6},
            {"!=",     6},
            {"&",      7},
            {"^",      8},
            {"~^",      8},
            {"^~",      8},
            {"|",      9},
            {"&&",    10},
            {"||",    11},
    };

    std::set<std::string> right_associative_set = {
           "$itor","$rtoi", "$clog2","$ceil", "$floor","$pow", "!", "~"
    };


};

#endif //ANANKE_EXPRESSION_COMPONENT_HPP
