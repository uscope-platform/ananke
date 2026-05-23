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

#include "data_model/HDL/parameters/components/HDL_function_call.hpp"
#include "data_model/HDL/parameters/components/Replication.hpp"
#include "data_model/HDL/parameters/components/Expression.hpp"

#include "analysis/loop_solver.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>


CEREAL_REGISTER_TYPE(HDL_function_call)

CEREAL_REGISTER_POLYMORPHIC_RELATION(Parameter_value_base, HDL_function_call)

HDL_function_call::HDL_function_call() {
    type = function;
}

void HDL_function_call::add_argument(const std::shared_ptr<Parameter_value_base> &p) {
    arguments.push_back(p);
}

std::set<qualified_identifier> HDL_function_call::get_dependencies() const {
    std::set<qualified_identifier> retval;
    for (auto &arg:arguments) {
        auto deps = arg->get_dependencies();
        retval.insert(deps.begin(), deps.end());
    }
    for(auto &a:assignments) {
        auto deps = a.get_value()->get_dependencies();
        retval.insert(deps.begin(), deps.end());
    }
    if(loop_metadata.has_value()) {
        auto loop_deps = loop_metadata.value().get_dependencies();
        retval.insert(loop_deps.begin(), loop_deps.end());
    }
    return retval;
}

bool HDL_function_call::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value) {
    bool retval = true;
    for (auto &arg:arguments) {
        retval &= arg->propagate_constant(constant_id, value);
    }
    for(auto &a:assignments) {
        if(a.get_index().has_value()) {
            retval &= a.get_index().value()->propagate_constant(constant_id, value);
        }
        retval &= a.get_value()->propagate_constant(constant_id, value);
    }
    if(loop_metadata.has_value()) {
        retval &= loop_metadata.value().propagate_constant(constant_id, value);
    }
    return  retval;
}

void HDL_function_call::propagate_function(const HDL_function_def &def) {
    if(def.name == function_name) {
        auto arg_names = def.get_arguments_names();
        assignments = def.get_assignments();
        loop_metadata = def.get_loop();
        for (int i =0;i<arg_names.size(); i++) {
            for (auto &a:assignments) {
                a.propagate_argument(arg_names[i], arguments[i]);
            }
        }
        //TODO: do actual argument inlining
    }
}

std::optional<resolved_parameter> HDL_function_call::evaluate() {
    if (function_name.starts_with("$")) {
        return evaluate_system_task();
    }
    if( !loop_metadata.has_value() && assignments.size() == 1) {
        return evaluate_scalar();
    } else {
        return evaluate_vector();
    }
}

std::optional<resolved_parameter> HDL_function_call::evaluate_scalar() {
    auto raw_value = assignments[0].get_value()->evaluate();
    if (!raw_value) return std::nullopt;
    if (packing) {
        if (!raw_value.value().is_int_array()) return raw_value;
        auto components = raw_value.value().get_int_array().get_1d_slice({0, 0});
        auto size = 0;
        if (assignments[0].get_value()->is_replication()) size = assignments[0].get_value()->as<Replication>().get_item()->get_size();
        else size = assignments[0].get_value()->get_size();
        std::vector<int64_t> packing_sizes(components.size(), size);
        return pack_values(components, packing_sizes);
    } else {
        return raw_value;
    }
}

std::optional<resolved_parameter> HDL_function_call::evaluate_vector() {
    std::vector<hdl_integer> loop_indexes;
    if(loop_metadata.has_value()) {
        loop_indexes = loop_solver::solve_loop(loop_metadata.value());

        qualified_identifier loop_var = loop_metadata.value().get_init().get_identifier();
    } else {
        loop_indexes = {};
    }
    std::vector<hdl_integer> values(assignments.size() + loop_indexes.size());
    std::vector<int64_t> value_sizes(assignments.size() + loop_indexes.size());
    for(auto &a:assignments) {
        auto idx = a.get_index().value()->evaluate();
        if(!idx.has_value()) return std::nullopt;
        if(!idx.value().is_integer()) return std::nullopt;
        auto idx_val = idx.value().get_integer();
        auto value = a.get_value()->evaluate();
        value_sizes[idx_val.get_value()] = a.get_value()->get_size();
        if(!value.has_value()) return std::nullopt;
        values[idx_val.get_value()] = value.value().get_integer();
    }

    if(loop_metadata.has_value()) {
        qualified_identifier loop_var =  loop_metadata.value().get_init().get_identifier();
        auto loop_assignments = loop_metadata.value().get_assignments();
        for(int i = 0; i<loop_assignments.size(); i++) {
            for(auto &l:loop_indexes) {
                auto la = loop_assignments[i].clone();
                la.get_index().value()->propagate_constant(loop_var, l);
                auto idx_val = la.get_index().value()->evaluate().value().get_integer();
                la.get_value()->propagate_constant(loop_var, l);
                value_sizes[idx_val.get_value()] = la.get_value()->get_size();
                auto var = la.get_value()->evaluate();
                values[idx_val.get_value()] = var.value().get_integer();
            }
        }
    }

    mdarray<hdl_integer> result;
    result.set_1d_slice({0, 0}, values);
    if (packing) {
        result.set_1d_slice({0,0}, {pack_values(values, value_sizes)});
    }
    return result;
}

