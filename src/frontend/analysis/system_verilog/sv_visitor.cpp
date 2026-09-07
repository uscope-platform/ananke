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

#include <algorithm>
#include <set>

#include "frontend/analysis/system_verilog/sv_visitor.hpp"

#include <sys/stat.h>

#include "frontend/analysis/system_verilog/sv_parsing_helpers.hpp"
#include "data_model/HDL/statement/hdl_parameter_override_statement.hpp"
#include "data_model/HDL/statement/hdl_assignment_statement.hpp"
#include "data_model/HDL/parameters/components/token/Numeric_token.hpp"
#include "data_model/HDL/parameters/components/token/Real_token.hpp"
#include "data_model/HDL/parameters/components/token/Time_token.hpp"
#include "data_model/HDL/parameters/components/token/Identifier_token.hpp"
#include "data_model/HDL/parameters/components/token/Type_ref.hpp"
#include "data_model/HDL/parameters/components/Streaming.hpp"
#include "data_model/HDL/parameters/components/token/String_token.hpp"
#include "data_model/HDL/parameters/components/HDL_builtin_function.hpp"
#include "data_model/HDL/types/HDL_external_type.hpp"


bool sv_visitor::is_known_system_function(const std::string &name) const {
    if (HDL_builtin_function::is_known(name)) return true;

    static const std::set<std::string> known = {
        // Random / distribution functions (recognized, not constant-evaluable)
        "random", "urandom", "urandom_range", "srandom",
        "dist_uniform", "dist_normal", "dist_exponential", "dist_poisson",
        "dist_chi_square", "dist_t", "dist_erlang",
        // Common procedural system tasks that may appear in parse contexts
        "display", "displayb", "displayh", "displayo",
        "write", "writeb", "writeh", "writeo",
        "monitor", "monitorb", "monitorh", "monitoro", "monitoroff", "monitoron",
        "finish", "stop", "fatal", "error", "warning", "info",
        "time", "realtime", "stime", "printtimescale",
        "readmemh", "readmemb",
        "fopen", "fclose", "fdisplay", "fdisplayb", "fdisplayh", "fdisplayo",
        "fwrite", "fwriteb", "fwriteh", "fwriteo", "fmonitor",
        "fscanf", "fread", "fseek", "ftell", "fflush", "feof", "ferror", "rewind",
        "fgetc", "fgets", "ungetc", "fputc", "fputs", "sscanf",
        "system", "value$plusargs", "test$plusargs",
        "assert", "asserton", "assertoff", "assertkill", "assertpasson",
        "cast", "root", "unit", "fatal", "error", "warning", "info",
        "urandom", "urandom_range", "random", "srandom"
    };
    return known.contains(name);
}

void sv_visitor::route_expression_text(const std::string& text) {
    if(loops_factory.in_loop()) {
        loops_factory.add_component(sv_parsing_helpers::make_value(text));
    }
    if(type_engine.active() || type_engine.is_ranging()){
        type_engine.add_component(sv_parsing_helpers::make_value(text));
    } else if(!in_streaming_slice && !in_type_argument && params_factory.is_component_relevant()){
        params_factory.add_component(sv_parsing_helpers::make_value(text));
    }
    if (f_factory.is_active() && !in_streaming_slice) {
        f_factory.add_component(sv_parsing_helpers::make_value(text));
    }
    if(deps_factory.is_valid_dependency()){
        deps_factory.add_connection_element(text);
    }
}

void sv_visitor::route_expression_component(const std::shared_ptr<Expression_base> &ec) {
    auto clone = [](const std::shared_ptr<Expression_base> &src) -> std::shared_ptr<Expression_base> {
        if (auto id = std::dynamic_pointer_cast<const Identifier_token>(src))
            return std::make_shared<Identifier_token>(*id);
        if (auto num = std::dynamic_pointer_cast<const Numeric_token>(src))
            return std::make_shared<Numeric_token>(*num);
        if (auto real = std::dynamic_pointer_cast<const Real_token>(src))
            return std::make_shared<Real_token>(*real);
        if (auto time = std::dynamic_pointer_cast<const Time_token>(src))
            return std::make_shared<Time_token>(*time);
        if (auto str = std::dynamic_pointer_cast<const String_token>(src))
            return std::make_shared<String_token>(*str);
        return src;
    };

    bool routed = false;
    if(loops_factory.in_loop()) {
        loops_factory.add_component(routed ? clone(ec) : ec);
        routed = true;
    }
    if (f_factory.is_active() && !in_streaming_slice) {
        f_factory.add_component(routed ? clone(ec) : ec);
        routed = true;
    }
    if(type_engine.active() || type_engine.is_ranging()){
        type_engine.add_component(routed ? clone(ec) : ec);
        routed = true;
    } else if(!in_streaming_slice && !in_type_argument && params_factory.is_component_relevant()){
        params_factory.add_component(routed ? clone(ec) : ec);
        routed = true;
    }
}


void sv_visitor::enterModule_declaration(sv2017::Module_declarationContext *ctx) {
    current_declaration_type = "module";
    size_t line_number = ctx->getStart()->getLine();
    auto module_name = ctx->module_header_common()->identifier()->getText();
    modules_factory.new_module(module_name, module, line_number);
}


void sv_visitor::exitModule_declaration(sv2017::Module_declarationContext *ctx) {
    type_engine.clear_type_params();
    entities.push_back(modules_factory.get_module());
}

void sv_visitor::enterInterface_declaration(sv2017::Interface_declarationContext *ctx) {
    current_declaration_type = "interface";
    size_t line_number = ctx->getStart()->getLine();
    std::string interface_name = ctx->interface_header()->identifier()->getText();
    interfaces_factory.new_interface(interface_name, line_number);
}

void sv_visitor::exitInterface_declaration(sv2017::Interface_declarationContext *ctx) {
    type_engine.clear_type_params();
    entities.push_back(interfaces_factory.get_interface());
}


void sv_visitor::enterModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *ctx) {
    auto id = ctx->identifier();
    auto hier = ctx->hierarchical_instance(0);
    auto inst_name = hier ? hier->name_of_instance() : nullptr;
    if (!id || !hier || !inst_name || !inst_name->identifier()) {
        spdlog::warn("Malformed module instantiation, skipping file");
        had_error = true;
        return;
    }
    std::string module_name = id->getText();
    std::string instance_name = inst_name->identifier()->getText();

    deps_factory.new_dependency(instance_name, module_name, module);
}


void sv_visitor::exitModule_or_interface_or_program_or_udp_instantiation(sv2017::Module_or_interface_or_program_or_udp_instantiationContext *ctx) {
    if(conditionals_factory.is_active()){
        conditionals_factory.add_statement(deps_factory.get_dependency());
    } else if(loops_factory.in_loop()){
        loops_factory.add_statement(deps_factory.get_dependency());
    } else {
        modules_factory.add_statement(deps_factory.get_dependency());
    }

}

void sv_visitor::enterName_of_instance(sv2017::Name_of_instanceContext *ctx) {
    if(!ctx->unpacked_dimension().empty()){
        params_factory.start_param_assignment();
        params_factory.set_type(std::make_shared<HDL_simple_type>());
        params_factory.new_parameter("instance_array_qualifier");
    }
}

void sv_visitor::exitName_of_instance(sv2017::Name_of_instanceContext *ctx) {
    if(!ctx->unpacked_dimension().empty()){
        deps_factory.add_array_quantifier(params_factory.get_parameter());
    }
}

void sv_visitor::enterTf_port_item(sv2017::Tf_port_itemContext *ctx) {
    if (ctx->identifier()) {
        f_factory.add_argument(ctx->identifier()->getText());
    } else if (ctx->data_type_or_implicit()) {
        auto name = ctx->data_type_or_implicit()->getText();
        f_factory.add_argument(name);
    }
}

void sv_visitor::exitTf_port_list(sv2017::Tf_port_listContext *ctx) {
    f_factory.start_body();
}

std::shared_ptr<hdl_type> sv_visitor::setup_data_type(sv2017::Data_type_or_implicitContext *dtoi) {
    type_engine.set_base_type(std::make_shared<HDL_simple_type>());
    if (dtoi && dtoi->data_type()) {
        if (dtoi->data_type()->package_or_class_scoped_path() &&
            !dtoi->data_type()->package_or_class_scoped_path()->DOUBLE_COLON().empty()) {
            auto pkg_ctx = dtoi->data_type()->package_or_class_scoped_path();
            auto qi = sv_parsing_helpers::parse_qualified_identifier(pkg_ctx);
            auto ext_type = std::make_shared<HDL_external_type>(qi);
            type_engine.set_base_type(ext_type);
            return ext_type;
        }
        std::string type;
        if (dtoi->data_type()->data_type_primitive())
            type = dtoi->data_type()->data_type_primitive()->getText();
        if (dtoi->data_type()->package_or_class_scoped_path())
            type = dtoi->data_type()->package_or_class_scoped_path()->getText();
        auto resolved = type_engine.resolve_type(type);
        type_engine.set_base_type(resolved);
        return resolved;
    }
    if (!dtoi) {
        auto implicit = Type_engine::create_primitive_type("implicit");
        type_engine.set_base_type(implicit);
        return implicit;
    }
    return std::make_shared<HDL_simple_type>();
}

