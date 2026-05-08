// Copyright 2021 Filippo Savi
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

#ifndef ANANKE_PERIPHERAL_DEFINITION_GENERATOR_HPP
#define ANANKE_PERIPHERAL_DEFINITION_GENERATOR_HPP

#include <memory>
#include <nlohmann/json.hpp>

#include "data_model/HDL/HDL_instance_AST.hpp"
#include "data_model/data_store.hpp"
#include "../../data_model/Depfile/Depfile.hpp"

class peripheral_definition_generator {
public:
    peripheral_definition_generator(std::shared_ptr<data_store> &d, const std::shared_ptr<HDL_instance_AST> &l);

    nlohmann::json get_peripheral_definitions() {return peripheral_defs;};
    std::map<std::string, std::string> get_alias_map() const{return alias_map;};
    void write_definition_file(const std::string &path);
    std::string get_definition_string();
    std::unordered_map<std::string, std::string>  get_variant_peripherals() {return variant_peripherals;};
private:

    void generate_peripheral(const HDL_Resource &res,Parameters_map &parameters, const std::string& inst_name);
    nlohmann::json generate_field(field_documentation &doc, const Parameters_map &parameters);
    std::vector<nlohmann::json> generate_simple_module_registers(const std::vector<register_documentation> &r);
    std::vector<nlohmann::json> generate_parametric_module_registers(const std::vector<register_documentation> &r, const Parameters_map &parameters);
    std::vector<nlohmann::json> generate_variant_module_registers(
            const std::vector<register_documentation> &r,
            const Parameters_map &parameters,
            const std::string &variant);

    std::string ver;
    std::shared_ptr<data_store> d_store;
    std::vector<HDL_Resource> submodules_to_generate;

    std::unordered_map<std::string, std::string> variant_peripherals;

    struct proxy_periph_t{
        nlohmann::json definitions;
        std::map<std::string, std::string> alias_map;
        std::unordered_map<std::string, std::string> variant_peripherals;
    };

    std::vector<proxy_periph_t> proxy_defs;

    std::map<std::string, std::string> alias_map;

    nlohmann::json peripheral_defs;
};


#endif //ANANKE_PERIPHERAL_DEFINITION_GENERATOR_HPP
