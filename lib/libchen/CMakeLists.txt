# libchen: A General Purpose C++ Toolkit
# Jian Chen <admin@chensoft.com>
# http://chensoft.com
# Licensed under MIT license
# Copyright 2016 Jian Chen
cmake_minimum_required(VERSION 2.8)
project(libchen)

# environment
include(cmake/Environment.cmake)

# include path
include_directories(include)

# source codes
file(GLOB INC_ROOT include/chen/*.hpp)
file(GLOB INC_BASE include/chen/base/*.hpp)
file(GLOB INC_DATA include/chen/data/*.hpp)
file(GLOB INC_MT   include/chen/mt/*.hpp)
file(GLOB INC_SYS  include/chen/sys/*.hpp)
file(GLOB INC_TIME include/chen/time/*.hpp)
file(GLOB INC_TOOL include/chen/tool/*.hpp)

aux_source_directory(src/base SRC_BASE)
aux_source_directory(src/data SRC_DATA)
aux_source_directory(src/mt   SRC_MT)
aux_source_directory(src/sys  SRC_SYS)
aux_source_directory(src/time SRC_TIME)
aux_source_directory(src/tool SRC_TOOL)

# generate lib, default is a static library
# use -DBUILD_SHARED_LIBS=ON if you want to build a shared library
set(ALL_FILES
        ${INC_BASE} ${INC_DATA} ${INC_MT} ${INC_SYS} ${INC_TIME} ${INC_TOOL} ${INC_ROOT}
        ${SRC_BASE} ${SRC_DATA} ${SRC_MT} ${SRC_SYS} ${SRC_TIME} ${SRC_TOOL})

add_library(libchen ${ALL_FILES})
set_target_properties(libchen PROPERTIES OUTPUT_NAME libchen PREFIX "")

# group files in IDEs like Xcode and Visual Studio
source_group("include\\chen"       FILES ${INC_ROOT})
source_group("include\\chen\\base" FILES ${INC_BASE})
source_group("include\\chen\\data" FILES ${INC_DATA})
source_group("include\\chen\\mt"   FILES ${INC_MT})
source_group("include\\chen\\sys"  FILES ${INC_SYS})
source_group("include\\chen\\time" FILES ${INC_TIME})
source_group("include\\chen\\tool" FILES ${INC_TOOL})

source_group("src\\base" FILES ${SRC_BASE})
source_group("src\\data" FILES ${SRC_DATA})
source_group("src\\mt"   FILES ${SRC_MT})
source_group("src\\sys"  FILES ${SRC_SYS})
source_group("src\\time" FILES ${SRC_TIME})
source_group("src\\tool" FILES ${SRC_TOOL})