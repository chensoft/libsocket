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
include_directories(./include)

# source codes
aux_source_directory(./src/base SRC_BASE)
aux_source_directory(./src/data SRC_DATA)
aux_source_directory(./src/mt   SRC_MT)
aux_source_directory(./src/sys  SRC_SYS)
aux_source_directory(./src/tool SRC_TOOL)

aux_source_directory(./src/net/ip SRC_NET_IP)
aux_source_directory(./src/net/tcp SRC_NET_TCP)
aux_source_directory(./src/net/udp SRC_NET_UDP)
aux_source_directory(./src/net/dns SRC_NET_DNS)

# platform codes
if (APPLE)
    # rarely used, if you want to do some things on OS X only
    add_definitions(-DCHEN_OS_APPLE)
endif()

if (UNIX)
    add_definitions(-DCHEN_OS_UNIX)
    aux_source_directory(./src/plat/unix SRC_PLAT)
elseif(WIN32)
    add_definitions(-DCHEN_OS_WIN)
    aux_source_directory(./src/plat/win32 SRC_PLAT)
else()
    message(FATAL_ERROR "Unsupported system")
endif()

# generate lib
add_library(
        chen
        ${SRC_BASE}
        ${SRC_DATA}
        ${SRC_MT}
        ${SRC_SYS}
        ${SRC_TOOL}
        ${SRC_NET_IP}
        ${SRC_NET_TCP}
        ${SRC_NET_UDP}
        ${SRC_NET_DNS}
        ${SRC_PLAT}
)