# Unit test for libchen
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(test_libchen)

# environment
include(../CMakeEnv.cmake)

# libraries
OPTION(LIBCHEN_BUILD_SHARED OFF)

add_subdirectory(.. libchen)
add_subdirectory(googletest/googletest)

# include path
include_directories(../include)
include_directories(googletest/googletest/include)

# source codes
file(GLOB INC_ROOT *.hpp)
file(GLOB INC_BASE src/base/*.hpp)
file(GLOB INC_SYS  src/sys/*.hpp)
file(GLOB INC_TOOL src/tool/*.hpp)

aux_source_directory(./ SRC_ROOT)

# generate app
add_executable(test_libchen ${INC_ROOT} ${INC_BASE} ${INC_SYS} ${INC_TOOL} ${SRC_ROOT})

# link library
target_link_libraries(test_libchen libchen_static gtest)

# group files in IDEs like Xcode and Visual Studio
source_group("" FILES ${INC_ROOT})
source_group("" FILES ${SRC_ROOT})

source_group("src\\base" FILES ${INC_BASE})
source_group("src\\sys"  FILES ${INC_SYS})
source_group("src\\tool" FILES ${INC_TOOL})