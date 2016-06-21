# Unit test for libsocket
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(test_libsocket)

# environment
include(../lib/libchen/CMakeEnv.cmake)

# libraries
OPTION(LIBCHEN_BUILD_SHARED OFF)
OPTION(LIBSOCKET_BUILD_SHARED OFF)

add_subdirectory(.. libsocket)
add_subdirectory(../lib/libchen/test/lib/googletest/googletest googletest)

# include path
include_directories(../include)
include_directories(../lib/libchen/include)
include_directories(../lib/libchen/test/lib/googletest/googletest/include)

# source codes
file(GLOB INC_ROOT *.hpp)
file(GLOB INC_IP   src/ip/*.hpp)

aux_source_directory(./ SRC_ROOT)

# generate app
add_executable(test_libsocket ${SRC_ROOT} ${INC_ROOT} ${INC_IP})

# link library
target_link_libraries(test_libsocket libsocket_static gtest)

# group files in IDEs like Xcode and Visual Studio
source_group("" FILES ${INC_ROOT})
source_group("" FILES ${SRC_ROOT})

source_group("src\\ip" FILES ${INC_IP})