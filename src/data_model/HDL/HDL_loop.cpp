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

#include "data_model/HDL/HDL_loop.hpp"

#include "data_model/HDL/parameters/components/Expression.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"

assignment::assignment(const std::string &n, const std::optional<std::shared_ptr<Parameter_value_base>> &idx,
                       const std::shared_ptr<Parameter_value_base> &val) {
    name = n;
    index = idx;
    value = val;
}

bool assignment::operator==(const assignment &rhs) const {
    bool retval = true;
    retval &= name == rhs.name;
    retval &= *value == *rhs.value;
    if(index.has_value() ^ rhs.index.has_value()) return false;
    if(index.has_value()) {
        retval &= *index.value() == *rhs.index.value();
    }

    return retval;
}

void assignment::set_index(const std::shared_ptr<Parameter_value_base> &idx) {
    index = idx;
}

void assignment::set_value(const std::shared_ptr<Parameter_value_base> &val) {
    value = val;
}

std::optional<std::shared_ptr<Parameter_value_base>> assignment::get_index() const {
    return index;
}

std::shared_ptr<Parameter_value_base> assignment::get_value() const {
    return value;
}

void assignment::propagate_argument(const std::string &name, const std::shared_ptr<Parameter_value_base> &arg_value) {

    if (index.has_value()) {
        index.value()->propagate_expression({"", "", name},arg_value);
    }
    value->propagate_expression({"", "", name}, arg_value);
}

assignment assignment::clone() const {
    assignment a;
    a.name = name;
    if(index.has_value()) a.set_index(index.value());
    a.set_value(value);
    return a;
}

HDL_loop_metadata::~HDL_loop_metadata() = default;

HDL_loop_metadata::HDL_loop_metadata(const HDL_loop_metadata &other)
        : init(other.init),
          assignments(other.assignments) {

    end_c = other.end_c ? std::make_unique<Expression>(*other.end_c): nullptr;
    iter = other.iter ? std::make_unique<Expression>(*other.iter): nullptr;
}

HDL_loop_metadata HDL_loop_metadata::clone() const{
    HDL_loop_metadata ret_val;
    ret_val.init = init ? init->clone(): nullptr;
    ret_val.end_c = end_c ? std::make_unique<Expression>(*end_c): nullptr;
    ret_val.iter = iter ? std::make_unique<Expression>(*iter): nullptr;
    for(const auto &a:assignments) {
        ret_val.assignments.push_back(a.clone());
    }
    return ret_val;
}

HDL_loop_metadata::HDL_loop_metadata(HDL_loop_metadata &&other) noexcept
        :
          init(std::move(other.init)),
          end_c(std::move(other.end_c)),
          iter(std::move(other.iter)),
          assignments(std::move(other.assignments)) {
}

HDL_loop_metadata & HDL_loop_metadata::operator=(const HDL_loop_metadata &other) {
    if(this == &other)
        return *this;
    init = other.init;
    end_c = other.end_c ? std::make_unique<Expression>(*other.end_c): nullptr;
    iter = other.iter ? std::make_unique<Expression>(*other.iter): nullptr;
    assignments = other.assignments;
    return *this;
}

HDL_loop_metadata & HDL_loop_metadata::operator=(HDL_loop_metadata &&other) noexcept {
    if(this == &other)
        return *this;
    init = std::move(other.init);
    end_c = std::move(other.end_c);
    iter = std::move(other.iter);
    assignments = std::move(other.assignments);
    return *this;
}

std::set<qualified_identifier> HDL_loop_metadata::get_dependencies() const {
    std::set<qualified_identifier> deps;
    auto loop_var = init->get_name();

    auto init_deps = init->get_dependencies();
    deps.insert(init_deps.begin(), init_deps.end());
    auto end_c_deps = end_c->get_dependencies();
    deps.insert(end_c_deps.begin(), end_c_deps.end());
    auto iter_deps = iter->get_dependencies();
    deps.insert(iter_deps.begin(), iter_deps.end());
    for(auto &a:assignments) {
        auto a_deps = a.get_value()->get_dependencies();
        deps.insert(a_deps.begin(), a_deps.end());
    }
    deps.erase({"","",  loop_var});
    return deps;

}


void HDL_loop_metadata::set_init(const HDL_parameter &p) {
    init = std::make_shared<HDL_parameter>(p);
}
void HDL_loop_metadata::set_end_c(const Expression &e) {
    end_c = std::make_unique<Expression>(e);
}
void HDL_loop_metadata::set_iter(const Expression &i) {
    iter = std::make_unique<Expression>(i);
}


HDL_parameter HDL_loop_metadata::get_init() const {
    if(init == nullptr) return HDL_parameter();
    return *init;
}
Expression HDL_loop_metadata::get_end_c() const {
    if(end_c == nullptr) return Expression();
    return *end_c;
}

Expression HDL_loop_metadata::get_iter() const {
    if(iter == nullptr) return Expression();
    return *iter;
}

void HDL_loop_metadata::add_assignment(const assignment &a) {
    assignments.push_back(a);
}
void HDL_loop_metadata::set_assignments(const std::vector<assignment> &a) {
    assignments = a;
}

std::vector<assignment> HDL_loop_metadata::get_assignments() const {
    return assignments;
}



bool HDL_loop_metadata::operator==(const HDL_loop_metadata &rhs) const {
    bool retval = true;
    retval &= *init == *rhs.init;
    retval &= *end_c == *rhs.end_c;
    retval &= *iter == *rhs.iter;
    retval &= assignments == rhs.assignments;
    return retval;
}