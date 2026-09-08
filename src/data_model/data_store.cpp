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
#include <spdlog/spdlog.h>

#include <cereal/types/polymorphic.hpp>

// These types are defined entirely in their headers, so their registration
// translation units contain no referenced symbols and would be dropped by the
// linker. Force the registration to be pulled in from the cache serializer.
CEREAL_FORCE_DYNAMIC_INIT(HDL_union_type)
CEREAL_FORCE_DYNAMIC_INIT(Type_ref)

// The store keys files by path, so several files can contribute same-named
// resources; hits pair each candidate with its source path.
std::vector<data_store::resource_hit> data_store::find_resources_by_name(
    const std::string &name, const std::string &arch, bool match_arch) {
    std::vector<resource_hit> hits;
    for (auto &file: cache | std::views::values) {
        if (!std::holds_alternative<hdl_file>(file.content)) continue;
        for (auto &res: std::get<hdl_file>(file.content).get_content()) {
            if (!res->is<hdl_resource_statement>()) continue;
            auto &r = res->as<hdl_resource_statement>();
            if (r.getName() != name) continue;
            if (match_arch && r.get_architecture() != arch) continue;
            if (!match_arch && !r.get_architecture().empty()) continue;
            hits.emplace_back(std::static_pointer_cast<hdl_resource_statement>(res), file.path);
        }
    }
    return hits;
}

// Single-pick policy shared by the get_HDL_resource overloads: unique hits
// pass through silently, duplicates defer to the deconfliction map (stored
// paths may be absolute while entries are relative, so match on equality or
// suffix) and warn when nothing (or nothing matching) is configured.
std::optional<data_store::resource_hit> data_store::pick_resource(
    const std::vector<resource_hit> &hits, const std::string &name) {
    if (hits.empty()) return std::nullopt;
    if (hits.size() == 1) return hits.front();
    if (auto it = deconfliction.find(name); it != deconfliction.end()) {
        for (const auto &hit : hits) {
            const auto &stored = hit.second, &wanted = it->second;
            if (!stored.empty() && !wanted.empty() &&
                (stored == wanted || stored.ends_with(wanted) || wanted.ends_with(stored))) {
                spdlog::info("Deconflicted resource '{}' → {}", name, hit.second);
                return hit;
            }
        }
        spdlog::warn("Deconfliction entry for '{}' ('{}') matches no candidate; using {}",
                     name, it->second, hits.front().second);
        return hits.front();
    }
    std::string paths;
    for (const auto &hit : hits) {
        paths += "\n\t";
        paths += hit.second.empty() ? "<unknown path>" : hit.second;
    }
    spdlog::warn("Multiple resources named '{}' found:{}\n\tusing {}", name, paths, hits.front().second);
    return hits.front();
}



data_store::data_store(bool e, std::string cache_dir_path) {
    ephemeral = e;
    store_path = std::move(cache_dir_path);
    std::error_code ec;
    std::filesystem::create_directories(store_path, ec);
    if (ec) {
        spdlog::warn("Could not create cache directory {}: {}", store_path, ec.message());
    }

    unified_cache = store_path + "/unified_cache";

    if (std::filesystem::exists(unified_cache) && !ephemeral) {
        load_cache();
    }
    clean_up_caches();
}

std::vector<std::shared_ptr<hdl_resource_statement>> data_store::get_all_HDL_resources(const std::string& name) {
    auto hits = find_resources_by_name(name, "", false);
    std::vector<std::shared_ptr<hdl_resource_statement>> out;
    out.reserve(hits.size());
    for (auto &[res, path] : hits) out.push_back(std::move(res));
    return out;
}

std::optional<std::shared_ptr<hdl_resource_statement>> data_store::get_HDL_resource(const std::string& name) {
    if (auto picked = pick_resource(find_resources_by_name(name, "", false), name))
        return picked->first;
    return std::nullopt;
}

std::optional<std::shared_ptr<hdl_resource_statement>> data_store::get_HDL_resource(const std::string &name,
    const std::string &arch) {
    if (auto picked = pick_resource(find_resources_by_name(name, arch, true), name))
        return picked->first;
    return std::nullopt;
}

std::optional<std::shared_ptr<hdl_resource_statement>> data_store::get_HDL_resource(const std::string &name,
    std::string &path) {
    if (auto picked = pick_resource(find_resources_by_name(name, "", false), name)) {
        path = picked->second;
        return picked->first;
    }
    return std::nullopt;
}

void data_store::store_file(const cached_item &file) {
    cache.insert_or_assign(file.path, file);
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

std::optional<hdl_function_statement> data_store::get_standalone_function(const std::string &name, const std::string &source_path) {
    if (!cache.contains(source_path)) return std::nullopt;
    auto &file = cache.at(source_path);
    if (!std::holds_alternative<hdl_file>(file.content)) return std::nullopt;
    for (auto &stmt : std::get<hdl_file>(file.content).get_content()) {
        auto f = std::dynamic_pointer_cast<hdl_function_statement>(stmt);
        if (f && f->get_name() == name) return *f;
    }
    return std::nullopt;
}

std::optional<std::vector<include_dependency>> data_store::get_includes(const std::string &name) const {
    if (!cache.contains(name)) return std::nullopt;
    return cache.at(name).includes;
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
    try {
        std::ifstream is(unified_cache, std::ios_base::binary);
        if (!is.good()) {
            spdlog::warn("Could not open cache file {}, starting with an empty cache", unified_cache);
            cache.clear();
            return;
        }
        cereal::BinaryInputArchive archive_in(is);
        std::string schema_hash;
        archive_in(schema_hash);
        if (schema_hash != get_cache_schema_hash()) {
            spdlog::warn("Cache schema changed, discarding stale cache {}", unified_cache);
            cache.clear();
            return;
        }
        archive_in(cache);
    } catch (const std::exception &e) {
        spdlog::warn("Could not load cache file {} ({}), starting with an empty cache", unified_cache, e.what());
        cache.clear();
    }
}


void data_store::store_cache() {
    try {
        std::error_code ec;
        std::filesystem::remove(unified_cache, ec);
        std::ofstream os(unified_cache, std::ios_base::binary);
        if (!os.good()) {
            spdlog::error("Could not write cache file {}", unified_cache);
            return;
        }
        cereal::BinaryOutputArchive archive_out(os);
        archive_out(get_cache_schema_hash(), cache);
    } catch (const std::exception &e) {
        spdlog::error("Could not save cache file {}: {}", unified_cache, e.what());
    }
}

void data_store::clean_up_caches() {
    std::vector<std::string> stale_paths;
    for (const auto &path : cache | std::views::keys) {
        std::error_code ec;
        if (!std::filesystem::exists(path, ec)) {
            stale_paths.push_back(path);
        }
    }
    for (const auto &path : stale_paths) {
        cache.erase(path);
    }
}

void data_store::remove_stale_info(const std::filesystem::path& p) {
    std::vector<std::string> evicted_items;
    cache.erase(p);
}


