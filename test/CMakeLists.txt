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
aux_source_directory(.      SRC_ROOT)
aux_source_directory(src/ip SRC_IP)
aux_source_directory(src/so SRC_SO)

# generate app
add_executable(test_libsocket ${SRC_ROOT} ${SRC_IP} ${SRC_SO})

# link library
target_link_libraries(test_libsocket libsocket_static gtest)

# group files in IDEs like Xcode and Visual Studio
source_group("" FILES ${SRC_ROOT})

source_group("src\\ip" FILES ${SRC_IP})
source_group("src\\so" FILES ${SRC_SO})