
set(TESTDATA_DIR ${CMAKE_CURRENT_BINARY_DIR}/check_files/test_data)
file(MAKE_DIRECTORY ${TESTDATA_DIR})

include(FetchContent)
FetchContent_Populate(
        test_data
        GIT_REPOSITORY https://github.com/uscope-platform/uscope_hdl.git
        GIT_TAG        cdb0a6fb17eb1d7b97b70a44404f2d32d8afb8c4
        SOURCE_DIR ${TESTDATA_DIR}
)

set(ANANKE_TESTS
        test_main.cpp
        parameters/parameter_basics.cpp
        parameters/parameter_arrays.cpp
        parameters/parameter_structs.cpp
        parameters/parameter_functions.cpp
        parameters/system_task_tests.cpp
        parameters/parameter_processing.cpp
        parameters/type_parameter_extraction.cpp
        parameters/parameter_defparam.cpp
        frontend/sv_preprocessor.cpp
        frontend/sv_class_tracking.cpp
        frontend/typedef_parsing.cpp
        frontend/source_mapping.cpp
        frontend/repository_walker.cpp
        frontend/vhdl_analyzer.cpp
        data_model/Depfile_test.cpp
        data_model/Script_test.cpp
        data_model/Constraints_test.cpp
        data_model/hdl_resource_testing.cpp
        data_model/initialization_list.cpp
        data_model/settings_store_test.cpp
        data_model/data_store_test.cpp
        data_model/DataFile_test.cpp
        data_model/Token.cpp
        data_model/bus_structure/bus_specs_manager.cpp
        data_model/module_register_documentation.cpp
        analysis/documentation_analyzer.cpp
        analysis/bus_analyzer.cpp
        analysis/analysis.cpp
        analysis/port_extraction.cpp
        analysis/hdl_ast_builder.cpp
        analysis/function_processing.cpp
        analysis/conditional_processing.cpp
        Backend/Application_definition_generator.cpp
        Backend/Peripheral_definition_generator.cpp
        Backend/toolchain_manager.cpp
        Backend/python_script_runner.cpp
        Backend/auxiliary_resolver.cpp
        Backend/project_generator.cpp
        Backend/lattice_project_generator.cpp
        Backend/dependency_resolver.cpp
        end_to_end/full_application.cpp
)


add_executable(tests ${ANANKE_TESTS})

target_link_libraries(tests PRIVATE ananke_lib gtest::gtest)
target_include_directories(tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_compile_definitions(tests PRIVATE TEST_DATA_DIR="${CMAKE_CURRENT_BINARY_DIR}")

# Register with CTest so that `ctest --test-dir build` runs the suite from the
# correct working directory regardless of where it is invoked.
add_test(NAME ananke_tests COMMAND tests)
set_tests_properties(ananke_tests PROPERTIES
    WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
    TIMEOUT 600
)
set(CHECK_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR}/check_files)
file(MAKE_DIRECTORY ${CHECK_FILES_DIR})

configure_file(files/Depfile ${CMAKE_CURRENT_BINARY_DIR} COPYONLY)
configure_file(check_files/Depfile_gs ${CHECK_FILES_DIR} COPYONLY)
configure_file(check_files/test_package.sv ${CHECK_FILES_DIR} COPYONLY)
configure_file(check_files/test_sv_module.sv ${CHECK_FILES_DIR} COPYONLY)
configure_file(check_files/test_vhdl_module.vhd ${CHECK_FILES_DIR} COPYONLY)
configure_file(check_files/top_synth_sv.sv ${CHECK_FILES_DIR} COPYONLY)
configure_file(check_files/test_sv_parameter_extraction.sv ${CHECK_FILES_DIR} COPYONLY)
configure_file(check_files/top_tb_sv.sv ${CHECK_FILES_DIR} COPYONLY)
configure_file(check_files/python_test.py ${CHECK_FILES_DIR} COPYONLY)

set(DOC_ANALYZER_DIR ${CMAKE_CURRENT_BINARY_DIR}/check_files/documentation_analyzer)
file(MAKE_DIRECTORY ${DOC_ANALYZER_DIR})
configure_file(check_files/documentation_analyzer/test_bus_hierarchy.sv ${DOC_ANALYZER_DIR} COPYONLY)
configure_file(check_files/documentation_analyzer/test_module_hierarchy.sv ${DOC_ANALYZER_DIR} COPYONLY)


set(REPO_WALKER_TEST_DIR ${CMAKE_CURRENT_BINARY_DIR}/repository_walker)
file(MAKE_DIRECTORY ${REPO_WALKER_TEST_DIR})
configure_file(check_files/repository_walker/test_sv_module.sv ${REPO_WALKER_TEST_DIR} COPYONLY)
configure_file(check_files/repository_walker/test_vhdl_module.vhd ${REPO_WALKER_TEST_DIR} COPYONLY)
configure_file(check_files/repository_walker/data.dat ${REPO_WALKER_TEST_DIR} COPYONLY)
configure_file(check_files/repository_walker/script_1.tcl ${REPO_WALKER_TEST_DIR} COPYONLY)
configure_file(check_files/repository_walker/script_2.py ${REPO_WALKER_TEST_DIR} COPYONLY)
configure_file(check_files/repository_walker/constraints.xdc ${REPO_WALKER_TEST_DIR} COPYONLY)
file(MAKE_DIRECTORY ${REPO_WALKER_TEST_DIR}/ignored_dir)
configure_file(check_files/repository_walker/ignored_dir/test_sv_module2.sv ${REPO_WALKER_TEST_DIR}/ignored_dir COPYONLY)
file(MAKE_DIRECTORY ${REPO_WALKER_TEST_DIR}/ignored_dir_2)
configure_file(check_files/repository_walker/ignored_dir_2/test_sv_module2.sv ${REPO_WALKER_TEST_DIR}/ignored_dir_2 COPYONLY)
configure_file(check_files/repository_walker/ignored_dir_2/example_proj.xpr ${REPO_WALKER_TEST_DIR}/ignored_dir_2 COPYONLY)
file(MAKE_DIRECTORY ${REPO_WALKER_TEST_DIR}/ignored_dir_3)
configure_file(check_files/repository_walker/ignored_dir_3/.mkignore ${REPO_WALKER_TEST_DIR}/ignored_dir_3 COPYONLY)
configure_file(check_files/repository_walker/ignored_dir_3/test_sv_module2.sv ${REPO_WALKER_TEST_DIR}/ignored_dir_3 COPYONLY)

