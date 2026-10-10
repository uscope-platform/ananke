//  Copyright 2025 Filippo Savi
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


#ifndef ANANKE_HDL_FUNCTIONS_FACTORY_HPP
#define ANANKE_HDL_FUNCTIONS_FACTORY_HPP

#include <memory>
#include <stack>
#include <string>
#include <utility>
#include <vector>

#include "data_model/HDL/statement/hdl_statements.hpp"
#include "data_model/HDL/HDL_definitions.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/factories/parameters/expressions_factory.hpp"
#include "data_model/HDL/factories/parameters/factory_base.hpp"
#include "data_model/HDL/parameters/components/Streaming.hpp"

class HDL_functions_factory {
public:
    HDL_functions_factory() = default;
    void set_name(const std::string  &s) {
        f.set_name(s);
        phase = arguments;
        active = true;
    }
    void start_assignment(const qualified_identifier &n);
    // Concatenation-lvalue frame: members are collected one by one as the
    // walker descends (add_concat_target opens index capture for each
    // member's own `[...]`), then finish_assignment emits a single statement
    // holding every member. Selects ride in the parallel index slots; a null
    // entry means "whole variable".
    void start_concat_lvalue();
    void add_concat_target(const qualified_identifier &n);
    [[nodiscard]] bool in_concat_lvalue() const { return in_concat; }
    // `return expr` assigns to the function itself with no lvalue select, so
    // unlike start_assignment it must not open lvalue_index capture: any
    // [...] in the returned expression belongs to the RHS.
    void start_return();
    std::string get_function_name() const { return f.get_name(); }
    void add_argument(const std::string &a);
    void add_local_variable(const std::shared_ptr<HDL_parameter> &p) { f.add_local_variable(p); }
    void add_component(const std::shared_ptr<Expression_base> &c);
    void add_value(const std::shared_ptr<Expression_base> &v);
    void close_lvalue();
    void start_body(){phase = body;}
    void finish_assignment();
    void add_loop(const std::shared_ptr<hdl_loop_statement> &ls) {
        f.add_statement(ls);
    }
    void add_statement(const std::shared_ptr<hdl_statement_base> &s) {
        f.add_statement(s);
    }
    std::shared_ptr<hdl_statement_base> pop_last() { return f.pop_last(); }
    hdl_function_statement get_function();
    void set_return_type_name(const std::string &n) { return_type_name = n; }
    std::string get_return_type_name() const { return return_type_name; }
    bool is_active()const{return active;}
    bool is_raw_body()const{return consumer_stack.empty();}

    void start_replication();
    void stop_replication();

    void start_concat();
    void stop_concat();
    void start_streaming();
    void stop_streaming();
    void set_stream_direction(Streaming::stream_direction d);
    void set_stream_slice_size(const std::shared_ptr<Expression_base> &s);

    void start_cast(bool expression_size);
    void stop_cast();
    void set_cast_type(const std::string &t);
    void advance_cast();

    void start_expression(bool new_expr = false);
    void stop_expression(bool new_expr = false);
    void start_function_call(const std::string &name);
    void stop_function_call();
    void set_function_package_prefix(const std::string &p);
    void set_language(hdl_language l) { language = l; }
    void add_call_argument(const std::shared_ptr<Expression_base> &ec);
    void start_ternary();
    void stop_ternary();

    void start_bit_selection();
    std::shared_ptr<Expression_base> get_last_value() const { return assignment_value; }
    int get_expression_level() const { return expr_factory_.get_level(); }
    // True while inside a [...] selection: the selection owns a pushed
    // expression context until stop_bit_selection, so level-0 closings
    // inside must not be mistaken for completed statement expressions.
    bool in_selection() const { return selection_depth > 0; }

    void stop_bit_selection();

    void set_operation(Expression_v2::expression_operator op);

private:
    template<typename T>
    T* top_as() {
        if (consumer_stack.empty()) return nullptr;
        return dynamic_cast<T*>(consumer_stack.top().get());
    }

    // Single sink for completed subexpressions (call/cast/concat/... results
    // and finished outer expressions): into a wrapping consumer, into the
    // open expression (statement RHS or pushed [...] selection alike), or
    // parked as the pending assignment value.
    void sink_value(const std::shared_ptr<Expression_base> &v);
    expressions_factory expr_factory_;
    bool active = false;
    Streaming::stream_direction pending_stream_direction = Streaming::left;
    std::shared_ptr<Expression_base> pending_stream_slice_size;
    // [...] selection state. A selection opens a pushed expression context
    // (mirroring function calls) so composite indices nest exactly like RHS
    // expressions. selection_depth guards nesting; lvalue_open distinguishes
    // an LHS selection (stashed for close_lvalue) from an RHS one (attached
    // to its identifier); pending_lhs_index carries the former.
    int selection_depth = 0;
    bool lvalue_open = false;
    std::shared_ptr<Expression_base> pending_lhs_index;
    std::stack<std::unique_ptr<factory_base>> consumer_stack;
    hdl_function_statement f;
    std::string return_type_name;
    hdl_language language = hdl_language::unknown;

    enum{
        arguments,
        body
    }phase;
    std::shared_ptr<Expression_base> assignment_value;
    qualified_identifier current_assigned_variable;
    std::shared_ptr<Expression_base> current_lhs_index;
    // Concatenation-lvalue frame, see start_concat_lvalue. Parallel with the
    // single-target fields above, which stay untouched while a frame is open.
    bool in_concat = false;
    std::vector<std::pair<qualified_identifier, std::shared_ptr<Expression_base>>> pending_concat_targets;
};



#endif //ANANKE_HDL_FUNCTIONS_FACTORY_HPP
