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

#include "data_model/HDL/HDL_net.hpp"

std::string HDL_net::get_full_name() const {
    auto base_name = name;
    if(!range.accessor.empty()) {
        base_name += "[" + range.accessor.print();
        if(!range.range.empty()) {
            if(range.type ==HDL_range::increasing_range) base_name += "+";
            else if(range.type ==HDL_range::decreasing_range) base_name += "-";
            base_name += ":" + range.range.print();
        }
        base_name += "]";
    } else if(!index.empty()) {
        base_name += "[" + index.print() + "]";
    }
    return base_name;

}

void HDL_net::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    auto val = range.accessor.evaluate(context);
    if(val.has_value()) {
        if(val.value().get_integer().get_value()) {
            range.accessor = {Expression_component(val.value().get_integer(), 0)};
        }
    }
    val = range.range.evaluate(context);
    if(val.has_value()) {
        if(val.value().is_integer()) {
            range.range = {Expression_component(val.value().get_integer(), 0)};
        }
    }
    index.evaluate(context);
    val = index.evaluate(context);
    if(val.has_value()) {
        if(val.value().is_integer()) {
            index = {Expression_component(val.value().get_integer(), 0)};
        }
    }
    val = replication.size.evaluate(context);
    if(val.has_value()) {
        if(val.value().is_integer()) {
            replication.size = {Expression_component(val.value().get_integer(), 0)};
        }
    }
    val = replication.target.evaluate(context);
    if(val.has_value()) {
        if(val.value().is_integer()) {
            replication.target = {Expression_component(val.value().get_integer(), 0)};
        }
    }
}

void HDL_net::propagate_constant(const qualified_identifier &id, const resolved_parameter &param) {

    range.accessor.propagate_constant(id, param);
    range.range.propagate_constant(id, param);
    index.propagate_constant(id, param);
    replication.size.propagate_constant(id, param);
    replication.target.propagate_constant(id, param);
}

nlohmann::json HDL_net::dump() {
    nlohmann::json ret;
    ret["name"] = name;
    return ret;
}
