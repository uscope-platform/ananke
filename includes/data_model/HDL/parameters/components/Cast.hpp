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


#ifndef ANANKE_CAST_HPP
#define ANANKE_CAST_HPP


#include "Expression.hpp"
#include "analysis/type_cast_engine.hpp"
#include "Parameter_value_base.hpp"

class Cast : public Parameter_value_base{
public:
    Cast();

    Cast & operator=(const Cast &other) {
        if (this == &other)
            return *this;
        Parameter_value_base::operator =(other);
        type = other.type;
        if (other.content != nullptr) content = other.content->clone_ptr();
        type_cast = other.type_cast;
        target_type = other.target_type;
        size = other.size;
        return *this;
    }

    Cast & operator=(Cast &&other) noexcept {
        if (this == &other)
            return *this;
        Parameter_value_base::operator =(std::move(other));
        type = other.type;
        type_cast = other.type_cast;
        target_type = other.target_type;
        if (other.content != nullptr) content = std::move(other.content->clone_ptr());
        size = std::move(other.size);
        return *this;
    }

    Cast(const Cast &other);
    Cast(Cast &&other) noexcept;

    void set_type_cast(){type_cast = true;}
    void set_target_type(const std::string &tt){target_type = tt;}

    void add_size_component(const Expression_component &c) {size.components.push_back(c);}
    void add_content_component(const Expression_component &c){content = nullptr;}
    void set_size(const Expression &expr){size = expr;}
    void set_content(const std::shared_ptr<Parameter_value_base> &c){content = c;}

    [[nodiscard]] std::set<qualified_identifier> get_dependencies()const override;
    bool propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value) override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Parameter_value_base> &value) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;
    [[nodiscard]] std::string print() const override;
    int64_t get_size() override;

    [[nodiscard]] std::shared_ptr<Parameter_value_base> clone_ptr() const override;

    void set_container_sizes(const resolved_type &s) override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(content, size, type_cast, target_type);
    }

protected:
    param_value_type type = cast;
    [[nodiscard]] bool isEqual(const Parameter_value_base& other) const override;

private:
    bool type_cast = false;
    std::string target_type;
    std::shared_ptr<Parameter_value_base> content;
    Expression size;
    std::optional<resolved_type> container_size = std::nullopt;
    bool packing = false;
};


#endif //ANANKE_CAST_HPP