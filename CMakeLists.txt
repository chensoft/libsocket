# libchen: A General Purpose C++ Toolkit
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(chen)

# c++ version
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if (UNIX AND NOT APPLE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
endif()

# include path
include_directories(./include)

# source codes
aux_source_directory(./src/base SRC_BASE)
aux_source_directory(./src/data SRC_DATA)
aux_source_directory(./src/mt   SRC_MT)
aux_source_directory(./src/sys  SRC_SYS)
aux_source_directory(./src/time SRC_TIME)
aux_source_directory(./src/tool SRC_TOOL)

# platform codes
if (UNIX)
    aux_source_directory(./src/plat/unix SRC_PLAT)
elseif(WIN32)
    aux_source_directory(./src/plat/win SRC_PLAT)
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
        ${SRC_TIME}
        ${SRC_TOOL}
        ${SRC_PLAT}
)