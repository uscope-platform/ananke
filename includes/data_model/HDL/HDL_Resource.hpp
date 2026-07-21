 // Copyright 2021 University of Nottingham Ningbo China
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

#ifndef ANANKE_HDL_RESOURCE_HPP
#define ANANKE_HDL_RESOURCE_HPP

#include <utility>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>

#include "data_model/HDL/parameters/HDL_parameter.hpp"
#include "data_model/HDL/parameters/Parameters_map.hpp"
#include "data_model/documentation/module_documentation.hpp"


#include <cereal/archives/binary.hpp>
#include <cereal/types/unordered_map.hpp>
#include <cereal/types/vector.hpp>
#include <cereal/types/memory.hpp>

#include "data_model/HDL/HDL_definitions.hpp"
#include "data_model/HDL/types/HDL_struct_type.hpp"
#include "data_model/HDL/statement/hdl_function_statement.hpp"
#include "data_model/HDL/statement/hdl_statement_base.hpp"

 struct if_port_specs {
    std::string type;
    std::string modport;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(type, modport);
    }

    bool operator==(const if_port_specs&) const = default;
};

struct HDL_port {
    port_direction_t direction;
    if_port_specs if_info;

    template<class Archive>
    void serialize( Archive & ar ) {
        ar(direction, if_info);
    }

    bool operator==(const HDL_port&) const = default;
};


class HDL_Resource {
    public:
        HDL_Resource( const HDL_Resource &c );
        explicit HDL_Resource(const std::string &n){name = n;}
        HDL_Resource();

        void process_calls();


        void add_typedef(const std::string &name, const std::shared_ptr<hdl_type> &type){typedefs.insert({name, type});}
        std::map<std::string, std::shared_ptr<hdl_type>> get_typedefs(){return typedefs;}



        void add_statement(const std::shared_ptr<hdl_statement_base> &s){ statements.push_back(s);}

        const std::vector<std::shared_ptr<hdl_statement_base>>& get_statements() const { return statements; }

        void set_name(const std::string &n) {
            name  = n;
        }

        [[nodiscard]] std::unordered_map<std::string, HDL_port> get_port_specs() const {return  port_specs;}
        const std::string &getName() const {return name;}

        void set_path(const std::string &p) {
            path  = p;
        }
        std::string get_path() {return path;}
        void set_type(const dependency_class t) {
            hdl_dependency_type  = t;
        };
        void set_line_n(unsigned int n){ line_n = n;}
        [[nodiscard]] unsigned int get_line_n()const{return  line_n;}

        dependency_class get_type() {return hdl_dependency_type;};
        bool is_interface();

        void set_ports(std::unordered_map<std::string, HDL_port> p_s) {
            port_specs = std::move(p_s);
        };
        void add_ports(const std::string &p_n, HDL_port spec) {
            port_specs[p_n] = spec;
        };

        void add_processor_doc(processor_instance &p) {
            processor_docs.push_back(p);
        };
        std::vector<processor_instance> get_processor_doc() {return processor_docs;};
        bool  has_processors() {return !processor_docs.empty();};

        void add_parameter(const std::shared_ptr<HDL_parameter> &p) {
            parameters_spec.insert(p);
        }
        void set_parameters(Parameters_map p);
        Parameters_map get_parameters() const {return parameters_spec;};

        void add_function(const hdl_function_statement &f) {
            statements.push_back(std::make_shared<hdl_function_statement>(f));
        }
        std::unordered_map<std::string, hdl_function_statement> get_functions();
        hdl_function_statement get_function(const std::string &fname);

        void set_documentation(module_documentation &d) {
            doc= d;
        };
        module_documentation get_documentation() const { return doc;}

        template<class Archive>
        void serialize( Archive & ar ) {
            ar(name, path, hdl_dependency_type, parameters_spec, port_specs, doc, processor_docs,
                line_n, typedefs, statements);
        }

        bool is_empty();
        friend bool operator <(const HDL_Resource& lhs, const HDL_Resource& rhs);
        friend bool operator==(const HDL_Resource&lhs, const HDL_Resource&rhs);

        friend void PrintTo(const HDL_Resource& res, std::ostream* os);


private:
        std::string name;
        std::string path;
        unsigned int line_n = 0;
        dependency_class hdl_dependency_type = module;
        std::unordered_map<std::string, HDL_port> port_specs;

        Parameters_map parameters_spec;

        std::vector<processor_instance> processor_docs;
        std::map<std::string, std::shared_ptr<hdl_type>> typedefs;

        std::vector<std::shared_ptr<hdl_statement_base>> statements;

        // DOCUMENTATION``
        module_documentation doc;
    };




#endif //ANANKE_HDL_RESOURCE_HPP
