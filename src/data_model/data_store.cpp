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
    for (auto &[_, file]: cache.cache_content) {
        if (!std::holds_alternative<std::vector<HDL_Resource>>(file.content)) continue;
        for (auto &res:std::get<std::vector<HDL_Resource>>(file.content)) {
            if (res.getName() == name) return res;
        }
    }
    return std::nullopt;
}

void data_store::store_file(const source_file &file) {
    cache.cache_content.insert({file.path, file});
}

void data_store::evict_file(const std::string &file) {
    cache.cache_content.erase(file);
}


std::string data_store::get_hash(const std::string &name) const {
    if (!cache.cache_content.contains(name)) return "";
    return cache.cache_content.at(name).hash;
}

bool data_store::contains(const std::string &name) const {
    return cache.cache_content.contains(name);
}


std::optional<Script> data_store::get_script(std::string &name) {
    for (auto &[_, file]: cache.cache_content) {
        if (!std::holds_alternative<Script>(file.content)) continue;
        auto scr = std::get<Script>(file.content);
        if (scr.get_name() == name) return scr;
    }
    return std::nullopt;
}

Constraints data_store::get_constraint(std::string &name) {

    return cache.constraints[name];
}


void data_store::store_constraint(const std::vector<Constraints> &vect, const std::string &hash,const std::string &path) {
    for(const auto &item: vect){
        cache.constraints[item.get_name()] = item;
    }
        cache.constraints_hash[path]= hash;
}

    DataFile data_store::get_data_file(const std::string &name) {
        return cache.data[name];
    }


void data_store::store_data_file(const std::vector<DataFile> &vect, const std::string &hash,const std::string &path) {
    for(auto &item: vect){
        cache.data[item.get_name()] = item;
    }
    cache.data_hash[path] = hash;
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
    for (auto  [key, val] : cache.hdl){
        if(!std::filesystem::exists(val.get_path())){
            evicted_items.push_back(key);
        }
    }
    for (const auto &item : evicted_items){
        cache.hdl.erase(item);
    }
    evicted_items.clear();

    for (auto  [key, val] : cache.interfaces){
        if(!std::filesystem::exists(val.get_path())){
            evicted_items.push_back(key);
        }
    }
    for (const auto &item : evicted_items){
        cache.interfaces.erase(item);
    }
    evicted_items.clear();


    for (auto  [key, val] : cache.scripts){
        if(!std::filesystem::exists(val.get_path())){
            evicted_items.push_back(key);
        }
    }
    for (const auto &item : evicted_items){
        cache.scripts.erase(item);
    }
    evicted_items.clear();


    for (auto  [key, val] : cache.constraints){
        if(!std::filesystem::exists(val.get_path())){
            evicted_items.push_back(key);
        }
    }
    for (const auto &item : evicted_items){
        cache.constraints.erase(item);
    }
}

void data_store::remove_stale_info(const std::filesystem::path& p) {
    std::vector<std::string> evicted_items;

    for (auto  [key, val] : cache.hdl){
        if(val.get_path()==p.string()){
            evicted_items.push_back(key);
        }
    }
    for (const auto &item : evicted_items){
        cache.hdl.erase(item);
    }
    evicted_items.clear();

    for (auto  [key, val] : cache.interfaces){
        if(val.get_path()==p.string()){
            evicted_items.push_back(key);
        }
    }
    for (const auto &item : evicted_items){
        cache.interfaces.erase(item);
    }
    evicted_items.clear();


    for (auto  [key, val] : cache.scripts){
        if(val.get_path()==p.string()){
            evicted_items.push_back(key);
        }
    }
    for (const auto &item : evicted_items){
        cache.scripts.erase(item);
    }
    evicted_items.clear();

    for (auto  [key, val] : cache.constraints){
        if(val.get_path()==p.string()){
            evicted_items.push_back(key);
        }
    }
    for (const auto &item : evicted_items){
        cache.constraints.erase(item);
    }
    evicted_items.clear();

    for (auto  [key, val] : cache.data){
        if(val.get_path()==p.string()){
            evicted_items.push_back(key);
        }
    }
    for (const auto &item : evicted_items){
        cache.data.erase(item);
    }

}


void data_store::evict_constraint(const std::string &name) {
    cache.constraints.erase(name);
}


void data_store::evict_data_file(const std::string &name) {
    cache.data.erase(name);
}

std::unordered_map<std::string, HDL_Resource> data_store::get_hdl_cache() {
    std::unordered_map<std::string, HDL_Resource> ret_val;

    ret_val.insert(cache.hdl.begin(), cache.hdl.end());
    ret_val.insert(cache.interfaces.begin(), cache.interfaces.end());

    return ret_val;
}

