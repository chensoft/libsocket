# libchen: The Foundation C++ Library
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 3.1)
project(chen)

# c++ version
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# include path
include_directories(./)

# source codes
aux_source_directory(./chen/base SRC_BASE)
aux_source_directory(./chen/data SRC_DATA)
aux_source_directory(./chen/mt   SRC_MT)
aux_source_directory(./chen/sys  SRC_SYS)
aux_source_directory(./chen/tool SRC_TOOL)

aux_source_directory(./chen/net/ip SRC_NET_IP)
aux_source_directory(./chen/net/so SRC_NET_SO)
aux_source_directory(./chen/net/tcp SRC_NET_TCP)
aux_source_directory(./chen/net/udp SRC_NET_UDP)
aux_source_directory(./chen/net/dns SRC_NET_DNS)

# platform codes
if (APPLE)
    # rarely used, if you want to do some things on OS X only
    add_definitions(-DCHEN_OS_APPLE)
endif()

if (UNIX)
    add_definitions(-DCHEN_OS_UNIX)
    aux_source_directory(./chen/plat/unix SRC_PLAT)
elseif(WIN32)
    add_definitions(-DCHEN_OS_WIN)
    aux_source_directory(./chen/plat/win32 SRC_PLAT)
else()
    message(FATAL_ERROR "Unsupported system")
endif()

# export
set(CHEN_LIBRARIES chen CACHE INTERNAL "")
set(CHEN_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} CACHE INTERNAL "")

# generate lib
add_library(
        chen
        ${SRC_BASE}
        ${SRC_DATA}
        ${SRC_MT}
        ${SRC_SYS}
        ${SRC_TOOL}
        ${SRC_NET_IP}
        ${SRC_NET_SO}
        ${SRC_NET_TCP}
        ${SRC_NET_UDP}
        ${SRC_NET_DNS}
        ${SRC_PLAT}
)