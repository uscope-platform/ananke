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

#include "frontend/Repository_walker.hpp"
#include <gtest/gtest.h>


std::string repw_settings_path = "/tmp/test_settings_store";
auto repw_settings_file = repw_settings_path + "/settings";

std::shared_ptr<settings_store> repw_setup_settings() {
    std::filesystem::create_directories(repw_settings_path);
    std::ofstream ofs(repw_settings_file);

    ofs << R"({"profiles": {"test_profile": {"hdl_store":"repository_walker"}}})";
    ofs.flush();
    ofs.close();
    std::error_code ec;
    std::filesystem::resize_file(repw_settings_file, std::filesystem::file_size(repw_settings_file, ec), ec);
    return std::make_shared<settings_store>(false, repw_settings_path, "test_profile");
}

void repw_clean_settings() {
    std::filesystem::remove_all(repw_settings_file);
    std::filesystem::remove_all(repw_settings_path);
}


class repository_walker : public ::testing::Test {
protected:

    void SetUp() {
        s_store = repw_setup_settings();
        d_store = std::make_shared<data_store>(true,"/tmp/test_data_store");
    }

    virtual void TearDown() {
        repw_clean_settings();
    }
    std::shared_ptr<data_store> d_store;
    std::shared_ptr<settings_store> s_store;
};




TEST_F(repository_walker , directory_analysis) {

    Repository_walker walker(s_store,d_store, false,{"repository_walker/ignored_dir","repository_walker/ignored_dir_2" });
    auto hdl_results = d_store->get_hdl_cache();
    auto script_results = d_store->get_scripts_cache();
    auto constraints_results = d_store->get_constraints_cache();
    auto data_results = d_store->get_data_cache();

    // NEW CHECKS

    auto file_name = "repository_walker/data.dat";
    auto d = d_store->get_file<DataFile>(file_name);
    DataFile check_d("data", "repository_walker/data.dat");
    ASSERT_TRUE(d.has_value());
    ASSERT_EQ(d.value(), check_d);


    file_name = "repository_walker/script_1.tcl";
    script_specs s;
    s.name = "script_1";
    s.type = "tcl";
    auto check_s  = Script(s);
    check_s.set_path(file_name);
    auto s1 = d_store->get_file<Script>(file_name);
    ASSERT_TRUE(s1.has_value());
    ASSERT_EQ(s1.value(), check_s);

    file_name = "repository_walker/script_2.py";
    s.name = "script_2";
    s.type = "py";
    check_s = Script(s);
    check_s.set_path(file_name);

    auto s2 = d_store->get_file<Script>(file_name);
    ASSERT_TRUE(s2.has_value());
    ASSERT_EQ(s2.value(), check_s);


    file_name = "repository_walker/constraints.xdc";
    auto c = d_store->get_file<Constraints>(file_name);
    Constraints check_c("constraints");
    check_c.set_path(file_name);
    ASSERT_TRUE(c.has_value());
    ASSERT_EQ(c.value(), check_c);


    file_name = "repository_walker/test_sv_module.sv";
    auto res = d_store->get_file<std::vector<HDL_Resource>>(file_name);

    std::unordered_map<std::string, HDL_port> test_ports;

    test_ports["clock"] = {input_port};
    test_ports["reset"] = {input_port};
    test_ports["data_in"] = {interface_port, {"axi_stream", "slave"}};
    test_ports["data_out"] = {interface_port, {"axi_stream", "master"}};

    HDL_Resource sv_res;
    sv_res.set_name("Decoder");
    sv_res.set_type(module);
    sv_res.set_path(file_name);
    sv_res.set_ports(test_ports);
    sv_res.set_line_n(2);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value(), std::vector{sv_res});


    file_name = "repository_walker/test_vhdl_module.vhd";
    HDL_Resource vh_res;
    vh_res.set_name("half_adder");
    vh_res.set_type(module);
    vh_res.set_path(file_name);
    vh_res.set_line_n(4);

    res = d_store->get_file<std::vector<HDL_Resource>>(file_name);
    ASSERT_TRUE(res.has_value());
    ASSERT_EQ(res.value(), std::vector{vh_res});

}


TEST_F(repository_walker , file_type_handling) {

    //VERILOG
    ASSERT_TRUE(Repository_walker::file_is_verilog("test.sv"));
    ASSERT_TRUE(Repository_walker::file_is_verilog("test.svh"));
    ASSERT_TRUE(Repository_walker::file_is_verilog("test.v"));
    ASSERT_TRUE(Repository_walker::file_is_verilog("test.vh"));
    ASSERT_FALSE(Repository_walker::file_is_verilog("test.xx"));
    //VHDL
    ASSERT_TRUE(Repository_walker::file_is_vhdl("test.vhd"));
    ASSERT_FALSE(Repository_walker::file_is_vhdl("test.xx"));
    //SCRIPT
    ASSERT_TRUE(Repository_walker::file_is_script("test.tcl"));
    ASSERT_TRUE(Repository_walker::file_is_script("test.py"));
    ASSERT_FALSE(Repository_walker::file_is_script("test.xx"));
    //CONSTRAINT
    ASSERT_TRUE(Repository_walker::file_is_constraint("test.xdc"));
    ASSERT_FALSE(Repository_walker::file_is_constraint("test.xx"));
    //DATA
    ASSERT_TRUE(Repository_walker::file_is_data("test.dat"));
    ASSERT_TRUE(Repository_walker::file_is_data("test.mem"));
    ASSERT_FALSE(Repository_walker::file_is_data("test.xx"));
}
