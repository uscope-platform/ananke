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
#include <variant>

#include "data_model/HDL/parameters/common/qualified_identifier.hpp"

#include <cereal/types/vector.hpp>
#include <cereal/types/variant.hpp>

#include "data_model/HDL/parameters/components/Parameter_value_base.hpp"

class HDL_function_call;

class Token : public Parameter_value_base{
public:

    Token();
    Token(const Token &c);

    enum token_type {
        number,
        string,
        identifier,
        function
    };

    explicit Token(const std::string &s, const token_type &t);
    explicit Token(std::variant<hdl_integer, double> n, int64_t b_s);
    explicit Token(const std::shared_ptr<Parameter_value_base> &param);

    std::set<qualified_identifier> get_dependencies() const override;
    void propagate_function(const HDL_function_def &def) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;

    bool is_subscripted() const {return !array_index.empty();}

    std::string print() const override;
    int64_t get_size() override;


    static token_type get_type(const std::string &s);
    static bool try_replace_identifier(std::shared_ptr<Parameter_value_base> &slot,
                                        const qualified_identifier &constant_id,
                                        const std::shared_ptr<Parameter_value_base> &value);


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


    void set_binary_size(int64_t s) {binary_size = s;}

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(value, type, array_index, instance_prefix, package_prefix, binary_size, container_size);
    }

private:
    std::string print_index(const std::vector<std::shared_ptr<Parameter_value_base>> &index) const;

    static std::pair<resolved_parameter, int64_t> process_number(const std::string &s);

    bool isEqual(const Parameter_value_base& other) const override;

    bool is_identifier() const {return type == identifier;}
    bool is_numeric() const {return type == number;}

    token_type type = number;

    resolved_parameter value;

    std::shared_ptr<HDL_function_call> call;

    std::string package_prefix;
    std::string instance_prefix;

    int64_t binary_size = 0;
    int64_t container_size = 0;

    std::vector<std::shared_ptr<Parameter_value_base>> array_index;

    static constexpr bool is_string_parenthesis(std::string_view op) {
        constexpr std::string_view operators[] = {
            "(", ")", "[", "]", "{", "}"
        };
        return std::ranges::any_of(operators, [op](std::string_view valid_op) {
            return op == valid_op;
        });
    }

};


#endif //ANANKE_TOKEN_HPP
