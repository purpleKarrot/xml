#=============================================================================
# Copyright (C) 2013 Daniel Pfeifer <daniel@pfeifer-mail.de>
#
# Distributed under the Boost Software License, Version 1.0.
# See accompanying file LICENSE_1_0.txt or copy at
#   http://www.boost.org/LICENSE_1_0.txt
#=============================================================================

find_package(Boost "1.53" REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

set(test_list
  simple
  )

create_test_sourcelist(test_sources
  main.cpp
  ${test_list}
  )

add_executable(xml_test_driver ${test_sources})
target_link_libraries(xml_test_driver xml)

find_program(VALGRIND_EXECUTABLE valgrind)

foreach(test IN LISTS test_list)
  add_test(NAME xml.${test}
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMAND $<TARGET_FILE:xml_test_driver> ${test} ${test}.xml
    )
  if(VALGRIND_EXECUTABLE)
    add_test(NAME xml.${test}.memcheck
      WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
      COMMAND ${VALGRIND_EXECUTABLE}
        --tool=memcheck --leak-check=full --show-reachable=yes --track-origins=yes
        $<TARGET_FILE:xml_test_driver> ${test} ${test}.xml
      )
    set_tests_properties(xml.${test}.memcheck PROPERTIES
      PASS_REGULAR_EXPRESSION " 0 errors from 0 contexts"
      DEPENDS xml.${test}
      )
  endif()
endforeach()

configure_file(coveralls.in ${CMAKE_BINARY_DIR}/coveralls @ONLY)
