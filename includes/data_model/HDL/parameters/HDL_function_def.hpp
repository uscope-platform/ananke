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

#ifndef ANANKE_HDL_FUNCTION_HPP
#define ANANKE_HDL_FUNCTION_HPP

#include <string>
#include <vector>
#include<set>


#include "data_model/HDL/HDL_loop.hpp"


class HDL_function_def {
public:

    HDL_function_def clone();

    void set_name(const std::string &s) { name = s;}
    void add_argument(const std::string &s){argument_names.push_back(s);}
    void start_assignment(const std::string &n, const std::shared_ptr<Parameter_value_base> & idx);
    void close_assignment(const std::shared_ptr<Parameter_value_base> &val);
    void add_assignment(const assignment &a){assignments.push_back(a);}
    void add_loop_metadata(const HDL_loop_metadata &l){loop_metadata = l;}
    bool is_scalar() const;
    std::vector<assignment> get_assignments()const{ return assignments;}
    std::vector<std::string> get_arguments_names()const{return  argument_names;}
    std::optional<HDL_loop_metadata> get_loop()const{ return loop_metadata;}
    bool operator==(const HDL_function_def &rhs) const;


    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, assignments, loop_metadata, argument_names);
    }
    std::string name;
private:
    std::vector<assignment> assignments;
    std::optional<HDL_loop_metadata> loop_metadata;
    std::vector<std::string> argument_names;
};



#endif //ANANKE_HDL_FUNCTION_HPP
