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
#include "data_model/mdarray.hpp"



struct qualified_identifier {
    qualified_identifier() = default;
    qualified_identifier(const std::string &p,const std::string &i, const std::string &n) {
        prefix = p;
        instance = i;
        name = n;
    }

    std::string prefix;
    std::string instance;
    std::string name;

    friend bool operator==(const qualified_identifier &lhs, const qualified_identifier &rhs) {
        return std::tie(lhs.prefix, lhs.name, lhs.instance) == std::tie(rhs.prefix, rhs.name, rhs.instance);
    }


    bool operator<(const qualified_identifier& other) const {
        return std::tie(prefix,instance, name) < std::tie(other.prefix,other.instance, other.name);
    }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(name, instance, prefix);
    }
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