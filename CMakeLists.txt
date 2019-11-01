cmake_minimum_required(VERSION 3.0)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_BUILD_TYPE Debug)

#set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/permissive-)
endif (MSVC)
project(drift)

enable_testing()

add_library(drift INTERFACE)
target_include_directories(drift INTERFACE include/)

set(header_files include/drift.h)
target_sources(drift INTERFACE "$<BUILD_INTERFACE:${header_files}>")

# tests
add_library(tests_main OBJECT tests/tests_main.cc)
#include_directories(tests_main OBJECT include/)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(test_zip tests/test_zip.cc $<TARGET_OBJECTS:tests_main>)
add_executable(test_algo tests/test_algo.cc $<TARGET_OBJECTS:tests_main>)
add_executable(test_generator tests/test_gen.cc $<TARGET_OBJECTS:tests_main>)

add_test(NAME test_zip COMMAND test_zip)
add_test(NAME test_algo COMMAND test_algo)
add_test(NAME test_generator COMMAND test_generator)

add_executable(algos_example algos_example.cc)