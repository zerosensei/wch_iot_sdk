# SPDX-License-Identifier: Apache-2.0
set(CMAKE_EXECUTABLE_SUFFIX .elf)


set(CMAKE_EXECUTABLE wch_final)

define_property(GLOBAL PROPERTY PROPERTY_OUTPUT_FORMAT BRIEF_DOCS " " FULL_DOCS " ")
set_property(   GLOBAL PROPERTY PROPERTY_OUTPUT_FORMAT elf32-little${ARCH}) # BFD format

# "wch_interface" is a source-less library that encapsulates all the global
# compiler options needed by all source files. All wch libraries,
# including the library named "wch" link with this library to
# obtain these flags.
# https://cmake.org/cmake/help/latest/manual/cmake-buildsystem.7.html#interface-libraries
add_library(wch_interface INTERFACE)

# "wch" is a catch-all CMake library for source files that can be
# built purely with the include paths, defines, and other compiler
# flags that come with wch_interface.
library_set_named(wch)

# set_target_properties(wch PROPERTIES LINKER_LANGUAGE C CXX ASM)

add_include_directories(
    include
    ${PROJECT_BINARY_DIR}/kconfig
)

add_include_directories(${SOC_DIR})

# @Intent: Set compiler specific flag for disabling strict aliasing rule
add_compile_options($<$<COMPILE_LANGUAGE:C>:$<TARGET_PROPERTY:compiler,no_strict_aliasing>>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:$<TARGET_PROPERTY:compiler-cpp,no_strict_aliasing>>)

# @Intent: Set compiler flags to enable buffer overflow checks in libc functions
# @config in CONFIG_NO_OPTIMIZATIONS optional : Optimizations may affect security
add_compile_definitions($<TARGET_PROPERTY:compiler,security_fortify> )

# @Intent: Set compiler flags to detect general stack overflows across all functions
# if(CONFIG_STACK_CANARIES)
#   add_compile_options($<TARGET_PROPERTY:compiler,security_canaries>)
# endif()

# @Intent: Obtain compiler optimizations flags and store in variables
get_property(OPTIMIZE_FOR_NO_OPTIMIZATIONS_FLAG TARGET compiler PROPERTY no_optimization)
get_property(OPTIMIZE_FOR_DEBUG_FLAG TARGET compiler PROPERTY optimization_debug)
get_property(OPTIMIZE_FOR_SPEED_FLAG TARGET compiler PROPERTY optimization_speed)
get_property(OPTIMIZE_FOR_SIZE_FLAG  TARGET compiler PROPERTY optimization_size)

# From kconfig choice, pick the actual OPTIMIZATION_FLAG to use.
# Kconfig choice ensures only one of these CONFIG_*_OPTIMIZATIONS is set.
if(CONFIG_NO_OPTIMIZATIONS)
  set(OPTIMIZATION_FLAG ${OPTIMIZE_FOR_NO_OPTIMIZATIONS_FLAG})
elseif(CONFIG_DEBUG_OPTIMIZATIONS)
  set(OPTIMIZATION_FLAG ${OPTIMIZE_FOR_DEBUG_FLAG})
elseif(CONFIG_SPEED_OPTIMIZATIONS)
  set(OPTIMIZATION_FLAG ${OPTIMIZE_FOR_SPEED_FLAG})
elseif(CONFIG_SIZE_OPTIMIZATIONS)
  set(OPTIMIZATION_FLAG ${OPTIMIZE_FOR_SIZE_FLAG}) # Default in kconfig
else()
  assert(0 "Unreachable code. Expected optimization level to have been chosen. See Kconfig")
endif()

# Apply the final optimization flag(s)
add_compile_options(${OPTIMIZATION_FLAG})

# @Intent: Obtain compiler specific flags related to C++ that are not influenced by kconfig
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:$<TARGET_PROPERTY:compiler-cpp,required>>)

