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

#include "data_model/data_store.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"
#include "data_model/HDL/types/HDL_simple_type.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/types/HDL_union_type.hpp"
#include "data_model/HDL/types/HDL_enum_type.hpp"
#include "data_model/HDL/parameters/components/token/Type_ref.hpp"


TEST( data_store_test , evict_constr) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    Constraints test_constr("test");

    store_1->store_file({"/test/constraint/file", "hash", {test_constr}});
    store_1->evict_file("/test/constraint/file");
    std::string n = "test";
    auto test_item = store_1->get_constraint(n);

    delete store_1;
    EXPECT_FALSE(test_item);

}


TEST( data_store_test , evict_script) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    script_specs s;
    s.name = "test";
    s.type = "py";
    Script test_scr(s);

    store_1->store_file({"/test/script", "", {test_scr}});
    store_1->evict_file("/test/script");
    std::string n = "test";
    auto test_item = store_1->get_script(n);
    delete store_1;
    EXPECT_FALSE(test_item);

}


TEST( data_store_test , evict_data_file) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    DataFile test_df("test","/data/file/path");

    store_1->store_file({"/data/file/path", "hash", {test_df}});
    store_1->evict_file("/data/file/path");
    std::string n = "test";
    auto test_item = store_1->get_data_file(n);

    delete store_1;
    ASSERT_FALSE(test_item);

}


TEST( data_store_test , evict_hdl_entity) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    auto test_entity = std::make_shared<hdl_resource_statement>();
    test_entity->set_name("test");
    test_entity->set_type(module);
    test_entity->set_line_n(15);
    hdl_file f;
    f.set_content({test_entity});
    store_1->store_file({"/test/path", "test_hash", f});
    store_1->evict_file("/test/path");
    std::string n = "test";
    auto  test_item = store_1->get_HDL_resource(n);

    delete store_1;
    ASSERT_FALSE(test_item.has_value());

}


TEST( data_store_test , ser_des_data_File) {


    DataFile data_out("test", "/test/path");

    std::stringstream os;
    {
        cereal::BinaryOutputArchive archive_out(os);
        archive_out(data_out);
    }

    std::string json_str = os.str();
    std::stringstream is(json_str);
    DataFile data_in;
    cereal::BinaryInputArchive archive_in(is);
    archive_in(data_in);
    ASSERT_EQ(data_out, data_in);

}





TEST( data_store_test , store_interface_vect) {

    auto *store = new data_store(true, "/tmp/test_data_store");
    auto test_res_1 = std::make_shared<hdl_resource_statement>();
    auto test_res_2 = std::make_shared<hdl_resource_statement>();
    test_res_1->set_name("test_1");
    test_res_2->set_name("test_2");
    test_res_1->set_type(interface);
    test_res_2->set_type(interface);
    hdl_file f;
    f.set_content({test_res_1,test_res_2});
    store->store_file({
        "/bin/sh",
        "test_hash",
        f
    });
    std::string name = "test_1";
    auto test_result_1 = store->get_HDL_resource(name);
    ASSERT_TRUE(test_result_1.has_value());
    name = "test_2";
    auto test_result_2 = store->get_HDL_resource(name);
    ASSERT_TRUE(test_result_2.has_value());

    store->evict_file("/bin/sh");

    delete store;
    ASSERT_EQ(test_res_1, test_result_1.value());
    ASSERT_EQ(test_res_2, test_result_2.value());
}

TEST( data_store_test , store_hdl_vect) {

    auto *store = new data_store(true, "/tmp/test_data_store");
    auto test_file_1 = std::make_shared<hdl_resource_statement>();
    auto test_file_2 = std::make_shared<hdl_resource_statement>();
    test_file_1->set_name("test_1");
    test_file_1->set_type(module);
    test_file_2->set_name("test_2");
    test_file_2->set_type(module);
    hdl_file f;
    f.set_content({test_file_1, test_file_2});
    store->store_file({
       "/bin/sh",
       "test_hash",
       f
    });
    std::string name = "test_1";
    auto test_result_1 = store->get_HDL_resource(name);
    ASSERT_TRUE(test_result_1.has_value());
    name = "test_2";
    auto test_result_2 = store->get_HDL_resource(name);
    ASSERT_TRUE(test_result_1.has_value());

    store->evict_file("/bin/sh");

    delete store;
    ASSERT_EQ(test_file_1, test_result_1);
    ASSERT_EQ(test_file_2, test_result_2);
}


TEST( data_store_test , constr_clean_up) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    Constraints test_constr("test");
    test_constr.set_path("/test");
    store_1->store_file({"/test/constraint/file", "hash", {test_constr}});
    delete store_1;
    auto *store_2 = new data_store(true, "/tmp/test_data_store");
    std::string name = "test";
    auto result = store_2->get_constraint(name);
    ASSERT_FALSE(result);
    store_2->evict_file("/test");
    delete store_2;

}


