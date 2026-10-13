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

#include <gtest/gtest.h>

#include "data_model/settings_store.hpp"

namespace {
std::string store_settings_path = "/tmp/ananke_settings_test";
auto store_settings_file = store_settings_path + "/settings";
}

TEST(settings_store, include_auto_discovery_default_on) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\"}}}";
    ofs.flush();
    ofs.close();

    settings_store s(false, store_settings_path, "test_profile");
    EXPECT_TRUE(s.get_include_auto_discovery());

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, include_auto_discovery_explicit_off) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
           "\"include_auto_discovery\":false}}}";
    ofs.flush();
    ofs.close();

    settings_store s(false, store_settings_path, "test_profile");
    EXPECT_FALSE(s.get_include_auto_discovery());

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, include_auto_discovery_persisted) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
           "\"include_auto_discovery\":false}}}";
    ofs.flush();
    ofs.close();

    {
        settings_store s(false, store_settings_path, "test_profile");
        EXPECT_FALSE(s.get_include_auto_discovery());
        s.flush();
    }

    settings_store s2(false, store_settings_path, "test_profile");
    EXPECT_FALSE(s2.get_include_auto_discovery());

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, strip_uvm_macros_default_off) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\"}}}";
    ofs.flush();
    ofs.close();

    settings_store s(false, store_settings_path, "test_profile");
    EXPECT_FALSE(s.get_strip_uvm_macros());

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, strip_uvm_macros_explicit_on) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
           "\"strip_uvm_macros\":true}}}";
    ofs.flush();
    ofs.close();

    settings_store s(false, store_settings_path, "test_profile");
    EXPECT_TRUE(s.get_strip_uvm_macros());

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, strip_uvm_macros_persisted) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
           "\"strip_uvm_macros\":true}}}";
    ofs.flush();
    ofs.close();

    {
        settings_store s(false, store_settings_path, "test_profile");
        EXPECT_TRUE(s.get_strip_uvm_macros());
        s.flush();
    }

    settings_store s2(false, store_settings_path, "test_profile");
    EXPECT_TRUE(s2.get_strip_uvm_macros());

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, defines_loaded_and_persisted) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
           "\"defines\":[\"KINTEX7\",\"BOARD_ID=42\"]}}}";
    ofs.flush();
    ofs.close();

    {
        settings_store s(false, store_settings_path, "test_profile");
        EXPECT_EQ(s.get_defines(), (std::set<std::string>{"KINTEX7", "BOARD_ID=42"}));
        s.flush();
    }

    settings_store s2(false, store_settings_path, "test_profile");
    EXPECT_EQ(s2.get_defines(), (std::set<std::string>{"KINTEX7", "BOARD_ID=42"}));

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, strict_default_off) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\"}}}";
    ofs.flush();
    ofs.close();

    settings_store s(false, store_settings_path, "test_profile");
    EXPECT_FALSE(s.get_strict());
    EXPECT_FALSE(s.get_parse_options().strict);

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, strict_explicit_on) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
           "\"strict\":true}}}";
    ofs.flush();
    ofs.close();

    settings_store s(false, store_settings_path, "test_profile");
    EXPECT_TRUE(s.get_strict());
    EXPECT_TRUE(s.get_parse_options().strict);

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, strict_persisted) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
           "\"strict\":true}}}";
    ofs.flush();
    ofs.close();

    {
        settings_store s(false, store_settings_path, "test_profile");
        EXPECT_TRUE(s.get_strict());
        s.flush();
    }

    settings_store s2(false, store_settings_path, "test_profile");
    EXPECT_TRUE(s2.get_strict());

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, strict_override_not_persisted) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\"}}}";
    ofs.flush();
    ofs.close();

    {
        settings_store s(false, store_settings_path, "test_profile");
        EXPECT_FALSE(s.get_strict());
        s.set_strict_override(true);
        EXPECT_TRUE(s.get_strict());
        EXPECT_TRUE(s.get_parse_options().strict);
        s.flush();
    }

    settings_store s2(false, store_settings_path, "test_profile");
    EXPECT_FALSE(s2.get_strict());

    std::filesystem::remove_all(store_settings_path);
}
TEST(settings_store, ignored_includes_default_empty) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\"}}}";
    ofs.flush();
    ofs.close();

    settings_store s(false, store_settings_path, "test_profile");
    EXPECT_TRUE(s.get_ignored_includes().empty());
    EXPECT_TRUE(s.get_include_deconfliction().empty());
    EXPECT_TRUE(s.get_parse_options().ignored_includes.empty());

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, ignored_includes_loaded_and_persisted) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
            "\"ignored_includes\":[\"avery_defines.svh\"],"
            "\"include_deconfliction\":{\"x.svh\": \"rtl/x.svh\","
            "\"defs.svh\": {\"rtl/foo\": \"rtl/foo/defs.svh\"}}}}}";
    ofs.flush();
    ofs.close();

    {
        settings_store s(false, store_settings_path, "test_profile");
        EXPECT_EQ(s.get_ignored_includes(), (std::set<std::string>{"avery_defines.svh"}));
        ASSERT_TRUE(s.get_include_deconfliction().contains("x.svh"));
        EXPECT_EQ(s.get_include_deconfliction().at("x.svh").global_target,
                  std::optional<std::string>("rtl/x.svh"));
        ASSERT_TRUE(s.get_include_deconfliction().contains("defs.svh"));
        EXPECT_EQ(s.get_include_deconfliction().at("defs.svh").scoped_targets.at("rtl/foo"),
                  "rtl/foo/defs.svh");
        EXPECT_EQ(s.get_parse_options().ignored_includes,
                  (std::set<std::string>{"avery_defines.svh"}));
        s.flush();
    }

    settings_store s2(false, store_settings_path, "test_profile");
    EXPECT_EQ(s2.get_ignored_includes(), (std::set<std::string>{"avery_defines.svh"}));
    ASSERT_TRUE(s2.get_include_deconfliction().contains("x.svh"));

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, ephemeral_include_overlay_unioned_not_persisted) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
            "\"ignored_includes\":[\"profile.svh\"]}}}";
    ofs.flush();
    ofs.close();

    {
        settings_store s(false, store_settings_path, "test_profile");
        EXPECT_FALSE(s.has_ephemeral_include_config());
        s.set_ephemeral_include_config({"depfile.svh"}, {});
        EXPECT_TRUE(s.has_ephemeral_include_config());
        EXPECT_EQ(s.get_parse_options().ignored_includes,
                  (std::set<std::string>{"profile.svh", "depfile.svh"}));
        s.flush();
    }

    settings_store s2(false, store_settings_path, "test_profile");
    EXPECT_FALSE(s2.has_ephemeral_include_config());
    EXPECT_EQ(s2.get_ignored_includes(), (std::set<std::string>{"profile.svh"}));

    std::filesystem::remove_all(store_settings_path);
}

