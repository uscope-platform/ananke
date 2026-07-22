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


#include "frontend/Repository_walker.hpp"



Repository_walker::Repository_walker(const std::shared_ptr<settings_store>& s, const std::shared_ptr<data_store>& d, bool ephimeral) : pool(max_threads){
    construct_walker(s, d, {".git"});
}

Repository_walker::Repository_walker(const std::shared_ptr<settings_store>& s, const std::shared_ptr<data_store>& d, bool ephimeral,std::set<std::string> ex) : pool(max_threads){
    construct_walker(s, d, std::move(ex));
}

void Repository_walker::construct_walker(std::shared_ptr<settings_store> s, std::shared_ptr<data_store> d,
                                         std::set<std::string> ex) {

    s_store = std::move(s);
    d_store = std::move(d);
    excluded_directories = std::move(ex);
    target_repository = s_store->get_hdl_store();
    default_includes = s_store->get_default_includes();
    analyze_dir();
}




/// This method analyses the Repository_walker target directory, mapping out useful things (like module dependencies, script location, etc.)
///
/// The implementation of this function iterates over the target directory recursively, calling the analyze_file method
/// on each file, to speed up the process directories can be skipped either based on their name (.git for example) or
/// on their content (exclude directories containing a .xpr file as they are vivado projecta and contain only auto-generated
/// stuff.
void Repository_walker::analyze_dir() {
    for(auto p_iter = std::filesystem::recursive_directory_iterator(target_repository);
        p_iter != std::filesystem::recursive_directory_iterator();
        ++p_iter ) {

        auto path = p_iter->path();
        std::string path_dbg = p_iter->path();
        if(std::filesystem::is_directory(path)){
            if(is_excluded_directory(path)){
                p_iter.disable_recursion_pending();
            } else{
                if(contains_excluding_file(path)){
                    p_iter.disable_recursion_pending();
                }
            }
        } else{
            if(working_threads == 2*max_threads){
               this->collect_analysis_results();
            }
            analyze_file(path);
        }
    }

    this->collect_analysis_results();
}

void Repository_walker::collect_analysis_results() {
    pool.wait_for_tasks();
    for(auto &f : hdl_futures){
        auto [path, file_hash, resource] = f.get();
        if (resource) {
            for (auto &res: resource.value())
                d_store->store_file({path, file_hash, std::vector{res}});
        }
    }
    for(auto &f : scripts_futures){
        auto [path, file_hash, resource] = f.get();
        if (resource) {
            for (auto &res: resource.value())
                d_store->store_file({path, file_hash, res});
        }
    }
    for(auto &f : constraints_futures){
        auto [path, file_hash, resource] = f.get();
        if (resource) {
            for (auto &res: resource.value())
                d_store->store_file({path, file_hash, res});
            d_store->store_constraint(resource.value(), file_hash, path);
        }
    }
    for(auto  &f: data_futures){
        auto [path, file_hash, resource] = f.get();
        if (resource) {
            for (auto &res: resource.value())
                d_store->store_file({path, file_hash, res});
        }
    }
    hdl_futures.erase(hdl_futures.begin(), hdl_futures.end());
    scripts_futures.erase(scripts_futures.begin(), scripts_futures.end());
    constraints_futures.erase(constraints_futures.begin(), constraints_futures.end());
    data_futures.erase(data_futures.begin(), data_futures.end());

    working_threads =0;
}


/// Check if the target directory needs to be skipped on the base of its name
/// \param dir Target directory
/// \return true if the directory needs to be skipped
bool Repository_walker::is_excluded_directory(const std::filesystem::path& dir) {
    auto norm_dir = dir.lexically_normal();

    for (const auto& excl_dir : excluded_directories) {
        if(excl_dir.starts_with('*')) {
            auto excl_name = excl_dir.substr(1, excl_dir.size() -1);
            if(norm_dir.filename() == excl_name) return true;
        }  else {
            if(norm_dir == std::filesystem::path(excl_dir).lexically_normal()) return true;
        }

    }
    return false;

}


/// Check if the target directory needs to be skipped on the base of its content
/// \param dir Target directory
/// \return true if the directory needs to be skipped
bool Repository_walker::contains_excluding_file(const std::filesystem::path &dir) {
    for(auto& p: std::filesystem::directory_iterator(dir)){
        if(!std::filesystem::is_directory(p.path())){
            bool is_excluded = excluding_extensions.find(p.path().extension()) != excluding_extensions.end();
            if(p.path().filename() == ignore_file_name){
                this->read_ignore_file(p.path());
            }
            if(is_excluded) return true;
        }
    }
    return false;
}

void Repository_walker::read_ignore_file(const std::filesystem::path &file) {
    std::ifstream content(file);
    std::string ignore_line;
    std::string ignore_path;
    while (std::getline(content, ignore_line)){
        ignore_path = file.parent_path().string() + "/" + ignore_line;
        if(std::filesystem::is_directory(ignore_path)) excluded_directories.insert(ignore_path);
    }

}



