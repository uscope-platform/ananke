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

#include "data_model/data_store.hpp"

#include <utility>


data_store::data_store(bool e, std::string cache_dir_path) {
    ephemeral = e;
    store_path = std::move(cache_dir_path);
    std::filesystem::create_directory(store_path);

    unified_cache = store_path + "/unified_cache";

    if(std::filesystem::exists(unified_cache) & !ephemeral){
        load_cache();
    }
    clean_up_caches();
}

std::optional<HDL_Resource> data_store::get_HDL_resource(const std::string& name) {
    for (auto &file: cache | std::views::values) {
        if (!std::holds_alternative<std::vector<HDL_Resource>>(file.content)) continue;
        for (auto &res:std::get<std::vector<HDL_Resource>>(file.content)) {
            if (res.getName() == name) return res;
        }
    }
    return std::nullopt;
}

void data_store::store_file(const source_file &file) {
    cache.insert({file.path, file});
}

void data_store::evict_file(const std::string &file) {
    cache.erase(file);
}


std::string data_store::get_hash(const std::string &name) const {
    if (!cache.contains(name)) return "";
    return cache.at(name).hash;
}

bool data_store::contains(const std::string &name) const {
    return cache.contains(name);
}


std::optional<Script> data_store::get_script(std::string &name) {
    for (auto &file: cache | std::views::values) {
        if (!std::holds_alternative<Script>(file.content)) continue;
        auto scr = std::get<Script>(file.content);
        if (scr.get_name() == name) return scr;
    }
    return std::nullopt;
}

std::optional<Constraints> data_store::get_constraint(const std::string &name) {
    for (auto &file: cache | std::views::values) {
        if (!std::holds_alternative<Constraints>(file.content)) continue;
        auto c = std::get<Constraints>(file.content);
        if (c.get_name() == name) return c;
    }
    return std::nullopt;
}


 std::optional<DataFile> data_store::get_data_file(const std::string &name) {
    for (auto &[_, file]: cache) {
        if (!std::holds_alternative<DataFile>(file.content)) continue;
        auto df = std::get<DataFile>(file.content);
        if (df.get_name() == name) return df;
    }
    return std::nullopt;
    }




bool data_store::is_primitive(const std::string &name) {
    return xilinx_primitives.find(name) != xilinx_primitives.end();
}

data_store::~data_store() {
    if(!ephemeral){
        store_cache();
    }
}


void data_store::load_cache() {
    std::ifstream is(unified_cache, std::ios_base::binary);

    cereal::BinaryInputArchive archive_in(is);
    archive_in(cache);

}


void data_store::store_cache() {
    std::filesystem::remove(unified_cache);
    std::ofstream os(unified_cache, std::ios_base::binary);
    {
        cereal::BinaryOutputArchive archive_out(os);
        archive_out(cache);
    }

}

void data_store::clean_up_caches() {
    std::vector<std::string> evicted_items;
    for (auto &path: cache | std::views::keys) {
        if(!std::filesystem::exists(path)){
           cache.erase(path);
        }
    }
}

void data_store::remove_stale_info(const std::filesystem::path& p) {
    std::vector<std::string> evicted_items;
    cache.erase(p);
}