std::shared_ptr<hdl_type> sv_visitor::resolve_data_type(sv2017::Data_typeContext *dt) {
    if (!dt) return Type_engine::create_primitive_type("implicit");
    if (dt->data_type_primitive()) {
        std::string type = dt->data_type_primitive()->getText();
        return type_engine.resolve_type(type);
    }
    if (dt->package_or_class_scoped_path()) {
        auto pkg_ctx = dt->package_or_class_scoped_path();
        if (!pkg_ctx->DOUBLE_COLON().empty()) {
            auto qi = sv_parsing_helpers::parse_qualified_identifier(pkg_ctx);
            return std::make_shared<HDL_external_type>(qi);
        }
        std::string name = pkg_ctx->getText();
        if (name == "bit" || name == "logic" || name == "reg") {
            return Type_engine::create_primitive_type(name);
        }
        if (type_engine.has_type(name)) {
            return type_engine.get_type(name);
        }
        return nullptr;
    }
    return std::make_shared<HDL_simple_type>();
}

std::shared_ptr<Expression_base> sv_visitor::build_data_type_expression(sv2017::Data_typeContext *dt) {
    auto base = resolve_data_type(dt);
    if (!base) {
        if (dt->package_or_class_scoped_path()) {
            auto pkg_ctx = dt->package_or_class_scoped_path();
            if (!pkg_ctx->DOUBLE_COLON().empty()) {
                auto qi = sv_parsing_helpers::parse_qualified_identifier(pkg_ctx);
                return std::make_shared<Identifier_token>(qi);
            }
            return std::make_shared<Identifier_token>(qualified_identifier(pkg_ctx->getText()));
        }
        return std::make_shared<Identifier_token>(qualified_identifier(dt->getText()));
    }

    // Preserve the original identifier when the data type is a named type reference
    // (e.g. a typedef or a type parameter), so dependency tracking and override
    // re-resolution keep working on the identifier itself.
    std::string name = "<data_type>";
    if (dt->package_or_class_scoped_path() && !dt->package_or_class_scoped_path()->DOUBLE_COLON().empty()) {
        auto qi = sv_parsing_helpers::parse_qualified_identifier(dt->package_or_class_scoped_path());
        name = qi.print();
    } else if (dt->package_or_class_scoped_path()) {
        name = dt->package_or_class_scoped_path()->getText();
    }
    auto id_token = std::make_shared<Identifier_token>(qualified_identifier(name));
    // Resolved-type placeholder (e.g. $bits argument): carries the type for
    // evaluation but is not a readable value, so no data dependency.
    id_token->set_type_placeholder(true);

    if (base->is<HDL_simple_type>()) {
        auto simple = base->as<HDL_simple_type>();
        std::vector<dimension_t> packed_dims;
        for (auto *vd : dt->variable_dimension()) {
            if (!vd || !vd->array_range_expression()) continue;
            auto re = vd->array_range_expression();
            dimension_t d;
            d.packed = true;
            auto exprs = re->expression();
            d.first_bound = sv_parsing_helpers::make_value(exprs[0]->getText());
            d.second_bound = exprs.size() > 1 ? sv_parsing_helpers::make_value(exprs[1]->getText()) : d.first_bound;
            packed_dims.push_back(d);
        }
        if (!packed_dims.empty()) simple.set_packed_dimensions(packed_dims);
        base = std::make_shared<HDL_simple_type>(simple);
    }
    id_token->set_expression_type(base);
    return id_token;
}

void sv_visitor::enterData_declaration(sv2017::Data_declarationContext *ctx) {
    if (f_factory.is_active() && !ctx->type_declaration()) {
        auto dtoi = ctx->data_type_or_implicit();
        auto dt = dtoi && dtoi->data_type() ? dtoi->data_type() : nullptr;
        if (dt && dt->struct_union() &&
            (dt->struct_union()->KW_STRUCT() || dt->struct_union()->KW_UNION())) {
            // Anonymous struct/union local: same composite flow as the module
            // path, finalized into a local instead of a module parameter.
            in_function_composite_decl = true;
            pending_function_local_init = f_factory.get_last_value();
            if (dt->struct_union()->KW_STRUCT())
                type_engine.start_composite_type_declaration(Type_engine::struct_type);
            else
                type_engine.start_composite_type_declaration(Type_engine::union_type);
            top_level_struct_started = true;
            return;
        }
        if (dt && dt->KW_ENUM()) {
            // Anonymous enum local: same composite flow, finalized into a
            // local instead of a module parameter.
            in_function_composite_decl = true;
            pending_function_local_init = f_factory.get_last_value();
            type_engine.start_composite_type_declaration(Type_engine::enum_type);
            top_level_struct_started = true;
            return;
        }
        if (!(dt && dt->struct_union())) {
            in_function_var_decl = true;
            pending_function_local_init = f_factory.get_last_value();
            setup_data_type(ctx->data_type_or_implicit());
            type_engine.start_range();
            return;
        }
    }
    if (ctx->type_declaration()) {
        if (ctx->type_declaration()->data_type() &&
            ctx->type_declaration()->data_type()->struct_union()
        ) {
            if (ctx->type_declaration()->data_type()->struct_union()->KW_STRUCT())
                type_engine.start_composite_type_declaration(Type_engine::struct_type);
            else if (ctx->type_declaration()->data_type()->struct_union()->KW_UNION())
                type_engine.start_composite_type_declaration(Type_engine::union_type);
            top_level_struct_started = true;
        } else if (ctx->type_declaration()->data_type() &&
                   ctx->type_declaration()->data_type()->KW_ENUM()) {
            type_engine.start_composite_type_declaration(Type_engine::enum_type);
            top_level_struct_started = true;
        } else {
            type_engine.start_simple_type_declaration();
        }
    } else {
        if (ctx->data_type_or_implicit() &&
            ctx->data_type_or_implicit()->data_type()
        ) {
            auto dt = ctx->data_type_or_implicit()->data_type();
            if (dt->struct_union()) {
                in_anonymous_struct = true;
                type_engine.start_composite_type_declaration(Type_engine::struct_type);
                top_level_struct_started = true;
                params_factory.start_param_assignment();
            } else if (dt->KW_ENUM()) {
                type_engine.start_composite_type_declaration(Type_engine::enum_type);
                in_anonymous_struct = true;
            }
        }
    }
}

void sv_visitor::exitData_declaration(sv2017::Data_declarationContext *ctx) {
    if (in_function_var_decl) {
        in_function_var_decl = false;
        auto var_decls = ctx->list_of_variable_decl_assignments();
        auto decl = var_decls && !var_decls->variable_decl_assignment().empty() ? var_decls->variable_decl_assignment(0) : nullptr;
        if (!decl || !decl->identifier()) {
            spdlog::warn("Malformed variable declaration, skipping file");
            had_error = true;
            return;
        }
        auto var_name = decl->identifier()->getText();
        auto t = type_engine.finalize_type();
        if (!t) t = Type_engine::create_primitive_type("implicit");
        auto param = std::make_shared<HDL_parameter>(var_name);
        param->set_type(t);
        f_factory.add_local_variable(param);
        // Preserve `type x = <expr>;` initializers by desugaring to a bare
        // declaration plus an assignment in body position: the existing
        // solver machinery (deps, substitution, walk_body seeding) then
        // applies unchanged. Only for a single `= expr` declarator outside
        // loops (loop machinery owns for-init and loop-body decls, as
        // before), and only if a fresh expression actually completed during
        // this declaration (otherwise assignment_value is stale).
        if (!loops_factory.in_loop() && var_decls->variable_decl_assignment().size() == 1 &&
            decl->ASSIGN() != nullptr && decl->expression() != nullptr) {
            auto init = f_factory.get_last_value();
            if (init && init != pending_function_local_init) {
                auto stmt = std::make_shared<hdl_assignment_statement>();
                stmt->set_target(var_name);
                stmt->set_value(init);
                f_factory.add_statement(stmt);
                if (conditionals_factory.is_active()) {
                    auto last = f_factory.pop_last();
                    if (last) conditionals_factory.add_statement(last);
                }
            }
        }
        pending_function_local_init.reset();
        return;
    }
    if (in_function_composite_decl) {
        in_function_composite_decl = false;
        auto var_decls = ctx->list_of_variable_decl_assignments();
        auto decl = var_decls && !var_decls->variable_decl_assignment().empty() ? var_decls->variable_decl_assignment(0) : nullptr;
        if (!decl || !decl->identifier()) {
            spdlog::warn("Malformed variable declaration, skipping file");
            had_error = true;
            return;
        }
        auto var_name = decl->identifier()->getText();
        auto t = type_engine.stop_composite_type_declaration("", true);
        if (!t) t = Type_engine::create_primitive_type("implicit");
        auto param = std::make_shared<HDL_parameter>(var_name);
        param->set_type(t);
        f_factory.add_local_variable(param);
        // Same initializer desugar as the simple path above.
        if (!loops_factory.in_loop() && var_decls->variable_decl_assignment().size() == 1 &&
            decl->ASSIGN() != nullptr && decl->expression() != nullptr) {
            auto init = f_factory.get_last_value();
            if (init && init != pending_function_local_init) {
                auto stmt = std::make_shared<hdl_assignment_statement>();
                stmt->set_target(var_name);
                stmt->set_value(init);
                f_factory.add_statement(stmt);
                if (conditionals_factory.is_active()) {
                    auto last = f_factory.pop_last();
                    if (last) conditionals_factory.add_statement(last);
                }
            }
        }
        pending_function_local_init.reset();
        return;
    }
    if (ctx->type_declaration()) {
        auto name = ctx->type_declaration()->identifier(0)->getText();
        if (type_engine.is_simple_type()) {
            modules_factory.add_typedef(name, type_engine.stop_type_declaration(name));
        } else {
            modules_factory.add_struct_def(name, type_engine.stop_composite_type_declaration(name, false));
        }
    } else {
        if (ctx->data_type_or_implicit() &&
            ctx->data_type_or_implicit()->data_type()
        ) {
            auto dt = ctx->data_type_or_implicit()->data_type();
            if (dt->struct_union()) {
                in_anonymous_struct = false;
                auto var_decls = ctx->list_of_variable_decl_assignments();
                auto decl = var_decls && !var_decls->variable_decl_assignment().empty() ? var_decls->variable_decl_assignment(0) : nullptr;
                if (!decl || !decl->identifier()) {
                    spdlog::warn("Malformed variable declaration, skipping file");
                    had_error = true;
                    return;
                }
                auto name = decl->identifier()->getText();
                params_factory.set_type(pending_anon_struct_type);
                params_factory.new_parameter(name);
                params_factory.stop_param_assignment();
                auto param = params_factory.get_parameter();
                param->set_type(pending_anon_struct_type);
                modules_factory.add_parameter(param);
            } else if (dt->KW_ENUM()) {
                in_anonymous_struct = false;
                type_engine.stop_composite_type_declaration("", true);
            }
        }
    }
}

