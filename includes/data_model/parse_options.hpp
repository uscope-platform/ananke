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

#ifndef ANANKE_PARSE_OPTIONS_HPP
#define ANANKE_PARSE_OPTIONS_HPP

#include <set>
#include <string>

// Parse-relevant settings bundled for dependency injection. This is the only
// options parcel threaded walker -> task -> analyzer -> preprocessor ->
// macro engine, so a future toggle is one field here and zero plumbing.
struct parse_options {
    std::set<std::string> include_directories;
    std::set<std::string> defines;
    bool strip_uvm_macros = false;
};

#endif //ANANKE_PARSE_OPTIONS_HPP
