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

#ifndef ANANKE_DATA_STORE_HPP
#define ANANKE_DATA_STORE_HPP

#include <functional>
#include <unordered_map>
#include <filesystem>
#include <utility>
#include <set>
#include <fstream>

#include <cereal/archives/binary.hpp>
#include <cereal/types/memory.hpp>
#include <utility>

#include "Script.hpp"
#include "Constraints.hpp"
#include "DataFile.hpp"
#include "hdl_file.hpp"
#include "include_dependency.hpp"
#include "macro_definition.hpp"
#include "cache_schema_hash.hpp"


class data_store {
public:


    using source_content = std::variant<
        Script,
        DataFile,
        Constraints,
        hdl_file
    >;

    struct cached_item {
        std::string path;
        std::string hash;
        source_content content;
        std::vector<include_dependency> includes;
        // Harvested preprocessor macros (order-dependent repository macros).
        // Persisted so cache-skipped files still contribute to the macro
        // table, and macro dependents can be invalidated across runs.
        std::vector<stored_macro_def> macro_definitions;
        // Repository macro names this file's parse consumed via injection.
        std::vector<std::string> macro_dependencies;
        template<class Archive> void serialize(Archive & ar) {
            ar(path, hash, content, includes, macro_definitions, macro_dependencies);
        }
    };

    data_store(bool e, std::string cache_dir_path);
    // NEW IF
    void store_file(const cached_item &file);
    // Preferred source path per resource name, from the Depfile
    // "deconfliction" section. Used to pick among duplicate resources.
    void set_deconfliction(const std::unordered_map<std::string, std::string> &d) { deconfliction = d; }
    void evict_file(const std::string &file);
    template<typename T> std::optional<T> get_file(const std::string &name) const;
    [[nodiscard]] std::string get_hash(const std::string &name) const;
    bool contains(const std::string &name)const;

    std::optional<std::shared_ptr<hdl_resource_statement>> get_HDL_resource(const std::string& name);
    std::vector<std::shared_ptr<hdl_resource_statement>> get_all_HDL_resources(const std::string& name);
    // Package lookups: packages are siblings of resources now, stored as
    // hdl_package_statement in the same hdl_file content.
    std::optional<std::shared_ptr<hdl_package_statement>> get_package(const std::string& name);
    std::optional<std::shared_ptr<hdl_package_statement>> get_package(const std::string& name, std::string &path);
    std::vector<std::shared_ptr<hdl_package_statement>> get_all_packages(const std::string& name);
    // Interface lookups: interfaces elaborate like modules (same solver and
    // builder paths) but are stored as hdl_interface_statement.
    std::optional<std::shared_ptr<hdl_interface_statement>> get_interface(const std::string& name);
    std::optional<std::shared_ptr<hdl_interface_statement>> get_interface(const std::string& name, std::string &path);
    std::vector<std::shared_ptr<hdl_interface_statement>> get_all_interfaces(const std::string& name);
    // Class lookups: SystemVerilog classes are tracked as definition-only
    // hdl_class_statement entities (no elaboration yet).
    std::optional<std::shared_ptr<hdl_class_statement>> get_class(const std::string& name);
    std::optional<std::shared_ptr<hdl_class_statement>> get_class(const std::string& name, std::string &path);
    std::vector<std::shared_ptr<hdl_class_statement>> get_all_classes(const std::string& name);
    // Elaboratable lookup for the shared module/interface paths (builder,
    // solvers, dependency resolution): modules first, then interfaces as a
    // transient resource view sharing the underlying objects, so downstream
    // code keeps working on hdl_resource_statement. Path is set on success.
    std::optional<std::shared_ptr<hdl_resource_statement>> get_elaboratable(const std::string& name);
    std::optional<std::shared_ptr<hdl_resource_statement>> get_elaboratable(const std::string& name, std::string &path);
    // Owner lookups: among same-named packages, resolve to the one declaring
    // the wanted member. Explicit deconfliction entries win; a unique owner
    // wins over first-match; otherwise (nobody or several declare it) falls
    // back to the legacy pick with its warnings, so downstream
    // missing-handling is unchanged. Nullopt only when the package itself
    // has no candidates at all.
    std::optional<std::shared_ptr<hdl_package_statement>> get_package_param_owner(
        const std::string& pkg, const qualified_identifier& dep);
    std::optional<std::shared_ptr<hdl_package_statement>> get_package_typedef_owner(
        const std::string& pkg, const std::string& type_name);
    std::optional<std::shared_ptr<hdl_package_statement>> get_package_function_owner(
        const std::string& pkg, const std::string& func_name);
    // Member owner across all categories (param, typedef, function): for
    // selective `import pkg::ITEM`, where the member kind is not known upfront.
    // Same owner policy as the per-kind lookups above.
    std::optional<std::shared_ptr<hdl_package_statement>> get_package_member_owner(
        const std::string& pkg, const std::string& member);
    std::optional<std::shared_ptr<hdl_resource_statement>> get_HDL_resource(const std::string& name, const std::string &arch);
    std::optional<std::shared_ptr<hdl_resource_statement>> get_HDL_resource(const std::string& name, std::string &path);
    std::optional<Script> get_script(std::string& name);
    std::optional<DataFile> get_data_file(const std::string& name);
    std::optional<Constraints> get_constraint(const std::string& name);
    std::optional<hdl_function_statement> get_standalone_function(const std::string &name, const std::string &source_path);
    std::optional<std::vector<include_dependency>> get_includes(const std::string &name) const;
    // Repository macro table support (compilation-order macros): per-file
    // harvested definitions for table seeding, per-file consumed macro names
    // for cross-run invalidation, and the persisted table fingerprint used to
    // detect inter-run macro changes.
    std::unordered_map<std::string, std::vector<stored_macro_def>> get_all_macro_definitions() const;
    std::vector<std::string> get_macro_dependencies(const std::string &name) const;
    std::vector<std::string> get_files_with_macro_dependencies() const;
    [[nodiscard]] std::string get_macro_table_fingerprint() const { return macro_table_fingerprint; }
    void set_macro_table_fingerprint(const std::string &fp) { macro_table_fingerprint = fp; }
    // OLD IF

