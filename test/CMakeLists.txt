# Unit test for libsocket
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(test_libsocket)

# environment
include(../lib/libchen/CMakeEnv.cmake)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
endif()

# libraries
option(LIBCHEN_BUILD_SHARED OFF)
option(LIBSOCKET_BUILD_SHARED OFF)

add_subdirectory(.. libsocket)
add_subdirectory(../lib/libchen/test/lib/googletest/googletest googletest)

# include path
include_directories(../include)
include_directories(../lib/libchen/include)
include_directories(../lib/libchen/test/lib/googletest/googletest/include)

# source codes
aux_source_directory(.       SRC_ROOT)
aux_source_directory(src/net SRC_NET)

# generate app
add_executable(test_libsocket ${SRC_ROOT} ${SRC_NET})

# link library
target_link_libraries(test_libsocket libsocket_static gtest)

# group files in IDEs like Xcode and Visual Studio
source_group("" FILES ${SRC_ROOT})

source_group("src\\net" FILES ${SRC_NET})