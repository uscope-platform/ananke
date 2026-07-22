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

#include "data_model/HDL/factories/resource_factory_base.hpp"
#include "data_model/HDL/HDL_Resource.hpp"
#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/statement/hdl_resource_statement.hpp"

template<class T>
void resources_factory_base<T>::new_basic_resource(const std::string &n) {
    push_resource();
    current_resource = T(n);
    valid_resource = true;
}


template<class T>
void resources_factory_base<T>::pop_resource() {
    if(!resource_stack.empty()){
        current_resource = resource_stack.top();
        resource_stack.pop();
    } else {
        valid_resource = false;
        current_resource = T();
    }
}

template<class T>
void resources_factory_base<T>::push_resource() {
    if(valid_resource){
        valid_resource = false;
        resource_stack.push(current_resource);
    }
}

template<class T>
T resources_factory_base<T>::get_resource() {
    T ret_val = current_resource;
    pop_resource();
    return ret_val;
}



template class resources_factory_base<HDL_Resource>;
template class resources_factory_base<hdl_resource_statement>;
template class resources_factory_base<HDL_parameter>;
