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

#ifndef ANANKE_PASS_MANAGER_HPP
#define ANANKE_PASS_MANAGER_HPP

#include <memory>
#include <vector>

#include "analysis/passes/pass_base.hpp"
#include "analysis/parameter_solver.hpp"
#include "analysis/passes/port_solution.hpp"
#include "analysis/passes/processor_detection.hpp"

class pass_manager {
public:
   pass_manager(const std::shared_ptr<data_store> &d);
   void apply_passes(std::shared_ptr<hdl_ast_node> &c);
private:
   void apply_pass(std::shared_ptr<hdl_ast_node> &c,const std::shared_ptr<pass_base> &pass);
   std::vector<std::shared_ptr<pass_base>> passes;
   bool data_store_valid = false;
};


#endif //ANANKE_PASS_MANAGER_HPP