std::optional<resolved_parameter> HDL_function_call::evaluate_system_task() {
    std::string task_name = function_name.substr(1, function_name.size()-1);
    std::vector<resolved_parameter> resolved_arguments;
    for (auto &arg:arguments) {
        auto resolved_val = arg->evaluate();
        if (!resolved_val.has_value()) return std::nullopt;
        resolved_arguments.push_back(resolved_val.value());
    }
    if (task_name == "rtoi") {
        if (resolved_arguments[0].is_real()) {
            return static_cast<hdl_integer>(std::round(resolved_arguments[0].get_real()));
        }
        if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer();
        }
        spdlog::warn("Encountered an invalid argument for a $rtoi call");
        return  0;
    }
    if (task_name == "itor") {
        if (resolved_arguments[0].is_real()) {
            return resolved_arguments[0].get_real();
        } else if (resolved_arguments[0].is_integer()) {
            return static_cast<double>(resolved_arguments[0].get_integer().get_value());
        }
        spdlog::warn("Encountered an invalid argument for a $itor call");
        return  0;
    }
    if (task_name == "ceil") {
        if (resolved_arguments[0].is_real()) {
            return std::ceil(resolved_arguments[0].get_real());
        } else if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer();
        }
        spdlog::warn("Encountered an invalid argument for a $ceil call");
    }
    if (task_name == "floor") {
        if (resolved_arguments[0].is_real()) {
            return std::floor(resolved_arguments[0].get_real());
        } else if (resolved_arguments[0].is_integer()) {
            return resolved_arguments[0].get_integer();
        }
        spdlog::warn("Encountered an invalid argument for a $floor call");

    }
    if (task_name == "clog2") {
        if (resolved_arguments[0].is_real()) {
            return static_cast<hdl_integer>(std::ceil(std::log2(resolved_arguments[0].get_real())));
        } else if (resolved_arguments[0].is_integer()) {
            return static_cast<hdl_integer>(std::ceil(std::log2(resolved_arguments[0].get_integer().get_value())));
        }
        spdlog::warn("Encountered an invalid argument for a $floor call");
    }
    spdlog::warn("Unsupported system task {} encountered while parsing a parameter", function_name);
    return 0;
}

void HDL_function_call::set_container_sizes(const resolved_type &s) {
    packing = s.unpacked_sizes.empty();
    if (s.packed_sizes.empty() && s.unpacked_sizes.empty()) return;
    if (s.unpacked_sizes.empty()) {
        if (assignments.size()==1) assignments[0].set_container_size(s);
        else {
            resolved_type lower_container_size;
            lower_container_size.packed_sizes.insert(
                lower_container_size.packed_sizes.begin(),
                s.packed_sizes.begin(),
                s.packed_sizes.end()-1
            );
            for (auto &a:assignments) {
                a.set_container_size(lower_container_size);
            }
        }
    } else {
        if (assignments.size()==1) assignments[0].set_container_size(s);
        else {
            resolved_type lower_container_size;
            lower_container_size.packed_sizes = s.unpacked_sizes;
            lower_container_size.unpacked_sizes.insert(
                lower_container_size.unpacked_sizes.begin(),
                s.unpacked_sizes.begin(),
                s.unpacked_sizes.end()-1
            );
            for (auto &a:assignments) {
                a.set_container_size(lower_container_size);
            }
        }
    }
    //TODO: deal with loop
}


std::string HDL_function_call::print() const {
    std::ostringstream result;
    result << function_name << "(";
    for(int i = 0; i< arguments.size(); i++) {
        result << arguments[0]->print();
        if( arguments.size()>1 && i<arguments.size()-1) result << ", ";
    }
    result << ")";
    return result.str();
}

int64_t HDL_function_call::get_size() {
    return 0;
}


bool HDL_function_call::empty() const {
    return function_name.empty();
}

std::shared_ptr<Parameter_value_base> HDL_function_call::clone_ptr() const {
    HDL_function_call c;
    for (auto &arg:arguments) {
        c.arguments.push_back(arg->clone_ptr());
    }
    c.function_name = function_name;
    c.packing = packing;
    if(loop_metadata) c.loop_metadata = loop_metadata.value().clone();
    for (auto &ass:assignments) {
        c.assignments.push_back(ass.clone());
    }
    return std::make_shared<HDL_function_call>(c);
}

bool HDL_function_call::isEqual(const Parameter_value_base &other) const {
    bool is_equal = true;


    const auto& rhs = static_cast<const HDL_function_call&>(other);
    is_equal &= function_name == rhs.function_name;
    if (arguments.size() != rhs.arguments.size()) return false;
    for (int i = 0; i< arguments.size(); i++) {
        is_equal &= *arguments[i] == *rhs.arguments[i];
    }
    is_equal &= loop_metadata == rhs.loop_metadata;
    is_equal &= assignments == rhs.assignments;
    return is_equal;
}
