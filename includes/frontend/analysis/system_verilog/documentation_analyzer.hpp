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

#ifndef ANANKE_DOCUMENTATION_ANALYZER_HPP
#define ANANKE_DOCUMENTATION_ANALYZER_HPP

#include <sstream>
#include <utility>
#include <spdlog/spdlog.h>
#include <ctre.hpp>
#include <nlohmann/json.hpp>

#include "data_model/documentation/module_documentation.hpp"
#include "data_model/documentation/register_documentation.hpp"
#include "data_model/documentation/processor_instance.hpp"
#include "data_model/documentation/channel_group.hpp"

#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/parameters/Parameters_map.hpp"

static constexpr auto doc_comment_pattern = ctre::search<R"(/\*\*.*?\*\*/)">;

class documentation_analyzer {
public:
    explicit documentation_analyzer(const std::vector<std::string> &comments);
    void set_source_path(const std::string &f_path) {path = f_path;};
    void process_documentation(Parameters_map parameters);
    std::unordered_map<std::string, module_documentation> get_modules_documentation();
    std::unordered_map<std::string, processor_instance> get_processors_documentation();
    std::unordered_map<std::string, std::vector<channel_group>> get_channel_groups() const{return groups;};
private:
    void analyze_documentation_object(nlohmann::json &obj);
    void analyze_channel_groups(nlohmann::json &obj);
    // BUS STRUCTURE DOCUMENTATION
    field_documentation analyze_register_field(nlohmann::json &obj, bool parametric);
    // PERIPHERAL DOCUMENTATION
    void analyze_peripheral(nlohmann::json &obj);
    void analyze_parametric_peripheral(nlohmann::json &obj);
    void analyze_variant_peripheral(nlohmann::json &obj);
    // PROCESSORS DOCUMENTATION
    void analyze_processor_instance(nlohmann::json &obj);

    Parameters_map parameters_dict;

    std::vector<std::string> raw_documentation_comments;

    std::unordered_map<std::string, module_documentation> modules_doc;
    std::unordered_map<std::string, processor_instance> processors;
    std::unordered_map<std::string, std::vector<channel_group>> groups;

    std::string path;
};


#endif //ANANKE_DOCUMENTATION_ANALYZER_HPP