# @Intent: Set compiler specific macro inclusion of AUTOCONF_H
add_compile_options("SHELL: $<TARGET_PROPERTY:compiler,imacros> ${AUTOCONF_H}")

# @Intent: Set compiler specific flag for tentative definitions, no-common
add_compile_options($<TARGET_PROPERTY:compiler,no_common>)

# @Intent: Set compiler specific flag for production of debug information
add_compile_options($<TARGET_PROPERTY:compiler,debug>)

# @Intent: Set compiler specific flag for diagnostic messages
add_compile_options($<TARGET_PROPERTY:compiler,diagnostic>)

add_compile_options(
  ${TOOLCHAIN_C_FLAGS}
)

# @Intent: Obtain compiler specific flags related to assembly
# ToDo: Remember to get feedback from Oticon on this, as they might use the `ASM_BASE_FLAG` since this is done this way.
add_compile_options($<$<COMPILE_LANGUAGE:ASM>:$<TARGET_PROPERTY:asm,required>>)

# Common toolchain-agnostic assembly flags
add_compile_options(
  $<$<COMPILE_LANGUAGE:ASM>:-D_ASMLANGUAGE>
)

# @Intent: Set fundamental linker specific flags
toolchain_ld_base()

# @Intent: Add the basic toolchain warning flags
add_compile_options($<$<COMPILE_LANGUAGE:C>:$<TARGET_PROPERTY:compiler,warning_base>>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:$<TARGET_PROPERTY:compiler-cpp,warning_base>>)

# ==========================================================================
#
# cmake -DW=... settings
#
# W=1 - warnings that may be relevant and does not occur too often
# W=2 - warnings that occur quite often but may still be relevant
# W=3 - the more obscure warnings, can most likely be ignored
# ==========================================================================
# @Intent: Add cmake -DW toolchain supported warnings, if any
if(W MATCHES "1")
  add_compile_options($<$<COMPILE_LANGUAGE:C>:$<TARGET_PROPERTY:compiler,warning_dw_1>>)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:$<TARGET_PROPERTY:compiler-cpp,warning_dw_1>>)
endif()

if(W MATCHES "2")
  add_compile_options($<$<COMPILE_LANGUAGE:C>:$<TARGET_PROPERTY:compiler,warning_dw_2>>)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:$<TARGET_PROPERTY:compiler-cpp,warning_dw_2>>)
endif()

if(W MATCHES "3")
  add_compile_options($<$<COMPILE_LANGUAGE:C>:$<TARGET_PROPERTY:compiler,warning_dw_3>>)
  add_compile_options($<$<COMPILE_LANGUAGE:CXX>:$<TARGET_PROPERTY:compiler-cpp,warning_dw_3>>)
endif()

# @Intent: Add extended, more specific, toolchain warning flags
add_compile_options($<TARGET_PROPERTY:compiler,warning_extended>)

# @Intent: Trigger an error when a declaration does not specify a type
add_compile_options($<$<COMPILE_LANGUAGE:C>:$<TARGET_PROPERTY:compiler,warning_error_implicit_int>>)
add_compile_options($<$<COMPILE_LANGUAGE:CXX>:$<TARGET_PROPERTY:compiler-cpp,warning_error_implicit_int>>)

# # Allow the user to inject options when calling cmake, e.g.
# # 'cmake -DEXTRA_CFLAGS="-Werror -Wno-deprecated-declarations" ..'
# include(cmake/extra_flags.cmake)

separate_arguments(COMPILER_OPT_AS_LIST UNIX_COMMAND ${CONFIG_COMPILER_OPT})
add_compile_options(${COMPILER_OPT_AS_LIST})

get_property(TOPT GLOBAL PROPERTY TOPT)
set_ifndef(  TOPT -Wl,-T) # clang doesn't pick -T for some reason and complains,
                          # while -Wl,-T works for both, gcc and clang

