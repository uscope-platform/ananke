

add_library(ananke_lib STATIC)

add_dependencies(ananke_lib cache_schema_hash)

target_include_directories(ananke_lib PUBLIC
        ${CMAKE_SOURCE_DIR}/includes
        ${CMAKE_SOURCE_DIR}/includes/third_party
)
target_compile_definitions(ananke_lib PRIVATE SPECS_FOLDER="${SPECS_FOLDER}")

target_compile_definitions(ananke_lib PUBLIC BOOST_MP_STANDALONE)

target_sources(ananke_lib
    PRIVATE
    ananke.cpp
    analysis/HDL_ast_builder_v2.cpp
    analysis/passes/pass_manager.cpp
    analysis/parameter_solver.cpp
    analysis/passes/processor_detection.cpp
    analysis/passes/port_solution.cpp
    analysis/topological_sorter.cpp
    analysis/data_acquisition_analysis.cpp
    analysis/control_bus_analysis.cpp
    analysis/type_cast_engine.cpp
    frontend/analysis/system_verilog/type_engine.cpp
    analysis/loop_solver.cpp
    analysis/proxy_bus_analysis.cpp
    Backend/Dependency_resolver.cpp
    Backend/uplatform/application_definition_generator.cpp
    Backend/uplatform/peripheral_definition_generator.cpp
    Backend/Xilinx/Vivado_manager.cpp
    Backend/Xilinx/xilinx_project_generator.cpp
    Backend/new_app_generator.cpp
    Backend/Auxiliary_resolver.cpp
    Backend/project_generator_base.cpp
    Backend/Lattice/lattice_project_generator.cpp
    Backend/Lattice/Radiant_manager.cpp
    Backend/Toolchain_manager.cpp
    Backend/python_script_runner.cpp
    data_model/HDL/hdl_ast_node.cpp
    data_model/mm_file.cpp
    data_model/hdl_file.cpp
    data_model/HDL/factories/HDL_instances_factory.cpp
    data_model/HDL/factories/HDL_modules_factory.cpp
    data_model/HDL/factories/HDL_interfaces_factory.cpp
    data_model/HDL/factories/HDL_loops_factory.cpp
    data_model/HDL/factories/HDL_conditionals_factory.cpp
    data_model/HDL/factories/HDL_parameters_factory.cpp
    data_model/HDL/factories/resource_factory_base.cpp
    data_model/HDL/parameters/HDL_parameter.cpp
    data_model/HDL/parameters/common/resolved_parameter.cpp
    data_model/HDL/parameters/common/hdl_integer.cpp
    data_model/HDL/types/HDL_simple_type.cpp
    data_model/HDL/types/HDL_struct_type.cpp
    data_model/HDL/types/HDL_enum_type.cpp
    data_model/HDL/types/HDL_union_type.cpp
    data_model/HDL/types/HDL_external_type.cpp
    data_model/HDL/parameters/components/Expression_v2.cpp
    data_model/HDL/parameters/components/HDL_builtin_function.cpp
    data_model/HDL/parameters/components/Concatenation.cpp
    data_model/HDL/parameters/components/Ternary.cpp
    data_model/HDL/parameters/components/Replication.cpp
    data_model/HDL/parameters/components/Streaming.cpp
    data_model/HDL/parameters/components/Cast.cpp
    data_model/HDL/parameters/components/token/Identifier_token.cpp
    data_model/HDL/parameters/components/token/Numeric_token.cpp
    data_model/HDL/parameters/components/token/Real_token.cpp
    data_model/HDL/parameters/components/token/Time_token.cpp
    data_model/HDL/parameters/components/token/String_token.cpp
    data_model/HDL/parameters/components/token/Type_ref.cpp
    data_model/HDL/parameters/components/HDL_function_call.cpp
    data_model/HDL/statement/hdl_resource_statement.cpp
    data_model/HDL/statement/hdl_function_statement.cpp
    data_model/HDL/statement/hdl_instance_statement.cpp
    data_model/HDL/statement/hdl_import_stmt.cpp
    data_model/HDL/statement/hdl_loop_statement.cpp
    data_model/HDL/statement/hdl_while_statement.cpp
    data_model/HDL/statement/hdl_repeat_statement.cpp
    data_model/HDL/statement/hdl_do_while_statement.cpp
    data_model/HDL/statement/hdl_conditional_statement.cpp
    data_model/HDL/statement/hdl_parameter_override_statement.cpp
    data_model/HDL/statement/hdl_statement_base.cpp
    data_model/HDL/statement/hdl_instance_statement.cpp
    data_model/HDL/statement/hdl_assignment_statement.cpp
    data_model/HDL/factories/HDL_functions_factory.cpp
    data_model/HDL/factories/nets/HDL_range_factory.cpp
    data_model/HDL/factories/nets/HDL_net_factory.cpp
    data_model/HDL/factories/nets/HDL_repetition_factory.cpp
    data_model/HDL/factories/parameters/cast_factory.cpp
    data_model/HDL/factories/parameters/ranges_factory.cpp
    data_model/HDL/factories/parameters/concatenation_factory.cpp
    data_model/HDL/factories/parameters/replication_factory.cpp
    data_model/HDL/factories/parameters/streaming_factory.cpp
    data_model/HDL/factories/parameters/expressions_factory.cpp
    data_model/HDL/factories/parameters/function_calls_factory.cpp
    data_model/HDL/factories/parameters/ternary_factory.cpp
    data_model/HDL/HDL_net.cpp
    data_model/bus_mapping/bus_specs_manager.cpp
    data_model/bus_mapping/mapper_bus_component.cpp
    data_model/settings_store.cpp
    data_model/data_store.cpp
    data_model/DataFile.cpp
    data_model/documentation/module_documentation.cpp
    data_model/documentation/register_documentation.cpp
    data_model/documentation/field_documentation.cpp
    data_model/documentation/processor_instance.cpp
    data_model/documentation/channel_group.cpp
    data_model/documentation/channel.cpp
    data_model/Depfile.cpp
    data_model/Script.cpp
    data_model/Constraints.cpp
    frontend/analysis/system_verilog/sv_visitor.cpp
    frontend/analysis/system_verilog/sv_parsing_helpers.cpp
    frontend/analysis/system_verilog/sv_analyzer.cpp
    frontend/Repository_walker.cpp
    frontend/repository_index.cpp
    frontend/analysis/vhdl/vhdl_analyzer.cpp
    frontend/analysis/system_verilog/documentation_analyzer.cpp
    frontend/analysis/vhdl/vhdl_visitor.cpp
    frontend/analysis/vhdl/vhdl_type_engine.cpp
    frontend/analysis/system_verilog/preprocessor/sv_preprocessor.cpp
    frontend/analysis/system_verilog/preprocessor/source_mapper.cpp
    frontend/analysis/system_verilog/preprocessor/conditional_solver.cpp
    frontend/analysis/system_verilog/preprocessor/macro_processor.cpp
)

