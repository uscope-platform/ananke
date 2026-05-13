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
    i_l = c.i_l.clone();
}
bool operator==(const HDL_parameter &lhs, const HDL_parameter &rhs) {
    bool ret_val = true;
    ret_val &= lhs.i_l == rhs.i_l;
    return ret_val;
}

bool operator<(const HDL_parameter &lhs, const HDL_parameter &rhs) {
    return lhs.get_name()<rhs.get_name();
}


std::shared_ptr<HDL_parameter> HDL_parameter::clone() const {
    return std::make_shared<HDL_parameter>(*this);
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


void PrintTo(const HDL_parameter &param, std::ostream *os) {
    std::string result = param.to_string();
    *os << result;
}


std::string HDL_parameter::to_string() const {
    std::string result = "\nHDL parameter:\n  NAME: " + i_l.get_name() +
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
