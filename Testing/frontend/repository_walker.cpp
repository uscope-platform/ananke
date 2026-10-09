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

#include "test_paths.hpp"


std::string repw_settings_path = "/tmp/test_settings_store";
auto repw_settings_file = repw_settings_path + "/settings";

std::shared_ptr<settings_store> repw_setup_settings() {
    std::filesystem::create_directories(repw_settings_path);
    std::ofstream ofs(repw_settings_file);

    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\""
        << td_file("repository_walker")
        << "\"}}}";
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

    Repository_walker walker(s_store,d_store, false,{td_file("repository_walker/ignored_dir"),td_file("repository_walker/ignored_dir_2") });

    // NEW CHECKS

    auto file_name = td_file("repository_walker/data.dat");
    auto d = d_store->get_file<DataFile>(file_name);
    DataFile check_d("data", td_file("repository_walker/data.dat"));
    ASSERT_TRUE(d.has_value());
    ASSERT_EQ(d.value(), check_d);


    file_name = td_file("repository_walker/script_1.tcl");
    script_specs s;
    s.name = "script_1";
    s.type = "tcl";
    auto check_s  = Script(s);
    check_s.set_path(file_name);
    auto s1 = d_store->get_file<Script>(file_name);
    ASSERT_TRUE(s1.has_value());
    ASSERT_EQ(s1.value(), check_s);

    file_name = td_file("repository_walker/script_2.py");
    s.name = "script_2";
    s.type = "py";
    check_s = Script(s);
    check_s.set_path(file_name);

    auto s2 = d_store->get_file<Script>(file_name);
    ASSERT_TRUE(s2.has_value());
    ASSERT_EQ(s2.value(), check_s);


    file_name = td_file("repository_walker/constraints.xdc");
    auto c = d_store->get_file<Constraints>(file_name);
    Constraints check_c("constraints");
    check_c.set_path(file_name);
    ASSERT_TRUE(c.has_value());
    ASSERT_EQ(c.value(), check_c);


    file_name = td_file("repository_walker/test_sv_module.sv");
    auto content = d_store->get_file<hdl_file>(file_name)->get_content();
    auto res = content[0]->as<hdl_resource_statement>();
    std::unordered_map<std::string, HDL_port> test_ports;

    test_ports["clock"] = {input_port};
    test_ports["reset"] = {input_port};
    test_ports["data_in"] = {interface_port, {"axi_stream", "slave"}};
    test_ports["data_out"] = {interface_port, {"axi_stream", "master"}};

    hdl_resource_statement sv_res;
    sv_res.set_language(hdl_language::system_verilog);
    sv_res.set_name("Decoder");
    sv_res.set_ports(test_ports);
    sv_res.set_line_n(2);
    ASSERT_EQ(res, sv_res);


    file_name = td_file("repository_walker/test_vhdl_module.vhd");
    hdl_resource_statement vh_res;
    vh_res.set_language(hdl_language::vhdl);
    vh_res.set_name("half_adder");
    vh_res.set_line_n(4);
    std::unordered_map<std::string, HDL_port> vh_ports;
    vh_ports["i_bit1"] = {input_port};
    vh_ports["i_bit2"] = {input_port};
    vh_ports["o_sum"] = {output_port};
    vh_ports["o_carry"] = {output_port};
    vh_res.set_ports(vh_ports);

    res = d_store->get_file<hdl_file>(file_name)->get_content()[0]->as<hdl_resource_statement>();
    ASSERT_EQ(res, vh_res);

}


TEST_F(repository_walker , mkignore_exclusion) {

    Repository_walker walker(s_store,d_store, false,{td_file("repository_walker/ignored_dir"),td_file("repository_walker/ignored_dir_2") });

    // The directory is skipped by the presence of a .mkignore marker, not by an
    // explicit exclusion, so its file must not end up in the data store.
    auto file_name = td_file("repository_walker/ignored_dir_3/test_sv_module2.sv");
    ASSERT_FALSE(d_store->get_file<hdl_file>(file_name).has_value());

    // A file outside the marker directory is still analyzed.
    file_name = td_file("repository_walker/test_sv_module.sv");
    ASSERT_TRUE(d_store->get_file<hdl_file>(file_name).has_value());
}


TEST_F(repository_walker , quarantine_report_grouping) {
    macro_table table;
    table.add_file_definitions("a.sv", {{"FATAL", std::string("1")}});
    table.add_file_definitions("b.sv", {{"FATAL", std::string("2")}});
    table.add_file_definitions("defs.svh", {{"SOLO", std::string("1")}});

    std::map<std::string, Repository_walker::quarantine_entry> quarantine;
    for (const auto *f : {"u1.sv", "u2.sv", "u3.sv", "u4.sv"}) {
        Repository_walker::quarantine_entry entry;
        entry.last_undefined["FATAL"] = {};
        quarantine[f] = entry;
    }
    Repository_walker::quarantine_entry solo;
    solo.last_undefined["SOLO"] = {};
    quarantine["solo.sv"] = solo;

    auto lines = Repository_walker::build_quarantine_report(table, quarantine);
    ASSERT_EQ(lines.size(), 2u);
    EXPECT_EQ(lines[0], "Error analyzing 4 files [u1.sv, u2.sv, u3.sv, and 1 more]: "
                        "macro FATAL has conflicting definitions in: a.sv, b.sv");
    EXPECT_EQ(lines[1], "Error analyzing 1 file [solo.sv]: "
                        "macro SOLO defined in defs.svh could not be resolved within 16 fixpoint passes");
}

