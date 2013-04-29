# Find re2c executable and provides macros to generate custom build rules
# The module defines the following variables:
#
#  RE2C_EXECUTABLE - path to the re2c program
#  RE2C_VERSION - version of re2c
#  RE2C_FOUND - true if re2c was found
#
find_program(RE2C_EXECUTABLE NAMES re2c DOC "path to re2c executable")
mark_as_advanced(RE2C_EXECUTABLE)

if(RE2C_EXECUTABLE)
  execute_process(COMMAND ${RE2C_EXECUTABLE} --version
    OUTPUT_VARIABLE RE2C_VERSION
    OUTPUT_STRIP_TRAILING_WHITESPACE
    )
  string(REPLACE "re2c " "" RE2C_VERSION "${RE2C_VERSION}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(RE2C
  REQUIRED_VARS RE2C_EXECUTABLE
  VERSION_VAR RE2C_VERSION
  )

function(RE2C_TARGET input output)
  add_custom_command(OUTPUT ${output}
    COMMAND ${RE2C_EXECUTABLE}
    ARGS ${ARGN} -o ${output} ${input}
    DEPENDS ${input}
    WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
    )
endfunction()
