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

#ifndef ANANKE_APPLICATION_DEFINITION_GENERATOR_HPP
#define ANANKE_APPLICATION_DEFINITION_GENERATOR_HPP

#include <string>
#include <utility>
#include <nlohmann/json.hpp>

#include "data_model/documentation/channel_group.hpp"
#include "data_model/documentation/channel.hpp"
#include "data_model/Depfile/Depfile.hpp"
#include "data_model/data_store.hpp"
#include "data_model/scope.hpp"

#include "data_model/HDL/hdl_ast_node.hpp"

class application_definition_generator {
public:
    explicit application_definition_generator(
            const std::shared_ptr<hdl_ast_node> &l,
            const nlohmann::json &p,
            const std::map<std::string, std::string> &a,
            const std::unordered_map<std::string, std::string> &vm
    );
    void add_datapoints(const std::vector<channel>& dp);
    void add_channel_groups(const std::vector<channel_group> &cgs);
    void add_scope(const scope_data &sd);

    void write_definition_file(const std::string &path);
    void construct_application(const std::string &name);
    std::string get_definition_string();

    std::vector<nlohmann::json> get_peripherals(){return peripherals;};

private:
    std::map<std::string, hdl_integer> get_parameters(const nlohmann::json &spec, std::shared_ptr<hdl_ast_node> &node);
    void process_ast(const std::shared_ptr<hdl_ast_node> &l);
    void detect_scope(const std::string &s, std::vector<int64_t> addr);
    void deduplicate_peripheral_names();
    void denormalize_addresses();
    static std::string uint_to_hex(uint64_t i);

    nlohmann::json application;
    std::vector<nlohmann::json> channel_groups;
    std::vector<nlohmann::json> channels;

    std::vector<nlohmann::json> peripherals;

    std::vector<processor_instance> cores_spec;
    std::vector<nlohmann::json> cores;

    std::unordered_map<std::string, std::string> variants;

    nlohmann::json periph_defs;
    std::map<std::string, std::string> alias_map;
    scope_data scope;
};


#endif //ANANKE_APPLICATION_DEFINITION_GENERATOR_HPP
