

add_library(ananke_lib STATIC)

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
    data_model/HDL/HDL_Resource.cpp
    data_model/HDL/hdl_ast_node.cpp
    data_model/mm_file.cpp
    data_model/hdl_file.cpp
    data_model/HDL/factories/HDL_instances_factory.cpp
    data_model/HDL/factories/HDL_modules_factory.cpp
    data_model/HDL/factories/HDL_interfaces_factory.cpp
    data_model/HDL/factories/HDL_loops_factory.cpp
    data_model/HDL/factories/HDL_parameters_factory.cpp
    data_model/HDL/factories/resource_factory_base.cpp
    data_model/HDL/parameters/HDL_parameter.cpp
    data_model/HDL/parameters/common/resolved_parameter.cpp
    data_model/HDL/parameters/common/hdl_integer.cpp
    data_model/HDL/types/HDL_simple_type.cpp
    data_model/HDL/types/HDL_struct_type.cpp
    data_model/HDL/types/HDL_external_type.cpp
    data_model/HDL/parameters/components/Expression_v2.cpp
    data_model/HDL/parameters/components/Concatenation.cpp
    data_model/HDL/parameters/components/Ternary.cpp
    data_model/HDL/parameters/components/Replication.cpp
    data_model/HDL/parameters/components/Cast.cpp
    data_model/HDL/parameters/components/token/Identifier_token.cpp
    data_model/HDL/parameters/components/token/Numeric_token.cpp
    data_model/HDL/parameters/components/token/String_token.cpp
    data_model/HDL/parameters/components/HDL_function_call.cpp
    data_model/HDL/statement/hdl_resource_statement.cpp
    data_model/HDL/statement/hdl_function_statement.cpp
    data_model/HDL/statement/hdl_instance_statement.cpp
    data_model/HDL/statement/hdl_loop_statement.cpp
    data_model/HDL/statement/hdl_conditional_statement.cpp
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
    frontend/analysis/vhdl/vhdl_analyzer.cpp
    frontend/analysis/system_verilog/documentation_analyzer.cpp
    frontend/analysis/vhdl/vhdl_visitor.cpp
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

set(TEMPLATE_FILES /
        ../templates/make_project_xlnx.j2 /
        ../templates/make_project_lscc.j2 /
        ../templates/make_bitstream_xlnx.j2)


add_executable(ananke ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    set_property(TARGET ananke PROPERTY INTERPROCEDURAL_OPTIMIZATION True)
endif(CMAKE_BUILD_TYPE STREQUAL "Release")

target_link_libraries(ananke PRIVATE ananke_lib  CLI11::CLI11 )

## INSTALLATION SECTION

install(TARGETS ananke DESTINATION /usr/local/bin)
install(DIRECTORY ../bus_specs/ DESTINATION /usr/local/etc/ananke/specs)

## PACKAGING SECTION

SET(MAJOR_VERSION "1")
SET(MINOR_VERSION "0")
SET(PATCH_VERSION "0")

SET(CPACK_PACKAGE_DESCRIPTION "ananke FPGA/SoC orchestration tool")
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Orchestration tool for FPGA and SoC development")
SET(CPACK_PACKAGE_VENDOR "filippo savi")
SET(CPACK_PACKAGE_CONTACT "filippo.savi@unimore.it")
SET(CPACK_PACKAGE_VERSION_MAJOR "${MAJOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_MINOR "${MINOR_VERSION}")
SET(CPACK_PACKAGE_VERSION_PATCH "${PATCH_VERSION}")
SET(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
SET(CPACK_SOURCE_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}_${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")
set(CPACK_PACKAGE_EXECUTABLES ananke_gen)

#dependencies for this service menu
SET(CPACK_DEBIAN_PACKAGE_DEPENDS " libssl-dev , libantlr4-runtime-dev ")
SET(CPACK_DEBIAN_ARCHITECTURE ${CMAKE_SYSTEM_PROCESSOR})

INCLUDE(CPack)
