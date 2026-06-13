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

#include "data_model/HDL/parameters/components/Parameter_value_base.hpp"

class Token : public Parameter_value_base{
public:

    enum token_type {
        number,
        string,
        identifier,
        function,
        operation,
        parenthesis
    };

    Token() = default;

    void set_instance_prefix(const std::string &p){instance_prefix = p;}
    std::string get_instance_prefix() {return instance_prefix;};

    void set_package_prefix(const std::string &s) {package_prefix = s;}
    std::string get_package_prefix() const {return package_prefix;};

    void set_value(const resolved_parameter &v) {value = v;}
    std::optional<resolved_parameter> get_value()const{return value;}


    int64_t get_binary_size() const{return binary_size;}
    void set_binary_size(int64_t s) {binary_size = s;}

    void set_array_index(const std::vector<std::shared_ptr<Parameter_value_base>> &a_i){array_index = a_i;};
    void add_array_index(const std::shared_ptr<Parameter_value_base> &a_i){array_index.push_back(a_i);}
    std::vector<std::shared_ptr<Parameter_value_base>> get_array_index(){return array_index;}



    typedef enum{
        unary_operator = 0,
        binary_operator = 1
    } operator_type_t;

    operator_type_t get_operator_type();


    template<class Archive>
    void serialize( Archive & ar ) {
        ar(value,type, array_index, instance_prefix, package_prefix, binary_size);
    }

    friend bool operator==(const Token &lhs, const Token &rhs);

private:

    bool isEqual(const Parameter_value_base& other) const override {
        const auto& rhs = static_cast<const Token&>(other);

        bool res = std::tie(type, value, package_prefix, instance_prefix, binary_size) == std::tie(rhs.type, rhs.value, rhs.package_prefix, rhs.instance_prefix, rhs.binary_size);
        if (array_index.size() != rhs.array_index.size()) return false;
        for (int i = 0; i< array_index.size(); i++) {
            res &= *array_index[i] == *rhs.array_index[i];
        }
        return res;
    }

    token_type type = number;

    resolved_parameter value;

    std::string package_prefix;
    std::string instance_prefix;

    int64_t binary_size = 0;

    std::vector<std::shared_ptr<Parameter_value_base>> array_index;
};


#endif //ANANKE_TOKEN_HPP
