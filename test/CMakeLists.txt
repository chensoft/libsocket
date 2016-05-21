# Unit test for libchen
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 3.1)
project(test_libchen)

# c++ version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# libraries
add_subdirectory(.. libchen)
add_subdirectory(googletest/googletest)

# include path
include_directories(..)
include_directories(${CHEN_INCLUDE_DIRS})
include_directories(googletest/googletest/include)

# source codes
aux_source_directory(./ SRC_ROOT)

# generate app
add_executable(test_libchen ${SRC_ROOT})

# link library
target_link_libraries(test_libchen ${CHEN_LIBRARIES} gtest)