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

#include <gtest/gtest.h>
#include <fstream>

#include "frontend/analysis/system_verilog/documentation_analyzer.hpp"


TEST( documentation_analyzer , simple_peripheral) {


    std::vector<std::string> test_pattern = {
       R"(
    {
        "name": "Decoder",
        "type": "peripheral",
        "registers":[
            {
                "name": "simple_register_r",
                "offset": "0x0",
                "description": "Single word read only register",
                "direction": "R"
            },
            {
                "name": "simple_register_w",
                "offset": "4",
                "description": "Single word write only register",
                "direction": "W"
            },
            {
                "name": "field_registers",
                "offset": "0x20",
                "description": "register with multiple fields",
                "direction": "RW",
                "fields": [
                    {
                        "name":"field_1",
                        "description": "First field",
                        "start_position": 0,
                        "length": 8
                    },
                    {
                        "name":"field_2",
                        "description": "Second Field",
                        "start_position": 8,
                        "length": 8
                    }
                ]
            }
        ]
       }
)"};

    Parameters_map params;

    documentation_analyzer doc(test_pattern);
    doc.process_documentation(params);
    std::unordered_map<std::string, module_documentation> results = doc.get_modules_documentation();

    module_documentation check_doc;
    check_doc.set_name("Decoder");
    register_documentation reg_1("simple_register_r", 0, "Single word read only register", true, false);
    register_documentation reg_2("simple_register_w", 0x4, "Single word write only register", false, true);
    register_documentation reg_3("field_registers", 0x20, "register with multiple fields", true, true);
    field_documentation field_1("field_1", "First field",0,8);
    field_documentation field_2("field_2", "Second Field",8,8);
    reg_3.add_field(field_1);
    reg_3.add_field(field_2);
    check_doc.add_register(reg_1);
    check_doc.add_register(reg_2);
    check_doc.add_register(reg_3);

    std::unordered_map<std::string, module_documentation> check_map;
    check_map["Decoder"] = check_doc;

    ASSERT_EQ(check_map, results);
}



