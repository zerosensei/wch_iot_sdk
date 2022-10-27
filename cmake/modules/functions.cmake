# SPDX-License-Identifier: Apache-2.0

include_guard(GLOBAL)

include(CheckCCompilerFlag)
include(CheckCXXCompilerFlag)

function(get_compile_options_for_lang lang i)
  get_parse_args(args ${ARGN})
  get_property(flags TARGET wch_interface PROPERTY INTERFACE_COMPILE_OPTIONS)

  process_flags(${lang} flags output_list)
  string(REPLACE ";" "$<SEMICOLON>" genexp_output_list "${output_list}")

  set_ifndef(args_DELIMITER "$<SEMICOLON>")
  set(result_output_list "$<JOIN:${genexp_output_list},${args_DELIMITER}>")

  set(${i} ${result_output_list} PARENT_SCOPE)
endfunction()

function(get_include_directories_for_lang lang i)
  get_parse_args(args ${ARGN})
  get_property(flags TARGET wch_interface PROPERTY INTERFACE_INCLUDE_DIRECTORIES)

  process_flags(${lang} flags output_list)
  string(REPLACE ";" "$<SEMICOLON>" genexp_output_list "${output_list}")

  if(NOT ARGN)
    set(result_output_list "-I$<JOIN:${genexp_output_list},$<SEMICOLON>-I>")
  elseif(args_STRIP_PREFIX)
    # The list has no prefix, so don't add it.
    set(result_output_list ${output_list})
  elseif(args_DELIMITER)
    set(result_output_list "-I$<JOIN:${genexp_output_list},${args_DELIMITER}-I>")
  endif()
  set(${i} ${result_output_list} PARENT_SCOPE)
endfunction()

function(process_flags lang input output)
  # The flags might contains compile language generator expressions that
  # look like this:
  # $<$<COMPILE_LANGUAGE:CXX>:-fno-exceptions>
  # $<$<COMPILE_LANGUAGE:CXX>:$<OTHER_EXPRESSION>>
  #
  # Flags that don't specify a language like this apply to all
  # languages.
  #
  # See COMPILE_LANGUAGE in
  # https://cmake.org/cmake/help/v3.3/manual/cmake-generator-expressions.7.html
  #
  # To deal with this, we apply a regex to extract the flag and also
  # to find out if the language matches.
  #
  # If this doesn't work out we might need to ban the use of
  # COMPILE_LANGUAGE and instead partition C, CXX, and ASM into
  # different libraries
  set(languages C CXX ASM)

  set(tmp_list "")

  foreach(flag ${${input}})
    set(is_compile_lang_generator_expression 0)
    foreach(l ${languages})
      if(flag MATCHES "<COMPILE_LANGUAGE:${l}>:([^>]+)>")
        set(updated_flag ${CMAKE_MATCH_1})
        set(is_compile_lang_generator_expression 1)
        if(${l} STREQUAL ${lang})
          # This test will match in case there are more generator expressions in the flag.
          # As example: $<$<COMPILE_LANGUAGE:C>:$<OTHER_EXPRESSION>>
          #             $<$<OTHER_EXPRESSION:$<COMPILE_LANGUAGE:C>:something>>
          string(REGEX MATCH "(\\\$<)[^\\\$]*(\\\$<)[^\\\$]*(\\\$<)" IGNORE_RESULT ${flag})
          if(CMAKE_MATCH_2)
            # Nested generator expressions are used, just substitute `$<COMPILE_LANGUAGE:${l}>` to `1`
            string(REGEX REPLACE "\\\$<COMPILE_LANGUAGE:${l}>" "1" updated_flag ${flag})
          endif()
          list(APPEND tmp_list ${updated_flag})
          break()
        endif()
      endif()
    endforeach()

    if(NOT is_compile_lang_generator_expression)
      # SHELL is used to avoid de-duplication, but when process flags
      # then this tag must be removed to return real compile/linker flags.
      if(flag MATCHES "SHELL:[ ]*(.*)")
        separate_arguments(flag UNIX_COMMAND ${CMAKE_MATCH_1})
      endif()
      # Flags may be placed inside generator expression, therefore any flag
      # which is not already a generator expression must have commas converted.
      if(NOT flag MATCHES "\\\$<.*>")
        string(REPLACE "," "$<COMMA>" flag "${flag}")
      endif()
      list(APPEND tmp_list ${flag})
    endif()
  endforeach()

  set(${output} ${tmp_list} PARENT_SCOPE)
endfunction()


# wch library
macro(set_library)
  library_get_current_dir_name(${ENV_WCH_BASE} lib_name)
  library_set_named(${lib_name})
endmacro()

macro(library_get_current_dir_name base lib_name)
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
function(wch_sources)
  foreach(arg ${ARGV})
    if(IS_DIRECTORY ${arg})
      message(FATAL_ERROR "add_sources() was called on a directory")
    endif()
    target_sources(wch PRIVATE ${arg})
  endforeach()