void sv_visitor::enterStruct_union_member(sv2017::Struct_union_memberContext *ctx) {
    type_engine.open_composite_member();
}

void sv_visitor::exitStruct_union_member(sv2017::Struct_union_memberContext *ctx) {
    auto var_decls = ctx->list_of_variable_decl_assignments();
    auto decl = var_decls && !var_decls->variable_decl_assignment().empty() ? var_decls->variable_decl_assignment(0) : nullptr;
    if (!decl || !decl->identifier()) {
        spdlog::warn("Malformed struct member declaration, skipping file");
        had_error = true;
        return;
    }
    auto name = decl->identifier()->getText();
    type_engine.close_composite_member(name);
}

void sv_visitor::enterEnum_name_declaration(sv2017::Enum_name_declarationContext *ctx) {
    type_engine.open_composite_member();
}

void sv_visitor::exitEnum_name_declaration(sv2017::Enum_name_declarationContext *ctx) {
    type_engine.close_composite_member(ctx->identifier()->getText());
    if (ctx->expression()) {
        std::string val_text = ctx->expression()->getText();
        auto tick = val_text.find('\'');
        if (tick != std::string::npos) {
            val_text = val_text.substr(tick + 1);
            if (!val_text.empty() && (val_text[0] == 'b' || val_text[0] == 'h'
                || val_text[0] == 'd' || val_text[0] == 'o'))
                val_text = val_text.substr(1);
        }
        char *end = nullptr;
        uint64_t v = std::strtoull(val_text.c_str(), &end, 0);
        if (end != val_text.c_str()) type_engine.set_current_enum_value(v);
    }
}

void sv_visitor::enterData_type_primitive(sv2017::Data_type_primitiveContext *ctx) {
    if (ctx->integer_type()) {
        auto it = ctx->integer_type();
        std::string base_type;
        if (it->integer_vector_type()) {
            base_type = it->integer_vector_type()->getText();
        } else if (it->integer_atom_type()) {
            base_type = it->integer_atom_type()->getText();
        } else {
            base_type = it->getText();
        }
        type_engine.set_type(base_type);
        if (ctx->signing()) {
            type_engine.set_member_signed(ctx->signing()->getText() == "signed");
        }
    }
}

void sv_visitor::enterData_type(sv2017::Data_typeContext *ctx) {
    auto packed =  ctx->KW_PACKED() != nullptr;
    if (type_engine.active() && ctx->struct_union()) {
        if (top_level_struct_started) {
            top_level_struct_started = false;
            if (packed) type_engine.set_packed();
        } else {
            if (ctx->struct_union()->KW_STRUCT())
                type_engine.start_composite_type_declaration(Type_engine::struct_type);
            else if (ctx->struct_union()->KW_UNION())
                type_engine.start_composite_type_declaration(Type_engine::union_type);
            if (packed) type_engine.set_packed();
        }
    } else if (type_engine.active() && packed) {
        type_engine.set_packed();
    }
}

void sv_visitor::exitData_type(sv2017::Data_typeContext *ctx) {
    type_engine.close_packed_dimensions();
    if (type_engine.active() && ctx->struct_union()) {
        if (type_engine.is_nested()) {
            auto completed = type_engine.stop_composite_type_declaration("", true);
            type_engine.set_current_member_type(completed);
        } else if (in_anonymous_struct) {
            pending_anon_struct_type = type_engine.stop_composite_type_declaration("", true);
        }
    }
}

void sv_visitor::exitInterface_header(sv2017::Interface_headerContext *ctx) {
    std::string interface_name = ctx->identifier()->getText();
    if(modules_factory.is_current_valid()){
        hdl_instance_statement statement;
        statement.set_name("__scoped_declaration__");
        statement.set_type(interface_name);
        statement.set_dependency_class(interface);
        modules_factory.add_statement(std::make_shared<hdl_instance_statement>(statement));
    }

}

std::vector<std::shared_ptr<hdl_statement_base>> sv_visitor::get_entities() {
    return entities;
}

void sv_visitor::enterPrimaryTfCall(sv2017::PrimaryTfCallContext *ctx) {
    if(params_factory.is_component_relevant() || f_factory.is_active()){
        std::string call_name = ctx->any_system_tf_identifier()->getText();
        if (!call_name.empty() && call_name[0] == '$' && !is_known_system_function(call_name.substr(1))) {
            spdlog::warn("Unknown system function {} encountered while parsing a parameter expression", call_name);
        }
        if (f_factory.is_active()) {
            f_factory.start_function_call(call_name);
        } else {
            params_factory.start_function_call(call_name);
        }
        if(ctx->data_type()){
            auto scoped_ctx = ctx->data_type()->package_or_class_scoped_path();
            if (scoped_ctx && !scoped_ctx->DOUBLE_COLON().empty()) {
                auto qi = sv_parsing_helpers::parse_qualified_identifier(scoped_ctx);
                auto ec = std::make_shared<Identifier_token>(qi);
                if (f_factory.is_active()) {
                    f_factory.add_component(ec);
                } else {
                    params_factory.add_component(ec);
                }
            } else{
                in_type_argument = true;
                auto ec = build_data_type_expression(ctx->data_type());
                if (f_factory.is_active()) {
                    f_factory.add_call_argument(ec);
                } else {
                    params_factory.add_component(ec, true);
                }
            }

        }
    }
}

void sv_visitor::enterCast_separator(sv2017::Cast_separatorContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.advance_cast();
    } else {
        params_factory.advance_cast();
    }
}

void sv_visitor::enterPrimaryCast2(sv2017::PrimaryCast2Context *ctx) {
    auto primary_text = ctx->primary()->getText();

    // Check if the identifier is a known type/struct/typedef
    if (type_engine.has_type(primary_text)) {
        if (f_factory.is_active()) {
            f_factory.start_cast(false);
            f_factory.set_cast_type(primary_text);
        } else {
            params_factory.start_cast(false);
            params_factory.set_cast_type(primary_text);
        }
    } else {
        auto expression_size = primary_text.starts_with("(") || primary_text.contains("::");
        if (f_factory.is_active()) {
            f_factory.start_cast(expression_size);
        } else {
            params_factory.start_cast(expression_size);
        }
    }
}

void sv_visitor::exitPrimaryCast2(sv2017::PrimaryCast2Context *ctx) {
    if (f_factory.is_active()) {
        f_factory.stop_cast();
    } else {
        params_factory.stop_cast();
    }
}

void sv_visitor::enterPrimaryCast(sv2017::PrimaryCastContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.start_cast(false);
    } else {
        params_factory.start_cast(false);
    }
    std::string cast_type;

    if (ctx->signing()) {
        cast_type = ctx->signing()->getText();
    }else if (ctx->integer_type()) {
        cast_type = ctx->integer_type()->getText();
    } else if (ctx->non_integer_type()) {
        cast_type = ctx->non_integer_type()->getText();
    }
    if (f_factory.is_active()) {
        f_factory.set_cast_type(cast_type);
    } else {
        params_factory.set_cast_type(cast_type);
    }
}

void sv_visitor::exitPrimaryCast(sv2017::PrimaryCastContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.stop_cast();
    } else {
        params_factory.stop_cast();
    }
}

void sv_visitor::enterClass_declaration(sv2017::Class_declarationContext *ctx) {
    in_class = true;
}

void sv_visitor::exitClass_declaration(sv2017::Class_declarationContext *ctx) {
    in_class = false;
}


void sv_visitor::exitPrimaryTfCall(sv2017::PrimaryTfCallContext *ctx) {
    std::string call_name = ctx->any_system_tf_identifier()->getText();
    if(call_name=="$readmemh" || call_name=="$readmemb"){
        auto args = ctx->list_of_arguments();
        auto exprs = args ? args->expression() : std::vector<sv2017::ExpressionContext*>{};
        if (exprs.empty()) {
            spdlog::warn("{} call has no arguments, ignoring memory initialization", call_name);
            return;
        }
        std::string data_file = exprs[0]->getText();
        data_file.erase(std::remove(data_file.begin(), data_file.end(), '\\'), data_file.end());
        data_file.erase(std::remove(data_file.begin(), data_file.end(), '"'), data_file.end());
        std::filesystem::path p = data_file;
        auto ext = p.extension().string();
        if(ext == ".dat"|| ext == ".mem"){
            auto stmt = std::make_shared<hdl_instance_statement>();
            stmt->set_name("__init_file__");
            stmt->set_type(p.stem());
            stmt->set_dependency_class(memory_init);
            modules_factory.add_statement(stmt);
        }
    }
    in_type_argument = false;
    if(params_factory.is_component_relevant()) {
        params_factory.stop_function_call();
    }
    if (f_factory.is_active()) {
        f_factory.stop_function_call();
    }
}

