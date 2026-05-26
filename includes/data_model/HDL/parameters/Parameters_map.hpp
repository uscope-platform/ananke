//  Copyright 2023 Filippo Savi
//  Author: Filippo Savi <filssavi@gmail.com>
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#ifndef ANANKE_PARAMETERS_MAP_HPP
#define ANANKE_PARAMETERS_MAP_HPP

#include <unordered_map>
#include "data_model/HDL/parameters/HDL_parameter.hpp"

class Parameters_map{
public:

    size_t size() const {return inner_map.size();}
    void clear() {inner_map.clear();}
    bool empty() const {return inner_map.empty();}
    bool contains(const std::string &name) const {
        return inner_map.contains(name);
    }

    void erase(const std::string &name){
        inner_map.erase(name);
    }

    std::shared_ptr<HDL_parameter> const_get(const std::string &name) const{
        auto it = inner_map.find(name);
        if(it != inner_map.end()) return it->second;
        return nullptr;
    }

    std::shared_ptr<HDL_parameter> get(const std::string &name){
        auto it = inner_map.find(name);
        if(it != inner_map.end()) return it->second;
        auto [new_it, _] = inner_map.emplace(name, std::make_shared<HDL_parameter>());
        new_it->second->set_name(name);
        return new_it->second;
    }

    friend bool operator==(const Parameters_map&lhs, const Parameters_map&rhs){
        if(lhs.inner_map.size() != rhs.inner_map.size()) return false;
        for(const auto &[key, val] : lhs.inner_map) {
            auto it = rhs.inner_map.find(key);
            if(it == rhs.inner_map.end()) return false;
            if(*val != *it->second) return false;
        }
        return true;
    };

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(inner_map);
    }

    void insert(const std::shared_ptr<HDL_parameter> &p){
        inner_map[p->get_name()] = p;
    }

    template<typename Iter>
    void insert(Iter begin, Iter end){
        for(; begin != end; ++begin) {
            insert(begin->second);
        }
    };

    auto begin() const { return inner_map.begin(); };
    auto begin() { return inner_map.begin(); };
    auto end() const { return inner_map.end(); };
    auto end() { return inner_map.end(); };

private:

    std::unordered_map<std::string, std::shared_ptr<HDL_parameter>> inner_map;
};

#endif //ANANKE_PARAMETERS_MAP_HPP
