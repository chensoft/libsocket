# Unit test for libchen
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(test_libchen)

# environment
include(../cmake/ChenInit.cmake)
include(../cmake/ChenUtil.cmake)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
endif()

# code coverage
option(LIBCHEN_TEST_CODE_COVERAGE "Enable libchen code coverage." OFF)

if(CMAKE_COMPILER_IS_GNUCXX AND LIBCHEN_TEST_CODE_COVERAGE)
    message("-- Enable code coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")  # debug, no optimisation, enabling coverage
endif()

# libraries
set(gtest_force_shared_crt ON CACHE BOOL "Use /MD in googletest")

add_subdirectory(.. libchen)
add_subdirectory(lib/googletest/googletest)

# include path
include_directories(../include)
include_directories(lib/googletest/googletest/include)

# source codes
file(GLOB INC_ROOT *.hpp)
file(GLOB SRC_ROOT *.CPP)
file(GLOB_RECURSE SRC_TEST src/*.cpp)

set(INC_TEST ${INC_ROOT})
set(SRC_TEST ${SRC_ROOT} ${SRC_TEST})

# generate app
add_executable(test_libchen ${INC_TEST} ${SRC_TEST})

# link library
target_link_libraries(test_libchen libchen gtest)

# group files in the IDE(e.g: Xcode and Visual Studio)
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${INC_TEST}")
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${SRC_TEST}")