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

