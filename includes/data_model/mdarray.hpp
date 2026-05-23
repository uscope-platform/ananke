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

#ifndef ANANKE_MDARRAY_HPP
#define ANANKE_MDARRAY_HPP

#include <vector>
#include <cstdint>
#include <iostream>
#include <nlohmann/json.hpp>

#include <cereal/types/vector.hpp>

template<typename T>
class mdarray {
public:

    using md_1d_array = std::vector<T>;
    using md_2d_array = std::vector<md_1d_array> ;
    using md_3d_array = std::vector<md_2d_array> ;

    mdarray() = default;
    mdarray(std::vector<uint64_t> dimensions, T value){
        if(dimensions.size()<3) throw std::invalid_argument("Dimensions vector must have at least 3 elements");

        for(auto &item:dimensions){
            if(item == 0){
                throw std::invalid_argument("Dimensions vector cannot contain 0 elements");
            }
        }
        md_1d_array l3(dimensions[2],value);
        md_2d_array l2(dimensions[1], l3);
        data = md_3d_array(dimensions[0], l2);
    };


    static std::optional<mdarray> concatenate(const mdarray &arr_1, const T &val) {
        mdarray tool;
        tool.set_scalar(val);
        return concatenate(arr_1, tool);
    }

    bool empty() const {
        return data.empty();
    }


    static std::optional<mdarray> stack(const mdarray &arr_1, const T &val) {
        mdarray res;
        if (arr_1.empty()) {
            res.set_scalar(val);
            return res;
        }
        auto arr = arr_1.get_1d_slice({0, 0});
        arr.push_back(val);
        res.set_1d_slice({0,0}, arr);
        return res;
    }


    static std::optional<mdarray> stack(const mdarray &arr_1, const mdarray &arr_2) {

        if (arr_1.data.empty()) {
            return arr_2;
        }

        if (arr_2.order > arr_1.order) return std::nullopt;

        mdarray result = arr_1;

        if (result.order == arr_2.order) {
            if (result.order == 1) {
                result.order = 2;
                result.data[0].push_back(arr_2.data[0][0]);
            } else if (result.order == 2) {
                result.order = 3;
                result.data.push_back(arr_2.data[0]);
            } else {

                return std::nullopt;
            }
        }

        else if (result.order == arr_2.order + 1) {
            if (result.order == 2) {

                result.data[0].push_back(arr_2.data[0][0]);
            } else if (result.order == 3) {

                result.data.push_back(arr_2.data[0]);
            }
        }

        else if (result.order == 3 && arr_2.order == 1) {
            if (!result.data.empty()) {
                result.data.back().push_back(arr_2.data[0][0]);
            } else {
                result.data.push_back({arr_2.data[0][0]});
            }
        } else {
            return std::nullopt;
        }

        return result;
    }

    static std::optional<mdarray> concatenate(const mdarray &arr_1, const mdarray &arr_2) {
        if (arr_1.order != arr_2.order) return std::nullopt;
        mdarray result = arr_1;
        switch (arr_1.order) {
            case 3:
                if (result.data.empty())  return arr_2;
                if (arr_2.data.empty()) return result;
                result.data.insert(result.data.end(), arr_2.data.begin(), arr_2.data.end());
                break;

            case 2:
                if (result.data.empty())  return arr_2;
                if (result.data[0].empty()) return arr_2;
                if (arr_2.data.empty()) return result;
                if (arr_2.data[0].empty()) return  result;
                result.data[0].insert(result.data[0].end(), arr_2.data[0].begin(), arr_2.data[0].end());
                break;
            case 1:
                if (result.data.empty())  return arr_2;
                if (result.data[0].empty()) return arr_2;
                if (result.data[0][0].empty()) return arr_2;
                if (arr_2.data.empty()) return result;
                if (arr_2.data[0].empty()) return  result;
                if (arr_2.data[0][0].empty()) return result;
                result.data[0][0].insert(result.data[0][0].end(), arr_2.data[0][0].begin(), arr_2.data[0][0].end());
                break;

            default:
                return std::nullopt;
        }
        return result;
    }


    void set_scalar(T val){
        if (data.size() == 0) data.resize(1);
        if (data[0].size() == 0) data[0].resize(1);
        if (data[0][0].size() == 0) data[0][0].resize(1);
        data[0][0][0] = val;
    }

