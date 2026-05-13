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

#ifndef ANANKE_INITIALIZATION_LIST_HPP
#define ANANKE_INITIALIZATION_LIST_HPP


#include <vector>
#include <utility>
#include <cereal/types/vector.hpp>

#include "data_model/HDL/parameters/Expression.hpp"
#include "data_model/HDL/parameters/Replication.hpp"
#include "data_model/HDL/parameters/Concatenation.hpp"
#include "data_model/mdarray.hpp"
#include  "data_model/HDL/parameters/dimension.hpp"


// FORWARD DECLARATIONS
class HDL_parameter;
class HDL_function_def;
class data_store;
class Parameters_map;


// This class manages the recursive construction and evaluation of multi-dimensional HDL initializers.
// It processes nested levels, handles expression repetitions, and resolves complex parameter
// dependencies into a concrete numerical multi-dimensional structure (mdarray).

// For non-scalar data, dimensions must be defined via add_dimension before the data is populated
// to ensure the internal recursive structure is correctly initialized.

class Initialization_list {

public:
    Initialization_list() = default;
    Initialization_list( const Initialization_list &i);

    Initialization_list clone() const;
    explicit Initialization_list(const std::shared_ptr<Parameter_value_base> &e);
    void add_dimension(const dimension_t &d);
    void set_dimensions(const std::vector<dimension_t> &d, bool packed);
    void add_item(const std::shared_ptr<Parameter_value_base> &e);
    bool empty() const;
    bool is_array() const{return !scalar;}
    bool is_packed() const{return unpacked_dimensions.empty() && !packed_dimensions.empty();}

    std::optional<resolved_parameter> get_values();

    void set_packed_dimensions(const std::vector<dimension_t>  &d) {packed_dimensions = d;};
    void set_unpacked_dimensions(const std::vector<dimension_t>  &d) {unpacked_dimensions = d;};
    std::vector<dimension_t> get_packed_dimensions(){return  packed_dimensions;};
    std::vector<dimension_t> get_unpacked_dimensions(){return  unpacked_dimensions;};

    std::optional<resolved_parameter> evaluate();

    bool propagate_constant(const qualified_identifier &constant_id, const resolved_parameter &value);
    void propagate_function(const HDL_function_def &def);

    std::optional<std::shared_ptr<Parameter_value_base>> get_scalar();
    void clear_scalar();
    void push_scalar_component(const Expression_component &comp);
    void set_scalar(const std::shared_ptr<Parameter_value_base> &expr);

    resolved_parameter process_default_initialization();


    enum parameter_type {
        string_parameter,
        numeric_parameter,
        array_parameter
    };

    parameter_type get_type() const;

    std::set<qualified_identifier> get_dependencies();
    void set_default() { default_initialization = true;};

    friend bool operator==(const Initialization_list&lhs, const Initialization_list&rhs);
    friend void PrintTo(const Initialization_list& res, std::ostream* os);

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(unpacked_dimensions, packed_dimensions, expression_leaves, default_initialization, scalar);
    }

    void set_solved_value(const resolved_parameter &v);
    [[nodiscard]] std::variant<mdarray<int64_t>, mdarray<std::string>::md_1d_array> get_solved_value() const;

private:


    bool scalar = true;


    std::vector<dimension_t> unpacked_dimensions;
    std::vector<dimension_t> packed_dimensions;

    std::vector<std::shared_ptr<Parameter_value_base>> expression_leaves;
    resolved_parameter solved_value;

    bool default_initialization = false;
};


#endif //ANANKE_INITIALIZATION_LIST_HPP