TEST(settings_store, ephemeral_defines_overlay_wins_per_name) {
    std::filesystem::create_directories(store_settings_path);
    std::ofstream ofs(store_settings_file);
    ofs << "{\"profiles\": {\"test_profile\": {\"hdl_store\":\"/tmp/repo\","
            "\"defines\":[\"KEEP=1\",\"OVERRIDDEN=old\",\"BARE\"]}}}";
    ofs.flush();
    ofs.close();

    {
        settings_store s(false, store_settings_path, "test_profile");
        EXPECT_FALSE(s.has_ephemeral_defines());
        s.set_ephemeral_defines({{"OVERRIDDEN", "new"}, {"FROM_DEPFILE", "x"}});
        EXPECT_TRUE(s.has_ephemeral_defines());
        EXPECT_EQ(s.get_parse_options().defines,
                  (std::set<std::string>{"KEEP=1", "OVERRIDDEN=new", "BARE", "FROM_DEPFILE=x"}));
        s.flush();
    }

    // Overlay is ephemeral: a fresh store sees the profile list only.
    settings_store s2(false, store_settings_path, "test_profile");
    EXPECT_FALSE(s2.has_ephemeral_defines());
    EXPECT_EQ(s2.get_defines(), (std::set<std::string>{"KEEP=1", "OVERRIDDEN=old", "BARE"}));

    std::filesystem::remove_all(store_settings_path);
}