set(LINKER_SCRIPT  ${ENV_WCH_BASE}/soc/${ARCH}/${SOC_SERIES}/Link.ld)
if(NOT EXISTS ${LINKER_SCRIPT})
  message(FATAL_ERROR "Could not find linker script: '${LINKER_SCRIPT}'. Corrupted configuration?")
endif()

add_subdirectory(arch)
add_subdirectory(drivers)
add_subdirectory(hal)
add_subdirectory(lib)
add_subdirectory(${SOC_DIR})
add_subdirectory(subsys)
add_subdirectory(kernel)

get_property(WCH_LIBS_PROPERTY GLOBAL PROPERTY WCH_LIBS)

get_property(CSTD GLOBAL PROPERTY CSTD)
set_ifndef(CSTD c99)

# @Intent: Obtain compiler specific flag for specifying the c standard
add_compile_options(
  $<$<COMPILE_LANGUAGE:C>:$<TARGET_PROPERTY:compiler,cstd>${CSTD}>
)
set(CMAKE_C_COMPILE_FEATURES ${compile_features_${CSTD}} PARENT_SCOPE)

get_property(TOPT GLOBAL PROPERTY TOPT)
set_ifndef(  TOPT -Wl,-T) # clang doesn't pick -T for some reason and complains,
                          # while -Wl,-T works for both, gcc and clang

add_executable(${CMAKE_EXECUTABLE} misc/empty_file.c )
toolchain_ld_link_elf(
  TARGET_ELF            ${CMAKE_EXECUTABLE}
  OUTPUT_MAP            ${PROJECT_BINARY_DIR}/${CMAKE_EXECUTABLE}.map
  LIBRARIES_PRE_SCRIPT  ""
  LINKER_SCRIPT         ${LINKER_SCRIPT}
  DEPENDENCIES          ${CODE_RELOCATION_DEP}
)

set(logical_target_for_elf ${CMAKE_EXECUTABLE})

# Export the variable to the application's scope to allow the
# application to know what the name of the final elf target is.
set(logical_target_for_elf ${logical_target_for_elf} PARENT_SCOPE)

# Override the base name of the last, "logical" .elf output (and last .map) so:
# 1. it doesn't depend on the number of passes above and the
#    post_build_commands below can always find it no matter which is it;
# 2. it can be defined in Kconfig
set_target_properties(${logical_target_for_elf} PROPERTIES OUTPUT_NAME ${KERNEL_NAME})

set(post_build_commands "")
set(post_build_byproducts "")

list(APPEND
  post_build_commands
  COMMAND
  ${CMAKE_COMMAND} -E copy ${logical_target_for_elf}.map ${KERNEL_MAP_NAME}
)
list(APPEND post_build_byproducts ${KERNEL_MAP_NAME})

if(NOT CONFIG_BUILD_NO_GAP_FILL)
  set(GAP_FILL "$<TARGET_PROPERTY:bintools,elfconvert_flag_gapfill>0xff")
endif()

if(CONFIG_OUTPUT_PRINT_MEMORY_USAGE)
  target_link_libraries(${logical_target_for_elf} $<TARGET_PROPERTY:linker,memusage>)

  get_property(memusage_build_command TARGET bintools PROPERTY memusage_command)

  if(memusage_build_command)
    # Note: The use of generator expressions allows downstream extensions to add/change the post build.
    # Unfortunately, the BYPRODUCTS does not allow for generator expression, so question is if we
    # should remove the downstream ability from start.
    # Or fix the output name, by the use of `get_property`
    list(APPEND
      post_build_commands
      COMMAND $<TARGET_PROPERTY:bintools,memusage_command>
              $<TARGET_PROPERTY:bintools,memusage_flag>
              $<TARGET_PROPERTY:bintools,memusage_infile>${KERNEL_ELF_NAME}
      )

    # For now, the byproduct can only be supported upstream on byproducts name,
    # cause byproduct does not support generator expressions
    get_property(memusage_byproducts TARGET bintools PROPERTY memusage_byproducts)
    list(APPEND
      post_build_byproducts
      ${memusage_byproducts}
      )
  endif()