TEST( data_store_test , data_file_clean_up) {

    auto *store_1 = new data_store(true,"/tmp/test_data_store");
    DataFile test_df("test","/data/file/path");
    store_1->store_file({"/data/file/path", "hash", {test_df}});
    delete store_1;
    auto *store_2 = new data_store(true,"/tmp/test_data_store");
    std::string name = "test";
    auto result = store_2->get_script(name);
    ASSERT_FALSE(result);
    delete store_2;

}

TEST( data_store_test , script_clean_up) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    script_specs s;
    s.name = "test";
    s.type = "py";
    Script test_scr(s);
    test_scr.set_path("/test");
    store_1->store_file({"/test", "hash", {test_scr}});
    delete store_1;
    auto *store_2 = new data_store(true,"/tmp/test_data_store");
    std::string name = "test";
    auto result = store_2->get_script(name);
    ASSERT_FALSE(result);
    store_2->evict_file("/test");
    delete store_2;

}

TEST( data_store_test , resource_clean_up) {

    auto *store_1 = new data_store(true,"/tmp/test_data_store");

    auto test_entity = std::make_shared<hdl_resource_statement>();
    test_entity->set_name("test");
    test_entity->set_type(module);
    test_entity->set_line_n(15);
    hdl_file f;
    f.set_content({test_entity});
    store_1->store_file({"/test", "hash", f});
    delete store_1;
    auto *store_2 = new data_store(true,"/tmp/test_data_store");
    std::string name = "test";
    auto result = store_2->get_HDL_resource(name);
    ASSERT_FALSE(result.has_value());
    store_2->evict_file("/test");
    delete store_2;

}


TEST( data_store_test , polymorphic_types_round_trip ) {
    // Every polymorphic subtype of hdl_type and Expression_base must be
    // registered with cereal (CEREAL_REGISTER_TYPE + the polymorphic relation,
    // with the binary archive header included in the same TU), otherwise saving
    // the cache throws "unregistered polymorphic type". Round-trip each through
    // the same BinaryOutputArchive/InputArchive pair the cache uses.
    auto round_trip_hdl_type = [](std::shared_ptr<hdl_type> out) {
        std::stringstream ss;
        {
            cereal::BinaryOutputArchive archive_out(ss);
            archive_out(out);
        }
        std::stringstream is(ss.str());
        std::shared_ptr<hdl_type> in;
        {
            cereal::BinaryInputArchive archive_in(is);
            archive_in(in);
        }
        return in;
    };

    EXPECT_TRUE(round_trip_hdl_type(std::make_shared<HDL_simple_type>())->is<HDL_simple_type>());
    EXPECT_TRUE(round_trip_hdl_type(std::make_shared<HDL_struct_type>())->is<HDL_struct_type>());
    EXPECT_TRUE(round_trip_hdl_type(std::make_shared<HDL_union_type>())->is<HDL_union_type>());
    EXPECT_TRUE(round_trip_hdl_type(std::make_shared<HDL_enum_type>())->is<HDL_enum_type>());
    EXPECT_TRUE(round_trip_hdl_type(std::make_shared<HDL_external_type>(qualified_identifier("foo")))->is<HDL_external_type>());

    // Type_ref is a polymorphic Expression_base subclass.
    std::shared_ptr<Expression_base> tr_out = std::make_shared<Type_ref>(qualified_identifier("foo"));
    std::stringstream ss;
    {
        cereal::BinaryOutputArchive archive_out(ss);
        archive_out(tr_out);
    }
    std::stringstream is(ss.str());
    std::shared_ptr<Expression_base> tr_in;
    {
        cereal::BinaryInputArchive archive_in(is);
        archive_in(tr_in);
    }
    ASSERT_TRUE(tr_in != nullptr);
    EXPECT_TRUE(tr_in->is<Type_ref>());
}

TEST( data_store_test , corrupted_cache_recovery ) {
    std::filesystem::create_directories("/tmp/ananke_ds_corrupt");
    {
        std::ofstream ofs("/tmp/ananke_ds_corrupt/unified_cache", std::ios::binary);
        ofs << "\x01\x02\x03 garbage not cereal data";
    }
    // Should not throw, should start with an empty cache.
    data_store ds(false, "/tmp/ananke_ds_corrupt");
    EXPECT_FALSE(ds.contains("anything"));
    std::filesystem::remove_all("/tmp/ananke_ds_corrupt");
}

