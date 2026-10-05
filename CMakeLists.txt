cmake_minimum_required(VERSION 3.16)
project(ananke VERSION 1.0.0 LANGUAGES CXX)

enable_testing()

set(CMAKE_CXX_STANDARD 23)

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
find_package(cereal REQUIRED)
find_package(GTest REQUIRED)
find_package(ctre REQUIRED)

if(CMAKE_BUILD_TYPE STREQUAL "Debug" OR CMAKE_BUILD_TYPE STREQUAL "RelWithDebInfo")
    set(SPECS_FOLDER "${CMAKE_SOURCE_DIR}/bus_specs")
else()
    set(SPECS_FOLDER "/usr/local/etc/ananke/specs")
endif()

option(ENABLE_PROFILING "Enable profiling" OFF)
option(ENABLE_COVERAGE "Enable code coverage" OFF)
option(ENABLE_ASAN "Enable AddressSanitizer" OFF)
option(ENABLE_TSAN "Enable ThreadSanitizer" OFF)
option(ENABLE_PEDANTIC "Enable -Wall -Wextra compiler warnings" OFF)
option(ENABLE_PARANOID "Enable -Werror, treating warnings as errors (implies ENABLE_PEDANTIC)" OFF)

if(ENABLE_PROFILING)
    message(STATUS "Enabling Profiling support")
    add_compile_options(--coverage -ftest-coverage)
    add_link_options(--coverage)
endif()


if(ENABLE_COVERAGE)
    message(STATUS "Enabling Code coverage support")
    add_compile_options(--coverage -ftest-coverage)
    add_link_options(--coverage)
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

if(ENABLE_PARANOID)
    set(ENABLE_PEDANTIC ON)
endif()
if(ENABLE_PEDANTIC)
    message(STATUS "Enabling pedantic compiler warnings (-Wall -Wextra)")
    add_compile_options(-Wall -Wextra)
endif()
if(ENABLE_PARANOID)
    message(STATUS "Enabling paranoid mode (warnings as errors)")
    add_compile_options(-Werror)
endif()



# ---------------------------------------------------------------------------
# Persistent cache schema fingerprint.
# The on-disk cache (data_store::unified_cache) embeds the parsed HDL data
# model; whenever any source that defines that model changes, the archive
# format may change. Correctness also depends on frontend analysis semantics
# (what gets harvested and extracted into the cache), so those sources
# version the cache as well. We derive the cache version from a hash of
# those sources so the cache invalidates automatically (no manual version
# bumps) and only when format- or semantics-relevant code changes.
# ---------------------------------------------------------------------------
file(GLOB_RECURSE CACHE_SCHEMA_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_SOURCE_DIR}/includes/data_model/*"
    "${CMAKE_SOURCE_DIR}/src/data_model/*"
    "${CMAKE_SOURCE_DIR}/includes/frontend/analysis/*"
    "${CMAKE_SOURCE_DIR}/src/frontend/analysis/*"
    "${CMAKE_SOURCE_DIR}/includes/frontend/macro_table.hpp"
    "${CMAKE_SOURCE_DIR}/src/frontend/macro_table.cpp"
    "${CMAKE_SOURCE_DIR}/includes/frontend/Repository_walker.hpp"
    "${CMAKE_SOURCE_DIR}/src/frontend/Repository_walker.cpp"
    "${CMAKE_SOURCE_DIR}/includes/frontend/repository_index.hpp"
    "${CMAKE_SOURCE_DIR}/src/frontend/repository_index.cpp"
    "${CMAKE_SOURCE_DIR}/includes/mgp_sv/*"
)

set(CACHE_SCHEMA_HASH_HEADER "${CMAKE_BINARY_DIR}/generated/cache_schema_hash.hpp")
add_custom_command(
    OUTPUT ${CACHE_SCHEMA_HASH_HEADER}
    COMMAND ${CMAKE_COMMAND}
        "-DSCHEMA_ROOT=${CMAKE_SOURCE_DIR}"
        "-DOUTPUT=${CACHE_SCHEMA_HASH_HEADER}"
        -P "${CMAKE_SOURCE_DIR}/cmake/hash_cache_schema.cmake"
    DEPENDS ${CACHE_SCHEMA_SOURCES}
    COMMENT "Generating cache schema fingerprint"
)
add_custom_target(cache_schema_hash DEPENDS ${CACHE_SCHEMA_HASH_HEADER})

include_directories(${CMAKE_BINARY_DIR}/generated)

add_subdirectory(includes)
add_subdirectory(src)
add_subdirectory(Testing)
add_subdirectory(grammars)