endfunction()

# https://cmake.org/cmake/help/latest/command/target_include_directories.html
function(wch_include_directories)
  target_include_directories(wch_interface INTERFACE ${ARGV})
endfunction()

# https://cmake.org/cmake/help/latest/command/target_include_directories.html
function(wch_system_include_directories)
  target_include_directories(wch_interface SYSTEM INTERFACE ${ARGV})
endfunction()

# https://cmake.org/cmake/help/latest/command/target_compile_definitions.html
function(wch_compile_definitions)
  target_compile_definitions(wch_interface INTERFACE ${ARGV})
endfunction()

# https://cmake.org/cmake/help/latest/command/target_compile_options.html
function(wch_compile_options)
  target_compile_options(wch_interface INTERFACE ${ARGV})
endfunction()

# https://cmake.org/cmake/help/latest/command/target_link_libraries.html
function(wch_link_libraries)
  target_link_libraries(wch_interface INTERFACE ${ARGV})
endfunction()

function(wch_ld_options)
  target_ld_options(wch_interface INTERFACE ${ARGV})  
endfunction()

function(wch_ld_option_ifdef feature_toggle)
  if(${${feature_toggle}})
    wch_ld_options(${ARGN})
  endif()
endfunction()


# library
function(library_include_directories)
  target_include_directories(${CURRENT_LIBRARY} PRIVATE ${ARGN})
endfunction()

function(library_link_libraries item)
  target_link_libraries(${CURRENT_LIBRARY} PUBLIC ${item} ${ARGN})
endfunction()

function(library_compile_definitions item)
  target_compile_definitions(${CURRENT_LIBRARY} PRIVATE ${item} ${ARGN})
endfunction()

function(add_subdirectory_ifdef feature_toggle source_dir)
  if(${${feature_toggle}})
    add_subdirectory(${source_dir} ${ARGN})
  endif()
endfunction()

function(target_sources_ifdef feature_toggle target scope item)
  if(${${feature_toggle}})
    target_sources(${target} ${scope} ${item} ${ARGN})
  endif()
endfunction()

function(target_compile_definitions_ifdef feature_toggle target scope item)
  if(${${feature_toggle}})
    target_compile_definitions(${target} ${scope} ${item} ${ARGN})
  endif()
endfunction()

function(target_include_directories_ifdef feature_toggle target scope item)
  if(${${feature_toggle}})
    target_include_directories(${target} ${scope} ${item} ${ARGN})
  endif()
endfunction()

function(target_link_libraries_ifdef feature_toggle target item)
  if(${${feature_toggle}})
    target_link_libraries(${target} ${item} ${ARGN})
  endif()
endfunction()

function(add_compile_option_ifdef feature_toggle option)
  if(${${feature_toggle}})
    add_compile_options(${option})
  endif()
endfunction()

function(target_compile_option_ifdef feature_toggle target scope option)
  if(${feature_toggle})
    target_compile_options(${target} ${scope} ${option})
  endif()
endfunction()

function(library_sources source)
  target_sources(${CURRENT_LIBRARY} PRIVATE ${source} ${ARGN})
endfunction()

function(library_sources_ifdef feature_toggle source)
  if(${${feature_toggle}})
    library_sources(${source} ${ARGN})
  endif()
endfunction()


# This function writes a dict to it's output parameter
# 'return_dict'. The dict has information about the parsed arguments,
#
# Usage:
#   get_parse_args(foo ${ARGN})
#   print(foo_STRIP_PREFIX) # foo_STRIP_PREFIX might be set to 1
function(get_parse_args return_dict)
  foreach(x ${ARGN})
    if(DEFINED single_argument)
      set(${single_argument} ${x} PARENT_SCOPE)
      unset(single_argument)
    else()
      if(x STREQUAL STRIP_PREFIX)
        set(${return_dict}_STRIP_PREFIX 1 PARENT_SCOPE)
      elseif(x STREQUAL NO_SPLIT)
        set(${return_dict}_NO_SPLIT 1 PARENT_SCOPE)
      elseif(x STREQUAL DELIMITER)
        set(single_argument ${return_dict}_DELIMITER)
      endif()
    endif()
  endforeach()
endfunction()

# Utility functions for silently omitting compiler flags when the
# compiler lacks support. *_cc_option was ported from KBuild, see
# cc-option in
# https://www.kernel.org/doc/Documentation/kbuild/makefiles.txt

