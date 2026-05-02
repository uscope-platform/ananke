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

#include "frontend/cache_manager.hpp"

cache_manager::cache_manager(std::shared_ptr<settings_store> settings, std::shared_ptr<data_store> data, bool no_cache) {
    s_store = std::move(settings);
    d_store = std::move(data);
    ephimeral = no_cache;
    if(!ephimeral){
        load_cache_backend();
    }


}

void cache_manager:: add_file(std::filesystem::path &file) {
    if(ephimeral) return;
    d_store->remove_stale_info(file);
    cache[file.string()] = hash_file(file);
}

bool cache_manager::is_changed(std::filesystem::path &file) {
    if(ephimeral) return true;
    bool ret_val;
    std::string current_digest = hash_file(file);
    ret_val = current_digest != cache[file.string()];

    return ret_val;
}

std::string cache_manager::hash_file(std::filesystem::path &file) {


    std::ifstream in(file);
    std::string file_content;
    if (in)
    {
        in.seekg(0, std::ios::end);
        file_content.resize(in.tellg());
        in.seekg(0, std::ios::beg);
        in.read(&file_content[0], file_content.size());
        in.close();
    }


    EVP_MD_CTX* context = EVP_MD_CTX_new();

    if(context != nullptr){
        if(EVP_DigestInit_ex(context, EVP_sha256(), nullptr)) {
            if(EVP_DigestUpdate(context, file_content.c_str(), file_content.length())) {
                unsigned char hash[EVP_MAX_MD_SIZE];
                unsigned int lengthOfHash = 0;

                if(EVP_DigestFinal_ex(context, hash, &lengthOfHash)) {
                    std::stringstream ss;
                    for(unsigned int i = 0; i < lengthOfHash; ++i) {
                        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
                        if(i<lengthOfHash-1){
                            ss<<":";
                        }
                    }
                    EVP_MD_CTX_free(context);
                    return ss.str();
                }
            }
        }
    }


    // If I get here there must have been some error with the hash calculation, throw an exception
    throw std::runtime_error("ERROR: could not calculate file hash");
}

bool cache_manager::is_cached(std::filesystem::path &file) {
    if(ephimeral) return false;
    return cache.count(file.string())>0;
}

void cache_manager::purge() {
    cache.clear();
}

void cache_manager::load_cache_backend() {
    std::string test = s_store->get_setting("cache_dump");
    std::istringstream cache_stream(s_store->get_setting("cache_dump"));

    std::string cache_line;
    while (std::getline(cache_stream, cache_line,';')){
        std::istringstream ss(cache_line);
        std::string tokens[2];
        int i = 0;
        while(std::getline(ss, tokens[i], '>')) {
            i++;
        }
        cache[tokens[0]] = tokens[1];
    }
}

void cache_manager::store_cache_backend() {
    std::ostringstream cache_str;
    for(const auto& item: cache){
        cache_str << item.first << ">" << item.second << ";";
    }
    s_store->set_setting("cache_dump", cache_str.str());
}

cache_manager::~cache_manager() {
    if(!ephimeral){
        store_cache_backend();
    }

}