void sv_visitor::enterList_of_arguments(sv2017::List_of_argumentsContext *ctx) {
    // The data_type portion of a system-task call is over once the real
    // argument list starts; stop suppressing expression routing there.
    in_type_argument = false;
}

void sv_visitor::enterPackage_declaration(sv2017::Package_declarationContext *ctx) {
    size_t line_number = ctx->getStart()->getLine();
    auto package_name = ctx->identifier()[0]->getText();
    modules_factory.new_module(package_name, package, line_number);
}

void sv_visitor::exitPackage_declaration(sv2017::Package_declarationContext *ctx) {
    auto package = modules_factory.get_module();
    entities.push_back(package);
}

void sv_visitor::enterPackage_import_declaration(sv2017::Package_import_declarationContext *ctx) {
    // `import pkg::*` / `import pkg::item` — file-level statements on the hdl_file.
    for (auto *item : ctx->package_import_item()) {
        auto stmt = std::make_shared<hdl_import_stmt>();
        if (!item->identifier().empty())
            stmt->set_package(item->identifier(0)->getText());
        if (item->MUL()) {
            stmt->set_wildcard(true);
        } else if (item->identifier().size() > 1) {
            stmt->set_item(item->identifier(1)->getText());
        }
        file_imports.push_back(stmt);
    }
}

void sv_visitor::exitPackage_or_class_scoped_path(sv2017::Package_or_class_scoped_pathContext *ctx) {
    if (type_engine.active() && ctx->DOUBLE_COLON().empty()) {
        type_engine.set_type(ctx->getText());
    }
    if(!ctx->DOUBLE_COLON().empty()){
        auto qi = sv_parsing_helpers::parse_qualified_identifier(ctx);
        auto pkg_prefix = qi.get_package_prefix();
        std::string prefix_str = pkg_prefix.empty() ? "" : pkg_prefix.back();
        auto stmt = std::make_shared<hdl_instance_statement>();
        stmt->set_name(qi.get_name());
        stmt->set_type(prefix_str);
        stmt->set_dependency_class(package);
        modules_factory.add_statement(stmt);
    }
}

void sv_visitor::enterParameter_declaration(sv2017::Parameter_declarationContext *ctx) {
    if (ctx->list_of_type_assignments()) {
        in_param_declaration = true;
        for (auto *ta : ctx->list_of_type_assignments()->type_assignment()) {
            std::string name = ta->identifier()->getText();
            auto p = std::make_shared<HDL_parameter>(name);
            p->is_type_param = true;
            if (auto *dt = ta->data_type()) {
                auto resolved = resolve_data_type(dt);
                if (resolved)
                    p->set_type(resolved);
                else {
                    auto pscp = dt->package_or_class_scoped_path();
                    if (!pscp) {
                        spdlog::warn("Malformed type parameter, skipping file");
                        had_error = true;
                        return;
                    }
                    p->set_raw_value(std::make_shared<Type_ref>(qualified_identifier(pscp->getText())));
                }
            }
            if (modules_factory.is_current_valid())
                modules_factory.add_parameter(p);
            else if (interfaces_factory.is_current_valid())
                interfaces_factory.add_parameter(p);
            if (p->get_type() && p->get_type()->is<HDL_simple_type>()
                && (p->get_type()->as<HDL_simple_type>().get_signed()
                    || !p->get_type()->as<HDL_simple_type>().get_packed_dimensions().empty()))
                type_engine.add_type_param(name, p->get_type());
        }
        return;
    }
    in_param_declaration = true;
    if (!ctx->list_of_param_assignments()) {
        spdlog::warn("Encountered non existent list of parameter declarations");
        had_error = true;
        return;
    }
    auto resolved = setup_data_type(ctx->data_type_or_implicit());
    type_engine.start_range();
    params_factory.set_type(resolved);
}

void sv_visitor::exitParameter_declaration(sv2017::Parameter_declarationContext *ctx) {
    params_factory.set_type(std::make_shared<HDL_simple_type>() );
    in_param_declaration = false;
}

void sv_visitor::enterParameter_port_declaration(sv2017::Parameter_port_declarationContext *ctx) {
    if (ctx->KW_TYPE()) {
        for (auto *ta : ctx->list_of_type_assignments()->type_assignment()) {
            std::string name = ta->identifier()->getText();
            auto p = std::make_shared<HDL_parameter>(name);
            p->is_type_param = true;
            if (auto *dt = ta->data_type()) {
                auto resolved = resolve_data_type(dt);
                if (resolved)
                    p->set_type(resolved);
                else {
                    auto pscp = dt->package_or_class_scoped_path();
                    if (!pscp) {
                        spdlog::warn("Malformed type parameter, skipping file");
                        had_error = true;
                        return;
                    }
                    p->set_raw_value(std::make_shared<Type_ref>(qualified_identifier(pscp->getText())));
                }
            }
            if (modules_factory.is_current_valid())
                modules_factory.add_parameter(p);
            else if (interfaces_factory.is_current_valid())
                interfaces_factory.add_parameter(p);
            if (p->get_type() && p->get_type()->is<HDL_simple_type>()
                && (p->get_type()->as<HDL_simple_type>().get_signed()
                    || !p->get_type()->as<HDL_simple_type>().get_packed_dimensions().empty()))
                type_engine.add_type_param(name, p->get_type());
        }
    }
}

void sv_visitor::enterParameter_override(sv2017::Parameter_overrideContext *ctx) {
    skip_current_defparam = false;
    auto list = ctx->list_of_defparam_assignments();
    if (list && !list->defparam_assignment().empty()) {
        auto da = list->defparam_assignment(0);
        if (da) {
            auto hi = da->hierarchical_identifier();
            if (hi) {
                std::string text = hi->getText();
                if (text.starts_with("$root.") || text.starts_with("$unit.")) {
                    spdlog::warn("Unsupported defparam scope qualifier '{}' ignored", text);
                    skip_current_defparam = true;
                } else {
                    pending_defparam_path.clear();
                    size_t start = 0;
                    while (true) {
                        auto dot = text.find('.', start);
                        if (dot == std::string::npos) break;
                        pending_defparam_path.push_back(text.substr(start, dot - start));
                        start = dot + 1;
                    }
                    pending_defparam_param = text.substr(start);
                }
            }
        }
    }
    params_factory.start_param_assignment();
    params_factory.new_parameter("defparam_value");
}

void sv_visitor::exitParameter_override(sv2017::Parameter_overrideContext *ctx) {
    auto param = params_factory.get_parameter();
    params_factory.stop_param_assignment();
    if (skip_current_defparam) {
        skip_current_defparam = false;
        return;
    }

    auto stmt = std::make_shared<hdl_parameter_override_statement>();
    stmt->set_instance_path(pending_defparam_path);
    stmt->set_parameter_name(pending_defparam_param);
    stmt->set_value(param->get_expression());
    pending_defparam_path.clear();
    pending_defparam_param.clear();
    modules_factory.add_statement(stmt);
}

bool sv_visitor::expression_in_decl_dimensions(antlr4::tree::ParseTree *node) {
    for (auto *p = node ? node->parent : nullptr; p; p = p->parent) {
        if (dynamic_cast<sv2017::Variable_dimensionContext *>(p)) return true;
    }
    return false;
}

void sv_visitor::enterExpression(sv2017::ExpressionContext *ctx) {
    if (loops_factory.in_loop() && loops_factory.in_body()) {
        loops_factory.start_expression(ctx->primary() == nullptr);
    }
    // Initializers of function-local variables belong to the function, even
    // though ranging is still active for the declaration: only [...] dimension
    // bounds stay on the type-engine path.
    bool function_local_init = (in_function_var_decl || in_function_composite_decl) && !expression_in_decl_dimensions(ctx);
    if ((type_engine.active() || type_engine.is_ranging()) && !function_local_init) {
        type_engine.start_expression();
    } else if(!in_streaming_slice && !in_type_argument && (params_factory.is_component_relevant()|| params_factory.is_param_assignment() || params_factory.is_param_override())) {
        if (auto primary = dynamic_cast<sv2017::PrimaryAssigContext*>(ctx->primary())) {
            if (primary->assignment_pattern_expression()) return;
        }
        params_factory.start_expression_new(ctx->primary() == nullptr);
        if(ctx->QUESTIONMARK()){
            params_factory.start_ternary_operator();
        }
    } else if (f_factory.is_active() && !in_streaming_slice) {
            f_factory.start_expression();
            if(ctx->QUESTIONMARK()){
                f_factory.start_ternary();
            }
    }
    if (deps_factory.is_valid_dependency()) {
        deps_factory.start_expression(ctx->primary() == nullptr);
    }
}

