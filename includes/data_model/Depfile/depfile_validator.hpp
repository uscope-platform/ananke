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

#ifndef ANANKE_DEPFILE_VALIDATOR_HPP
#define ANANKE_DEPFILE_VALIDATOR_HPP

#endif //ANANKE_DEPFILE_VALIDATOR_HPP

//  Copyright 2022 Filippo Savi <filssavi@gmail.com>
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FCORE_TOOLCHAIN_SCHEMA_VALIDATOR_BASE_H
#define FCORE_TOOLCHAIN_SCHEMA_VALIDATOR_BASE_H

#include <string>
#include <nlohmann/json.hpp>

#include <valijson/adapters/nlohmann_json_adapter.hpp>
#include <valijson/utils/nlohmann_json_utils.hpp>
#include <valijson/schema.hpp>
#include <valijson/schema_parser.hpp>
#include <valijson/validator.hpp>

#include "data_model/Depfile/depfile_schema.hpp"

namespace  depfile_validator {


    inline void validate(const nlohmann::json &spec_file) {
        valijson::Schema schema;
        std::string error;
        std::string schema_name;

        auto schema_doc = nlohmann::json::parse(depfile_schema);

        valijson::SchemaParser parser;
        valijson::adapters::NlohmannJsonAdapter schema_adapter(schema_doc);
        parser.populateSchema(schema_adapter, schema);

        valijson::Validator validator;
        valijson::ValidationResults results;
        valijson::adapters::NlohmannJsonAdapter myTargetAdapter(spec_file);
        if (!validator.validate(schema, myTargetAdapter, &results)) {
            valijson::ValidationResults::Error err;
            unsigned int errorNum = 1;
            while (results.popError(err)) {

                std::string context;
                auto itr = err.context.begin();
                for (; itr != err.context.end(); ++itr) {
                    context += *itr;
                }

                std::cerr << "Error #" << errorNum << std::endl
                          << "  context: " << context << std::endl
                          << "  desc:    " << err.description << std::endl;
                ++errorNum;
            }
            throw std::invalid_argument("");
        }
    }
}



#endif //FCORE_TOOLCHAIN_SCHEMA_VALIDATOR_BASE_H