# Writes 1 to the output variable 'ok' for the language 'lang' if
# the flag is supported, otherwise writes 0.
#
# lang must be C or CXX
#
# TODO: Support ASM
#
# Usage:
#
# check_compiler_flag(C "-Wall" my_check)
# print(my_check) # my_check is now 1
function(check_compiler_flag lang option ok)
  if(NOT DEFINED CMAKE_REQUIRED_QUIET)
    set(CMAKE_REQUIRED_QUIET 1)
  endif()

  string(MAKE_C_IDENTIFIER
    check${option}_${lang}_${CMAKE_REQUIRED_FLAGS}
    ${ok}
    )

  if(${lang} STREQUAL C)
    check_c_compiler_flag("${option}" ${${ok}})
  else()
    check_cxx_compiler_flag("${option}" ${${ok}})
  endif()

  if(${${${ok}}})
    set(ret 1)
  else()
    set(ret 0)
  endif()

  set(${ok} ${ret} PARENT_SCOPE)
endfunction()

function(target_ld_options target scope)
  get_parse_args(args ${ARGN})
  list(REMOVE_ITEM ARGN NO_SPLIT)

  foreach(option ${ARGN})
    if(args_NO_SPLIT)
      set(option ${ARGN})
    endif()
    string(JOIN "" check_identifier "check" ${option})
    string(MAKE_C_IDENTIFIER ${check_identifier} check)

    set(SAVED_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
    string(JOIN " " CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS} ${option})
    check_compiler_flag(C "" ${check})
    set(CMAKE_REQUIRED_FLAGS ${SAVED_CMAKE_REQUIRED_FLAGS})

    target_link_libraries_ifdef(${check} ${target} ${scope} ${option})

    if(args_NO_SPLIT)
      break()
    endif()
  endforeach()
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


# 'check_set_linker_property' is a function that check the provided linker
# flag and only set the linker property if the check succeeds
#
# This function is similar in nature to the CMake set_property function, but
# with the extension that it will check that the linker supports the flag before
# setting the property.
#
# APPEND: Flag indicated that the property should be appended to the existing
#         value list for the property.
# TARGET: Name of target on which to add the property (commonly: linker)
# PROPERTY: Name of property with the value(s) following immediately after
#           property name
function(check_set_linker_property)
  set(options APPEND)
  set(single_args TARGET)
  set(multi_args  PROPERTY)
  cmake_parse_arguments(LINKER_PROPERTY "${options}" "${single_args}" "${multi_args}" ${ARGN})

  if(LINKER_PROPERTY_APPEND)
   set(APPEND "APPEND")
  endif()

  list(GET LINKER_PROPERTY_PROPERTY 0 property)
  list(REMOVE_AT LINKER_PROPERTY_PROPERTY 0)
  set(option ${LINKER_PROPERTY_PROPERTY})

  string(MAKE_C_IDENTIFIER check${option} check)

  set(SAVED_CMAKE_REQUIRED_FLAGS ${CMAKE_REQUIRED_FLAGS})
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} ${option}")
  zephyr_check_compiler_flag(C "" ${check})
  set(CMAKE_REQUIRED_FLAGS ${SAVED_CMAKE_REQUIRED_FLAGS})

  if(${check})
    set_property(TARGET ${LINKER_PROPERTY_TARGET} ${APPEND} PROPERTY ${property} ${option})
  endif()
endfunction()