TEST_F(repository_walker , file_type_handling) {

    //VERILOG
    ASSERT_TRUE(Repository_walker::file_is_verilog("test.sv"));
    ASSERT_TRUE(Repository_walker::file_is_verilog("test.svh"));
    ASSERT_TRUE(Repository_walker::file_is_verilog("test.v"));
    ASSERT_TRUE(Repository_walker::file_is_verilog("test.vh"));
    ASSERT_FALSE(Repository_walker::file_is_verilog("test.xx"));
    ASSERT_FALSE(Repository_walker::file_is_verilog("test.h"));
    //SV INCLUDE HEADERS (index-only: discoverable via `include, never top-level parsed)
    ASSERT_TRUE(Repository_walker::file_is_sv_include_header("test.h"));
    ASSERT_FALSE(Repository_walker::file_is_sv_include_header("test.svh"));
    ASSERT_FALSE(Repository_walker::file_is_sv_include_header("test.sv"));
    ASSERT_FALSE(Repository_walker::file_is_sv_include_header("test.vh"));
    ASSERT_FALSE(Repository_walker::file_is_sv_include_header("test.v"));
    ASSERT_FALSE(Repository_walker::file_is_sv_include_header("test.xx"));
    //VHDL
    ASSERT_TRUE(Repository_walker::file_is_vhdl("test.vhd"));
    ASSERT_TRUE(Repository_walker::file_is_vhdl("test.vhdl"));
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
    //.h is index-only: no other family claims it
    ASSERT_FALSE(Repository_walker::file_is_vhdl("test.h"));
    ASSERT_FALSE(Repository_walker::file_is_script("test.h"));
    ASSERT_FALSE(Repository_walker::file_is_constraint("test.h"));
    ASSERT_FALSE(Repository_walker::file_is_data("test.h"));
}

TEST_F(repository_walker, h_include_index_only) {
    // Veer-like layout: `include "global.h" lives in a different directory
    // than the includer, so it can only resolve via repository-index
    // auto-discovery. The unrelated C header must be indexed but never
    // parsed as HDL.
    const std::string root = "/tmp/ananke_h_idx_test";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root + "/design/include");
    std::filesystem::create_directories(root + "/design");
    std::filesystem::create_directories(root + "/fw");

    {
        std::ofstream ofs(root + "/design/include/global.h");
        ofs << "`define GLOBAL_VAL 42\n";
    }
    {
        std::ofstream ofs(root + "/design/veer_wrapper.sv");
        ofs << "`include \"global.h\"\nmodule veer_wrapper;\n  parameter P = `GLOBAL_VAL;\nendmodule\n";
    }
    {
        std::ofstream ofs(root + "/fw/util.h");
        ofs << "#ifndef UTIL_H\n#define UTIL_H\n#include <stdint.h>\n"
               "typedef struct { int x; } util_t;\nstatic inline int f(int a) { return a + 1; }\n#endif\n";
    }

    const std::string settings_dir = root + "/settings";
    std::filesystem::create_directories(settings_dir);
    {
        std::ofstream ofs(settings_dir + "/settings");
        ofs << "{\"profiles\": {\"h_idx\": {\"hdl_store\":\"" << root << "\"}}}";
    }
    auto s = std::make_shared<settings_store>(false, settings_dir, "h_idx");
    auto d = std::make_shared<data_store>(true, root + "/data_store");

    Repository_walker walker(s, d, true);

    auto idx = walker.get_repository_index();
    ASSERT_NE(idx, nullptr);
    // Both the SV header and the C header are indexed for lookup...
    EXPECT_EQ(idx->lookup("global.h").size(), 1u);
    EXPECT_EQ(idx->lookup("util.h").size(), 1u);

    // ...but only the .sv includer is analyzed as HDL; no top-level parse
    // of either .h (a C-header parse would error / pollute the store).
    const std::string top = root + "/design/veer_wrapper.sv";
    EXPECT_TRUE(d->get_file<hdl_file>(top).has_value());
    EXPECT_FALSE(d->get_file<hdl_file>(root + "/design/include/global.h").has_value());
    EXPECT_FALSE(d->get_file<hdl_file>(root + "/fw/util.h").has_value());

    std::filesystem::remove_all(root);
}

