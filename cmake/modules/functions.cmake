# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

message("functions")

# library
macro(set_library)
  library_get_current_dir_name($ENV{WCH_BASE} lib_name)
  library_set_named(${lib_name})
endmacro()

macro(library_get_current_dir_name base name)
  # Remove the prefix (/home/user/project/driver/serial/CMakeLists.txt => driver/serial/CMakeLists.txt)
  file(RELATIVE_PATH name ${base} ${CMAKE_CURRENT_LIST_FILE})

  # Remove the filename (driver/serial/CMakeLists.txt => driver/serial)
  get_filename_component(name ${name} DIRECTORY)

  # Replace / with __ (driver/serial => driver__serial)
  string(REGEX REPLACE "/" "__" name ${name})

  # Replace : with __ (C:/zephyrproject => C____zephyrproject)
  string(REGEX REPLACE ":" "__" name ${name})

  set(${lib_name} ${name})  
endmacro()

macro(library_set_named name)
  # This is a macro because we need add_library() to be executed
  # within the scope of the caller.
  set(CURRENT_LIBRARY ${name})
  add_library(${name} STATIC "")

  append_cmake_library(${name})

  target_link_libraries(${name} PUBLIC wch_interface)
endmacro()

function(append_cmake_library library)
  # if(TARGET zephyr_prebuilt)
  #   message(WARNING
  #     "zephyr_library() or zephyr_library_named() called in Zephyr CMake "
  #     "application mode. `${library}` will not be treated as a Zephyr library."
  #     "To create a Zephyr library in Zephyr CMake kernel mode consider "
  #     "creating a Zephyr module. See more here: "
  #     "https://docs.zephyrproject.org/latest/guides/modules.html"
  #   )
  # endif()
  set_property(GLOBAL APPEND PROPERTY WCH_LIBS ${library})
endfunction()


# https://cmake.org/cmake/help/latest/command/target_sources.html
function(add_sources)
  foreach(arg ${ARGV})
    if(IS_DIRECTORY ${arg})
      message(FATAL_ERROR "add_sources() was called on a directory")
    endif()
    target_sources(WCH PRIVATE ${arg})
  endforeach()
endfunction()

# https://cmake.org/cmake/help/latest/command/target_include_directories.html
function(include_directories)
  target_include_directories(wch_interface INTERFACE ${ARGV})
endfunction()

# https://cmake.org/cmake/help/latest/command/target_include_directories.html
function(system_include_directories)
  target_include_directories(wch_interface SYSTEM INTERFACE ${ARGV})
endfunction()

# https://cmake.org/cmake/help/latest/command/target_compile_definitions.html
function(compile_definitions)
  target_compile_definitions(wch_interface INTERFACE ${ARGV})
endfunction()

# https://cmake.org/cmake/help/latest/command/target_compile_options.html
function(compile_options)
  target_compile_options(wch_interface INTERFACE ${ARGV})
endfunction()

# https://cmake.org/cmake/help/latest/command/target_link_libraries.html
function(link_libraries)
  target_link_libraries(wch_interface INTERFACE ${ARGV})
endfunction()












# import_kconfig(<prefix> <kconfig_fragment> [<keys>])
#
# Parse a KConfig fragment (typically with extension .config) and
# introduce all the symbols that are prefixed with 'prefix' into the
# CMake namespace. List all created variable names in the 'keys'
# output variable if present.
function(import_kconfig prefix kconfig_fragment)
  # Parse the lines prefixed with 'prefix' in ${kconfig_fragment}
  file(
    STRINGS
    ${kconfig_fragment}
    DOT_CONFIG_LIST
    REGEX "^${prefix}"
    ENCODING "UTF-8"
  )

  foreach (CONFIG ${DOT_CONFIG_LIST})
    # CONFIG could look like: CONFIG_NET_BUF=y

    # Match the first part, the variable name
    string(REGEX MATCH "[^=]+" CONF_VARIABLE_NAME ${CONFIG})

    # Match the second part, variable value
    string(REGEX MATCH "=(.+$)" CONF_VARIABLE_VALUE ${CONFIG})
    # The variable name match we just did included the '=' symbol. To just get the
    # part on the RHS we use match group 1
    set(CONF_VARIABLE_VALUE ${CMAKE_MATCH_1})

    if("${CONF_VARIABLE_VALUE}" MATCHES "^\"(.*)\"$") # Is surrounded by quotes
      set(CONF_VARIABLE_VALUE ${CMAKE_MATCH_1})
    endif()

    set("${CONF_VARIABLE_NAME}" "${CONF_VARIABLE_VALUE}" PARENT_SCOPE)
    list(APPEND keys "${CONF_VARIABLE_NAME}")
  endforeach()

  foreach(outvar ${ARGN})
    set(${outvar} "${keys}" PARENT_SCOPE)
  endforeach()
endfunction()



function(set_ifndef variable value)
  if(NOT ${variable})
    set(${variable} ${value} ${ARGN} PARENT_SCOPE)
  endif()
endfunction()

# will cause a FATAL_ERROR and print an error message if the first
# expression is false
macro(assert test comment)
  if(NOT ${test})
    message(FATAL_ERROR "Assertion failed: ${comment}")
  endif()
endmacro()