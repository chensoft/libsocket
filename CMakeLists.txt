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
file(GLOB INC_ROOT include/socket/*.hpp)
file(GLOB INC_BSD  include/socket/bsd/*.hpp)
file(GLOB INC_IP   include/socket/ip/*.hpp)
file(GLOB INC_NET  include/socket/net/*.hpp)
file(GLOB INC_TCP  include/socket/tcp/*.hpp)
file(GLOB INC_UDP  include/socket/udp/*.hpp)
file(GLOB INC_DNS  include/socket/dns/*.hpp)

aux_source_directory(src/bsd  SRC_BSD)
aux_source_directory(src/ip   SRC_IP)
aux_source_directory(src/net  SRC_NET)
aux_source_directory(src/tcp  SRC_TCP)
aux_source_directory(src/udp  SRC_UDP)
aux_source_directory(src/dns  SRC_DNS)

# generate lib, default is a static library
# use -DBUILD_SHARED_LIBS=ON if you want to build a shared library
set(ALL_FILES
        ${INC_BSD} ${INC_IP} ${INC_NET} ${INC_TCP} ${INC_UDP} ${INC_DNS} ${INC_ROOT}
        ${SRC_BSD} ${SRC_IP} ${SRC_NET} ${SRC_TCP} ${SRC_UDP} ${SRC_DNS})

add_library(libsocket ${ALL_FILES})
target_link_libraries(libsocket libchen)
set_target_properties(libsocket PROPERTIES OUTPUT_NAME libsocket PREFIX "")

# group files in IDEs like Xcode and Visual Studio
source_group("include\\socket"       FILES ${INC_ROOT})
source_group("include\\socket\\bsd"  FILES ${INC_BSD})
source_group("include\\socket\\ip"   FILES ${INC_IP})
source_group("include\\socket\\net"  FILES ${INC_NET})
source_group("include\\socket\\tcp"  FILES ${INC_TCP})
source_group("include\\socket\\udp"  FILES ${INC_UDP})
source_group("include\\socket\\dns"  FILES ${INC_DNS})

source_group("src\\bsd"  FILES ${SRC_BSD})
source_group("src\\ip"   FILES ${SRC_IP})
source_group("src\\net"  FILES ${SRC_NET})
source_group("src\\tcp"  FILES ${SRC_TCP})
source_group("src\\udp"  FILES ${SRC_UDP})
source_group("src\\dns"  FILES ${SRC_DNS})