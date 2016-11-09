# libsocket: A Modern C++ Network Library
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(libsocket)

# environment
include(lib/libchen/cmake/ChenInit.cmake)
include(lib/libchen/cmake/ChenUtil.cmake)

# libraries
add_subdirectory(lib/libchen)

# include path
include_directories(include)
include_directories(lib/libchen/include)

# source codes
file(GLOB_RECURSE INC_SOCKET include/socket/*.hpp)
file(GLOB_RECURSE SRC_SOCKET src/*.cpp)

# generate lib, build static library by default
# use -DBUILD_SHARED_LIBS=ON if you want to build a shared library
add_library(libsocket ${INC_SOCKET} ${SRC_SOCKET})
target_link_libraries(libsocket libchen)
set_target_properties(libsocket PROPERTIES OUTPUT_NAME libsocket PREFIX "")

# group files in the IDE(e.g: Xcode and Visual Studio)
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${INC_SOCKET}")
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${SRC_SOCKET}")