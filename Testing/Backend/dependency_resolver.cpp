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
#include "analysis/HDL_ast_builder_v2.hpp"
#include "Backend/Dependency_resolver.hpp"
#include "data_model/HDL/statement/hdl_instance_statement.hpp"



class dep_resolver : public ::testing::Test {
protected:
    void SetUp() {
        d_store = std::make_shared<data_store>(true,"/tmp/test_data_store");
        hdl_resource_statement mod_entity;
        mod_entity.set_name("test_module");
        mod_entity.set_type(module);

        auto s1 = std::make_shared<hdl_instance_statement>();
        s1->set_name("inst"); s1->set_type("test_dep"); s1->set_dependency_class(module);
        mod_entity.add_statement(s1);
        auto s2 = std::make_shared<hdl_instance_statement>();
        s2->set_name("mem_init"); s2->set_type("test_mem_init"); s2->set_dependency_class(memory_init);
        mod_entity.add_statement(s2);
        auto s3 = std::make_shared<hdl_instance_statement>();
        s3->set_name("exm"); s3->set_type("excluded_module"); s3->set_dependency_class(module);
        mod_entity.add_statement(s3);
        auto s4 = std::make_shared<hdl_instance_statement>();
        s4->set_name("pkg"); s4->set_type("test_package"); s4->set_dependency_class(package);
        mod_entity.add_statement(s4);

        std::vector entities ={mod_entity};
        hdl_file f;
        f.set_content({std::make_shared<hdl_resource_statement>(mod_entity)});
        d_store->store_file({"test/mod.sv", "file_hash", f});
        DataFile D("test_mem_init", "test/mem_init.mem");
        d_store->store_file({"test/mem_init.mem", "hash", {D}});

        hdl_resource_statement expl_dep;
        expl_dep.set_name("expl_dep");
        expl_dep.set_type(module);
        f.set_content({std::make_shared<hdl_resource_statement>(expl_dep)});
        d_store->store_file({"test/explicit/dep.sv", "file_hash", f});

        hdl_resource_statement dep_entity;
        dep_entity.set_name("test_dep");
        dep_entity.set_type(module);
        f.set_content({std::make_shared<hdl_resource_statement>(dep_entity)});
        d_store->store_file({"test/dep.sv", "file_hash", f});

        hdl_resource_statement pkg_entity;
        pkg_entity.set_name("test_package");
        pkg_entity.set_type(package);

        f.set_content({std::make_shared<hdl_resource_statement>(pkg_entity)});
        d_store->store_file({"test/pkg.sv", "file_hash", f});
    }

    virtual void TearDown() {
    }
    std::shared_ptr<data_store> d_store;
};

TEST_F(dep_resolver , dependency_resolver) {
    std::shared_ptr<settings_store> s_store = std::make_shared<settings_store>(true, "/tmp/test_data_store", "test_profile");

    Depfile df;
    df.add_excluded_module("excluded_module");
    HDL_ast_builder_v2 b(s_store, d_store, df);
    auto synth = b.build_ast(std::vector<std::string>({"test_module"}))[0];
    std::vector<std::string> add_synyh_mod = {"expl_dep"};
    auto additional_synth_modules = b.build_ast(add_synyh_mod);
    additional_synth_modules.insert(additional_synth_modules.end(), synth);

    // RESOLVE DEPENDENCIES
    Dependency_resolver_v2 synth_r(additional_synth_modules, d_store);
    auto synth_sources = synth_r.get_dependencies();

    Dependency_resolver_v2 res(additional_synth_modules, d_store);
    std::set<std::string> check = {"test/mod.sv", "test/dep.sv", "test/explicit/dep.sv"};
    std::set<std::string> check_mem = {"test/mem_init.mem"};
    std::set<std::string> check_pkg = {"test/pkg.sv"};
    ASSERT_EQ(res.get_dependencies(), check);
    ASSERT_EQ(res.get_data(), check_mem);
    ASSERT_EQ(res.get_packages(), check_pkg);
}
