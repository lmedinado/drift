cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
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

add_executable(test_zip tests/test_zip.cc $<TARGET_OBJECTS:tests_main>)
add_test(NAME test_zip COMMAND test_zip)