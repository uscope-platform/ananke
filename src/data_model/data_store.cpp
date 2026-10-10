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

bool data_store::paths_match(const std::string &a, const std::string &b) {
    // Stored paths may be absolute while recorded ones are relative (or vice
    // versa), so match on equality or suffix — same tolerance as the
    // deconfliction lookup below.
    if (a.empty() || b.empty()) return false;
    return a == b || a.ends_with(b) || b.ends_with(a);
}

std::optional<std::string> data_store::resolve_cache_key(const std::string &p) const {
    if (p.empty()) return std::nullopt;
    if (cache.contains(p)) return p;
    for (const auto &key : cache | std::views::keys) {
        if (paths_match(key, p)) return key;
    }
    return std::nullopt;
}

bool data_store::file_includes(const std::string &from, const std::string &target) const {
    // True when `target` is reachable from `from` through the recorded
    // `include graph (transitive). Bounded by a visited set, so include
    // cycles cannot loop forever.
    if (from.empty() || target.empty()) return false;
    auto start = resolve_cache_key(from);
    if (!start.has_value()) return false;
    std::set<std::string> visited{start.value()};
    std::vector<std::string> stack{start.value()};
    while (!stack.empty()) {
        auto cur = std::move(stack.back());
        stack.pop_back();
        auto deps = get_includes(cur);
        if (!deps.has_value()) continue;
        for (const auto &dep : deps.value()) {
            if (paths_match(dep.path, target)) return true;
            auto key = resolve_cache_key(dep.path);
            if (key.has_value() && visited.insert(key.value()).second) {
                stack.push_back(key.value());
            }
        }
    }
    return false;
}

template<class StmtT>
std::vector<std::pair<std::shared_ptr<StmtT>, std::string>> data_store::drop_include_shadowed(
    const std::vector<std::pair<std::shared_ptr<StmtT>, std::string>> &hits) const {
    // A module defined in def.sv and `included by top.sv is parsed twice:
    // once standalone under def.sv and once inlined under top.sv. Both cache
    // entries then surface as "duplicates" of one logical definition. Drop
    // any hit whose file includes another hit's file — the surviving hit is
    // the canonical defining file. Genuine duplicates (no include edge
    // between the hits) are untouched. Never returns empty: on full cycles
    // fall back to the unfiltered set and let the caller warn as before.
    if (hits.size() <= 1) return hits;
    std::vector<std::pair<std::shared_ptr<StmtT>, std::string>> kept;
    for (size_t i = 0; i < hits.size(); ++i) {
        bool shadowed = false;
        for (size_t j = 0; j < hits.size(); ++j) {
            if (i != j && file_includes(hits[i].second, hits[j].second)) {
                shadowed = true;
                break;
            }
        }
        if (!shadowed) kept.push_back(hits[i]);
    }
    return kept.empty() ? hits : kept;
}