    void remove_stale_info(const std::filesystem::path& p);
    bool is_primitive(const std::string &name);

    static void clear_cache(const std::string &cache_dir_path) {
        std::filesystem::remove_all( cache_dir_path + "/unified_cache");
    }
    static std::string get_cache_schema_hash() { return ANANKE_CACHE_SCHEMA_HASH; }
    ~data_store();
private:
    using package_predicate = std::function<bool(const std::shared_ptr<hdl_package_statement>&)>;
    // Generic lookup cores shared by the resource/package/interface trios:
    // StmtT is the statement type, kind is the singular log word
    // ("resource", "package", "interface", pluralized as "{}s" in messages).
    // Only hdl_resource_statement honors the arch filter.
    template<class StmtT> using stmt_hit = std::pair<std::shared_ptr<StmtT>, std::string>;
    template<class StmtT> std::vector<stmt_hit<StmtT>> find_by_name(
        const std::string &name, const std::string &arch, bool match_arch);
    template<class StmtT> void report_dups(
        const std::string &kind, const std::string &name,
        const std::vector<stmt_hit<StmtT>> &hits, const std::string &picked_path);
    template<class StmtT> std::optional<stmt_hit<StmtT>> pick_stmt(
        const std::string &kind, const std::vector<stmt_hit<StmtT>> &hits, const std::string &name);
    template<class StmtT> std::vector<std::shared_ptr<StmtT>> get_all_impl(const std::string &name);
    template<class StmtT> std::optional<std::shared_ptr<StmtT>> get_one_impl(
        const std::string &kind, const std::string &name);
    template<class StmtT> std::optional<std::shared_ptr<StmtT>> get_one_path_impl(
        const std::string &kind, const std::string &name, std::string &path);
    std::optional<std::shared_ptr<hdl_package_statement>> pick_owned_package(
        const std::string &name, const std::string &member, const package_predicate &declares);
    void clean_up_caches();
    void load_cache();
    void store_cache();
    // Transient shared view of an interface as a resource (same statements,
    // typedefs and functions; empty ports/docs). Lets the elaboration paths
    // stay single-typed; mutations land on the shared objects.
    static std::shared_ptr<hdl_resource_statement> interface_view(
        const std::shared_ptr<hdl_interface_statement> &iface);


