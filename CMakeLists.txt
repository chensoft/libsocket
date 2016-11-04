# libsocket: A Modern C++ Network Library
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(libsocket)

# environment
include(lib/libchen/cmake/Environment.cmake)

# libraries
add_subdirectory(lib/libchen)

# include path
include_directories(include)
include_directories(lib/libchen/include)

# source codes
file(GLOB INC_ROOT     include/socket/*.hpp)
file(GLOB INC_BSD      include/socket/bsd/*.hpp)
file(GLOB INC_SYS      include/socket/sys/*.hpp)
file(GLOB INC_NET_IP   include/socket/net/ip/*.hpp)
file(GLOB INC_NET_INET include/socket/net/inet/*.hpp)
file(GLOB INC_NET_TCP  include/socket/net/tcp/*.hpp)
file(GLOB INC_NET_UDP  include/socket/net/udp/*.hpp)
file(GLOB INC_NET_DNS  include/socket/net/dns/*.hpp)

aux_source_directory(src/bsd      SRC_BSD)
aux_source_directory(src/sys      SRC_SYS)
aux_source_directory(src/net/ip   SRC_NET_IP)
aux_source_directory(src/net/inet SRC_NET_INET)
aux_source_directory(src/net/tcp  SRC_NET_TCP)
aux_source_directory(src/net/udp  SRC_NET_UDP)
aux_source_directory(src/net/dns  SRC_NET_DNS)

# generate lib, default is a static library
# use -DBUILD_SHARED_LIBS=ON if you want to build a shared library
set(ALL_FILES
        ${INC_BSD} ${INC_SYS} ${INC_NET_IP} ${INC_NET_INET} ${INC_NET_TCP} ${INC_NET_UDP} ${INC_NET_DNS} ${INC_ROOT}
        ${SRC_BSD} ${SRC_SYS} ${SRC_NET_IP} ${SRC_NET_INET} ${SRC_NET_TCP} ${SRC_NET_UDP} ${SRC_NET_DNS})

add_library(libsocket ${ALL_FILES})
target_link_libraries(libsocket libchen)
set_target_properties(libsocket PROPERTIES OUTPUT_NAME libsocket PREFIX "")

# group files in IDEs like Xcode and Visual Studio
source_group("include\\socket"           FILES ${INC_ROOT})
source_group("include\\socket\\bsd"      FILES ${INC_BSD})
source_group("include\\socket\\sys"      FILES ${INC_SYS})
source_group("include\\socket\\net\\ip"  FILES ${INC_NET_IP})
source_group("include\\socket\\net\\net" FILES ${INC_NET_INET})
source_group("include\\socket\\net\\tcp" FILES ${INC_NET_TCP})
source_group("include\\socket\\net\\udp" FILES ${INC_NET_UDP})
source_group("include\\socket\\net\\dns" FILES ${INC_NET_DNS})

source_group("src\\bsd"      FILES ${SRC_BSD})
source_group("src\\sys"      FILES ${SRC_SYS})
source_group("src\\net\\ip"  FILES ${SRC_NET_IP})
source_group("src\\net\\net" FILES ${SRC_NET_INET})
source_group("src\\net\\tcp" FILES ${SRC_NET_TCP})
source_group("src\\net\\udp" FILES ${SRC_NET_UDP})
source_group("src\\net\\dns" FILES ${SRC_NET_DNS})