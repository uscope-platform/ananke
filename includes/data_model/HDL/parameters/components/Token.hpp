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

    friend bool operator==(const Token &lhs, const Token &rhs);

    bool isEqual(const Parameter_value_base& other) const override {
        const auto& rhs = static_cast<const Token&>(other);

        bool res = std::tie(type, value, package_prefix, instance_prefix, binary_size) == std::tie(rhs.type, rhs.value, rhs.package_prefix, rhs.instance_prefix, rhs.binary_size);
        if (array_index.size() != rhs.array_index.size()) return false;
        for (int i = 0; i< array_index.size(); i++) {
            res &= *array_index[i] == *rhs.array_index[i];
        }
        return res;
    }

private:

    token_type type = number;

    resolved_parameter value;

    std::string package_prefix;
    std::string instance_prefix;

    int64_t binary_size = 0;

    std::vector<std::shared_ptr<Parameter_value_base>> array_index;
};


#endif //ANANKE_TOKEN_HPP
