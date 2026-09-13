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

#ifndef ANANKE_HDL_FUNCTION_CALL_HPP
#define ANANKE_HDL_FUNCTION_CALL_HPP

#include "Expression_base.hpp"
#include "data_model/HDL/statement/hdl_function_statement.hpp"
#include "data_model/HDL/statement/hdl_assignment_statement.hpp"
#include "data_model/HDL/statement/hdl_loop_statement.hpp"
#include "data_model/HDL/statement/hdl_conditional_statement.hpp"

#include <map>

class HDL_function_call : public Expression_base{
public:
    HDL_function_call() = default;
    explicit HDL_function_call(const std::string &n) {
        function_name = n;
    }
    void set_name(const std::string &n){function_name = n;}
    std::string get_name(){return function_name;}
    void add_argument(const std::shared_ptr<Expression_base> &p);
    std::vector<std::shared_ptr<Expression_base>> get_arguments() const { return arguments; }
    void add_package_prefix(const std::string &p){package_prefix = p;}
    std::string get_package_prefix() const {return package_prefix;}
    parameter_deps_t get_dependencies() const override;
    void propagate_function(const hdl_function_def_ptr &def) override;
    std::expected<resolved_parameter, solver_errors> evaluate(const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) override;

    std::optional<resolved_type> resolve_expression_type(
        const std::map<qualified_identifier, resolved_parameter> &context, const std::optional<resolved_type> &expected_type = std::nullopt) const override;
    void apply_return_order_reversal(
        std::vector<hdl_integer> &values,
        std::vector<int64_t> &value_sizes,
        const std::map<qualified_identifier, resolved_parameter> &context,
        bool packing,
        bool has_return_unpacked_ascending,
        bool return_unpacked_ascending,
        bool container_unpacked_ascending
    );

    std::string print() const override;

    [[nodiscard]] bool empty() const;

    // The linked shared definition, if propagate_function resolved one.
    // Exposed read-only so tests can prove the definition is never mutated.
    [[nodiscard]] hdl_function_def_ptr get_linked_definition() const { return linked_; }

    template<class Archive>
    void serialize(Archive & ar) {
        // NOTE: linked_ is deliberately excluded: definitions live in their
        // owning resource/file and are re-linked by propagate_functions
        // before solving (stale disk caches self-invalidate via the schema
        // hash, so no unlinked call can be loaded as linked).
        ar(function_name, arguments, package_prefix);
    }

private:
    static int64_t declared_member_width(
        const std::shared_ptr<hdl_type> &member_type,
        const std::map<qualified_identifier, resolved_parameter> &context,
        int64_t fallback
    );
    static void walk_body(
        const std::string &fcn_name,
        const std::vector<std::shared_ptr<hdl_statement_base>> &stmts,
        std::map<qualified_identifier, resolved_parameter> &ctx,
        std::map<int64_t, hdl_integer> &value_map,
        std::map<int64_t, int64_t> &size_map,
        const std::shared_ptr<hdl_type> &rt,
        const std::optional<resolved_type> &expected_type = std::nullopt
    );

    std::string function_name;
    std::string package_prefix;
    std::vector<std::shared_ptr<Expression_base>> arguments;

    // Link to the shared definition. Set once (idempotently) by
    // propagate_function; never cloned, never written through — the const
    // pointee makes definition mutation a compile error. All per-site state
    // lives in evaluation contexts, never here.
    hdl_function_def_ptr linked_;

    bool isEqual(const Expression_base& other) const override;
};

#endif //ANANKE_HDL_FUNCTION_CALL_HPP