void sv_visitor::exitExpression(sv2017::ExpressionContext *ctx) {
    if (loops_factory.in_loop() && loops_factory.in_body()) {
        loops_factory.stop_expression(ctx->primary() == nullptr);
    }
    bool function_local_init = (in_function_var_decl || in_function_composite_decl) && !expression_in_decl_dimensions(ctx);
    if ((type_engine.active() || type_engine.is_ranging()) && !function_local_init) {
        type_engine.stop_expression();
    } else if(!in_streaming_slice && !in_type_argument && (params_factory.is_component_relevant() || params_factory.is_param_assignment() || params_factory.is_param_override())) {
        if (auto primary = dynamic_cast<sv2017::PrimaryAssigContext*>(ctx->primary())) {
            if (primary->assignment_pattern_expression()) return;
        }
        std::string type;
        if(ctx->QUESTIONMARK()){
            params_factory.stop_ternary();
        }
        params_factory.stop_expression_new(ctx->primary() == nullptr);
    }else if (f_factory.is_active() && !in_streaming_slice) {
        if(ctx->QUESTIONMARK()){
            f_factory.stop_ternary();
        }
        f_factory.stop_expression();
        // Nested sub-expressions exit here too, while assignment_value still
        // holds the previous statement's value: only take the condition once
        // the outermost expression just completed (level back to 0).
        if (conditionals_factory.is_active() && !conditionals_factory.has_condition()
            && f_factory.get_expression_level() == 0)
            conditionals_factory.set_condition(f_factory.get_last_value());
        // Case-item values complete here one by one; collect them for the
        // branch condition built when the item body starts.
        if (!case_stack.empty() && case_stack.back().in_values
            && f_factory.get_expression_level() == 0) {
            if (auto v = f_factory.get_last_value())
                case_stack.back().item_values.push_back(v);
        }
    }
    if (deps_factory.is_valid_dependency()) {
        deps_factory.stop_expression(ctx->primary() == nullptr);
    }
}

void sv_visitor::enterConditional_statement(sv2017::Conditional_statementContext *) {
    if (!conditionals_factory.is_active())
        conditionals_factory.new_conditional();
    else if (conditionals_factory.in_else_branch())
        conditionals_factory.add_branch();
    else
        conditionals_factory.push_nested();
}

void sv_visitor::exitConditional_statement(sv2017::Conditional_statementContext *) {
    auto stmt = conditionals_factory.get_conditional();
    if (stmt.is_empty()) return;
    auto ptr = std::make_shared<hdl_conditional_statement>(stmt);
    if (loops_factory.in_loop())
        loops_factory.add_statement(ptr);
    else if (f_factory.is_active())
        f_factory.add_statement(ptr);
}

void sv_visitor::enterCase_statement(sv2017::Case_statementContext *ctx) {
    // Plain `case` in function bodies desugars to an if/else-if chain.
    // casex/casez, matches and inside keep previous behavior, as do cases
    // outside functions. The conditional is activated lazily at the first
    // item body so the selector itself is never taken as a condition.
    if (!f_factory.is_active() || !ctx->case_keyword() || !ctx->case_keyword()->KW_CASE()) return;
    case_stack.push_back({});
}

void sv_visitor::exitCase_statement(sv2017::Case_statementContext *) {
    if (case_stack.empty()) return;
    case_stack.pop_back();
    auto stmt = conditionals_factory.get_conditional();
    if (stmt.is_empty()) return;
    auto ptr = std::make_shared<hdl_conditional_statement>(stmt);
    if (loops_factory.in_loop())
        loops_factory.add_statement(ptr);
    else if (f_factory.is_active())
        f_factory.add_statement(ptr);
}

void sv_visitor::enterCase_item(sv2017::Case_itemContext *ctx) {
    if (case_stack.empty() || !f_factory.is_active()) return;
    auto &frame = case_stack.back();
    frame.in_values = true;
    frame.is_default = ctx->KW_DEFAULT() != nullptr;
    frame.item_values.clear();
    if (!frame.selector) frame.selector = f_factory.get_last_value();
}

void sv_visitor::enterStatement_or_null(sv2017::Statement_or_nullContext *) {
    // A case-item body starts: freeze the collected values into a branch.
    // A default with no prior branches is modeled as an always-true branch,
    // which is equivalent to an else for evaluation purposes.
    if (!case_stack.empty() && case_stack.back().in_values) {
        auto &frame = case_stack.back();
        frame.in_values = false;
        if (!frame.cond_started) {
            frame.cond_started = true;
            if (!conditionals_factory.is_active())
                conditionals_factory.new_conditional();
            else
                conditionals_factory.push_nested();
        }
        conditionals_factory.add_branch();
        if (frame.is_default) {
            conditionals_factory.set_condition(std::make_shared<Numeric_token>("1"));
        } else if (frame.selector && !frame.item_values.empty()) {
            std::shared_ptr<Expression_base> cond;
            for (auto &v : frame.item_values) {
                auto eq = std::make_shared<Expression_v2>();
                eq->set_lhs(frame.selector);
                eq->set_rhs(v);
                eq->set_operation(Expression_v2::case_equal);
                if (!cond) {
                    cond = eq;
                } else {
                    auto or_expr = std::make_shared<Expression_v2>();
                    or_expr->set_lhs(cond);
                    or_expr->set_rhs(eq);
                    or_expr->set_operation(Expression_v2::logical_or);
                    cond = or_expr;
                }
            }
            conditionals_factory.set_condition(cond);
        }
        frame.item_values.clear();
    }
    if (conditionals_factory.is_active())
        conditionals_factory.enter_body_item();
}

void sv_visitor::exitStatement_or_null(sv2017::Statement_or_nullContext *) {
    if (conditionals_factory.is_active())
        conditionals_factory.exit_body_item();
}

void sv_visitor::exitPrimaryLit(sv2017::PrimaryLitContext *ctx) {
    auto text = ctx->getText();
    route_expression_text(text);
    if(deps_factory.is_valid_dependency() && deps_factory.is_in_port()) {
        deps_factory.start_scalar_net(text);
    }
}

void sv_visitor::enterPrimaryPath(sv2017::PrimaryPathContext *ctx) {
    auto dbg = ctx->getText();
    if(deps_factory.is_valid_dependency()){
        if(!deps_factory.is_interface()) {
            deps_factory.add_connection_element(ctx->getText());
            if(deps_factory.is_in_port()) {
                deps_factory.start_scalar_net(ctx->getText());
            }
        }
    }
}


void sv_visitor::exitPrimaryPath(sv2017::PrimaryPathContext *ctx) {
    std::shared_ptr<Expression_base> ec;

    auto scoped_ctx = ctx->package_or_class_scoped_path();
    if (scoped_ctx && !scoped_ctx->DOUBLE_COLON().empty()) {
        auto qi = sv_parsing_helpers::parse_qualified_identifier(scoped_ctx);
        ec = std::make_shared<Identifier_token>(qi);
    } else {
        std::vector<std::string> dot_chain;
        auto node = ctx->parent;
        while (node) {
            if (auto dot = dynamic_cast<sv2017::PrimaryDotContext *>(node)) {
                dot_chain.push_back(dot->identifier()->getText());
                node = dot->parent;
            } else {
                break;
            }
        }
        if (dot_chain.empty()) {
            ec = sv_parsing_helpers::make_value(ctx->getText());
        } else if (scoped_ctx) {
            auto leaf = scoped_ctx->getText();
            qualified_identifier qi(dot_chain.back());
            std::vector<std::string> instance = {leaf};
            for (size_t i = 0; i < dot_chain.size() - 1; ++i)
                instance.push_back(dot_chain[i]);
            qi.set_instance_prefix(instance);
            ec = std::make_shared<Identifier_token>(qi);
        } else {
            spdlog::warn("Malformed primary path, skipping file");
            had_error = true;
            return;
        }
    }

    route_expression_component(ec);
}

void sv_visitor::process_operation(Expression_v2::expression_operator op) {
    params_factory.set_operation(op);
    type_engine.set_operation(op);
    if (loops_factory.in_loop()) loops_factory.set_operation(op);
    if (f_factory.is_active() && !in_streaming_slice) f_factory.set_operation(op);
    if (deps_factory.is_valid_dependency()) deps_factory.set_operation(op);
}

void sv_visitor::exitOperator_plus_minus(sv2017::Operator_plus_minusContext *ctx) {

    if (ctx->PLUS())  process_operation(Expression_v2::expression_operator::add);
    if (ctx->MINUS()) process_operation(Expression_v2::expression_operator::subtract);
}

void sv_visitor::exitOperator_mul_div_mod(sv2017::Operator_mul_div_modContext *ctx) {

    if (ctx->DIV()) process_operation(Expression_v2::expression_operator::divide);
    if (ctx->MOD()) process_operation(Expression_v2::expression_operator::modulo);
    if (ctx->MUL()) process_operation(Expression_v2::expression_operator::multiply);

}


void sv_visitor::exitOperator_shift(sv2017::Operator_shiftContext *ctx) {

    if (ctx->SHIFT_LEFT()) process_operation(Expression_v2::expression_operator::logic_shift_left);
    if (ctx->SHIFT_RIGHT()) process_operation(Expression_v2::expression_operator::logic_shift_right);
    if (ctx->ARITH_SHIFT_LEFT()) process_operation(Expression_v2::expression_operator::arithmetic_shift_left);
    if (ctx->ARITH_SHIFT_RIGHT()) process_operation(Expression_v2::expression_operator::arithmetic_shift_right);
}

void sv_visitor::exitUnary_operator(sv2017::Unary_operatorContext *ctx) {

    if (ctx->PLUS()) process_operation(Expression_v2::expression_operator::add);
    if (ctx->MINUS()) process_operation(Expression_v2::expression_operator::subtract);

    if (auto um = ctx->unary_module_path_operator()) {
        if (um->NOT()) process_operation(Expression_v2::expression_operator::logic_neg);
        if (um->NEG()) process_operation(Expression_v2::expression_operator::bitwise_neg);
        if (um->AMPERSAND()) process_operation(Expression_v2::expression_operator::reduction_and);
        if (um->NAND()) process_operation(Expression_v2::expression_operator::reduction_nand);
        if (um->BAR()) process_operation(Expression_v2::expression_operator::reduction_or);
        if (um->NOR()) process_operation(Expression_v2::expression_operator::reduction_nor);
        if (um->XOR()) process_operation(Expression_v2::expression_operator::reduction_xor);
        if (um->NXOR() || um->XORN()) process_operation(Expression_v2::expression_operator::reduction_xnor);
    }
}

