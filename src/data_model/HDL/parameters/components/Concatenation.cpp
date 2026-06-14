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


#include "data_model/HDL/parameters/components/Concatenation.hpp"
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Concatenation)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Parameter_value_base, Concatenation)

Concatenation::Concatenation(const Concatenation &other) {

    components = other.components;
    container_size = other.container_size;
    default_initialization = other.default_initialization;
    packing = other.packing;
    unpacked_dimension = other.unpacked_dimension;
}

Concatenation::Concatenation(Concatenation &&other) noexcept {
    components = other.components;
    container_size = other.container_size;
    default_initialization = other.default_initialization;
    packing = other.packing;
    unpacked_dimension = other.unpacked_dimension;
}

Concatenation Concatenation::clone()  const{
    Concatenation ret;

    ret.components = components;
    ret.packing = packing;
    ret.default_initialization = default_initialization;
    ret.unpacked_dimension = unpacked_dimension;
    ret.container_size = container_size;
    return ret;
}

std::set<qualified_identifier> Concatenation::get_dependencies() const{
    std::set<qualified_identifier> result;
    for (auto &comp:components) {
        auto comp_deps = comp->get_dependencies();
        result.insert(comp_deps.begin(), comp_deps.end());
    }
    return result;
}

void Concatenation::propagate_expression(const qualified_identifier &constant_id,
    const std::shared_ptr<Parameter_value_base> &value) {
    for (auto &comp:components) {
        comp->propagate_expression(constant_id, value);
    }
}

void Concatenation::propagate_function(const HDL_function_def &def) {
    for (auto &comp:components) {
        comp->propagate_function(def);
    }
}

std::optional<resolved_parameter> Concatenation::evaluate(const std::map<qualified_identifier, resolved_parameter> &context){
    std::optional<resolved_parameter> result;
    auto concat_size = components.size();
    if (packing) {
        std::vector<int64_t> sizes(concat_size);
        std::vector<hdl_integer> values(concat_size);
        for (int i = 0;i<concat_size; i++) {

            auto value_opt = components[concat_size-i-1]->evaluate(context);
            if (!fields_sizes.empty()) {
                sizes[i] = 1;
                for (auto &ps : fields_sizes[concat_size-i-1].packed_sizes) sizes[i] *= ps;
            } else {
                sizes[i] = components[concat_size-i-1]->get_size();
            }
            if (!value_opt.has_value()) return std::nullopt;
            auto raw_value = value_opt.value();
            if (!raw_value.is_integer()) throw std::runtime_error("packing concatenations of arrays is unsupported");
            values[i] = raw_value.get_integer();
        }
        result = pack_values(values, sizes);
    } else {
        if (components.empty())return std::nullopt;
        auto v = components[0]->evaluate(context);
        if (v.value().is_string()) {
            mdarray<std::string> result_string;
            for (int64_t i = 0;i<concat_size; i++) {
                auto value_opt = components[concat_size-i-1]->evaluate(context);
                if (!value_opt.has_value()) return std::nullopt;
                mdarray<std::string> to_concat;
                to_concat.set_value(0,value_opt.value().get_string());
                result_string = mdarray<std::string>::concatenate(result_string, to_concat).value();
            }
            result = result_string;
        } else {
            mdarray<hdl_integer> result_array;
            for (int64_t i = 0;i<concat_size; i++) {
                auto value_opt = components[concat_size-i-1]->evaluate(context);
                if (!value_opt.has_value()) return std::nullopt;
                if (value_opt.value().is_integer()) {
                    mdarray<hdl_integer> to_concat;
                    to_concat.set_value(0,value_opt.value().get_integer());
                    result_array = mdarray<hdl_integer>::concatenate(result_array, to_concat).value();
                } else {
                    auto array_res = value_opt.value().get_int_array();
                    if( unpacked_dimension.size() ==1)
                        result_array= mdarray<hdl_integer>::concatenate(result_array, array_res).value();
                    else
                        result_array= mdarray<hdl_integer>::stack(result_array, array_res).value();
                }
            }
            result = result_array;
        }
    }
    if (default_initialization) {
        if (!result.has_value()) return result;
        auto dims = unpacked_dimension;
        while (dims.size()<3) dims.insert(dims.begin(), 1);
        if(result.value().is_int_array()) {
            auto val = result.value().get_int_array().get_scalar();
            if (!val) throw std::runtime_error("Error: initializer of default array should be defined");
            mdarray result_array = {dims, val.value()};
            return result_array;
        }
        if(result.value().is_string_array()) {
            auto val = result.value().get_string_array().get_scalar();
            if (!val) throw std::runtime_error("Error: initializer of default array should be defined");
            mdarray result_array = {dims, val.value()};
            return result_array;
        }
    }
    return result;
}

std::string Concatenation::print()  const{
    std::ostringstream oss;
    oss << "{";
    for (int i = 0; i< components.size(); i++) {
        oss << components[i]->print();
        if (components.size() == 1) break;
        if (i<components.size()-1) oss <<", ";
    }
    oss <<"}\n";
    auto dbg =  oss.str();
    return oss.str();
}


void Concatenation::set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context) {

    if (s.struct_sizes.empty()) {

        resolved_type content_sizes;
        unpacked_dimension = s.unpacked_sizes;
        if (s.packed_sizes.empty() && s.unpacked_sizes.empty()) {
            container_size = 32;
            packing = true;
            return;
        };
        if (!s.unpacked_sizes.empty()) {
            if (s.unpacked_sizes.size()>1) content_sizes.unpacked_sizes.insert(content_sizes.unpacked_sizes.end(), s.unpacked_sizes.begin(), s.unpacked_sizes.end()-1);
            content_sizes.packed_sizes = s.packed_sizes;
            container_size = s.unpacked_sizes.back();
            packing = false;
        } else {
            container_size = s.packed_sizes.back();
            packing = true;
            content_sizes.packed_sizes.insert(content_sizes.packed_sizes.end(), s.packed_sizes.begin(), s.packed_sizes.end());
        }
        for (auto &item:components) {
            item->set_container_sizes(content_sizes, context);
        }
    } else {
        packing = s.packed_struct;
        process_struct_size(s.struct_sizes, s.packed_sizes[0], context);
    }


}

void Concatenation::process_struct_size(
    const std::vector<struct_member_resolved_type> &members,
    uint64_t size,
    const std::map<qualified_identifier, resolved_parameter> &context
) {
    container_size = size;
    fields_sizes = members;
    for (int i = 0; i<members.size(); i++) {
        resolved_type rt;
        rt.packed_sizes = members[i].packed_sizes;
        rt.unpacked_sizes = members[i].unpacked_sizes;
        components[i]->set_container_sizes(rt, context);
    }
}
