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

#ifndef ANANKE_RESOLVED_PARAMETER_HPP
#define ANANKE_RESOLVED_PARAMETER_HPP

#include <variant>
#include <iomanip>
#include <cstdint>
#include <string>
#include "data_model/mdarray.hpp"
#include "data_model/HDL/parameters/common/hdl_integer.hpp"


#include <cereal/types/variant.hpp>

class resolved_parameter {
public:
    resolved_parameter() = default;
    resolved_parameter(int init_val) {
        content = static_cast<hdl_integer>(init_val);
        undefined = false;
    }
    resolved_parameter(hdl_integer init_val) {
        content = init_val;
        undefined = false;
    }
    resolved_parameter(const std::string &init_val) {
        content = init_val;
        undefined = false;
    }
    resolved_parameter(double init_val) {
        content = init_val;
        undefined = false;
    }
    resolved_parameter(const mdarray<hdl_integer> &init_val) {
        content = init_val;
        undefined = false;
    }
    resolved_parameter(const mdarray<std::string> &init_val) {
        content = init_val;
        undefined = false;
    }
    [[nodiscard]] hdl_integer get_integer() const;
    [[nodiscard]] std::string get_string() const;
    [[nodiscard]] mdarray<hdl_integer> get_int_array() const;
    [[nodiscard]] mdarray<std::string> get_string_array() const;
    [[nodiscard]] double get_real() const;
    [[nodiscard]] bool is_integer() const {return std::holds_alternative<hdl_integer>(content);}
    [[nodiscard]] bool is_string() const {return std::holds_alternative<std::string>(content);}
    [[nodiscard]] bool is_int_array() const {return std::holds_alternative<mdarray<hdl_integer>>(content);}
    [[nodiscard]] bool is_string_array() const {return std::holds_alternative<mdarray<std::string>>(content);}
    [[nodiscard]] bool is_real() const {return std::holds_alternative<double>(content);}

    void set_undefined() {
        content = 0;
        undefined = true;
    }

    bool operator==(const resolved_parameter&) const = default;

    friend std::ostream& operator<<(std::ostream& os, const resolved_parameter& rp) {
        std::visit([&os](const auto& v) {
            using T = std::decay_t<decltype(v)>;
            if constexpr (std::is_same_v<T, int64_t>) {
                os << v;
            } else if constexpr (std::is_same_v<T, double>) {
                os << std::setprecision(17) << v;
            } else if constexpr (std::is_same_v<T, std::string>) {
                os << '"' << v << '"';
            } else {
                os << v.to_string();
            }
        }, rp.content);
        return os;
    }

    friend void PrintTo(const resolved_parameter& rp, std::ostream* os) {
        *os << rp;
    }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(content);
    }

private:

    std::variant<hdl_integer, std::string, mdarray<hdl_integer>,  double, mdarray<std::string>> content;
    bool undefined = true;

};



#endif //ANANKE_RESOLVED_PARAMETER_HPP
