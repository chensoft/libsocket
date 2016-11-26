# Unit test for libchen
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(libchen_test)

# environment
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
endif()

# libraries
set(gtest_force_shared_crt ON CACHE BOOL "Use /MD in googletest")

add_subdirectory(lib/googletest/googletest)

# include path
include_directories(lib/googletest/googletest/include)

# source codes
file(GLOB_RECURSE INC_TEST src/*.hpp)
file(GLOB_RECURSE SRC_TEST src/*.cpp)

# generate app
add_executable(libchen_test ${INC_TEST} ${SRC_TEST})

# link library
target_link_libraries(libchen_test libchen gtest)

# group files in the IDE(e.g: Xcode and Visual Studio)
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${INC_TEST}")
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${SRC_TEST}")