target_link_libraries(ananke_lib PUBLIC
        mgp_sv_lib
        mgp_vh_lib
        antlr4_static
        spdlog::spdlog
        ctre::ctre
        Threads::Threads
        cereal
        nlohmann_json::nlohmann_json
        ValiJSON::valijson
        OpenSSL::SSL
        gtest::gtest
)

add_executable(ananke ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET ananke PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

target_link_libraries(ananke PRIVATE ananke_lib  CLI11::CLI11 )

## INSTALLATION SECTION

install(TARGETS ananke DESTINATION /usr/local/bin)
install(DIRECTORY ../bus_specs/ DESTINATION /usr/local/etc/ananke/specs)

## PACKAGING SECTION

SET(CPACK_PACKAGE_DESCRIPTION "ananke FPGA/SoC orchestration tool")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Orchestration tool for FPGA and SoC development")
SET(CPACK_PACKAGE_VENDOR "filippo savi")
SET(CPACK_PACKAGE_CONTACT "filippo.savi@unimore.it")
SET(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
SET(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${PROJECT_VERSION}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${PROJECT_VERSION}")
set(CPACK_PACKAGE_EXECUTABLES "ananke;ananke")

# Runtime dependencies of the packaged binary (static-linked deps excluded).
SET(CPACK_DEBIAN_PACKAGE_DEPENDS "libssl3, libc6")
if(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64|amd64")
    set(CPACK_DEBIAN_ARCHITECTURE amd64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64|arm64")
    set(CPACK_DEBIAN_ARCHITECTURE arm64)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "armv7|armhf|arm")
    set(CPACK_DEBIAN_ARCHITECTURE armhf)
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "i[3-6]86|x86")
    set(CPACK_DEBIAN_ARCHITECTURE i386)
else()
    set(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})
endif()

INCLUDE(CPack)