template<class StmtT>
std::optional<std::pair<std::shared_ptr<StmtT>, std::string>> data_store::pick_stmt(
    const std::string &kind,
    const std::vector<std::pair<std::shared_ptr<StmtT>, std::string>> &hits,
    const std::string &name) {
    if (hits.empty()) return std::nullopt;
    if (hits.size() == 1) return hits.front();
    // Collapse include shadows first: same definition seen via an `include
    // is a single logical resource, not a conflict — resolve silently to
    // the defining file.
    auto filtered = drop_include_shadowed(hits);
    if (filtered.size() == 1) return filtered.front();
    if (auto it = deconfliction.find(name); it != deconfliction.end()) {
        for (const auto &hit : filtered) {
            const auto &stored = hit.second, &wanted = it->second;
            if (paths_match(stored, wanted)) {
                // Explicit pick: configured by the user, so no conflict
                // warning — just trace the decision.
                spdlog::trace("Deconflicted {} '{}' → {}", kind, name, hit.second);
                return hit;
            }
        }
        report_dups<StmtT>(kind, name, filtered, filtered.front().second);
        spdlog::warn("Deconfliction entry for '{}' ('{}') matches no candidate; using {}",
                     name, it->second, filtered.front().second);
        return filtered.front();
    }
    report_dups<StmtT>(kind, name, filtered, filtered.front().second);
    return filtered.front();
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
    // Same `include-shadowing collapse as pick_stmt: one logical package
    // seen via an `include is not a conflict.
    hits = drop_include_shadowed(hits);
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

std::optional<std::shared_ptr<hdl_class_statement>> data_store::get_class(const std::string& name) {
    return get_one_impl<hdl_class_statement>("class", name);
}

std::optional<std::shared_ptr<hdl_class_statement>> data_store::get_class(const std::string &name,
    std::string &path) {
    return get_one_path_impl<hdl_class_statement>("class", name, path);
}

std::vector<std::shared_ptr<hdl_class_statement>> data_store::get_all_classes(const std::string& name) {
    return get_all_impl<hdl_class_statement>(name);
}

std::shared_ptr<hdl_resource_statement> data_store::interface_view(
    const std::shared_ptr<hdl_interface_statement> &iface) {
    auto view = std::make_shared<hdl_resource_statement>();
    view->set_name(iface->getName());
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

std::unordered_map<std::string, std::vector<stored_macro_def>> data_store::get_all_macro_definitions() const {
    std::unordered_map<std::string, std::vector<stored_macro_def>> result;
    for (const auto &[path, file] : cache) {
        if (!file.macro_definitions.empty()) result[path] = file.macro_definitions;
    }
    return result;
}

std::vector<std::string> data_store::get_macro_dependencies(const std::string &name) const {
    if (!cache.contains(name)) return {};
    return cache.at(name).macro_dependencies;
}

std::vector<std::string> data_store::get_files_with_macro_dependencies() const {
    std::vector<std::string> result;
    for (const auto &[path, file] : cache) {
        if (!file.macro_dependencies.empty()) result.push_back(path);
    }
    return result;
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
            macro_table_fingerprint.clear();
            include_file_hashes.clear();
            return;
        }
        cereal::BinaryInputArchive archive_in(is);
        std::string schema_hash;
        archive_in(schema_hash);
        if (schema_hash != get_cache_schema_hash()) {
            spdlog::warn("Cache schema changed, discarding stale cache {}", unified_cache);
            cache.clear();
            macro_table_fingerprint.clear();
            include_file_hashes.clear();
            return;
        }
        archive_in(cache);
        // Fingerprint entry postdates older archives; absence means
        // "unknown previous table", which conservatively forces macro
        // dependent revalidation downstream.
        try {
            archive_in(macro_table_fingerprint);
        } catch (const std::exception &) {
            macro_table_fingerprint.clear();
        }
        // Header-hash entry postdates older archives; absence just means
        // "no previously tracked headers" (one conservative re-parse pass
        // for `.h` consumers), never a reason to discard the cache.
        try {
            archive_in(include_file_hashes);
        } catch (const std::exception &) {
            include_file_hashes.clear();
        }
    } catch (const std::exception &e) {
        spdlog::warn("Could not load cache file {} ({}), starting with an empty cache", unified_cache, e.what());
        cache.clear();
        macro_table_fingerprint.clear();
        include_file_hashes.clear();
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
        archive_out(get_cache_schema_hash(), cache, macro_table_fingerprint, include_file_hashes);
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
    // NOTE: include_file_hashes is deliberately not pruned here. A deleted
    // header must stay detectable (previous real hash vs current missing),
    // so its consumers are invalidated once and re-parse with the usual
    // "include file not found" warning. Unreferenced entries are dropped by
    // the walker's refresh once nothing consumes them anymore.
}

void data_store::remove_stale_info(const std::filesystem::path& p) {
    std::vector<std::string> evicted_items;
    cache.erase(p);
}


