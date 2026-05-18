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


#include "data_model/HDL/parameters/Concatenation.hpp"
#include <cereal/types/polymorphic.hpp>
#include <cereal/archives/binary.hpp>

CEREAL_REGISTER_TYPE(Concatenation)
CEREAL_REGISTER_POLYMORPHIC_RELATION(Parameter_value_base, Concatenation)

Concatenation::Concatenation(const Concatenation &other) {
    for(auto &item: other.components) {
        components.push_back(item->clone_ptr());
    }
    type = concatenation;
}

Concatenation::Concatenation(Concatenation &&other) noexcept {
    for(const auto &item: other.components) {
        components.push_back(item->clone_ptr());
    }
    type = concatenation;
}

Concatenation Concatenation::clone()  const{
    Concatenation ret;
    for(auto &c : components) {
        ret.add_component(c->clone_ptr());
    }
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

bool Concatenation::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value) {
   bool retval = true;
    for (auto &comp:components) {
        retval &= comp->propagate_constant(constant_id, value);
    }
    return retval;
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

std::optional<resolved_parameter> Concatenation::evaluate(bool pack_result){
    auto concat_size = components.size();
    auto depth = get_depth();
    if (pack_result && depth == 1) {
        std::vector<int64_t> sizes(concat_size);
        std::vector<int64_t> values(concat_size);
        for (int i = 0;i<concat_size; i++) {

            auto value_opt = components[concat_size-i-1]->evaluate(false);
            sizes[i] = components[concat_size-i-1]->get_size();
            if (!value_opt.has_value()) return std::nullopt;
            auto raw_value = value_opt.value();
            if (!std::holds_alternative<int64_t>(raw_value)) throw std::runtime_error("packing concatenations of arrays is unsupported");
            values[i] = std::get<int64_t>(raw_value);
        }
        return pack_values(values, sizes);
    } else {
        if (components.empty())return std::nullopt;
        auto v = components[0]->evaluate(pack_result);
        if (std::holds_alternative<std::string>(v.value())) {
            mdarray<std::string> result;
            for (int64_t i = 0;i<concat_size; i++) {
                auto value_opt = components[concat_size-i-1]->evaluate(pack_result);
                if (!value_opt.has_value()) return std::nullopt;
                mdarray<std::string> to_concat;
                to_concat.set_value(0,std::get<std::string>(value_opt.value()));
                result = mdarray<std::string>::concatenate(result, to_concat).value();
            }
            return result;
        } else {
            mdarray<int64_t> result;
            for (int64_t i = 0;i<concat_size; i++) {
                auto value_opt = components[concat_size-i-1]->evaluate(pack_result);
                if (!value_opt.has_value()) return std::nullopt;
                if (std::holds_alternative<int64_t>(value_opt.value())) {
                    mdarray<int64_t> to_concat;
                    to_concat.set_value(0,std::get<int64_t>(value_opt.value()));
                    result = mdarray<int64_t>::concatenate(result, to_concat).value();
                } else {
                    auto array_res = std::get<mdarray<int64_t>>(value_opt.value());
                    if( depth ==1)
                        result= mdarray<int64_t>::concatenate(result, array_res).value();
                    else
                        result= mdarray<int64_t>::stack(result, array_res).value();
                }
            }
            return result;
        }

    }
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

int64_t Concatenation::get_depth() {
    int64_t ret = 1;
    for(auto &comp:components){
        if(comp->is_concatenation() || comp->is_replication()) {
           auto child_ret = comp->get_depth();
           ret = std::max(ret, child_ret+1);
        }

    }
    return ret;
}


int64_t Concatenation::pack_values(const std::vector<int64_t> &components, std::vector<int64_t> &sizes) {
    int64_t total_size = 0;
    for(auto &size:sizes){
        total_size += size;
    }
    std::vector<bool> result(total_size, false);

    uint64_t current_wp = 0;
    for(ssize_t i =0; i<components.size(); i++){
        std::bitset<64> data = components[i];
        auto size = sizes[i];
        for(int j = 0; j<size; j++){
            result[current_wp] = data[j];
            current_wp++;
        }
    }

    int64_t packed_result = 0;
    for(int i = 0; i<result.size(); i++){
        packed_result += result[i]*std::pow(2, i);
    }

    return packed_result;
}
