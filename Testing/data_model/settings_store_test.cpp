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

TEST( settings_store_test , get_set_setting) {
    settings_store store(true, "/tmp/test_settings_store");
    store.set_setting("test_Setting", "test_123");
    std::string result = store.get_setting("test_Setting");
    store.remove_setting("test_setting");
    ASSERT_EQ(result, "test_123");
}

TEST( settings_store_test , ser_des) {
    auto *store_1 = new settings_store(false, "/tmp/test_settings_store");
    store_1->set_setting("test_Setting", "test_123");
    delete store_1;
    auto *store_2 = new settings_store(false, "/tmp/test_settings_store");
    std::string result = store_2->get_setting("test_Setting");
    store_2->remove_setting("test_setting");
    delete store_2;

    ASSERT_EQ(result, "test_123");
}