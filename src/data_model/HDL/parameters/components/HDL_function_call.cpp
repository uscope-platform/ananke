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

#include "analysis/loop_solver.hpp"

#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>
#include <algorithm>


CEREAL_REGISTER_TYPE(HDL_function_call)

CEREAL_REGISTER_POLYMORPHIC_RELATION(Expression_base, HDL_function_call)

void HDL_function_call::add_argument(const std::shared_ptr<Expression_base> &p) {
    arguments.push_back(p);
}

parameter_deps_t HDL_function_call::get_dependencies() const {
    parameter_deps_t retval;
    for (auto &arg:arguments) {
        retval.merge(arg->get_dependencies());
    }
    for(auto &a:assignments) {
        retval.merge(a.get_value()->get_dependencies());
    }
    if(loop_metadata.has_value()) {
        retval.merge(loop_metadata.value().get_dependencies());
    }
    retval.functions.insert(qualified_identifier(package_prefix, function_name));
    return retval;
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
    }
}

std::optional<resolved_parameter> HDL_function_call::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    if (function_name.starts_with("$")) {
        return evaluate_system_task(context);
    }
    if( !loop_metadata.has_value() && assignments.size() == 1) {
        return evaluate_scalar(context);
    } else {
        return evaluate_vector(context);
    }
}

std::optional<resolved_parameter> HDL_function_call::evaluate_scalar(const std::map<qualified_identifier, resolved_parameter> &context) {
    auto raw_value = assignments[0].get_value()->evaluate(context);
    if (!raw_value) return std::nullopt;
    if (packing) {
        if (!raw_value.value().is_int_array()) return raw_value;
        auto components = raw_value.value().get_int_array().get_1d_slice({0, 0});
        auto size = 0;
        if (assignments[0].get_value()->is<Replication>()) size = assignments[0].get_value()->as<Replication>().get_item()->get_size();
        else size = assignments[0].get_value()->get_size();
        std::vector<int64_t> packing_sizes(components.size(), size);
        return pack_values(components, packing_sizes);
    } else {
        return raw_value;
    }
}

std::optional<resolved_parameter> HDL_function_call::evaluate_vector(const std::map<qualified_identifier, resolved_parameter> &context) {
    std::vector<hdl_integer> loop_indexes;
    if(loop_metadata.has_value()) {
        loop_indexes = loop_solver::solve_loop(loop_metadata.value(), context);

        qualified_identifier loop_var = loop_metadata.value().get_init().get_identifier();
    } else {
        loop_indexes = {};
    }
    std::vector<hdl_integer> values(assignments.size() + loop_indexes.size());
    std::vector<int64_t> value_sizes(assignments.size() + loop_indexes.size());
    for(auto &a:assignments) {
        auto idx = a.get_index().value()->evaluate(context);
        if(!idx.has_value()) return std::nullopt;
        if(!idx.value().is_integer()) return std::nullopt;
        auto idx_val = idx.value().get_integer();
        auto value = a.get_value()->evaluate(context);
        value_sizes[idx_val.get_value()] = a.get_value()->get_size();
        if(!value.has_value()) return std::nullopt;
        values[idx_val.get_value()] = value.value().get_integer();
    }

    if(loop_metadata.has_value()) {
        qualified_identifier loop_var =  loop_metadata.value().get_init().get_identifier();
        auto loop_assignments = loop_metadata.value().get_assignments();
        for(int i = 0; i<loop_assignments.size(); i++) {
            for(auto &l:loop_indexes) {
                auto la = loop_assignments[i] ;
                auto ctx = context;
                ctx[loop_var] = resolved_parameter(l);
                auto idx_opt = la.get_index();
                if(!idx_opt.has_value()) continue;
                auto idx = idx_opt.value()->evaluate(ctx);
                if(!idx.has_value()) continue;
                if(!idx.value().is_integer()) continue;
                auto idx_val = idx.value().get_integer();
                auto var = la.get_value()->evaluate(ctx);
                if(!var.has_value()) continue;
                value_sizes[idx_val.get_value()] = var.value().get_integer().get_size();
                values[idx_val.get_value()] = var.value().get_integer();
            }
        }
    }

    apply_return_order_reversal(values, value_sizes, context);

    mdarray<hdl_integer> result;
    result.set_1d_slice({0, 0}, values);
    if (packing) {
        result.set_1d_slice({0,0}, {pack_values(values, value_sizes)});
    }
    return result;
}

