# Unit test for libsocket
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen

# environment
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wno-unused-parameter -Wno-missing-field-initializers")
endif()

# libraries
set(gtest_force_shared_crt ON CACHE BOOL "Use /MD in googletest")

if(NOT CHEN_ENABLE_UNIT_TEST)
    add_subdirectory(../lib/libchen/test/lib/googletest/googletest googletest)
endif()

# include path
include_directories(../lib/libchen/test/lib/googletest/googletest/include)

# source codes
file(GLOB_RECURSE INC_TEST src/*.hpp)
file(GLOB_RECURSE SRC_TEST src/*.cpp)

# generate app
add_executable(libsocket_test ${INC_TEST} ${SRC_TEST})

# link library
target_link_libraries(libsocket_test libsocket gtest)

# group files in the IDE(e.g: Xcode and Visual Studio)
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${INC_TEST}")
chen_group_files(${CMAKE_CURRENT_SOURCE_DIR} "${SRC_TEST}")