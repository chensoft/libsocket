# libchen: A General Purpose C++ Toolkit
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(libchen)

# policy CMP0042
if(APPLE)
    SET(CMAKE_MACOSX_RPATH ON)
endif()

# build type
OPTION(LIBCHEN_BUILD_STATIC "Build libchen as a static library." ON)
OPTION(LIBCHEN_BUILD_SHARED "Build libchen as a shared library." ON)

# c++ version
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(UNIX AND NOT APPLE)
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
if(UNIX)
    aux_source_directory(./src/plat/unix SRC_PLAT)
elseif(WIN32)
    aux_source_directory(./src/plat/win SRC_PLAT)
else()
    message(FATAL_ERROR "Unsupported system")
endif()

# generate lib(both static and shared)
# rename library to libchen.a & libchen.so or libchen.dylib
if(LIBCHEN_BUILD_STATIC)
    add_library(
            libchen_static
            STATIC
            ${SRC_BASE}
            ${SRC_DATA}
            ${SRC_MT}
            ${SRC_SYS}
            ${SRC_TIME}
            ${SRC_TOOL}
            ${SRC_PLAT}
    )
    set_target_properties(libchen_static PROPERTIES OUTPUT_NAME libchen PREFIX "")
endif()

if(LIBCHEN_BUILD_SHARED)
    add_library(
            libchen_shared
            SHARED
            ${SRC_BASE}
            ${SRC_DATA}
            ${SRC_MT}
            ${SRC_SYS}
            ${SRC_TIME}
            ${SRC_TOOL}
            ${SRC_PLAT}
    )
    set_target_properties(libchen_shared PROPERTIES OUTPUT_NAME libchen PREFIX "")
endif()