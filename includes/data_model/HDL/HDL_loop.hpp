//  Copyright 2025 Filippo Savi
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

#ifndef ANANKE_HDL_LOOP_HPP
#define ANANKE_HDL_LOOP_HPP


#include <optional>
#include <set>
#include <cereal/types/optional.hpp>
#include <spdlog/spdlog.h>

#include "parameters/common/qualified_identifier.hpp"
#include "parameters/components/Expression_base.hpp"
#include "parameters/components/Expression_v2.hpp"

class HDL_parameter;


struct assignment {
    assignment() = default;
    assignment(const std::string &n,const std::optional<std::shared_ptr<Expression_base>> &idx, const  std::shared_ptr<Expression_base> &val);
    bool operator==(const assignment &rhs) const;

    template<class Archive>
    void serialize( Archive & ar ){
        ar(name, index, value);
    }
    void set_container_size(const resolved_type &r, const std::map<qualified_identifier, resolved_parameter> &context = {}) {value->set_container_sizes(r, context);}
    void set_index(const std::shared_ptr<Expression_base> &idx);
    void set_value(const std::shared_ptr<Expression_base> &val);
    std::optional<std::shared_ptr<Expression_base>> get_index() const;
    std::shared_ptr<Expression_base> get_value() const;
    std::string get_name() const { return name; }
    void propagate_argument(const std::string &name, const std::shared_ptr<Expression_base> &value);
private:
    std::string name;
    std::optional<std::shared_ptr<Expression_base>> index;
    std::shared_ptr<Expression_base> value;
};

class HDL_loop_metadata {

public:

    HDL_loop_metadata() = default;
    ~HDL_loop_metadata();

    HDL_loop_metadata(const HDL_loop_metadata &other);

    HDL_loop_metadata(HDL_loop_metadata &&other) noexcept;

    HDL_loop_metadata & operator=(const HDL_loop_metadata &other);

    HDL_loop_metadata & operator=(HDL_loop_metadata &&other) noexcept;
    parameter_deps_t get_dependencies() const;

    void set_init(const HDL_parameter &p);
    void set_end_c(const Expression_v2 &e);
    void set_iter(const Expression_v2 &i);

    void add_assignment(const assignment &a);

    void set_assignments(const std::vector<assignment> &a);

    HDL_parameter get_init() const;
    Expression_v2 get_end_c() const;
    Expression_v2 get_iter() const;

    std::vector<assignment> get_assignments() const;

    template<class Archive>
    void serialize(Archive & archive){
        archive( init, end_c, iter, assignments);
    }


    bool operator==(const HDL_loop_metadata &rhs) const;


private:

    std::shared_ptr<HDL_parameter> init;
    std::unique_ptr<Expression_v2> end_c;
    std::unique_ptr<Expression_v2> iter;
    std::vector<assignment> assignments;

};

#endif //ANANKE_HDL_LOOP_HPP
