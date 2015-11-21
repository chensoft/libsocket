# libchen: The Foundation C++ Library
# Jian Chen <admin@chensoft.com>
# http://www.chensoft.com
# Licensed under MIT license
# Copyright 2015 Jian Chen

cmake_minimum_required(VERSION 2.8)
project(chen)

# c++ flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# include path
include_directories(./)

# source codes
aux_source_directory(./chen/data SRC_DATA)
aux_source_directory(./chen/lang SRC_LANG)
aux_source_directory(./chen/sync SRC_SYNC)
aux_source_directory(./chen/tool SRC_TOOL)
aux_source_directory(./chen/net/ip SRC_NET_IP)
aux_source_directory(./chen/net/udp SRC_NET_UDP)
aux_source_directory(./chen/net/dns SRC_NET_DNS)

# platform codes
if (UNIX)
    add_definitions(-DCHEN_OS_UNIX)
    aux_source_directory(./chen/plat/unix SRC_PLAT)
elseif(WIN32)
    add_definitions(-DCHEN_OS_WIN32)
    aux_source_directory(./chen/plat/win32 SRC_PLAT)
else()
    message(FATAL_ERROR "Unsupported system")
endif()

# unit test
if ((NOT CMAKE_BUILD_TYPE) OR  (CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "") OR (CHEN_BUILD_TEST STREQUAL "true"))
    message("-- Test enabled")
    aux_source_directory(./test SRC_TEST)
else()
    message("-- Test disabled")
endif()

# generate lib
add_library(chen STATIC ${SRC_DATA} ${SRC_LANG} ${SRC_SYNC} ${SRC_TOOL} ${SRC_NET_IP} ${SRC_NET_UDP} ${SRC_NET_DNS} ${SRC_PLAT} ${SRC_TEST})