void sv_visitor::exitOperator_cmp(sv2017::Operator_cmpContext *ctx) {

    if (ctx->GT()) process_operation(Expression_v2::expression_operator::greater);
    if (ctx->GE()) process_operation(Expression_v2::expression_operator::greater_equal);
    if (ctx->LT())  process_operation(Expression_v2::expression_operator::less);
    if (ctx->LE()) process_operation(Expression_v2::expression_operator::less_equal);
}

void sv_visitor::exitOperator_eq_neq(sv2017::Operator_eq_neqContext *ctx) {
    if (ctx->EQ()) process_operation(Expression_v2::expression_operator::equal);
    if (ctx->NE()) process_operation(Expression_v2::expression_operator::not_equal);
    if (ctx->CASE_EQ()) process_operation(Expression_v2::expression_operator::case_equal);
    if (ctx->CASE_NE()) process_operation(Expression_v2::expression_operator::case_not_equal);
    if (ctx->WILDCARD_EQ()) process_operation(Expression_v2::expression_operator::wildcard_equal);
    if (ctx->WILDCARD_NE()) process_operation(Expression_v2::expression_operator::wildcard_not_equal);
}

void sv_visitor::exitOperator_bitwise_and(sv2017::Operator_bitwise_andContext *ctx) {
    process_operation(Expression_v2::expression_operator::bitwise_and);
}

void sv_visitor::exitOperator_bitwise_or(sv2017::Operator_bitwise_orContext *ctx) {
    process_operation(Expression_v2::expression_operator::bitwise_or);
}

void sv_visitor::exitOperator_xor(sv2017::Operator_xorContext *ctx) {

    if (ctx->XOR()) process_operation(Expression_v2::expression_operator::bitwise_xor);
    if (ctx->XORN()|| ctx->NXOR()) process_operation(Expression_v2::expression_operator::bitwise_xnor);

}

void sv_visitor::exitOperator_power(sv2017::Operator_powerContext *ctx) {
    process_operation(Expression_v2::expression_operator::power);
}

void sv_visitor::exitOperator_log_and(sv2017::Operator_log_andContext *ctx) {
    process_operation(Expression_v2::expression_operator::logical_and);
};
void sv_visitor::exitOperator_log_or(sv2017::Operator_log_orContext *ctx) {
    process_operation(Expression_v2::expression_operator::logical_or);
};

void sv_visitor::exitAnsi_port_declaration(sv2017::Ansi_port_declarationContext *ctx) {
    if(current_declaration_type == "module"){
        std::string port_name = ctx->port_identifier()->getText();
        HDL_port port;
        port.direction = raw_port;
        if(!ctx->port_direction()){
            if(ctx->DOT()){
                port.direction = interface_port;
                if(ctx->identifier().size() >= 2) {
                    port.if_info.type =ctx->identifier(0)->getText();
                    port.if_info.modport = ctx->identifier(1)->getText();
                }
            } else if(ctx->net_or_var_data_type()){
                port.direction  = interface_port;
                port.if_info.type = ctx->net_or_var_data_type()->getText();
            } else{
                port.direction = raw_port;
            }
        } else{
            std::string dir_s = ctx->port_direction()->getText();
            if(dir_s=="input")
                port.direction = input_port;
            else if(dir_s=="output")
                port.direction = output_port;
            else if(dir_s=="inout")
                port.direction = inout_port;
        }
        modules_factory.add_port(port_name, port);
    }
}

void sv_visitor::enterNamed_port_connection(sv2017::Named_port_connectionContext *ctx) {
    if(deps_factory.is_valid_dependency()){
        if(ctx->port_concatenation_connection()){
            deps_factory.start_concat_port(ctx->identifier()->getText());
        }
        if(ctx->port_replication_connection()) {
            deps_factory.start_replication_port(ctx->identifier()->getText());
        }
        deps_factory.start_port();
    }
}

void sv_visitor::exitNamed_port_connection(sv2017::Named_port_connectionContext *ctx) {
    if (!ctx->identifier()) {
        if (ctx->MUL()) {
            deps_factory.set_wildcard(true);
            return;
        }
        else {
            spdlog::warn("Encountered a named port connection without an identifier, this is not supported yet");
            had_error = true;
            return;
        }
    }
    auto port_name = ctx->identifier()->getText();

    deps_factory.stop_port();
    if(ctx->port_expression_connection()){\
        if(deps_factory.is_valid_dependency()){
            deps_factory.add_port(ctx->identifier()->getText());
        }
    }
    if(ctx->port_concatenation_connection()){
        if(deps_factory.is_valid_dependency()){
            deps_factory.stop_concat_port();
        }
    }
    if(ctx->port_replication_connection()){
        if(deps_factory.is_valid_dependency()){
            deps_factory.add_port(ctx->identifier()->getText());
        }
    }
}

void sv_visitor::enterNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *ctx) {
    params_factory.start_instance_parameter_assignment(ctx->identifier()->getText());
    params_factory.start_param_override();
}

void sv_visitor::exitNamed_parameter_assignment(sv2017::Named_parameter_assignmentContext *ctx) {
    params_factory.stop_param_override();
    auto param = params_factory.get_parameter();
    auto t = type_engine.finalize_dimensions();
    param->set_type(t);
    if(deps_factory.is_valid_dependency()){
        deps_factory.add_parameter(param);
    }
}

void sv_visitor::enterParam_assignment(sv2017::Param_assignmentContext *ctx) {
    auto p_n = ctx->identifier()->getText();
    params_factory.start_param_assignment();
    params_factory.new_parameter(p_n);
}


void sv_visitor::exitParam_assignment(sv2017::Param_assignmentContext *ctx) {
    params_factory.stop_param_assignment();


    auto p_n = ctx->identifier()->getText();
    if(params_factory.in_packed_context()) {
        params_factory.stop_packed_assignment();
    }else if(ctx->replication_assignment()){
        params_factory.start_packed_assignment();
        params_factory.stop_packed_assignment();
    }
    if (!in_class) {
        auto param = params_factory.get_parameter();
        auto t = type_engine.finalize_type();
        if (!t) {
            t = Type_engine::create_primitive_type("implicit");
        }
        param->set_type(t);
        if(modules_factory.is_current_valid()){
            modules_factory.add_parameter(param);
        } else if(interfaces_factory.is_current_valid()){
            interfaces_factory.add_parameter(param);
        }
    }
}


void sv_visitor::enterAssignment_pattern(sv2017::Assignment_patternContext *ctx) {
    if (!ctx->replication_assignment()) params_factory.start_initialization_list();
}

void sv_visitor::exitAssignment_pattern(sv2017::Assignment_patternContext *ctx) {
    bool default_assignment = false;
    if(!ctx->structure_pattern_key().empty()){
        if(ctx->structure_pattern_key()[0]->assignment_pattern_key()){
            if(ctx->structure_pattern_key()[0]->assignment_pattern_key()->KW_DEFAULT()){
                default_assignment = true;
            }
        }
    }

    if (!ctx->replication_assignment()) params_factory.stop_initialization_list(default_assignment);
}


void sv_visitor::exitPrimaryBitSelect(sv2017::PrimaryBitSelectContext *ctx) {
    params_factory.close_array_index();
}


void sv_visitor::exitPrimaryIndex(sv2017::PrimaryIndexContext *ctx) {
    if(deps_factory.is_valid_dependency()){
        deps_factory.add_connection_element(ctx->getText());
    }
}

void sv_visitor::enterPrimaryDot(sv2017::PrimaryDotContext *ctx) {
    if(deps_factory.is_valid_dependency()) {
        deps_factory.start_interface();
        if(!deps_factory.in_concatenation() || deps_factory.is_in_replication()) {
            if(deps_factory.is_in_port()) {
                deps_factory.start_scalar_net(ctx->getText());
            }
        }
    }
    if(params_factory.is_component_relevant()|| f_factory.is_active()) {
        instance_prefix = ctx->primary()->getText();
        instance_item = ctx->identifier()->getText();
    }

}

void sv_visitor::exitPrimaryDot(sv2017::PrimaryDotContext *ctx) {
    if(deps_factory.is_valid_dependency()){
        deps_factory.add_connection_element(ctx->getText());
        deps_factory.stop_interface();
    }
}

void sv_visitor::enterReplication_value(sv2017::Replication_valueContext *ctx) {
    if(deps_factory.is_valid_dependency() && deps_factory.is_in_replication()) {
        deps_factory.advance_replication();
    }
}

void sv_visitor::enterPrimaryCall(sv2017::PrimaryCallContext *ctx) {
    if(params_factory.is_component_relevant()) {
        std::string call_text = ctx->primary()->getText();
        auto pos = call_text.find("::");
        if (pos != std::string::npos) {
            std::string prefix = call_text.substr(0, pos);
            std::string func_name = call_text.substr(pos + 2);
            params_factory.start_function_assignment(func_name);
            params_factory.set_function_package_prefix(prefix);
        } else {
            params_factory.start_function_assignment(call_text);
        }
    } else if (f_factory.is_active()) {
        std::string call_text = ctx->primary()->getText();
        auto pos = call_text.find("::");
        if (pos != std::string::npos) {
            f_factory.start_function_call(call_text.substr(pos + 2));
            f_factory.set_function_package_prefix(call_text.substr(0, pos));
        } else {
            f_factory.start_function_call(call_text);
        }
    }
}


