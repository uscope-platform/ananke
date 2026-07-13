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
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"

HDL_net::HDL_net() = default;

HDL_net::HDL_net(const std::string &s) {
    name = s;
}

std::string HDL_net::get_full_name() const {
    auto base_name = name;
    if(range.accessor.get_lhs() != nullptr) {
        base_name += "[" + range.accessor.print();
        if(range.range.get_lhs() != nullptr) {
            if(range.type ==HDL_range::increasing_range) base_name += "+";
            else if(range.type ==HDL_range::decreasing_range) base_name += "-";
            base_name += ":" + range.range.print();
        }
        base_name += "]";
    } else if(!index.empty()) {
        for (auto &idx : index) {
            base_name += "[" + idx.print() + "]";
        }
    }
    return base_name;

}

void HDL_net::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    auto val = range.accessor.evaluate(context);
    if(val.has_value()) {
        if(val.value().get_integer().get_value()) {
            Expression_v2 acc;
            acc.set_lhs(std::make_shared<Numeric_token>(val.value().get_integer(), 0));
            range.accessor = acc;
        }
    }
    val = range.range.evaluate(context);
    if(val.has_value()) {
        if(val.value().is_integer()) {
            Expression_v2 rng;
            rng.set_lhs(std::make_shared<Numeric_token>(val.value().get_integer(), 0));
            range.range = rng;
        }
    }
    for (auto &idx : index) {
        val = idx.evaluate(context);
        if(val.has_value()) {
            if(val.value().is_integer()) {
                Expression_v2 idx_expr;
                idx_expr.set_lhs(std::make_shared<Numeric_token>(val.value().get_integer(), 0));
                idx = idx_expr;
            }
        }
    }
    val = replication.size.evaluate(context);
    if(val.has_value()) {
        if(val.value().is_integer()) {
            Expression_v2 sz;
            sz.set_lhs(std::make_shared<Numeric_token>(val.value().get_integer(), 0));
            replication.size = sz;
        }
    }
    val = replication.target.evaluate(context);
    if(val.has_value()) {
        if(val.value().is_integer()) {
            Expression_v2 tgt;
            tgt.set_lhs(std::make_shared<Numeric_token>(val.value().get_integer(), 0));
            replication.target = tgt;
        }
    }
}

nlohmann::json HDL_net::dump() {
    nlohmann::json ret;
    ret["name"] = name;
    return ret;
}