/// File analysis Dispatcher
/// \param file File to analyze
///
/// This method is a simple dispatcher that based on the file type calls the appropriate analysis method.
void Repository_walker::analyze_file(std::filesystem::path &file) {
    if(!(file_is_verilog(file) || file_is_vhdl(file) || file_is_constraint(file)|| file_is_script(file) || file_is_data(file))) return;

    spdlog::trace("Analizing file: {}", file.string());
    if(file_is_verilog(file)){
        auto old_hash = d_store->get_hash(file);
        hdl_futures.push_back(pool.submit(analyze_verilog, file, default_includes, old_hash));
        working_threads++;
    } else if(file_is_script(file)){
        std::set<std::string> includes;
        auto old_hash = d_store->get_hash(file);
        scripts_futures.push_back(pool.submit(analyze_script, file, includes, old_hash));
        working_threads++;
    } else if(file_is_vhdl(file)){
        auto old_hash = d_store->get_hash(file);
        hdl_futures.push_back(pool.submit(analyze_vhdl, file, default_includes, old_hash));
        working_threads++;
    } else if(file_is_constraint(file)){
        std::set<std::string> includes;
        auto old_hash = d_store->get_hash(file);
        constraints_futures.push_back(pool.submit(analyze_constraint, file, includes, old_hash));
        working_threads++;
    } else if(file_is_data(file)){
        std::set<std::string> includes;
        auto old_hash = d_store->get_hash(file);
        data_futures.push_back(pool.submit(analyze_data, file, includes, old_hash));
        working_threads++;
    }

}

/// Check if the target file appertains to the verilog language family
/// \param file Target file
/// \return True if the file is verilog
bool Repository_walker::file_is_verilog(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".svh" || extension == ".sv" || extension == ".vh" || extension == ".v";
}

/// Check if the target file appertains to the vhdl language family
/// \param file Target file
/// \return
bool Repository_walker::file_is_vhdl(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".vhd";
}

/// Check if the target file is a recognized script (TCL and python)
/// \param file Target file
/// \return true if the file is a recognized script
bool Repository_walker::file_is_script(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".tcl" || extension == ".py";
}

/// Check if the target is a constrain file
/// \param file Target file
/// \return true if the file is a constrain file
bool Repository_walker::file_is_constraint(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".xdc";
}

bool Repository_walker::file_is_data(const std::filesystem::path &file) {
    std::string extension = file.extension();
    return extension == ".dat" || extension == ".mem";
}


/// Analyze the target verilog-type file to extract declared and used instantiated design elements
/// \param file Target file
file_analysis_context<HDL_Resource> analyze_verilog(
    const std::filesystem::path &file,
    std::set<std::string> i_d, const std::string &old_hash
) {
    spdlog::trace("PARSING: {}", file.c_str());
    try {

        mm_file f(file);
        std::string hash = hash_file(f.view());
        if (old_hash == hash) {
            return {file.string(), hash, std::nullopt};
        }
        sv_analyzer file_processor;
        file_processor.set_include_directories(i_d);
        return {file.string(), hash,file_processor.analyze(file, f.view())};
    } catch (std::runtime_error &err) {
        spdlog::error(err.what());
        return {};
    }
}
std::string hash_file(const std::string_view &file_content) {

    EVP_MD_CTX* context = EVP_MD_CTX_new();

    if(context != nullptr){
        if(EVP_DigestInit_ex(context, EVP_sha256(), nullptr)) {
            if(EVP_DigestUpdate(context, file_content.begin(), file_content.length())) {
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
};

/// Analyze the target vhdl-type file to extract declared and used instantiated design elements
/// \param file Target file
file_analysis_context<HDL_Resource> analyze_vhdl(
    const std::filesystem::path &file,
    std::set<std::string> i_d,
    const std::string &old_hash
) {

    mm_file f(file);
    auto hash = hash_file(f.view());
    if (old_hash == hash) {
        return {file.string(), hash, std::nullopt};
    }
    vhdl_analyzer file_processor(file);
    file_processor.cleanup_content("");
    return {file.string(), hash, file_processor.analyze()};
}


/// Analyze the target Script extracting the necessary metadata
/// \param file Target file
file_analysis_context<DataFile> analyze_data(
    const std::filesystem::path &file,
    std::set<std::string> i_d,
    const std::string &old_hash
) {

    mm_file f(file);
    auto hash = hash_file(f.view());
    if (old_hash == hash) {
        return {file.string(), hash, std::nullopt};
    }

    DataFile data(file.stem(), file.string());
    std::vector res = {data};
    return {file.string(), hash, res};
}

/// Analyze the target Script extracting the necessary metadata
/// \param file Target file
file_analysis_context<Script> analyze_script(
    const std::filesystem::path &file,
    std::set<std::string> i_d,
    const std::string &old_hash
) {
    mm_file f(file);
    auto hash = hash_file(f.view());
    if (old_hash == hash) {
        return {file.string(), hash, std::nullopt};
    }

    std::string ext = file.extension();
    if (!ext.empty() && ext[0] == '.') ext = ext.substr(1);
    script_specs s;
    s.name = file.stem();
    s.type = ext;
    Script scr(s);
    scr.set_path(file);
    std::vector ret = {scr};
    return {file.string(), hash, ret};
}

/// Analyze the target constraint file extracting the necessary metadata
/// \param file Target file
file_analysis_context<Constraints> analyze_constraint(
    const std::filesystem::path &file,
    std::set<std::string> i_d,
    const std::string &old_hash
) {
    mm_file f(file);
    auto hash = hash_file(f.view());
    if (old_hash == hash) {
        return {file.string(), hash, std::nullopt};
    }
    Constraints constr(file.stem());
    constr.set_path(file);
    std::vector ret = {constr};
    return {file.string(), hash, ret};
}




