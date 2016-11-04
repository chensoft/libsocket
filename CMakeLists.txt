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
file(GLOB INC_ROOT      include/socket/*.hpp)
file(GLOB INC_BASE      include/socket/base/*.hpp)
file(GLOB INC_CORE      include/socket/core/*.hpp)
file(GLOB INC_PROT_IP   include/socket/prot/ip/*.hpp)
file(GLOB INC_PROT_INET include/socket/prot/inet/*.hpp)
file(GLOB INC_PROT_TCP  include/socket/prot/tcp/*.hpp)
file(GLOB INC_PROT_UDP  include/socket/prot/udp/*.hpp)
file(GLOB INC_PROT_DNS  include/socket/prot/dns/*.hpp)

aux_source_directory(src/base      SRC_BASE)
aux_source_directory(src/core      SRC_CORE)
aux_source_directory(src/prot/ip   SRC_PROT_IP)
aux_source_directory(src/prot/inet SRC_PROT_INET)
aux_source_directory(src/prot/tcp  SRC_PROT_TCP)
aux_source_directory(src/prot/udp  SRC_PROT_UDP)
aux_source_directory(src/prot/dns  SRC_PROT_DNS)

# generate lib, default is a static library
# use -DBUILD_SHARED_LIBS=ON if you want to build a shared library
set(ALL_FILES
        ${INC_BASE} ${INC_CORE} ${INC_PROT_IP} ${INC_PROT_INET} ${INC_PROT_TCP} ${INC_PROT_UDP} ${INC_PROT_DNS} ${INC_ROOT}
        ${SRC_BASE} ${SRC_CORE} ${SRC_PROT_IP} ${SRC_PROT_INET} ${SRC_PROT_TCP} ${SRC_PROT_UDP} ${SRC_PROT_DNS})

add_library(libsocket ${ALL_FILES})
target_link_libraries(libsocket libchen)
set_target_properties(libsocket PROPERTIES OUTPUT_NAME libsocket PREFIX "")

# group files in IDEs like Xcode and Visual Studio
source_group("include\\socket"            FILES ${INC_ROOT})
source_group("include\\socket\\base"      FILES ${INC_BASE})
source_group("include\\socket\\core"      FILES ${INC_CORE})
source_group("include\\socket\\prot\\ip"  FILES ${INC_PROT_IP})
source_group("include\\socket\\prot\\net" FILES ${INC_PROT_INET})
source_group("include\\socket\\prot\\tcp" FILES ${INC_PROT_TCP})
source_group("include\\socket\\prot\\udp" FILES ${INC_PROT_UDP})
source_group("include\\socket\\prot\\dns" FILES ${INC_PROT_DNS})

source_group("src\\base"      FILES ${SRC_BASE})
source_group("src\\core"      FILES ${SRC_CORE})
source_group("src\\prot\\ip"  FILES ${SRC_PROT_IP})
source_group("src\\prot\\net" FILES ${SRC_PROT_INET})
source_group("src\\prot\\tcp" FILES ${SRC_PROT_TCP})
source_group("src\\prot\\udp" FILES ${SRC_PROT_UDP})
source_group("src\\prot\\dns" FILES ${SRC_PROT_DNS})