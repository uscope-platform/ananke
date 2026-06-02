cmake_minimum_required(VERSION 3.16)
project(ananke)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(OPENSSL_USE_STATIC_LIBS TRUE)
set(THREADS_PREFER_PTHREAD_FLAG ON)
set(ANTLR_VERSION 4.13.1)

find_package(OpenSSL REQUIRED)
find_package(Threads REQUIRED)
find_package(antlr4-runtime CONFIG REQUIRED)
find_package(CLI11 REQUIRED)
find_package(spdlog REQUIRED)
find_package(nlohmann_json REQUIRED)
find_package(valijson REQUIRED)
find_package(cereal)
find_package(GTest REQUIRED)
find_package(ctre REQUIRED)
find_package(tomlplusplus REQUIRED)

include_directories(includes)

include_directories(${inja_INCLUDE_DIRS})

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_definitions(-DTEMPLATES_FOLDER="${CMAKE_SOURCE_DIR}/templates")
    add_definitions(-DSPECS_FOLDER="${CMAKE_SOURCE_DIR}/bus_specs")
    add_compile_options(--coverage -ftest-coverage)
    add_link_options(--coverage)
elseif(CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    add_definitions(-DTEMPLATES_FOLDER="${CMAKE_SOURCE_DIR}/templates")
    add_definitions(-DSPECS_FOLDER="${CMAKE_SOURCE_DIR}/bus_specs")
else()
    add_definitions(-DTEMPLATES_FOLDER="/usr/local/etc/ananke/templates")
    add_definitions(-DSPECS_FOLDER="/usr/local/etc/ananke/specs")
endif(CMAKE_BUILD_TYPE STREQUAL "Debug")

option(ENABLE_PROFILING "Enable profiling" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)

if(ENABLE_PROFILING)
    message(STATUS "Enabling Profiling support")
    add_compile_options(-fprofile-arcs)
endif()


if(ENABLE_ASAN)
    message(STATUS "Enabling AddressSanitizer")
    add_compile_options(-fsanitize=address -fno-omit-frame-pointer)
    add_link_options(-fsanitize=address)
elseif(ENABLE_TSAN)
    message(STATUS "Enabling ThreadSanitizer")
    # TSan requires at least -O1 optimization to be effective
    add_compile_options(-fsanitize=thread -g -O1 -fno-omit-frame-pointer)
    add_link_options(-fsanitize=thread)
endif()



add_subdirectory(includes)
add_subdirectory(src)
add_subdirectory(Testing)
add_subdirectory(grammars)

ExternalProject_Add(antlr_runtime
        GIT_REPOSITORY https://github.com/antlr/antlr4.git
        GIT_TAG ${ANTLR_VERSION}
        CONFIGURE_COMMAND cmake ${CMAKE_BINARY_DIR}/antlr_runtime-prefix/src/antlr_runtime/runtime/Cpp
        INSTALL_COMMAND DESTDIR=${CMAKE_BINARY_DIR}/antlr_runtime/dist make install
        )

add_custom_target(parsers)
add_dependencies(parsers vh_parser_gen sv_parser_gen)