TEST( documentation_analyzer , parametric_peripheral) {

    std::string doc_comment = R"par(
       {
        "name": "ChainControlUnit",
        "type": "parametric_peripheral",
        "registers":[
            {
                "name": "tresh_$L",
                "n_regs": ["N_CHANNELS"],
                "description": "Comparator $ treshold low",
                "fields":[],
                "direction": "RW"
            },
            {
                "name": "tresh_$H",
                "n_regs": ["N_CHANNELS"],
                "description": "Comparator $ treshold high",
                "fields":[],
                "direction": "RW"
            },
            {
                "name": "deadtime_$",
                "n_regs": ["N_CHANNELS"],
                "description": "Length of deadtime automatically inserted in pair A (if enabled)",
                "fields":[],
                "direction": "RW"
            },
            {
                "name": "counter_start",
                "n_regs": ["1"],
                "description": "Start Value for the PWM generator",
                "fields":[],
                "direction": "RW"
            },
            {
                "name": "counter_stop",
                "n_regs": ["1"],
                "description": "Stop Value for the PWM generator",
                "fields":[],
                "direction": "RW"
            },
            {
                "name": "tb_shift",
                "n_regs": ["1"],
                "description": "Carrier phase shift",
                "fields":[],
                "direction": "RW"
            },
            {
                "name": "out_en",
                "n_regs": ["1"],
                "description": "Output enable register",
                "fields":[],
                "direction": "RW",
                "fields":[
                    {
                        "name":"out_$",
                        "description": "enable output pair $",
                        "start_position": 0,
                        "length": 2,
                        "n_fields": ["N_CHANNELS"]
                    }
                ]
            },
            {
                "name": "dt_en",
                "n_regs": ["1"],
                "description": "Deadtime insertion enable register",
                "direction": "RW",
                "fields":[
                    {
                        "name":"pair_$",
                        "description": "Enable deadtime insertion pair $",
                        "start_position": 0,
                        "length": 1,
                        "n_fields": ["N_CHANNELS"]
                    }
                ]
            },
            {
                "name": "ctrl",
                "n_regs": ["1"],
                "description": "Chain control register",
                "direction": "RW",
                "fields":[
                    {
                        "name":"mode",
                        "description": "Chain counter mode",
                        "start_position": 0,
                        "length": 3
                    }
                ]
            }
        ]
       }
)par";
    Parameters_map params;
    std::vector<std::string> comm = {doc_comment};
    documentation_analyzer doc(comm);
    doc.process_documentation(params);
    std::unordered_map<std::string, module_documentation> results = doc.get_modules_documentation();

    module_documentation check_doc;
    check_doc.set_name("ChainControlUnit");
    check_doc.set_parametric();

    check_doc.add_register(register_documentation("tresh_$L", "Comparator $ treshold low", true, true, {"N_CHANNELS"}));
    check_doc.add_register(register_documentation("tresh_$H", "Comparator $ treshold high", true, true, {"N_CHANNELS"}));
    check_doc.add_register(register_documentation("deadtime_$", "Length of deadtime automatically inserted in pair A (if enabled)", true, true, {"N_CHANNELS"}));
    check_doc.add_register(register_documentation("counter_start", "Start Value for the PWM generator", true, true, {"1"}));
    check_doc.add_register(register_documentation("counter_stop", "Stop Value for the PWM generator", true, true, {"1"}));
    check_doc.add_register(register_documentation("tb_shift", "Carrier phase shift", true, true, {"1"}));

    register_documentation reg("out_en", "Output enable register", true, true, {"1"});
    field_documentation f("out_$", "enable output pair $",0,2);
    f.set_n_fields({"N_CHANNELS"});
    reg.add_field(f);
    check_doc.add_register(reg);

    reg = register_documentation("dt_en", "Deadtime insertion enable register", true, true, {"1"});
    f = field_documentation("pair_$", "Enable deadtime insertion pair $",0,1);
    f.set_n_fields({"N_CHANNELS"});
    reg.add_field(f);
    check_doc.add_register(reg);

    reg = register_documentation("ctrl", "Chain control register", true, true, {"1"});
    f = field_documentation("mode", "Chain counter mode",0,3);
    reg.add_field(f);
    check_doc.add_register(reg);


    std::unordered_map<std::string, module_documentation> check_map;
    check_map["ChainControlUnit"] = check_doc;

    ASSERT_EQ(check_map, results);
}





TEST( documentation_analyzer , processor_doc) {

    std::vector<std::string> test_pattern = {
        R"(
        {
            "name": "current_controller",
            "type": "processor_instance",
            "target": "processor",
            "address": {
                "parameter":"PROCESSOR_BASE_ADDRESS"
            },
            "parent": "SicDriveMasterCurrentControl",
            "dma_io": [
                {
                    "name": "Current",
                    "type": "input",
                    "address": "1"
                },
                {
                    "name": "Speed",
                    "type": "input",
                    "address": "2"
                },
                {
                    "name": "duty",
                    "type": "output",
                    "address": "15"
                }
            ]
        }
)"};
    Parameters_map params;

    documentation_analyzer doc(test_pattern);
    doc.process_documentation(params);
    std::unordered_map<std::string, processor_instance> results = doc.get_processors_documentation();

    processor_instance check_doc;
    check_doc.set_name("current_controller");
    check_doc.set_target("processor");
    check_doc.set_address("PROCESSOR_BASE_ADDRESS");
    io a;
    a.name = "Current";
    a.type = input;
    a.address = 1;
    check_doc.add_io(a);

    a.name = "Speed";
    a.type = input;
    a.address = 2;
    check_doc.add_io(a);

    a.name = "duty";
    a.type = output;
    a.address = 15;
    check_doc.add_io(a);

    std::unordered_map<std::string, processor_instance> check_map;
    check_map["SicDriveMasterCurrentControl"] = check_doc;

    ASSERT_EQ(check_map, results);
}
