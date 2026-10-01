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
#include <type_traits>

// These types are defined entirely in their headers, so their registration
// translation units contain no referenced symbols and would be dropped by the
// linker. Force the registration to be pulled in from the cache serializer.
CEREAL_FORCE_DYNAMIC_INIT(HDL_union_type)
CEREAL_FORCE_DYNAMIC_INIT(Type_ref)
CEREAL_FORCE_DYNAMIC_INIT(LoopVar_token)

template<class StmtT>
std::vector<std::pair<std::shared_ptr<StmtT>, std::string>> data_store::find_by_name(
    const std::string &name, const std::string &arch, bool match_arch) {
    std::vector<std::pair<std::shared_ptr<StmtT>, std::string>> hits;
    for (auto &file: cache | std::views::values) {
        if (!std::holds_alternative<hdl_file>(file.content)) continue;
        for (auto &res: std::get<hdl_file>(file.content).get_content()) {
            if (!res->is<StmtT>()) continue;
            auto &r = res->as<StmtT>();
            if (r.getName() != name) continue;
            if constexpr (std::is_same_v<StmtT, hdl_resource_statement>) {
                if (match_arch && r.get_architecture() != arch) continue;
                if (!match_arch && !r.get_architecture().empty()) continue;
            }
            hits.emplace_back(std::static_pointer_cast<StmtT>(res), file.path);
        }
    }
    return hits;
}

template<class StmtT>
void data_store::report_dups(const std::string &kind, const std::string &name,
    const std::vector<std::pair<std::shared_ptr<StmtT>, std::string>> &hits,
    const std::string &picked_path) {
    if (hits.size() <= 1 || !reported_duplicates.insert(name).second) return;
    std::string paths;
    for (const auto &hit : hits) {
        paths += "\n\t";
        paths += hit.second.empty() ? "<unknown path>" : hit.second;
    }
    if (picked_path.empty())
        spdlog::warn("Multiple {}s named '{}' found:{}", kind, name, paths);
    else
        spdlog::warn("Multiple {}s named '{}' found:{}\n\tusing {}", kind, name, paths, picked_path);
}

template<class StmtT>
std::optional<std::pair<std::shared_ptr<StmtT>, std::string>> data_store::pick_stmt(
    const std::string &kind,
    const std::vector<std::pair<std::shared_ptr<StmtT>, std::string>> &hits,
    const std::string &name) {
    if (hits.empty()) return std::nullopt;
    if (hits.size() == 1) return hits.front();
    if (auto it = deconfliction.find(name); it != deconfliction.end()) {
        for (const auto &hit : hits) {
            const auto &stored = hit.second, &wanted = it->second;
            if (!stored.empty() && !wanted.empty() &&
                (stored == wanted || stored.ends_with(wanted) || wanted.ends_with(stored))) {
                // Explicit pick: configured by the user, so no conflict
                // warning — just trace the decision.
                spdlog::trace("Deconflicted {} '{}' → {}", kind, name, hit.second);
                return hit;
            }
        }
        report_dups<StmtT>(kind, name, hits, hits.front().second);
        spdlog::warn("Deconfliction entry for '{}' ('{}') matches no candidate; using {}",
                     name, it->second, hits.front().second);
        return hits.front();
    }
    report_dups<StmtT>(kind, name, hits, hits.front().second);
    return hits.front();
}

template<class StmtT>
std::vector<std::shared_ptr<StmtT>> data_store::get_all_impl(const std::string &name) {
    auto hits = find_by_name<StmtT>(name, "", false);
    std::vector<std::shared_ptr<StmtT>> out;
    out.reserve(hits.size());
    for (auto &[res, path] : hits) out.push_back(std::move(res));
    return out;
}

template<class StmtT>
std::optional<std::shared_ptr<StmtT>> data_store::get_one_impl(
    const std::string &kind, const std::string &name) {
    if (auto picked = pick_stmt<StmtT>(kind, find_by_name<StmtT>(name, "", false), name))
        return picked->first;
    return std::nullopt;
}

template<class StmtT>
std::optional<std::shared_ptr<StmtT>> data_store::get_one_path_impl(
    const std::string &kind, const std::string &name, std::string &path) {
    auto picked = pick_stmt<StmtT>(kind, find_by_name<StmtT>(name, "", false), name);
    if (!picked.has_value()) return std::nullopt;
    path = picked->second;
    return picked->first;
}

// Single-pick policy shared by the get_* lookup overloads: unique hits
// pass through silently, duplicates defer to the deconfliction map (stored
// paths may be absolute while entries are relative, so match on equality or
// suffix) and warn when nothing (or nothing matching) is configured.
// Conflict reports fire once per name per store lifetime, so hot solver
// loops don't flood the log, while no successful disambiguation ever hides
// the conflict itself.



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

// Shared owner policy: explicit deconfliction wins (silent, it was asked
// for); a unique declaring candidate wins over first-match but still
// reports the conflict; otherwise (nobody or several declare it) falls
// back to the legacy pick with its warnings.
std::optional<std::shared_ptr<hdl_package_statement>> data_store::pick_owned_package(
    const std::string &name, const std::string &member, const package_predicate &declares) {
    auto hits = find_by_name<hdl_package_statement>(name, "", false);
    if (hits.empty()) return std::nullopt;
    if (hits.size() == 1) return hits.front().first;
    if (!deconfliction.contains(name)) {
        std::optional<stmt_hit<hdl_package_statement>> owner;
        for (auto &hit : hits) {
            if (!declares(hit.first)) continue;
            if (owner.has_value()) break;
            owner = hit;
        }
        if (owner.has_value()) {
            report_dups<hdl_package_statement>("package", name, hits, owner->second);
            spdlog::info("Resolved '{}' in package '{}' to {}", member, name, owner->second);
            return owner->first;
        }
    }
    auto picked = pick_stmt<hdl_package_statement>("package", hits, name);
    if (!picked.has_value()) return std::nullopt;
    return picked->first;
}

