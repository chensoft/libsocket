# libchen: A General Purpose C++ Toolkit
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(libchen)

# environment
include(cmake/ChenInit.cmake)
include(cmake/ChenUtil.cmake)

# include path
include_directories(include)

# source codes
file(GLOB_RECURSE INC_CHEN include/chen/*.hpp)
file(GLOB_RECURSE SRC_CHEN src/*.cpp)

# generate lib, build static library by default
# use -DBUILD_SHARED_LIBS=ON if you want to build a shared library
add_library(libchen ${INC_CHEN} ${SRC_CHEN})
set_target_properties(libchen PROPERTIES OUTPUT_NAME libchen PREFIX "")

# group files in the IDE(e.g: Xcode and Visual Studio)
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${INC_CHEN}")
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${SRC_CHEN}")