void sv_visitor::exitPrimaryCall(sv2017::PrimaryCallContext *ctx) {
    if(params_factory.is_component_relevant()) {
        params_factory.stop_function_assignment();
    } else if (f_factory.is_active()) {
        f_factory.stop_function_call();
    }
}

void sv_visitor::enterConstant_param_expression(sv2017::Constant_param_expressionContext *ctx) {
    type_engine.stop_range();
    if(ctx->concatenation()){
        params_factory.start_packed_assignment();
    }
}

void sv_visitor::enterBit_select(sv2017::Bit_selectContext *ctx) {
    if (loops_factory.in_loop() && loops_factory.in_body()) {
        loops_factory.start_bit_selection();
    }
    if (f_factory.is_active()) {
        f_factory.start_bit_selection();
    } else {
        params_factory.start_bit_selection();
    }

    deps_factory.start_bit_selection();
}

void sv_visitor::exitBit_select(sv2017::Bit_selectContext *ctx) {
    if (loops_factory.in_loop() && loops_factory.in_body()) {
        loops_factory.stop_bit_selection();
    }
    if (f_factory.is_active()) {
        f_factory.stop_bit_selection();
    } else {
        params_factory.stop_bit_selection();
    }

    deps_factory.stop_bit_selection();
}

void sv_visitor::exitFirst_range_identifier(sv2017::First_range_identifierContext *ctx) {
    type_engine.advance_range();
}


void sv_visitor::exitRange_separator(sv2017::Range_separatorContext *ctx) {
    if(deps_factory.is_valid_dependency()) {
        if(ctx->PLUS()) {
            deps_factory.advance_array_range_phase( "+");
        } else if(ctx->MINUS()) {
            deps_factory.advance_array_range_phase( "-");
        } else {
            deps_factory.advance_array_range_phase("");
        }
    }
    type_engine.advance_range();
}

void sv_visitor::enterRange_expression(sv2017::Range_expressionContext *ctx) {
    type_engine.open_range();
}

void sv_visitor::exitRange_expression(sv2017::Range_expressionContext *ctx) {
   type_engine.close_range();
}

void sv_visitor::enterArray_range_expression(sv2017::Array_range_expressionContext *ctx) {
    type_engine.open_range();
    if(deps_factory.is_valid_dependency()) {
        deps_factory.start_array_range();
    }
}

void sv_visitor::exitArray_range_expression(sv2017::Array_range_expressionContext *ctx) {
    type_engine.close_range();
    if(deps_factory.is_valid_dependency()) {
        deps_factory.stop_array_range();
    }
}

void sv_visitor::enterUnpacked_dimension(sv2017::Unpacked_dimensionContext *ctx) {
    type_engine.close_packed_dimensions();
    type_engine.start_unpacked_dimension_declaration();
}


void sv_visitor::exitConcatenation_item(sv2017::Concatenation_itemContext *ctx) {
    if(deps_factory.is_valid_dependency()) {
        deps_factory.add_concatenation_net();
    }
}


void sv_visitor::enterReplication(sv2017::ReplicationContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.start_replication();
    } else {
        params_factory.start_replication();
    }
    if(deps_factory.is_valid_dependency()) {
        deps_factory.start_replication();
    }
}



void sv_visitor::exitReplication(sv2017::ReplicationContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.stop_replication();
    } else {
        params_factory.stop_replication();
    }
    if(deps_factory.is_valid_dependency()) {
        deps_factory.stop_replication();
    }
}

void sv_visitor::enterReplication_assignment(sv2017::Replication_assignmentContext *ctx) {
    type_engine.stop_range();
    params_factory.start_replication_assignment();
}

void sv_visitor::exitReplication_assignment(sv2017::Replication_assignmentContext *ctx) {
    params_factory.stop_replication_assignment();
}

void sv_visitor::enterConcatenation(sv2017::ConcatenationContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.start_concat();
    } else {
        params_factory.start_concatenation();
    }
}

void sv_visitor::exitConcatenation(sv2017::ConcatenationContext *ctx) {
    if(f_factory.is_active()){
        f_factory.stop_concat();
    } else {
        params_factory.stop_concatenation();
    }
}

void sv_visitor::enterStreaming_concatenation(sv2017::Streaming_concatenationContext *ctx) {
    in_streaming_slice = ctx->slice_size() != nullptr;
    if (f_factory.is_active()) {
        if (ctx->stream_operator()) {
            if (ctx->stream_operator()->SHIFT_LEFT())
                f_factory.set_stream_direction(Streaming::left);
            else
                f_factory.set_stream_direction(Streaming::right);
        }
        return;
    }
    if (ctx->stream_operator()) {
        if (ctx->stream_operator()->SHIFT_LEFT())
            params_factory.set_stream_direction(Streaming::left);
        else
            params_factory.set_stream_direction(Streaming::right);
    }
}

void sv_visitor::exitStreaming_concatenation(sv2017::Streaming_concatenationContext *ctx) {
    in_streaming_slice = false;
}

void sv_visitor::enterStream_concatenation(sv2017::Stream_concatenationContext *ctx) {
    in_streaming_slice = false;
    if (f_factory.is_active()) {
        f_factory.start_streaming();
        return;
    }
    params_factory.start_streaming();
}

void sv_visitor::exitStream_concatenation(sv2017::Stream_concatenationContext *ctx) {
    if (f_factory.is_active()) {
        f_factory.stop_streaming();
        return;
    }
    params_factory.stop_streaming();
}

void sv_visitor::exitSlice_size(sv2017::Slice_sizeContext *ctx) {
    std::string text = ctx->getText();
    if (f_factory.is_active()) {
        f_factory.set_stream_slice_size(sv_parsing_helpers::make_value(text));
        return;
    }
    params_factory.set_stream_slice_size(sv_parsing_helpers::make_value(text));
}


void sv_visitor::exitData_type_or_implicit(sv2017::Data_type_or_implicitContext *ctx) {
    if(!in_param_declaration) {
        params_factory.clear_expression();
    }
}

void sv_visitor::enterLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) {
    if (ctx->list_of_type_assignments()) {
        in_param_declaration = true;
        for (auto *ta : ctx->list_of_type_assignments()->type_assignment()) {
            std::string name = ta->identifier()->getText();
            auto p = std::make_shared<HDL_parameter>(name);
            p->is_type_param = true;
            if (auto *dt = ta->data_type()) {
                auto resolved = resolve_data_type(dt);
                if (resolved)
                    p->set_type(resolved);
                else {
                    auto pscp = dt->package_or_class_scoped_path();
                    if (!pscp) {
                        spdlog::warn("Malformed type parameter, skipping file");
                        had_error = true;
                        return;
                    }
                    p->set_raw_value(std::make_shared<Type_ref>(qualified_identifier(pscp->getText())));
                }
            }
            if (modules_factory.is_current_valid())
                modules_factory.add_parameter(p);
            else if (interfaces_factory.is_current_valid())
                interfaces_factory.add_parameter(p);
            if (p->get_type() && p->get_type()->is<HDL_simple_type>()
                && (p->get_type()->as<HDL_simple_type>().get_signed()
                    || !p->get_type()->as<HDL_simple_type>().get_packed_dimensions().empty()))
                type_engine.add_type_param(name, p->get_type());
        }
        return;
    }
    in_param_declaration = true;
    auto resolved = setup_data_type(ctx->data_type_or_implicit());
    type_engine.start_range();
    params_factory.set_type(resolved);
}

void sv_visitor::exitLocal_parameter_declaration(sv2017::Local_parameter_declarationContext *ctx) {
    params_factory.set_type(std::make_shared<HDL_simple_type>() );
    in_param_declaration = false;
}

void sv_visitor::enterLoop_generate_construct(sv2017::Loop_generate_constructContext *) {
    loops_factory.new_loop();
}

void sv_visitor::exitLoop_generate_construct(sv2017::Loop_generate_constructContext *) {
    if (conditionals_factory.is_active())
        conditionals_factory.add_statement(loops_factory.get_loop_statement());
    else
        modules_factory.add_statement(loops_factory.get_loop_statement());
}

void sv_visitor::enterGenvar_initialization(sv2017::Genvar_initializationContext *ctx) {
    std::string id = ctx->identifier()->getText();
    loops_factory.set_phase(HDL_loops_factory::init);
    params_factory.start_param_assignment();
    params_factory.new_parameter(id);
}

void sv_visitor::exitGenvar_initialization(sv2017::Genvar_initializationContext *ctx) {
    auto param = params_factory.get_parameter();
    params_factory.stop_param_assignment();
    loops_factory.set_loop_init(*param);
    loops_factory.advance_phase();
}

void sv_visitor::enterGenvar_expression(sv2017::Genvar_expressionContext *ctx) {
    params_factory.start_param_assignment();
    params_factory.new_parameter("genvar_expr");
}

void sv_visitor::exitGenvar_expression(sv2017::Genvar_expressionContext *ctx) {
    loops_factory.advance_phase();
    auto param = params_factory.get_parameter();
    auto ex = param->get_expression();
    if (ex) {
        if (ex->is<Expression_v2>()) {
            loops_factory.add_expression(ex->as<Expression_v2>());
        } else {
            Expression_v2 e;
            e.set_lhs(ex);
            loops_factory.add_expression(e);
        }
    }
    params_factory.stop_param_assignment();
}

void sv_visitor::enterConstant_expression(sv2017::Constant_expressionContext *) {
    if (conditionals_factory.is_active()) {
        params_factory.start_param_assignment();
        params_factory.new_parameter("if_cond");
    }
}