std::vector<std::shared_ptr<hdl_resource_statement>> data_store::get_all_HDL_resources(const std::string& name) {
    return get_all_impl<hdl_resource_statement>(name);
}

std::optional<std::shared_ptr<hdl_resource_statement>> data_store::get_HDL_resource(const std::string& name) {
    return get_one_impl<hdl_resource_statement>("resource", name);
}

std::optional<std::shared_ptr<hdl_resource_statement>> data_store::get_HDL_resource(const std::string &name,
    const std::string &arch) {
    if (auto picked = pick_stmt<hdl_resource_statement>(
            "resource", find_by_name<hdl_resource_statement>(name, arch, true), name))
        return picked->first;
    return std::nullopt;
}

std::optional<std::shared_ptr<hdl_resource_statement>> data_store::get_HDL_resource(const std::string &name,
    std::string &path) {
    return get_one_path_impl<hdl_resource_statement>("resource", name, path);
}

std::optional<std::shared_ptr<hdl_package_statement>> data_store::get_package(const std::string& name) {
    return get_one_impl<hdl_package_statement>("package", name);
}

std::optional<std::shared_ptr<hdl_package_statement>> data_store::get_package(const std::string &name,
    std::string &path) {
    return get_one_path_impl<hdl_package_statement>("package", name, path);
}

std::vector<std::shared_ptr<hdl_package_statement>> data_store::get_all_packages(const std::string& name) {
    return get_all_impl<hdl_package_statement>(name);
}

std::optional<std::shared_ptr<hdl_interface_statement>> data_store::get_interface(const std::string& name) {
    return get_one_impl<hdl_interface_statement>("interface", name);
}

std::optional<std::shared_ptr<hdl_interface_statement>> data_store::get_interface(const std::string &name,
    std::string &path) {
    return get_one_path_impl<hdl_interface_statement>("interface", name, path);
}

std::vector<std::shared_ptr<hdl_interface_statement>> data_store::get_all_interfaces(const std::string& name) {
    return get_all_impl<hdl_interface_statement>(name);
}

std::shared_ptr<hdl_resource_statement> data_store::interface_view(
    const std::shared_ptr<hdl_interface_statement> &iface) {
    auto view = std::make_shared<hdl_resource_statement>();
    view->set_name(iface->getName());
    view->set_type(interface);
    view->set_language(iface->get_language());
    view->set_line_n(iface->get_line_n());
    for (auto &[n, t] : iface->get_typedefs()) view->add_typedef(n, t);
    for (auto &s : iface->get_statements()) view->add_statement(s);
    return view;
}

std::optional<std::shared_ptr<hdl_resource_statement>> data_store::get_elaboratable(const std::string& name) {
    std::string path;
    return get_elaboratable(name, path);
}

std::optional<std::shared_ptr<hdl_resource_statement>> data_store::get_elaboratable(
    const std::string& name, std::string &path) {
    if (auto picked = pick_stmt<hdl_resource_statement>(
            "resource", find_by_name<hdl_resource_statement>(name, "", false), name)) {
        path = picked->second;
        return picked->first;
    }
    if (auto picked = pick_stmt<hdl_interface_statement>(
            "interface", find_by_name<hdl_interface_statement>(name, "", false), name)) {
        path = picked->second;
        return interface_view(picked->first);
    }
    return std::nullopt;
}

std::optional<std::shared_ptr<hdl_package_statement>> data_store::get_package_param_owner(
    const std::string& pkg, const qualified_identifier& dep) {
    const auto inst = dep.get_instance();
    const std::string member = inst.empty() ? dep.get_name() : inst.front();
    return pick_owned_package(pkg, member,
        [&member](const std::shared_ptr<hdl_package_statement> &res) {
            for (const auto &p : res->get_parameter_statements()) {
                if (p->get_name() == member) return true;
            }
            return false;
        });
}

std::optional<std::shared_ptr<hdl_package_statement>> data_store::get_package_typedef_owner(
    const std::string& pkg, const std::string& type_name) {
    return pick_owned_package(pkg, type_name,
        [&type_name](const std::shared_ptr<hdl_package_statement> &res) {
            return res->get_typedefs().contains(type_name);
        });
}

std::optional<std::shared_ptr<hdl_package_statement>> data_store::get_package_function_owner(
    const std::string& pkg, const std::string& func_name) {
    return pick_owned_package(pkg, func_name,
        [&func_name](const std::shared_ptr<hdl_package_statement> &res) {
            return res->get_function_shared(func_name) != nullptr;
        });
}

std::optional<std::shared_ptr<hdl_package_statement>> data_store::get_package_member_owner(
    const std::string& pkg, const std::string& member) {
    return pick_owned_package(pkg, member,
        [&member](const std::shared_ptr<hdl_package_statement> &res) {
            for (const auto &p : res->get_parameter_statements()) {
                if (p && p->get_name() == member) return true;
            }
            if (res->get_typedefs().contains(member)) return true;
            return res->get_function_shared(member) != nullptr;
        });
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


