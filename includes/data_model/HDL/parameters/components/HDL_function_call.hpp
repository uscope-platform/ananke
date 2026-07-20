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


#ifndef ANANKE_HDL_FUNCTION_CALL_HPP
#define ANANKE_HDL_FUNCTION_CALL_HPP

#include "Expression_base.hpp"
#include "data_model/HDL/statement/hdl_function_statement.hpp"
#include "data_model/HDL/statement/hdl_assignment_statement.hpp"
#include "data_model/HDL/statement/hdl_loop_statement.hpp"


class HDL_function_call : public Expression_base{
public:
    HDL_function_call() = default;
    explicit HDL_function_call(const std::string &n) {
        function_name = n;
    }
    void set_name(const std::string &n){function_name = n;}
    std::string get_name(){return function_name;}
    void add_argument(const std::shared_ptr<Expression_base> &p);
    void add_package_prefix(const std::string &p){package_prefix = p;}
    std::string get_package_prefix() const {return package_prefix;}
    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_statement &def) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;

    std::optional<resolved_parameter> evaluate_system_task(const std::map<qualified_identifier, resolved_parameter> &context);
    void apply_return_order_reversal(
        std::vector<hdl_integer> &values,
        std::vector<int64_t> &value_sizes,
        const std::map<qualified_identifier, resolved_parameter> &context
    );

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    std::string print() const override;
    int64_t get_size() override;

    [[nodiscard]] bool empty() const;

    template<class Archive>
    void serialize(Archive & ar) {
        ar(function_name, arguments, body, package_prefix);
    }

private:
    std::string function_name;
    std::string package_prefix;
    std::vector<std::shared_ptr<Expression_base>> arguments;

    std::vector<std::shared_ptr<hdl_statement_base>> body;

    bool packing = false;
    bool container_unpacked_ascending = false;
    bool has_return_unpacked_ascending = false;
    bool return_unpacked_ascending = false;

    bool isEqual(const Expression_base& other) const override;
};


#endif //ANANKE_HDL_FUNCTION_CALL_HPP
