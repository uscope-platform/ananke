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

#include "Parameter_value_base.hpp"
#include "Expression_component.hpp"
#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/HDL_loop.hpp"


class HDL_function_call : public Parameter_value_base{
public:
    HDL_function_call();
    explicit HDL_function_call(const std::string &n) {
        function_name = n;
        type = function;
    }
    void set_name(const std::string &n){function_name = n;}
    std::string get_name(){return function_name;}
    void add_argument(const std::shared_ptr<Parameter_value_base> &p);
    void add_assignment(const assignment &a) {assignments.push_back(a);}
    void set_loop(const HDL_loop_metadata &l){loop_metadata = l;}
    std::set<qualified_identifier> get_dependencies()const  override;
    void propagate_function(const HDL_function_def &def) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context)  override;

    std::optional<resolved_parameter> evaluate_scalar(const std::map<qualified_identifier, resolved_parameter> &context);
    std::optional<resolved_parameter> evaluate_vector(const std::map<qualified_identifier, resolved_parameter> &context);
    std::optional<resolved_parameter> evaluate_system_task(const std::map<qualified_identifier, resolved_parameter> &context);

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    std::string print() const  override;
    int64_t get_size()  override;


    [[nodiscard]] bool empty() const;


    std::shared_ptr<Parameter_value_base> clone_ptr() const  override;

    void add_body(const std::vector<assignment> &a, const std::optional<HDL_loop_metadata> &loop) {
        assignments = a;
        loop_metadata = loop;
    };

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(function_name, arguments, assignments, loop_metadata);
    }

private:
    std::string function_name;
    bool packing = false;
    std::vector<std::shared_ptr<Parameter_value_base>> arguments;


    std::vector<assignment> assignments;
    std::optional<HDL_loop_metadata> loop_metadata;

    bool isEqual(const Parameter_value_base& other) const override;

};


#endif //ANANKE_HDL_FUNCTION_CALL_HPP