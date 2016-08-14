# libsocket: A Modern C++ Network Library
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(libsocket)

# environment
include(lib/libchen/CMakeEnv.cmake)

# build type
OPTION(LIBSOCKET_BUILD_STATIC "Build libsocket as a static library." ON)
OPTION(LIBSOCKET_BUILD_SHARED "Build libsocket as a shared library." ON)

# libraries
add_subdirectory(lib/libchen)

# include path
include_directories(include)
include_directories(lib/libchen/include)

# source codes
file(GLOB INC_ROOT include/socket/*.hpp)
file(GLOB INC_NET  include/socket/net/*.hpp)
file(GLOB INC_TCP  include/socket/tcp/*.hpp)
file(GLOB INC_UDP  include/socket/udp/*.hpp)
file(GLOB INC_DNS  include/socket/dns/*.hpp)

aux_source_directory(src/net  SRC_NET)
aux_source_directory(src/tcp  SRC_TCP)
aux_source_directory(src/udp  SRC_UDP)
aux_source_directory(src/dns  SRC_DNS)
aux_source_directory(src/plat SRC_PLAT)

# generate lib(both static and shared)
# rename library to libsocket.a & libsocket.so or libsocket.dylib
set(ALL_FILES
        ${INC_NET} ${INC_TCP} ${INC_UDP} ${INC_DNS} ${INC_ROOT}
        ${SRC_NET} ${SRC_TCP} ${SRC_UDP} ${SRC_DNS} ${SRC_PLAT})

if(LIBSOCKET_BUILD_STATIC)
    add_library(libsocket_static STATIC ${ALL_FILES})
    target_link_libraries(libsocket_static libchen_static)
    set_target_properties(libsocket_static PROPERTIES OUTPUT_NAME libsocket PREFIX "")
endif()

if(LIBSOCKET_BUILD_SHARED)
    add_library(libsocket_shared SHARED ${ALL_FILES})
    target_link_libraries(libsocket_shared libchen_shared)
    set_target_properties(libsocket_shared PROPERTIES OUTPUT_NAME libsocket PREFIX "")
endif()

# group files in IDEs like Xcode and Visual Studio
source_group("include\\socket"      FILES ${INC_ROOT})
source_group("include\\socket\\net" FILES ${INC_NET})
source_group("include\\socket\\tcp" FILES ${INC_TCP})
source_group("include\\socket\\udp" FILES ${INC_UDP})
source_group("include\\socket\\dns" FILES ${INC_DNS})

source_group("src\\net"  FILES ${SRC_NET})
source_group("src\\tcp"  FILES ${SRC_TCP})
source_group("src\\udp"  FILES ${SRC_UDP})
source_group("src\\dns"  FILES ${SRC_DNS})
source_group("src\\plat" FILES ${SRC_PLAT})