endif()

if(CONFIG_BUILD_OUTPUT_HEX)
  get_property(elfconvert_formats TARGET bintools PROPERTY elfconvert_formats)
  if(ihex IN_LIST elfconvert_formats)
    list(APPEND
      post_build_commands
      COMMAND $<TARGET_PROPERTY:bintools,elfconvert_command>
              $<TARGET_PROPERTY:bintools,elfconvert_flag>
              ${GAP_FILL}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_outtarget>ihex
              ${remove_sections_argument_list}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_infile>${KERNEL_ELF_NAME}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_outfile>${KERNEL_HEX_NAME}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_final>
      )
    list(APPEND
      post_build_byproducts
      ${KERNEL_HEX_NAME}
      )
  endif()
endif()

if(CONFIG_BUILD_OUTPUT_BIN)
  get_property(elfconvert_formats TARGET bintools PROPERTY elfconvert_formats)
  if(binary IN_LIST elfconvert_formats)
    list(APPEND
      post_build_commands
      COMMAND $<TARGET_PROPERTY:bintools,elfconvert_command>
              $<TARGET_PROPERTY:bintools,elfconvert_flag>
              ${GAP_FILL}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_outtarget>binary
              ${remove_sections_argument_list}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_infile>${KERNEL_ELF_NAME}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_outfile>${KERNEL_BIN_NAME}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_final>
      )
    list(APPEND
      post_build_byproducts
      ${KERNEL_BIN_NAME}
      # ${out_hex_byprod} # Is this needed ?
      )
  endif()
endif()

if(CONFIG_BUILD_OUTPUT_S19)
  get_property(elfconvert_formats TARGET bintools PROPERTY elfconvert_formats)
  if(srec IN_LIST elfconvert_formats)
    # Should we print a warning if case the tools does not support converting to s19 ?
    list(APPEND
      post_build_commands
      COMMAND $<TARGET_PROPERTY:bintools,elfconvert_command>
              $<TARGET_PROPERTY:bintools,elfconvert_flag>
              ${GAP_FILL}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_outtarget>srec
              $<TARGET_PROPERTY:bintools,elfconvert_flag_srec_len>1
              $<TARGET_PROPERTY:bintools,elfconvert_flag_infile>${KERNEL_ELF_NAME}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_outfile>${KERNEL_S19_NAME}
              $<TARGET_PROPERTY:bintools,elfconvert_flag_final>
      )
    list(APPEND
      post_build_byproducts
      ${KERNEL_S19_NAME}

      )
  endif()
endif()

if(CONFIG_OUTPUT_DISASSEMBLY)
  if(CONFIG_OUTPUT_DISASSEMBLE_ALL)
    set(disassembly_type "$<TARGET_PROPERTY:bintools,disassembly_flag_all>")
  else()
    set(disassembly_type "$<TARGET_PROPERTY:bintools,disassembly_flag_inline_source>")
  endif()
  list(APPEND
    post_build_commands
    COMMAND $<TARGET_PROPERTY:bintools,disassembly_command>
            $<TARGET_PROPERTY:bintools,disassembly_flag>
            ${disassembly_type}
            $<TARGET_PROPERTY:bintools,disassembly_flag_infile>${KERNEL_ELF_NAME}
            $<TARGET_PROPERTY:bintools,disassembly_flag_outfile>${KERNEL_LST_NAME}
            $<TARGET_PROPERTY:bintools,disassembly_flag_final>
    )
  list(APPEND
    post_build_byproducts
    ${KERNEL_LST_NAME}
#    ${out_disassembly_byprod} # Needed ??
    )
endif()

