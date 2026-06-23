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

#ifndef ANANKE_TOKEN_HPP
#define ANANKE_TOKEN_HPP

#include <string>
#include <utility>
#include <ctre.hpp>
#include <set>
#include <map>
#include <unordered_map>
#include <variant>
#include <cmath>

#include "data_model/HDL/parameters/common/qualified_identifier.hpp"

#include <cereal/types/vector.hpp>
#include <cereal/types/variant.hpp>

#include "data_model/HDL/parameters/components/Parameter_value_base.hpp"

class Expression;
class HDL_function_call;

class Token : public Parameter_value_base{
public:

    Token();
    Token(const Token &c);

    enum token_type {
        number,
        string,
        identifier,
        function,
        operation,
        parenthesis
    };

    enum sv_operators {
        logic_neg,
        bitwise_neg,
        power,
        multiply,
        divide,
        modulo,
        add,
        subtract,
        logic_shift_left,
        logic_shift_right,
        arithmetic_shift_left,
        arithmetic_shift_right,
        greater,
        greater_equal,
        less,
        less_equal,
        equal,
        not_equal,
        bitwise_and,
        bitwise_xor,
        bitwise_xnor,
        bitwise_or,
        logical_and,
        logical_or
    };

    explicit Token(const sv_operators &op);
    explicit Token(const std::string &s, const token_type &t);
    explicit Token(std::variant<hdl_integer, double> n, int64_t b_s);
    explicit Token(const std::shared_ptr<Parameter_value_base> &param);

    std::set<qualified_identifier> get_dependencies() const override;
    void propagate_function(const HDL_function_def &def) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;

    bool is_subscripted() const {return !array_index.empty();}
    bool is_string() const {return type == string;}
    bool is_identifier() const {return type == identifier;}
    bool is_array() const {return value.is_int_array();}
    bool is_function() const {return type == function;}
    bool is_operator() const {return type == operation;}
    bool is_numeric() const {return type == number;}
    bool is_parenthesis() const {return type == parenthesis;}

    bool is_right_associative();
    int64_t get_operator_precedence();
    std::string print() const override;
    int64_t get_size() override;

    typedef enum{
        unary_operator = 0,
        binary_operator = 1
    } operator_type_t;

    static token_type get_type(const std::string &s);
    operator_type_t get_operator_type();

    friend bool operator==(const Token &lhs, const Token &rhs);

    void set_array_index(const std::vector<std::shared_ptr<Parameter_value_base>> &v) {array_index = v;}
    void add_array_index(const std::shared_ptr<Parameter_value_base> &a_i) {array_index.push_back(a_i);}
    std::vector<std::shared_ptr<Parameter_value_base>> get_array_index() {return array_index;}

    void set_instance_prefix(const std::string &p){instance_prefix = p;}
    std::string get_instance_prefix() {return instance_prefix;};

    void set_package_prefix(const std::string &s) {package_prefix = s;}
    std::string get_package_prefix() const {return package_prefix;};

    void set_value(const resolved_parameter &v) {value = v;}
    std::optional<resolved_parameter> get_value() const {return value;}

    sv_operators get_operation() const {return operator_value;}

    int64_t get_binary_size() const {return binary_size;}
    void set_binary_size(int64_t s) {binary_size = s;}

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(value, type, array_index, instance_prefix, package_prefix, binary_size, operator_value);
    }

private:
    std::string print_index(const std::vector<std::shared_ptr<Parameter_value_base>> &index) const;

    static std::pair<resolved_parameter, int64_t> process_number(const std::string &s);

    bool isEqual(const Parameter_value_base& other) const override;

    token_type type = number;

    resolved_parameter value;
    sv_operators operator_value;

    std::shared_ptr<HDL_function_call> call;

    std::string package_prefix;
    std::string instance_prefix;

    int64_t binary_size = 0;

    std::vector<std::shared_ptr<Parameter_value_base>> array_index;

    static constexpr bool is_string_parenthesis(std::string_view op) {
        constexpr std::string_view operators[] = {
            "(", ")", "[", "]", "{", "}"
        };
        return std::ranges::any_of(operators, [op](std::string_view valid_op) {
            return op == valid_op;
        });
    }

    std::unordered_map<sv_operators, int64_t> operators_precedence = {
        {logic_neg,      1},
        {bitwise_neg,      1},
        {power,     2},
        {multiply,      3},
        {divide,      3},
        {modulo,      3},
        {add,      4},
        {subtract,      4},
        {logic_shift_left,     5},
        {logic_shift_right,     5},
        {arithmetic_shift_left,     5},
        {arithmetic_shift_right,     5},
        {greater,      6},
        {greater_equal,     6},
        {less,      6},
        {less_equal,     6},
        {equal,     6},
        {not_equal,     6},
        {bitwise_and,      7},
        {bitwise_xor,      8},
        {bitwise_xnor,      8},
        {bitwise_or,      9},
        {logical_and,    10},
        {logical_or,    11},
    };

    std::set<sv_operators> right_associative_set = {
        bitwise_neg, logic_neg
    };

    std::map<sv_operators, std::string> print_operator = {
        {logic_neg, "!"},
        {bitwise_neg, "~"},
        {power,"**"},
        {multiply,"*"},
        {divide,"/"},
        {modulo,"%"},
        {add,"+"},
        {subtract,"-"},
        {logic_shift_left,"<<"},
        {logic_shift_right,">>"},
        {arithmetic_shift_left,"<<<"},
        {arithmetic_shift_right,">>>"},
        {greater,">"},
        {greater_equal,">="},
        {less,"<"},
        {less_equal,"<="},
        {equal,"=="},
        {not_equal,"!="},
        {bitwise_and,"&"},
        {bitwise_xor,"^"},
        {bitwise_xnor,"~^"},
        {bitwise_or,"|"},
        {logical_and,"&&"},
        {logical_or,"||"}
    };
};


#endif //ANANKE_TOKEN_HPP
