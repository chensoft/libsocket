# cmake for android ifaddrs
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
cmake_minimum_required(VERSION 2.8)
project(ifaddrs)

# environment
include(../libchen/cmake/ChenInit.cmake)
include(../libchen/cmake/ChenUtil.cmake)

# include path
include_directories(.)

# source codes
file(GLOB INC_IFADDRS *.h)
file(GLOB SRC_IFADDRS *.c)

# generate lib, build static library by default
# use -DBUILD_SHARED_LIBS=ON if you want to build a shared library
add_library(ifaddrs ${INC_IFADDRS} ${SRC_IFADDRS})
set_target_properties(ifaddrs PROPERTIES OUTPUT_NAME ifaddrs PREFIX "")

# group files in the IDE(e.g: Xcode and Visual Studio)
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${INC_IFADDRS}")
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${SRC_IFADDRS}")