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


#ifndef ANANKE_HDL_TYPE_HPP
#define ANANKE_HDL_TYPE_HPP

#include <vector>

#include "data_model/HDL/types/hdl_type.hpp"
#include "data_model/HDL/parameters/common/dimension.hpp"
#include "data_model/HDL/types/resolved_type.hpp"


class HDL_simple_type : public hdl_type {
public:
    HDL_simple_type() = default;
    HDL_simple_type(const HDL_simple_type &other) = default;

    HDL_simple_type(HDL_simple_type &&other) noexcept
        : is_signed(other.is_signed),
          is_real(other.is_real),
          is_implicit(other.is_implicit),
          unpacked_dimensions(std::move(other.unpacked_dimensions)),
          packed_dimensions(std::move(other.packed_dimensions)) {
    }

    HDL_simple_type & operator=(const HDL_simple_type &other) {
        if (this == &other)
            return *this;
        is_signed = other.is_signed;
        is_real = other.is_real;
        is_implicit = other.is_implicit;
        unpacked_dimensions = other.unpacked_dimensions;
        packed_dimensions = other.packed_dimensions;
        return *this;
    }

    HDL_simple_type & operator=(HDL_simple_type &&other) noexcept {
        if (this == &other)
            return *this;
        is_signed = other.is_signed;
        is_real = other.is_real;
        is_implicit = other.is_implicit;
        unpacked_dimensions = std::move(other.unpacked_dimensions);
        packed_dimensions = std::move(other.packed_dimensions);
        return *this;
    }

    friend bool operator==(const HDL_simple_type &lhs, const HDL_simple_type &rhs) {
        bool ret = true;

        ret &= lhs.is_signed == rhs.is_signed;
        ret &= lhs.is_implicit == rhs.is_implicit;
        ret &= lhs.is_real == rhs.is_real;
        if(lhs.unpacked_dimensions.size() != rhs.unpacked_dimensions.size()) return false;
        for(int i = 0; i<lhs.unpacked_dimensions.size(); i++){
            ret &= lhs.unpacked_dimensions[i] == rhs.unpacked_dimensions[i];
        }

        if(lhs.packed_dimensions.size() != rhs.packed_dimensions.size()) return false;
        for(int i = 0; i<lhs.packed_dimensions.size(); i++){
            ret &= lhs.packed_dimensions[i] == rhs.packed_dimensions[i];
        }
        return ret;
    }


    void add_dimension(const dimension_t &d);

    [[nodiscard]] bool is_scalar()const override {return unpacked_dimensions.empty();}

    void set_packed_dimensions(const std::vector<dimension_t>  &d);
    void set_unpacked_dimensions(const std::vector<dimension_t>  &d);
    [[nodiscard]] std::vector<dimension_t> get_packed_dimensions() const {return  packed_dimensions;};
    [[nodiscard]] std::vector<dimension_t> get_unpacked_dimensions() const {return  unpacked_dimensions;};

    void set_signed(bool s) {is_signed = s;}
    [[nodiscard]] bool get_signed() const {return is_signed;}
    void set_real(bool r) {is_real = r;}
    [[nodiscard]] bool get_real() const {return is_real;}
    void set_implicit(bool i) {is_implicit = i;}
    [[nodiscard]] bool get_implicit() const {return is_implicit;}
    std::set<qualified_identifier> get_dependencies() override;


    std::optional<resolved_type> evaluate_type(const std::map<qualified_identifier, resolved_parameter> &context) override;

    [[nodiscard]] std::string to_print() const override;

    [[nodiscard]] bool is_equal(const hdl_type &other) const override {
        if (auto* o = dynamic_cast<const HDL_simple_type*>(&other)) {
            return *this == *o;
        }
        return false;
    }

    friend void PrintTo(const HDL_simple_type& t, std::ostream* os) {
        if (!t.packed_dimensions.empty()) {
            *os << ", packed: [";
            for (size_t i = 0; i < t.packed_dimensions.size(); ++i) {
                if (i > 0) *os << ", ";
                *os << t.packed_dimensions[i];
            }
            *os << "]";
        }
        if (!t.unpacked_dimensions.empty()) {
            *os << ", unpacked: [";
            for (size_t i = 0; i < t.unpacked_dimensions.size(); ++i) {
                if (i > 0) *os << ", ";
                *os << t.unpacked_dimensions[i];
            }
            *os << "]";
        }
        *os << " }";
    }

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(unpacked_dimensions, packed_dimensions);
    }
private:
    bool is_signed = false;
    bool is_real = false;
    bool is_implicit = false;
    std::vector<dimension_t> unpacked_dimensions;
    std::vector<dimension_t> packed_dimensions;
};



#endif //ANANKE_HDL_TYPE_HPP