    std::unordered_map<std::string, cached_item> cache;
    std::string macro_table_fingerprint;
    std::unordered_map<std::string, std::string> deconfliction;
    std::set<std::string> reported_duplicates;
    bool ephemeral;

    std::string store_path;
    std::string unified_cache;

    std::set<std::string> xilinx_primitives = {
            "xpm_cdc_array_single", "xpm_cdc_async_rst", "xpm_cdc_gray", "xpm_cdc_handshake", "xpm_cdc_pulse", "xpm_cdc_single",
            "xpm_cdc_sync_rst", "xpm_fifo_async", "xpm_fifo_axis", "xpm_fifo_axif", "xpm_fifo_axil", "xpm_fifo_sync", "xpm_memory_dpdistram",
            "xpm_memory_dprom", "xpm_memory_sdpram", "xpm_memory_spram", "xpm_memory_sprom", "xpm_memory_tdpram", "BRAM_SDP_MACRO",
            "BRAM_SINGLE_MACRO", "BRAM_TDP_MACRO", "ADDMACC_MACRO", "ADDSUB_MACRO", "COUNTER_LOAD_MACRO", "COUNTER_TC_MACRO",
            "EQ_COMPARE_MACRO", "MACC_MACRO", "MULT_MACRO", "FIFO_DUALCLOCK_MACRO", "FIFO_SYNC_MACROv", "BSCANE2", "BUFG",
            "BUFGCE", "BUFGCE_1", "BUFGCTRL", "BUFGMUX", "BUFGMUX_1", "BUFGMUX_CTRL", "BUFH", "BUFHCE", "BUFIO", "BUFMR",
            "BUFMRCE", "BUFR", "CAPTUREE2", "CARRY4", "CFGLUT5", "DCIRESET", "DNA_PORT", "DSP48E1", "EFUSE_USR", "FDCE",
            "FDPE", "FDRE", "FDSE", "FIFO18E1", "FIFO36E1", "FRAME_ECCE2", "IBUF", "IBUF_IBUFDISABLE", "IBUF_INTERMDISABLE",
            "IBUFDS", "IBUFDS_DIFF_OUT", "IBUFDS_DIFF_OUT_IBUFDISABLE", "IBUFDS_DIFF_OUT_INTERMDISABLE", "IBUFDS_GTE2", "IBUFDS_IBUFDISABLE",
            "IBUFDS_INTERMDISABLE", "ICAPE2", "IDDR", "IDDR_2CLK", "IDELAYCTRL", "IDELAYE2", "IN_FIFO","IOBUF", "IOBUF_DCIEN",
            "IOBUF_INTERMDISABLE", "IOBUFDS", "IOBUFDS_DCIEN", "IOBUFDS_DIFF_OUT", "IOBUFD S_DIFF_OUT_DCIEN",
            "IOBUFDS_DIFF_OUT_INTERMDISABLE", "IOBUFDS_INTERMDISABLE", "ISERDESE2", "KEEPER", "LDCE", "LDPE", "LUT1",
            "LUT2", "LUT3", "LUT4", "LUT5", "LUT6", "LUT6_2", "MMCME2_ADV", "MMCME2_BASE", "MUXF7", "MUXF8", "OBUF", "OBUFDS",
            "OBUFT", "OBUFTDS", "ODDR", "ODELAYE2", "OSERDESE2", "OUT_FIFO", "PLLE2_ADV", "PLLE2_BASE", "PULLDOWN", "PULLUP",
            "RAM128X1D", "RAM128X1S", "RAM256X1S", "RAM32M", "RAM32X1D", "RAM32X1S", "RAM32X1S_1", "RAM32X2S", "RAM64M",
            "RAM64X1D", "RAM64X1S", "RAM64X1S_1", "RAMB18E1", "RAMB36E1", "ROM128X1", "ROM256X1", "ROM32X1", "ROM64X1",
            "SRL16E", "SRLC32E", "STARTUPE2", "USR_ACCESSE2", "XADC"
    };
};


template<typename T>
std::optional<T> data_store::get_file(const std::string &name) const {
    if (!cache.contains(name)) return std::nullopt;
    if (const auto* val = std::get_if<T>(&cache.at(name).content)) {
        return *val;
    }

    return std::nullopt;
}
#endif //ANANKE_DATA_STORE_HPP
