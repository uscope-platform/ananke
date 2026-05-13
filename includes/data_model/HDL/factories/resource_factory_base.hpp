// Copyright 2021 Filippo Savi
// Author: Filippo Savi <filssavi@gmail.com>
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef ANANKE_RESOURCES_FACTORY_BASE_HPP
#define ANANKE_RESOURCES_FACTORY_BASE_HPP

#include <string>
#include <stack>

template <class T>
class resources_factory_base {

protected:
    void new_basic_resource(const std::string &n);
    T get_resource();

    virtual ~resources_factory_base() = default;

    T current_resource;
    void push_resource();
    void pop_resource();
    std::stack<T> resource_stack;

    bool valid_resource = false;
};




#endif //ANANKE_RESOURCES_FACTORY_BASE_HPP
