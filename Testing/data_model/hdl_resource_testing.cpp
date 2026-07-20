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

#include "data_model/HDL/HDL_Resource.hpp"
#include "data_model/HDL/statement/hdl_instance_statement.hpp"




TEST( HDL_resource_test , is_interface) {
    HDL_Resource resource;

    ASSERT_FALSE(resource.is_interface());
}


TEST( HDL_resource_test , ser_des_hdl_resource) {


    HDL_Resource hdl_out;
    hdl_out.set_name("test");
    hdl_out.set_type(module);
    hdl_out.set_path("/bin/sh");
    hdl_out.set_line_n(13);
    std::stringstream os;
    {
        cereal::BinaryOutputArchive archive_out(os);
        archive_out(hdl_out);
    }

    std::string json_str = os.str();
    std::stringstream is(json_str);
    HDL_Resource hdl_in;
    cereal::BinaryInputArchive archive_in(is);
    archive_in(hdl_in);

    ASSERT_EQ(hdl_out, hdl_in);

    std::shared_ptr<HDL_Resource> hdl_out_ptr = std::make_shared<HDL_Resource>(hdl_out);

    std::stringstream os_ptr;
    {
        cereal::BinaryOutputArchive archive_out(os_ptr);
        archive_out(*hdl_out_ptr);
    }

    std::string json_str_ptr = os.str();
    std::stringstream is_ptr(json_str_ptr);

    cereal::BinaryInputArchive archive_in_ptr(is_ptr);

    HDL_Resource hdl_in_inner;
    archive_in_ptr(hdl_in_inner);

    std::shared_ptr<HDL_Resource> hdl_in_ptr = std::make_shared<HDL_Resource>(hdl_in_inner);

    ASSERT_EQ(*hdl_out_ptr, *hdl_in_ptr);


}

TEST( HDL_resource_test , get_path) {

    HDL_Resource res;
    res.set_name("test_module");
    res.set_type(module);
    res.set_path("/test_path/test.sv");

    ASSERT_EQ(res.get_path(), "/test_path/test.sv");
}

TEST( HDL_resource_test , get_statements) {
    HDL_Resource test_item;
    auto s1 = std::make_shared<hdl_instance_statement>();
    s1->set_name("inst"); s1->set_type("test_module_1"); s1->set_dependency_class(module);
    test_item.add_statement(s1);
    auto s2 = std::make_shared<hdl_instance_statement>();
    s2->set_name("inst"); s2->set_type("test_module_2"); s2->set_dependency_class(module);
    test_item.add_statement(s2);

    ASSERT_EQ(test_item.get_statements().size(), 2);
    ASSERT_EQ(*test_item.get_statements()[0], *s1);
    ASSERT_EQ(*test_item.get_statements()[1], *s2);
}

TEST( HDL_resource_test , get_name) {

    HDL_Resource test_item;
    test_item.set_name("test");
    test_item.set_type(module);
    test_item.set_path("/test/test.sv");

    ASSERT_EQ(test_item.getName(), "test");
}