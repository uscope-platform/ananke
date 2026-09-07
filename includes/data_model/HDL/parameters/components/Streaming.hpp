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


#ifndef ANANKE_STREAMING_HPP
#define ANANKE_STREAMING_HPP

#include "Expression_base.hpp"
#include <cereal/types/vector.hpp>

class Streaming : public Expression_base {
public:
    enum stream_direction {
        left,
        right
    };

    Streaming() = default;
    Streaming(const Streaming &other) = default;
    Streaming(Streaming &&other) noexcept = default;
    Streaming &operator=(const Streaming &other) = default;
    Streaming &operator=(Streaming &&other) noexcept = default;

    void set_direction(const stream_direction &d) { direction = d; }
    stream_direction get_direction() const { return direction; }

    void set_slice_size(const std::shared_ptr<Expression_base> &s) { slice_size = s; }
    std::shared_ptr<Expression_base> get_slice_size() const { return slice_size; }

    void add_component(const std::shared_ptr<Expression_base> &c) { components.push_back(c); }
    std::vector<std::shared_ptr<Expression_base>> get_components() const { return components; }

    parameter_deps_t get_dependencies() const override;
    void propagate_expression(const qualified_identifier &constant_id, const std::shared_ptr<Expression_base> &value) override;
    void propagate_function(const hdl_function_def_ptr &def) override;
    std::expected<resolved_parameter, solver_errors> evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) override;
    std::string print() const override;
    void set_container_sizes(const resolved_type &s, const std::map<qualified_identifier, resolved_parameter> &context = {}) override;
    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const override;

    friend bool operator==(const Streaming &lhs, const Streaming &rhs);

    template<class Archive>
    void serialize(Archive & ar) {
        ar(direction, slice_size, components);
    }

protected:
    bool isEqual(const Expression_base& other) const override;

private:
    stream_direction direction = left;
    std::shared_ptr<Expression_base> slice_size;
    std::vector<std::shared_ptr<Expression_base>> components;
};

#endif //ANANKE_STREAMING_HPP
