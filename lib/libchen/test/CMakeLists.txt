# Unit test for libchen
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(test_libchen)

# environment
include(../CMakeEnv.cmake)

if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter")
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-unused-parameter")
endif()

option(LIBCHEN_TEST_CODE_COVERAGE "Enable libchen code coverage." OFF)

if(CMAKE_COMPILER_IS_GNUCXX AND LIBCHEN_TEST_CODE_COVERAGE)
    message("-- Enable code coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")      # debug, no optimisation
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} --coverage")  # enabling coverage
endif()

# libraries
option(LIBCHEN_BUILD_SHARED OFF)

add_subdirectory(.. libchen)
add_subdirectory(lib/googletest/googletest)

# include path
include_directories(../include)
include_directories(lib/googletest/googletest/include)

# source codes
file(GLOB INC_ROOT *.hpp)

aux_source_directory(.        SRC_ROOT)
aux_source_directory(src/base SRC_BASE)
aux_source_directory(src/data SRC_DATA)
aux_source_directory(src/mt   SRC_MT)
aux_source_directory(src/sys  SRC_SYS)
aux_source_directory(src/tool SRC_TOOL)

# generate app
add_executable(test_libchen ${INC_ROOT} ${SRC_ROOT} ${SRC_BASE} ${SRC_DATA} ${SRC_MT} ${SRC_SYS} ${SRC_TOOL})

# link library
target_link_libraries(test_libchen libchen_static gtest)

# group files in IDEs like Xcode and Visual Studio
source_group("" FILES ${INC_ROOT})
source_group("" FILES ${SRC_ROOT})

source_group("src\\base" FILES ${SRC_BASE})
source_group("src\\data" FILES ${SRC_DATA})
source_group("src\\mt"   FILES ${SRC_MT})
source_group("src\\sys"  FILES ${SRC_SYS})
source_group("src\\tool" FILES ${SRC_TOOL})