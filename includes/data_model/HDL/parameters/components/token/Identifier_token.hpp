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

#ifndef ANANKE_IDENTIFIER_TOKEN_HPP
#define ANANKE_IDENTIFIER_TOKEN_HPP

#include <bitset>
#include "data_model/HDL/parameters/common/qualified_identifier.hpp"

#include <cereal/types/vector.hpp>
#include <cereal/types/variant.hpp>

#include "data_model/HDL/parameters/components/Expression_base.hpp"

class Identifier_token :public Expression_base{
public:
    Identifier_token() = default;
    Identifier_token(const Identifier_token &c);

    explicit Identifier_token(const qualified_identifier &q_i);

    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_statement &def) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;

    bool is_subscripted() const {return !array_index.empty();}

    std::string print() const override;
    int64_t get_size() override {return 0;}

    friend bool operator==(const Identifier_token &lhs, const Identifier_token &rhs);

    void set_array_index(const std::vector<std::shared_ptr<Expression_base>> &v) {array_index = v;}
    void add_array_index(const std::shared_ptr<Expression_base> &a_i) {array_index.push_back(a_i);}
    std::vector<std::shared_ptr<Expression_base>> get_array_index() {return array_index;}

    qualified_identifier get_value() const {return id;}


    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(id, array_index);
    }

private:
    std::string print_index(const std::vector<std::shared_ptr<Expression_base>> &index) const;

    bool isEqual(const Expression_base& other) const override;

    qualified_identifier id;
    std::vector<std::shared_ptr<Expression_base>> array_index;
};


#endif //ANANKE_IDENTIFIER_TOKEN_HPP
