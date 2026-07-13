//  Copyright 2023 Filippo Savi
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

#include "data_model/HDL/parameters/HDL_parameter.hpp"


HDL_parameter::HDL_parameter(const HDL_parameter &c) {

    name = c.name;
    type = c.type;
    solved_value = c.solved_value;

    raw_value = c.raw_value;
    return_unpacked_range_left = c.return_unpacked_range_left;
    return_unpacked_range_right = c.return_unpacked_range_right;
}


bool operator==(const HDL_parameter &lhs, const HDL_parameter &rhs) {
    bool ret = true;

    ret &= lhs.name == rhs.name;
    if (!lhs.solved_value.has_value() || !rhs.solved_value.has_value()) {
        if (lhs.raw_value && rhs.raw_value) {
            ret &= *lhs.raw_value == *rhs.raw_value;
        } else {
            ret &= lhs.raw_value == rhs.raw_value;
        }
    }

    if (lhs.type && rhs.type) {
        ret &= lhs.type->is_equal(*rhs.type);
    } else {
        ret &= lhs.type == rhs.type;
    }

    ret &= lhs.solved_value == rhs.solved_value;

    return ret;
}

bool operator<(const HDL_parameter &lhs, const HDL_parameter &rhs) {
    return lhs.get_name()<rhs.get_name();
}



std::optional<resolved_parameter> HDL_parameter::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    if (!type) return std::nullopt;
    std::optional<resolved_type> container_size;
    if (type->is<HDL_external_type>()) {
        int i = 0;
    } else {
        container_size = type->evaluate_type(context);
    }
    if (!container_size) return std::nullopt;
    if (return_unpacked_range_left && return_unpacked_range_right) {
        auto lower = return_unpacked_range_left->evaluate(context);
        auto upper = return_unpacked_range_right->evaluate(context);
        if (lower && upper && lower->is_integer() && upper->is_integer()) {
            container_size->return_unpacked_ascending = lower->get_integer() < upper->get_integer();
        }
    }
    raw_value->set_container_sizes(container_size.value(), context);
    auto val = raw_value->evaluate(context);
    if (type->is<HDL_simple_type>()) {
        return cast_result(val, container_size);
    } else {
        return val;
    }
}

std::optional<resolved_parameter> HDL_parameter::cast_result(
    const std::optional<resolved_parameter> &in,
    const std::optional<resolved_type> &sizes
    ) {
    auto t = type->as<HDL_simple_type>();
    if (in->is_integer()) {
        if (t.get_signed() && !in->get_integer().get_signed()) {
            if (sizes->packed_sizes[0] == 32) {
                uint32_t truncated_val = in->get_integer().get_value() & 0xFFFFFFFF;
                return static_cast<int32_t>(truncated_val);
            }
        }
        if (t.get_signed() && !in->get_integer().get_signed()) {
            int i = 0;
        }
    }
    return in;

}


void HDL_parameter::propagate_function(const HDL_function_def &def) {
    if (raw_value) raw_value->propagate_function(def);
    auto call = std::dynamic_pointer_cast<HDL_function_call>(raw_value);
    if (call && call->get_name() == def.get_name()) {
        return_unpacked_range_left = def.get_return_unpacked_range_left();
        return_unpacked_range_right = def.get_return_unpacked_range_right();
    }
}

HDL_parameter::operator std::string() {
    if (!solved_value.has_value()) return "";
    std::string ret_val;
    if(solved_value.value().is_string()) {
        ret_val = solved_value.value().get_string();
    }
    if(solved_value.value().is_integer()) {
        auto val = solved_value.value().get_integer();
        ret_val =  std::to_string(val);
    }
    if(solved_value.value().is_int_array()) {
        ret_val = "array value";
    }

    return ret_val;

}


void PrintTo(const HDL_parameter &param, std::ostream *os) {
    std::string result = param.to_string();
    *os << result;
}

std::string HDL_parameter::to_string() const {
    std::string result = name;

   if (type) result += type->to_print();

    if (raw_value) {
        result += " = " + raw_value->print();
    } else {
        result += " = !!no expression!!";
    }

    if (solved_value.has_value()) {
        result += "  [solved: ";
        std::ostringstream ss;
        ss << solved_value.value();
        result += ss.str();
        result += "]";
    }

    return result;
}

parameter_deps_t HDL_parameter::get_dependencies() {
    parameter_deps_t result;
    if (type) result.merge(type->get_dependencies());

    if (raw_value)  result.merge(raw_value->get_dependencies());
    return result;
}


nlohmann::json HDL_parameter::dump() {
    nlohmann::json ret;


    ret["name"] = name;

    if (!solved_value.has_value())  ret["type"] = "unknown parameter type";
    else if (solved_value.value().is_string()) {
        ret["type"] = "string_parameter";
        std::vector<std::string> values_s;
        auto value = solved_value.value();
        std::string str_val = solved_value.value().get_string();
        if (str_val.size() >= 2 && str_val.front() == '"' && str_val.back() == '"') {
            str_val = str_val.substr(1, str_val.size() - 2);
        }
        values_s.push_back(str_val);
        ret["value"] = values_s;
    }
    else if (solved_value.value().is_integer()) {
        ret["type"] = "numeric_parameter";
        ret["value"]= std::vector<int64_t>();
        if(solved_value.has_value()) ret["value"].push_back(solved_value.value().get_integer().get_value());
    }
    else if (solved_value.value().is_int_array()) {
        ret["type"] = "array_parameter";
        ret["value"] = solved_value.value().get_int_array().dump();
    }

    return ret;
}
