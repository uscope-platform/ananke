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

#include <unordered_map>
#include <filesystem>
#include <utility>
#include <set>
#include <fstream>

#include "data_model/HDL/HDL_Resource.hpp"
#include "Script.hpp"
#include "Constraints.hpp"
#include "DataFile.hpp"

struct cache_t{
    std::unordered_map<std::string, HDL_Resource> hdl;
    std::unordered_map<std::string, HDL_Resource> interfaces;
    std::unordered_map<std::string, Script> scripts;
    std::unordered_map<std::string, Constraints> constraints;
    std::unordered_map<std::string, DataFile> data;

    std::unordered_map<std::string, std::string> hdl_hash;
    std::unordered_map<std::string, std::string> interface_hash;
    std::unordered_map<std::string, std::string> scripts_hash;
    std::unordered_map<std::string, std::string> constraints_hash;
    std::unordered_map<std::string, std::string> data_hash;

    template <class Archive>
    void serialize( Archive & ar )
    {
        ar( hdl, scripts, constraints, interfaces, data, hdl_hash, interface_hash, scripts_hash, constraints_hash, data_hash);
    }
};

class data_store {
public:
    data_store(bool e, std::string cache_dir_path);
    HDL_Resource get_HDL_resource(const std::string& name);
    void store_hdl_entity(HDL_Resource& entity, const std::string &hash);
    void store_hdl_entity(std::vector<HDL_Resource> & vect, const std::string &hash);
    void evict_hdl_entity(const std::string& name);
    bool contains_hdl_entity(const std::string& name) const;


    Script get_script(std::string& name);
    void store_script(Script entity, const std::string &hash);
    void store_script(const std::vector<Script> & vect, const std::string &hash);
    void evict_script(const std::string& name);

    DataFile get_data_file(const std::string& name);
    void store_data_file(DataFile entity, const std::string &hash);
    void store_data_file(const std::vector<DataFile> & vect, const std::string &hash);
    void evict_data_file(const std::string& name);

    Constraints get_constraint(std::string& name);
    void store_constraint(Constraints entity, const std::string &hash);
    void store_constraint(const std::vector<Constraints> & vect, const std::string &hash);
    void evict_constraint(const std::string& name);

    void remove_stale_info(const std::filesystem::path& p);
    bool is_primitive(const std::string &name);

    std::unordered_map<std::string, HDL_Resource> get_hdl_cache();
    std::unordered_map<std::string, Script> get_scripts_cache() const {return cache.scripts;};
    std::unordered_map<std::string, Constraints> get_constraints_cache() const {return cache.constraints;};
    std::unordered_map<std::string, DataFile> get_data_cache() const {return cache.data;};


    static void clear_cache(const std::string &cache_dir_path) {
        std::filesystem::remove_all( cache_dir_path + "/unified_cache");
    }
    ~data_store();
private:
    void clean_up_caches();
    void load_cache();
    void store_cache();

    cache_t cache;
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





#endif //ANANKE_DATA_STORE_HPP
