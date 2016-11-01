# Unit test for libsocket
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(test_libsocket)

# environment
include(../lib/libchen/cmake/Environment.cmake)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
endif()

# code coverage
option(LIBSOCKET_TEST_CODE_COVERAGE "Enable libsocket code coverage." OFF)

if(CMAKE_COMPILER_IS_GNUCXX AND LIBSOCKET_TEST_CODE_COVERAGE)
    message("-- Enable code coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage")  # debug, no optimisation, enabling coverage
endif()

# libraries
set(gtest_force_shared_crt ON CACHE BOOL "Use /MD in googletest")

add_subdirectory(.. libsocket)
add_subdirectory(../lib/libchen/test/lib/googletest/googletest googletest)

# include path
include_directories(../include)
include_directories(../lib/libchen/include)
include_directories(../lib/libchen/test/lib/googletest/googletest/include)

# source codes
aux_source_directory(.       SRC_ROOT)
aux_source_directory(src/bsd SRC_BSD)
aux_source_directory(src/ip  SRC_IP)
aux_source_directory(src/net SRC_NET)
aux_source_directory(src/tcp SRC_TCP)
aux_source_directory(src/udp SRC_UDP)
aux_source_directory(src/dns SRC_DNS)

# generate app
add_executable(test_libsocket ${SRC_ROOT} ${SRC_BSD} ${SRC_IP} ${SRC_NET} ${SRC_TCP} ${SRC_UDP} ${SRC_DNS})

# link library
target_link_libraries(test_libsocket libsocket gtest)

# group files in IDEs like Xcode and Visual Studio
source_group("" FILES ${SRC_ROOT})

source_group("src\\bsd" FILES ${SRC_BSD})
source_group("src\\ip"  FILES ${SRC_IP})
source_group("src\\net" FILES ${SRC_NET})
source_group("src\\tcp" FILES ${SRC_TCP})
source_group("src\\udp" FILES ${SRC_UDP})
source_group("src\\dns" FILES ${SRC_DNS})