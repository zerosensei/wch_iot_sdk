# SPDX-License-Identifier: Apache-2.0


set(CMAKE_EXECUTABLE_SUFFIX .elf)


set(FINAL_EXECUTABLE wch_final)

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

include_directories(
    include
    ${PROJECT_BINARY_DIR}
)

include_directories(${SOC_DIR})

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

# toolchain_ld_force_undefined_symbols(
#   _OffsetAbsSyms
#   _ConfigAbsSyms
# )

set(LINKER_SCRIPT  ${ENV_WCH_BASE}/soc/${ARCH}/${SOC_SERIES}/Link.ld)
if(NOT EXISTS ${LINKER_SCRIPT})
  message(FATAL_ERROR "Could not find linker script: '${LINKER_SCRIPT}'. Corrupted configuration?")
endif()

add_link_options(-nostartfiles 
                -Xlinker --gc-sections  
                -Wl,--print-memory-usage
                -Wl,-Map,${PROJECT_NAME}.map 
                --specs=nano.specs 
                --specs=nosys.specs)
add_link_options(-T ${LINKER_SCRIPT})



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

# zephyr_cc_option(-fno-asynchronous-unwind-tables)
# zephyr_cc_option(-fno-pie)
# zephyr_cc_option(-fno-pic)

separate_arguments(COMPILER_OPT_AS_LIST UNIX_COMMAND ${CONFIG_COMPILER_OPT})
add_compile_options(${COMPILER_OPT_AS_LIST})

get_property(TOPT GLOBAL PROPERTY TOPT)
set_ifndef(  TOPT -Wl,-T) # clang doesn't pick -T for some reason and complains,
                          # while -Wl,-T works for both, gcc and clang


# get_include_directories_for_lang(C ZEPHYR_INCLUDES)


# add_subdirectory(arch)
# add_subdirectory(drivers)
# add_subdirectory(hal)
# add_subdirectory(lib)

# message("SOC DIR: " ${SOC_DIR})
add_subdirectory(${SOC_DIR})

add_subdirectory(subsys)


get_target_property(TEST wch SOURCES)
message("test: " ${TEST})

# get_include_directories_for_lang(C
#   ZEPHYR_INCLUDE_DIRS
#   STRIP_PREFIX # Don't use a -I prefix
#   )

add_subdirectory(kernel)


get_property(WCH_LIBS_PROPERTY GLOBAL PROPERTY WCH_LIBS)

message("wch libs: " ${WCH_LIBS_PROPERTY})
# foreach(wch_lib ${WCH_LIBS_PROPERTY})
#   get_property(lib_type TARGET ${wch_lib} PROPERTY TYPE)

#   if(${lib_type} STREQUAL STATIC_LIBRARY 
#       AND NOT ${wch_lib} STREQUAL app
#   )
#     get_property(source_list TARGET ${wch_lib} PROPERTY SOURCES)
#     get_property(lib_imported TARGET ${wch_lib} PROPERTY IMPORTED)

#     if(NOT source_list 
#       AND NOT ${lib_imported}
#     )

#     endif()


#   endif()

# endforeach()



get_property(CSTD GLOBAL PROPERTY CSTD)
set_ifndef(CSTD -std=c99)

# @Intent: Obtain compiler specific flag for specifying the c standard
add_compile_options(
  $<$<COMPILE_LANGUAGE:C>:$<TARGET_PROPERTY:compiler,cstd>${CSTD}>
)
set(CMAKE_C_COMPILE_FEATURES ${compile_features_${CSTD}} PARENT_SCOPE)

set(ZEPHYR_LINK_STAGE_EXECUTABLE zephyr_test)


get_property(TOPT GLOBAL PROPERTY TOPT)
set_ifndef(  TOPT -Wl,-T) # clang doesn't pick -T for some reason and complains,
                          # while -Wl,-T works for both, gcc and clang

add_executable(${ZEPHYR_LINK_STAGE_EXECUTABLE} misc/empty_file.c )
toolchain_ld_link_elf(
  TARGET_ELF            ${ZEPHYR_LINK_STAGE_EXECUTABLE}
  OUTPUT_MAP            ${PROJECT_BINARY_DIR}/${ZEPHYR_LINK_STAGE_EXECUTABLE}.map
  LIBRARIES_PRE_SCRIPT  ""
  # LINKER_SCRIPT         ${PROJECT_BINARY_DIR}/${ZEPHYR_CURRENT_LINKER_CMD}
  DEPENDENCIES          ${CODE_RELOCATION_DEP}
)

# configure_linker_script(
#   ${ZEPHYR_CURRENT_LINKER_CMD}
#   "${LINKER_PASS_${ZEPHYR_CURRENT_LINKER_PASS}_DEFINE}"
#   ${APP_SMEM_ALIGNED_DEP}
#   ${KOBJECT_LINKER_DEP}
#   ${CODE_RELOCATION_DEP}
#   zephyr_generated_headers
#   )



# @Intent: Set compiler specific flags for standard C/C++ includes
# Done at the very end, so any other system includes which may
# be added by wch components were first in list.
# Note, the compile flags are moved, but the system include is still present here.
add_compile_options($<TARGET_PROPERTY:compiler,nostdinc>)
target_include_directories(wch_interface SYSTEM INTERFACE $<TARGET_PROPERTY:compiler,nostdinc_include>)


get_target_property(TEST wch SOURCES)
message("test: " ${TEST})
