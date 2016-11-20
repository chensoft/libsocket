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

# android ifaddrs
# android-ndk-r13+ provide a cmake toolchain file(require cmake 3.6+)
# use cmake -DCMAKE_TOOLCHAIN_FILE=android-ndk-r13/build/cmake/android.toolchain.cmake \
#           -DANDROID_CPP_FEATURES="rtti exceptions" ..
# to generate android makefile
if(ANDROID)
    add_subdirectory(lib/ifaddrs)
    include_directories(lib/ifaddrs)
    target_link_libraries(libsocket ifaddrs)
endif()

# group files in the IDE(e.g: Xcode and Visual Studio)
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${INC_SOCKET}")
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${SRC_SOCKET}")

# enable unit test for libsocket
option(LIBSOCKET_ENABLE_UNIT_TEST "Enable libsocket unit test." ON)

if(LIBSOCKET_ENABLE_UNIT_TEST)
    add_subdirectory(test)
endif()