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
    ret &= *lhs.raw_value == *rhs.raw_value;


    ret &= lhs.type == rhs.type;

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
    par.raw_value = raw_value->clone_ptr();
    return std::make_shared<HDL_parameter>(par);
}


void HDL_parameter::set_declared_type(const std::string &t) {
    type.set_declared_type(t);
}

void HDL_parameter::add_dimension(const dimension_t &d) {
    type.add_dimension(d);
}

std::optional<resolved_parameter> HDL_parameter::evaluate() {
    auto container_size = type.evaluate_type();
    if (!container_size) return std::nullopt;
    raw_value->set_container_sizes(container_size.value());

    return raw_value->evaluate();
}

bool HDL_parameter::propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &constant_value) {
    bool retval = true;
    retval &= type.propagate_constant(constant_id, constant_value);

    retval &=  raw_value->propagate_constant(constant_id, constant_value);
    return retval;
}

void HDL_parameter::propagate_function(const HDL_function_def &def) {
    raw_value->propagate_function(def);
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

void HDL_parameter::add_component(const Expression_component &component) {
    if (!raw_value) raw_value = std::make_shared<Expression>();
    if(raw_value->is_expression()) {
        auto expr = static_cast<Expression *>(raw_value.get());
        expr->push_back(component);
    }
}

void HDL_parameter::set_scalar(const std::shared_ptr<Parameter_value_base> &expr) {
    type.set_scalar(true);
    raw_value  = {expr};
}

void HDL_parameter::add_item(const std::shared_ptr<Parameter_value_base> &e) {
    type.set_scalar(false);
    raw_value = e;
}

std::string HDL_parameter::to_string() const {
    std::string result = "\nHDL parameter:\n  NAME: " + name +
                         "\n  TYPE: ";

    if (!solved_value.has_value()) result += "unknown parameter type";
    else if (solved_value.value().is_string()) result += "string_parameter";
    else if (solved_value.value().is_integer()) {
        result += "numeric_parameter";
        auto val = solved_value.value().get_integer();
        result += "\n  VALUE: " + std::to_string(val);
    }
    else if (solved_value.value().is_int_array()) result += "array_parameter";

    result += "\n  INITIALIZATION LIST:\n    ";

    std::ostringstream s;

    if (type.is_scalar()) {
        if (!raw_value) result += "!!EMPTY SCALAR!!";
        else  result += raw_value->print();
    }
    result += "-----------------------------\n";
    result += "-----------------------------\n";
    result += "UNPACKED DIMENSIONS\n";
    for(const auto &item:type.get_unpacked_dimensions()){
        result +="[" + item.first_bound.print() + ":" + item.second_bound.print()+ "]";
    }
    result += "PACKED DIMENSIONS\n";
    for(const auto &item:type.get_packed_dimensions()){
        result +="[" + item.first_bound.print() + ":" + item.second_bound.print()+ "]";
    }

    result += "EXPRESSION\n";
    result +=  "  " + raw_value->print() + "\n";

    result += "-----------------------------\n";
    result += "-----------------------------\n";

    return result;
}

std::set<qualified_identifier> HDL_parameter::get_dependencies() {
    std::set<qualified_identifier> result = type.get_dependencies();

    std::set<qualified_identifier> deps = raw_value->get_dependencies();
    result.insert(deps.begin(), deps.end());
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
        values_s.push_back(std::regex_replace(solved_value.value().get_string(), std::regex(R"ctrl(^"(.*)"$)ctrl"), "$1"));
        ret["value"] = values_s;
    }
    else if (solved_value.value().is_integer()) {
        ret["type"] = "numeric_parameter";
        ret["value"]= std::vector<int64_t>();
        if(solved_value.has_value()) ret["value"].push_back(solved_value.value().get_integer());
    }
    else if (solved_value.value().is_int_array()) {
        ret["type"] = "array_parameter";
        ret["value"] = solved_value.value().get_int_array().dump();
    }

    return ret;
}