    std::optional<T> get_scalar() const {
        if(data.empty() || data[0].empty() || data[0][0].empty()) return std::nullopt;
        return data[0][0][0];
    }

    void set_value(std::vector<int64_t> idx, T val){
        order = idx.size();
        if(idx[0]>=data.size()){
            data.resize(idx[0]+1);
        }
        if(idx[1]>=data[idx[0]].size()){
            data[idx[0]].resize(idx[1]+1);
        }
        if(idx[2]>=data[idx[0]][idx[1]].size()){
            data[idx[0]][idx[1]].resize(idx[2]+1);
        }
        data[idx[0]][idx[1]][idx[2]] = val;
    }

    void set_value(int64_t idx, T val){
        order = 1;
        if(data.empty()){
            data.resize(1);
        }
        if(data[0].empty()){
            data[0].resize(1);
        }
        if(idx>=data[0][0].size()){
            data[0][0].resize(idx+1);
        }
        data[0][0][idx] = val;
    }


    void set_1d_slice(std::vector<int64_t> idx, const md_1d_array &val){
        order = 1;
        if(idx[0]>=data.size()){
            data.resize(idx[0]+1);
        }
        if(idx[1]>=data[idx[0]].size()){
            data[idx[0]].resize(idx[1]+1);
        }
        data[idx[0]][idx[1]] = val;
    }
    void set_2d_slice(std::vector<int64_t> idx, const md_2d_array& val){
        order = 2;
        if(idx[0]>=data.size()){
            data.resize(idx[0]+1);
        }
        data[idx[0]] =  val;
    }



    void set_data(const md_3d_array &d){data = d;}

    md_3d_array get_data(){return data;}

    std::string to_string() const {
        std::string result = "{";
        for(auto &item_2d: data) {
            result += "{";
            for(auto &item_1d: item_2d) {
                result += "{";
                for(auto &item: item_1d) {
                    result += stringify(item);
                    if(&item != &item_1d.back()) result += ", ";
                }
                result += "}";
                if(&item_1d != &item_2d.back()) result += ", ";
            }
            result += "}";
            if(&item_2d != &data.back()) result += ", ";
        }
        result += "}";
        return result;
    }

md_2d_array get_2d_slice(std::vector<int64_t> idx) {
        if(idx.empty() || idx[0] >= data.size()) {
            std::cout << "Index out of range in get_2d_slice" << std::endl;
            exit(1);
        }
        return data[idx[0]];
    }

    md_1d_array get_1d_slice(std::vector<int64_t> idx) const {
        if(idx.size() < 2 || idx[0] >= data.size() || idx[1] >= data[idx[0]].size()) {
            std::cout << "Index out of range in get_1d_slice" << std::endl;
            exit(1);
        }
        return data[idx[0]][idx[1]];
    }

    std::optional<T> get_value(std::vector<int64_t> idx) {
        if (idx.size()<3) {
            for (int i = idx.size(); i <3 ; i++) {
                idx.insert(idx.begin(), 0);
            }
        }
        if(idx[0] >= data.size() ||
           idx[1] >= data[idx[0]].size() ||
           idx[2] >= data[idx[0]][idx[1]].size()) {
            return std::nullopt;
        }
        return data[idx[0]][idx[1]][idx[2]];
    }

    md_3d_array dump(){
        return data;
    }


    template<class Archive>
    void serialize(Archive & ar ) {
        ar(data, order);
    }

    static std::string stringify(const T& item) {
        if constexpr (std::is_same_v<T, std::string>) {
            return item;
        }else if constexpr (requires { item.to_string(); }) {
            return item.to_string(); // Handles hdl_integer once you add the member function
        } else {
            return std::to_string(item);
        }
    }

    friend bool operator==(const mdarray&lhs, const mdarray&rhs){
        return lhs.data == rhs.data;
    };

    friend void PrintTo(const mdarray& res, std::ostream* os){
        std::string result = "{";
        for(auto & item_2d:res.data){
            result += "{";
            for(auto &item_1d:item_2d){
                result += "{";
                for(auto &item:item_1d){
                    result += stringify(item);
                    if (&item != &item_1d.back()) result += ", ";
                }
                result += "}";
                if (&item_1d != &item_2d.back()) result += ", ";
            }
            result += "}";
            if (&item_2d != &res.data.back()) result += ", ";
        }

        *os << result;
    }

private:
    md_3d_array data;
    uint8_t order = 1;
};


#endif //ANANKE_MDARRAY_HPP
