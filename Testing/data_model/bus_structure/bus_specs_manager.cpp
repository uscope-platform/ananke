// Copyright 2023 Filippo Savi
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

#include <gtest/gtest.h>

#include "data_model/bus_mapping/bus_specs_manager.hpp"

TEST( bus_specs_manager, parsing) {
    bus_specs_manager manager("axi_lite", TEST_SPECS_FOLDER);
    auto res = manager.get_bus_specs();

    std::vector<mapper_bus_component> check_vect;
    auto spec = nlohmann::json::parse(R"({"class":"interconnect","type":"n2m","n_sources":"NM","n_sinks":"NS","in_port":"slaves","out_port":"masters"})");
    check_vect.emplace_back("axil_crossbar_interface", spec);

    spec = nlohmann::json::parse(R"({"class":"sink","type":"cu","in_port":"axi_in","out_port":"axi_out"})");
    check_vect.emplace_back("axil_external_registers_cu", spec);

    spec = nlohmann::json::parse(R"({"class":"sink","type":"cu","in_port":"axi_in","out_port":"axi_out"})");
    check_vect.emplace_back("axil_simple_register_cu", spec);

    spec = nlohmann::json::parse(R"({"class":"source","type":"bridge","in_port":"axi_in","out_port":"axi_out"})");
    check_vect.emplace_back("axis_to_axil", spec);


    ASSERT_EQ(check_vect, res);
}


