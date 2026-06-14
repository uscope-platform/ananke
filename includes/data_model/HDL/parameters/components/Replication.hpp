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


#ifndef ANANKE_REPLICATION_HPP
#define ANANKE_REPLICATION_HPP

#include "Expression.hpp"
#include "Concatenation.hpp"
#include "Parameter_value_base.hpp"



class Replication : public Parameter_value_base{
public:
    Replication() = default;
    Replication(const std::shared_ptr<Expression> &size, std::shared_ptr<Parameter_value_base> item);

    Replication(const Replication &other);

    Replication(Replication &&other) noexcept;

    Replication clone() const;

    Replication &operator=(const Replication &other);

    Replication &operator=(Replication &&other) noexcept;


    void set_item(std::shared_ptr<Parameter_value_base> item){ repeated_item = std::move(item);}
    std::shared_ptr<Parameter_value_base> get_item()const { return repeated_item;}
    void set_size(const std::shared_ptr<Parameter_value_base> &expr);

    std::set<qualified_identifier> get_dependencies()const override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Parameter_value_base> &value) override;
    void propagate_function(const HDL_function_def &def) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;

    hdl_integer pack_repetition(hdl_integer value, int64_t width, int64_t count);

    std::string print() const override;
    friend bool operator==(const Replication &lhs, const Replication &rhs) {
        if(lhs.repeated_item == nullptr &&  lhs.repeated_item == nullptr ) return true;
        if(lhs.repeated_item == nullptr || lhs.repeated_item == nullptr) return false;

        return std::tie(lhs.repetition_size, *lhs.repeated_item) ==  std::tie(rhs.repetition_size, *rhs.repeated_item);
    }



    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(repetition_size, repeated_item);
    }
    bool isEqual(const Parameter_value_base &other) const override{
        // Clangd error "No viable conversion" usually means:
        // 1. The compiler thinks 'other' isn't a Parameter_value_base (check headers)
        // 2. Replication isn't fully defined yet.

        const auto& rhs = static_cast<const Replication&>(other);
        bool res = true;
        res &= *repetition_size == *rhs.repetition_size;
        res &= *repeated_item == *rhs.repeated_item;
        return res;
    }
private:
    bool packing = false;
    std::shared_ptr<Parameter_value_base>  repetition_size;
    std::shared_ptr<Parameter_value_base> repeated_item;
};


#endif //ANANKE_REPLICATION_HPP