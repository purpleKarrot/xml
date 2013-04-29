#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(RE2C REQUIRED)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

re2c_target(
  ${CMAKE_CURRENT_SOURCE_DIR}/re2c.in.hpp
  ${CMAKE_CURRENT_BINARY_DIR}/re2c.hpp -s
  )

add_library(xml
  re2c.hpp
  re2c.in.hpp
  reader.cpp
  writer.cpp
  )