TEST( data_store_test , persistent_cache_schema_round_trip ) {
    auto dir = "/tmp/ananke_ds_schema";
    std::filesystem::create_directories(dir);
    std::string stored_path = dir + std::string("/schema_test.sv");
    {
        std::ofstream ofs(stored_path);
        ofs << "// test";
    }

    {
        data_store store(false, dir);
        auto test_res = std::make_shared<hdl_resource_statement>();
        test_res->set_name("schema_test");
        test_res->set_type(interface);
        hdl_file f;
        f.set_content({test_res});
        store.store_file({stored_path, "test_hash", f});
    } // destructor persists the cache stamped with the current schema hash

    {
        data_store store(false, dir);
        EXPECT_TRUE(store.contains(stored_path));
        auto res = store.get_HDL_resource("schema_test");
        ASSERT_TRUE(res.has_value());
        EXPECT_EQ(res.value()->getName(), "schema_test");
    }

    std::filesystem::remove_all(dir);
}

TEST( data_store_test , persistent_cache_schema_mismatch_discarded ) {
    auto dir = "/tmp/ananke_ds_schema_mismatch";
    std::filesystem::create_directories(dir);

    // Cache written with a stale schema fingerprint must be discarded on load.
    std::unordered_map<std::string, data_store::cached_item> bogus_cache;
    {
        std::ofstream ofs(dir + std::string("/unified_cache"), std::ios::binary);
        cereal::BinaryOutputArchive oa(ofs);
        oa(std::string("stale_schema_fingerprint"), bogus_cache);
    }

    data_store store(false, dir);
    EXPECT_FALSE(store.contains("anything"));

    std::filesystem::remove_all(dir);
}

TEST( data_store_test , duplicate_resource_name_lists_all) {
    // Two files contributing a same-named resource: the enumerating lookup
    // must surface both, while the single-pick lookup still resolves.
    // (The single pick warns; log output is not asserted here.)
    auto *store = new data_store(true, "/tmp/test_data_store");
    auto dup_a = std::make_shared<hdl_resource_statement>();
    dup_a->set_name("dup");
    dup_a->set_type(module);
    auto dup_b = std::make_shared<hdl_resource_statement>();
    dup_b->set_name("dup");
    dup_b->set_type(module);
    hdl_file fa;
    fa.set_content({dup_a});
    hdl_file fb;
    fb.set_content({dup_b});
    store->store_file({"/path/one", "hash_one", fa});
    store->store_file({"/path/two", "hash_two", fb});

    auto all = store->get_all_HDL_resources("dup");
    ASSERT_EQ(all.size(), 2);

    auto single = store->get_HDL_resource("dup");
    ASSERT_TRUE(single.has_value());
    EXPECT_EQ(single.value()->getName(), "dup");

    auto missing = store->get_all_HDL_resources("nope");
    EXPECT_TRUE(missing.empty());
    EXPECT_FALSE(store->get_HDL_resource("nope").has_value());

    delete store;
}

TEST( data_store_test , duplicate_resource_deconfliction) {
    // With two same-named resources, the deconfliction map selects by
    // source path; a stale entry falls back to first-match; no entry keeps
    // the old first-match behavior.
    auto *store = new data_store(true, "/tmp/test_data_store");
    auto dup_a = std::make_shared<hdl_resource_statement>();
    dup_a->set_name("dup");
    dup_a->set_type(module);
    auto dup_b = std::make_shared<hdl_resource_statement>();
    dup_b->set_name("dup");
    dup_b->set_type(module);
    hdl_file fa;
    fa.set_content({dup_a});
    hdl_file fb;
    fb.set_content({dup_b});
    store->store_file({"/path/one", "hash_one", fa});
    store->store_file({"/path/two", "hash_two", fb});

    store->set_deconfliction({{"dup", "/path/two"}});
    auto picked = store->get_HDL_resource("dup");
    ASSERT_TRUE(picked.has_value());
    EXPECT_EQ(picked.value(), dup_b);

    store->set_deconfliction({{"dup", "two"}});
    picked = store->get_HDL_resource("dup");
    ASSERT_TRUE(picked.has_value());
    EXPECT_EQ(picked.value(), dup_b);

    store->set_deconfliction({{"dup", "/nowhere/nothing.sv"}});
    picked = store->get_HDL_resource("dup");
    ASSERT_TRUE(picked.has_value());

    store->set_deconfliction({});
    picked = store->get_HDL_resource("dup");
    ASSERT_TRUE(picked.has_value());

    delete store;
}

TEST( data_store_test , store_cache_never_throws ) {
    // Point store_path at an existing regular file so directory creation fails;
    // the constructor and the destructor's store_cache must not throw.
    std::filesystem::create_directories("/tmp/ananke_ds_nodir");
    {
        std::ofstream ofs("/tmp/ananke_ds_nodir/blocker");
        ofs << "x";
    }
    {
        data_store ds(false, "/tmp/ananke_ds_nodir/blocker");
        (void)ds;
    }
    std::filesystem::remove_all("/tmp/ananke_ds_nodir");
}
