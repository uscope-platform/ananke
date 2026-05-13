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
    i_l = c.i_l.clone();
}
bool operator==(const HDL_parameter &lhs, const HDL_parameter &rhs) {
    bool ret_val = true;
    ret_val &= lhs.name == rhs.name;
    ret_val &= lhs.i_l == rhs.i_l;
    return ret_val;
}

bool operator<(const HDL_parameter &lhs, const HDL_parameter &rhs) {
    return lhs.name<rhs.name;
}

void HDL_parameter::set_value(const resolved_parameter &val) {
    i_l.set_solved_value(val);
}


std::shared_ptr<HDL_parameter> HDL_parameter::clone() const {
    return std::make_shared<HDL_parameter>(*this);
}

bool HDL_parameter::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &constant_value) {
    return i_l.propagate_constant(constant_id, constant_value);
}

void HDL_parameter::propagate_function(const HDL_function_def &def) {
    i_l.propagate_function(def);
}


HDL_parameter::operator std::string() {
    if (!i_l.get_solved_value().has_value()) return "";
    std::string ret_val;
    if(i_l.get_type() == Initialization_list::string_parameter) {
        ret_val = std::get<std::string>(i_l.get_solved_value().value());
    }
    if(i_l.get_type() == Initialization_list::numeric_parameter) {
        auto val = std::get<mdarray<int64_t>>(i_l.get_solved_value().value()).get_scalar();
        if(val.has_value()) ret_val =  std::to_string(val.value());
    }
    if(i_l.get_type() == Initialization_list::array_parameter) {
        ret_val = "array value";
    }

    return ret_val;

}

void HDL_parameter::add_component(const Expression_component& component) {
    i_l.push_scalar_component(component);
}

void PrintTo(const HDL_parameter &param, std::ostream *os) {
    std::string result = param.to_string();
    *os << result;
}

std::string HDL_parameter::value_as_string() const {
    if(i_l.get_type() == Initialization_list::string_parameter) {
        return std::get<std::string>(i_l.get_solved_value().value());
    }
    if(i_l.get_type() == Initialization_list::numeric_parameter) {
        auto val = std::get<mdarray<int64_t>>(i_l.get_solved_value().value()).get_scalar();
        if(val.has_value()) return std::to_string(val.value());
        else return "";
    }
    if(i_l.get_type() == Initialization_list::array_parameter) {
        return std::get<mdarray<int64_t>>(i_l.get_solved_value().value()).to_string();
    }
    return "";
}

std::string HDL_parameter::to_string() const {
    std::string result = "\nHDL parameter:\n  NAME: " + name +
                         "\n  TYPE: " + parameter_type_to_string(i_l.get_type());

    if(i_l.get_type() == Initialization_list::numeric_parameter){
        auto val = std::get<mdarray<int64_t>>(i_l.get_solved_value().value()).get_scalar();
        if(val.has_value()) result += "\n  VALUE: " + std::to_string(val.value());
        else result += "\n  VALUE: xxx";
    }


    result += "\n  INITIALIZATION LIST:\n    ";

    std::ostringstream s;
    PrintTo(i_l,&s);
    result += s.str();

    return result;
}

std::set<qualified_identifier> HDL_parameter::get_dependencies() {
    return i_l.get_dependencies();
}

nlohmann::json HDL_parameter::dump() {
    nlohmann::json ret;

    ret["name"] = name;
    ret["type"] = parameter_type_to_string(i_l.get_type());
    if(i_l.get_type() == Initialization_list::string_parameter){
        std::vector<std::string> values_s;
        auto value = i_l.get_solved_value().value();
        values_s.push_back(std::regex_replace(std::get<std::string>(value), std::regex(R"ctrl(^"(.*)"$)ctrl"), "$1"));
        ret["value"] = values_s;
    } else if(i_l.get_type() == Initialization_list::numeric_parameter){
        ret["value"]= std::vector<int64_t>();
        auto val = i_l.get_solved_value();
        if(val.has_value()) ret["value"].push_back(std::get<int64_t>(val.value()));

    } else if(i_l.get_type() == Initialization_list::array_parameter){
        ret["value"] = std::get<mdarray<int64_t>>(i_l.get_solved_value().value()).dump();
    }



    return ret;
}

