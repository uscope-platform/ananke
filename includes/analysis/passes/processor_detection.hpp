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


#ifndef ANANKE_PROCESSORS_DETECTION_HPP
#define ANANKE_PROCESSORS_DETECTION_HPP

#include "analysis/passes/pass_base.hpp"
#include "data_model/data_store.hpp"

class processor_detection : public pass_base{
public:
    processor_detection(const std::shared_ptr<data_store> &d);
    void process_node(const std::shared_ptr<HDL_instance_AST> &node) override;
private:
    std::shared_ptr<data_store> d_store;
};



#endif //ANANKE_PROCESSORS_DETECTION_HPP