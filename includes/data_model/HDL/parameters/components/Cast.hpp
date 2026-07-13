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

#include "analysis/type_cast_engine.hpp"
#include <spdlog/spdlog.h>
#include "Expression_base.hpp"

class Cast : public Expression_base{
public:
    Cast() = default;

    Cast & operator=(const Cast &other) {
        if (this == &other)
            return *this;
        Expression_base::operator =(other);
        content = other.content;
        type_cast = other.type_cast;
        container_size = other.container_size;
        target_type = other.target_type;
        size = other.size;
        return *this;
    }

    Cast & operator=(Cast &&other) noexcept {
        if (this == &other)
            return *this;
        Expression_base::operator =(std::move(other));
        type_cast = other.type_cast;
        target_type = other.target_type;
        container_size = other.container_size;
        if (other.content != nullptr) content = std::move(other.content);
        size = std::move(other.size);
        return *this;
    }

    Cast(const Cast &other);
    Cast(Cast &&other) noexcept;

    void set_type_cast(){type_cast = true;}
    void set_target_type(const std::string &tt){target_type = tt;}

    void set_size(const std::shared_ptr<Expression_base> &s){size = s;}
    void set_content(const std::shared_ptr<Expression_base> &c){content = c;}

    [[nodiscard]] parameter_deps_t get_dependencies()const override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Expression_base> &value) override;
    std::optional<resolved_parameter> evaluate(const std::map<qualified_identifier, resolved_parameter> &context) override;
    [[nodiscard]] std::string print() const override;
    int64_t get_size() override;

    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(content, size, type_cast, target_type);
    }

protected:
    [[nodiscard]] bool isEqual(const Expression_base& other) const override;

private:
    bool type_cast = false;
    std::string target_type;
    std::shared_ptr<Expression_base> content;
    std::shared_ptr<Expression_base> size;
    std::optional<resolved_type> container_size = std::nullopt;
};


#endif //ANANKE_CAST_HPP