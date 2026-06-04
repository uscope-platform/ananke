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

#ifndef ANANKE_RANGES_FACTORY_HPP
#define ANANKE_RANGES_FACTORY_HPP

#include "data_model/HDL/parameters/common/dimension.hpp"
#include "data_model/HDL/parameters/common/HDL_type.hpp"
#include "data_model/HDL/factories/parameters/factory_base.hpp"

class ranges_factory : public factory_base{
public:
    void start();
    void open_range();
    void close_range();
    void advance_stage();
    void add_expression(const Expression &e);
    void stop();
    void advance_range();
    void clear();
    [[nodiscard]] bool active()const override {return is_active;}
    std::pair<std::vector<dimension_t>,std::vector<dimension_t>> get_dimensions();

    void consume(const std::shared_ptr<Parameter_value_base>& v) override;
    std::shared_ptr<Parameter_value_base> result() override {return nullptr;}

private:
    enum dimensions_stage{
        packed,
        unpacked
    };
    enum mode {
        first_bound,
        second_bound
    };
    bool is_active = false;
    mode status = first_bound;
    dimensions_stage stage = packed;
    dimension_t current_dim{};
    std::string name;
    std::vector<dimension_t> packed_dimensions{};
    std::vector<dimension_t> unpacked_dimensions{};
};



#endif //ANANKE_RANGES_FACTORY_HPP
