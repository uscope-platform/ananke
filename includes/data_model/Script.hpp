// Copyright 2021 University of Nottingham Ningbo China
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ANANKE_SCRIPT_HPP
#define ANANKE_SCRIPT_HPP

#include <string>
#include <vector>
#include <sstream>
#include <utility>
#include <cereal/types/vector.hpp>
#include <cereal/types/utility.hpp>
#include <cereal/types/string.hpp>
#include <nlohmann/json.hpp>
#include <nlohmann/detail/meta/std_fs.hpp>

#define SCRIPT_TCL 0
#define SCRIPT_PYTHON 1
#define SCRIPT_UNINITIALIZED 2

enum script_type_t {tcl_script=SCRIPT_TCL, python_script=SCRIPT_PYTHON, uninit_script=SCRIPT_UNINITIALIZED};

///  Templated function used to convert a resource_type_t enum instance to the underlying integer for string conversion
/// \return integer feature code
template <typename script_type_t>
auto script_to_integer(script_type_t const value)
-> typename std::underlying_type<script_type_t>::type
{
    return static_cast<typename std::underlying_type<script_type_t>::type>(value);
}

struct script_specs {
    std::string name = "";
    std::string type = "";
    std::string products_type = "";
    bool function_mode = false;
    bool include_products = false;
    std::vector<std::pair<std::string, std::string>> named_arguments = {};
    std::vector<std::string> positional_arguments = {};
};

class Script {
public:
    Script() = default;
    Script(const Script &S);
    Script(const script_specs &specs);
    std::string get_name() const;
    script_type_t get_type();
    bool get_function_mode() const {return function_mode;}
    void set_product(const bool gen,const std::string &t) {product_include = gen, product_type = t;}
    [[nodiscard]] bool get_product_include() const;
    [[nodiscard]] std::string get_product_type() const;
    void set_arguments(const std::vector<std::pair<std::string,std::string>> &args) {arguments = args;}
    void set_path(std::string p);
    std::string get_path();
    std::map<std::string,std::string> get_arguments_map();
    std::vector<std::pair<std::string,std::string>> get_arguments();

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, path, type, product_type, product_include,arguments, function_mode);
    }

    friend bool operator==(const Script&lhs, const Script&rhs);
private:
    std::string name;
    std::string path;
    bool function_mode = false;
    script_type_t type;
    std::string product_type;
    bool product_include;
    std::vector<std::pair<std::string,std::string>> arguments;
};


#endif //ANANKE_SCRIPT_HPP
