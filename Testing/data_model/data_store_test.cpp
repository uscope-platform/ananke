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

#include "../includes/data_model/data_store.hpp"


TEST( data_store_test , evict_constr) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    Constraints test_constr("test");

    store_1->store_constraint(test_constr);
    store_1->evict_constraint(test_constr.get_name());
    std::string n = "test";
    Constraints test_item = store_1->get_constraint(n);

    delete store_1;
    ASSERT_EQ(test_item, Constraints());

}


TEST( data_store_test , evict_script) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    script_specs s;
    s.name = "test";
    s.type = "py";
    Script test_scr(s);

    store_1->store_script(test_scr);
    store_1->evict_script(test_scr.get_name());
    std::string n = "test";
    Script test_item = store_1->get_script(n);

    delete store_1;
    ASSERT_EQ(test_item.get_name(), "");

}


TEST( data_store_test , evict_data_file) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    DataFile test_df("test","/data/file/path");

    store_1->store_data_file(test_df);
    store_1->evict_data_file(test_df.get_name());
    std::string n = "test";
    DataFile test_item = store_1->get_data_file(n);

    delete store_1;
    ASSERT_EQ(test_item, DataFile());

}


TEST( data_store_test , evict_hdl_entity) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    HDL_Resource test_entity(module, "test", "/test/path");
    test_entity.set_line_n(15);

    store_1->store_hdl_entity(test_entity);
    store_1->evict_hdl_entity(test_entity.getName());
    std::string n = "test";
    HDL_Resource test_item = store_1->get_HDL_resource(n);

    delete store_1;
    ASSERT_EQ(test_item, HDL_Resource());

}


TEST( data_store_test , evict_interface_entity) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    HDL_Resource test_entity(interface, "test", "/test/path");
    test_entity.set_line_n(15);

    store_1->store_hdl_entity(test_entity);
    store_1->evict_hdl_entity(test_entity.getName());
    std::string n = "test";
    HDL_Resource test_item = store_1->get_HDL_resource(n);

    delete store_1;
    ASSERT_EQ(test_item, HDL_Resource());

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




TEST( data_store_test , store_script_vect) {

    auto *store = new data_store(true, "/tmp/test_data_store");
    script_specs s;
    s.name = "test_1";
    s.type = "py";
    Script test_scr_1(s);
    s.name = "test_2";
    s.type = "py";
    Script test_scr_2(s);
    std::vector<Script> test_vect = {test_scr_1,test_scr_2};
    store->store_script(test_vect);
    std::string name = "test_1";
    Script test_res_1 = store->get_script(name);
    name = "test_2";
    Script test_res_2 = store->get_script(name);

    std::vector<Script> res_vect = {test_res_1, test_res_2};

    store->evict_script("test_1");
    store->evict_script("test_2");

    delete store;
    ASSERT_EQ(test_vect[0], res_vect[0]);
    ASSERT_EQ(test_vect[1], res_vect[1]);
}


TEST( data_store_test , store_data_file_vect) {

    auto *store = new data_store(true, "/tmp/test_data_store");
    DataFile test_df_1("test_1","/path/1");
    DataFile test_df_2("test_2","/path/2");
    std::vector<DataFile> test_vect = {test_df_1,test_df_2};
    store->store_data_file(test_vect);
    std::string name = "test_1";
    DataFile test_res_1 = store->get_data_file(name);
    name = "test_2";
    DataFile test_res_2 = store->get_data_file(name);
    std::vector<DataFile> res_vect = {test_res_1, test_res_2};

    store->evict_script("test_1");
    store->evict_script("test_2");

    delete store;
    ASSERT_EQ(test_vect[0], res_vect[0]);
    ASSERT_EQ(test_vect[1], res_vect[1]);
}





TEST( data_store_test , store_interface_vect) {

    auto *store = new data_store(true, "/tmp/test_data_store");
    HDL_Resource test_res_1(interface, "test_1", "/bin/sh");
    HDL_Resource test_res_2(interface, "test_2", "/bin/sh");
    std::vector<HDL_Resource> test_vect = {test_res_1,test_res_2};
    store->store_hdl_entity(test_vect);
    std::string name = "test_1";
    HDL_Resource test_result_1 = store->get_HDL_resource(name);
    name = "test_2";
    HDL_Resource test_result_2 = store->get_HDL_resource(name);
    std::vector<HDL_Resource> res_vect = {test_result_1, test_result_2};

    store->evict_hdl_entity("test_1");
    store->evict_hdl_entity("test_2");

    delete store;
    ASSERT_EQ(test_vect[0], res_vect[0]);
    ASSERT_EQ(test_vect[1], res_vect[1]);
}

