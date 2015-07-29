cmake_minimum_required(VERSION 2.8)
project(chen)

# c++ library
#find_package(Boost REQUIRED)

# include path
#include_directories(${Boost_INCLUDE_DIR})
include_directories(./)

# source codes
aux_source_directory(./tool SRC_TOOL)

# unit test
if ((CMAKE_BUILD_TYPE STREQUAL "Debug") OR (CMAKE_BUILD_TYPE STREQUAL "") OR (CHEN_BUILD_UNIT STREQUAL "true"))
  message("-- Unit enabled")
  aux_source_directory(./unit SRC_UNIT)
endif()

# generate lib
add_library(chen STATIC ${SRC_TOOL} ${SRC_UNIT})

# link library
#target_link_libraries(chen ${Boost_LIBRARIES})