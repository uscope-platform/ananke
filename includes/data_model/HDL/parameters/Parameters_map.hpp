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

#include <vector>
#include "data_model/HDL/parameters/HDL_parameter.hpp"

class Parameters_map{
public:

    size_t size(){return inner_map.size();}
    void clear() {inner_map.clear();}
    bool empty() {return inner_map.empty();}
    bool contains(const std::string &name) const {
        bool retval = false;
        for(const auto &item:inner_map){
            retval |= item->get_name() == name;
        }
        return retval;
    }

    void erase(const std::string &name){
        int i =-1 ;
        for(i = 0; i<inner_map.size(); i++){
            if(inner_map[i]->get_name()==name) break;
        }
        if(i !=-1){
            inner_map.erase(inner_map.begin()+i);
        }
    }

    std::shared_ptr<HDL_parameter> const_get(const std::string &name) const{
        for(int i = 0; i<inner_map.size(); i++){
            if(inner_map[i]->get_name()==name){
                return inner_map[i];
            };
        }
        return nullptr;
    }

    std::shared_ptr<HDL_parameter> get(const std::string &name){
        bool found = false;
        int item_idx = 0;
        for(int i = 0; i<inner_map.size(); i++){
            if(inner_map[i]->get_name()==name){
                found = true;
                item_idx = i;
            };
        }
        if(!found){
            inner_map.emplace_back();
            inner_map.back()->set_name(name);
            return inner_map.back();
        } else{
            return inner_map[item_idx];
        }
    }

    friend bool operator==(const Parameters_map&lhs, const Parameters_map&rhs){
        if(lhs.inner_map.size() != rhs.inner_map.size()) return false;
        bool ret_val = true;
        for(int i = 0; i<lhs.inner_map.size(); i++){
            ret_val &= *lhs.inner_map[i] == *rhs.inner_map[i];
        }
        return ret_val;
    };

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(inner_map);
    }
    void insert(const std::shared_ptr<HDL_parameter> &p){
        bool  found = false;
        for(auto & i : inner_map){
            if(i->get_name()==p->get_name()) {
                i = p;
                found = true;
            }
        }
        if(!found) inner_map.push_back(p);
    }

    void insert(std::vector<std::shared_ptr<HDL_parameter>>::iterator  begin, std::vector<std::shared_ptr<HDL_parameter>>::iterator end){
        std::for_each(begin, end, [&](std::shared_ptr<HDL_parameter> const & p){
            this->insert(p);
        });
    };

    std::vector<std::shared_ptr<HDL_parameter>>::iterator begin()
    {
        return inner_map.begin();
    };

    std::vector<std::shared_ptr<HDL_parameter>>::const_iterator begin() const{
        return inner_map.begin();
    };

    std::vector<std::shared_ptr<HDL_parameter>>::iterator end()
    {
        return inner_map.end();
    };

    std::vector<std::shared_ptr<HDL_parameter>>::const_iterator end() const{
        return inner_map.end();
    };



private:

    std::vector<std::shared_ptr<HDL_parameter>> inner_map;
};



#endif //ANANKE_PARAMETERS_MAP_HPP