TEST( data_store_test , store_hdl_vect) {

    auto *store = new data_store(true, "/tmp/test_data_store");
    HDL_Resource test_res_1(module, "test_1", "/bin/sh");
    HDL_Resource test_res_2(module, "test_2", "/bin/sh");
    std::vector<HDL_Resource> test_vect = {test_res_1,test_res_2};
    store->store_hdl_entity(test_vect);
    std::string name = "test_1";
    HDL_Resource test_result_1 = store->get_HDL_resource(name);
    name = "test_2";
    HDL_Resource test_result_2 = store->get_HDL_resource(name);
    std::vector<HDL_Resource> res_vect = {test_result_1, test_result_2};

    store->evict_hdl_entity("test_1");
    store->evict_hdl_entity("test_2");

    delete store;
    ASSERT_EQ(test_vect[0], res_vect[0]);
    ASSERT_EQ(test_vect[1], res_vect[1]);
}

TEST( data_store_test , store_const_vect) {

    auto *store = new data_store(true, "/tmp/test_data_store");
    Constraints test_const_1( "test_1");
    Constraints test_const_2("test_2");
    std::vector<Constraints> test_vect = {test_const_1,test_const_2};
    store->store_constraint(test_vect);
    std::string name = "test_1";
    Constraints test_result_1 = store->get_constraint(name);
    name = "test_2";
    Constraints test_result_2 = store->get_constraint(name);
    std::vector<Constraints> res_vect = {test_result_1, test_result_2};

    store->evict_constraint("test_1");
    store->evict_constraint("test_2");

    delete store;
    ASSERT_EQ(test_vect[0], res_vect[0]);
    ASSERT_EQ(test_vect[1], res_vect[1]);
}


TEST( data_store_test , constr_clean_up) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    Constraints test_constr("test");
    test_constr.set_path("/test");
    store_1->store_constraint(test_constr);
    delete store_1;
    auto *store_2 = new data_store(true, "/tmp/test_data_store");
    std::string name = "test";
    Constraints result = store_2->get_constraint(name);
    ASSERT_EQ(result, Constraints());
    store_2->evict_constraint(test_constr.get_name());
    delete store_2;

}


TEST( data_store_test , data_file_clean_up) {

    auto *store_1 = new data_store(true,"/tmp/test_data_store");
    DataFile test_df("test","/data/file/path");
    store_1->store_data_file(test_df);
    delete store_1;
    auto *store_2 = new data_store(true,"/tmp/test_data_store");
    std::string name = "test";
    Script result = store_2->get_script(name);
    ASSERT_EQ(result.get_name(), "");
    store_2->evict_script(test_df.get_name());
    delete store_2;

}

TEST( data_store_test , script_clean_up) {

    auto *store_1 = new data_store(true, "/tmp/test_data_store");
    script_specs s;
    s.name = "test";
    s.type = "py";
    Script test_scr(s);
    test_scr.set_path("/test");
    store_1->store_script(test_scr);
    delete store_1;
    auto *store_2 = new data_store(true,"/tmp/test_data_store");
    std::string name = "test";
    Script result = store_2->get_script(name);
    ASSERT_EQ(result.get_name(), "");
    store_2->evict_script(test_scr.get_name());
    delete store_2;

}

TEST( data_store_test , resource_clean_up) {

    auto *store_1 = new data_store(true,"/tmp/test_data_store");

    HDL_Resource test_entity(module, "test", "/test");
    test_entity.set_line_n(15);

    store_1->store_hdl_entity(test_entity);
    delete store_1;
    auto *store_2 = new data_store(true,"/tmp/test_data_store");
    std::string name = "test";
    HDL_Resource result = store_2->get_HDL_resource(name);
    ASSERT_EQ(result, HDL_Resource());
    store_2->evict_script(test_entity.getName());
    delete store_2;

}

