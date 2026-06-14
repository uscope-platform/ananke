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


std::shared_ptr<HDL_parameter> HDL_parameter::clone() const {
    HDL_parameter par;
    par.name = name;
    par.type = type;
    par.solved_value = solved_value;
    par.raw_value = raw_value;
    return std::make_shared<HDL_parameter>(par);
}


std::optional<resolved_parameter> HDL_parameter::evaluate(const std::map<qualified_identifier, resolved_parameter> &context) {
    if (!type) return std::nullopt;
    auto container_size = type->evaluate_type(context);
    if (!container_size) return std::nullopt;
    raw_value->set_container_sizes(container_size.value(), context);

    return raw_value->evaluate(context);
}


void HDL_parameter::propagate_function(const HDL_function_def &def) {
    if (raw_value) raw_value->propagate_function(def);
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

void HDL_parameter::add_component(const Token &component) {
    if (!raw_value) raw_value = std::make_shared<Expression>();
    if(raw_value->is<Expression>()) {
         raw_value->as<Expression>().push_back(component);
    }
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

std::set<qualified_identifier> HDL_parameter::get_dependencies() {
    std::set<qualified_identifier> result;
    if (type) {
        result = type->get_dependencies();
    }

    if (raw_value) {
        std::set<qualified_identifier> deps = raw_value->get_dependencies();
        result.insert(deps.begin(), deps.end());
    }
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