TEST_F(repository_walker, mkignore_patterns) {
    // Gitignore-style `.mkignore`: single-file, anchored, floating, dir-only
    // and negated patterns; nested markers extend their subtree. Mirrors the
    // VeeR snapshots layout (generated fragment + needed headers side by
    // side).
    const std::string root = "/tmp/ananke_ignore_test";
    const std::string settings_dir = root + "/settings";
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root + "/design/include");
    std::filesystem::create_directories(root + "/design");
    std::filesystem::create_directories(root + "/snapshots");
    std::filesystem::create_directories(root + "/build");
    std::filesystem::create_directories(settings_dir);

    auto write = [](const std::string &path, const std::string &content) {
        std::ofstream ofs(path, std::ios::trunc);
        ofs << content;
    };
    write(root + "/.mkignore",
          "# verilator-only fragment and scratch outputs\n"
          "snapshots/*.sv\n"
          "!snapshots/keep.sv\n"
          "*.scratch.h\n"
          "build/\n");
    write(root + "/design/.mkignore", "nested.sv\n");
    write(root + "/design/top.sv", "module top;\nendmodule\n");
    write(root + "/design/nested.sv", "module nested_excluded;\nendmodule\n");
    write(root + "/design/include/defs.h", "`define D 1\n");
    write(root + "/design/include/stuff.scratch.h", "`define S 1\n");
    write(root + "/snapshots/frag.sv", "module frag_excluded;\nendmodule\n");
    write(root + "/snapshots/keep.sv", "module keep;\nendmodule\n");
    write(root + "/build/junk.sv", "module junk_excluded;\nendmodule\n");
    write(settings_dir + "/settings",
          "{\"profiles\": {\"ign\": {\"hdl_store\":\"" + root + "\"}}}");

    auto s = std::make_shared<settings_store>(false, settings_dir, "ign");
    auto d = std::make_shared<data_store>(true, root + "/data_store");
    Repository_walker walker(s, d, true);

    std::string n;
    n = "top";
    EXPECT_TRUE(d->get_HDL_resource(n).has_value());
    n = "keep";
    EXPECT_TRUE(d->get_HDL_resource(n).has_value());
    n = "frag_excluded";
    EXPECT_FALSE(d->get_HDL_resource(n).has_value());
    n = "junk_excluded";
    EXPECT_FALSE(d->get_HDL_resource(n).has_value());
    n = "nested_excluded";
    EXPECT_FALSE(d->get_HDL_resource(n).has_value());

    auto idx = walker.get_repository_index();
    ASSERT_NE(idx, nullptr);
    EXPECT_EQ(idx->lookup("defs.h").size(), 1u);
    EXPECT_TRUE(idx->lookup("stuff.scratch.h").empty());

    std::filesystem::remove_all(root);
}

TEST_F(repository_walker, h_include_invalidation) {
    // Editing a consumed index-only `.h` must invalidate its (cache-hit)
    // consumers on the next run even though the consumers' own bytes are
    // unchanged. The header renames the module via macro, so a stale cache
    // hit is directly observable as the old resource name.
    const std::string root = "/tmp/ananke_h_inv_test";
    const std::string settings_dir = root + "/settings";
    const std::string cache_dir = root + "/cache";
    const std::string hdr = root + "/design/include/defs.h";
    const std::string top = root + "/design/top.sv";
    auto write_header = [&](const std::string &mod) {
        std::ofstream ofs(hdr, std::ios::trunc);
        ofs << "`define MODNAME " << mod << "\n";
    };
    std::filesystem::remove_all(root);
    std::filesystem::create_directories(root + "/design/include");
    std::filesystem::create_directories(root + "/design");
    std::filesystem::create_directories(settings_dir);
    write_header("top1");
    {
        std::ofstream ofs(top);
        ofs << "`include \"defs.h\"\nmodule `MODNAME;\nendmodule\n";
    }
    {
        std::ofstream ofs(settings_dir + "/settings");
        ofs << "{\"profiles\": {\"h_inv\": {\"hdl_store\":\"" << root << "\"}}}";
    }
    auto make_store = [&](const std::string &profile) {
        return std::make_pair(std::make_shared<settings_store>(false, settings_dir, profile),
                              std::make_shared<data_store>(false, cache_dir));
    };

    {
        auto [s, d] = make_store("h_inv");
        Repository_walker walker(s, d, true);
        std::string n1 = "top1";
        ASSERT_TRUE(d->get_HDL_resource(n1).has_value());
        EXPECT_TRUE(d->get_include_file_hashes().contains(hdr));
    } // dtors flush settings + cache to disk

    write_header("top2");

    {
        auto [s, d] = make_store("h_inv");
        Repository_walker walker(s, d, true);
        std::string n1 = "top1", n2 = "top2";
        // Without header-hash tracking the second run serves top1 from cache.
        EXPECT_FALSE(d->get_HDL_resource(n1).has_value());
        EXPECT_TRUE(d->get_HDL_resource(n2).has_value());
    }

    std::filesystem::remove_all(root);
}
