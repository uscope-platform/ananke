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

#include "Concatenation.hpp"
#include "Expression_base.hpp"

class Replication : public Expression_base{
public:
    Replication() = default;
    Replication(const Replication &other);

    Replication(Replication &&other) noexcept;

    Replication &operator=(const Replication &other);

    Replication &operator=(Replication &&other) noexcept;


    void set_item(std::shared_ptr<Expression_base> item){ repeated_item = std::move(item);}
    std::shared_ptr<Expression_base> get_item()const { return repeated_item;}
    void set_size(const std::shared_ptr<Expression_base> &expr);
    std::shared_ptr<Expression_base> get_size() const { return repetition_size; }

    parameter_deps_t get_dependencies()const override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Expression_base> &value) override;
    void propagate_function(const hdl_function_statement &def) override;
    std::expected<resolved_parameter, solver_errors> evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) override;

    hdl_integer pack_repetition(hdl_integer value, int64_t width, int64_t count);

    std::string print() const override;
    friend bool operator==(const Replication &lhs, const Replication &rhs) {
        if(lhs.repeated_item == nullptr &&  rhs.repeated_item == nullptr ) return true;
        if(lhs.repeated_item == nullptr ^ rhs.repeated_item == nullptr) return false;

        if(lhs.repetition_size == nullptr &&  rhs.repetition_size == nullptr ) return true;
        if(lhs.repetition_size == nullptr ^ rhs.repetition_size == nullptr) return false;

        return std::tie(*lhs.repetition_size, *lhs.repeated_item) ==  std::tie(*rhs.repetition_size, *rhs.repeated_item);
    }



    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;
    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(repetition_size, repeated_item);
    }
    bool isEqual(const Expression_base &other) const override{

        const auto& rhs = static_cast<const Replication&>(other);
        if (repetition_size == nullptr && rhs.repetition_size == nullptr && repeated_item == nullptr && rhs.repeated_item == nullptr)
            return true;
        if (repetition_size == nullptr || rhs.repetition_size == nullptr) return false;
        if (repeated_item == nullptr || rhs.repeated_item == nullptr) return false;
        bool res = true;
        res &= *repetition_size == *rhs.repetition_size;
        res &= *repeated_item == *rhs.repeated_item;
        return res;
    }
private:
    bool packing = false;
    std::shared_ptr<Expression_base>  repetition_size;
    std::shared_ptr<Expression_base> repeated_item;
};


#endif //ANANKE_REPLICATION_HPP