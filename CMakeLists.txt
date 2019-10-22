cmake_minimum_required(VERSION 3.0)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_COMPILER "clang")
set(CMAKE_CXX_COMPILER "clang++")
#set(CMAKE_CXX_EXTENSIONS OFF)

if (MSVC)
    add_compile_options(/permissive-)
endif (MSVC)
project(ranges)

enable_testing()

add_library(ranges INTERFACE )
target_include_directories(ranges INTERFACE include/)

set(header_files include/ranges.h)
target_sources(ranges INTERFACE "$<BUILD_INTERFACE:${header_files}>")

# tests
add_library(tests_main OBJECT tests/tests_main.cc)

add_executable(test_zip tests/test_zip.cc $<TARGET_OBJECTS:tests_main>)
add_test(NAME test_zip COMMAND test_zip)

#add_executable(test1 test1.cc $<TARGET_OBJECTS:test_main>)
#add_test(NAME test1 COMMAND test1)

#add_executable(test2 test2.cc $<TARGET_OBJECTS:test_main>)
#add_test(NAME test2 COMMAND test2 )