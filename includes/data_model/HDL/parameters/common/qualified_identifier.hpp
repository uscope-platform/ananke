//  Copyright 2026 Filippo Savi
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


#ifndef ANANKE_QUALIFIED_IDENTIFIER_HPP
#define ANANKE_QUALIFIED_IDENTIFIER_HPP

#include <string>
#include <set>
#include "data_model/mdarray.hpp"


class qualified_identifier {
public:
    qualified_identifier() = default;

    template <typename T>
    qualified_identifier(std::initializer_list<T>) = delete;

    explicit qualified_identifier(const std::string n) {
        _name = n;
        _instance = {};
        _package_prefix = {};
        _package_prefix = {};
    }

    qualified_identifier(const std::string &p, const std::string &n) {
        if (!p.empty()) _package_prefix = {p};
        _instance = {};
        _name = n;
    }

    explicit qualified_identifier(const std::string &p,const std::string &i, const std::string &n) {
        if (!p.empty()) _package_prefix = {p};
        _instance = i;
        _name = n;
    }
    void set_package_prefix(const std::vector<std::string> &p){_package_prefix = p;}
    void set_instance_prefix(const std::vector<std::string> &i){_instance = i.back();}
    std::string get_name() const {return _name;}
    std::string get_instance() const {return _instance;}
    std::vector<std::string> get_package_prefix() const {return _package_prefix;}

    friend bool operator==(const qualified_identifier &lhs, const qualified_identifier &rhs) {
        return std::tie(lhs._package_prefix, lhs._name, lhs._instance) == std::tie(rhs._package_prefix, rhs._name, rhs._instance);
    }


    bool operator<(const qualified_identifier& other) const {
        return std::tie(_package_prefix,_instance, _name) < std::tie(other._package_prefix,other._instance, other._name);
    }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(_name, _instance, _package_prefix);
    }

private:
    std::vector<std::string> _package_prefix;
    std::string _instance;
    std::string _name;

};

struct parameter_deps_t {
    std::set<qualified_identifier> data;
    std::set<qualified_identifier> functions;
    [[nodiscard]] bool empty() const {
        return data.empty() && functions.empty();
    }
    void merge (const parameter_deps_t &p) {
        data.insert(p.data.begin(), p.data.end());
        functions.insert(p.functions.begin(), p.functions.end());
    }

    friend bool operator==(const parameter_deps_t &lhs, const parameter_deps_t &rhs) {
        return lhs.data == rhs.data
               && lhs.functions == rhs.functions;
    }

    friend bool operator!=(const parameter_deps_t &lhs, const parameter_deps_t &rhs) {
        return !(lhs == rhs);
    }
};

#endif //ANANKE_QUALIFIED_IDENTIFIER_HPP