void HDL_function_call::apply_return_order_reversal(
    std::vector<hdl_integer> &values,
    std::vector<int64_t> &value_sizes,
    const std::map<qualified_identifier, resolved_parameter> &context
) {
    if (packing || !has_return_unpacked_ascending) return;
    if (return_unpacked_ascending != container_unpacked_ascending) {
        std::reverse(values.begin(), values.end());
        std::reverse(value_sizes.begin(), value_sizes.end());
    }
}

std::optional<resolved_parameter> HDL_function_call::evaluate_system_task(const std::map<qualified_identifier, resolved_parameter> &context) {
    std::string task_name = function_name.substr(1, function_name.size()-1);
    std::vector<resolved_parameter> resolved_arguments;
    for (auto &arg:arguments) {
        auto resolved_val = arg->evaluate(context);
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

void HDL_function_call::set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context) {
    packing = s.unpacked_sizes.empty();
    container_unpacked_ascending = s.unpacked_ascending.empty() ? true : s.unpacked_ascending[0];
    if (s.packed_sizes.empty() && s.unpacked_sizes.empty()) {
        if (assignments.size()==1 && !loop_metadata.has_value()){
            assignments[0].set_container_size(s, context);
        }
        return;
    }
    if (s.unpacked_sizes.empty()) {
        if (assignments.size()==1 && !loop_metadata.has_value()){
            assignments[0].set_container_size(s, context);
        } else {
            resolved_type lower_container_size;
            lower_container_size.packed_sizes.insert(
                lower_container_size.packed_sizes.begin(),
                s.packed_sizes.begin(),
                s.packed_sizes.end()-1
            );
            lower_container_size.packed_ascending.insert(
                lower_container_size.packed_ascending.begin(),
                s.packed_ascending.begin(),
                s.packed_ascending.end()-1
            );
            for (auto &a:assignments) {
                a.set_container_size(lower_container_size, context);
            }
            if (loop_metadata) {
                std::vector<assignment> new_assignments;
                for (auto &a:loop_metadata->get_assignments()) {
                    a.set_container_size(lower_container_size, context);
                    new_assignments.push_back(a);
                }
                loop_metadata->set_assignments(new_assignments);
            }
        }
    } else {
        if (assignments.size()==1 && !loop_metadata.has_value()) {
            assignments[0].set_container_size(s, context);
        } else {
            resolved_type lower_container_size;
            lower_container_size.packed_sizes = s.unpacked_sizes;
            lower_container_size.packed_ascending = s.unpacked_ascending;
            lower_container_size.unpacked_sizes.insert(
                lower_container_size.unpacked_sizes.begin(),
                s.unpacked_sizes.begin(),
                s.unpacked_sizes.end()-1
            );
            lower_container_size.unpacked_ascending.insert(
                lower_container_size.unpacked_ascending.begin(),
                s.unpacked_ascending.begin(),
                s.unpacked_ascending.end()-1
            );
            for (auto &a:assignments) {
                a.set_container_size(lower_container_size, context);
            }
            if (loop_metadata) {
                std::vector<assignment> new_assignments;
                for (auto &a:loop_metadata->get_assignments()) {
                    a.set_container_size(lower_container_size, context);
                    new_assignments.push_back(a);
                }
                loop_metadata->set_assignments(new_assignments);
            }
        }

    }
    if (s.return_unpacked_ascending.has_value()) {
        return_unpacked_ascending = s.return_unpacked_ascending.value();
        has_return_unpacked_ascending = true;
    }
}


std::string HDL_function_call::print() const {
    std::ostringstream result;
    if (!package_prefix.empty()) result<< package_prefix << "::";
    result << function_name << "(";
    for(int i = 0; i< arguments.size(); i++) {
        result << arguments[i]->print();
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

bool HDL_function_call::isEqual(const Expression_base &other) const {
    bool is_equal = true;


    const auto& rhs = static_cast<const HDL_function_call&>(other);
    is_equal &= function_name == rhs.function_name;
    if (arguments.size() != rhs.arguments.size()) return false;
    for (int i = 0; i< arguments.size(); i++) {
        is_equal &= *arguments[i] == *rhs.arguments[i];
    }
    is_equal &= loop_metadata == rhs.loop_metadata;
    is_equal &= assignments == rhs.assignments;
    is_equal &= package_prefix == rhs.package_prefix;
    return is_equal;
}
