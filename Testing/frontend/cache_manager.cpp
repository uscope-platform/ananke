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
#include <filesystem>

#include "data_model/settings_store.hpp"
#include "data_model/data_store.hpp"
#include "frontend/cache_manager.hpp"





std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE,  int(*)(FILE*)> pipe(popen(cmd, "r"), pclose);

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}


class cache_manager_Test : public ::testing::Test {
protected:
    void SetUp() {
        d_store = std::make_shared<data_store>(true,"/tmp/test_data_store");


        module_documentation doc;
        doc.set_name("test_module");
        register_documentation reg_1("simple_register_r", 0, "Single word read only register", true, false);
        register_documentation reg_2("simple_register_w", 0x4, "Single word write only register", false, true);
        register_documentation reg_3("field_registers", 0x20, "register with multiple fields", true, true);
        field_documentation field_1("field_1", "First field",0,8);
        field_documentation field_2("field_2", "Second Field",8,8);
        reg_3.add_field(field_1);
        reg_3.add_field(field_2);
        doc.add_register(reg_1);
        doc.add_register(reg_2);
        doc.add_register(reg_3);


        HDL_Resource mod_entity(module, "test_module", "test/path.sv");
        mod_entity.set_documentation(doc);
        d_store->store_hdl_entity(mod_entity);

        s_store = std::make_shared<settings_store>(true,"/tmp/test_settings_store");
        s_store->set_setting("cache_dump", "");

        std::filesystem::copy("check_files/test_package.sv", "check_files/test_package.sv.test");
    }

    virtual void TearDown() {
        d_store->evict_hdl_entity("register_type");
        std::filesystem::remove("test_cache_dump");
        std::filesystem::remove("check_files/test_package.sv.test");
    }
    std::shared_ptr<data_store> d_store;
    std::shared_ptr<settings_store> s_store;
};


TEST_F(cache_manager_Test , file_hashing) {
    cache_manager cm(s_store, d_store, false);
    std::filesystem::path p("check_files/test_package.sv");
    cm.add_file(p);
    std::string cl = cm.get_cache_line("check_files/test_package.sv");

    std::string check_hash = exec("openssl dgst -sha256 -c check_files/test_package.sv");
    unsigned int pos = check_hash.find(' ');
    check_hash = check_hash.substr(pos+1, check_hash.size()-(pos+2));

    ASSERT_EQ(cl, check_hash);
}

TEST_F(cache_manager_Test , file_is_cached) {
    auto file_path = "check_files/test_package.sv";
    cache_manager cm(s_store, d_store, false);
    std::filesystem::path p(file_path);
    cm.add_file(p);

    bool is_cached = cm.is_cached(p);

    ASSERT_TRUE(is_cached);
    cm.purge();
    std::filesystem::path p2(file_path);
    is_cached = cm.is_cached(p2);
    ASSERT_FALSE(is_cached);
}

TEST_F(cache_manager_Test , file_is_changed) {
    cache_manager cm(s_store, d_store, false);


    std::filesystem::path p("check_files/test_package.sv.test");
    cm.add_file(p);

    bool is_changed = cm.is_changed(p);

    ASSERT_FALSE(is_changed);

    {
        std::ofstream outfile;
        outfile.open("check_files/test_package.sv.test", std::ios_base::app); // append instead of overwrite
        outfile << "change";
    }
    is_changed = cm.is_changed(p);
    ASSERT_TRUE(is_changed);

}


TEST_F(cache_manager_Test , cache_loading) {
    std::string filename = "check_files/test_package.sv.test";
    std::filesystem::path p(filename);
    std::string original_hash, loaded_hash;
    {
        cache_manager cm(s_store, d_store, false);
        cm.add_file(p);
        original_hash = cm.get_cache_line(filename);
    }
    {
        cache_manager cm(s_store, d_store, false);
        loaded_hash = cm.get_cache_line(filename);
    }
    ASSERT_EQ(original_hash, loaded_hash);
}