if(CONFIG_OUTPUT_SYMBOLS)
  list(APPEND
    post_build_commands
    COMMAND $<TARGET_PROPERTY:bintools,symbols_command>
            $<TARGET_PROPERTY:bintools,symbols_flag>
            $<TARGET_PROPERTY:bintools,symbols_infile>${KERNEL_ELF_NAME}
            $<TARGET_PROPERTY:bintools,symbols_outfile>${KERNEL_SYMBOLS_NAME}
            $<TARGET_PROPERTY:bintools,symbols_final>
    )
  list(APPEND
    post_build_byproducts
    ${KERNEL_SYMBOLS_NAME}
    )
endif()

if(CONFIG_OUTPUT_STAT)
  list(APPEND
    post_build_commands
    COMMAND $<TARGET_PROPERTY:bintools,readelf_command>
            $<TARGET_PROPERTY:bintools,readelf_flag>
            $<TARGET_PROPERTY:bintools,readelf_flag_headers>
            $<TARGET_PROPERTY:bintools,readelf_flag_infile>${KERNEL_ELF_NAME}
            $<TARGET_PROPERTY:bintools,readelf_flag_outfile>${KERNEL_STAT_NAME}
            $<TARGET_PROPERTY:bintools,readelf_flag_final>
    )
  list(APPEND
    post_build_byproducts
    ${KERNEL_STAT_NAME}
    )
endif()

if(CONFIG_BUILD_OUTPUT_STRIPPED)
  list(APPEND
    post_build_commands
    COMMAND $<TARGET_PROPERTY:bintools,strip_command>
            $<TARGET_PROPERTY:bintools,strip_flag>
            $<TARGET_PROPERTY:bintools,strip_flag_all>
            $<TARGET_PROPERTY:bintools,strip_flag_infile>${KERNEL_ELF_NAME}
            $<TARGET_PROPERTY:bintools,strip_flag_outfile>${KERNEL_STRIP_NAME}
            $<TARGET_PROPERTY:bintools,strip_flag_final>
    )
  list(APPEND
    post_build_byproducts
    ${KERNEL_STRIP_NAME}
    )
endif()

if(CONFIG_BUILD_OUTPUT_EXE)
  list(APPEND
    post_build_commands
    COMMAND
    ${CMAKE_COMMAND} -E copy ${KERNEL_ELF_NAME} ${KERNEL_EXE_NAME}
    )
  list(APPEND
    post_build_byproducts
    ${KERNEL_EXE_NAME}
    )
endif()


get_property(extra_post_build_commands
  GLOBAL PROPERTY
  extra_post_build_commands
  )

list(APPEND
  post_build_commands
  ${extra_post_build_commands}
  )

get_property(extra_post_build_byproducts
  GLOBAL PROPERTY
  extra_post_build_byproducts
  )

list(APPEND
  post_build_byproducts
  ${extra_post_build_byproducts}
  )

# Add post_build_commands to post-process the final .elf file produced by
# either the ZEPHYR_LINK_STAGE_EXECUTABLE or the KERNEL_ELF executable
# targets above.
add_custom_command(
  TARGET ${logical_target_for_elf}
  POST_BUILD
  ${post_build_commands}
  BYPRODUCTS
  ${post_build_byproducts}
  COMMENT "Generating files from ${KERNEL_ELF_NAME} for board: ${BOARD}"
  COMMAND_EXPAND_LISTS
  # NB: COMMENT only works for some CMake-Generators
  )

# @Intent: Set compiler specific flags for standard C/C++ includes
# Done at the very end, so any other system includes which may
# be added by wch components were first in list.
# Note, the compile flags are moved, but the system include is still present here.
add_compile_options($<TARGET_PROPERTY:compiler,nostdinc>)
target_include_directories(wch_interface SYSTEM INTERFACE $<TARGET_PROPERTY:compiler,nostdinc_include>)

# add_subdirectory_ifdef(
#   CONFIG_MAKEFILE_EXPORTS
#   cmake/makefile_exports
#   )