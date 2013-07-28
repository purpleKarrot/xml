#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(XML CXX)

list(APPEND CMAKE_MODULE_PATH "${XML_SOURCE_DIR}/CMake")

enable_testing()
include_directories(include)

if(${CMAKE_CXX_COMPILER_ID} MATCHES Clang)
  set(CMAKE_CXX_FLAGS "-std=c++11 -stdlib=libc++")
  set(CMAKE_CXX_LINK_FLAGS "-std=c++11 -stdlib=libc++")
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  set(CMAKE_CXX_FLAGS -std=c++11)
endif()

option(BUILD_COVERAGE "build with gcov support" OFF)
if(BUILD_COVERAGE AND ${CMAKE_CXX_COMPILER_ID} STREQUAL GNU)
  set(CMAKE_CXX_FLAGS_DEBUG
    "${CMAKE_CXX_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage"
    )
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG
    "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage"
    )
endif()

option(BUILD_EXAMPLES "Build examples" OFF)
if(BUILD_EXAMPLES)
  add_subdirectory(example)
endif()

option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
  add_subdirectory(test)
endif()

find_package(RE2C REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/src/re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/re2c.hpp -s
  )

add_library(xml STATIC
  re2c.hpp
  src/re2c.in.hpp
  src/reader.cpp
  src/writer.cpp
  )

set_target_properties(xml PROPERTIES
  INCLUDE_DIRECTORIES "${XML_SOURCE_DIR}/include"
  INTERFACE_INCLUDE_DIRECTORIES "${XML_SOURCE_DIR}/include"
  )