function(zephyr_check_compiler_flag lang option check)
  # Check if the option is covered by any hardcoded check before doing
  # an automated test.
  zephyr_check_compiler_flag_hardcoded(${lang} "${option}" check exists)
  if(exists)
    set(check ${check} PARENT_SCOPE)
    return()
  endif()

  # Locate the cache directory
  set_ifndef(
    ZEPHYR_TOOLCHAIN_CAPABILITY_CACHE_DIR
    ${USER_CACHE_DIR}/ToolchainCapabilityDatabase
    )

  # The toolchain capability database/cache is maintained as a
  # directory of files. The filenames in the directory are keys, and
  # the file contents are the values in this key-value store.

  # We need to create a unique key wrt. testing the toolchain
  # capability. This key must include everything that can affect the
  # toolchain test.
  #
  # Also, to fit the key into a filename we calculate the MD5 sum of
  # the key.

  # The 'cacheformat' must be bumped if a bug in the caching mechanism
  # is detected and all old keys must be invalidated.
  set(cacheformat 3)

  set(key_string "")
  set(key_string "${key_string}${cacheformat}_")
  set(key_string "${key_string}${TOOLCHAIN_SIGNATURE}_")
  set(key_string "${key_string}${lang}_")
  set(key_string "${key_string}${option}_")
  set(key_string "${key_string}${CMAKE_REQUIRED_FLAGS}_")

  string(MD5 key ${key_string})

  # Check the cache
  set(key_path ${ZEPHYR_TOOLCHAIN_CAPABILITY_CACHE_DIR}/${key})
  if(EXISTS ${key_path})
    file(READ
    ${key_path}   # File to be read
    key_value     # Output variable
    LIMIT 1       # Read at most 1 byte ('0' or '1')
    )

    set(${check} ${key_value} PARENT_SCOPE)
    return()
  endif()

  # Flags that start with -Wno-<warning> can not be tested by
  # check_compiler_flag, they will always pass, but -W<warning> can be
  # tested, so to test -Wno-<warning> flags we test -W<warning>
  # instead.
  if("${option}" MATCHES "-Wno-(.*)")
    set(possibly_translated_option -W${CMAKE_MATCH_1})
  else()
    set(possibly_translated_option ${option})
  endif()

  check_compiler_flag(${lang} "${possibly_translated_option}" inner_check)

  set(${check} ${inner_check} PARENT_SCOPE)

  # Populate the cache
  if(NOT (EXISTS ${key_path}))

    # This is racy. As often with race conditions, this one can easily be
    # made worse and demonstrated with a simple delay:
    #    execute_process(COMMAND "sleep" "5")
    # Delete the cache, add the sleep above and run twister with a
    # large number of JOBS. Once it's done look at the log.txt file
    # below and you will see that concurrent cmake processes created the
    # same files multiple times.

    # While there are a number of reasons why this race seems both very
    # unlikely and harmless, let's play it safe anyway and write to a
    # private, temporary file first. All modern filesystems seem to
    # support at least one atomic rename API and cmake's file(RENAME
    # ...) officially leverages that.
    string(RANDOM LENGTH 8 tempsuffix)

    file(
      WRITE
      "${key_path}_tmp_${tempsuffix}"
      ${inner_check}
      )
    file(
      RENAME
      "${key_path}_tmp_${tempsuffix}" "${key_path}"
      )

    # Populate a metadata file (only intended for trouble shooting)
    # with information about the hash, the toolchain capability
    # result, and the toolchain test.
    file(
      APPEND
      ${ZEPHYR_TOOLCHAIN_CAPABILITY_CACHE_DIR}/log.txt
      "${inner_check} ${key} ${key_string}\n"
      )
  endif()
endfunction()

function(zephyr_check_compiler_flag_hardcoded lang option check exists)
  # Various flags that are not supported for CXX may not be testable
  # because they would produce a warning instead of an error during
  # the test.  Exclude them by toolchain-specific blocklist.
  if((${lang} STREQUAL CXX) AND ("${option}" IN_LIST CXX_EXCLUDED_OPTIONS))
    set(check 0 PARENT_SCOPE)
    set(exists 1 PARENT_SCOPE)
  else()
    # There does not exist a hardcoded check for this option.
    set(exists 0 PARENT_SCOPE)
  endif()
endfunction(zephyr_check_compiler_flag_hardcoded)


function(set_compiler_property)
  set(options APPEND)
  set(multi_args  PROPERTY)
  cmake_parse_arguments(COMPILER_PROPERTY "${options}" "${single_args}" "${multi_args}" ${ARGN})
  if(COMPILER_PROPERTY_APPEND)
   set(APPEND "APPEND")
   set(APPEND-CPP "APPEND")
  endif()

  set_property(TARGET compiler ${APPEND} PROPERTY ${COMPILER_PROPERTY_PROPERTY})
  set_property(TARGET compiler-cpp ${APPEND} PROPERTY ${COMPILER_PROPERTY_PROPERTY})
endfunction()


# 'check_set_compiler_property' is a function that check the provided compiler
# flag and only set the compiler or compiler-cpp property if the check succeeds
#
# This function is similar in nature to the CMake set_property function, but
# with the extension that it will check that the compiler supports the flag
# before setting the property on compiler or compiler-cpp targets.
#
# APPEND: Flag indicated that the property should be appended to the existing
#         value list for the property.
# PROPERTY: Name of property with the value(s) following immediately after
#           property name
function(check_set_compiler_property)
  set(options APPEND)
  set(multi_args  PROPERTY)
  cmake_parse_arguments(COMPILER_PROPERTY "${options}" "${single_args}" "${multi_args}" ${ARGN})
  if(COMPILER_PROPERTY_APPEND)
   set(APPEND "APPEND")
   set(APPEND-CPP "APPEND")
  endif()

  list(GET COMPILER_PROPERTY_PROPERTY 0 property)
  list(REMOVE_AT COMPILER_PROPERTY_PROPERTY 0)

  foreach(option ${COMPILER_PROPERTY_PROPERTY})
    if(CONFIG_CPLUSPLUS)
      zephyr_check_compiler_flag(CXX ${option} check)

      if(${check})
        set_property(TARGET compiler-cpp ${APPEND-CPP} PROPERTY ${property} ${option})
        set(APPEND-CPP "APPEND")
      endif()
    endif()

    zephyr_check_compiler_flag(C ${option} check)

    if(${check})
      set_property(TARGET compiler ${APPEND} PROPERTY ${property} ${option})
      set(APPEND "APPEND")
    endif()
  endforeach()
endfunction()