void sv_visitor::exitConstant_expression(sv2017::Constant_expressionContext *) {
    if (conditionals_factory.is_active()) {
        auto param = params_factory.get_parameter();
        conditionals_factory.set_condition(param->get_expression());
        params_factory.stop_param_assignment();
    }
}

void sv_visitor::enterIf_generate_construct(sv2017::If_generate_constructContext *) {
    if (!conditionals_factory.is_active())
        conditionals_factory.new_conditional();
    else if (conditionals_factory.in_else_branch())
        conditionals_factory.add_branch();
    else
        conditionals_factory.push_nested();
}

void sv_visitor::enterGenerate_item(sv2017::Generate_itemContext *) {
    if (conditionals_factory.is_active())
        conditionals_factory.enter_body_item();
}

void sv_visitor::exitGenerate_item(sv2017::Generate_itemContext *) {
    if (conditionals_factory.is_active())
        conditionals_factory.exit_body_item();
}

void sv_visitor::exitIf_generate_construct(sv2017::If_generate_constructContext *) {
    auto stmt = conditionals_factory.get_conditional();
    if (stmt.is_empty()) return;
    auto ptr = std::make_shared<hdl_conditional_statement>(stmt);
    if (loops_factory.in_loop())
        loops_factory.add_statement(ptr);
    else if (modules_factory.is_current_valid())
        modules_factory.add_statement(ptr);
}

void sv_visitor::enterUntyped_function_declaration(sv2017::Untyped_function_declarationContext *ctx) {
    auto name = ctx->task_and_function_declaration_common()->identifier()[0]->getText();
    f_factory.set_name(name);
}

void sv_visitor::exitFunction_declaration(sv2017::Function_declarationContext *ctx) {
    auto func_type = ctx->function_data_type_or_implicit();
    std::string ret_type_name;
    if (func_type) {
        auto dv = func_type->data_type_or_void();
        if (dv && !dv->KW_VOID()) {
            auto dt = dv->data_type();
            if (dt && dt->package_or_class_scoped_path()) {
                ret_type_name = dt->package_or_class_scoped_path()->getText();
                f_factory.set_return_type_name(ret_type_name);
            }
        }
    }
    auto func = f_factory.get_function();
    if (modules_factory.is_current_valid()) {
        modules_factory.add_function(func, ret_type_name);
    } else {
        entities.push_back(std::make_shared<hdl_function_statement>(func));
    }
}

void sv_visitor::enterLoop_statement(sv2017::Loop_statementContext *ctx) {
    if(f_factory.is_active()) {
        f_factory.pause();
        loops_factory.new_loop();
    }
}


void sv_visitor::exitLoop_statement(sv2017::Loop_statementContext *ctx) {
    if(f_factory.is_active()) {
        f_factory.add_loop(loops_factory.get_loop_statement());
        loops_factory.clear();
        f_factory.resume();
        if (conditionals_factory.is_active()) {
            auto last = f_factory.pop_last();
            if (last) conditionals_factory.add_statement(last);
        }
    }
}

void sv_visitor::exitStatement_item(sv2017::Statement_itemContext *ctx) {
    if(f_factory.is_active() && loops_factory.in_loop()) {
        loops_factory.close_expression();
    }
}

void sv_visitor::exitAssignment_operator(sv2017::Assignment_operatorContext *ctx) {
    if(f_factory.is_active() && loops_factory.in_loop()) {
        loops_factory.advance_expression();
    }
}


void sv_visitor::enterFor_initialization(sv2017::For_initializationContext *ctx) {
    if(f_factory.is_active()) {
        loops_factory.set_phase(HDL_loops_factory::init);
        if(!ctx->for_variable_declaration().empty()) {
            auto var_assigns = ctx->for_variable_declaration()[0]->for_variable_declaration_var_assign();
            if (var_assigns.empty() || !var_assigns[0]->identifier()) {
                spdlog::warn("Malformed for-loop variable declaration, skipping file");
                had_error = true;
                return;
            }
            loops_factory.add_loop_variable(var_assigns[0]->identifier()->getText());
        }
    }
}

void sv_visitor::exitFor_initialization(sv2017::For_initializationContext *ctx) {
    sv2017BaseListener::exitFor_initialization(ctx);
}

void sv_visitor::enterFor_end_expression(sv2017::For_end_expressionContext *ctx) {
    if(f_factory.is_active()) {
        loops_factory.set_phase(HDL_loops_factory::end);
        params_factory.start_param_assignment();
        params_factory.new_parameter("for_end_expr");
    }
}

void sv_visitor::exitFor_end_expression(sv2017::For_end_expressionContext *ctx) {
    if(f_factory.is_active()) {
        auto param = params_factory.get_parameter();
        auto ex = param->get_expression();
        if (!ex) {
            spdlog::warn("Malformed for-loop end expression, skipping file");
            had_error = true;
            return;
        }
        if (ex->is<Expression_v2>()) {
            loops_factory.add_expression(ex->as<Expression_v2>());
        } else {
            Expression_v2 e;
            e.set_lhs(ex);
            loops_factory.add_expression(e);
        }
        params_factory.stop_param_assignment();
    }
}

void sv_visitor::enterFor_step(sv2017::For_stepContext *ctx) {
    if(f_factory.is_active()) {
        loops_factory.set_phase(HDL_loops_factory::step);
    }
}

void sv_visitor::exitFor_step(sv2017::For_stepContext *ctx) {
    if(f_factory.is_active()) {
        loops_factory.set_phase(HDL_loops_factory::body);
    }
}

void sv_visitor::enterInc_or_dec_expressionPost(sv2017::Inc_or_dec_expressionPostContext *ctx) {
    if(f_factory.is_active()) {
        if(loops_factory.in_definition()) {
            auto name = ctx->variable_lvalue()->getText();
            loops_factory.add_component(sv_parsing_helpers::make_value(name));
            if(ctx->inc_or_dec_operator()->INCR()){
                loops_factory.set_operation(Expression_v2::add);
            } else if(ctx->inc_or_dec_operator()->DECR()){
                loops_factory.set_operation(Expression_v2::subtract);
            }
            loops_factory.add_component(std::make_shared<Numeric_token>("1"));
        }
    }
}

void sv_visitor::exitBlocking_assignment(sv2017::Blocking_assignmentContext *ctx) {
    if(!loops_factory.in_loop() && f_factory.is_active()) {
        f_factory.finish_assignment();
        if (conditionals_factory.is_active()) {
            auto last = f_factory.pop_last();
            if (last) conditionals_factory.add_statement(last);
        }
    }

}

void sv_visitor::enterJump_statement(sv2017::Jump_statementContext *ctx) {
    // `return expr;` assigns to the function itself. Only `return` carries a
    // value; `break`/`continue` are not supported in constant evaluation.
    if (!f_factory.is_active() || !ctx->expression() || !ctx->KW_RETURN()) return;
    if (loops_factory.in_loop()) {
        if (loops_factory.in_body())
            loops_factory.start_assignment(f_factory.get_function_name());
    } else {
        f_factory.start_return();
    }
}

void sv_visitor::exitJump_statement(sv2017::Jump_statementContext *ctx) {
    if (!f_factory.is_active() || !ctx->expression() || !ctx->KW_RETURN()) return;
    if (!loops_factory.in_loop()) {
        f_factory.finish_assignment();
        if (conditionals_factory.is_active()) {
            auto last = f_factory.pop_last();
            if (last) conditionals_factory.add_statement(last);
        }
    }
}

void sv_visitor::enterVariable_lvalue(sv2017::Variable_lvalueContext *ctx) {
    if(f_factory.is_active()) {
        auto hier = ctx->package_or_class_scoped_hier_id_with_select();
        if (!hier || !hier->package_or_class_scoped_path()) {
            spdlog::warn("Unsupported lvalue in function body, skipping file");
            had_error = true;
            return;
        }
        auto var_name = hier->package_or_class_scoped_path()->getText();
        for (size_t i = 0; i < hier->DOT().size(); ++i) {
            auto bit_sel = hier->identifier_with_bit_select(i);
            if (bit_sel && bit_sel->identifier())
                var_name += "." + bit_sel->identifier()->getText();
        }
        if(loops_factory.in_loop()) {
            loops_factory.start_assignment(var_name);
            if (loops_factory.in_body()) {
                auto var_token = sv_parsing_helpers::make_value(var_name);
                loops_factory.add_component(var_token);
            } else if (loops_factory.in_initialization()) {
                auto var_token = sv_parsing_helpers::make_value(var_name);
                loops_factory.add_component(var_token);
            }
        } else {
            f_factory.start_assignment(var_name);
        }

    }
}

void sv_visitor::exitVariable_lvalue(sv2017::Variable_lvalueContext *ctx) {
    if(f_factory.is_active() && !loops_factory.in_loop()) {
        f_factory.close_lvalue();
    }
}


void sv_visitor::exitGenvar_iteration(sv2017::Genvar_iterationContext *ctx) {
   if(ctx->inc_or_dec_operator()) {
       auto str = ctx->identifier()->getText();
       loops_factory.add_component(sv_parsing_helpers::make_value(str));
       if(ctx->inc_or_dec_operator()->INCR()){
           loops_factory.set_operation(Expression_v2::add);
       } else if(ctx->inc_or_dec_operator()->DECR()){
           loops_factory.set_operation(Expression_v2::subtract);
       }
       loops_factory.add_component(std::make_shared<Numeric_token>("1"));
   }
    if (!ctx->genvar_expression()) loops_factory.advance_phase();
}





